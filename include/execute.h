#ifndef EXECUTE_H
#define EXECUTE_H
#include "cmd_struct.h"

// execute and wait for cmd to exit
int exec_w(char* path, char** argv);

// execute and return pid of child
int exec_bg(char* path, char** argv);

// execute a pipeline of commands cmd | cmd ...| cmd [> filename] | [>> filename]
int execute_pipeline(char*** argp, int npipes, int file_output_flag);

// execute parsed wish input
int execute_cmd_struct(cmd_struct* cmd);

#endif //EXECUTE_H

