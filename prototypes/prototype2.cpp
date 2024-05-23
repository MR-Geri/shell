#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

using namespace std;
// tests ls and invalid input using syscalls
int main() {
    char* args[2];
    char* args2[2];
    string cmd = "ls";
    string cmd2 = "dsafaad"; //invalid command
//First process
    args[0] = (char*)cmd.c_str();
    args[1] = NULL;
//Second Process
    args2[0] = (char*)cmd2.c_str();
    args2[1] = NULL;

pid_t pid = fork();

if (pid == 0) { //child process
    if (execvp(args[0],args) == -1) {
        perror("exec");
    }
}
if (pid > 0) { //parent process
    if (waitpid(pid, NULL, 0) == -1) {
        perror("waitpid");
    }
    if (execvp(args2[0],args2) == -1) {
        perror("exec");
    }
}
    return 0;
}