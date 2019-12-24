//******************************************** Header Files Inclusions ********************************************************************

#include <unistd.h>
#include "readline.h"
#include <dirent.h> //for directory related syscalls
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include "stack.h"
#include <signal.h>
#include <time.h>
//******************************************************************************************************************************************


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
int checkTokens(char **argv);
void list_files(char *directory);
char** w_tokenizer(char* inp_str,char* deli,int dquote);
char** wish_tok_quote_muted(char* inp_str);
char** wish_tok(char* inp_str);
void changedir(char** argv);
void sighandler(int);
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

    int length;

    char* sep_primary="&&;";//has problem since it will tokenize single & too which is not primary sep
    char* sep_io="|&>><";//actually all delimiters have problems since all p n c are considered while tokenizing
	//char* cmddeli=" ";//whitespace
    
    char** argp[10];
    int syntax_correct =1; // by default correct
    wish_init();     
     
    
    while(1){
        /* code */
        printPrompt();   
    //getting user input 


        //getting user input 
//        get_stream();
        stream=readLine();
		//stream=NULL;//NULL check
        int npipes=0;
		int optfile = 0;
		
        //tokenization of user input
        int cmdcount = 0;
	//	printf("creating complex cmd arr...\n");
        char** complex_cmd_arr= wish_tok_quote_muted(stream);
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
//		printf("number of pipes =%d\n",npipes);
			//calculate number of pipes and stuff here
			//also lots of free() calls r to be added
			//printf("creating pipe n io separated sub cmd arr...\n");
			//basically "ping -c 3","grep "google" ","head -3",null
			//individual cmds with their arguments in a pipe chain
			complex_cmd_component_cmd_arr=w_tokenizer(complex_cmd_arr[cmdcount],sep_io,0);
			while (complex_cmd_component_cmd_arr[y])
        	{
            	argp[y]=wish_tok(complex_cmd_component_cmd_arr[y]);
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
        if(stream!=NULL){free(stream); //free cant free NULL lol
        stream= NULL;}
    }
}

