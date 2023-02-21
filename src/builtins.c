#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "builtins.h"
#include "stack.h"
#include "w_env.h"

void changedir(char **argv)
{
  int dir_found=2;
  if (!argv[1]) //cd 
  {
    push(&DIRSTACK, PWD);
    dir_found = chdir(getenv("HOME"));
  }
  else if (strlen(argv[1]) > 1)
  {
    if (argv[1][0] == '/')
    {
      dir_found = chdir(argv[1]);
    }
    else
    {
      push(&DIRSTACK, PWD);
      strcat(PWD, "/");
      strcat(PWD, argv[1]);
      dir_found = chdir(PWD);
    }
  }
  else if (strlen(argv[1]) == 1)
  {
    if (argv[1][0] == '-') // cd -
    {
      if (DIRSTACK.top != -1) 
        dir_found = chdir(pop(&DIRSTACK));
      else
        printf("No previous working directory!\n");
      push(&DIRSTACK, PWD);
    }
    else if(argv[1][0] == '~')
    {
      push(&DIRSTACK, PWD);
      dir_found = chdir(getenv("HOME"));
    }
    else if(argv[1][0] == '/')
    {
      push(&DIRSTACK, PWD);
      dir_found = chdir(argv[1]);
    }
    else if(argv[1][0] == '.')
    {
    }
    else
    {
      push(&DIRSTACK, PWD);
      strcat(PWD, "/");
      strcat(PWD, argv[1]);
      dir_found = chdir(PWD);
    }
  }
  if (dir_found == -1)
  {
    perror("cd");
  }
  else if(dir_found == 0)
  {
    getcwd(PWD, PATHLEN);
  }
}

