#include<string.h>
#include<stddef.h>
#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
///make a free stack function too
void init_stack(STACK* s)
{
	s->top=-1;

	int i;

		for(i=0;i<100;i++)//basically 100 cmds history
		{
			s->base[i] = malloc(sizeof(char) * (1000));
			s->base[i][0]='\0';// 0 len strings
		}

		

}

void push(STACK* s,char *str)
{
		if(s->top== -1)
		{
			strcpy(s->base[++(s->top)],str);
			return;
		}
		else if(s->top >= 0){
			if(strcmp(s->base[s->top],str) == 0) return;//if the prev cmd is same no need to add it again will save memory
			else strcpy(s->base[++(s->top)],str);
		}
		else return;//weird lonely else


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

