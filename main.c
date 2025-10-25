// Seth Ciancio 11/8/23
// Assembler
// Input File: "testASM/CiancioTest11.asm".
// A program to simulate the operation of an x86 CPU.

//need to create the functions run the array notation. 
#define _CRT_SECURE_NO_WARNINGS  // lets us use depricated code

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX 150			// strlen of simulator's memory can be changed
#define COL 7			// number of columns for output
#define LINE_SIZE 40	// For c-strings
#define varLength 3		// Max length of variables. Includes string stopper

typedef short int Memory;		// Sets the type of memory to short int 

char ASM_FILE_NAME[LINE_SIZE] = "CiancioTest9.asm";		// Default parameter. Can be changed in settings
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

	//commands
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

	// The string names of all the instructions that the code compares against. Longer instructions from a similar set must come first.
const char instructionTable[numInstructions][LINE_SIZE] = { "mov [\0","mov\0","add\0","sub\0","and\0","or\0","cmp\0","put\0","get\0",
				  "ret\0","halt\0","jmp\0","jne\0","jbe\0","jae\0","ja\0","jb\0","je\0","fun\0","\n\0",";\0" };

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

	//boolean
#define TRUE 1
#define FALSE 0

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
		 "COMPILER: UNKNOWN PARAMETER","UNKNOWN VARIABLE","TOO MANY 3-BIT PARAMETERS"};

enum operType { reg, mem, constant, arrayBx, arrayBxPlus, none };  //list of all types of operand types

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

				//****************** Core Functions **********************

	//runMachineCode: Executes the machine code in memory.
void runMachineCode( );

	//ConvertToMachineCode: Converts a single line of ASM to machine code
		// INPUT: char line[ ] - a line of ASM to convert
void convertToMachineCode(char line[ ]);

	// assembler: Converts the entire ASM file and stores it in memory
void assembler( );

	// labelConvert: Converts varLength long labels in the ASM file to actual memory locations, fills fileBuffer with converted ASM file, returns fileBuffer length
int labelConvert(FILE* labeled, char fileBuffer[MAX][LINE_SIZE]);

	//printMemoryDump: Prints the memory of the simulated computer.
	// dumpType: 0 = Readable, 1 = Hex, 2 = Decimal
void printMemoryDump(int dumpType);

	//printMemoryDumpReadable: Prints memory with instruction names & labels for values.
void printMemoryDumpReadable();

				//****************** Helper Functions **********************

	//changeToLowerCase: replaces all capital letters in a string with lowercase letters.
		// INPUT: line[ ] - the string to turn lowercase
		// OUTPUT: line[ ] - the string made lowercase
void changeToLowerCase( char line[ ] );

	//getFromLine: Seperates a word from a line of text.
		// INPUT: line[] - the line to search; search & secondarySearch - the characters to search for; index - the location to start
		// OUTPUT: result - the first word found.
void getFromLine(char line[], char result[], char search, char secondarySearch, int* index); 

	//getValue: takes a register (int) and returns that register's value
		// INPUT: registerID - the integer ID of a register
		// OUTPUT: return - the value stored in the selected register
int getValue(int registerID);

	//putValue: takes a register ID and a value and puts that value into the corresponding register.
		// INPUT: registerID - the ID of a register you want to change; value - the value you want to put into the register;
void putValue(int registerID, int value);

	//paramDecode: Fills the last three bits of a command & puts it into memory
		// INPUT: param2 - the second parameter of a line of ASM; tempCommand - the first five bits of tempCommand; inlineParams - whether or not you want inline params
		// OUTPUT: stores tempCommand in memory
void paramDecode(char* param2, Memory tempCommand, bool inlineParams);

	//getInstructionName: Gets the name of an instruction from its numerican value.
		// INPUT: location - the location of an instruction;
		// OUTPUT: name - this is where it puts the name of the command
void getInstructionName(Memory location, char name[],Memory displayType[ ]);

	//popStack: pops a value from the stack
		// OUTPUT: Memory - the value popped off the stack
Memory popStack();

	//pushStack: pushes a value onto the stack
		// INPUT: value - the value to be pushed onto the stack
void pushStack(Memory value);

			//*************Compiler Aides********************

	// Compiles the FUN command
		//INPUT: a full line of asm
void fillFun(char line[]);

	//getLineWithoutTrash: gets one line of ASM, and removes the trash if it needs to.
bool getLineWithoutTrash(FILE *inputFile, char line[]);

//splitCommand: breaks one line of ASM into its constituent parts.
	// INPUT: line[] - one line of asm; 
	// OUTPUT: command[] - the instruction; param1[], param2[] -  the two parameters;
void splitCommand(char line[], char command[], char param1[], char param2[]);

			//*************Instructions//*************

	//runPUT: Outputs whatever is in the A register
void runPUT();

	//runCMP: Takes two ints, sets flag to 1 if param1 is bigger, 0 if they're equal, & -1 if param2 is bigger
		// INPUT: param1, param2 - the two parameters we're comparing
		// OUTPUT: adjusts flag
void runCMP(Memory param1, Memory param2);

	//runJMP: Takes a conditional statement & jumps if conditional is true
		// INPUT: conditional - a conditional statement which decides whether or not to jump
		// OUTPUT: adjusts instruction pointer
void runJMP(bool conditional);

	//runGET: Takes user input and puts it into the A register.
void runGET();

	//runFUN: Pushes state onto stack & moves address pointer to the function
void runFUN();

	//runRET: Pulls state from the stack & returns to correct point in execution
void runRET();
	
	//debugUI: Provides some basic tools for debugging.
void debugUI();
	
	//findString: searches an array of strings for a particular string
		// INPUT: searchFor[] - the string we're searching for
		// OUTPUT: int - the location of the string found in the array.
int findString(char searchFor[], const char stringArray[][LINE_SIZE], int limit);

	//optionsMenu: presents the user with an options menu to choose what program functionality they want to use
		// INPUT: printMemoryAtEnd - whether or not to do a memory dump at the end of the program
		// OUTPUT: printMemoryAtEnd - same thing as input, but might be changed by the user
