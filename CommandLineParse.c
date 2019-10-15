#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CommandLineParse.h"

struct CommandLineParse ParseCommandLine (char* input)
{
	char pipeInput[512];
	strcpy(pipeInput, input);

	int argNum = 0;
	char string[10] = "";

	struct CommandLineParse Parse;

	char input2[512];
	strcpy(input2, input);
	int j = 0;
	Parse.inRedirects = 0;
	Parse.outRedirects = 0;
	Parse.numPipes = 0;
	Parse.numArgs = 0;
	
	char* newToken;
	char* delimiter = " <>|";

	int ARGS = 0;
	
	newToken = strtok(pipeInput, delimiter);

	while( newToken != NULL )
	{	
		newToken = strtok(NULL, delimiter);
		ARGS++;		
	}

	Parse.numArgs = ARGS;

	while(j < strlen(input2))
	{
		if (input2[j] != ' ' && input2[j] != '\0') {
			if (input2[j] == '>') {
				if (argNum == Parse.numArgs ) {
					Parse.valid = 1;
					printf("Error: no output file\n");
					return Parse;
				}

				if (argNum == 0) {
					Parse.valid = 1;
					printf("Error: missing command\n");
					return Parse;
				}		

				if (strlen(string) != 0)
					++argNum;
				Parse.outRedirects = argNum;
				string[0] = '\0';

			} else if (input2[j] == '<') {
				if (argNum == Parse.numArgs ) {
					Parse.valid = 1;
					printf("Error: no input file\n");
					return Parse;
				}

				if (argNum == 0) {
					Parse.valid = 1;
					printf("Error: missing command\n");
					return Parse;
				}

				if (strlen(string) != 0)
					++argNum;

				Parse.inRedirects = argNum;
				string[0] = '\0';

			} else if (input2[j] == '|') {
				if (strlen(string) != 0)
					++argNum;
				string[0] = '\0';
			} else if (input2[j] == '&') {
				if (argNum == 0) {
					Parse.valid = 1;
					printf("Error: missing command\n");
					return Parse;
				} else if (argNum != Parse.numArgs - 1) {
					Parse.valid = 1;
					printf("Error: mislocated background sign\n");
					return Parse;
				}				
			} else {
				strncat(string, &input2[j], 1);
			}

		} else {
			if (strlen(string) == 0) {
				if (++j == strlen(input2))
					argNum--;
				continue;

			} else {
				argNum++;
				string[0] = '\0';
			}
		}
		++j;
	}

	char* findToken;
	int i = 0;
	findToken = strtok(input, delimiter);

	Parse.valid = 0;
	Parse.Command = findToken;
	Parse.args[i] = findToken;

	while( findToken != NULL )
	{	
		findToken = strtok(NULL, delimiter);
		Parse.args[++i] = findToken;
		Parse.numArgs++;

		if (i > 16) 
		{		
			printf("Error: too many process arguments\n");
			Parse.valid = 1;
			return Parse;
		}
	}

	return Parse;

}
