#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX_SUB_COMMANDS 10
#define MAX_ARGS 10


struct SubCommand
{
  char *line;
  char *argv[MAX_ARGS];
};

struct Command
{
  struct SubCommand sub_commands[MAX_SUB_COMMANDS];
  int num_sub_commands;
  char *stdin_redirect;
  char *stdout_redirect;
  int background;
};

//Initialising the functions
void ReadCommand(char* ,struct Command *);
void PrintCommand(struct Command *);
void ReadArgs(char* , char** , int );
void print_args(char** );
void ReadRedirectsAndBackground(struct Command *command);

//exec.c functions
int exec_subcom(struct Command *);
int one_sub_command(struct Command *);
void signal_handler(int );

#endif //FUNCTIONS_H