bool optionsMenu(bool printMemoryAtEnd);
	
	//loadPreferences: loads settings (filename, debug, dump type, and printMemoryAtEnd) from preferencesFile
		// OUTPUT: printMemoryAtEnd
bool loadPreferences();

	// customSettings: enables the user to change their settings (filename, debug, dump type, and printMemoryAtEnd)
		// OUTPUT: printMemoryAtEnd
bool customSettings();

	// getType: returns the type of a parameter
		// INPUT: param - a string containing a parameter
		// OUTPUT: Memory - the integer ID of the type of the parameter (0-7)
Memory getType(char* param);

int main()
{
	stackPointer = MAX - 1;
	bool printMemoryAtEnd = false;
	printMemoryAtEnd = loadPreferences();	// Load preferences from file, if it's available

	while (true)
	{
		printMemoryAtEnd = optionsMenu(printMemoryAtEnd);	//display options menu

		printf("\n\nConverting Machine code...\n");
		
		assembler();

		if (debug)
		{
			debugUI();
		}

		printf("Running machine code...\n\n");

		runMachineCode();

		if (printMemoryAtEnd)
		{
			debugUI();
		}

		printf("\n\n\n\tProgram complete!\n\n");
	}
	return 0;
}

	// Enables the user to change their settings (filename, debug, dump type, and printMemoryAtEnd)
bool customSettings()
{
	char stringIn[LINE_SIZE];		// For taking string input
	char input;						// For taking char input
	bool printMemoryAtEnd = false;	// For returning

		// Get new filename
	printf("\n\n\n\nEnter filename: ");
	scanf("%s", &stringIn);
	strcpy(ASM_FILE_NAME, stringIn);

		// Get new debug mode
	printf("Debug mode? (t - trace / p - dump at end / n - no dumps): ");
	scanf(" %c", &input);			

	switch (input)			// Set debug mode
	{
	case 't':					// Trace
		debug = true;				// Enable tracing
		printMemoryAtEnd = false;	// Disable dumpAtEnd (redundant)
		break;
	case 'p':					// Dump at end
		debug = false;				// Disable tracing
		printMemoryAtEnd = true;	// Enable dumpAtEnd
		break;
	case 'n':					// No dumps
		debug = false;				// disable tracing
		printMemoryAtEnd = false;	// disable dumpAtEnd
		break;
	default:
		printf("\nError! Invalid input.");
		return customSettings();
	}

		// Get dump type
	printf("Print mode? (r - readable / d - decimal / h - hex): ");
	scanf(" %c", &input);

	switch (input)
	{
	case 'r':
		dumpType = 0;
		break;
	case 'd':
		dumpType = 1;
		break;
	case 'h':
		dumpType = 2;
		break;
	default:
		printf("\nError! Invalid input.");
		return customSettings();
	}

	printf("\n\n\tSettings updated.\n\n");

	return printMemoryAtEnd;
}

bool loadPreferences()
{
	FILE* fin;
	bool printMemoryAtEnd = false;

	fopen_s(&fin, preferencesFile, "r");
	if (fin == NULL)
	{
		printf("\tNo %s file found. Using default preferences.\n\n", preferencesFile);
		printMemoryAtEnd = false;
		debug = true;
	}
	else
	{
		char temp[LINE_SIZE];
		fgets(ASM_FILE_NAME, LINE_SIZE, fin);
		int index = 0;
		getFromLine(ASM_FILE_NAME, ASM_FILE_NAME, ' ', ' ', &index);
		fgets(temp, LINE_SIZE, fin);
		debug = atoi(temp) & 1;
		printMemoryAtEnd = atoi(temp) & 2;
		fgets(temp, LINE_SIZE, fin);
		dumpType = atoi(temp);
		fclose(fin);
		printf("Loaded preferences from file.\n\n");
	}

	return printMemoryAtEnd;
}

void removeLabelsFromFile(char fileName[LINE_SIZE])
{
	char fileBuffer[MAX][LINE_SIZE];
	FILE* fin;
	FILE* fout;
	fopen_s(&fin, ASM_FILE_NAME, "r");
	if (fin == NULL)
	{
		printf("Error, file didn't open\n\nExiting program...\n\n");
		system("pause");
		exit(1);
	}

	int fileSize = labelConvert(fin, fileBuffer);
	fclose(fin);

	fopen_s(&fout, fileName, "w");
	if (fin == NULL)
	{
		printf("Error, file didn't open\n\nExiting program...\n\n");
		system("pause");
		exit(1);
	}

	for (int i = 0; i < fileSize; i++)
	{
		char stringBuffer[LINE_SIZE];
		strcpy(stringBuffer, fileBuffer[i]);
		if (stringBuffer[0] != '\n')
		{
			fprintf(fout, "%s\n", stringBuffer);
		}
		else
		{
			fprintf(fout, "%s", stringBuffer);
		}
	}
	fclose(fout);

	printf("\n\n\tLabels removed & new file saved.\n\n");
}

