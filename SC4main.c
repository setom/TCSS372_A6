/*
 * CSS 372 - Fall 2015
 * Group: Geoffrey Tanay, Matthew Seto
 */
 
#include <stdlib.h>
#include <stdio.h>
#include "SC4.h"

/*
 * The main function of the SC4 simulator with debug interface. This
 * program starts a debug interface with options to load a new program
 * into memory, to step through, or to run through a loaded program. The
 * debugger will go through a loaded program until the program halts at 
 * which point  the step option will reset the PC and go through the 
 * program again.
 * 
 */
int main() {
	
	unsigned int state = FETCH;
	unsigned int microstate = IFETCH1;
	unsigned int Rx = 0; //first register
	unsigned int Ry = 0; //second register
	unsigned int Rz = 0; //third register
	int immed = 0;
	unsigned int memory [MEM_SIZE];
	int command = 0;
	int i = 0, j = 0; //loop variable
	cpu myCpu = { .PC = 0, .IR = 0, .MAR = 0, .MDR = 0, .A = 0, .B = 0, .R = 0, .statusWord = 0};
	cpu * cpuPointer = &myCpu;
	
	
	//fill registers with random values
	for (i = 0; i < REG_SIZE; i++) {
		myCpu.registerFile[i] = rand();
	}
	//fill memory with random values
	for (i = 0; i < MEM_SIZE; i++) {
		memory[i] = rand();
	}
	
	//Runs debugger interface until terminatae command is given
	while (command != TERMINATE) {
		
		switch (command) {
			case LOAD:
				loadFile(cpuPointer, memory);
				command = 0;
				break;
			case STEP:
				if (state == HALT) {
					myCpu.PC = 0;
					state = FETCH;
					microstate = IFETCH1;
				}
				break;
			case RUN:
				if (state == HALT) command = 0;
				break;
			case MEMORY:
				printf(" Starting memory address: ");
				scanf("%x", &j);
				//shift right to pad zeroes as required
				for(i = 0x80000000; i > 0x00000001; i>>1){
					if(!(j&i)){
						j >> 1;
					}
				}
				if(j > MEM_SIZE || j < 0){
					printf("ERR: memory index outside of memory bounds\n");
					j = 0;
				}
				command = 0;
				break;
			case SAVE:
				saveToFile(memory);
				command = 0;
				break;
			case EDITMEM:
				printf("Edit Memory\n");
				editMemAtLocation(memory);
				command = 0;
				break;
			default:
				command = printDebugAtLocation(cpuPointer, memory, j);
				break;
		}
		
		//Show the contents of memory at a specific location
		/*if(command == MEMORY){
			printf(" Starting memory address: ");
			scanf("%x", &j);
			//shift right to pad zeroes as required
			for(i = 0x80000000; i > 0x00000001; i>>1){
				if(!(j&i)){
					j >> 1;
				}
			}
			if(j > MEM_SIZE || j < 0){
				printf("ERR: memory index outside of memory bounds\n");
			} else {
				//print the debug monitor but show the address desired by the user
				command = printDebugAtLocation(regfile, memory, PC, IR, SW, MAR, MDR, A, B, R, j);
			}
		}*/
		
		//Run processor if in step or run command state
		if (state != HALT && (command == STEP || command == RUN)) {
			switch (state) {
				//fetch current instruction
				case FETCH:
					while (state == FETCH) {
						switch (microstate) {
							case IFETCH1:
								myCpu.MAR = myCpu.PC;
								myCpu.A = myCpu.PC;
								microstate = IFETCH2;
								break;
							case IFETCH2:
								myCpu.IR = memory[myCpu.MAR];
								microstate = IFETCH3;
								break;
							case IFETCH3:
								myCpu.B = 1;
								myCpu.R = myCpu.A + myCpu.B;
								myCpu.PC = myCpu.R;
								microstate = IFETCH4;
								break;
							case IFETCH4:
								state = DECODE;
								break;
						}
					}
					break;
				
				//Decode instruction	
				case DECODE:
					microstate = myCpu.IR >> OP_CODE_SHIFT;
					
					//set Rx if applicable
					if (microstate < BR || microstate > BRO) {
						Rx = myCpu.IR & RX_MASK;
						Rx = Rx >> RX_SHIFT;
					}
					
					//set Ry if applicable
					if ((microstate >= LD && microstate < PUSH)
						|| (microstate >= ADD && microstate < BR)) 
					{
						Ry = myCpu.IR & RY_MASK;
						Ry = Ry >> RY_SHIFT;
					}
					
					//do format specific decoding
					switch (microstate) {
						case LDI:
							immed = myCpu.IR & F1_MASK;
							//sign extend immediate value
							if (immed & F1_CHECK) {
								immed += F1_EXTENSION;
								if (immed >= 0) {
									state = HALT;
									printf("Sign extension error");
								}
							}
							break;
							
						case LD:
						case ST:
						case NOT:
							immed = myCpu.IR & F2_MASK;
							//sign extend immediate value
							if (immed & F2_CHECK) {
								immed += F2_EXTENSION;
								if (immed >= 0) {
									state = HALT;
									printf("Sign extension error");
								}
							}
							break;
							
						case ADD:
						case SUB:
						case AND:
						case OR:
							Rz = myCpu.IR & RZ_MASK;
							Rz = Rz >> RZ_SHIFT;
							break;
							
						case BR:
						case BRZ:
							immed = myCpu.IR & F4_MASK;
							//sign extend immediate value
							if (immed & F4_CHECK) {
								immed += F4_EXTENSION;
								if (immed >= 0) {
									state = HALT;
									printf("Sign extension error");
								}
							}
							break;	
					}
					state = EXECUTE;
					break;
				
				//execute instruction	
				case EXECUTE:
					
					//load ALU A
					if (microstate >= LD && microstate < BR) {
						myCpu.A = myCpu.registerFile[Ry];
					} else if (microstate >= BR && microstate <= BRO) {
						myCpu.A = myCpu.PC;
					}
					
					//load ALU B
					if (microstate >= ADD && microstate <= OR) {
						myCpu.B = myCpu.registerFile[Rz];
					} else if ((microstate >= LD && microstate <= ST)
								|| (microstate >= BR && microstate <= BRO))
					{
						myCpu.B = immed;
					} 
					
					//do specific instructions
					switch (microstate) {
						case LDI:
							myCpu.registerFile[Rx] = immed;
							myCpu.statusWord = setStatusWord(myCpu.registerFile[Rx]);
							break;
						case LD:
							myCpu.R = myCpu.A + myCpu.B;
							myCpu.MAR = myCpu.R;
							myCpu.MDR = memory[myCpu.MAR];
							myCpu.registerFile[Rx] = myCpu.MDR;
							myCpu.statusWord = setStatusWord(myCpu.MDR);
							break;
						case ST:
							myCpu.R = myCpu.A + myCpu.B;
							myCpu.MAR = myCpu.R;
							myCpu.MDR = myCpu.registerFile[Rx];
							memory[myCpu.MAR] = myCpu.MDR;
							break;
						case ADD:
							myCpu.R = myCpu.A + myCpu.B;
							myCpu.registerFile[Rx] = myCpu.R;
							myCpu.statusWord = setStatusOverflow(myCpu.A, myCpu.B, myCpu.R);
							//if addition causes a carry out of the leftmost bit
							if ((myCpu.A < 0 || myCpu.B < 0) && myCpu.R >= 0){
								myCpu.statusWord |= STATUS_CARRY;
							}
							break;
						case SUB:
							myCpu.R = myCpu.A - myCpu.B;
							myCpu.registerFile[Rx] = myCpu.R;
							//subtracting a negative is equivalent to adding a positive
							myCpu.statusWord = setStatusOverflow(myCpu.A, -myCpu.B, myCpu.R);
							//if subtraction requires a borrow into the leftmost bit
							if (myCpu.A >= 0 && myCpu.R < 0) {
								myCpu.statusWord |= STATUS_CARRY;
							}
							break;
						case AND:
							myCpu.R = myCpu.A & myCpu.B;
							myCpu.registerFile[Rx] = myCpu.R;
							break;
						case OR:
							myCpu.R = myCpu.A | myCpu.B;
							myCpu.registerFile[Rx] = myCpu.R;
							break;
						case NOT:
							myCpu.R = ~myCpu.A;
							myCpu.registerFile[Rx] = myCpu.R;
							break;
						case BR:
							myCpu.R = myCpu.A + myCpu.B;
							myCpu.PC = myCpu.R;
							break;
						case BRZ:
							if (myCpu.statusWord & STATUS_ZERO) {
								myCpu.R = myCpu.A + myCpu.B;
								myCpu.PC = myCpu.R;
							}
							break;
						case HALT:
							printf(" Program halted.");
					}
					
					//set status word for ALU operations without overflow
					if (microstate >= AND && microstate <= SHR) {
						myCpu.statusWord = setStatusWord(myCpu.R);
					}
					
					
					if (microstate == HALT) {
						state = HALT;
						command = 0;
					} else {
						state = FETCH;
						microstate = IFETCH1;
						if (command == STEP) {
							command = 0;
						}
					}
					break;
			}
		}	
	}
	return 0;
}
