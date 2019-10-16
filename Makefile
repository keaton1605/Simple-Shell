##sshell: sshell.c
##	gcc -Wall sshell.c -o sshell

sshell: sshell.o CommandLineParse.o
	gcc -Wall -Werror sshell.c CommandLineParse.c -o sshell

CommandLineParse.o: CommandLineParse.c CommandLineParse.h
	gcc -c -Wall -Werror CommandLineParse.c -o CommandLineParse.o

sshell.o: sshell.c CommandLineParse.h
	gcc -c -Wall -Werror sshell.c -o sshell.o 

clean: 
	rm *.o sshell
