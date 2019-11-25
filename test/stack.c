#include"stack.h"
#include<stddef.h>
#define STKSIZE 20


void init_stack(STACK* s)
{
	s->top=-1;

}

void push(STACK* s,int value)
{
	if(s->top!=STKSIZE)
	{
		s->arr[++(s->top)]=value;
	}
}

int pop(STACK* s)
{
	if(s->top!=-1)
		{
			(s->top)--;
			return (s->arr[(s->top)+1]);
		}
	else return -1;
}

