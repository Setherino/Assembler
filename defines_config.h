#ifndef DEFINES_CONFIG_H
#define DEFINES_CONFIG_H

#define MAX_FILE_WIDTH 128
#define MAX_FILE_LENGTH 200
#define MAX_FILES 15
#define LINE_SIZE 40	// For c-strings
#define MAX 150			// Size of simulator's memory
#define COL 7			// number of columns for output
#define varLength 3		// Max length of labels. Includes string stopper

char ASM_FILE_NAME[LINE_SIZE] = "CiancioTest11.asm";		// Default parameter. Can be changed in settings
char preferencesFile[LINE_SIZE] = "preferences.txt";

//OPERAND TYPES, REGISTERS AND OTHER
#define numDataTypes 8
#define AXREG 0
#define BXREG 1
#define CXREG 2
#define DXREG 3
#define BXADR 4
#define BXADRPLUS 5
#define MEMORYLOC 6
#define CONSTANT 7

//enum operType { reg, mem, constant, arrayBx, arrayBxPlus, none };  //list of all types of operand types

	// Instructions
#define numInstructions 21	// The number of supported instructions (includes newline & comment)
#define HALT 5
#define MOVREG 192
#define MOVMEM 224
#define ADD 160
#define SUB 128
#define AND 64
#define OR  32
#define PUT 7
#define GET 6
#define CMP 96
#define JE 8
#define JNE 9
#define JB 10
#define JBE 11		
#define JA 12
#define JAE 13
#define JMP 14
#define FUN 4
#define RET 3
#define BLANK 0		// Just inserts a new line
#define COMNT 0		// Never inserted into memory (see control word), just a placeholder for the assembler

//boolean
#define TRUE 1
#define FALSE 0

typedef short int Memory;		// Sets the type of memory to short int 

// The string names of all the instructions that the code compares against. Longer instructions from a similar set must come first.
const char instructionTable[numInstructions][LINE_SIZE] = { "mov [\0","mov\0","add\0","sub\0","and\0","or\0","cmp\0","put\0","get\0",
				  "ret\0","halt\0","jmp\0","jne\0","jbe\0","jae\0","ja\0","jb\0","je\0","fun\0","\n\0",";\0" };

//Registers and flag add the stack pointer
struct Registers
{
	int AX;
	int BX;
	int CX;
	int DX;
	int flag;
}regis;

//GLOBAL VARIABLES
Memory memory[MAX] = { 0 };		// Global variable the memory of the virtual machine
Memory address;					// Global variable the current address in the virtual machin
Memory stackPointer;			// Global variable the current address of the stack pointer
int internalError = FALSE;		// For detecting program anomalies (see errorMessages above)
bool debug = false;				// For debugging
int dumpType = 0;				// 0 - Readable, 1 - Decimal, 2 - Hex
int latestOutput = -1;			// Stores the most recent output (for easier debuging)

// This is the lookup table for all the instructions, matched with the instructionTable.
// The "Control word" is a 6-bit binary number that tells the compiler how to compile each command. Allows for adding commands without modifying compiler code.
// Two parameter commands go first, then single param, then jmps, then functions, then the non-instruction ones.
const Memory controlWordKey[numInstructions][2] = {
	{MOVMEM, 42},		// 101010 - Inline params,	initCommandBW,	paramDecodeBW
	{MOVREG, 52},		// 110100 - Inline params,	doInitCommand,	doParamDecode 
	{ADD,	 52},		// 110100 - Inline params,	doInitCommand,	doParamDecode 
	{SUB,    52},		// 110100 - Inline params,  doInitCommand,  doParamDecode
	{AND,    52},		// 110100 - Inline params,  doInitCommand,  doParamDecode
	{OR,     52},		// 110100 - Inline params,  doInitCommand,  doParamDecode
	{CMP,    52},		// 110100 - Inline params,	doInitCommand,	doParamDecode 
	{PUT,    16},		// 010000 - doInitCommand
	{GET,    16},		// 010000 - doInitCommand
	{RET,    16},		// 010000 - doInitCommand
	{HALT,   16},		// 010000 - doInitCommand
	{JMP,    10},		// 001010 - initCommandBW, paramDecodeBW
	{JNE,    10},		// 001010 - initCommandBW, paramDecodeBW
	{JBE,    10},		// 001010 - initCommandBW, paramDecodeBW
	{JAE,    10},		// 001010 - initCommandBW, paramDecodeBW
	{JA,     10},		// 001010 - initCommandBW, paramDecodeBW
	{JB,     10},		// 001010 - initCommandBW, paramDecodeBW
	{JE,     10},		// 001010 - initCommandBW, paramDecodeBW
	{FUN,     1},		// 000001 - doTheFunction
	{BLANK,  16},		// 010000 - doInitCommand
	{COMNT,  0},		// 000000 - None
};

//internal debugging errors
#define PARAMS_OVERLOAD 8
#define UNKNOWN_VARIABL 7
#define UNKNOWN_PRAMETR 6
#define RUNTIME_UNKNOWN 5
#define INVALID_CNSTANT 4
#define COMPILE_UNKNOWN 3
#define COMPILE_OVERRUN 2
#define OVERRUN 1

// Errors to assist in debugging ASM programs. They don't actually halt execution, "fatal" just means very bad.
const char errorMessages[9][LINE_SIZE] = { "No errors reported!","FATAL; INSTRUCTION = ZERO", "COMPILE UNKNOWN",
		 "FATAL; CANNOT STORE DATA IN CONSTANT","FATAL; UNKNOWN INSTRUCTION","FATAL; COMPILE OVERRUN",
		 "COMPILER: UNKNOWN PARAMETER","UNKNOWN VARIABLE","TOO MANY 3-BIT PARAMETERS" };
#endif