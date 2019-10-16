#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>


//stackoverflow rules \m/

int main()
{

    /*/pipe is basically a buffer(basically a byte array) created by OS shared among a family of process
                read fd-> (=================)<-write fd
                pipe() syscall creates a pair of fd pointing to this pipe buffer(this buffer can be assumed a file too)  
     since fd are nothing but pointers(more precisely entries) to a buffer or file associated a process
     
     now fork() and execv() syscalls create child process
     but these child process will retain the fd of the original meaning they will also point to the same file/buffer
        parent:     read fd->(   pipe    )<-write fd
        child1:     read fd->(           )<-write fd   
        child2:     read fd->(   buffer  )<-write fd

    
    
    */   
    
    //anyway this prgram is a very umm shell like program
    int npipes=1; // number of pipes
    int pipefd[2*npipes];
    int a = 0;
    for( ;a < npipes; a++ ){    //creating pipes
    if( pipe(pipefd + a*2) < 0 ){
        perror("Pipe not created ");
        exit(666);
    }
}    

    int EXIT_STAT;
    int i=0;//command counter
    //pipe(pipefd);
    pid_t pid[]={-1,-1};
    //int number_of_cmd=2;//no need to hardcode 
    char* cmd[]={"lspci",NULL,"tail","-3",NULL};
    char** argp[]={cmd,cmd+2,NULL};
    //char ** argp1 = NULL, **argp2 = NULL;
    
        //parent function calls here
        while(argp[i]){
        //if(!argp[i])i++;
        //argp1=argp+i;
        pid[i]=fork();
        //if(pid1)pid_t pid2=fork();
        if( pid[i]==0 )
        {
        //child function calls here
        if(i){
            //setbuf(stdout, NULL);
            printf("not cmd 1:%s i=%d\n",*argp[i],i);
            //close(pipefd[1]);
            dup2(pipefd[(i-1)*2],0);
            //close(pipefd[0]);
            
            //write(0,"\n",1);    
            //close(pipefd[0]);
            //close(0);
            //dup2(pipefd[1],1);
            //printf("going to execute\n");
            //execvp(argp[i],(argp+i));
            //write(pipefd[1],"\n",1);
            //exit(0);
            //close(fd[1]);
        }
        //open()
        if(i<2) {//close(pipefd[0]);
        //close(pipefd[0]);
        printf("not the last cmd:%s i=%d\n",*argp[i],i);
        dup2(pipefd[i*2+1],1);
        //close(pipefd[1]);
        }
            for( int j = 0; j < 2 * npipes; j++ ){
                close( pipefd[j] );
                }
        //close(pipefd[0]);
            execvp(*argp[i],argp[i]);
        
        //exit(0);
            }
        i++;
        printf("inside parent:i=%d\n",i);
    
    }    
        


//            close(pipefd[0]);
 //           close(pipefd[1]);
        //waitpid(pid,&EXIT_STAT,WUNTRACED);
        
        //while(argp[i])i++;
        //argp2 = argp+i+1;
        
               
        //dup2(1,pipefd[1]);
        //fprintf(stdout,"%s");

    for( int k = 0; k < 2 * npipes; k++ ){
            close( pipefd[k] );
    }
            for(int b=0;b<3;b++)
            {
                    do{//waiting for last to exit
                    waitpid(pid[b],&EXIT_STAT,WUNTRACED);
                    }while(!WIFEXITED(EXIT_STAT)&&WIFSIGNALED(EXIT_STAT));
                    printf("cmd[%d] dead\n",b);
            }   
    printf("Parent dead!\n");




    
    
    
    
    
    
    
    return 0;
}