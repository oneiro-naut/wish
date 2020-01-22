#ifndef W_ENV_H
#define W_ENV_H
//i think it should be called GLOBAL_H or global.h

#define INPUTSIZE 1000//sufficient length
#define CMDPERINPUT 10	//sufficient size of cmd queue array
#define PATHLEN 1000 
#define DIRSTCKSIZE 20  //
#define TOKSIZE 100


int STD_IN_DUP ;//0
int STD_OUT_DUP ;//1
int STD_ERR_DUP ;//2


char* stream;
char PWD[1000];
STACK DIRSTACK;
char *host_name;
char *user_name;
//extern char** environ;



#endif