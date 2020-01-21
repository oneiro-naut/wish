#include "../include/process.h"
#include <unistd.h>
#include <sys/wait.h>


int waitfor(pid_t pid)
{
    int EXIT_STAT;
    do{
        waitpid(pid,&EXIT_STAT,WUNTRACED);
    }while(!WIFEXITED(EXIT_STAT)&&!WIFSIGNALED(EXIT_STAT));

    return EXIT_STAT;
}