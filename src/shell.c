#include "../include/readline.h"
#include <dirent.h> //for directory related syscalls
#include "../include/stack.h"
#include <signal.h>
#include <time.h>
#include "../include/execute.h"
#include "../include/w_parser.h"


#define INPUTSIZE 1000//sufficient length
#define CMDPERINPUT 10	//sufficient size of cmd queue array
#define PATHLEN 1000 
#define DIRSTCKSIZE 20  //
#define TOKSIZE 100


char *host_name;
char *user_name;
extern char** environ;
//char* envp[]={"/bin:/usr/bin/"};
char** CMD[10];//command queue array basically array of argv[]
char* stream;
pid_t shell_PID;
int EXIT_STAT;
char PWD[1000];
STACK DIRSTACK;


//******************************************** Function Declarations ********************************************************************
int wish_init();
void shell_loop();
void printPrompt();
void list_files(char *directory);
void changedir(char** argv);
void sighandler(int);

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
   struct sigaction act;//must not be global since it belongs only to parent process

   memset(&act, 0, sizeof(act));
   act.sa_handler = sighandler;
    act.sa_flags=0;//not SA_RESTART
   sigaction(SIGINT,  &act, 0);//should work only for parent process 
   sigaction(SIGTSTP, &act, 0);//
 
    wish_init(); //initialize globals
    //getprompt();
    shell_loop();
    
    return 0;
}

void sighandler(int sig_num) 
{ 
    
     //sleep(100);
    //fflush(stdout); 
} 

int wish_init()
{
    getcwd(PWD,PATHLEN);

    init_stack(&DIRSTACK);

    initreadLine(INPUTSIZE);
    
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

void printPrompt()
{
        printf("\033[0;31m"); 
    	printf("\033[1m");
    	printf("%s  ➜  ",host_name);
    	printf("\033[0m");
	    printf("\033[0m"); 

}


void shell_loop()
{

    wish_init();     
     
    cmd_struct* current_cmd_struct = NULL;

    while(1){
        /* code */
        printPrompt();   

        stream=readLine();
        //parse function here
        current_cmd_struct = parse_input(stream);
        execute_cmd_struct(current_cmd_struct);
        free_cmd_struct(&current_cmd_struct);
        if(stream!=NULL){free(stream); //free cant free NULL lol
        stream= NULL;}
    
    }
}




// void list_files(char *directory){

//     struct dirent *di;
    
//     DIR *dr = directory==NULL  ? opendir(".") : opendir(directory);

//     if(dr == NULL){
//     printf("could not open the directory / directory not present");
//     return;
//     }


//     while((di = readdir(dr)) != NULL){
//         puts(di->d_name);
//     }
    


// }

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
            push(&DIRSTACK,PWD);
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
            push(&DIRSTACK,PWD);
            //push(&DIRSTACK,PWD);
            //getcwd(PWD,PATHLEN);
        }
        else if(argv[1][0]=='~')
        {
            push(&DIRSTACK,PWD);
            dir_found=chdir(getenv("HOME"));
            //push(&DIRSTACK,PWD);
            //getcwd(PWD,PATHLEN);
        }
        else if(argv[1][0]=='/')
        {
            push(&DIRSTACK,PWD);
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
            push(&DIRSTACK,PWD);
            strcat(PWD,"/");
            strcat(PWD,argv[1]);
            dir_found=chdir(PWD);
        }
    }
    if(dir_found==-1){
        perror("Error:");
    }
    else if(dir_found==0){
        
        getcwd(PWD,PATHLEN);
    }
    
    

}

