#ifndef SIMULATOR_C
#define SIMULATOR_C

//runMachineCode: Executes the machine code in memory.
void runMachineCode();

//getValue: takes a register (int) and returns that register's value
	// INPUT: registerID - the integer ID of a register
	// OUTPUT: return - the value stored in the selected register
int getValue(int registerID);

//putValue: takes a register ID and a value and puts that value into the corresponding register.
	// INPUT: registerID - the ID of a register you want to change; value - the value you want to put into the register;
void putValue(int registerID, int value);

//popStack: pops a value from the stack
	// OUTPUT: Memory - the value popped off the stack
Memory popStack();

//pushStack: pushes a value onto the stack
	// INPUT: value - the value to be pushed onto the stack
void pushStack(Memory value);

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

// getType: returns the type of a parameter
	// INPUT: param - a string containing a parameter
	// OUTPUT: Memory - the integer ID of the type of the parameter (0-7)
Memory getType(char* param);

//								runMachineCode()
//				Executes the machine code, the virtual machine

void runMachineCode()
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


//**************   Helper functions   *****************************************

/*********************   changeToLowerCase   ********************
Changes each character to lower case
* line - the string that was entered the line is completely changed to lower case
----------------------------------------------------------------*/
void changeToLowerCase(char line[])
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
void putValue(int valType, int value)
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
void getFromLine(char line[], char result[], char search, char secondarySearch, int* index)
{
	int resultIndex = 0;
	// go through the line string until you find one of the stopper chars
	while (!(line[*index] == search || line[*index] == secondarySearch || line[*index] == '\n' || line[*index] == '\0'))
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

#endif