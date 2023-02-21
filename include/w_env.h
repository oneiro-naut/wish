#ifndef W_ENV_H
#define W_ENV_H
//i think it should be called GLOBAL_H or global.h
#include "stack.h"

#define INPUTSIZE 1024  //sufficient length
#define CMDPERINPUT 16  //sufficient size of cmd queue array
#define PATHLEN 1024
#define DIRSTCKSIZE 32
#define TOKSIZE 128

extern struct sigaction act;
extern char* stream;
extern char PWD[1000];
extern STACK DIRSTACK;
extern STACK HISTSTACK;
extern char *host_name;
extern char *user_name;

#endif //W_ENV_H

