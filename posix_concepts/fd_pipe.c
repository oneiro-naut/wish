#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<fcntl.h> 
#include <errno.h>
#include <sys/wait.h>

#define PIPE 1
#define WRITE 2
#define APPEND 3

//stackoverflow rules \m/


int executePipe(char*** argp,int npipes,int outputtoafile)
{

     // number of pipes
    printf("%d number of pipes inside exep func..\n",npipes);
	printf("%d output to file inside exep func..\n",outputtoafile);
	int pipefd[2*npipes];
    int a = 0;
    for( ;a < npipes; a++ ){    //creating pipes
    if( pipe(pipefd + a*2) < 0 ){
        perror("Pipe not created ");
        exit(666);
        }
    }    
    
    int filedescriptor = 0;//
    int EXIT_STAT;
    int ci=0;//command counter
    //pipe(pipefd);

    pid_t pid[npipes+1];
    for (int i=0;i<=npipes;i++)pid[i]=-1;
    //int number_of_cmd=2;//no need to hardcode 
    //char* cmd[]={"lspci",NULL,"tail","-3",NULL,"evilfile",NULL};
    //char** argp[]={cmd,cmd+2,cmd+5,NULL};
    //char ** argp1 = NULL, **argp2 = NULL;
    
        //parent function calls here
        while(argp[ci]&&ci<=npipes){
    
        pid[ci]=fork();
        //if(pid1)pid_t pid2=fork();
        if( pid[ci]==0 )
        {
        //child function calls here

        if(ci){
            
            printf("not cmd 1:%s i=%d\n",*argp[ci],ci);
            
            dup2(pipefd[(ci-1)*2],0);
            
        }
        
        if(ci<=npipes) {
        
        
        dup2(pipefd[ci*2+1],1);
        
        }
        if(ci==npipes&&outputtoafile==1)
        {
            filedescriptor= open(argp[ci+1][0],O_WRONLY|O_CREAT); 
            dup2(filedescriptor,1);
        }        

            for( int j = 0; j < 2 * npipes; j++ ){
                close( pipefd[j] );
                }
        
            
			execvp(*argp[ci],argp[ci]);
        
        
            }
        ci++;
        printf("inside parent:i=%d\n",ci);
    
    }    
        



    for( int k = 0; k < 2 * npipes; k++ ){
            close( pipefd[k] );
    }
            for(int b=0;b<=npipes;b++)
            {
                    do{//waiting for last to exit
                    waitpid(pid[b],&EXIT_STAT,WUNTRACED);
                    }while(!WIFEXITED(EXIT_STAT)&&WIFSIGNALED(EXIT_STAT));
                    printf("cmd[%d] dead\n",b);
            }   
    printf("Parent dead!\n");

    return 0;

}



int countPipes(char * str,int* fileoptflag)
{
    int i, j, npipes,napp,nwrite,foundPipe,foundAppend,foundwritef;
    char* pipestr="|"; 
	char* append_sym=">>";
	char* write_sym=">";
    int io_pushed = 0;

	*fileoptflag=0;//by default 0

    npipes = 0;
	napp =0;
	nwrite =0; //always FUUCCKKIN INITIALIZE YOUR VARIABLES BEFORE U START TESTING !!!!!!
	i=0; //added these init later ;/ thats why i m pissed
	j=0;
    while(str[i])
    {
        /* Match word with string */
        foundPipe = 1;
        foundAppend = 1;
        foundwritef = 1;
        while(pipestr[j]&&str[i+j])
        {
            if(str[i + j] != pipestr[j])
            {
                foundPipe = 0;
                break;
            }
            j++;
        }
        j=0;
        while(write_sym[j]&&str[i+j])
        {
            if(str[i + j] != write_sym[j])
            {
                foundwritef = 0;
                break;
            }
            j++;
        }
        j=0;
        while(append_sym[j]&&str[i+j])
        {
            if(str[i + j] != append_sym[j])
            {
                foundAppend = 0;
                break;
            }
            j++;
        }
        j=0;
        if(foundAppend==1)foundwritef= 0;



        if(foundPipe == 1)
        {
            if(io_pushed>0)return -1;
            //push(stk,PIPE);
            npipes++;
        }
        if(foundAppend == 1)
        {
            io_pushed++;
            //push(stk,APPEND);
            napp++;
        }
        if(foundwritef == 1)
        {
            //push(stk,WRITE);
            io_pushed++;
            nwrite++;
        }
        i++;
    }
    if(napp>1||nwrite>1)return -1;
    if(napp&&nwrite)return -1;
	if(napp||nwrite)*fileoptflag =1;
	
	return npipes;
}







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
    int outputtoafile= 0;//inactive by default 
    //                      but it works 
    int filedescriptor = 0;//
    int EXIT_STAT;
    int i=0;//command counter
    //pipe(pipefd);
    pid_t pid[]={-1,-1};
    //int number_of_cmd=2;//no need to hardcode 
    char* cmd[]={"lspci",NULL,"tail","-3",NULL,"evilfile",NULL};
    char** argp[]={cmd,cmd+2,cmd+5,NULL};
    executePipe(argp,npipes,outputtoafile);
    
    return 0;
}