bool optionsMenu(bool printMemoryAtEnd)
{
	FILE* fout;
	
	char debugModeNames[3][LINE_SIZE] = { "Readable","Decimal","Hexidecimal" };
	char traceMode[2][LINE_SIZE] = { "Disabled","Enabled" };
	printf("--------Seth Ciancio Assembler: Program to Simulate x86 CPU by me, Seth Ciancio--------\n");
	printf(" Memory: %d Bytes; Input file: %s; Tracing: %s; Dump Type: %s\n\n",MAX,ASM_FILE_NAME,traceMode[debug],debugModeNames[dumpType]);
	printf("\t1. Run program with current settings\n");
	printf("\t2. Run program without any debug or dumps\n");
	printf("\t3. Change settings (filename, dump mode, ect)\n");
	printf("\t4. Convert .ASM file to remove labels\n");
	printf("\t5. Save preferences to file\n");
	printf("\t6. Load preferences from file\n");
	printf("\t7. Exit program\n\n");
	printf("Please enter your selection (1-7): ");

	char input = 'n';
	char unlabeledFilename[LINE_SIZE];

	scanf(" %c", &input);
	bool tempDebug = false;


	switch (input)
	{
	case '1':								// Run with current settings
		return printMemoryAtEnd;
	case '2':								// Run with no dumps
		tempDebug = debug;
		debug = false;
		assembler();
		runMachineCode();
		debug = tempDebug;
		return optionsMenu(printMemoryAtEnd);
	case '3':								// Change settings
		printMemoryAtEnd = customSettings();
		return optionsMenu(printMemoryAtEnd);
	case '4':								// Convert file
		printf("\n\nNote: comments will also be removed from the file.\n");
		printf("Enter new file name: ");
		scanf("%s", &unlabeledFilename);
		removeLabelsFromFile(unlabeledFilename);
		return optionsMenu(printMemoryAtEnd);
	case '5':								// Save settings
		fopen_s(&fout, preferencesFile, "w");
		fprintf(fout,"%s\n", ASM_FILE_NAME);
		fprintf(fout, "%d%d\n", printMemoryAtEnd, debug);
		fprintf(fout, "%d\n", dumpType);
		fclose(fout);
		printf("\n\n\tSettings saved!\n\n");
		return optionsMenu(printMemoryAtEnd);
	case '6':
		printf("\n\nAttempting to load from %s...\n", preferencesFile);
		loadPreferences();
		return optionsMenu(printMemoryAtEnd);
	case '7':
		exit(0);
	default:
		printf("\n\n\n Invalid entry. Please enter a number from 1 to 6.\n\n\n");
		return optionsMenu(printMemoryAtEnd);
	}
	return optionsMenu(printMemoryAtEnd);
}

	//										Assembler: 
	//		Changes the assembly code to machine code and places the commands into the memory.

void assembler( )
{
	address = 0;
	FILE *fin;			  // File pointer for reading in the assembly code.

	fopen_s( &fin, ASM_FILE_NAME, "r" );
	if ( fin == NULL )
	{
		printf( "Error, file didn't open\n\nExiting program...\n\n" );
		system( "pause" );
		exit( 1 );
	}
	
	char fileBuffer[MAX][LINE_SIZE];
	int fileSize = labelConvert(fin, fileBuffer);

	regis.AX = 0;
	regis.BX = 0;
	regis.CX = 0;
	regis.DX = 0;
	regis.flag = 0;
	latestOutput = 0;

	for (int i = 0; i < MAX; i++)
	{
		memory[address] = 0;
		address++;
	}

	address = 0;

	for ( int i = 0; i < fileSize; i++ )
	{
		char line[LINE_SIZE];
		strcpy(line, fileBuffer[i]);
		convertToMachineCode(line);
	}
}

int labelConvert(FILE* labeled, char fileBuffer[MAX][LINE_SIZE])
{
	int fileSize = 0;
	char labels[MAX][LINE_SIZE];	// Stores all the names of the labels
	int labelAdr[MAX];				// Stores the line numbers the labels are found on.
	int numLabels = 0;				// The length of the previous two arrays.
	int adr = 0;					// The current address of the file we're on
	char line[LINE_SIZE] = "\n";	// Holds one line of .asm from the file
	int lineLength = 1;				// The length of the previous string
	char command[LINE_SIZE]; char param1[LINE_SIZE]; char param2[LINE_SIZE];
	Memory param1Type = 0; Memory param2Type = 1; Memory commandType = 0; // The types of each parameter
	int index = 0;									// For counting & such

		// First pass - finding all labels & their addresses
	while (getLineWithoutTrash(labeled, line))	// Go through the entire file
	{
		splitCommand(line, command, param1, param2);	// Break the command into three parts
		commandType = findString(line, instructionTable, numInstructions);	// Figure out what type of command it is

			// If it's shorter than two, not a command, & its made of letters, it's probably a label
		if (strlen(line) <= varLength && commandType == -1 && isalpha(line[0]))
		{
			strcpy(labels[numLabels], command);	// Add that label name to the list
			labelAdr[numLabels] = adr;			// Record its location
			numLabels++;
		}
		else	// If its not a label
		{
			strcpy(fileBuffer[fileSize], line);	// Just copy it into the buffer (for now)
			fileSize++;
			adr++;								// Keep track of the address we're on

			if (controlWordKey[commandType][0] == FUN)			// If it's a function
			{
				adr = adr + 3 + atoi(param2);	// Add 3 for the address, numparams, blank space, and add atoi(param2), the number of parameters.
			}
			else if (getType(param1) > BXADR || getType(param2) > BXADR)	// If it's any other two-byte command
			{
				adr++;	// account for the second byte
			}
		}
	}
	fclose(labeled);
	

		// Second pass - replace labels inside [parameters] with the right addresses
	char stringBuffer[LINE_SIZE];	// For holding & operating on a string taken from the fileBuffer
	for (int i = 0; i < fileSize; i++)
	{
		char newStr[LINE_SIZE] = "";			// For rebuilding the line of ASM w/o the label
		strcpy(stringBuffer, fileBuffer[i]);	// Get a line
		
		if (stringBuffer[0] == '\n')			// If it's a newline
		{
			continue;							// Leave it, otherwise we're going to mess it up
		}

		index = 0;
		getFromLine(stringBuffer, newStr, ' ', ' ', &index);	// Get the command
		index++;												// Skip next space
		
		while (stringBuffer[index] != '\0')						// Go through every parameter in the string
		{
			char temp[LINE_SIZE] = "";			// For VERY temporary string storage
			getFromLine(stringBuffer, temp, ' ', ' ', &index);	// Get next parameter

			if (getType(temp) == MEMORYLOC && isalpha(temp[1]))	// Check if it's got a label in it
			{
				int jndex = 1;									// Skip '[' char
				getFromLine(temp, temp, '[', ']', &jndex);		// Remove the brackets around the label
				jndex = findString(temp, labels, numLabels);	// Find the label in the list
				if (jndex == -1)								// ...If you couldn't find it
				{
					internalError = UNKNOWN_VARIABL;			// Throw an error
				}
				sprintf(newStr, "%s [%d]", newStr, labelAdr[jndex]); // Append newStr w/ the memory address
			}
			else											// If it doesn't have a label in it
			{
				sprintf(newStr, "%s %s", newStr, temp);		// Then just append the parameter as is
			}
			index++;									// Skip the next space
		}
		strcpy(fileBuffer[i], newStr);	
	}

	return fileSize;
}

	//								ConvertToMachineCode:
	//					Converts a single line of ASM to machine code

