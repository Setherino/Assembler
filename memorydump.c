#ifndef MEMORYDUMP_C
#define MEMORYDUMP_C

//printMemoryDump: Prints the memory of the simulated computer.
// dumpType: 0 = Readable, 1 = Hex, 2 = Decimal
//void printMemoryDump(int dumpType);

//printMemoryDumpReadable: Prints memory with instruction names & labels for values.
//void printMemoryDumpReadable();

//getInstructionName: Gets the name of an instruction from its numerican value.
	// INPUT: location - the location of an instruction;
	// OUTPUT: name - this is where it puts the name of the command
void getInstructionName(Memory location, char name[], Memory displayType[]);


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
	printf("AX:%d\tBX:%d\tCX:%d\tDX:%d\tFlag: %2d\t", regis.AX, regis.BX, regis.CX, regis.DX, regis.flag);
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
	Memory instruction = command & 224;		// 11100000 For instructions with inline parameters, the first 3 bytes store which command it is
	Memory param1 = (command & 24) >> 3;	// 00011000 The next two store the first parameter
	Memory param2 = command & 7;			// 00000111 The last three store the second parameter

	// We need special cases for variables addressed by BXADR, because we can only know where they are at the moment
	// The program uses them. So... if the program's current instruction is using BXADR outside of a function...
	if ((memory[address] & 7) == BXADR && displayType[regis.BX] != funParam)
	{
		if ((memory[address] & 224) != 0)	// Check to exclude functions w/o parameters
		{
			displayType[regis.BX] = Variable;	// Set the displayType at the address of BX as a variable.
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
	// If we got to this point, we should be dealing with an instruction, so:
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

		if (param2 > BXADR && location + 1 < MAX && location >= 0)		// If the instruction has a second byte
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
	for (int row = 0; row < numRows; row++)
	{
		location = row;
		for (int column = 0; location < MAX && column < COL; column++)
		{
			if (!(numRows - 1 == row && carryOver - 1 < column))
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
		printf("\n");
	}

	printf("\n");
	printf("AX:%d\tBX:%d\tCX:%d\tDX:%d\t", regis.AX, regis.BX, regis.CX, regis.DX);
	printf("\n\n");
	printf("Instruction: %d\n", address);
	printf("Flag: %d", regis.flag);

	printf("\n\n");
}

/*

If only we could create some kind of "memory dump" object...

*/

#endif