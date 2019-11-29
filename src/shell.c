//******************************************** Header Files Inclusions ********************************************************************

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h> //for directory related syscalls
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h> 
#include <termios.h>
//#include"wishparser.h"
#include "stack.h"
#include <signal.h>
//#include"queue.h"

//******************************************************************************************************************************************
//#define PIPE 1
//#define WRITE 2
//#define APPEND 3


#define INPUTSIZE 1000//sufficient length
#define CMDPERINPUT 10	//sufficient size of cmd queue array
#define PATHLEN 1000 
#define DIRSTCKSIZE 20  //



char *host_name;
char *user_name;
extern char** environ;
//char* envp[]={"/bin:/usr/bin/"};
char** CMD[10];//command queue array basically array of argv[]
char stream[INPUTSIZE];
pid_t shell_PID;
int EXIT_STAT;
char PWD[1000];
STACK DIRSTACK;


//******************************************** Function Declarations ********************************************************************
int wish_init();
void shell_loop();
int get_stream();
int checkTokens(char **argv);
void list_files(char *directory);
char** w_tokenizer(char* inp_str,char* deli);
void changedir(char** argv);
void sigintHandler(int);
int countPipes(char * str,int* fileoptflag);
int executePipe(char*** argp,int npipes,int outputtoafile);
//******************************************************************************************************************************************

/*void getprompt()
{
    // Retrieving hostname from /etc/hostaname file
    FILE *file = fopen("/etc/hostname","r");
    host_name=(char *)malloc(15*sizeof(char));
    if(!file)
    exit(0);
    fscanf(file,"%s",host_name);
    fclose(file);

    // Retrieving username using getlogin() fron <unistd.h>
    user_name=(char *)malloc(10*sizeof(char));
    user_name=getlogin();
}*/



int main()
{
  //  printf("Welcome to wish shell !\n");
    //a function to retreive hostname and user 
    

    wish_init();
    //getprompt();
    shell_loop();

}

void sigintHandler(int sig_num) 
{ 
    signal(SIGINT, sigintHandler); 
    fflush(stdout); 
} 

int wish_init()
{
    getcwd(PWD,PATHLEN);

    init_stack(&DIRSTACK);
    
    // Retrieving hostname from /etc/hostaname file
    FILE *file = fopen("/etc/hostname","r");
    host_name=(char *)malloc(15*sizeof(char));
    if(!file)
    exit(0);
    fscanf(file,"%s",host_name);
    fclose(file);

    // Retrieving username using getlogin() fron <unistd.h>
    user_name=(char *)malloc(10*sizeof(char));
    user_name=getlogin();
}