void convertToMachineCode(char line[])
{
	if (isdigit(line[0]))
	{
		memory[address] = atoi(line);
		address++;
		return;
	}

	char param1[LINE_SIZE]; char param2[LINE_SIZE];	// the two operands, could be empty
	Memory param1Type = -1; Memory param2Type = -1;	// the types of the two operands
	Memory machineCode = 0;							// One byte of converted asm code from the file
	Memory controlWord = 0;							// The 6 bit value that tells the compiler what to do
	Memory instruction = 0;							// The numerical representation of the instruction
	int index = 0;									// For counting and such.
	
	// Each instruction has a 6-bit control word to decide which functions need to be called to properly...
	// convert the line to machine code. Each bit enables or disables a particular function of the code below.
	// the control words for each instructoin are stored in a global array, since it needs to be accessed by the labelConvert
	bool doInlineParam;	// 100000 - Decides whether to put param info in the byte w/ the instruction
	bool doInitCommand;	// 010000 - Puts the command & param1 in the first 5 bits of the instruction
	bool initCommandBW;	// 001000 - Puts the command & param2 in the first 5 bits of the instruction
	bool doParamDecode;	// 000100 - Identifies & sends param2 to the final 3 bits of the instruction
	bool paramDecodeBW;	// 000010 - Identifies & sends param1 to the final 3 bits of the instruction
	bool doTheFunction;	// 000001 - Does FUN command FUNction. Fills every 8 bits of the instruction

			// Breaking apart the line (splitcommand)
	changeToLowerCase(line);						// Prevent case errors
	splitCommand(line, param1, param1, param2);		// Split the command apart. We don't use the command.
	param1Type = getType(param1);
	param2Type = getType(param2);

	int instructionID = 0;	// The index of the current instruction in the instructionTable
	instructionID = findString(line, instructionTable, numInstructions); // Search instruction table for the right command.

	if (instructionID == -1)				//If we went through all of them & didn't find it,
	{
		internalError = COMPILE_UNKNOWN;	// that's bad. Throw an error or whatever.
		return;								// Return to prevent illegal array access. 
	}

	instruction = controlWordKey[instructionID][0];
	controlWord = controlWordKey[instructionID][1];

	if ((param1Type > DXREG) && (param2Type > DXREG) && instructionID > 18)
	{
		internalError = PARAMS_OVERLOAD;
	}

	doInlineParam = controlWord & 32;	// 100000 - Decides whether to put param info in the byte w/ the instruction
	doInitCommand = controlWord & 16;	// 010000 - Puts the command & param1 in the first 5 bits of the instruction
	initCommandBW = controlWord &  8;	// 001000 - Puts the command & param2 in the first 5 bits of the instruction
	doParamDecode = controlWord &  4;	// 000100 - Identifies & sends param2 to the final 3 bits of the instruction
	paramDecodeBW = controlWord &  2;	// 000010 - Identifies & sends param1 to the final 3 bits of the instruction
	doTheFunction = controlWord &  1;	// 000001 - Does FUN command FUNction. Fills every 8 bits of the instruction
	
			// Multiply values of the parameters by the bool control word components (1 = true, 0 = false) to achieve desired result.
			// Might be faster than an if statment depending on how long the function calls take. Mostly I just think it's fun.
	machineCode = instruction + doInlineParam * (((param1Type * doInitCommand) + (param2Type * initCommandBW)) << 3);
	
	if (doParamDecode)	// 001000
	{
		paramDecode(param2, machineCode, doInlineParam);
		return;
	}
	if (paramDecodeBW)	// 000100
	{
		paramDecode(param1, machineCode, doInlineParam);
		return;
	}
	if (doTheFunction)	// 000010
	{
		fillFun(line);
		return;
	}

	if (controlWord > 0)// 000001
	{
		memory[address] = machineCode;
		address++;
	}
}

	//fills the last three bits of a command & puts it into memory.
void paramDecode(char* param2, Memory tempCommand, bool inlineParams)
{
	const int  indexOffset[numDataTypes] = { 0,0,0,0,1,4,1 }; // For knowing when the number starts.
	Memory numType = getType(param2);

	tempCommand = tempCommand + numType * inlineParams; // Puts in inline parameters if you want them

	memory[address] = tempCommand;
	address++;

	if (numType >= BXADRPLUS)				// if it needs a second byte,
	{
		int index = indexOffset[numType];				// Offset to start of number
		getFromLine(param2, param2, '[', ']', &index);	// Clean the number up
		memory[address] = atoi(param2);					// Put the (now clean) number into memory
		address++;
	}
}

	//Finds a string in an array of strings
int findString(char searchFor[], const char stringArray[][LINE_SIZE], int limit)
{
	int index = 0; int charCount = 0;
	while (index < limit)				// go through the list
	{
		if (stringArray[index][charCount] == '\0')	// if you make it to the end of the key string w/o finding a difference
		{
			return index;							// then that's the right one! (assumes longer strings come first, JAE before JA)
		}
													// If you found a difference or the end of the line string
		else if (stringArray[index][charCount] != searchFor[charCount] || searchFor[charCount] == '\0')
		{
			index++;								// Move onto the next one
			charCount = -1;							// & Reset the second counter (to -1 so it's at 0 for after iterating)
		}
		charCount++;								// Iterate second counter
	}

	return -1;	// If we went through the whole array & didn't find the string we were looking for.
}

	//Compiles the FUN command
