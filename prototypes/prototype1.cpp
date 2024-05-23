#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

using namespace std;
// tests echo and git branch using syscalls
int main() {
    char* args[3];
    char* args2[3];
    string cmd = "echo";
    string cmd2 = "first process";
    string cmd3 = "git";
    string cmd4 = "branch";
//First process
    args[0] = (char*)cmd.c_str();
    args[1] = (char*)cmd2.c_str();
    args[2] = NULL;
//Second Process
    args2[0] = (char*)cmd3.c_str();
    args2[1] = (char*)cmd4.c_str();
    args2[2] = NULL;

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