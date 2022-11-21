#define _CRT_SECURE_NO_WARNINGS
#include "asm.h"

int main(int argc, char* argv[])
{
	FILE *fp = NULL;
	Label labels[MEM_SIZE] = { NULL };
	Memory memo[MEM_SIZE] = { NULL }; // An array in which we save the Assembly code

	// First run - Sets the labels in an array with thier name and address

	if (argc < 2) { // No files were sent to the assembler
		printf("No files were sent");
		exit(1);
	}
	fp = fopen(argv[1], "r");
	if (fp == NULL)
		exit(1);
	LabelsArr(fp, labels);
	fclose(fp);

	// Second run - Breaks and sets the code inside the memory array

	fp = fopen(argv[1], "r");
	if (fp == NULL)
		exit(1);
	MemoryArr(fp, memo);
	fclose(fp);

	// Writes the labels address into the memory

	SetLabelAddress(memo, labels);
	
	// Writes the memory into the file as an Hexa number

	fp = fopen(argv[2], "w");
	if (fp == NULL)
		exit(1);
	WriteToFile(fp, memo);
	fclose(fp);

	return(0);
}


// Input:
// fp - A pointer to the asm file
// labels - An array of labels and their address

// The function finds the labels and thier addresses and sets them in an array

void LabelsArr(FILE* fp, Label labels[]) {
	char line[LINE_LEN];
	char current_label[LABEL_LEN];
	int label_count = 0;
	int pc = 0;
	while (!feof(fp)) {
		fgets(line, LINE_LEN, fp);
		if (FirstCharInLine(line) == '#') //Skip the line if it's just a comment
		{
			continue;
		}
		if (FirstCharInLine(line) == '.') //Skip the line if it's .word
		{
			continue;
		}
		if (FirstCharInLine(line) == '\n') //Skip the line if it's empty
		{	
			continue;
		}
		CleanLine(line);
		strcpy(current_label, IsItALabel(line));
		if (_stricmp(current_label, "0") != 0) { // If current label is indeed a label
			labels[label_count].address = pc;
			strcpy(labels[label_count].name, current_label);
			label_count++;
			continue;
		}
		pc ++;
	}
}

char FirstCharInLine(char* line) {
	while (*line == ' ' || *line == '\t') {// Ignore spaces and tabs
		line++;
	}
	return *line;
}

// Input:
// line

// The function removes comments from a code line

void CleanLine(char line[]) {
	for (char* cp = line; *cp != '\0'; cp++) { // clean line from comments
		if (*cp == '#') {
			*cp = '\n';
			cp++;
			*cp = '\0';
			break;
		}
	}
}
// Input:
// line - A string representing a line in the input file

// The function checks if a line is a label line. If so it returns the label name, else returns 0

char *IsItALabel(char line[LINE_LEN]) {
	char label[LABEL_LEN] = "0";
	char cp[LINE_LEN];
	char* end_of_label = NULL;
	strcpy(cp, line);
	int i = 0;
	while (!isalpha(cp[i])) {
		if (cp[i] == '\0')
			return label;
		i++;
	}
	end_of_label = strchr(cp, ':');
	if (end_of_label != NULL) {// The line is a label
		*end_of_label = '\0';
		return &cp[i];
	}
	return label;
}

// Input:
// fp - A pointer to the asm file
// memo - The memory array

// The function sets the memory matrix with the code

