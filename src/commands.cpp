#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "commands.h"
#include "connectors.h"
#include "integrator.h"
#include "results.h"

using namespace std;

Result *SysCommand::execute() {
  char *args[3];
  args[0] = (char *)this->command.c_str();
  args[1] = (char *)this->args.c_str();
  args[2] = NULL;
  pid_t pid = fork();
  if (pid == -1) {
    throw __throw_runtime_error;
  } else if (pid > 0) {
    int returnval = 0;
    wait(&returnval);
    if (returnval != 0) {
      return new Result(false);
    } else {
      return new Result(true);
    }

  } else {
    int result = execvp(args[0], args);
    if (result == -1) {
      perror("Error");
      exit(-1);
    }
  }
}

Result *CdCommand::execute() {
  if (chdir(this->args.c_str()) != 0) {
    return new Result(false);
  }
  return new Result(true);
}

bool TestCommand::exists(string file) {
  struct stat validate;

  if (file.find("-d") != string::npos) { // checks if it's a directory
    file.replace(file.find("-d"), 3, "");
    if (stat(file.c_str(), &validate) != 0) {
      return false;
    }
    return S_ISDIR(validate.st_mode);
  }
  if (file.find("-f") != string::npos) { // checks if it's a regular file
    file.replace(file.find("-f"), 3, "");
    if (stat(file.c_str(), &validate) != 0) {
      return false;
    }
    return S_ISREG(validate.st_mode);
  }
  if (file.find("-e") != string::npos) {
    file.replace(file.find("-e"), 3, ""); // checks if file exists
  }
  return (stat(file.c_str(), &validate) == 0);
}

Result *TestCommand::execute() {
  if (exists(this->args)) {
    cout << "(True)" << endl;
    return new Result(true);
  }
  cout << "(False)" << endl;
  return new Result(false);
}

Result *OutRedir::execute() {
  int fd; // 0 = stdin, 1 = stdout, 2 = stderr
  int result;
  int stdout = dup(1); // save stdout to revert back later

  fd = open(this->file.c_str(), O_RDWR | O_CREAT | O_TRUNC,
            0666); // overwrites file or creates a new one

  if (fd < 0) {
    // something went wrong...
    perror("Error");
    exit(1);
  }

  result = dup2(fd, 1); // replace stdout w/ file

  if (result < 0) {
    perror("Error");
    exit(1);
  }

  if (this->prev != NULL) {
    Result *res = this->prev->execute();
    dup2(stdout, 1);
    close(stdout);

    return res;
  }
}

Result *InRedir::execute() {
  int fd; // 0 = stdin, 1 = stdout, 2 = stderr
  int result;
  int result2;
  int fd2;
  int stdin = dup(0); // save stdin to revert back later
  int stdout = dup(1);

  fd = open(this->file.c_str(), O_RDONLY); // opens input file

  if (fd < 0) {
    // something went wrong...
    perror("Error");
    exit(1);
  }

  result = dup2(fd, STDIN_FILENO); // replace stdin w/ file
  if (result < 0) {
    perror("Error");
    exit(1);
  }

  vector<string> ssss;
  if (foundString(this->command, "tr")) {
    this->command = command + " " + this->flag + " " + this->file;
    istringstream ss(this->command);
    vector<string> ssss;

    while (ss) {
      string temp;
      ss >> temp;
      ssss.push_back(temp);
    }
  } else {
    int spaceLocation = this->command.find(' ');
    if (spaceLocation > -1) {
      string first = command;
      string second = command;
      first.erase(spaceLocation, first.size());
      second.erase(0, spaceLocation + 1);
      ssss.push_back((char *)first.c_str());
      ssss.push_back((char *)second.c_str());
    } else {
      string first = command;
      ssss.push_back((char *)first.c_str());
    }
  }

  char *arg[ssss.size() + 1];
  for (int i = 0; i < ssss.size(); i++) {
    arg[i] = (char *)ssss.at(i).c_str();
  }
  arg[ssss.size()] = NULL;

  pid_t pid = fork();
  if (pid == -1) {
    throw __throw_runtime_error;
  } else if (pid > 0) {
    int returnval = 0;
    wait(&returnval);
    if (returnval != 0) {
      dup2(stdin, STDIN_FILENO);
      close(stdin);

      return new Result(false);
    } else {
      dup2(stdin, STDIN_FILENO);
      close(stdin);
      return new Result(true);
    }

  } else {
    int result = execvp(arg[0], arg);
    if (result == -1) {
      perror("Error");
      exit(-1);
    }
  }
}

Result *DubOutRedir::execute() {
  int fd; // 0 = stdin, 1 = stdout, 2 = stderr
  int result;
  int stdout = dup(1); // save stdout to revert back later

  fd = open(this->file.c_str(), O_RDWR | O_CREAT | O_APPEND,
            0666); // overwrites file or creates a new one

  if (fd < 0) {
    // something went wrong...
    perror("Error");
    exit(1);
  }

  result = dup2(fd, 1); // replace stdout w/ file

  if (result < 0) {
    perror("Error");
    exit(1);
  }

  if (this->prev != NULL) {
    Result *res = this->prev->execute();
    dup2(stdout, 1);
    close(stdout);

    return res;
  }
}

Result *PipeCommand::execute() {
  int fds[2];
  int fds2[2];

  istringstream ss(this->command);
  vector<string> ssss;

  while (ss) {
    string temp;
    ss >> temp;
    ssss.push_back(temp);
  }

  char *arg[ssss.size()];
  for (int i = 0; i < ssss.size(); i++) {
    arg[i] = (char *)ssss.at(i).c_str();
  }
  arg[ssss.size() - 1] = NULL;

  const int PIPE_WRITE = 1;
  const int PIPE_READ = 0;

  int ret = pipe(fds);

  if (ret < 0) {
    perror("Error");
    exit(1);
  }

  pid_t firstFork = fork();
  if (firstFork == 0) {
    dup2(fds[1], PIPE_WRITE);
    close(fds[0]);
    close(fds[1]);
    this->prev->execute()->getResult();
    exit(0);
  }
  pid_t secondFork = fork();
  if (secondFork == 0) {
    dup2(fds[0], PIPE_READ);
    close(fds[0]);
    close(fds[1]);
    int result = execvp(arg[0], arg);
    if (result == -1) {
      perror("Error");
      exit(-1);
    }
    exit(0);
  }
  close(fds[0]);
  close(fds[1]);
  int returnval = 0;

  waitpid(firstFork, 0, 0);
  wait(&returnval);
  // waitpid(secondFork, 0, 0);

  if (returnval != 0) {
    return new Result(false);
  } else {
    return new Result(true);
  }
}
