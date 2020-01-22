#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../include/builtins.h"
#include "../include/process.h"
#include "../include/execute.h"
#include "../include/w_io.h"
#include "../include/w_env.h"


int exec_w(char* path,char** argv) //execute and wait for cmd to exit
{
    pid_t pid =fork();
    int exit_code;
    if(pid==0) //inside child process
      {
        
        execvp(argv[0],argv);
        exit(666);
      }
    else if(pid>0){  //inside parent process
        waitfor(pid);
        return 0;
    }
    else return -1;

}

//path arg is unnecessary 
int exec_bg(char* path,char** argv)//execute and return pid of child
{
    pid_t pid =fork();
    //int exit_code;
    if(pid==0) //inside child process
      {
        
        execvp(path,argv);
        exit(666);
      }
    else if(pid>0)return pid;  //parent process returns without waiting
    else return -1;
}


static int create_pipeline(int* pipefd_arr,int n_pipes)
{
    int a = 0;
    for( ;a < n_pipes; a++ ){    //creating pipes
    if( pipe(pipefd_arr + a*2) < 0 ){
        perror("Pipe not created ");
        return -1;
        }
    }
    return 0;    

}

static int execute_builtins(char* cmd_name,char** argv)
{
        if(strcmp(cmd_name,"exit")==0)exit(0);
        else if(strcmp(cmd_name,"cd")==0)
        {
            changedir(argv);
            return 0;
        }
        else if(strcmp(cmd_name,"roit")==0)
        {
            printf("I love environmental science!\n");
            return 0;
        }
        else return -1;//ie this was not a builtin






}





int execute_pipeline(char*** argp,int npipes,int file_output_flag)//executes a pipeline of commands cmd | cmd ...| cmd [> filename] | [>> filename]
{

     // number of pipes
    if(npipes==-1){
		perror("Syntax Error!");
		return -1;
	}

	int pipefd[2*npipes];
    if(create_pipeline(pipefd,npipes)==-1)exit(666);
    
    int file_fd = 0;//
    int EXIT_STAT;
    int ci=0;//command counter
    int internal = -1;//-1 for external and 0 for internal
    pid_t pid[npipes+1];
    for (int i=0;i<=npipes;i++)pid[i]=-1;

        //parent function calls here
    while(argp[ci]&&ci<=npipes){
    
        if((internal=execute_builtins(argp[ci][0],argp[ci]))==-1){
        pid[ci]=fork();

        if( pid[ci]==0 ) //child process inside this if
        {
        

            if(ci)redir_in_to(pipefd[(ci-1)*2]); //start of pipe
            
            if(ci<npipes)redir_out_to(pipefd[ci*2+1]); // between start and end
        
        //possible pipe ends
            if(ci==npipes&&file_output_flag==1)
                file_fd=out_to_file(argp[ci+1][0]);
                                                       //S_IRWXU --> permissions are same as touch cmd to user who created file
                                                        // --rw-r--r-
            else if(ci==npipes&&file_output_flag==2)
                file_fd=append_to_file(argp[ci+1][0]);
                    


            for( int j = 0; j < 2 * npipes; j++ )
                close( pipefd[j] );
        
            
			execvp(*argp[ci],argp[ci]); //executes current cmd
			perror("command not found!\n");
			exit(0);
        
        }
         //parent process 
        else if(pid[ci] > 0)ci++;
        else return -1; //fork error
    
    }
    
        
    else if(internal == 0) {
            if(ci)redir_in_to(pipefd[(ci-1)*2]); //start of pipe
            
            if(ci<npipes)redir_out_to(pipefd[ci*2+1]); // between start and end
        
        //possible pipe ends
            if(ci==npipes&&file_output_flag==1)
                file_fd=out_to_file(argp[ci+1][0]);
                                                       //S_IRWXU --> permissions are same as touch cmd to user who created file
                                                        // --rw-r--r-
            else if(ci==npipes&&file_output_flag==2)
                file_fd=append_to_file(argp[ci+1][0]);
                    


        ci++;    
            
       
 


    }    
    
    
    
        }
        
    for( int k = 0; k < 2 * npipes; k++ ){
            close( pipefd[k] );
    }
    
    for(int b=0;b<=npipes;b++)waitfor(pid[b]); //waiting for last child process to exit(terminate)
    return 0;

}

//still to be implemented********(needs to be implemented immediately) ****internal command execution support needs to be added
int execute_cmd_struct(cmd_struct* cmd)
{
    int i = 0;
    int exit_code = 0;
    while(cmd->argp_arr[i])
    {
        exit_code = execute_pipeline(cmd->argp_arr[i],cmd->n_pipes[i],cmd->file_out[i]);
        i++;
    }
    return 0;
}