void MemoryArr(FILE* fp, Memory memo[MEM_SIZE]) {
	char line[LINE_LEN], is_label[LABEL_LEN], opcode[11], rd[11], rs[11], rt[11], imm[11];
	char* colon_pointer = NULL;
	int i = 0, memo_indx = 0;

	while (fgets(line, LINE_LEN, fp)) {
		i = 0;
		if (FirstCharInLine(line) == '#') // If the line is a comment line, skip it
			continue;
		CleanLine(line);
		if (_stricmp(IsItALabel(line), "0") != 0)  // The line is a label
				continue;

		// line is not a label - check if it's blank
		while (line[i] == ' ' || line[i] == '\t' || line[i] == ',')
			i++;
		if (line[i] == '\n')
			continue;

		//line is ok
		strcpy(opcode, NextWord(i, line)); // Gets opcode
		i += strlen(opcode);
		strcpy(rd, NextWord(i, line)); // Gets rd

		//If .word - puts the address in rd and the value in imm
		if (_stricmp(opcode, ".word") == 0) { // If .word
			int address = (int)strtol(rd, NULL, 0);
			i += strlen(rd)+1;
			strcpy(memo[address].rd, rd);;
			strcpy(rs, NextWord(i, line)); // Gets rs
			strcpy(memo[address].imm, rs);
			strcpy(memo[address].opcode, ".word");
			strcpy(memo[address].rs, "");
			strcpy(memo[address].rt, "");
			continue;
		}

		while (line[i] != ',')
			i++;
		//i += strlen(rd) + 2;
		strcpy(rs, NextWord(i, line)); // Gets rs
		i++;
		while (line[i] != ',')
			i++;
		//i += strlen(rs) + 2;
		strcpy(rt, NextWord(i, line)); // Gets rt
		i++;
		while (line[i] != ',')
			i++;
		strcpy(imm, NextWord(i, line)); // Gets imm
		strcpy(memo[memo_indx].imm, imm);
		strcpy(memo[memo_indx].opcode, opcode);
		strcpy(memo[memo_indx].rd, rd);
		strcpy(memo[memo_indx].rs, rs);
		strcpy(memo[memo_indx].rt, rt);

		memo_indx ++;
	}
}

// Input:
// start index - Where to start reading
// line - The rest of the line to read

// The function returns the next "word" in the line

const char* NextWord(int start_index, char const line[]) {
	char word[LABEL_LEN];
	int i = 0;
	while (isalnum((int)line[start_index]) == 0 && line[start_index] != '.' && line[start_index] != '$' && line[start_index] != '-')
		start_index++;
	while (isalnum((int)line[start_index]) != 0 || line[start_index] == '.' || line[start_index] == '$' || line[start_index] == '-') {
		word[i] = line[start_index];
		start_index++;
		i++;
	}
	word[i] = '\0';
	return word;
}

// Input:
// memo - The array of the loaded memory
// labels - The matrix with the labels and thier addresses

// The function changes the labels in the memory to thier addresses

void SetLabelAddress(Memory memo[MEM_SIZE], Label labels[MEM_SIZE]) {
	Label* current_label_pointer = labels;
	Memory* memo_pointer = memo;
	char current_label_name[LABEL_LEN], current_memo_imm[LABEL_LEN], current_label_address[LABEL_LEN];

	while (_stricmp(current_label_pointer->name, "") != 0) {
		strcpy(current_label_name, current_label_pointer->name); // Label's name
		_itoa(current_label_pointer->address, current_label_address, 10); // Label's address as string
		while (_stricmp(memo_pointer->opcode, "") != 0) { // Searches for labels in the memory
			strcpy(current_memo_imm, memo_pointer->imm);
			if (_stricmp(current_memo_imm, current_label_name) == 0) {
				strcpy(memo_pointer->imm, current_label_address);
			}
			memo_pointer++;
		}
		memo_pointer = memo; // Returns the pointer to the beginning of the memory
		current_label_pointer++;
	}
}

// Input:
// file pointer and memory array

// The function writes the memory into the file as an Hexa number