void fillFun(char line[ ])
{
	char tempLine[LINE_SIZE];
	int index = 5;			// set it to 5 so it skips the 'fun [' part of the line
	int tempInt = 0;		// for storing the values of the parameters
	int numParams = 0;		// for storing the number of parameters (must have unique variable so we can use it for the loop)

		//put the command in memory
	memory[address] = FUN;
	address++;

		//Get function location
	getFromLine(line, tempLine, '[', ']', &index);	// pull the address from the line
	tempInt = atoi(tempLine);						// convert the string to a number
	memory[address] = tempInt;						// store first parameter (address of the function)
	address++;
	index++;										// iterate the index to account for the ']' character
	index++;										// iterate the index to account for the ' ' character

		//get second parameter (number of parameters)
	getFromLine(line, tempLine, ' ', ' ', &index);	// pull the # of parameters from the line
	numParams = atoi(tempLine);						// convert it to a number
	memory[address] = numParams;					// store number of parameters
	address++;
	index++;										// iterate the index to account for the ' ' character

		//get all the other parameters
	for (int i = 0; i < numParams; i++)
	{
		getFromLine(line, tempLine, ' ', ' ', &index);				// get next parameter
		index++;													// iterate the index to account for the ' ' character

		if (tempLine[0] == '[')										// if it's a memory address
		{
			int tempIndex = 1;										// set tempIndex to account for the '[' character
			getFromLine(tempLine, tempLine, '[', ']', &tempIndex);	// remove the brackets
		}
		tempInt = atoi(tempLine);					// convert the text to a number
		memory[address] = tempInt;					// store that bad boy
		address++;
	}

	memory[address] = 0;
	address++; 
}

void splitCommand(char line[], char command[], char param1[], char param2[])
{
	int index = 0;
	getFromLine(line, command, ' ', ' ', &index);			//  Get the instruction

	if (line[index] != '\0')						//  If the string continues...
	{
		index++;											//  Move past the space
		getFromLine(line, param1, ' ', ' ', &index);		//  Get the first parameter
		index++;											//  Move past the second space
		getFromLine(line, param2, ' ', ' ', &index);		//  Get the third parameter
	}
	else
	{
		strcpy(param1, "");
		strcpy(param2, "");
		return;
	}
}

	//								runMachineCode()
	//				Executes the machine code, the virtual machine

void runMachineCode( )
{
	// Bitmasks for simple instructions with parameters. For "special instructions" (see below) the whole byte is used for the opcode.
	Memory mask1 = 224;	// 111 00 000 - First 3 bits contain an instruction, see "special instructions" below for exceptions
	Memory mask2 = 24;	// 000 11 000 - Second 2 bits tell us which register the first parameter is in
	Memory mask3 = 7;	// 000 00 111 - Last 3 bits tell us where the second parameter is (register, BXADR, BXADRPLUS, MEMORYLOC, or CONST)
	Memory instruction = 0, param1 = 0, param2 = 0;
	Memory command = 0;
	address = 0;
	
	do
	{
		command = memory[address];
		address++;
		
		instruction = command & mask1;
		param1 = ((command & mask2) >> 3); //filter for param2, move right.
		param2 = command & mask3;
		
		switch (instruction) 
		{
		case MOVREG:														//MOVREG
			putValue(param1, getValue(param2));
			break;
		case MOVMEM:														//MOVMEM
			putValue(param2, getValue(param1));
			break;
		case ADD:															//ADD
			putValue(param1, getValue(param1) + getValue(param2));
			break;
		case SUB:															//SUB
			putValue(param1, getValue(param1) - getValue(param2));
			break;
		case AND:															//AND
			putValue(param1, getValue(param1) & getValue(param2));
			break;
		case OR:															//OR
			putValue(param1, getValue(param1) | getValue(param2));
			break;
		case CMP:															//CMP
			runCMP(param1, param2);
			break;
		case 0:														//Special Instructions
			switch (command) 
			{
			case PUT:													//PUT
				runPUT();
				break;
			case GET:													//GET
				runGET();
				break;
			case JMP:													//JMP
				runJMP(true);
				break;
			case JE:													//JE
				runJMP(regis.flag == 0);
				break;
			case JNE:													//JNE
				runJMP(regis.flag != 0);
				break;
			case JB:													//JB
				runJMP(regis.flag < 0);
				break;
			case JBE:													//JBE
				runJMP(regis.flag <= 0);
				break;
			case JA:													//JA
				runJMP(regis.flag > 0);
				break;
			case JAE:													//JAE
				runJMP(regis.flag >= 0);
				break;
			case FUN:													//FUN
				runFUN();
				break;
			case RET:													//RET
				runRET();
				break;
			default:													//ZERO ERR
				break;
			}
			break;
		default:
			internalError = RUNTIME_UNKNOWN;							//UKN ERR
			return;
		}
		
		if (debug)		//Debugging stuff
		{
			debugUI();
		}
	} while (command != HALT);
}

	//Stores the state in the stack & moves to the location of a function
void runFUN()
{
	// Store the state of the machine on the stack
	pushStack(regis.flag);
	pushStack(regis.AX);
	pushStack(regis.BX);
	pushStack(regis.CX);
	pushStack(regis.DX);
	pushStack(address + memory[address + 1] + 3);

	memory[memory[address] - 1] = address + 1;

	address = memory[address];
}

	//Pulls the state from the stack & returns to the correct place in execution
void runRET()
{
	address = popStack();

	//put return value in blank space.
	memory[address - 1] = regis.AX;

	//restore the machines state from the stack
	regis.DX = popStack();
	regis.CX = popStack();
	regis.BX = popStack();
	regis.AX = popStack();
	regis.flag = popStack();
}

	//Prints the value of the A register to the screen.
void runPUT() 
{
	latestOutput = regis.AX;
	printf(" -> value in A register: %d\n", regis.AX);
}

	//Compares two parameters and sets the flag to 1 if the first is bigger, -1 
	//if the second is bigger, and 0 if they're the same
void runCMP(Memory param1, Memory param2) 
{
	int compareValue = getValue(param1) - getValue(param2);
	regis.flag = (compareValue > 0) + ((compareValue < 0) * -1);
}

	//runs the jump instruction based on a conditional.
