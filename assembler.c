/***********************************
 * Author: Anshul Gupta            *
 * Date Created: 29 Oct, 2020      *
 * Date last updated: 29 Oct,2020  *
***********************************/

// This is the HACK Assembly that shall convert
// HACK Assembly program in Mnemonics to 16-bit Binary
// HACK Machine Language program

//This is done as project 06 defined in
// The Elements of Computing Book by Noam Nisan and Shimon Schoken

#include <stdio.h>
#include <string.h>

// We shall define first an entry in the symbol table.

#define MAXLENGTH 20 //This is maximum length of any symbol or variable name
#define MAXENTRIES 1024 //Maximum number of enteries in Symbol Table

//This is a single entry in Symbol table

struct SymbolTableEntry {
	char symbol[MAXLENGTH];
	int address;
};

//Since the SymbolTable is going to be used by many functions
// we define it as a global variable as an array of
// MAXENTERIES of SymbolTableEntry. The following declaration
// "CONSTRUCTS" the SymbolTable

struct SymbolTableEntry SymbolTable[MAXENTRIES];

//The following function initializes the Symbol Table
// This fills up the predefined symbols as given in the table
// in Section 6.2.3 (pg. 110) of the book.

void InitializeSymbolTable() {
	strcpy(SymbolTable[0].symbol,"SP");
	strcpy(SymbolTable[1].symbol,"LCL");
	strcpy(SymbolTable[2].symbol,"ARG");
	strcpy(SymbolTable[3].symbol,"THIS");
	strcpy(SymbolTable[4].symbol,"THAT");
	strcpy(SymbolTable[5].symbol,"R0");
	strcpy(SymbolTable[6].symbol,"R1");
	strcpy(SymbolTable[7].symbol,"R2");
	strcpy(SymbolTable[8].symbol,"R3");
	strcpy(SymbolTable[9].symbol,"R4");
	strcpy(SymbolTable[10].symbol,"R5");
	strcpy(SymbolTable[11].symbol,"R6");
	strcpy(SymbolTable[12].symbol,"R7");
	strcpy(SymbolTable[13].symbol,"R8");
	strcpy(SymbolTable[14].symbol,"R9");
	strcpy(SymbolTable[15].symbol,"R10");
	strcpy(SymbolTable[16].symbol,"R11");
	strcpy(SymbolTable[17].symbol,"R12");
	strcpy(SymbolTable[18].symbol,"R13");
	strcpy(SymbolTable[19].symbol,"R14");
	strcpy(SymbolTable[20].symbol,"R15");
	strcpy(SymbolTable[21].symbol,"SCREEN");
	strcpy(SymbolTable[22].symbol,"KBD");

   SymbolTable[0].address = 0;
   SymbolTable[1].address = 1;
   SymbolTable[2].address = 2;
   SymbolTable[3].address = 3;
   SymbolTable[4].address = 4;
   SymbolTable[5].address = 0;
   SymbolTable[6].address = 1;
   SymbolTable[7].address = 2;
   SymbolTable[8].address = 3;
   SymbolTable[9].address = 4;
   SymbolTable[10].address = 5;
   SymbolTable[11].address = 6;
   SymbolTable[12].address = 7;
   SymbolTable[13].address = 8;
   SymbolTable[14].address = 9;
   SymbolTable[15].address = 10;
   SymbolTable[16].address = 11;
   SymbolTable[17].address = 12;
   SymbolTable[18].address = 13;
   SymbolTable[19].address = 14;
   SymbolTable[20].address = 15;
   SymbolTable[21].address = 16384;
   SymbolTable[22].address = 24576;

   // Initialize all other enteries to NULL symbol and -1 as address
   for (int i = 23; i< MAXENTRIES; i++){
	   strcpy(SymbolTable[i].symbol,"\0");
	   SymbolTable[i].address = -1;
	}
}

