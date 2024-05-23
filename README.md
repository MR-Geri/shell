# Introduction
Our program will consist of a command shell that
will be able to read in bash commands, typically located within PATH directories, and execute them using the fork, execvp, and waitpid syscalls. We will be using a composite pattern for our classes.

Inputs will be tokenized by spaces and put into an array of strings. From there, they will be broken up by given connectors. The broken up data is put into a command class, storing its command and arguments. Then, the connector is put into a connector class with reference to it's command and the connector that will come next.

We will be using a linked-list of connectors, of which there are three diffrent types. The different types dictate control flow to all the following commands (passed through an argument).

We will iterate through the connectors and we will pass the result (error or not error) into the execute function of the next connector, based on that input the connector will react. The connector's execute function will then run the execute function of the command, with its args.

Parentheses will serve as precedence operators that will give certain connectors higher precedence over others. For instance, **(echo A && echo B) || (echo C && echo D)** will output:

    A
    B


**Our program will also include I/O redirection and pipes:**
    
 '>': will redirect output to files. If the file does not already exist, it will be created. If the file does already exist, it will be overwritten.

'>>': will redirect output to files. If the file does not already exist, it will be created. If the file does already exist, output from the program will be appended to the end of the file.

'<': will redirect input from files to commands. This command will only work with existing files.

'|': is the symoblic representation of pipe. When it is used, the output and input will be piped between two different commands within the program.


# Classes

**Command Classes**:
The command classes are classes which execute the given user's commands. They will store the command, and its arguments. When execute is ran it will execute them using a combination of execvp and fork. 

* The system call command subclass will work as stated above.

* The exit command subclass will terminate the program.

* The test command subclass will check if the given file name exists (-e), is a directory (-d), or if it is a regular file (-f). If no flag is passed in, the -e flag is assumed by default. Additionally, tests commands will have a symbolic equivalent, "[]" that will work in place of the actual command "test". **For example, "[-d directory_1]".**

* The input redirection command subclass will be structured like the other subclasses, however it will redirect input to a command from an existing input file using the dup2() sys call.

* The output redirection, both the double and single output, will additionally take in an output file and another Command object. The output will first be redirected to the output file. Then, the Command object will execute, thus passing its output from the Command to the output file.

* Pipe will work in a similar way to output redirection. However, we will duplicate the output from the first process to the second process, and read input from the second process to the first process, using the dup2() and pipe() syscalls.

**Connector Classes**:
The connector classes will be the classes which determine if the command will actually be ran. The connector also has a pointer to its next connector and will call its execute with the result of its current execution passed in as an argument. 

* Fail connector executes its command if it is passed in a false result.

* Pass connector executes its command if it is passed in a true result.

* The any connector will always execute its code.

If a connector doesn't execute its command, it will pass in the given result into the next result.

**Result Class**:
    A class to store a boolean. It's purpose is to allow for the code to be more easily expanded later on.



# Prototypes/Research
* **waitpid():** Used to pause the calling process until the appropriate system information about a child process is received. More specifically, it waits until the child process has ended or been terminated. The *pid* value determines which child process the system should wait for.

* **execvp():** Searchs a path for a desired program to execute. If a match is found, the program is loaded into memory, effectively replacing the current running program.
    * The name of the program that we are searching for
    * Its arguments (in the form of a null-terminated array of character pointers)
* **fork():** Creates a new child process that runs alongside the parent process. The fork() call returns an integer: 
    * ***Negative value***: represents parent process; indicates that the child process was not created successfully
    * ***Zero***: represents the child process
    * ***Positive Value***: represents the parent process

    The total number of processes is equal to 2<sup>n</sup> where n = # of fork() syscalls.

## ***waitpid()*, *execvp()*, and *fork()* can also be used concurrently:**
* ***execvp()*** : used to run a program (a bash command in our case)

* ***fork()*** : used to continue the program after the first process has been terminated (and to allow for additional processes)
* ***waitpid()*** :  used to ensure that the processes run one-by-one by waiting until the child process has ended before calling the parent process 

**see prototypes for example of this*

## **Connectors:**
**We will be implementing three connectors to separate processes (commands) in our program:**
*  " ; " : will be used to separate multiple processes which be then execute in the given order

* " && " : will be used to run a subsequent process only if the process before it was successful

* " | | " : will be used to run a subsequent process only if the process before it was unsuccessful 


