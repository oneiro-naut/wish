//******************************************** Header Files Inclusions ********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//******************************************************************************************************************************************


#define INPUTSIZE 1000//sufficient length
#define CMDPERINPUT 10	//sufficient size of cmd queue array

#define PATHLEN 1000  //

//char *host_name;
//char *user_name;
//extern char** environ;
//char* envp[]={"/bin:/usr/bin/"};
//char** CMD[10];//command queue array basically array of argv[]
char stream[INPUTSIZE];
//pid_t shell_PID;
//int EXIT_STAT;
//char PWD[1000];
//STACK DIRSTACK;


//******************************************** Function Declarations ********************************************************************
//int wish_init();
void shell_loop();
int get_stream();
//void list_files(char *directory);
char** w_tokenizer(char *stream,char* deli);
//int scan0(char **argv); commented out stuff not being analyzed
//int contains(char*,char);
//int execpipe(char**,char**);
//void changedir(char** argv);

//******************************************************************************************************************************************




int main()
{

    shell_loop();
}



void shell_loop()
{

    int length;

    char* sep_primary="&&;";//has problem since it will tokenize single & too which is not primary sep
    char* sep_io="|&>><";//actually all delimiters have problems since all p n c are considered while tokenizing
	char* cmddeli=" ";//whitespace
    
    char** argp[10];
    
    
     
    
    while(1){
        /* code */
        printf("TEST>>");
        //getting user input 
        get_stream();
		char** argp[10];
        //tokenization of user input
        int cmdcount=0;
		printf("before w_tokenizer()\n");
        char** complex_cmd_arr=w_tokenizer(stream,sep_primary);
		char** complex_cmd_component_cmd_arr;
        printf("after call 1 to w_tokenizer()\n");
		while (separated_cmd_arr[cmdcount])
        {
            int y=0;
			//calculate number of pipes and stuff here
			//also lots of free() calls r to be added
			complex_cmd_component_cmd_arr=w_tokenizer(separated_cmd_arr[cmdcount],sep_io);
			while (complex_cmd_component_cmd_arr[y])
        	{
            	argp[y]=w_tokenizer(complex_cmd_component_cmd_arr[y],cmddeli);
				printf("creating argp[]\n");
				y++;
        	}
			argp[cmdcount]=	NULL;//making argp NULL terminated

			printf("creating complex_cmd_component_arr[]\n");
			cmdcount++;
        }
		complex_cmd_component_cmd_arr[cmdcount]=NULL;//making cmd argp NULL terminated
        cmdcount=0;
		while (complex_cmd_component_cmd_arr[cmdcount])
        {
            argp[cmdcount]=w_tokenizer(complex_cmd_component_cmd_arr[cmdcount],cmddeli);
			printf("creating argp[]\n");
			cmdcount++;
        }
		argp[cmdcount]=	NULL;//making argp NULL terminated
		int i=0;
        int j=0;
        while(argp[i])
        {
            while(argp[i][j])
            {
                printf("argp[%d][%d]: %s\n",i,j,argp[i][j]);
                j++;
            }
            if(!argp[i][j])printf("argp[%d][%d]: NULL\n",i,j,argp[i][j]);
            i++;
			j=0;
            printf("\n");
        }

    }
}


int get_stream(){
	//this function gets user input  
	
    fgets(stream,INPUTSIZE,stdin);//input overflow needs to be handled
    int length = strlen(stream);
    stream[length-1] = '\0'; //removing \n character from input
	printf("exiting get_stream()\n");
    return 0;
}




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
	
	int i=0;
	for(;tokarr[i]!=NULL;i++)puts(tokarr[i]);
    if(!tokarr[i])printf("NULL ");
	printf("going to return from w_tokenizer()\n");
    return tokarr;
}


















