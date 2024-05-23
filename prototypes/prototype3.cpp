#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

using namespace std;

int main() {
    int fd;
    int result;

    fd = open("file3.txt", O_CREAT | O_APPEND | O_WRONLY);

    if (fd < 0) {
        perror("Error"); 
        exit(1);
    }

    result = dup2(fd,1);

    if (result < 0) {
        perror("Error");
        exit(1);
    }

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


    close(fd);



    return 0;
}