void runJMP(bool conditional) 
{
	address = ((address + 1) * !conditional) + (memory[address] * conditional);
}

	// Takes number and puts it into the A register.
void runGET() 
{
	char temp[LINE_SIZE];
	do
	{
		printf("Enter value: ");
		scanf("%s", &temp);
	} while (!isdigit(temp[0]));
	regis.AX = atoi(temp);
}

	/****************************   printMemoryDump   ********************************
	prints memory by number
	MAX is the amount of elements in the memory array (Vicki used 100)
	COL is the number of columns that are to be displayed (Vicki used 7; was originally called COLUMNS)
	---------------------------------------------------------------------------------*/
void printMemoryDump(int dumpType)
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;					//the current location being called
	for ( int row = 0; row < numRows; row++ )
	{
		location = row;
			for ( int column = 0; location < MAX&&column < COL; column++ )
		{
			if ( !(numRows - 1 == row&&carryOver - 1 < column) )
			{
				if (dumpType == 1)
				{
					printf("%6d.%6d", location, memory[location]);
				}
				else
				{
					printf("%6x.%6x", location, memory[location]);
				}
				
				location += (numRows - (carryOver - 1 < column));
			}
		}
		printf( "\n" );
	}

	printf( "\n" );
	printf( "AX:%d\tBX:%d\tCX:%d\tDX:%d\t", regis.AX,regis.BX,regis.CX,regis.DX );
	printf( "\n\n" );
	printf( "Instruction: %d\n", address );
	printf( "Flag: %d", regis.flag );

	printf( "\n\n" );
}

	//**************   Helper functions   *****************************************

	/*********************   changeToLowerCase   ********************
	Changes each character to lower case
	* line - the string that was entered the line is completely changed to lower case
	----------------------------------------------------------------*/
void changeToLowerCase( char line[ ] )
{
	for (int i = 0; i < strlen(line); i++)
	{
		line[i] = tolower(line[i]);
	}
}

	//Returns value from given register based on its ID
int getValue(int param) 
{
	switch (param)
	{
	case AXREG:
		return regis.AX;
	case BXREG:
		return regis.BX;
	case CXREG:
		return regis.CX;
	case DXREG:
		return regis.DX;
	case BXADR:
		return memory[regis.BX];
	case BXADRPLUS:
		param = memory[regis.BX + memory[address]];
		address++;
		break;
	case MEMORYLOC:
		param = memory[memory[address]];			// Fetch the address, & the value
		address++;									// Increment address
		break;
	case CONSTANT:
		param = memory[address];					// Fetch the constant
		address++;									// Increment address
		break;
	}

	return param;
}

	//Takes registerID & a value. Puts value into given register based on its ID.
void putValue( int valType, int value) 
{
	switch (valType)
	{
	case AXREG:
		regis.AX = value;
		break;
	case BXREG:
		regis.BX = value;
		break;
	case CXREG:
		regis.CX = value;
		break;
	case DXREG:
		regis.DX = value;
		break;
	case BXADR:
		memory[regis.BX] = value;
		break;
	case BXADRPLUS:
		memory[regis.BX + memory[address]] = value;
		address++;
		break;
	case MEMORYLOC:
		memory[memory[address]] = value;			// Fetch the address, & the value
		address++;									// Increment address
		break;
	case CONSTANT:
		internalError = INVALID_CNSTANT;
		break;
	}
}

	//searches a line for a word.
void getFromLine(char line[], char result[], char search, char secondarySearch, int * index) 
{
	int resultIndex = 0;
				// go through the line string until you find one of the stopper chars
	while (!(line[*index] == search ||	line[*index] == secondarySearch || line[*index] == '\n' || line[*index] == '\0'))
	{
		result[resultIndex] = line[*index];  // start filling the result string
		(*index)++;			// iterate the...
		resultIndex++;		// two indicies.
	}
	result[resultIndex] = '\0';	// add the string stopper
}

Memory popStack() 
{
	stackPointer++;
	int temp = memory[stackPointer];
	return temp;
}

void pushStack(Memory value)
{
	memory[stackPointer] = value;
	stackPointer = stackPointer - 1;
}

			//Auxilliary Functions
Memory displayType[MAX];			// To keep track of the type of each memory address for printMemoryDumpReadable()
bool displayTypeFilled = false;

//prints a more readable version of the memory dump
void printMemoryDumpReadable()
{
	int numRows = MAX / COL + 1;		//number of rows that will print
	int carryOver = MAX % COL;			//number of columns on the bottom row
	int location;						//the current location being called
	char buffer[MAX][LINE_SIZE];		// Buffer so we can go through the instructions in order
	char name[LINE_SIZE];				// Name of a byte

	if (!displayTypeFilled)
	{
		for (int i = 0; i < MAX; i++)		// Initialize DisplayType (only once)
		{
			displayType[i] = 0;							// Set their display type to unknown.
		}
		displayTypeFilled = true;
	}
	for (int i = 0; i < MAX; i++)		// Go through all the bytes again
	{
		getInstructionName(i, name, displayType);	// Figure out what label to use for them
		strcpy(buffer[i], name);					// Add that label to the buffer
	}

	for (int row = 0; row < numRows; row++)
	{
		location = row;

		for (int column = 0; location < MAX && column < COL; column++)
		{
			if (!(numRows - 1 == row && carryOver - 1 < column))
			{
				if (location == address)
				{
					printf("\t)> %s", buffer[location]);
				}
				else
				{
					printf("\t%3d. %s", location, buffer[location]);
				}
				location += (numRows - (carryOver - 1 < column));
			}
		}
		printf("\n");
	}

	printf("\n");
	printf("AX:%d\tBX:%d\tCX:%d\tDX:%d\tFlag: %2d\t", regis.AX, regis.BX, regis.CX, regis.DX,regis.flag);
	printf("Instruction pointer: %3d\tStack Pointer: %3d\t\n\n", address, stackPointer);
	printf("Latest output: %3d\t", latestOutput);
	printf("Error Message: %s\t\n", errorMessages[internalError]);

	printf("\n\n");
}

