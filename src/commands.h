#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "results.h"
#include <string>
#include <vector>

using namespace std;

class Command {
public:
  string command;
  string args;
  Command(){};
  virtual Result *execute() = 0;
};

class ExitCommand : public Command {
public:
  ExitCommand(){};
  Result *execute();
};

class CdCommand : public Command {
public:
  CdCommand(string command, string args) {
    this->command = command;
    this->args = args;
  };
  Result *execute();
};

class SysCommand : public Command {
public:
  SysCommand(string command, string args) {
    this->command = command;
    this->args = args;
  };
  Result *execute();
};

class TestCommand : public Command {
public:
  TestCommand(string command, string args) {
    this->command = command;
    this->args = args;
  }
  bool exists(string);
  Result *execute();
};

class OutRedir : public Command {
private:
  string file;
  Command *prev = NULL;

public:
  OutRedir(string file, Command *prev) {
    this->file = file;
    this->prev = prev;
  }
  Result *execute();
};

class DubOutRedir : public Command {
private:
  string file;
  Command *prev = NULL;

public:
  DubOutRedir(string file, Command *prev) {
    this->file = file;
    this->prev = prev;
  }
  Result *execute();
};

class InRedir : public Command {
private:
  string file;
  string flag;

public:
  InRedir(string command, string file, string flag) {
    this->command = command;
    this->file = file;
    this->flag = flag;
  }
  Result *execute();
};

class PipeCommand : public Command {
private:
  Command *prev = nullptr;

public:
  PipeCommand(string command, Command *prev) {
    this->command = command;
    this->prev = prev;
  }

  Result *execute();
};

#endif