//The following function adds an entry into the SymbolTable
void addEntry(struct SymbolTableEntry item) {
  //Check if Entry already exists
  int i = 0;
  //Keep checking till the Entry is either there or we have scanned all the Enteries entered so far

  while( (strcmp(SymbolTable[i].symbol,item.symbol) != 0) && (strcmp(SymbolTable[i].symbol,"\0") !=0) && (i < MAXENTRIES))
    i++;

  //The symbol is not contained in the Symbol Table
  // We add it.

  if(strcmp(SymbolTable[i].symbol,"\0") == 0) {
	strcpy(SymbolTable[i].symbol,item.symbol);
	SymbolTable[i].address = item.address;
  }

  //The symbol is contained in Symbol Table but address is -1
  // This is the case that arises when we have processed @END much before
  // and currently processing (END), we need to update the address

  if ( (strcmp(SymbolTable[i].symbol,item.symbol) == 0) && (SymbolTable[i].address == -1) )
	SymbolTable[i].address = item.address;

  //We have more symbols then what is permitted. This must be reported.
  if (i >= MAXENTRIES)
    printf("ERROR: Symbol Table full.Increase MAXENTRIES by two folds and recompile the Assembler\n");
}

//The following function checks Membership of a given symbol in the symbol table
// It returns the address associated with the symbol if it is found
//else it returns-1
// This is invoked only in Pass 2 during which all symbols are assigned
// an integer address

int containGetAddress(char *symbol) {
	int i = 0;
	while( (i < MAXENTRIES) && (SymbolTable[i].address != -1))
	{
		//If symbol is found return its address
		if (strcmp(SymbolTable[i].symbol,symbol) == 0)
			return (SymbolTable[i].address);
		i++;
	}
    return -1;	//Will reach here only if symbol is not in the symbol table
}

#include <stdlib.h>

FILE* initializeParser(char *filename){
    FILE *fp;
    if( ( fp = fopen( filename, "r" ) ) == NULL){
        fprintf(stderr, "Opening File %s failed\n",filename );
        exit(1);
    }
    return fp;
}

#define MAXCHARPERLINE 80
#define A_COMMAND 0
#define C_COMMAND 1
#define L_COMMAND 2
#define L_AND_A_COMMAND 3 //This  is addition to Pg 113 Label + A in same line
#define L_AND_C_COMMAND 4 //This is addition to Pg 113 Label + C in the same line
#define COMMANDERROR 5

