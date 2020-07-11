#include "cmd_struct.h"
#include <stdlib.h>
#define MAXCMDS 20
#define MAXPIPELINES 10

void init_cmd_struct(cmd_struct** cmd)
{
    *cmd = (cmd_struct*)malloc(sizeof(cmd_struct));
    (*cmd)->argp_arr =(char****)malloc(sizeof(char***)*MAXPIPELINES); //parsed commands to be executed
    (*cmd)->n_pipes =(int*)malloc(sizeof(int*)*MAXPIPELINES);//contains no of pipes of info;
    (*cmd)->file_out =(int*)malloc(sizeof(int*)*MAXPIPELINES);
    (*cmd)->cmd_flow =(int*)malloc(sizeof(int*)*MAXPIPELINES);
    
    int i ;
    for(i=0;i<MAXPIPELINES;i++)(*cmd)->n_pipes[i] = 0;
    for(i=0;i<MAXPIPELINES;i++)(*cmd)->file_out[i] = 0;
    for(i=0;i<MAXPIPELINES;i++)(*cmd)->cmd_flow[i] = 0;

    (*cmd)->n_pipelines = 0;
}



void free_cmd_struct(cmd_struct** cmd)
{
    if(*cmd==NULL)return;
    int i;
    for(i=0;i< (*cmd)->n_pipelines;i++)
    {
        free((*cmd)->argp_arr[i]);
    }
    free((*cmd)->n_pipes);
    free((*cmd)->file_out);
    free((*cmd)->cmd_flow);
}