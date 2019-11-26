
typedef struct STACK{ int top; char base[3][1000]; }STACK;

void init_stack(STACK* s);

void push(STACK* s,char *str);

char *pop(STACK* s);