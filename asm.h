#ifndef Assembler_h
#define Assembler_h

#include <stdio.h>	
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>


#define LABEL_LEN 50
#define LINE_LEN 500
#define MEM_SIZE 4096

typedef struct label {
	char name[51];
	int address;
} Label;

typedef struct memory {
	char opcode[10];
	char rd[10];
	char rs[11];
	char rt[10];
	char imm[50];
} Memory;

void LabelsArr(FILE* fp, Label labels[]);
char FirstCharInLine(char* line);
char *IsItALabel(char line[LINE_LEN]);
void CleanLine(char line[]);
void MemoryArr(FILE* fp, Memory memo[MEM_SIZE]);
const char* NextWord(int start_index, char const line[]);
void SetLabelAddress(Memory memo[MEM_SIZE], Label labels[MEM_SIZE]);
void WriteToFile(FILE* fp, Memory memo[MEM_SIZE]);
int NumOfLines(Memory memo[MEM_SIZE]);
char* OpToHex(const char opcode[LABEL_LEN]);
char RegToHex(const char reg_name[LABEL_LEN]);
char* fromDeci(char res[], int base, int inputNum);
void strev(char *str);
char reVal(int num);
void concatZeros(char* string);
void concatZerosWord(char* string);
#endif
