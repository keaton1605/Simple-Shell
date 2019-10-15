#ifndef COMMANDLINEPARSE_H
#define COMMANDLINEPARSE_H

typedef struct CommandLineParse
{
	char* Command;
	char* args[15];
	char specialChar[16];
	int inRedirects;
	int outRedirects;
	int numPipes;
	int valid;
	int backP;
	int numArgs;
} CommandLineParse;

struct CommandLineParse ParseCommandLine (char* input);

#endif
