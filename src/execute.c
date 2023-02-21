#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "builtins.h"
#include "process.h"
#include "execute.h"
#include "w_io.h"
#include "w_env.h"

static int create_pipeline(int* pipefd_arr, int n_pipes);

int exec_w(char* path, char** argv)
{
  pid_t pid = fork();
  if (pid == 0)     // inside child process
  {
    execvp(argv[0], argv);
    exit(666);      // 666 the number of the beast...sacrifice is going on tonight
                    // this line would never be executed..coz the process image has 
                    // already been changed by this time
  }
  else if (pid > 0) // inside parent process(the shell)
  { 
    waitfor(pid);
    return 0;
  }
  else
  {
    return -1;
  }
}

int exec_bg(char* path, char** argv) 
{
  pid_t pid = fork();
  if (pid == 0)         // inside child process
  {
    execvp(path, argv);
    exit(666);
  }
  else if (pid > 0)     // parent process returns without waiting
  {
    return pid;         
  }
  else
  {
    return -1;
  }
}

static int create_pipeline(int* pipefd_arr, int n_pipes)
{
  int idx;
  //creating pipes, pipeline is a made up word
  for (idx = 0; idx < n_pipes; idx++)
  {
    if (pipe(pipefd_arr + idx * 2) < 0)
    {
      perror("Pipe not created");
      return -1;
    }
  }
  return 0;
}

static int check_builtins(char* cmd_name)
{
  if (!strcmp(cmd_name, "exit"))
    return 0;
  else if (!strcmp(cmd_name, "cd"))
    return 0;
  else if (!strcmp(cmd_name, "roit"))
    return 0;
  return -1;
}

static int execute_builtins(char* cmd_name, char** argv)
{
  //every internal command must follow this pattern
  //{ 
  //builtincall();
  //fflush(stdout);
  //return 0;
  //}
  // can reduce runtime by using function pointers stored in lookup tables for these
  if (strcmp(cmd_name, "exit") == 0)
    exit(0); 
  else if (strcmp(cmd_name, "cd") == 0)
  {
    changedir(argv);
    fflush(stdout);
    return 0;
  }
  else if(strcmp(cmd_name,"roit") == 0) // for the sake of memories
  {
    printf("I love environmental science!\n");
    printf("I love jaypee pankha!\n");
    fflush(stdout);
    return 0;
  }
  return -1;
}

//executes a pipeline of commands cmd | cmd ...| cmd [> filename] | [>> filename]
int execute_pipeline(char*** argp, int npipes, int file_output_flag)
{
  // number of pipes
  if (npipes == -1)
  {
    perror("Syntax Error!");
    return -1;
  }

  int* pipefd = (int*)malloc(2 * npipes * sizeof(int)); // should be heap allocated instead
  if (create_pipeline(pipefd, npipes) == -1)
    return -1;

  int file_fd = -1;// never ever init any fd with positive values man it produced dangerous bug
  int EXIT_STAT;
  int ci = 0;//command counter
  int internal = -1;//-1 for external and 0 for internal
  int pre_ = -1;
  pid_t* pid = (pid_t*)malloc((npipes + 1) * sizeof(pid_t));
  int k;
  for (k = 0; k <= npipes; k++)
    pid[k] = -1;

  //parent function calls here
  while (argp[ci] && ci <= npipes)
  {
    if ((internal = check_builtins(argp[ci][0])) == -1)
    {
      pid[ci] = fork();
      if (pid[ci] == 0) // inside child process
      {
        restore_std_fd();
        //internal cmd piped to external fix:
        //https://stackoverflow.com/questions/59866269/grep-hangs-when-executed-using-execvp-function-in-my-c-program 
        // start of pipe
        if (ci)
          redir_in_to(pipefd[(ci - 1) * 2]); 
        //dup2(STD_OUT_DUP,1);//necessary if child inherits already redirected std fd(part of fix)
        // between start and end
        if (ci < npipes)
          redir_out_to(pipefd[ci * 2 + 1]); 
        //possible pipe ends
        if (ci == npipes && file_output_flag == 1)
          file_fd = out_to_file(argp[ci + 1][0]);
        else if (ci == npipes && file_output_flag == 2)
          file_fd = append_to_file(argp[ci + 1][0]);

        for (k = 0; k < 2 * npipes; k++)
          close(pipefd[k]);

        execvp(*argp[ci], argp[ci]); //executes current cmd
        char errStr[2048];
        sprintf(errStr, "bash: %s", *argp[ci]);
        perror(errStr);
        exit(0);
      }
      else if (pid[ci] > 0) // inside parent process
      {
        restore_std_fd();//it causes the EOF to happen if internal cmd is connected to external by pipe(part of fix)
        ci++;
      }
      return -1; //fork error
    }
    else if (internal == 0) // builtins
    {
      pid[ci] = -1;
      pre_ = 0;
      if (ci)
        redir_in_to(pipefd[(ci - 1) * 2]); //input redir of pipe
      if (ci < npipes)
        redir_out_to(pipefd[ci * 2 + 1]); // output redir of pipe

      //possible pipe ends
      if (ci == npipes && file_output_flag == 1)
        file_fd = out_to_file(argp[ci + 1][0]);
      else if(ci == npipes && file_output_flag == 2)
        file_fd = append_to_file(argp[ci + 1][0]);

      execute_builtins(argp[ci][0], argp[ci]);
      ci++;
    }
  }
  //restore shell stdin and stdout
  if (internal == -1 || file_output_flag > 0)
    restore_std_fd();      

  for (k = 0; k < 2 * npipes; k++)
    close(pipefd[k]);
  // waiting for last child process to exit(terminate)
  for (k = 0; k <= npipes; k++)
    waitfor(pid[k]); 
  //important was causing shell to hang
  close(file_fd);
  return 0;
}

int execute_cmd_struct(cmd_struct* cmd)
{
  int i = 0;
  int exit_code = 0;
  while (cmd->argp_arr[i])
  {
    exit_code = execute_pipeline(cmd->argp_arr[i], cmd->n_pipes[i], cmd->file_out[i]);
    i++;
  }
  return 0;
}