//Takes a memory location & a string. Fills string w/ readable name of the instruction/value at that location
void getInstructionName(Memory location, char name[], Memory displayType[])
{
		// Since we never display a register or BXADR, we can use the first 4 combinations for other labels.
	#define Unknown 0		// The default value.
	#define Instruction 1	// For instructions
	#define Variable 2		// For memory locations where the program stores something.
	#define jumpTo 3		// For memory locations the program jumps to.
	#define funParam 4		// For any function related numbers

	// Display names associated w/ the tags above.
	const char displayTypeNames[8][4] = { "UKN","INS","VAR","JTO","FPM","BX+","ADR","CON" };
	// Display names for inline values. Non-registers just have 'x', since the data type will be shown in the next byte
	const char displayRegister[8] = { 'A','B','C','D','p','x','x','x' };

		// The functions are broken into four categories which can use the same logic
	#define inlineCommandRange 4	// The commands with parameters in their binary
	#define simpleCommandRange 8	// The commands with no parameters at all
	#define jumperCommandRange 15	// The JMP commands
	#define functionCommandLoc 15	// The function command

	// The display names & associated numerical values of each command.
	const int funValues[16] = { MOVREG,MOVMEM,ADD,CMP,PUT,GET,RET,HALT,JMP,JA,JB,JE,JNE,JBE,JAE,FUN };
	const char funNames[16][4] = { "MVR","MVM","ADD","CMP","PUT","GET","RET","HLT","JMP","JA ","JB ","JE ",
								   "JNE","JBE","JAE","FUN" };

	strcpy(name, "       ");		// clear garbage
	char temp[2];					// for storing register letters.  Not a string, just an array of characters.

	//Get command info.
	Memory command = memory[location];		// Pull byte from memory
	Memory instruction = command & 224;		// 11100000
	Memory param1 = (command & 24) >> 3;	// 00011000
	Memory param2 = command & 7;			// 00000111
	
		// We need special cases for variables addressed by BXADR, because we can only know where they are at the moment
		// The program uses them. So... if the program's current instruction is using BXADR outside of a function...
	if ((memory[address] & 7) == BXADR && displayType[regis.BX] != funParam)
	{
		if ((memory[address] & 224) != 0)	// Check to exclude functions w/o parameters
		{
			displayType[regis.BX] = Variable;	// Set the address of BX as a variable.
		}
	}
	else if ((memory[address] & 7) == BXADRPLUS && displayType[regis.BX + memory[address + 1]] != funParam)
	{
		if ((memory[address] & 224) != 0)
		{
			displayType[regis.BX + memory[address + 1]] = Variable;
		}
	}

	// Code for labeling non-instructions
	if (location > stackPointer)	// if it's the stack...
	{
		sprintf(name, "STK %3d", command);	// Use tag STacK
		return;
	}
	else if (displayType[location] > Instruction)	// if it's any other non-instruction
	{
		sprintf(name, "%s %3d", displayTypeNames[displayType[location]], command); // display the name based on the table
		return;
	}

	// Code for labeling instructions
	displayType[location] = Instruction; // Set displayType to Instruction

	// Get the characters to display for inline values
	temp[0] = displayRegister[param1];
	temp[1] = displayRegister[param2];

	// Search table to get instruction index ID.
	// Special search to exclude inline parameters.
	int index = 0;
	while (funValues[index] != instruction && index < inlineCommandRange)
	{
		index++;
	}
	// Normal search for all other instructions
	while (funValues[index] != memory[location] && index >= inlineCommandRange && index <= jumperCommandRange)
	{
		index++;
	}

	if (index < inlineCommandRange)									//For instructions with inline values
	{
		sprintf(name, "%s  %c%c", funNames[index], temp[0], temp[1]);	// fill name string with the name & inline values

		if (param2 > BXADR && location + 1 < MAX && location >=0)		// If the instruction has a second byte
		{
			displayType[location + 1] = param2;							// Mark that byte as whatever datatype it is
			if (param2 == MEMORYLOC)									// If that datatype is an address
			{
				if (memory[location + 1] < MAX && memory[location + 1] >= 0) {
					displayType[memory[location + 1]] = Variable;			// Mark that address as a variable
				}

			}
		}
	}
	else if (index < simpleCommandRange)							//For instructions with nothing special
	{
		sprintf(name, "%s   ", funNames[index]);						// Just fill in their name
	}
	else if (index < jumperCommandRange)							// For JMP instructions
	{
		sprintf(name, "%s   ", funNames[index]);						// Fill in the instruction name

		if (location + 1 < MAX)											// Check if we've got space
		{
			displayType[location + 1] = jumpTo;							// If we do, mark the next byte as a jumpTo
		}
	}
	else if (index == functionCommandLoc && location + 3 < MAX)		// For FUN instructions
	{
		strcpy(name, "FUN    ");										// Fill in the instruciton name
		int paramLength = memory[location + 2] + 4;						// Get the number of parameters
		displayType[memory[location + 1] - 1] = funParam;				// Mark the byte before the function code as a funParam

		for (int i = 1; i < paramLength; i++)							// Loop through the function's parameters
		{
			if (location + i < MAX)										// If they're not past the MAX value
			{
				displayType[location + i] = funParam;					// Mark them as funParam
			}
		}
	}
	else									// For anything else
	{
		sprintf(name, "  %5d", command);								// We have no idea what it is, so mark as undefined.
	}
}

void debugUI()
{
	if (dumpType == 0)	
	{
		printMemoryDumpReadable();	//Dump memory
	}
	else
	{
		printMemoryDump(dumpType);
	}
	
	system("pause");
}

