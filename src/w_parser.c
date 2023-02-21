#include "w_parser.h"
#include "w_tokenizer.h"
#include "cmd_struct.h"
#include <stdlib.h>
#include <ctype.h>//isalpha(char)

#define MAXCMDS 20
#define MAXPIPELINES 10

static int checkTokens(char **argv)
{
  /*
   *     argv[0] = . / _ / alphabet /  
   */
  int i = 0 ;
  if (argv[0][0] != '.' && argv[0][0] != '_' && !isalpha(argv[0][0]))
    return -1;
  return 0;
}

// can be implemented using a FSM
static int countPipes(char * str,int* fileoptflag)
{
  int i, j, npipes, napp, nwrite, foundPipe, foundAppend, foundwritef;
  char* pipestr = "|"; 
  char* append_sym = ">>";
  char* write_sym = ">";
  int io_pushed = 0;
  *fileoptflag=0; //by default 0
  npipes = 0;
  napp = 0;
  nwrite = 0; //always F****N INITIALIZE YOUR VARIABLES BEFORE U START TESTING !!!!!!
  i = 0; //added these init later ;/ thats why i m pissed
  j = 0;
  while (str[i])
  {
    // Match word with string
    foundPipe = 1;
    foundAppend = 1;
    foundwritef = 1;
    while (pipestr[j] && str[i + j])
    {
      if(str[i + j] != pipestr[j])
      {
        foundPipe = 0;
        break;
      }
      j++;
    }
    j = 0;
    while (write_sym[j] && str[i + j])
    {
      if (str[i + j] != write_sym[j])
      {
        foundwritef = 0;
        break;
      }
      j++;
    }
    j = 0;
    while (append_sym[j] && str[i + j])
    {
      if (str[i + j] != append_sym[j])
      {
        foundAppend = 0;
        break;
      }
      j++;
    }
    j = 0;
    if (foundAppend == 1)
    {
      foundwritef = 0;
      i = i + 2;
    }
    if (foundPipe == 1)
    {
      if (io_pushed > 0)
        return -1;
      npipes++;
    }
    if (foundAppend == 1)
    {
      io_pushed++;
      napp++;
    }
    if (foundwritef == 1)
    {
      io_pushed++;
      nwrite++;
    }
    i++;
  }

  if (napp > 1 || nwrite > 1)
    return -1;
  if (napp && nwrite)
    return -1;
  if (napp) 
    *fileoptflag = 2;
  if (nwrite)
    *fileoptflag = 1;
  return npipes;
}

//needs immediate testing**...well it works
cmd_struct* parse_input(char* stream)
{
  int length;
  cmd_struct* cmd = NULL;
  init_cmd_struct(&cmd);
  // has problem since it will tokenize single '&' too which is not primary seperator
  char* sep_primary = "&&;";
  // actually all delimiters have problems since all PnC are considered while tokenizing
  char* sep_io = "|&>><";
  char*** argp = NULL;
  int syntax_correct = 1; // by default correct
  int npipes = 0;
  int optfile = 0;
  int cmdcount = 0; // no. of pipelines

  // tokenization of user input
  char** complex_cmd_arr = wish_tok_quote_muted(stream);
  char** complex_cmd_component_cmd_arr = NULL;

  while (complex_cmd_arr[cmdcount])
  {
    argp = (char ***)malloc(sizeof(char**) * MAXCMDS);
    int y = 0;
    syntax_correct = 1;
    cmd->n_pipes[cmdcount] = countPipes(complex_cmd_arr[cmdcount], &(cmd->file_out[cmdcount]));
    complex_cmd_component_cmd_arr = w_tokenizer(complex_cmd_arr[cmdcount], sep_io, 0);

    while (complex_cmd_component_cmd_arr[y])
    {
      argp[y] = wish_tok(complex_cmd_component_cmd_arr[y]);
      y++;
    }
    if (complex_cmd_component_cmd_arr)
      free(complex_cmd_component_cmd_arr);
    complex_cmd_component_cmd_arr = NULL;
    argp[y] = NULL;    //making argp NULL terminated
    y = 0;

    for (int i = 0; argp[i] != NULL; i++)
    {
      if (checkTokens(argp[i]) == -1)
      {
        syntax_correct = 0;
        break;
      }
    }
    cmd->argp_arr[cmdcount] = argp;
    argp = NULL;        
    cmdcount++;
  }

  if (complex_cmd_arr)
    free(complex_cmd_arr);
  complex_cmd_arr = NULL;
  cmd->argp_arr[cmdcount] = NULL; // making argp_arr NULL terminated
  return cmd;
}

