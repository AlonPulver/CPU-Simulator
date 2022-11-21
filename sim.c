#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define MEMORY_SIZE 4096 // seting the constant variable that were given in the instruction file
#define DISK_SIZE (128 * 128)
#define MONITOR_SIZE (256 * 256)

uint32_t irq2 = 0;
bool irq2_initialized = false;

uint32_t pc = 0; 
uint32_t registers[16] = { 0 };
uint32_t memory[MEMORY_SIZE] = { 0 };
uint32_t disk[DISK_SIZE] = { 0 };
uint32_t io_registers[23] = { 0 };
uint8_t monitor[MONITOR_SIZE] = { 0 };

bool halted = false; // נאפס את המשתנה, כך שברגע שיהפוך ל-1 תסתיים הריצה
bool in_irq = false;
uint32_t disk_counter = 0;

FILE *fMemIn = NULL;
FILE *fDiskIn = NULL;
FILE *fIrq2In = NULL;

FILE *fMemOut = NULL;
FILE *fRegOut = NULL;
FILE *fTrace = NULL;
FILE *fHwRegTrace = NULL;
FILE *fCycles = NULL;
FILE *fLeds = NULL;
FILE *fDisplay7Seg = NULL;
FILE *fDiskOut = NULL;
FILE *fMonitor = NULL;
FILE *fMonitorYuv = NULL;

enum {
	OPCODE_ADD = 0, // having each opcode name to its matching number
	OPCODE_SUB = 1,
	OPCODE_MUL = 2,
	OPCODE_AND = 3,
	OPCODE_OR = 4,
	OPCODE_XOR = 5,
	OPCODE_SLL = 6,
	OPCODE_SRA = 7,
	OPCODE_SRL = 8,
	OPCODE_BEQ = 9,
	OPCODE_BNE = 10,
	OPCODE_BLT = 11,
	OPCODE_BGT = 12,
	OPCODE_BLE = 13,
	OPCODE_BGE = 14,
	OPCODE_JAL = 15,
	OPCODE_LW = 16,
	OPCODE_SW = 17,
	OPCODE_RETI = 18,
	OPCODE_IN = 19,
	OPCODE_OUT = 20,
	OPCODE_HALT = 21,
};

enum {
	REGISTER_ZERO = 0, // having each register name to its matching number
	REGISTER_IMM = 1,
	REGISTER_V0 = 2,
	REGISTER_A0 = 3,
	REGISTER_A1 = 4,
	REGISTER_A2 = 5,
	REGISTER_A3 = 6,
	REGISTER_T0 = 7,
	REGISTER_T1 = 8,
	REGISTER_T2 = 9,
	REGISTER_S0 = 10,
	REGISTER_S1 = 11,
	REGISTER_S2 = 12,
	REGISTER_GP = 13,
	REGISTER_SP = 14,
	REGISTER_RA = 15,
};

enum {
	IO_REGISTER_IRQ0ENABLE = 0,   // having each IO_register name to its matching number
	IO_REGISTER_IRQ1ENABLE = 1,
	IO_REGISTER_IRQ2ENABLE = 2,
	IO_REGISTER_IRQ0STATUS = 3,
	IO_REGISTER_IRQ1STATUS = 4,
	IO_REGISTER_IRQ2STATUS = 5,
	IO_REGISTER_IRQHANDLER = 6,
	IO_REGISTER_IRQRETURN = 7,
	IO_REGISTER_CLKS = 8,
	IO_REGISTER_LEDS = 9,
	IO_REGISTER_DISPLAY7SEG = 10,
	IO_REGISTER_TIMERENABLE = 11,
	IO_REGISTER_TIMERCURRENT = 12,
	IO_REGISTER_TIMERMAX = 13,
	IO_REGISTER_DISKCMD = 14,
	IO_REGISTER_DISKSECTOR = 15,
	IO_REGISTER_DISKBUFFER = 16,
	IO_REGISTER_DISKSTATUS = 17,
	IO_REGISTER_RESERVED0 = 18,
	IO_REGISTER_RESERVED1 = 19,
	IO_REGISTER_MONITORADDR = 20,
	IO_REGISTER_MONITORDATA = 21,
	IO_REGISTER_MONITORCMD = 22,
};

