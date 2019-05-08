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
//#include"wishparser.h"
#include"stack.h"
//#include"queue.h"

//******************************************************************************************************************************************


#define MAXBUFFERLEN 1000//sufficient length
#define MAXCMDSIZE 100	//sufficient size of cmd queue array
#define DIRSTCKSIZE 20  //
#define PATHLEN 1000  //

char *host_name;
char *user_name;
extern char** environ;
//char* envp[]={"/bin:/usr/bin/"};
char** CMD[10];//command queue array basically array of argv[]
char stream[MAXBUFFERLEN];
pid_t shell_PID;
int EXIT_STAT;
char PWD[1000];
STACK DIRSTACK;


//******************************************** Function Declarations ********************************************************************
int wish_init();
void shell_loop();
void get_stream();
void list_files(char *directory);
int w_tokenizer(char *stream,char **argv);
int scan0(char **argv);
int contains(char*,char);
int execpipe(char**,char**);
void changedir(char** argv);

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
    char **pipeargs1,**pipeargs2;
    int isPipe=0;
    
    char* argv[MAXCMDSIZE];
    //argv=(char **)malloc(100*sizeof(char*));

    
    // Printing promp which contains usename and hostname 
    
    while(1){
        /* code */
    printf("\033[0;31m"); 
    printf("\033[1m");
    printf("%s  ➜  ",host_name);
    printf("\033[0m");
    printf("\033[0m"); 
    //getting user input 
    get_stream();
    length = strlen(stream);
    stream[length-1] = '\0';
    
    
    //converting user input to tokens
    /*if(contains(stream,'|')){
        w_tokenizer(stream,argv,'|');
        w_tokenizer(argv[0],pipeargs1,' ');
        w_tokenizer(argv[1],pipeargs2,' ');
        isPipe = 1;

    }
    else
    */
    w_tokenizer(stream,argv);
    
    //checking syntax: a trivial check indeed
    if(scan0(argv)==-1)
    {
        printf("LEVEL0_SYNTAX_ERR");
    }

    //executing the COMMAND if it's syntactically correct
    
    
    else{
        
    //below are the BUILTIN SHELL COMMANDS******************************************************************************************
        /*if(isPipe){
            execpipe(pipeargs1,pipeargs2);
        }*/
        //printf("i m no here\n");
        if(!strcmp(argv[0],"exit")){
            printf("Yippikaya Mr Falcon\n");
            exit(0);
        }
 
        else if(!strcmp(argv[0],"ls")){
            list_files(argv[1]);       
        }
        else if(!strcmp(argv[0],"cd"))
        {
            changedir(argv);
        }
    
    
    //below are the EXTERNAL COMMANDS aka THE BINARIES*******************************************************************************
        else {
    	    shell_PID=fork();

            if(shell_PID==0){//CHILD PROCESS:THE BINARY OR THE COMMAND
                execvp(argv[0],argv);
                exit(0);
            }
            else if(shell_PID>0){//PARENT PROCESS:WISH SHELL
                do{
                    waitpid(shell_PID,&EXIT_STAT,WUNTRACED);
                    }while(!WIFEXITED(EXIT_STAT)&&WIFSIGNALED(EXIT_STAT));
            }
            else{
                perror("forking error!\n");
            }
        } 
    }
}

}
void get_stream(){
	//this function would read the string buffer 
	//will check if buffer has not exceeded the MAXBUFFERLEN that is 200 characters
	//if not exceeded it will return the char pointer
	//else it would return NULL pointer
    //stream=(char*)malloc(sizeof(char)*MAXBUFFERLEN);

    fgets(stream,MAXBUFFERLEN,stdin);
    
}

int contains(char* string , char regex){
    int i=0;
    for (;i<strlen(string);i++){
        if(regex==*(string+i))
        return 1;
    }
    return 0;

}

int execpipe (char ** argv1, char ** argv2) {
    int fds[2];
    pipe(fds);
    int i;
    pid_t pid = fork();
    if (pid == -1) { //error
        char *error = strerror(errno);
        printf("error fork!!\n");
        return 1;
    } 
    if (pid == 0) { // child process
        close(fds[1]);
        dup2(fds[0], 0);
        //close(fds[0]);
        execvp(argv2[0], argv2); // run command AFTER pipe character in userinput
        char *error = strerror(errno);
        printf("unknown command\n");
        return 0;
    } else { // parent process
        close(fds[0]);
        dup2(fds[1], 1);
        //close(fds[1]);
        execvp(argv1[0], argv1); // run command BEFORE pipe character in userinput
        char *error = strerror(errno);
        printf("unknown command\n");
        return 0;
    }
}

int w_tokenizer(char* stream,char** tokarr)
{
    
	int length =strlen(stream);
	char* curr=stream;
	char* temp;
	char* tok;

	const char* del=" |;&&><";

	int index=-1;
	int err=-1;

	do
	{
		while(*(curr) == ' '){
			curr++;
		}
		
		if(*(curr)!='\"'){
			if(index==-1)tok=strtok_r(stream,del,&curr);
			else tok=strtok_r(NULL,del,&curr);
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
	}while(tokarr[index]!=NULL);
	
	//int i=0;
	//for(;tokarr[i]!=NULL;i++)puts(tokarr[i]);
    
    return 0;
}

char isCMDseparator(char ch){
    
    if(ch != ';' && ch != '|' && ch != '>' && ch != '<'){
        return ch;
    }
    return 0;

}

int scan0(char **argv){

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
    int dir_found=0;
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
    }
    else if(strlen(argv[1])==1){
        if(argv[1][0]=='-')
        {
        
            if(DIRSTACK.top!=-1)dir_found=chdir(pop(&DIRSTACK));
            else printf("No previous working directory!\n");
            push(&DIRSTACK,PWD);
            getcwd(PWD,PATHLEN);
        }
        else if(argv[1][0]=='~')
        {
            dir_found=chdir(getenv("HOME"));
            push(&DIRSTACK,PWD);
            getcwd(PWD,PATHLEN);
        }
        else if(argv[1][0]=='/')
        {
            dir_found=chdir(argv[1]);
            push(&DIRSTACK,PWD);
            getcwd(PWD,PATHLEN);
        }
        else if(argv[1][0]=='.')
        {
            push(&DIRSTACK,PWD);
            strcat(PWD,"/");
            strcat(PWD,argv[1]);
            dir_found=chdir(PWD);
            getcwd(PWD,PATHLEN);
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
















