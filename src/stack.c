#include<string.h>
#include<stddef.h>
#include "../include/stack.h"

void init_stack(STACK* s)
{
	s->top=-1;

}

void push(STACK* s,char *str)
{
	if(s->top!=2)
	{
		strcpy(s->base[++(s->top)],str);
	}
}

char *pop(STACK* s)
{
	if(s->top!=-1)
		{
			(s->top)--;
			return (s->base[(s->top)+1]);
		}
	else return NULL;
}