void WriteToFile(FILE* fp, Memory memo[MEM_SIZE]) {
	char  hex_rd = '0', hex_rs = '0', hex_rt = '0';
	char* hex_imm=calloc(13, sizeof(char));
	char* hex_opcode = calloc(3, sizeof(char));
	int memo_indx = 0, num_of_lines_to_print = 0;

	num_of_lines_to_print = NumOfLines(memo); // The number of lines that needs to be printed

	while (memo_indx <= num_of_lines_to_print) {
		if (_stricmp(memo->opcode, "") != 0) {
			strcpy(hex_opcode, OpToHex(memo->opcode));
			if (memo->imm[0] == '0' && (memo->imm[1] == 'x' || memo->imm[1] == 'X')) { // If it's an hexa number
				strcpy(hex_imm, &memo->imm[2]);
				concatZeros(hex_imm);
			}
			else if (atoi(memo->imm)>=0){
				strcpy(hex_imm, fromDeci(hex_imm, 16, atoi(memo->imm)));
			}
			else {
				sprintf(hex_imm, "%X", atoi(memo->imm));
				strcpy(hex_imm, &hex_imm[5]);
			}
			
			hex_rd = RegToHex(memo->rd);
			hex_rs = RegToHex(memo->rs);
			hex_rt = RegToHex(memo->rt);

			if (_stricmp(hex_opcode, "..") == 0) { // If the opcode is .word
				if (memo->imm[0] == '0' && (memo->imm[1] == 'x' || memo->imm[1] == 'X')) { //if it's an hexa number
					strcpy(hex_imm, &memo->imm[2]);
				}
				else {
					sprintf(hex_imm, "%X", atoll(memo->imm));
				}
				concatZerosWord(hex_imm);
				fputs(hex_imm, fp);
				putc('\n', fp);
				strcpy(hex_imm, "000");
				strcpy(hex_opcode, "00");
				hex_rd = '0', hex_rs = '0', hex_rt = '0';
				memo_indx++;
				memo++;
				continue;
			}
			}
			
		// Writes to file
		
		fputs(hex_opcode, fp);
		putc(hex_rd, fp);
		putc(hex_rs, fp);
		putc(hex_rt, fp);
		fputs(hex_imm, fp);
		putc('\n', fp);
		strcpy(hex_imm, "000");
		strcpy(hex_opcode, "00");
		hex_rd = '0', hex_rs = '0', hex_rt = '0';
		memo_indx++;
		memo++;
	}
}

// Input:
// memo - An array containing the none blank lines

// The function returns the number of lines that needs to be printed to memin

int NumOfLines(Memory memo[MEM_SIZE]) {
	int indx = 4095;

	while (indx >= 0) {
		if (_stricmp(memo[indx].opcode, "") != 0 || _stricmp(memo[indx].rd, "") != 0 || _stricmp(memo[indx].rs, "") != 0 || _stricmp(memo[indx].rt, "") != 0 || _stricmp(memo[indx].imm, "") != 0)
			break;
		indx--;
	}

	return indx;
}

// Input:
// opcode - String to convert

// The function returns the opcode representation in Hexa

char* OpToHex(const char opcode[LABEL_LEN]) {
	char* hex_op=calloc(3, sizeof(char));

	if (_stricmp(opcode, "add") == 0)
		strcpy(hex_op,"00");
	else if (_stricmp(opcode, "sub") == 0)
		strcpy(hex_op, "01");
	else if (_stricmp(opcode, "mul") == 0)
		strcpy(hex_op, "02");
	else if (_stricmp(opcode, "and") == 0)
		strcpy(hex_op, "03");
	else if (_stricmp(opcode, "or") == 0)
		strcpy(hex_op, "04");
	else if (_stricmp(opcode, "xor") == 0)
		strcpy(hex_op, "05");
	else if (_stricmp(opcode, "sll") == 0)
		strcpy(hex_op, "06");
	else if (_stricmp(opcode, "sra") == 0)
		strcpy(hex_op, "07");
	else if (_stricmp(opcode, "srl") == 0)
		strcpy(hex_op, "08");
	else if (_stricmp(opcode, "beq") == 0)
		strcpy(hex_op, "09");
	else if (_stricmp(opcode, "bne") == 0)
		strcpy(hex_op, "0A");
	else if (_stricmp(opcode, "blt") == 0)
		strcpy(hex_op, "0B");
	else if (_stricmp(opcode, "bgt") == 0)
		strcpy(hex_op, "0C");
	else if (_stricmp(opcode, "ble") == 0)
		strcpy(hex_op, "0D");
	else if (_stricmp(opcode, "bge") == 0)
		strcpy(hex_op, "0E");
	else if (_stricmp(opcode, "jal") == 0) // special case: memory cell is actually the immediate cell
		strcpy(hex_op, "0F");
	else if (_stricmp(opcode, "lw") == 0)
		strcpy(hex_op, "11");
	else if (_stricmp(opcode, "sw") == 0)
		strcpy(hex_op, "12");
	else if (_stricmp(opcode, "reti") == 0) // special case: memory cell is actually the immediate cell
		strcpy(hex_op, "13");
	else if (_stricmp(opcode, "in") == 0)
		strcpy(hex_op, "14");
	else if (_stricmp(opcode, "out") == 0)
		strcpy(hex_op, "15");
	else if (_stricmp(opcode, "halt") == 0) // special case: memory cell is actually the immediate cell
		strcpy(hex_op, "16");
	else if (_stricmp(opcode, ".word") == 0) // special case: cell was modified by .word
		strcpy(hex_op, "..");

	return hex_op;
}

