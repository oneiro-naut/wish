#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main()
{
    
    int pipe_fd[2] = {-1,-1};
    int EXIT_STAT = 0;

    pipe(pipe_fd);//creating pipe

    
    char* argv[]={"grep","Radiohead",NULL};
    
    int std_out_dup = dup(1);
    //printf("%d \n",std_out_dup);
    dup2(pipe_fd[1],1); // redirecting stdout to parent pipe's read end
    //close(pipe_fd[0]);//closing parent pipe's write end 

    printf("Alice in Chains\n");
    printf("Radiohead\n");
    printf("Dinosaur Jr.\n");
    printf("Pixies\n");
    printf("Soundgarden\n");
    printf("Porcupine Tree\n");
    printf("Pain of Salvation\n");
    fflush(stdout);
    
    pid_t pid = fork();
    if(pid == 0)//child process
    {
    close(pipe_fd[1]);
    dup2(pipe_fd[0],0); //redirecting grep's stdin to parent pipe's write end
    //close(pipe_fd[0]);
    dup2(std_out_dup,1); // restore stdout fd for parent process
    execvp(argv[0],argv);//executing grep command
    perror("\n");
    exit(-1);
    }
    
    
    else if(pid > 0)//parent process
    {
    
        int i;//closing pipes for parent
        
        for(i=0;i<2;i++)
           close(pipe_fd[i]);

        dup2(std_out_dup,1); // restore stdout fd for parent process

        do{ // waiting for grep to exit
            waitpid(pid,&EXIT_STAT,WUNTRACED);
        }while(!WIFEXITED(EXIT_STAT)&&!WIFSIGNALED(EXIT_STAT));
    }
    else ;//fork error
    //dup2(std_out_dup,1); // restore stdout fd for parent process
    


    return 0;
}
