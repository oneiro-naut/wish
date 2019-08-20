#include<string.h>
typedef struct STACK{ int top; char base[3][1000]; }STACK;
void init_stack(STACK* S);
void push(STACK* s,char* str);
char* pop(STACK* s);