// Input:
// register string to convert

// The function returns the register representation in Hexa

char RegToHex(const char reg_name[LABEL_LEN]) {
	char hex_op = NULL;

	if (_stricmp(reg_name, "$zero") == 0)
		hex_op = '0';
	else if (_stricmp(reg_name, "$imm") == 0)
		hex_op = '1';
	else if (_stricmp(reg_name, "$v0") == 0)
		hex_op = '2';
	else if (_stricmp(reg_name, "$a0") == 0)
		hex_op = '3';
	else if (_stricmp(reg_name, "$a1") == 0)
		hex_op = '4';
	else if (_stricmp(reg_name, "$a2") == 0)
		hex_op = '5';
	else if (_stricmp(reg_name, "$a3") == 0)
		hex_op = '6';
	else if (_stricmp(reg_name, "$t0") == 0)
		hex_op = '7';
	else if (_stricmp(reg_name, "$t1") == 0)
		hex_op = '8';
	else if (_stricmp(reg_name, "$t2") == 0)
		hex_op = '9';
	else if (_stricmp(reg_name, "$s0") == 0)
		hex_op = 'A';
	else if (_stricmp(reg_name, "$s1") == 0)
		hex_op = 'B';
	else if (_stricmp(reg_name, "$s2") == 0)
		hex_op = 'C';
	else if (_stricmp(reg_name, "$gp") == 0)
		hex_op = 'D';
	else if (_stricmp(reg_name, "$sp") == 0)
		hex_op = 'E';
	else if (_stricmp(reg_name, "$ra") == 0)
		hex_op = 'F';

	return hex_op;
}



// To return char for a value. For example '2' 
// is returned for 2. 'A' is returned for 10. 'B' 
// for 11 
char reVal(int num)
{
	if (num >= 0 && num <= 9)
		return (char)(num + '0');
	else
		return (char)(num - 10 + 'A');
}

// Utility function to reverse a string 
void strev(char *str)
{
	int len = strlen(str);
	int i;
	for (i = 0; i < len / 2; i++)
	{
		char temp = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = temp;
	}
}

// Function to convert a given decimal number 
// to a base 'base' and 

char* fromDeci(char res[], int base, int inputNum)
{
	int index = 0;  // Initialize index of result 

	// Convert input number is given base by repeatedly 
	// dividing it by base and taking remainder 
	
	while (inputNum > 0)
	{
		res[index++] = reVal(inputNum % base);
		inputNum /= base;
	}
	res[index] = '\0';

	// Reverse the result 

	strev(res);

	concatZeros(res);

	return res;
}

// Input:
// string - the line that is about to be put in the file

// The function adds zeros for the string if it's too short

void concatZeros(char* string)
{
	if (strlen(string) == 3) {
		return;
	}
	char* old_value = calloc(strlen(string) + 1, sizeof(char));
	strcpy(old_value, string);
	int num_zeros = 3 - strlen(string); // The amount of zeros to add
	if (num_zeros == 3) {
		strcpy(string, "000\0");
		strcat(string, old_value);
	}
	else if (num_zeros == 2) {
		strcpy(string, "00\0");
		strcat(string, old_value);
	}
	else if (num_zeros == 1) {
		strcpy(string, "0\0");
		strcat(string, old_value);
	}
	free(old_value);
}
// Input:
// string - the line that is about to pe put in the file

// The function adds zeros for .word if the string is too short

void concatZerosWord(char* string)
{
	if (strlen(string) == 5) {
		return;
	}
	char* old_value = calloc(strlen(string) + 1, sizeof(char));
	strcpy(old_value, string);
	int num_zeros = 5 - strlen(string); // The amount of zeros to add
	if (num_zeros == 3) {
		strcpy(string, "000\0");
		strcat(string, old_value);
	}
	else if (num_zeros == 2) {
		strcpy(string, "00\0");
		strcat(string, old_value);
	}
	else if (num_zeros == 4) {
		strcpy(string, "0000\0");
		strcat(string, old_value);
	}
	else if (num_zeros == 5) {
		strcpy(string, "00000\0");
		strcat(string, old_value);
	}
	else if (num_zeros == 1) {
		strcpy(string, "0\0");
		strcat(string, old_value);
	}
	free(old_value);
}