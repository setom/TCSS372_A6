/*
 * CSS 372 - Fall 2015
 * Group: Geoffrey Tanay, Matthew Seto
 */

/*Interface Commands
 */
#define LOAD 1
#define STEP 2
#define RUN 3
#define MEMORY 4
#define SAVE 5
#define EDITMEM 6
#define TERMINATE 9

/*States  the FSM
*/

#define FETCH 0
#define DECODE 1
#define EXECUTE 2

/* Microstates for Fetch
 */
 
#define IFETCH1 0
#define IFETCH2 1
#define IFETCH3 2
#define IFETCH4 3

/* Microstates for Execute
 */
 
// data movement
#define LDI 1
#define LD 2
#define LDIN 3
#define ST 4
#define STIN 0x1f
#define MOV 5
#define PUSH 6
#define POP 7

//ALU operations
#define ADD 8
#define SUB 9
#define AND 0xa
#define OR 0xb
#define NOT 0xc
#define XOR 0xd
#define SHL 0xe
#define SHR 0xf

//Control of Flow
#define BR 0x10
#define BRZ 0x11
#define BRN 0x12
#define BRC 0x13
#define BRO 0x14
#define JSR 0x15
#define JSRZ 0x16
#define JSRN 0x17

//Miscellaneous
#define DI 0x18
#define EI 0x19
#define RET 0x1a
#define RETI 0x1b
#define TRAP 0x1c
#define HALT 0x1d

//size of memory
#define MEM_SIZE 100

//size of register file
#define REG_SIZE 16

//decode constants
#define OP_CODE_SHIFT 27

#define RX_SHIFT 23
#define RY_SHIFT 19
#define RX_MASK 0x07800000
#define RY_MASK 0x00780000

#define RZ_SHIFT 15
#define RZ_MASK 0x00078000

//Instruction format 1
#define F1_MASK 0x007fffff
#define F1_CHECK 0x00400000
#define F1_EXTENSION 0xff800000

//Instruction format 2
#define F2_MASK 0x0007ffff
#define F2_CHECK 0x00040000
#define F2_EXTENSION 0xfff80000

//Instruction format 4
#define F4_MASK 0x07ffffff
#define F4_CHECK 0x04000000
#define F4_EXTENSION 0xf8000000

//status word values
#define STATUS_ZERO 0x80000000
#define STATUS_NEG 0x40000000
#define STATUS_CARRY 0x20000000
#define STATUS_OVER 0x10000000

//struct that represents cpu
//groups all special registers and the register file

typedef struct CPU {
	unsigned int PC;
	unsigned int IR;
	unsigned int MAR;
	unsigned int MDR;
	int A;
	int B;
	int R;
	unsigned int statusWord;
	unsigned int registerFile [REG_SIZE];
} cpu;

//function definitions

/*
 * This function sets the status word for operations that do not overflow
 * or carry. Thus only the zero or negative flags can be set.
 */
int setStatusWord(int Result);

/*
 * This function sets the status word for operations that can overflow,
 * ADD and SUB.
 */
int setStatusOverflow(int A, int B, int R);

/*
 * This function prints the debug status to standard output. It prints
 * all registers and a section of memory.
 */
//int printDebug(cpu * theCpu, int memory[]);

/*
 * This function reads a program form a file and places it into memory
 */
void loadFile(cpu * theCpu, int theMemory[]);

/*
 * This function prompts the user for the starting and ending addresses
 * of memory that he or she would like to save, and then saves memory
 * between those two addresses to a specified file.
 */
 void saveToFile(int theMemory[]);
 
 /*
* This function allows a user to edit one particular memory location
* The user will be prompted for a memory address to edit, 
* Then the user will be prompted for the desired instruction/data to be stored at that location
*/
void editMemAtLocation(int theMemory[]);
 
 /*
  * Prints the debug information: all registers in the register file,
  * the PC, IR, and status word, and the contents of 16 sections of
  * memory that cen be changed with the memory dump command
  */
 int printDebugAtLocation(cpu * theCpu, int memory[], int memLocation);

