#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h> 
#include <sys/types.h> 
#include <signal.h>
#include <time.h>
#include <readLine/readline.h>
#include "stack.h"
#include "execute.h"
#include "w_parser.h"
#include "w_env.h"
#include "w_io.h"

pid_t shell_PID;
int EXIT_STAT;

struct sigaction act;
char* stream = NULL;
char PWD[1000];
STACK DIRSTACK;
STACK HISTSTACK;
char *host_name = NULL;
char *user_name = NULL;

int wish_init();
int shell_loop();
void printPrompt();
void changedir(char** argv);
void sighandler(int);

#if 0
void getprompt()
{
  // Retrieving hostname from /etc/hostaname file
  FILE *file = fopen("/etc/hostname","r");
  host_name=(char *)malloc(15*sizeof(char));
  if(!file)
    exit(0);
  fscanf(file,"%s",host_name);
  fclose(file);

  // Retrieving username using getlogin() fron <unistd.h>
  user_name=(char *)malloc(10*sizeof(char));
  user_name=getlogin();
}
#endif

int main(int argc, char**argv)
{
  //initialize globals
  if (wish_init())
    return EXIT_FAILURE;
  return shell_loop();
}

void sighandler(int sig_num) 
{ 
  //sleep(100);
  //fflush(stdout); 
} 

int wish_init()
{
  //setting up signal handler for Parent process that is wish shell
  memset(&act, 0, sizeof(act));
  act.sa_handler = sighandler;
  act.sa_flags = 0; //not SA_RESTART
  sigaction(SIGINT, &act, 0); //should work only for parent process 
  sigaction(SIGTSTP, &act, 0);

  getcwd(PWD, PATHLEN);
  init_stack(&DIRSTACK);
  init_stack(&HISTSTACK);

  STD_IN_DUP = 0;
  STD_OUT_DUP = 1;
  STD_ERR_DUP = 2;

  // Retrieving hostname from /etc/hostname file
  FILE *file = fopen("/etc/hostname","r");
  host_name = (char *)malloc(15 * sizeof(char));
  if (!file)
    return -1;
  fscanf(file, "%s", host_name);
  fclose(file);

  // Retrieving username using getlogin() from <unistd.h>
  user_name = getlogin();
  return 0;
}

void printPrompt()
{
  printf("\033[0;31m"); 
  printf("\033[1m");
  printf("%s  ➜  ",host_name);
  printf("\033[0m");
  printf("\033[0m"); 
}

int shell_loop()
{
  cmd_struct* current_cmd_struct = NULL;
  while (1)
  {
    printPrompt();
    stream = readLine();
    if (!stream)
      continue;
    if (stream!=NULL)
      push(&HISTSTACK, stream); //el draco is cool but dont push NULL ptr to stack NULL ptr deref caused seg faults
    if (stream[0] == 0x4) // ctrl-D
    {
      printf("\nexit\n");
      // maybe call some cleanup function
      break;
    }
    current_cmd_struct = parse_input(stream);
    execute_cmd_struct(current_cmd_struct);
    free_cmd_struct(&current_cmd_struct);

    if (stream)
      free(stream); //free cant free NULL lol
    stream = NULL;
  }
  if (host_name)
    free(host_name);
  host_name = NULL;
  return EXIT_SUCCESS;
}

#if 0
void list_files(char *directory){
  struct dirent *di;
  DIR *dr = directory==NULL  ? opendir(".") : opendir(directory);
  if(dr == NULL){
    printf("could not open the directory / directory not present");
    return;
  }
  while((di = readdir(dr)) != NULL){
    puts(di->d_name);
  }
}
#endif

