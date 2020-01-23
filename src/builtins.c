#include "../include/builtins.h"
#include "../include/stack.h"
#include "../include/w_env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

void changedir(char **argv)
{
    //char *temp;
    int dir_found=2;
    if(argv[1]==NULL)//cd 
    {
        push(&DIRSTACK,PWD);
        dir_found=chdir(getenv("HOME"));
            
    }
    else if(strlen(argv[1])>1){
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
        perror("\n");
    }
    else if(dir_found==0){
        
        getcwd(PWD,PATHLEN);
    }
    
    

}


