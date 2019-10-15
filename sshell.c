#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>      
#include <sys/stat.h>

#include "CommandLineParse.h"

int main(int argc, char *argv[])
{
	while(1) 
	{
		char pipeInput[512] = "";
		char input2[512] = "";
		char input[512] = "";
		printf("sshell$ ");
		fgets(input,512,stdin);

		if (!isatty(STDIN_FILENO)) {
			printf("%s", input);
			fflush(stdout);
		}

		input[strlen(input) - 1] = 0;
		strcpy(input2, input);
		strcpy(pipeInput, input);

		char* delimiter = "|\0";
		char* findToken = "smh";
		int numPipes = 0;
		char* Comm[512];

		findToken = strtok(pipeInput, delimiter);

		/*if (findToken == NULL)
		{
			printf("Error: missing command\n");
			continue;
		}*/

		Comm[0] = findToken;

		while(1)
		{	
			findToken = strtok(NULL, delimiter);
			if ( findToken == NULL )
				break;
			Comm[++numPipes] = findToken;
		}

		struct CommandLineParse* AllCommands = (struct CommandLineParse*)malloc(numPipes * sizeof(CommandLineParse));

		int commandNum = 0;
		for (commandNum = 0; commandNum <= numPipes; ++commandNum) {
			AllCommands[commandNum] = ParseCommandLine(Comm[commandNum]);
		}

		if (AllCommands[0].valid == 1) {
			free(AllCommands);
			continue;
		}

		pid_t pid;
		pid_t pipeId;

		int retval;
		
		if (strcmp(AllCommands[0].Command, "exit") == 0)
		{
			fprintf(stderr, "Bye...\n");
			free(AllCommands);
			exit(0);
		} else if (strcmp(AllCommands[0].Command, "cd") == 0) {
			if (chdir(AllCommands[0].args[1]) != 0) {
				printf("Error: no such directory\n");
				printf("+ completed 'cd %s' [1]\n", AllCommands[0].args[1]);
				free(AllCommands);
				continue;
			}
		}

		int currPipe = 0;
		int Pipes = 0;

		//if(numPipes != 0)
		//{
			Pipes = numPipes;
			//numPipes--;
		//}

		struct CommandLineParse Commands = AllCommands[currPipe];

		if (strcmp(Commands.Command, "cd") != 0)
		{
			int fd[2];
			pipe(fd);
			pid = fork();

			if (pid == 0) {		

				if (Pipes != 0) {

					int fd[2];
					pipe(fd);
					int workingPipe = 0;
					pipeId = fork();

					for (workingPipe = 0; workingPipe < Pipes; ++workingPipe)
					{
						//pipeId = fork();
						//printf("%d\n", workingPipe);
						if (pipeId == 0)
						{	
							//printf("In pipe: %d\n", workingPipe);
							//if (workingPipe == 0)
								dup2(fd[1], STDOUT_FILENO);
							if (workingPipe != 0)
								dup2(fd[0], STDIN_FILENO);
							//printf("wow\n");
							execvp(AllCommands[workingPipe].Command, AllCommands[workingPipe].args); 
						} else {
							waitpid(-1, &retval, 0);
							//printf("done waiting\n");
						}
					}
						
					//if (pipeId != 0)
					//	waitpid(-1, &retval, 0);
					//printf("Done piping!\n");
					//close(savedStdout);

					//if (pipeId == 0)

					if (AllCommands[Pipes].outRedirects != 0) {
						int filed = open(AllCommands[Pipes].args[AllCommands[Pipes].outRedirects], O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
						char *file = AllCommands[Pipes].args[AllCommands[Pipes].outRedirects];
						if (access(file, W_OK) == -1) {
							printf("Error: cannot open output file\n");
							free(AllCommands);
							exit(1);
						}

						dup2(filed,STDOUT_FILENO);
						close(filed);
						AllCommands[Pipes].args[AllCommands[Pipes].outRedirects] = NULL;

					}
					close(fd[1]);
					dup2(fd[0], STDIN_FILENO);
					close(fd[0]);
					execvp(AllCommands[Pipes].Command, AllCommands[Pipes].args);
					//else 
					//	waitpid(-1, &retval, 0);
					//free(AllCommands);
					continue;

				}

				if (Commands.outRedirects != 0) {
					int fd = open(Commands.args[Commands.outRedirects], O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
					char *file = Commands.args[Commands.outRedirects];
					if (access(file, W_OK) == -1) {
						printf("Error: cannot open output file\n");
						free(AllCommands);
						exit(1);
					}

					dup2(fd,STDOUT_FILENO);
					close(fd);
					Commands.args[Commands.outRedirects] = NULL;
				
				} else if (Commands.inRedirects != 0) {
					int fd = open(Commands.args[Commands.inRedirects], O_RDONLY);
					if (fd == -1) {
						printf("Error: cannot open input file\n");
						free(AllCommands);
						exit(1);
					}

					dup2(fd,STDIN_FILENO);
					close(fd);
				
				}

				if (strcmp(AllCommands[0].Command, "pwd") == 0) {
					free(AllCommands);						
					printf("%s\n", getcwd(pipeInput, 512)); 
					exit(0);
				}

				retval = execvp(Commands.Command, Commands.args);
				if (retval != 0) {
					printf("Error: command not found\n");
					exit(1);					
				}

			} else if (pid > 0) {
				waitpid(-1, &retval, 0);
			} else {
				perror("fork");
				exit(1);
			}
		}

		//if (WEXITSTATUS(retval) != 1)
		fprintf(stderr, "+ completed '%s' [%d]\n", input2, WEXITSTATUS(retval));
		free(AllCommands);
	}

	return EXIT_SUCCESS;
}