// Returns the datatype of a string
Memory getType(char* param)
{
	switch (param[0])	// Check if its a register (for speed. Not strictly necessary.)
	{
	case 'a':
		return AXREG;
	case 'b':
		return BXREG;
	case 'c':
		return CXREG;
	case 'd':
		return DXREG;
	}
		
	// Data types. Put in order so that their index in this list is equal to their binray identifier
	const char dataTypeKey[7][LINE_SIZE] = { "ax\0","bx\0","cx\0","dx\0","[bx]\0","[bx+\0","[\0" };

	Memory numType = 3; // for the type of value. Init to 3 since we know it's not a register.
	numType = findString(param, dataTypeKey, 7); // Finding the datatype

	switch (numType)
	{
	case -1:	// If we couldn't find it
		if (isdigit(param[0]) || param[0] == '-')	//Check if it's a number
		{
			return CONSTANT;						// If it is, return constant
		}
		return numType;								// If not, return -1
	default:	// If we did find it
		return numType;		// just return numType
	}
}
	// Removes comments & oversized lines from the input file & fills "line" with clean ASM. Returns true until file is empty.
bool getLineWithoutTrash(FILE *inputFile,char line[LINE_SIZE])
{

	if (feof(inputFile))
	{
		return false;	// No line, return false
	}
	
	fgets(line, LINE_SIZE, inputFile);
	
	// If the line is not trash (oversized line / comment)
	if ((feof(inputFile) || line[strlen(line) - 1] == '\n') && line[0] != ';')
	{
		return true;				// Return true, line is good
	}

	// If it's an oversized line (trash), we need to fgets() our way through it one LINE_SIZE at a time.
	while (line[strlen(line) - 1] != '\n')
	{
		fgets(line, LINE_SIZE, inputFile);
	}
	
	// Hopefully the next line won't be trash!
	return getLineWithoutTrash(inputFile, line);
}

/*
Problems 3: 
Problems: in the RunMachineCode Function, the second parameter was 8 times what it should be, 
because I never shifted it to the left. I fixed this by dividing it by 8, putting it back into 
the ones place.

Problems 3-2: 
Lots of issues with returning from the whichReg() function when it's fed something 
other than a register. At first it returned -1 to indicate an error. This resulted in a 159 
being put into the command buffer. Then I had it return 5, for a constant. Then I realiszed 
the add function doesn't need that, so I had it return nothing. Then I had undefined behaviour, 
so now it returns zero.

Problems 3-3: 
Lots of different issues with putting a value at a particular memory address.

Problem 4-1: 
I spent like an hour and a half trying to get MOV reg address to work, because I had
a single unnecessary "address++" after accessing the memory location with the value in it. This
wasn't part of the runtime, so it didn't need the address++. And because it just skipped
whatever line was in front of it, it's behevaiour was super unpredictible. At one point while testing 
it, I removed everything from the .asm file except the MOV REG ADDRESS and the HALT to make the testing
faster. At that point, I started getting exceptions! I was so confused as to how that happened. I thought 
somehow I had messed up one of the functions, so I'm going back and fourth, tripple checking the 
runMachineCode() function and the convertToMachineCode() like- what is wrong with these?? Of course, 
nothing was wrong with either of them. The problem was that it was skipping the HALT command and 
overruning forever. So I added a warning for overruns.

5-1: 
I had an issue with negative numbers, where paramDecode was comparing "param2 != '-'" instead of
"param2[0] != '-'". That meant it could not deal with negative constants.

5-2: 
I actually had no issues with my code, but I did struggle with my .asm file. I kept mixing up the 
JA/JB and JAE/JBE instructions, so something would be equal but not greater and it would (or would't)
jump when I expected it to do the opposite. I eventually made a new memorydump function that prints 
the memory in a way which is much more readible, and then I went step-by-step through my .asm program 
in order to debug it. 

6-0 (Correcting mistakes in 5):
The reason my asm had an infinite loop in your compiler was because it didn't have brackets around the jumps.

6-1:
I had no issues other than minor syntax stuff.

7-0:
Off by one error: Are you supposed to run the code at the point you jump to? 

Problems 7: There were really a lot of small problems. I'm just going to list a few.

7-1:
I  had the RET function pulling AX from the  stack before it put AX into the return location. That one was 
pretty easy, since I knew the problem was going to be in the runRET function, and I'd already had other 
issues with the order of the stack. 

7-2:
I had an issue with the way [BX+xxx] was implemented, because I'd initially assumed we were supposed to put spaces
on either side of the plus [BX + xxx]. That broke splitCommand, and almost forced me to either redesign the entire 
assembler, or create some truly horrible bodge. I remember the sinking feeling of dread as I tried to come up with
some solution that wouldn't be horrible. that's when I had the thought, "we don't use spaces in anything." I think
it's really interesting how such a small assumption can run incredibly deep throughout piece of computer software.

7-3:
I had so many issues with my memoryDumpReadable() function, and it still totally doesn't work. I know I shouldn't be focusing
on it, but I find it to be a lot of fun to work on, and I like it when it works. With that said, in heindsight, it
might have actually been better before. Now it relies on some complex patchwork of overlapping state that's nearly
impossible for me to get my head around. It's almost like I couldn't have designed it worse if I tried. Like the functions
set their parameters which set the addresses they point to? This means that a single value being somewhere the function
doesn't expect can lead to strange and unpredictible casade failures, causing it to read half your program
as if it's a signle function with a hundred parameters. That said, it has no impact on the running of the code, so
it doesn't really matter if it goes totally whack. And it's correct enough that it's still very helpful for me.

8-0:
I finally made the memoryDumpReadable() function good, and changed the way the compiler works.

8-1:
Most of my issues were just with the addresses pointed to by the program. Although I also had to change the register
used to count the number of numbers entered from bx to dx so I could access memory within the loop. Also I accidentally
tried to do a "mov [100] 0", which of course is not possible.

9-1:
I had a bug where there would be an infinite loop if there wasn't an endline at the end of the file, because the getLineWithoutTrash
function would just keep looking for endlines over and over again. Fixed that, and also made that function finally work.

10-1:
This one is fun: I wasn't initializing the param1 & param2 variables. This was fine with most ASM programs, which open with some sort of mov
instruction. But my linked list program opens with a get, so it crashed when it tried to do a getType() on an uninitialized variable. So
I just initialize them to whatever and that works fine.
*/