char** wish_tok(char* inp_str)
{
    char** tokarr =(char**) malloc(sizeof(char*)*CMDPERINPUT);
   // if(inp_str==NULL){tokarr[] return tokarr;
   // if(*inp_str==0)return NULL;
	char *p=inp_str;//so that original buffer is not affected
	if(p==NULL){ tokarr[0]=NULL; return tokarr;}
	char* start_of_word=NULL;
	int index_of_token= -1;
	int c;
	enum states { DULL, ALT_DULL, IN_WORD, IN_STRINGD, IN_STRING, ERROR, EXIT } state = DULL;

		for(;state!=EXIT && state!=ERROR;p++) {
			c = (unsigned char) *p; /* convert to unsigned char for is* functions */
			switch (state) {
			case DULL: /* not in a word, not in a double quoted string */
				if(c==0){ state=EXIT; continue;}
				if (c==' ') {
					/* still not in a word, so ignore this char */
					continue;
				}
				/* not a space -- if it's a double quote we go to IN_STRING, else to IN_WORD */
				if (c == '"') {
					state = IN_STRINGD;
					start_of_word = p + 1; /* word starts at *next* char, not this one */
					index_of_token++;
					continue;
				}
				if (c == '\'') {
					state = IN_STRING;
					start_of_word = p + 1; /* word starts at *next* char, not this one */
					index_of_token++;
					continue;
				}
				state = IN_WORD;
				start_of_word = p; /* word starts here */
				index_of_token++;
				continue;

			case ALT_DULL: /* not in a word, not in a double quoted string */
				if(c==0){ state=EXIT; continue;}
				if (c==' ') {
					/* still not in a word, so ignore this char */
					state=DULL;
					continue;
				}
				/* not a space -- if it's a double quote we go to IN_STRING, else to IN_WORD */
				if (c == '"') {
					state = IN_STRINGD;
					start_of_word = p + 1; /* word starts at *next* char, not this one */
					index_of_token++;
					continue;
				}
				if (c == '\'') {
					state = IN_STRING;
					start_of_word = p + 1; /* word starts at *next* char, not this one */
					index_of_token++;
					continue;
				}
				state = ERROR;
				continue;


			case IN_STRINGD:
				/* we're in a double quoted string, so keep going until we hit a close " */
				
				if(c==0){ state = ERROR;  }
				if (c == '"') {
					/* word goes from start_of_word to p-1 */
					*p= 0;
					tokarr[index_of_token]=start_of_word;
					state = ALT_DULL; /* back to "not in word, not in string" state */
				}
				continue; /* either still IN_STRING or we handled the end above */

			case IN_STRING:
				/* we're in a single quoted string, so keep going until we hit a close " */
				
				if(c==0){ state = ERROR;  }
				if (c == '\'') {
					/* word goes from start_of_word to p-1 */
					*p= 0;
					tokarr[index_of_token]=start_of_word;
					state = ALT_DULL; /* back to "not in word, not in string" state */
				}
				continue; /* either still IN_STRING or we handled the end above */



			case IN_WORD:
				/* we're in a word, so keep going until we get to a space */
				if(c==0){ 
					state=EXIT;
					tokarr[index_of_token]=start_of_word;
					
					}
				if (c==' ') {
					/* word goes from start_of_word to p-1 */
					
					state = DULL; /* back to "not in word, not in string" state */
					*p= '\0';
					
					tokarr[index_of_token]=start_of_word;
				}
				if(c=='\'' || c=='"'){ state=ERROR;}

				continue; /* either still IN_WORD or we handled the end above */
			

			}
		}
		//2 possible final states of our automata
		if(state==EXIT)tokarr[++index_of_token]=NULL;  //case EXIT:
		else if(state==ERROR)
		{
			printf("input error related to quotes occured\n"); //case ERROR: 
			tokarr[++index_of_token]=NULL;
		}
		    

 	//  int i=0;
 	//  printf("\n");
 	//  for(;tokarr[i]!=NULL;i++)puts(tokarr[i]); //for debugging purpose
    //   if(!tokarr[i])printf("NULL ");
 	//  printf("\n");			
    
	return tokarr;
}


//tokenizer below does not tokenize the delimitor if its inside a balanced double quoted substring
//more delimitors can be added by adding more if conditions 
char** wish_tok_quote_muted(char* inp_str) 
{
    
    //if(*inp_str==0)return NULL;
    char** tokarr =(char**) malloc(sizeof(char*)*CMDPERINPUT);
	if(inp_str==NULL){ tokarr[0]=NULL; return tokarr;}
    char *p=strdup(inp_str);//so that original buffer is not affected
	
	
	char* start_of_word=NULL;
	int index_of_token= -1;
	int c;
	enum states { DULL, IN_WORD, IN_STRING, IN_STRINGD, EXIT, ERROR } state = DULL;

		for (;state!=EXIT && state!=ERROR;p++) {
			c = (unsigned char) *p; /* convert to unsigned char for is* functions */
			switch (state) {
			case DULL: /* not in a word, not in a double quoted string */
				if(c==0){ state=EXIT; continue;}
				if (c==';') {
					/* still not in a word, so ignore this char */
					continue;
				}
				/* not a space -- if it's a double quote we go to IN_STRING, else to IN_WORD */
				if (c == '"') {
					state = IN_STRING;
					//start_of_word = p + 1; /* word starts at *next* char, not this one */
					//index_of_token++;
					continue;
				}
				state = IN_WORD;
				start_of_word = p; /* word starts here */
				index_of_token++;
				continue;

			case IN_STRINGD:
				/* we're in a double quoted string, so keep going until we hit a close " */
				if(c==0){ 
					state=ERROR;
					tokarr[index_of_token]=start_of_word;
					
					}				
				if (c == '"') {
					/* word goes from start_of_word to p-1 */
					// unlike the prev version this IN_STRING state does nothing just skips tokenizing process
					//until it finds that its finally going to next non STRING state
					state = IN_WORD; /* back to "not in word, not in string" state */
				}
				continue; /* either still IN_STRING or we handled the end above */

			case IN_STRING:
				/* we're in a double quoted string, so keep going until we hit a close " */
				if(c==0){ 
					state=ERROR;
					tokarr[index_of_token]=start_of_word;
					
					}
				if (c == '\'') {
					/* word goes from start_of_word to p-1 */
					// unlike the prev version this IN_STRING state does nothing just skips tokenizing process
					//until it finds that its finally going to next non STRING state
					state = IN_WORD; /* back to "not in word, not in string" state */
				}
				continue; /* either still IN_STRING or we handled the end above */



			case IN_WORD:
				/* we're in a word, so keep going until we get to a space or our delimiter char */
				if(c==0){ 
					state=EXIT;
					tokarr[index_of_token]=start_of_word;
					
					}				
				if (c==';') {
					
					
					state = DULL; 
					*p= '\0';
					
					tokarr[index_of_token]=start_of_word;
				}
				if(c=='"')
				{
					state=IN_STRINGD;

				}
				if(c=='\'')
				{
					state=IN_STRING;
				}
				continue; /* either still IN_WORD or we handled the end above */

			}
		}
		//exit state check here sort of pythonic while-else construct
		if(state==EXIT)tokarr[++index_of_token]=NULL;  //case EXIT:
		else if(state==ERROR)
		{
			printf("input error related to quotes occured\n"); //case ERROR: 
			tokarr[++index_of_token]=NULL;
		}

 	//  int i=0;
 	//  printf("\n");
 	//  for(;tokarr[i]!=NULL;i++)puts(tokarr[i]); //for debugging purpose
    //   if(!tokarr[i])printf("NULL ");
 	//  printf("\n");			
    
	return tokarr;
}







