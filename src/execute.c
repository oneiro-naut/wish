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


static int check_builtins(char* cmd_name)
{
    if(strcmp(cmd_name,"exit")==0)return 0;
        else if(strcmp(cmd_name,"cd")==0)
        {
            //changedir(argv);
            return 0;
        }
        else if(strcmp(cmd_name,"roit")==0)
        {
            
            return 0;
        }
        else return -1;//ie this was not a builtin






}

static int execute_builtins(char* cmd_name,char** argv)
{
        if(strcmp(cmd_name,"exit")==0)exit(0); //every internal command must follow this pattern { builtincall(); fflush(stdout); return 0;}
        else if(strcmp(cmd_name,"cd")==0)
        {
            changedir(argv);
            fflush(stdout);
            return 0;
        }
        else if(strcmp(cmd_name,"roit")==0)
        {
            printf("I love environmental science!\n");
            printf("I love jaypee pankha!\n");
            fflush(stdout);
            
            //printf("I love environmental science!\n");
            //fflush(stdout);//(part of fix::internal piped to external)::reason::
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
    
    int file_fd = -1;// never ever init any fd with positive values man it produced dangerous bug
    int EXIT_STAT;
    int ci=0;//command counter
    int internal = -1;//-1 for external and 0 for internal
    int pre_ =-1;
    pid_t pid[npipes+1];
    for (int i=0;i<=npipes;i++)pid[i]=-1;

        //parent function calls here
    while(argp[ci]&&ci<=npipes){
    
        if((internal=check_builtins(argp[ci][0]))==-1 ){
        pid[ci]=fork();

        if( pid[ci]==0 ) //child process inside this if
        {
        
            restore_std_fd();
            //internal cmd piped to external fix:
            //https://stackoverflow.com/questions/59866269/grep-hangs-when-executed-using-execvp-function-in-my-c-program 
            if(ci)redir_in_to(pipefd[(ci-1)*2]); //start of pipe
            //dup2(STD_OUT_DUP,1);//necessary if child inherits already redirected std fd(part of fix)
            if(ci<npipes)redir_out_to(pipefd[ci*2+1]); // between start and end
        
        //possible pipe ends
            if(ci==npipes&&file_output_flag==1)
                file_fd=out_to_file(argp[ci+1][0]);
    
            else if(ci==npipes&&file_output_flag==2)
                file_fd=append_to_file(argp[ci+1][0]);
                    


            for( int j = 0; j < 2 * npipes; j++ )
                close( pipefd[j] );
        
            
			execvp(*argp[ci],argp[ci]); //executes current cmd
			perror("\n");
			exit(0);
        
        }
         //parent process 
        else if(pid[ci] > 0)
        {
            
            
            restore_std_fd();//it causes the EOF to happen if internal cmd is connected to external by pipe(part of fix)
            ci++;
        
        }
        else return -1; //fork error
    
    }
    
        
    else if(internal == 0) {
        pid[ci]=-1;
            pre_ = 0;
            if(ci)redir_in_to(pipefd[(ci-1)*2]); //input redir of pipe
            
            if(ci<npipes)redir_out_to(pipefd[ci*2+1]); // output redir of pipe
        
        //possible pipe ends
            if(ci==npipes&&file_output_flag==1)
                file_fd=out_to_file(argp[ci+1][0]);
    
            else if(ci==npipes&&file_output_flag==2)
                file_fd=append_to_file(argp[ci+1][0]);
                    
        
        execute_builtins(argp[ci][0],argp[ci]);

        ci++;    
       
 


    }    
    
    
    
        }
    //restore shell stdin and stdout
    if(internal==-1 ||file_output_flag>0)restore_std_fd();      
     

    for( int k = 0; k < 2 * npipes; k++ ){
            close( pipefd[k] );
    }


    for(int b=0;b<=npipes;b++)
        waitfor(pid[b]); //waiting for last child process to exit(terminate)

    close(file_fd);////important was causing shell to hang
    return 0;

}

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