#include "cmd_struct.h"

int exec_w(char* path,char** argv); //execute and wait for cmd to exit

int exec_bg(char* path,char** argv);//execute and return pid of child

//executes a pipeline of commands cmd | cmd ...| cmd [> filename] | [>> filename]
int execute_pipeline(char*** argp,int npipes,int file_output_flag);

static int create_pipeline(int* pipefd_arr,int n_pipes);

int execute_cmd_struct(cmd_struct* cmd);//executes parsed wish input




