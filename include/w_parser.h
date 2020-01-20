#include "cmd_struct.h"

static int checkTokens(char **argv);

static int countPipes(char * str,int* fileoptflag);// can be implemented using a FSM

cmd_struct* parse_input(char* stream);


