/*
 * CSS 372 - Fall 2015
 * Group: Geoffrey Tanay, Matthew Seto
 */

#include <stdio.h>
#include <stdlib.h>
#include "SC4.h"

#define FILE_NAME_LENGTH 100

/*
 * This function sets the status word for operations that do not overflow
 * or carry. Thus only the zero or negative flags can be set.
 */
int setStatusWord(int Result) {
	int statusWord = 0;
	if (Result == 0) {
		statusWord |= STATUS_ZERO;
	} else if (Result < 0) {
		statusWord |= STATUS_NEG;
	}
	return statusWord;
}

/*
 * This function sets the status word for operations that can overflow,
 * ADD and SUB.
 */
int setStatusOverflow(int A, int B, int R) {
	int statusWord = setStatusWord(R);
	if (A * B > 0 && A * R < 0) {
		statusWord |= STATUS_OVER;
	}
	return statusWord;
}

/*
 * This function prints the debug status to standard output. It prints
 * all registers and a section of memory.
 */
/*int printDebug(cpu * theCpu, int memory[]) 
{
	int i = 0, command = 0;
	//int instruction = theCpu->PC - INSTRUCTION_OFFSET;
	
	if (instruction < 0) {
		instruction = 0;
	}
	
	system("clear");
	printf("\n SC-4 Debug Monitor\n");
	printf(" Register File %16c Memory Dump\n", ' ');
	for (i = 0; i < 16; i++) {
		if (i == theCpu->PC - 1) {
			printf("%X: %08X    current --> %08X: %08X\n", i, theCpu->registerFile[i], i, memory[i]);
		} else {
			printf("%X: %08X %14c %08X: %08X\n", i, theCpu->registerFile[i], ' ', i, memory[i]);
		}
		//instruction++;
	}
	
	printf("\n PC: %08X  IR: %08X  SW: %08X", theCpu->PC, theCpu->IR, theCpu->statusWord);
	//printf(" MAR: %08X MDR: %08X ALU.A: %08X ALU.B: %08X ALU.R: %08X\n", MAR, MDR, A, B, R);
	printf("\n Commands: Load(1), Step(2), Run(3), Memory(4), Save(5), Terminate(9)		Enter:");
	scanf("%d", &command);
	return command;
}
*/


/*
 * This function reads a program form a file and places it into memory
 */
void loadFile(cpu * theCpu, int theMemory[]) {
	char fileName [FILE_NAME_LENGTH];
	FILE * hexFile;
	int status, address, data;
	
	printf(" Enter file name: ");
	scanf("%99s", fileName);
	//Open hexfile and place in memory
	hexFile = fopen(fileName, "r");
	if (hexFile == NULL) {
		printf(" Error opening file");
	} else {
		status = fscanf(hexFile, "%x%*c%x", &address, &data);
		theCpu->PC = address; //set PC to start of program
		while (status != EOF) {
			theMemory[address] = data;
			status = fscanf(hexFile, "%x%*c%x", &address, &data);
		}
	}
}

/*
 * This function prompts the user for the starting and ending addresses
 * of memory that he or she would like to save, and then saves memory
 * between those two addresses to a specified file.
 */
void saveToFile(int theMemory[]) {
	FILE *saveFile;
	char fileName[FILE_NAME_LENGTH];
	int start = 0, end = 0;
	int i = 0;
	
	printf("Name of file to save: ");
	scanf("%99s", fileName);
	printf("Starting address: ");
	scanf("%x", &start);
	printf("Ending address: ");
	scanf("%x", &end);
	
	saveFile = fopen(fileName, "w");
	if (saveFile == NULL) {
		printf(" Error opening file");
	} else {
		for (i = start; i <= end; i++) {
			fprintf(saveFile, "%X:%08X\n", i, theMemory[i]);
		}
		fclose(saveFile);
	}
}

/*
* This function allows a user to edit one particular memory location
* The user will be prompted for a memory address to edit, 
* Then the user will be prompted for the desired instruction/data to be stored at that location
*/
void editMemAtLocation(int theMemory[]){
	int i, newAddress, newValue;
	
	printf("Address to be modified: ");
	scanf("%x", &newAddress);
	//shift right to pad zeroes as required
	for(i = 0x80000000; i > 0x00000001; i>>1){
		if(!(newAddress & i)){
			newAddress >> 1;
		}
	}
	if(newAddress > MEM_SIZE || newAddress < 0){
		printf("ERR: memory index outside of memory bounds\n");
		return;
	}
	printf("\nNew value: ");
	scanf("%x", &newValue);
	
	theMemory[newAddress] = newValue; 

}
/*
 *This is simply an overloaded function of printDebug (renamed as I'm not sure if C supports overloading)
 *It takes all the standard input as well as an int variable that tells the monitor which 
 *section of memory the user wants to look at
 */
int printDebugAtLocation(cpu * theCpu, int memory[], int memLocation) 
{
	int i = 0, command = 0;
	
	system("clear");
	printf("\n SC-4 Debug Monitor 	Group: Geoffrey Tanay, Matthew Seto\n");
	printf(" Register File %16c Memory Dump\n", ' ');
	for (i = 0; i < 16; i++) {
		if (memLocation == theCpu->PC - 1) {
			printf("%X: %08X    current --> %08X: %08X\n", i, theCpu->registerFile[i], memLocation, memory[memLocation]);
		} else {
			printf("%X: %08X %14c %08X: %08X\n", i, theCpu->registerFile[i], ' ', memLocation, memory[memLocation]);
		}
		memLocation++;
	}
	
	printf("\n PC: %08X  IR: %08X  SW: %08X\n", theCpu->PC, theCpu->IR, theCpu->statusWord);
	//printf(" MAR: %08X MDR: %08X ALU.A: %08X ALU.B: %08X ALU.R: %08X\n", MAR, MDR, A, B, R);
	printf("\n Commands: Load(1), Step(2), Run(3), Memory(4), Save(5), Edit Memory(6), Terminate(9)		Enter:");
	scanf("%x", &command);
	return command;
}