char* io_regs_name[] = { // creating a stack of the IO-registers
	"irq0enable",
	"irq1enable",
	"irq2enable",
	"irq0status",
	"irq1status",
	"irq2status",
	"irqhandler",
	"irqreturn",
	"clks",
	"leds",
	"display7seg",
	"timerenable",
	"timercurrent",
	"timermax",
	"diskcmd",
	"disksector",
	"diskbuffer",
	"diskstatus",
	"reserved0",
	"reserved1",
	"monitoraddr",
	"monitordata",
	"monitorcmd",
};

typedef struct {
	uint32_t rt : 4;
	uint32_t rs : 4;
	uint32_t rd : 4;
	uint32_t opcode : 8;
} instruction_R;

typedef struct {
	uint32_t imm : 20;
} instruction_I;

//input argument:
//fp - file pointer to the memin file
//MEME - the memory array of int type
//function read every line from the file and convert every line for decimal number represnt in 20 bits.

void read_file_hex_to_array(uint32_t *arr, unsigned int array_size, FILE *fp) { //
	char line[10] = { 0 };
	unsigned int i = 0;
	while (!feof(fp) && i < array_size) {
		if (fscanf(fp, "%x\n", &arr[i]) != 1) {
			printf("Error parsing file!\n");
			exit(1);
		}
		i++;
	}
}

void read_irq2(FILE *fp) {                 // reads  the number in the file irq2 and saving the value inthe line if ones exist
	if (fscanf(fp, "%d", &irq2) == 1) {
		irq2_initialized = true;
	}
	else {
		irq2_initialized = false;
	}
}

void write_to_register(int r, uint32_t value) {       // saving the value of register while making sure the values of zero and imm are saved
	if (r == REGISTER_IMM || r == REGISTER_ZERO) {
		return;
	}

	registers[r] = value;
}

// the function chceks in which type of operation we are, to add 1,2,3 time cycles to the PC
void clock_cycle() {
	io_registers[IO_REGISTER_CLKS]++;

	if (irq2_initialized && io_registers[IO_REGISTER_CLKS] == irq2) {
		read_irq2(fIrq2In); /* Read next irq2 from file */
		io_registers[IO_REGISTER_IRQ2STATUS] = 1;
	}

	if (io_registers[IO_REGISTER_TIMERENABLE]) {
		io_registers[IO_REGISTER_TIMERCURRENT]++;
		if (io_registers[IO_REGISTER_TIMERCURRENT] == io_registers[IO_REGISTER_TIMERMAX]) {
			io_registers[IO_REGISTER_IRQ0STATUS] = 1;
		}
	}

	if (io_registers[IO_REGISTER_DISKCMD]) {
		disk_counter--;
		if (disk_counter == 0) {
			if (io_registers[IO_REGISTER_DISKCMD] == 1) {
				for (unsigned int i = 0; i < 128; ++i) {
					memory[(io_registers[IO_REGISTER_DISKBUFFER] & 0xFFF) + i] = disk[(io_registers[IO_REGISTER_DISKSECTOR] & 0x7f) * 128 + i];
				}
			}
			else if (io_registers[IO_REGISTER_DISKCMD] == 2) {
				for (unsigned int i = 0; i < 128; ++i) {
					disk[(io_registers[IO_REGISTER_DISKSECTOR] & 0x7f) * 128 + i] = memory[(io_registers[IO_REGISTER_DISKBUFFER] & 0xFFF) + i];
				}
			}

			io_registers[IO_REGISTER_DISKSTATUS] = 0;
			io_registers[IO_REGISTER_DISKCMD] = 0;
			io_registers[IO_REGISTER_IRQ1STATUS] = 1;
		}
	}
}

uint32_t* get_memory_ptr(uint32_t address) {
	clock_cycle();
	return &memory[address & 0xFFF];
}

void jal(instruction_R *inst) {
	write_to_register(inst->rd, pc);
	pc = registers[inst->rs];
}


