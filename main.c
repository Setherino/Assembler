// Seth Ciancio 10/24/25
// Assembler: WebAssembly
// A program to simulate the operation of an x86 CPU, modified to run on the web!

//need to create the functions run the array notation. 
#define _CRT_SECURE_NO_WARNINGS  // lets us use depricated code

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
//#include <emscripten.h>
#include "defines_config.h"
#include "files.h"		// Actually quite important!
#include "memorydump.c"
#include "simulator.c" // Relies on code from ^
#include "assembler.c" // Relies on code from ^

	//optionsMenu: presents the user with an options menu to choose what program functionality they want to use
		// INPUT: printMemoryAtEnd - whether or not to do a memory dump at the end of the program
		// OUTPUT: printMemoryAtEnd - same thing as input, but might be changed by the user
//EMSCRIPTEN_KEEPALIVE
bool optionsMenu(bool printMemoryAtEnd);

	//loadPreferences: loads settings (filename, debug, dump type, and printMemoryAtEnd) from preferencesFile
		// OUTPUT: printMemoryAtEnd
//EMSCRIPTEN_KEEPALIVE
bool loadPreferences();

	// customSettings: enables the user to change their settings (filename, debug, dump type, and printMemoryAtEnd)
	// OUTPUT: printMemoryAtEnd
//EMSCRIPTEN_KEEPALIVE
bool customSettings();

/*

Take note of what happens in this file, and repeat the same order of operations in your HTML/JS version
BUT DO NOT print debugs to the console. KEEP DEBUG INFORMATION SEPERATE.

You may have the user interact with the 

*/

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

		printf("Running machine code...\n\n");

		runMachineCode();
		
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
	sfile* fin;
	bool printMemoryAtEnd = false;

	sopen(&fin, preferencesFile, "r");
	if (fin == NULL)
	{
		printf("\tNo %s file found. Using default preferences.\n\n", preferencesFile);
		printMemoryAtEnd = false;
		debug = true;
	}
	else
	{
		char temp[LINE_SIZE];
		sgets(ASM_FILE_NAME, LINE_SIZE, fin);
		int index = 0;
		getFromLine(ASM_FILE_NAME, ASM_FILE_NAME, ' ', ' ', &index);
		sgets(temp, LINE_SIZE, fin);
		debug = atoi(temp) & 1;
		printMemoryAtEnd = atoi(temp) & 2;
		sgets(temp, LINE_SIZE, fin);
		dumpType = atoi(temp);
		sclose(fin);
		printf("Loaded preferences from file.\n\n");
	}

	return printMemoryAtEnd;
}


bool optionsMenu(bool printMemoryAtEnd)
{
	sfile* fout;
	
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
		sopen(&fout, preferencesFile, "w");
		sfprintf(fout,"%s\n", ASM_FILE_NAME);
		sfprintf(fout, "%d%d\n", printMemoryAtEnd, debug);
		sfprintf(fout, "%d\n", dumpType);
		sclose(fout);
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