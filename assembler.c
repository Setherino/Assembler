#ifndef ASSEMBLER_C
#define ASSEMBLER_C

// assembler: Converts the entire ASM file and stores it in memory
//EMSCRIPTEN_KEEPALIVE
void assembler();

//getFromLine: Seperates a word from a line of text.
	// INPUT: line[] - the line to search; search & secondarySearch - the characters to search for; index - the location to start
	// OUTPUT: result - the first word found.
void getFromLine(char line[], char result[], char search, char secondarySearch, int* index);

//changeToLowerCase: replaces all capital letters in a string with lowercase letters.
	// INPUT: line[ ] - the string to turn lowercase
	// OUTPUT: line[ ] - the string made lowercase
void changeToLowerCase(char line[]);

// labelConvert: Converts varLength long labels in the ASM file to actual memory locations, fills fileBuffer with converted ASM file, returns fileBuffer length
int labelConvert(sfile* labeled, char fileBuffer[MAX][LINE_SIZE]);

//ConvertToMachineCode: Converts a single line of ASM to machine code
	// INPUT: char line[ ] - a line of ASM to convert
void convertToMachineCode(char line[]);

//paramDecode: Fills the last three bits of a command & puts it into memory
	// INPUT: param2 - the second parameter of a line of ASM; tempCommand - the first five bits of tempCommand; inlineParams - whether or not you want inline params
	// OUTPUT: stores tempCommand in memory
void paramDecode(char* param2, Memory tempCommand, bool inlineParams);

//splitCommand: breaks one line of ASM into its constituent parts.
	// INPUT: line[] - one line of asm; 
	// OUTPUT: command[] - the instruction; param1[], param2[] -  the two parameters;
void splitCommand(char line[], char command[], char param1[], char param2[]);

// Compiles the FUN command
	//INPUT: a full line of asm
void fillFun(char line[]);

//getLineWithoutTrash: gets one line of ASM, and removes the trash if it needs to.
bool getLineWithoutTrash(sfile* inputFile, char line[]);

void removeLabelsFromFile(char fileName[LINE_SIZE]);

//										Assembler: 
//		Changes the assembly code to machine code and places the commands into the memory.
void assembler()
{
	address = 0;
	sfile* fin;			  // File pointer for reading in the assembly code.

	sopen(&fin, ASM_FILE_NAME, "r");
	if (fin == NULL)
	{
		printf("Error, file didn't open\n\nExiting program...\n\n");
		system("pause");
		exit(1);
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

	for (int i = 0; i < fileSize; i++)
	{
		char line[LINE_SIZE];
		strcpy(line, fileBuffer[i]);
		convertToMachineCode(line);
	}
}

int labelConvert(sfile* labeled, char fileBuffer[MAX][LINE_SIZE])
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
	sclose(labeled);


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
	initCommandBW = controlWord & 8;	// 001000 - Puts the command & param2 in the first 5 bits of the instruction
	doParamDecode = controlWord & 4;	// 000100 - Identifies & sends param2 to the final 3 bits of the instruction
	paramDecodeBW = controlWord & 2;	// 000010 - Identifies & sends param1 to the final 3 bits of the instruction
	doTheFunction = controlWord & 1;	// 000001 - Does FUN command FUNction. Fills every 8 bits of the instruction

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
void fillFun(char line[])
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

// Removes comments & oversized lines from the input file & fills "line" with clean ASM. Returns true until file is empty.
bool getLineWithoutTrash(sfile* inputFile, char line[LINE_SIZE])
{

	if (seof(inputFile))
	{
		return false;	// No line, return false
	}

	sgets(line, LINE_SIZE, inputFile);

	// If the line is not trash (oversized line / comment)
	if ((seof(inputFile) || line[strlen(line) - 1] == '\n') && line[0] != ';')
	{
		return true;				// Return true, line is good
	}

	// If it's an oversized line (trash), we need to sgets() our way through it one LINE_SIZE at a time.
	while (line[strlen(line) - 1] != '\n')
	{
		sgets(line, LINE_SIZE, inputFile);
	}

	// Hopefully the next line won't be trash!
	return getLineWithoutTrash(inputFile, line);
}

void removeLabelsFromFile(char fileName[LINE_SIZE])
{
	char fileBuffer[MAX][LINE_SIZE];
	sfile* fin;
	sfile* fout;
	sopen(&fin, ASM_FILE_NAME, "r");
	if (fin == NULL)
	{
		printf("Error, file didn't open\n\nExiting program...\n\n");
		system("pause");
		exit(1);
	}

	int fileSize = labelConvert(fin, fileBuffer);
	sclose(fin);

	sopen(&fout, fileName, "w");
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
			sfprintf(fout, "%s\n", stringBuffer);
		}
		else
		{
			sfprintf(fout, "%s", stringBuffer);
		}
	}
	sclose(fout);

	printf("\n\n\tLabels removed & new file saved.\n\n");
}

#endif