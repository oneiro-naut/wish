#include "../include/process.h"
#include <unistd.h>
#include <sys/wait.h>
//#include <stdio.h>

int waitfor(pid_t pid)
{
    int EXIT_STAT;
    do{
        waitpid(pid,&EXIT_STAT,WUNTRACED);
    }while(!WIFEXITED(EXIT_STAT)&&!WIFSIGNALED(EXIT_STAT));
    //printf("%d exited\n",pid);
    return 0;
}
