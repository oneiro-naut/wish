#include "w_parser.h"
#include "w_tokenizer.h"
#include "cmd_struct.h"
#include <stdlib.h>
#include <ctype.h>//isalpha(char)

#define MAXCMDS 20
#define MAXPIPELINES 10

static int checkTokens(char **argv){

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




static int countPipes(char * str,int* fileoptflag)// can be implemented using a FSM
{
    int i, j, npipes,napp,nwrite,foundPipe,foundAppend,foundwritef;
    char* pipestr="|"; 
	char* append_sym=">>";
	char* write_sym=">";
    int io_pushed = 0;

	*fileoptflag=0;//by default 0

    npipes = 0;
	napp =0;
	nwrite =0; //always F****N INITIALIZE YOUR VARIABLES BEFORE U START TESTING !!!!!!
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

	
	if(napp>1||nwrite>1)return -1;
    if(napp&&nwrite)return -1;
	if(napp)*fileoptflag =2;
	if(nwrite)*fileoptflag =1;
	
	return npipes;
}

//needs immediate testing**

cmd_struct* parse_input(char* stream)
{
        int length;
        cmd_struct* cmd = NULL;
        init_cmd_struct(&cmd);
        char* sep_primary="&&;";//has problem since it will tokenize single & too which is not primary sep
        char* sep_io="|&>><";//actually all delimiters have problems since all p n c are considered while tokenizing
        char*** argp=NULL;
        

        
        int syntax_correct =1; // by default correct
        
        int npipes=0;
		int optfile = 0;
		
        //tokenization of user input
        int cmdcount = 0;//no of pipelines
	
        char** complex_cmd_arr= wish_tok_quote_muted(stream);
		char** complex_cmd_component_cmd_arr;
       
       while (complex_cmd_arr[cmdcount])
        {
            argp = (char ***)malloc(sizeof(char**)*MAXCMDS);
            int y = 0;
            syntax_correct = 1;

			cmd->n_pipes[cmdcount]=countPipes(complex_cmd_arr[cmdcount],&(cmd->file_out[cmdcount]));
			
            complex_cmd_component_cmd_arr=w_tokenizer(complex_cmd_arr[cmdcount],sep_io,0);
			
            while (complex_cmd_component_cmd_arr[y])
        	{
            	argp[y]=wish_tok(complex_cmd_component_cmd_arr[y]);
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
            
            cmd->argp_arr[cmdcount]= argp;
            argp = NULL;        
            cmdcount++;
        } free(complex_cmd_arr);
        cmd->argp_arr[cmdcount]=NULL; // making argp_arr NULL terminated

    
    
    return cmd;

}