//input argument:
//Instruction *inst: pointer to the current instruction on the memory
//operator - the operator for comparison
//The function update the PC to the value of low 12 bits of register rd
void conditional_jump(instruction_R *inst) {
	bool condition = false;

	switch (inst->opcode) {
	case OPCODE_BEQ:
		condition = registers[inst->rs] == registers[inst->rt];
		break;
	case OPCODE_BNE:
		condition = registers[inst->rs] != registers[inst->rt];
		break;
	case OPCODE_BLT:
		condition = registers[inst->rs] < registers[inst->rt];
		break;
	case OPCODE_BGT:
		condition = registers[inst->rs] > registers[inst->rt];
		break;
	case OPCODE_BLE:
		condition = registers[inst->rs] <= registers[inst->rt];
		break;
	case OPCODE_BGE:
		condition = registers[inst->rs] >= registers[inst->rt];
		break;
	}

	if (condition) {
		pc = registers[inst->rd] & 0xFFF;
	}
}

void update_monitor(uint32_t cmd) {
	if (cmd != 1) {
		return;
	}

	monitor[io_registers[IO_REGISTER_MONITORADDR & 0xffff]] = io_registers[IO_REGISTER_MONITORDATA] & 0xFF;
}

void disk_cmd(uint32_t cmd) {
	disk_counter = 1024;
	io_registers[IO_REGISTER_DISKSTATUS] = 1;
	io_registers[IO_REGISTER_DISKCMD] = cmd;
}

void leds_cmd(uint32_t leds) {
	io_registers[IO_REGISTER_LEDS] = leds;
	fprintf(fLeds, "%d %08X\n", io_registers[IO_REGISTER_CLKS], leds);
}

void display7seg_cmd(uint32_t disp) {
	io_registers[IO_REGISTER_DISPLAY7SEG] = disp;
	fprintf(fDisplay7Seg, "%d %08X\n", io_registers[IO_REGISTER_CLKS], disp);
}

void inout(instruction_R *inst) {
	uint32_t io_register = registers[inst->rs] + registers[inst->rt];
	uint32_t data;
	char* cmd="";

	if (io_register > 22) {
		return;
	}

	// the function checks if the simulator needs to read from a file or to write to one.
	// in case it needs to read, the function cheks to which sub function it needs to be sent in order to write to the correct file
	switch (inst->opcode) {
	case OPCODE_IN:
		cmd = "READ";
		data = io_registers[io_register];
		write_to_register(inst->rd, data);
		break;
	case OPCODE_OUT:
		cmd = "WRITE";
		data = registers[inst->rd];
		if (io_register == IO_REGISTER_MONITORCMD) {
			update_monitor(registers[inst->rd]);
		}
		else if (io_register == IO_REGISTER_DISKCMD) {
			disk_cmd(registers[inst->rd]);
		}
		else if (io_register == IO_REGISTER_LEDS) {
			leds_cmd(registers[inst->rd]);
		}
		else if (io_register == IO_REGISTER_DISPLAY7SEG) {
			display7seg_cmd(registers[inst->rd]);
		}
		else {
			io_registers[io_register] = data;
		}
		break;
	}

	fprintf(fHwRegTrace, "%d %s %s %08X\n", io_registers[IO_REGISTER_CLKS], cmd, io_regs_name[io_register], data);
}

// input argument:
//inst: pointer to the current instruction in the memory
//this function identify the opcode and excute the intruction.After the excute it change the PC according the function write_to_register