void shell_loop()
{

    int length;

    char* sep_primary="&&;";//has problem since it will tokenize single & too which is not primary sep
    char* sep_io="|&>><";//actually all delimiters have problems since all p n c are considered while tokenizing
	char* cmddeli=" ";//whitespace
    
    char** argp[10];
    int syntax_correct =1; // by default correct
    wish_init();    
    
        signal(SIGINT, sigintHandler); 
     
    
    while(1){
        /* code */
        printf("\033[0;31m"); 
    	printf("\033[1m");
    	printf("%s  ➜  ",host_name);
    	printf("\033[0m");
	    printf("\033[0m"); 
    //getting user input 


        //getting user input 
        get_stream();
		int npipes=0;
		int optfile = 0;
		
        //tokenization of user input
        int cmdcount = 0;
	//	printf("creating complex cmd arr...\n");
        char** complex_cmd_arr=w_tokenizer(stream,sep_primary);
		char** complex_cmd_component_cmd_arr;
       // printf("created complex cmd arr\n");
		//basically "ls | grep "something"| tail -5 ","ping google.com -c 3|grep "google"|head -3",null
		//separate pipe chains
		while (complex_cmd_arr[cmdcount])
        {
            int y = 0;
            syntax_correct = 1;
		//	printf("%s \n",complex_cmd_arr[cmdcount]);	
			npipes=countPipes(complex_cmd_arr[cmdcount],&optfile);
			//printf("number of pipes =%d\n",npipes);
			//calculate number of pipes and stuff here
			//also lots of free() calls r to be added
			//printf("creating pipe n io separated sub cmd arr...\n");
			//basically "ping -c 3","grep "google" ","head -3",null
			//individual cmds with their arguments in a pipe chain
			complex_cmd_component_cmd_arr=w_tokenizer(complex_cmd_arr[cmdcount],sep_io);
			while (complex_cmd_component_cmd_arr[y])
        	{
            	argp[y]=w_tokenizer(complex_cmd_component_cmd_arr[y],cmddeli);
				//cmd broken into name,arg1,arg2,...,null format tokens
				//can be used as argv
				//argp contains ptrs to various argvs
				//their offset is returned by w_tokenizer
				//example "ping","google.com","-c","3",null
				//w_tokenizer returns &argv[0] which is a double ptr
                //argp has format argv1,argv2,argv3,argv3,...null
				//printf("creating argp[]\n");
				y++;
        	}free(complex_cmd_component_cmd_arr);
			argp[y]=NULL;    //making argp NULL terminated
			y=0;
            
            for(int i=0;argp[i]!=NULL;i++){
                if(checkTokens(argp[i])==-1)
                {
                    syntax_correct = 0;
                    break;
                }
            }
            
            if(syntax_correct==1){
                
			    if(!strcmp(argp[y][0],"exit")){
                    // printf("Yippikaya Mr Falcon\n");
                    exit(0);
                }
                else if(!strcmp(argp[y][0],"cd")){
                    changedir(argp[y]);

                }
                else executePipe(argp,npipes,optfile);
            }
            else if(syntax_correct==0){perror("wrong syntax!\n"); }
			//printf("creating complex_cmd_component_arr[]\n");
		
        	cmdcount++;
            for(int i=0;argp[i]!=NULL;i++)free(argp[i]);
        
        } free(complex_cmd_arr);
        
	

    }
}




int get_stream(){
	//this function gets user input  
	
    fgets(stream,INPUTSIZE,stdin);//input overflow needs to be handled
    int length = strlen(stream);
    stream[length-1] = '\0'; //removing \n character from input
//	printf("exiting get_stream()\n");
    return 0;
}


//tokenizer returns argv now**

char** w_tokenizer(char* inp_str,char* deli) //make a different tok function for separator that does not tokenize double quotes
{
    char** tokarr =(char**) malloc(sizeof(char*)*CMDPERINPUT);
	int length =strlen(stream);
	char* curr=inp_str;
	char* temp;
	char* tok;

	//char* del=" |;&&><";
    
	int index=-1;
	int err=-1;

	do
	{
		while(*(curr) == ' '){
			curr++;
		}
		
		if(*(curr)!='\"'){
			if(index==-1)tok=strtok_r(inp_str,deli,&curr);
			else tok=strtok_r(NULL,deli,&curr);
			index++;
			tokarr[index]=tok;
		}
		//else curr++;

		else
		{
			//int i = 0;
			temp=curr+1;
			do{
				curr++;
				
			}while((*curr)!='\"'&&(*curr)!='\0');

			index++;
			if(*curr=='\"'){
				*curr='\0';
				//index++;
				tokarr[index]=temp;
				curr++;
			}
			else {
				err++;
				tokarr[index]=NULL;
			}
			
		}
	}while(tokarr[index]);
	
	// int i=0;
	// printf("\n");
	// for(;tokarr[i]!=NULL;i++)puts(tokarr[i]);
    // if(!tokarr[i])printf("NULL ");
	// printf("\n");

    return tokarr;
}




int checkTokens(char **argv){

    /*
    
    argv[0] = . / _ / alphabet /  
    
    */
   int i = 0 ;
   if(argv[0][0] != '.' && argv[0][0] != '_' && !isalpha(argv[0][0]) ){
       return -1;
   }
   /*while(argv[i] != NULL && isCMDseparator(argv[i][0])){
        printf("scanning done...\n");    
   }*/
   
   return 0;

   
}


