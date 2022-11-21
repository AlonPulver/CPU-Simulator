# Computer Architecture Course - Final Project - CPU-Simulator



## TABLE OF CONTENTS

>[ Summary ](#sum)

>[ Flow ](#flo)

>[ Basic Specification ](#spec)

>[ CPU Registers ](#cpu_reg)

>[ ISA & Memory ](#isa)

>[ I/O Devices ](#I/O)

>[ Interrupts ](#IRQ)

>[ Files ](#files)

>[ Assembly Code ](#asm)

<a name="sum"></a>
## SUMMARY

Basic Assembler and Simulator programs for a simplified MIPS Architecture Processor

The Simulator implements Arithmetic, Branching, and I/O instructions, Interrupts derived from devices and user-generated,
and interconnection between CPU and I/O devices  

The project is comprised of 3 parts: Assembly Files, Assembler program, and Simulator Program.
Both the Assembler and Simulator are implemented in C language

Lines of code: ~1800

<a name="flo"></a>
## FLOW

1. The Assembler receives an Assembly file which its content corresponds to the CPU's ISA (further description in '_ISA & Memory_' section)
For your Convenience, several Assembly files with different functionality are provided.
2. The Assembler translates the Assembly language to machine language instructions and outputs two txt files - Instruction Memory
and Data Memory files
3. The Simulator receives the output from the Assembler along with additional input files (further description in the '_files_' section)
and simulates a fetch-decode-execute loop according to the input files
4. The Simulator generates txt files which provide overall extensive information regarding CPU and I/O devices state at each cycle 

 

>![image](https://github.com/AlonPulver/CPU-Simulator/issues/2#issue-1458345823)

The files which are marked in red are the input files from the user and the files marked in green will
be created automatically by the Assembler and Simulator programs (description for each file is provided in the '_files_' segment below)

At your convenience, several Assembly files with different functionalities have been provided and further detailed in the '_Assembly_Files_Description.md_' file
<a name="spec"></a>
## BASIC SPECIFICATION
1. Clock Rate - 1024 cycles per second
2. Assembly Instructions which utilize the $imm register - takes 2 cycles to complete; other Instructions takes 1 cycle.
3. The CPU executes only a single instruction at a time

<a name="cpu_reg"></a>
## CPU REGISTERS

The CPU contains 16 32-bit registers, as seen below:

>![image](https://user-images.githubusercontent.com/118295713/203125403-8d5214b0-08a0-4780-ac1a-ba5df2c5a874.png)

<a name="isa"></a>
## ISA & MEMORY

Memory is divided between Instruction Memory and Data Memory:
  1. Instruction memory is comprised of 1024 20-bit instructions (1024 instructions encoded to machine language)
  2. Data Memory is comprised of 4096 32-bit memory addresses


**Assembly Code to Machine Language**

The CPU has two instructions encoding formats:
  1. Instructions which utilizes the $imm register:
 
![image](https://user-images.githubusercontent.com/72262159/128008429-2065a264-f0cd-4f3d-838b-80675e405c08.png)

  2. Other Instructions:

![image](https://user-images.githubusercontent.com/72262159/128007915-c94f7b1f-3e34-47be-a30f-2085c6e23a82.png)

**Instructions Set and Encoding**

![image](https://user-images.githubusercontent.com/72262159/128018301-f9f1e0f8-1810-4cd5-8bc0-ded6247fa479.png)

<a name="I/O"></a>
## I/O DEVICES
The CPU is connected to the following I/O devices - LEDs, screen, timer, and hard-drive

The processor is a dedicated I/O processor - each device has its own I/O register and the CPU is able to communicate with these registers using the 'in' and 'out' I/O commands. below are the I/O registers and their encodings:

![image](https://user-images.githubusercontent.com/72262159/128021392-f5402533-8c17-48aa-91c3-c535c1e7bc7f.png)

**Computer Screen**

The CPU is connected to a 352x288 monochromatic computer screen. each pixel is represented with 8 bits where 0 is black and 256 is white

 The screen has a 352x288 frame buffer which at any time will store the current screen state. at the beginning all values are set to 0

‘monitorx’ register contains the x coordinate where the CPU will change it's pixel value. equivalently to monitory register and y coordinate.
the ‘monitordata’ register holds the pixel value which the CPU wishes to write

**Timer**

when changing the ‘timerenable’ register value to 1 - the timer is activated. in each clock cycle where timer is enabled – ‘timercurren’t register's value is
incremented by 1

**Hard Drive**

The CPU is connected to 64KB hard-drive, comprised of 128 512-bytes sectors. The CPU uses DMA to copy a sector from main memory to disk and vice versa

It takes 1024 clock cycles to copy a sector which during this time the diskstatus register value will be 1 (indicating hard-drive is busy).  Upon receiving write/read command, the Assembly code must assure that the hard-drive is free to receive a new command by checking the diskstatus register. 
after 1024 clock cycles, diskcmd and diskstatus registers' values will be set to zero


<a name="IRQ"></a>
## INTERRUPTS
The CPU supports 3 types of interrupts, as detailed at the table below:

| Name  | Device  |  Description |
| ------------ | ------------ | ------------ |
| irq0  | Timer  | when timercurrent=timermax, 'irq0status' is set to 1 and timercurrent is reset to 0  |
|  irq1 |  Hard-Disk |  when DMA is done copying a sector - 'irq1status' is set to 1 |
| irq2  | User-Generated  |  The simulator is provided by the user with the irq2in.txt file which specifies at which clock cycles an interrupt should occur |  |

before carrying out each machine language instruction, the CPU examines the signals:
irq = (irq0enable & irq0status) | (irq1enable & irq1status) | (irq2enable & irq2status)
if irq  == 1, and the CPU is not currently using its interrupt service routine, the processor handles the signal, by moving the PC register to the address
stored in the irqhandler register while saving the original PC value in the irqreturn register
The Assembly code should reset the irqstatus registers.
terminating the interrupt service is done with the ‘reti’ command, which will result in PC=irqreturn

<a name="files"></a>
## FILES
At the beginning of the simulation, PC=0. with each iteration, the simulator fetches the next instruction according to the PC, decodes it, and updates registers' state and other internal constituents of the program

The Simulator program will receive the following files (files' path name) as command line arguments:

imemin.txt dmemin.txt diskin.txt irq2in.txt dmemout.txt regout.txt
trace.txt hwregtrace.txt cycles.txt leds.txt monitor.txt monitor.yuv diskout.txt

|File   |  Type |  Description |
| ------------ | ------------ | ------------ |
|  **imemin.txt** | input  | contains Instruction Memory (machine language instructions). each row is a memory address and contains an instruction comprised of 5 hex digits. if the number of rows is less than 1024, we assume that the rest of the data is uninitialized  |
|  **dmemin.txt** | input  |  contains Data Memory. each row contains data comprised of 8 hex digits. Data is stored in memory using a special command which can used in the assembly program -  **.word (address) (data)**  |
| **diskin.txt**  | input  | this file has the same format as 'dmemin.txt' and represents the data stored in the disk when the program is executed  |
|   **irq2in.txt** | input  |  specifies in which clock cycles an interrupt should occur (can contain nothing) |
|  **dmemout.txt** |  output | this file specifies the information stored on the RAM when the execution is finished. has the same format as 'dmemout.txt'  |
|  **regout.txt**  | output  | contains the CPU’s registers' content at the end of the simulation  |
| **trace.txt**  |  output | This file contains a line of text for each instruction executed by the simulator, in the following format: **PC INST R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 R10 R11 R12 R13 R14 R15**. each field is printed with hex digits. the PC is the Program Counter, INST is the encoded opcode, and the following are the register's content prior to executing the current command (with sign extension)  |
|  **hwregtrace.txt**  |  output | contains a line of text for each I/O operation, in the following format: **CYCLE READ/WRITE NAME DATA**. where CYCLE is the current time cycle, READ/WRITE is dependent on the operation executed, NAME is the HW register's name, and DATA is the data written/ read 
  |
|  **cycles.txt** | output  | contains two lines, one with the overall time cycles taken to simulate the program and the second with the overall machine instructions executed  |
| **monitor.txt**  |  output |  contains the pixel values at the end of the simulation on the screen. each row contains a single pixel's value represented with two hex digits. The screen's overview is top-down, left-right. |
|  **diskout.txt** | output  |  contains the disk's data at the end of the simulation |


<a name="asm"></a>
## ASSEMBLY CODE
As mentioned, the assembler program translates assembly file to machine language instructions. data can be stored into memory using the '.word (address) (data)' special command

Each assembly line of code must contain all of the 5 parameters separated by commas, for example:
```asm
add $t2, $t1, $t0, 0   # $t2 = $t1 + $t0
add $t1, $t1, $imm, 2  # $t1 = $t1 + 2
add $t1, $imm, $imm, 2 # $t1 = 2 + 2
```

In addition, the assembly files can contain LABELS. As in MIPS, A label can be placed at the beginning of a statement and can be assigned the current value of the active location counter and be served as an instruction operand. The following exemplifies proper usage of Labels:
```asm
>bne $imm, $t0, $t1, L1      # if ($t0 != $t1) goto L1 (reg1 = address of L1)
>add $t2, $t2, $imm, 1       # $t2 = $t2 + 1 (reg1 = 1)
>beq $imm, $zero, $zero, L2  # unconditional jump to L2 (reg1 = address L2)
L1:
>sub $t2, $t2, $imm, 1 # $t2 = $t2 – 1 (reg1 = 1)
L2:
>add $t1, $zero, $imm, L3    # $t1 = address of L3 (reg1 = address L3)
>beq $t1, $zero, $zero, 0    # jump to the address specified in t1 (reg1 = 0)
L3:
>jal $imm, $zero, $zero, L4 # function call L4, save return addr in $ra
>halt $zero, $zero, $zero, 0 # halt execution
L4:
>beq $ra, $zero, $zero, 0 # return from function in address in $ra (reg1=0)
```

Note: each label must start with an alphabet character and end with a colon

The Assembler program can handle white spaces, and blank lines in the assembly files - feel free to leave blank lines, use tabs etc. the program can handle it.