FILE* initilaizeOutput (char *filename) {
	int i = 0;
	char tempFile[MAXCHARPERLINE];
	strcpy(tempFile,filename);
	while(tempFile[i] != '.') 
	    i++;

    tempFile[i+1] = 'h';
	tempFile[i+2] = 'a';
	tempFile[i+3] = 'c';
	tempFile[i+4] = 'k';
	tempFile[i+5] = '\0';

	return (fopen(tempFile,"w"));
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


/************ End of Stage 2 *********************/





/*********Beginning of Stage 3 - PASS 2************/

//Useful for A-Instructions
void convertToBinary(int inpNum,char *outNum) {
	for(int i = 0; i < 15; i++)
	{
	 	if (inpNum%2 == 0) 
	 	    outNum[i] = '0'; 
	 	else 
	 	    outNum[i] = '1';
		inpNum = inpNum/2;
	}
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

//The InterFile is opened in PASS 2
FILE* initializeInterRead(char *filename) {

	int i = 0;
	char tempFile[MAXCHARPERLINE];

	strcpy(tempFile,filename);

	while(tempFile[i] != '.') 
	    i++;

	tempFile[i+1] = 'i';
	tempFile[i+2] = 'n';
	tempFile[i+3] = 't';
	tempFile[i+4] = '\0';

	return (fopen(tempFile,"r"));
}

//This converts Jump mnemonics to binary

void getJmpCode(char *inpJmp, char *jmp)
{
        if (strcmp(inpJmp,"JGT") == 0) strcpy(jmp,"001");
        if (strcmp(inpJmp,"JEQ") == 0) strcpy(jmp,"010");
        if (strcmp(inpJmp,"JGE") == 0) strcpy(jmp,"011");
        if (strcmp(inpJmp,"JLT") == 0) strcpy(jmp,"100");
        if (strcmp(inpJmp,"JNE") == 0) strcpy(jmp,"101");
        if (strcmp(inpJmp,"JLE") == 0) strcpy(jmp,"110");
        if (strcmp(inpJmp,"JMP") == 0) strcpy(jmp,"111");
}

//This converts Designation part of mnemonics to binary

void getDestCode(char *inpDest, char *dest)
{
  if (strcmp(inpDest,"M") == 0) strcpy(dest,"001");
  if (strcmp(inpDest,"D") == 0) strcpy(dest,"010");
  if (strcmp(inpDest,"MD") == 0) strcpy(dest,"011");
  if (strcmp(inpDest,"DM") == 0) strcpy(dest,"011");
  if (strcmp(inpDest,"A") == 0) strcpy(dest,"100");
  if (strcmp(inpDest,"AM") == 0) strcpy(dest,"101");
  if (strcmp(inpDest,"MA") == 0) strcpy(dest,"101");
  if (strcmp(inpDest,"AD") == 0) strcpy(dest,"110");
  if (strcmp(inpDest,"DA") == 0) strcpy(dest,"110");
  if (strcmp(inpDest,"AMD") == 0) strcpy(dest,"111");
  if (strcmp(inpDest,"ADM") == 0) strcpy(dest,"111");
  if (strcmp(inpDest,"MAD") == 0) strcpy(dest,"111");
  if (strcmp(inpDest,"MDA") == 0) strcpy(dest,"111");
  if (strcmp(inpDest,"DAM") == 0) strcpy(dest,"111");
  if (strcmp(inpDest,"DMA") == 0) strcpy(dest,"111");
}

//This converts Compute part of mnemonics to binary

void getCompCode (char *inpComp, char *comp)
{
  if (strcmp(inpComp,"0") == 0) strcpy(comp,"0101010");
  if (strcmp(inpComp,"1") == 0) strcpy(comp,"0111111");
  if (strcmp(inpComp,"-1") == 0) strcpy(comp,"0111010");
  if (strcmp(inpComp,"D") == 0) strcpy(comp,"0001100");
  if (strcmp(inpComp,"A") == 0) strcpy(comp,"0110000");
  if (strcmp(inpComp,"M") == 0) strcpy(comp,"1110000");
  if (strcmp(inpComp,"!D") == 0) strcpy(comp,"0001101");
  if (strcmp(inpComp,"!A") == 0) strcpy(comp,"0110001");
  if (strcmp(inpComp,"!M") == 0) strcpy(comp,"1110001");
  if (strcmp(inpComp,"-D") == 0) strcpy(comp,"0001111");
  if (strcmp(inpComp,"-A") == 0) strcpy(comp,"0110011");
  if (strcmp(inpComp,"-M") == 0) strcpy(comp,"1110011");
  if (strcmp(inpComp,"D+1") == 0) strcpy(comp,"0011111");
  if (strcmp(inpComp,"1+D") == 0) strcpy(comp,"0011111");
  if (strcmp(inpComp,"A+1") == 0) strcpy(comp,"0110111");
  if (strcmp(inpComp,"1+A") == 0) strcpy(comp,"0110111");
  if (strcmp(inpComp,"M+1") == 0) strcpy(comp,"1110111");
  if (strcmp(inpComp,"1+M") == 0) strcpy(comp,"1110111");
  if (strcmp(inpComp,"D-1") == 0) strcpy(comp,"0001110");
  if (strcmp(inpComp,"-1+D") == 0) strcpy(comp,"0001110");
  if (strcmp(inpComp,"A-1") == 0) strcpy(comp,"0110010");
  if (strcmp(inpComp,"-1+A") == 0) strcpy(comp,"0110010");
  if (strcmp(inpComp,"M-1") == 0) strcpy(comp,"1110010");
  if (strcmp(inpComp,"-1+M") == 0) strcpy(comp,"0110010");
  if (strcmp(inpComp,"D+A") == 0) strcpy(comp,"0000010");
  if (strcmp(inpComp,"A+D") == 0) strcpy(comp,"0000010");
  if (strcmp(inpComp,"M+D") == 0) strcpy(comp,"1000010");
  if (strcmp(inpComp,"D+M") == 0) strcpy(comp,"1000010");
  if (strcmp(inpComp,"D-A") == 0) strcpy(comp,"0010011");
  if (strcmp(inpComp,"-A+D") == 0) strcpy(comp,"0010011");
  if (strcmp(inpComp,"D-M") == 0) strcpy(comp,"1010011");
  if (strcmp(inpComp,"-M+D") == 0) strcpy(comp,"1010011");
  if (strcmp(inpComp,"A-D") == 0) strcpy(comp,"0000111");
  if (strcmp(inpComp,"-D+A") == 0) strcpy(comp,"0000111");
  if (strcmp(inpComp,"M-D") == 0) strcpy(comp,"1000111");
  if (strcmp(inpComp,"-D+M") == 0) strcpy(comp,"1000111");
  if (strcmp(inpComp,"D&A") == 0) strcpy(comp,"0000000");
  if (strcmp(inpComp,"A&D") == 0) strcpy(comp,"0000000");
  if (strcmp(inpComp,"M&D") == 0) strcpy(comp,"1000000");
  if (strcmp(inpComp,"D&M") == 0) strcpy(comp,"1000000");
  if (strcmp(inpComp,"D|A") == 0) strcpy(comp,"0010101");
  if (strcmp(inpComp,"A|D") == 0) strcpy(comp,"0010101");
  if (strcmp(inpComp,"M|D") == 0) strcpy(comp,"1010101");
  if (strcmp(inpComp,"D|M") == 0) strcpy(comp,"1010101");
}
/**********End of Stage 3 *************/

/********* Beginning of The Final stage (The main()) function */

//The main() function takes the name of input assembly file as command line

void main(int argc, char **argv) {

	struct SymbolTableEntry item;

	//PASS 1
	FILE *inputFile;

	//PASS2-Start
    FILE *interFile;
	//PASS2-END

    FILE *outputFile;

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

	InitializeSymbolTable();	//Initializing the Symbol Table

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
	                while((parsedCommand[j+1] != ')') && (parsedCommand[j+1] !='\n')) {
						//Extracts the first symbol in case of L_, L_AND_A and L_AND_C
			            symbol1[j] = parsedCommand[j+1];
				        j++;
			        }
			    }
			    symbol1[j] = '\0';

				//Check for duplicate or illegal symbols like () - No symbol or starting with number or no ')'
			    if ((containGetAddress(symbol1) >=0) || (j == 0) || (parsedCommand[j+1] != ')'))
			    {
				    printf("Error: Duplicate or Illegal Label Definition for %s in Line %d with command%s\n",symbol1,lineNumber+1, parsedCommand);
				    exit(-1);
			    }

			    //Add the symbol to symbol table
			    strcpy(item.symbol,symbol1);
			    item.address = instructionNumber;
			    addEntry(item);

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

	                    //Add the symbol2 to symbol table
			            strcpy(item.symbol,symbol2);
			            item.address = -1;
			            addEntry(item);
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
					    //Extracts the first symbol in case of A_COMMAND					This is the correct comment
			            symbol1[j] = parsedCommand[j+1];
					    j++;
				    }
		            symbol1[j] = '\0';
			        //Add the symbol1 to symbol table
			        strcpy(item.symbol,symbol1);
			        item.address = -1;
			        addEntry(item);
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

	// It is not advisable to use
	// fgets() in uncontrolled environment as it does not check
	// for the buffer bounds. This can cause a vulnerability called
	// Buffer overflow

	//Assign Address to Data variables starting from 16. These are symbols with address -1
	j = 16;

	for (i = 0; i < MAXENTRIES; i++) {
		if ((strcmp(SymbolTable[i].symbol,"\0:") != 0) && (SymbolTable[i].address == -1))
		SymbolTable[i].address = j++;

	}

	//Closing the input and output files
	fclose(inputFile);
	fclose(interFile);

	//Beginning of PASS 2

	interFile = initializeInterRead(argv[1]);
	outputFile = initilaizeOutput(argv[1]);

	char assembly_instr[16];

	while(fgets(parsedCommand,MAXCHARPERLINE,interFile/*inputFile*/) != NULL)
	{
		i = commandType(parsedCommand);
		if (i == A_COMMAND) {
			assembly_instr[15] = '0';
			if ((parsedCommand[1] < '0') || (parsedCommand[1] > '9'))
			{
				//The Command is @SYMBOL
				//Copying SYMBOL to symbol string
				j = 1;
				while(parsedCommand[j] != '\n')
				{
					symbol1[j-1] = parsedCommand[j];
					j = j + 1;
				}
				symbol1[j-1] = '\0';
				//Getting address of symbol1 from SymbolTable
				k = containGetAddress(symbol1);
				if (k < 0) {
					printf("ERROR: Symbol not in Symbol Table in PASS 2 in %s \n",parsedCommand);
					exit(-2);
				}
				//Converting address to binary
				convertToBinary(k,assembly_instr);
			}
			else
			{
				//The command is @CONSTANT
				j = 1;
				k = 0;
				//Converting Constant to Integer
				while(parsedCommand[j] != '\n')
				{
					if ((parsedCommand[j] < '0') || (parsedCommand[j] > '9'))
					{
						printf("ERROR: Constant is non-numeric in %s\n",parsedCommand);
						exit(-2);
					}
					k = k*10 + parsedCommand[j] - '0';
					j = j + 1;
				}
				//Converting integer to binary
				convertToBinary(k,assembly_instr);
			}
		}
		else
		{
			//This is a C_Instruction
			// The Instruction would be inpDest = inpComp;inpJmp

			char inpDest[10];
			char inpComp[10];
			char inpJmp[10];

			// These are the respective bits in the C-instruction
			char dest[4];
			char comp[8];
			char jmp[4];

			strcpy(inpDest,"\0");
			strcpy(inpComp,"\0");
			strcpy(inpJmp,"\0");
			strcpy(dest,"\0");
			strcpy(comp,"\0");
			strcpy(jmp,"\0");

			j = 0;
			// Till '=' or ';' is encountered we do not know if it is a 
			// Jmp or compute instruction. So we fill both Dest and Comp
			// fields.
			while ((parsedCommand[j] != '=') && (parsedCommand[j] != ';'))
			{
				inpDest[j] = parsedCommand[j];
				inpComp[j] = parsedCommand[j];
				j++;
			}
			k = 0;

			if (parsedCommand[j] == '=') { //This is a dest = comp statement
				inpDest[j] = '\0'; //Destination need to be retained
				j++;
				while (parsedCommand[j] != '\n') { //Comp need to be recomputed
					inpComp[k] = parsedCommand[j];
					j++;
					k++;
				}
				inpComp[k] = '\0';
				jmp[0] = '0';
				jmp[1] = '0';
				jmp[2] = '0';
				getDestCode(inpDest,dest);
				getCompCode(inpComp,comp);
			}
			else { //This is a comp;jmp instruction
				inpComp[j] = '\0'; //Comp nees to be retained
				j++;
				k=0;
				while (parsedCommand[j] != '\n') { //Jmp need to be computed
					inpJmp[k] = parsedCommand[j];
					j++;
					k++;
				}
				inpJmp[k] = '\0';
				dest[0] = '0';
				dest[1] = '0';
				dest[2] = '0';
				getCompCode(inpComp,comp);
				getJmpCode(inpJmp,jmp);
			}
			assembly_instr[15]='1';
			assembly_instr[14]='1';
			assembly_instr[13]='1';
			assembly_instr[12]=comp[0];
			assembly_instr[11]=comp[1];
			assembly_instr[10]=comp[2];
			assembly_instr[9]=comp[3];
			assembly_instr[8]=comp[4];
			assembly_instr[7]=comp[5];
			assembly_instr[6]=comp[6];
			assembly_instr[5]=dest[0];
			assembly_instr[4]=dest[1];
			assembly_instr[3]=dest[2];
			assembly_instr[2]=jmp[0];
			assembly_instr[1]=jmp[1];
			assembly_instr[0]=jmp[2];
		}
	for(i=0;i<16;i++)
	    fprintf(outputFile,"%c",assembly_instr[i]);
	fprintf(outputFile,"%c",'\n');
	}
	fclose(interFile);
	fclose(outputFile);
}