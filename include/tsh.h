#ifndef _SIMPLE_SHELL_H
#define _SIMPLE_SHELL_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

#define MAX_LINE 81

class Process {
 public:
  Process(char *_cmd, int _pipe_in, int _pipe_out);
  ~Process();

  void split_string();
  char *cmd;
  char *cmdTokens[25];

  bool pipe_in;
  bool pipe_out;

  int pipe_fd[2];
};

void run();
void display_prompt();
void cleanup(list<Process *> &process_list, char *input_line);
char *read_input();
void parse_input(char *input_line, list<Process *> &process_list);
bool run_commands(list<Process *> &command_list);
bool isQuit(Process *process);

#endif