void list_files(char *directory){

    struct dirent *di;
    
    // if(directory != NULL){
    //     if(*directory == '~'){
    //         directory = strcat("/home/eldraco",(directory+1));
    //     }
    // }

    DIR *dr = directory==NULL  ? opendir(".") : opendir(directory);

    if(dr == NULL){
    printf("could not open the directory / directory not present");
    return;
    }


    while((di = readdir(dr)) != NULL){
        puts(di->d_name);
    }
    


}

void changedir(char **argv)
{
    //char *temp;
    int dir_found=2;
    if(strlen(argv[1])>1){
       if(argv[1][0]=='/')
        {
            
            dir_found=chdir(argv[1]);
            //getcwd(PWD,PATHLEN);
        }
        else
        {
            //push(&DIRSTACK,PWD);
            strcat(PWD,"/");
            strcat(PWD,argv[1]);
            dir_found=chdir(PWD);
        }
         //dir_found=chdir(argv[1]);

    }
    else if(strlen(argv[1])==1){
        if(argv[1][0]=='-')
        {
        
            if(DIRSTACK.top!=-1)dir_found=chdir(pop(&DIRSTACK));
            else printf("No previous working directory!\n");
            //push(&DIRSTACK,PWD);
            //getcwd(PWD,PATHLEN);
        }
        else if(argv[1][0]=='~')
        {
            dir_found=chdir(getenv("HOME"));
            //push(&DIRSTACK,PWD);
            //getcwd(PWD,PATHLEN);
        }
        else if(argv[1][0]=='/')
        {
            dir_found=chdir(argv[1]);
            //push(&DIRSTACK,PWD);
            //getcwd(PWD,PATHLEN);
        }
        else if(argv[1][0]=='.')
        {
            //push(&DIRSTACK,PWD);
            //strcat(PWD,"/");
            //strcat(PWD,argv[1]);
            //dir_found=chdir(PWD);
            //getcwd(PWD,PATHLEN);
        }
        else {
            //dir_found=chdir(argv[1]);
            strcat(PWD,"/");
            strcat(PWD,argv[1]);
            dir_found=chdir(PWD);
        }
    }
    if(dir_found==-1){
        perror("Error:");
    }
    else if(dir_found==0){
        push(&DIRSTACK,PWD);
        getcwd(PWD,PATHLEN);
    }
    
    

}


int executePipe(char*** argp,int npipes,int outputtoafile)
{

     // number of pipes
    if(npipes==-1){
		perror("Syntax Error!");
		return -1;
	}
	//printf("%d number of pipes inside exep func..\n",npipes);
	//printf("%d output to file inside exep func..\n",outputtoafile);
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
            
         //   printf("not cmd 1:%s i=%d\n",*argp[ci],ci);
            
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
        if(ci==npipes&&outputtoafile==2)
        {
            filedescriptor= open(argp[ci+1][0],O_WRONLY|O_CREAT|O_APPEND); 
            dup2(filedescriptor,1);
        }        


            for( int j = 0; j < 2 * npipes; j++ ){
                close( pipefd[j] );
                }
        
            
			execvp(*argp[ci],argp[ci]);
			perror("command not found!\n");
			exit(0);
        
            }
        ci++;
       // printf("inside parent:i=%d\n",ci);
    
    }    
        



    for( int k = 0; k < 2 * npipes; k++ ){
            close( pipefd[k] );
    }
            for(int b=0;b<=npipes;b++)
            {
                    do{//waiting for last to exit
                    waitpid(pid[b],&EXIT_STAT,WUNTRACED);
                    }while(!WIFEXITED(EXIT_STAT)&&WIFSIGNALED(EXIT_STAT));
                   // printf("cmd[%d] dead\n",b);
            }   
   // printf("Parent dead!\n");

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
        if(foundAppend==1)
		{
			foundwritef= 0;
			i=i+2;
		}


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
   // printf("napp=%d\n",napp);
//	printf("nwrite=%d\n",nwrite);
	
	if(napp>1||nwrite>1)return -1;
    if(napp&&nwrite)return -1;
	if(napp)*fileoptflag =2;
	if(nwrite)*fileoptflag =1;
	
	return npipes;
}





