void run_opcode(instruction_R *inst) {
	switch (inst->opcode) {
	case OPCODE_ADD:
		write_to_register(inst->rd, registers[inst->rs] + registers[inst->rt]);
		break;
	case OPCODE_SUB:
		write_to_register(inst->rd, registers[inst->rs] - registers[inst->rt]);
		break;
	case OPCODE_MUL:
		write_to_register(inst->rd, registers[inst->rs] * registers[inst->rt]);
		break;
	case OPCODE_AND:
		write_to_register(inst->rd, registers[inst->rs] & registers[inst->rt]);
		break;
	case OPCODE_OR:
		write_to_register(inst->rd, registers[inst->rs] | registers[inst->rt]);
		break;
	case OPCODE_XOR:
		write_to_register(inst->rd, registers[inst->rs] ^ registers[inst->rt]);
		break;
	case OPCODE_SLL:
		write_to_register(inst->rd, registers[inst->rs] << registers[inst->rt]);
		break;
	case OPCODE_SRA:
		write_to_register(inst->rd, (uint32_t)((int32_t)registers[inst->rs] >> registers[inst->rt]));
		break;
	case OPCODE_SRL:
		write_to_register(inst->rd, registers[inst->rs] >> registers[inst->rt]);
		break;
	case OPCODE_BEQ:
	case OPCODE_BNE:
	case OPCODE_BLT:
	case OPCODE_BGT:
	case OPCODE_BLE:
	case OPCODE_BGE:
		conditional_jump(inst);
		break;
	case OPCODE_JAL:
		jal(inst);
		break;
	case OPCODE_LW:
		write_to_register(inst->rd, *get_memory_ptr((registers[inst->rs] + registers[inst->rt])));
		break;
	case OPCODE_SW:
		*get_memory_ptr(registers[inst->rs] + registers[inst->rt]) = registers[inst->rd] & 0xFFFFF;
		break;
	case OPCODE_RETI:
		pc = io_registers[IO_REGISTER_IRQRETURN] & 0xFFF;
		in_irq = false;
		break;
	case OPCODE_IN:
	case OPCODE_OUT:
		inout(inst);
		break;
	case OPCODE_HALT:
		halted = true;
		break;
	}
}

unsigned trailzero(uint32_t arr[]){
	unsigned i;
	for (i = 4095; i >= 0; --i)
	{
		if (arr[i] != 0)
			break;
	}
	return i;
}

unsigned trailzero2(uint8_t arr[]) {
	unsigned i;
	for (i = 4095; i >= 0; --i)
	{
		if (arr[i] != 0)
			break;
	}
	return i;
}

