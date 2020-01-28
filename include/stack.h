#ifndef STACK_H
#define STACK_H

typedef struct STACK{ int top; char *base[1000]; }STACK;

void init_stack(STACK* s);

void push(STACK* s,char *str);

char *pop(STACK* s);

char* printI( STACK* s, int i);


#endif