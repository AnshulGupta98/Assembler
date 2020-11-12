#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXCHARPERLINE 80
#define A_COMMAND 0
#define C_COMMAND 1
#define L_COMMAND 2
#define L_AND_A_COMMAND 3 //This  is addition to Pg 113 Label + A in same line
#define L_AND_C_COMMAND 4 //This is addition to Pg 113 Label + C in the same line
#define COMMANDERROR 5

FILE* initializeParser(char *filename){
    FILE *fp;
    if( ( fp = fopen( filename, "r" ) ) == NULL){
        fprintf(stderr, "Opening File %s failed\n",filename );
        exit(1);
    }
    return fp;
}


int commandType(char *parsedCommand) {

    int i;

    if (parsedCommand[0] == '@') 
        return A_COMMAND;
    if (parsedCommand[0] != '(') 
        return C_COMMAND;
    if (parsedCommand[0] == '(') {
	    i = 0;
	    while ((parsedCommand[i] != '\n') && parsedCommand[i] != ')')
	        i++;
	    if (parsedCommand[i] == ')')
		{
			if (parsedCommand[i+1] == '\n') 
			    return L_COMMAND;
			if (parsedCommand[i+1] == '@') 
			    return L_AND_A_COMMAND;
			else 
			    return L_AND_C_COMMAND;
		}
	}
    return COMMANDERROR;
}

// The Interfile is written in PASS 1
FILE* initializeInter (char *filename) {
	int i = 0;
	char tempFile[MAXCHARPERLINE];
	strcpy(tempFile,filename);
	while(tempFile[i] != '.') 
	    i++;

    tempFile[i+1] = 'i';
	tempFile[i+2] = 'n';
	tempFile[i+3] = 't';
	tempFile[i+4] = '\0';

	return (fopen(tempFile,"w"));
}

/********* Beginning of The Final stage (The main()) function */

//The main() function takes the name of input assembly file as command line

void main(int argc, char **argv) {


	//PASS 1
	FILE *inputFile;

	//PASS2-Start
    FILE *interFile;
	//PASS2-END


	int i,j,k; //Temporary variables

	int instructionNumber = 0; //This keeps the count of instructions
	int lineNumber = 0;	//This keeps the count of number of lines read

	//The nextCommand string will store the command to be processed
	// This is called a buffer
	char nextCommand[MAXCHARPERLINE];

	//This parsedCommand string is nextCommand without whitespaces
	char parsedCommand[MAXCHARPERLINE];

	// Temporary strings for holding symbols
	char symbol1[MAXCHARPERLINE];
	char symbol2[MAXCHARPERLINE];


	//Opening the input and output files
	inputFile = initializeParser(argv[1]);
	interFile = initializeInter(argv[1]);

	//Reading the input file line after line till End of file
	//fgets() is used for reading. This implements the steps
	// hasMoreCommands and advance steps in pg. 113 of the book

	while(fgets(nextCommand,MAXCHARPERLINE,inputFile) != NULL) {

	//Inside this loop we parse the command got through nextCommand
	//This is PASS 1 of the assembler and used for only Symbol
	//Resolution.

		i = 0;
		j = 0;
		parsedCommand[0] = '\0';  //Initializing parsed command

		while(nextCommand[i] != '\n') {

		    //Remove all whitespaces
		    if ((nextCommand[i] != ' ') && (nextCommand[i] != '\t'))
				parsedCommand[j++] = nextCommand[i];
			i++;
		}
		parsedCommand[j] = '\n';	//Add the end of the line
		parsedCommand[j+1] = '\0';	//Add the end of string


		//Remove comments if any inside a line

		i = 0;
		while (parsedCommand[i] != '\n') {
		    if ((parsedCommand[i] == '/') && (parsedCommand[i+1] == '/')) {
			    parsedCommand[i] = '\n';
			    parsedCommand[i+1] = '\0';
				break;
		    }
		    i++;
		}

		//Find the commandType and extract symbols

		if (parsedCommand[0] != '\n') //This is an instruction
		{
		    i = commandType(parsedCommand);
	        if ((i == L_COMMAND) || (i == L_AND_A_COMMAND) || (i == L_AND_C_COMMAND))
			{
			    j = 0;
			    if ((parsedCommand[1] < '0') || (parsedCommand[1] > '9'))
		        {
	                while((parsedCommand[j+1] != ')')/* && (parsedCommand[j+1] !='\n')*/) {
						//Extracts the first symbol in case of L_, L_AND_A and L_AND_C
			            symbol1[j] = parsedCommand[j+1];
				        j++;
			        }
			    }
			    symbol1[j] = '\0';


			    //Extract Symbol2 in case of L_AND_A instruction

			    if (i == L_AND_A_COMMAND) {

			        // Increment j by 1 to reach the ")" character in parsedCommand
			        // Increment j by 1 more to reach the "@" character in parsedCommand
			        // Increment j by 1 more to reach the first character of A_Instruction in parsedCommand
			        j = j + 3;

			        if ((parsedCommand[j] < '0') || (parsedCommand[j] > '9'))
			        {   
			            k = 0;
			            while(parsedCommand[j] != '\n')
                        {	//Extracts the second symbol in case of L_AND_A
			                symbol2[k] = parsedCommand[j];
				            j++;
				            k++;
			            }
	                    symbol2[k] = '\0';
		            }
			    }
		    }

	        //Handle Symbols in A_COMMAND
	        if (i == A_COMMAND)
	        {
		        j = 0;
		        if ((parsedCommand[1] < '0') || (parsedCommand[1] > '9'))
			    {
				    while(/*(parsedCommand[j+1] != ')') && */(parsedCommand[j+1] != '\n')) {
					    //Extracts the first symbol in case of L_, L_AND_A and L_AND_C		This is a wrong comment
					    //Extracts the symbol in case of A_COMMAND          This is the correct comment
			            symbol1[j] = parsedCommand[j+1];
					    j++;
				    }
		            symbol1[j] = '\0';
			    }
		    }

		    if (i == COMMANDERROR) { //Diagnostics
		        printf("Error in Line number %d\n",lineNumber);
			    exit(-1);
		    }
		    if (i != L_COMMAND) {
			   if(i != L_AND_A_COMMAND && i !=  L_AND_C_COMMAND)
				{
					  fprintf(interFile,"%s",parsedCommand);
				}
	        	else {
			    	k = 0;
		        	while(parsedCommand[k] != ')') 
		            	k++;
		        	k++;
			    	while(parsedCommand[k] != '\n') {
						fprintf(interFile,"%c",parsedCommand[k]);
						k++;
					}
					fprintf(interFile,"\n");
				}
				instructionNumber++;
			}
	    }
	    lineNumber++;
	}

	//Closing the input and output files
	fclose(inputFile);
	fclose(interFile);
}