int main(int argc, char* argv[]) {
	if (argc != 14) { // no files were sent via command line
		printf("Missing arguments: memin.txt diskin.txt irq2in.txt memout.txt regout.txt trace.txt hwregtrace.txt cycles.txt leds.txt display7seg.txt diskout.txt monitor.txt monitor.yuv\n");
		exit(1);
	}

	// checks if the files that is given isn't empty or having any errors opening them
	fMemIn = fopen(argv[1], "r");
	if (fMemIn == NULL) {
		printf("Can't load MemIn file\n");
		exit(1);
	}

	fDiskIn = fopen(argv[2], "r");
	if (fDiskIn == NULL) {
		printf("Can't load DiskIn file\n");
		exit(1);
	}

	fIrq2In = fopen(argv[3], "r");
	if (fIrq2In == NULL) {
		printf("Can't load Irq2In file\n");
		exit(1);
	}

	fMemOut = fopen(argv[4], "w");
	if (fMemOut == NULL) {
		printf("Can't load MemOut file\n");
		exit(1);
	}

	fRegOut = fopen(argv[5], "w");
	if (fRegOut == NULL) {
		printf("Can't load RegOut file\n");
		exit(1);
	}

	fTrace = fopen(argv[6], "w");
	if (fTrace == NULL) {
		printf("Can't load Trace file\n");
		exit(1);
	}

	fHwRegTrace = fopen(argv[7], "w");
	if (fHwRegTrace == NULL) {
		printf("Can't load HwRegTrace file\n");
		exit(1);
	}

	fCycles = fopen(argv[8], "w");
	if (fCycles == NULL) {
		printf("Can't load Cycles file\n");
		exit(1);
	}

	fLeds = fopen(argv[9], "w");
	if (fLeds == NULL) {
		printf("Can't load Leds file\n");
		exit(1);
	}

	fDisplay7Seg = fopen(argv[10], "w");
	if (fDisplay7Seg == NULL) {
		printf("Can't load Display7Seg file\n");
		exit(1);
	}

	fDiskOut = fopen(argv[11], "w");
	if (fDiskOut == NULL) {
		printf("Can't load DiskOut file\n");
		exit(1);
	}

	fMonitor = fopen(argv[12], "w");
	if (fMonitor == NULL) {
		printf("Can't load Monitor file\n");
		exit(1);
	}

	fMonitorYuv = fopen(argv[13], "w");
	if (fMonitorYuv == NULL) {
		printf("Can't load MonitorYuv file\n");
		exit(1);
	}

	read_file_hex_to_array(memory, MEMORY_SIZE, fMemIn);
	read_file_hex_to_array(disk, DISK_SIZE, fDiskIn);
	read_irq2(fIrq2In);

	io_registers[IO_REGISTER_CLKS] = -1;

	while (!halted) { // while we werent commended to exit thw simulation
		int irq = 0;
		irq |= io_registers[IO_REGISTER_IRQ0STATUS] && io_registers[IO_REGISTER_IRQ0ENABLE];
		irq |= io_registers[IO_REGISTER_IRQ1STATUS] && io_registers[IO_REGISTER_IRQ1ENABLE];
		irq |= io_registers[IO_REGISTER_IRQ2STATUS] && io_registers[IO_REGISTER_IRQ2ENABLE];

		if (irq && !in_irq) { // counting the pc 
			in_irq = true;
			io_registers[IO_REGISTER_IRQRETURN] = pc;
			pc = io_registers[IO_REGISTER_IRQHANDLER];
		}

		io_registers[IO_REGISTER_IRQ2STATUS] = 0;

		uint32_t opcode = *get_memory_ptr(pc) & 0xFFFFF;
		uint32_t trace_pc = pc;
		instruction_R *inst = (instruction_R *)&opcode;
		++pc;

		if (inst->rt == REGISTER_IMM || inst->rs == REGISTER_IMM || inst->rd == REGISTER_IMM) {
			instruction_I *imm = (instruction_I *)get_memory_ptr(pc);
			registers[REGISTER_IMM] = (uint32_t)(((int32_t)(imm->imm << 12)) >> 12); /* sign extension 20-bits */
			++pc;
		}
		else {
			registers[REGISTER_IMM] = 0;
		}

		fprintf(fTrace, "%03X %05X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X\n",// print to trace file
			trace_pc, opcode, registers[REGISTER_ZERO], registers[REGISTER_IMM], registers[REGISTER_V0],
			registers[REGISTER_A0], registers[REGISTER_A1], registers[REGISTER_A2], registers[REGISTER_A3],
			registers[REGISTER_T0], registers[REGISTER_T1], registers[REGISTER_T2], registers[REGISTER_S0],
			registers[REGISTER_S1], registers[REGISTER_S2], registers[REGISTER_GP], registers[REGISTER_SP],
			registers[REGISTER_RA]);

		run_opcode(inst);
	}

	unsigned mem = trailzero(memory);
	for (unsigned i = 0 ;  i < mem + 1; ++i) {
			fprintf(fMemOut, "%05X\n", memory[i]);
	}

	for (unsigned i = 2; i < 16; ++i) {
		fprintf(fRegOut, "%08X\n", registers[i]);
	}

	for (unsigned i = 0; i < DISK_SIZE; ++i) {
		fprintf(fDiskOut, "%05X\n", disk[i]);
	}

	unsigned mon = trailzero2(monitor);
	for (unsigned i = 0; i < mon + 1; ++i) {
			fprintf(fMonitor, "%02X\n", monitor[i]);
	}

	fwrite(monitor, 1, MONITOR_SIZE, fMonitorYuv);

	fprintf(fCycles, "%d\n", io_registers[IO_REGISTER_CLKS] + 1);

	fclose(fMemIn); // סגירת כל הקבצים הפתוחים בסיום הריצה
	fclose(fDiskIn);
	fclose(fIrq2In);
	fclose(fMemOut);
	fclose(fRegOut);
	fclose(fTrace);
	fclose(fHwRegTrace);
	fclose(fCycles);
	fclose(fLeds);
	fclose(fDisplay7Seg);
	fclose(fDiskOut);
	fclose(fMonitor);
	fclose(fMonitorYuv);
}