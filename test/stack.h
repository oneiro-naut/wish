typedef struct STACK{ int top; int* arr; }STACK;
void init_stack(STACK* S);
void push(STACK* s,int value);
int pop(STACK* s);