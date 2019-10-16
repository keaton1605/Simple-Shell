## Overall Strategy:  
 
A while(1) loop continues getting input from the command line until "exit" is called.
The input of the command line is parsed into arguments (strings seperated by spaces
or special characters), and special character locations are noted. Using this
information, if any special characters existed where they shouldn't an error message
would be printed and the loop would either exit(1) if it was in a child process or
continue() if fork() hadn't been called yet. With the input parsed into different parts of
the command, and all white-space or special characters taken out, the command can
easily be seperated into its parts and passed to execvp(). If any special characters
were detected, these would modify STDIN/STDOUT to ensure that the input/output
would be where it should. With piping, instead of using a single command split into
parts, the command line would instead be put into arrays of commands, with each
command containing the elements of one pipe.

## STRUCT variables

`char* Command` - The first argument (the command) parsed from the command line 
`char* args[15]` - Each argument including the command 
`int inRedirects` - Set to position within command line when "<" found
`int outRedirects` - Set to position within command line when ">" found
`int numPipes` - Incremented everytime "|" found
`int valid` - Used to locate mislocated special characters
`int backP` - Set to position within command line when "&" found
`int numArgs` - total number of arguments 

## Phase 2: read commands from the input
**Error management**
For errors that do not involve special characters, we had specific error catching
methods.
For the "too many process arguments" error, we checked after the command line
input was fully split into strings if there were more than 16.
For the "command not found" error, we checked the return value of execvp() and if it
didn't equal 0 the error message was printed.


## Phase 3: arguments

To initially parse the command line, we use strtok() with each special character (<, >, |,
&, " ") as a delimiter to store each separate argument in an array of strings within a
struct.  We use a second loop to locate the positions of the special characters and
print an error if they are mislocated. These locations are put into their respective
variables within the struct.

## Phase 4: builtin commands
**exit:** Checks the first argument of the command line with strcmp() and if it equals
"exit" then exits with status 0 and "bye" gets printed to stderr. This is executed before
the fork() because this is the one case that the shell should terminate.
**cd:** Checks if the second argument is a valid directory, and switches with chdir() if
it is valid. This happens before fork() because exec() is not necessary.
**pwd:** Uses getcwd to determine current path and prints it to the terminal.

## Phase 5: Input redirection
The struct containing each command has an element called inRedirect which stores
the position of the "<" (if it exists) in relation to the other arguments. This element is
used to determine which argument in the command STDIN_FILENO should be
replaced with. Dup2() is used to accomplish this.

## Phase 6: Output redirection
The struct containing each command has an element called outRedirect which stores
the position of the ">" (if it exists) in relation to the other arguments. This element is
used to determine which argument in the command STDOUT_FILENO should be
replaced with. Dup2() is used to accomplish this.

## Phase 7: Pipeline commands
**Idea:**
After the input is taken from the terminal, the entire command line is sent through
strtok() looking specifically for the pipe delimiter "|". The number of pipes found is
stored in numPipes. Also, an array of CommandLineParse structs is created, each
object containing the arguments of one pipe. If there are no pipes, only the first
element of this array will be used. After the fork(), if numPipes is 0, the process runs
once and has its usual output. If numPipes is nonzero, the process enters a special for
loop that runs as long as there are still pipes after the last executed command. Before
this loop, a file directory int is created and piped. In the loop, if the current command
is the first command, only the STDOUT will be changed, to the write pipe of this file
directory. If the command is between two pipes, it will get input from the read pipe of
the file directory rather than STDIN, and its output will go to the write pipe rather
than STDOUT. After using dup2() to change these inputs/outputs, whatever
command is being worked with is passed into execvp(). This for loop runs until only
the last command has not been executed. At that point, STDIN is replaced with the
read pipe of the file directory and the last command is passed to execvp().
**Actuality:** 
The waitpid() in this for loop would hang waiting for a response from its child for any
pipe other than the first one. Because of this, the command only works currently for
commands that only involve one pipe.

## Phase 8: Background commands
**Idea:** 
To create a separate process group, setpgid() was called within the child process. With
the assumption that one process group at a time has access to the terminal,
tcgetpgrp() is called to check the pgid of the foreground process. Then, a call to
tcsetpgrp() is made to give the child access to the foreground. 
**Actuality:** 
The implementation would hang waiting for a response and didn't work.

## Sources Cited
- https://linux.die.net/man/
- http://man7.org/index.html
