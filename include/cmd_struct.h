#ifndef CMD_STRUCT_H
#define CMD_STRUCT_H

// this structure is crazy
typedef struct cmd_struct
{
  int n_pipelines;   // no. of pipelines
  char**** argp_arr; // contains cmd argv for each command for each pipeline
  int* n_pipes;      // contains no of pipes info for pipeline
  int* file_out;     // contains output to file info about each pipeline
  int* cmd_flow;     // contains pipeline flow info that is && separator implementation
} cmd_struct;

void init_cmd_struct(cmd_struct** cmd);
void free_cmd_struct(cmd_struct** cmd);

#endif
