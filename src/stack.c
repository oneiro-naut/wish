#include<string.h>
#include<stddef.h>
#include "../include/stack.h"
#include <stdlib.h>
#include <stdio.h>

void init_stack(STACK* s)
{
	s->top=-1;

	int i;

		for(i=0;i<100;i++)
		{
			s->base[i] = malloc(sizeof(char) * (1000));
		}

}

void push(STACK* s,char *str)
{
		strcpy(s->base[++(s->top)],str);
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

char* printI(STACK* s, int i){

	//printf("%s",s->base[s->top - i + 1]);
	//printf("%d   %i \n",s->top,i);
	int idx = s->top;
	idx = idx - i;
	idx++;
	return s->base[idx];
	// return "command";
}