// int get_stream(){
// 	//this function gets user input  
	
//     fgets(stream,INPUTSIZE,stdin);//input overflow needs to be handled
//     int length = strlen(stream);
//     stream[length-1] = '\0'; //removing \n character from input
// //	printf("exiting get_stream()\n");
//     return 0;
// }


//tokenizer returns argv now**

char** w_tokenizer(char* inp_str,char* deli,int dquote) //tokenization of double quotes can now be turned off
{                                                   // *****cases like echo "some;thing" remain unhandled***
    char** tokarr =(char**) malloc(sizeof(char*)*CMDPERINPUT);
	int length =strlen(stream);
	char* curr=inp_str;
	char* temp;
	char* tok;
    if(curr==NULL){ tokarr[0]=NULL; return tokarr;}
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
			//printf("@char= <%c> \n",*temp);
            do{
				curr++;
				
			}while((*curr)!='\"'&&(*curr)!='\0');

			index++;
			if(*curr=='\"'&&dquote){
				*curr='\0';
				//index++;
				tokarr[index]=temp;
				curr++;
			}
			else if(*curr=='\"'&&dquote==0)
            {
                curr++; //dont tokenize just skip tokenizing whats between the quotes
            }
            else {
				err++;
				tokarr[index]=NULL;
			}
			
		}
	}while(tokarr[index]);
	
	//  int i=0;
	//  printf("\n");
	//  for(;tokarr[i]!=NULL;i++)puts(tokarr[i]); //for debugging purpose
    //  if(!tokarr[i])printf("NULL ");
	//  printf("\n");
    
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
        
        if(ci<npipes) {
        
        
        dup2(pipefd[ci*2+1],1);
        
        }
       
        if(ci==npipes&&outputtoafile==1)
        {
            filedescriptor= open(argp[ci+1][0],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); //added mode because of O_CREAT flag 
            dup2(filedescriptor,1);
        }                                           //S_IRWXU --> permissions are same as touch cmd to user who created file
                                                        // --rw-r--r-
        if(ci==npipes&&outputtoafile==2)
        {
            filedescriptor= open(argp[ci+1][0],O_WRONLY|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);//added mode because of O_CREAT flag 
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



int countPipes(char * str,int* fileoptflag)// can be implemented using a FSM
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
