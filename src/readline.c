#include "../include/readline.h"
#include "../include/execute.h"
#include "../include/stack.h"
#include "../include/w_env.h"
#include <stdio.h>


static void insertchar(lbuf *v_buf,char c)
{
    
    if(v_buf==NULL)return;
    //if(ind>(v_buf->len))return;
    
    char* newbuf=NULL;
    
    
    
    if(v_buf->len  >=  v_buf->size -1)
    {
      (v_buf->size)*=2;
      newbuf = realloc(v_buf->b,v_buf->size);//realloc is awesome
    }
    
    for(int i=v_buf->len; i >= v_buf->cursor ;i--)v_buf->b[i+1]=v_buf->b[i];//shift each byte forward by 1
    (v_buf->len)++;
    //v_buf->b[i]='\0'; <= takes care of shifting null char as well
    v_buf->b[v_buf->cursor]=c;//c must not be a control char
    render_lbuf(v_buf);
    (v_buf->cursor)++;
    if(v_buf->len - v_buf->cursor)cursorbackward(v_buf->len - v_buf->cursor);
    
    
    
  

}

static void deletechar(lbuf* v_buf)
{
    if(v_buf==NULL)return;
    if(v_buf->b==NULL)return;
    if(v_buf->cursor==0)return;
    (v_buf->cursor)--;
    for(int i=v_buf->cursor; i < v_buf->len ;i++)v_buf->b[i]=v_buf->b[i+1];//shift 1 byte back
    (v_buf->len)--;
    if(v_buf->cursor >=0)cursorbackward(1);
    render_lbuf(v_buf);
    if(v_buf->len - v_buf->cursor)cursorbackward(v_buf->len - v_buf->cursor);
}

static int render_lbuf(lbuf *buf)
{
  char whit=' ';
  int shift=0;
  for(int i=buf->cursor;i <= buf->len;i++,++shift)
  {
    if(buf->b[i])putchar(buf->b[i]);
    else
    { 
      putchar(whit);
      cursorbackward(1);//adding this else fixed issue with backspace
    }
  }
  return shift;
}


void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}

static int getch(char* c,int vmin,int vtime)// returns immediate char hit to c
{
    int ret = 0;

    tcgetattr(0, &oterm);//original term config struct= oterm here
    memcpy(&term, &oterm, sizeof(term));//copying that to term struct
    //modifying the copy 
    term.c_lflag &= ~(ICANON | ECHO);//turn off echo(char type wont be printed to term) and canonical mode(line by line read mode) to raw mode(byte by byte read mode)
    //term.c_lflag &= ~(ICANON );
    term.c_cc[VMIN] = vmin;//min number bytes to be read before read returns it is 1 B here 
    term.c_cc[VTIME] = vtime;//read timeout after which it returns
    tcsetattr(0, TCSANOW, &term);//setting new config using our modified term config struct
    ret = getchar();//tcsanow == apply change immediately
    *c = ret;
    //ret =read(STDIN_FILENO, c, 1);
    tcsetattr(0, TCSANOW, &oterm);//reverted back to original config
    return ret;
}



static int editorReadKey() {
  int nread;
  char c=0;
  nread=getch(&c,1,0);
  if(nread==-1)return nread;

  if (c == KEY_ESCAPE) {
    char seq[3];

    if (getch(&seq[0],0,1) == EOF) return KEY_ESCAPE;
    if (getch(&seq[1],0,1) == EOF) return KEY_ESCAPE;

    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (getch(&seq[2],0,1) == EOF) return KEY_ESCAPE;
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '1': return HOME_KEY;
            case '3': return DEL_KEY;
            case '4': return END_KEY;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
            case '7': return HOME_KEY;
            case '8': return END_KEY;
          }
        }
      } else {
        switch (seq[1]) {
          case 'A': return ARROW_UP;
          case 'B': return ARROW_DOWN;
          case 'C': return ARROW_RIGHT;
          case 'D': return ARROW_LEFT;
          case 'H': return HOME_KEY;
          case 'F': return END_KEY;
        }
      }
    } else if (seq[0] == 'O') {
      switch (seq[1]) {
        case 'H': return HOME_KEY;
        case 'F': return END_KEY;
      }
    }

    return KEY_ESCAPE;
  } else {
    return c;
  }
}

void initreadLine(size_t bufsize)
{
  buffersize = bufsize;

}

char *readLine() {

  int itr;
  int prevStringLength=0;
  int hist=0;
  lbuf buf = {NULL,0,buffersize,0};
  buf.b =(char*)malloc(sizeof(char)*buf.size);
  buf.b[0] = '\0';
  char *tempChar = (char*)malloc(sizeof(char)*1000);
//  printf("crazY prompt>>");

  while (1) {

    int c = editorReadKey();
    if(c==-1){putchar('\n'); 
    free(buf.b);  
    return NULL; } //***when ctrl c or ctrl z is pressed getchar will return -1 that is it got interrupted by sigint or something 
    // this behaviour is by default set to restarting the syscall that is restarting the get char 
    //can be turned off by setting sa_flag =0 which is  by default set to SA_RESTART***this is some serious OS concept**more to learn
    //https://stackoverflow.com/questions/7978315/ctrl-c-eaten-by-getchar <--this question discusses this     
    if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
      deletechar(&buf);
    } 
    else if(c==CTRL_KEY('d'))
    {
      putchar('\n');
      exit(0);
    }
    else if(c==CTRL_KEY('l'))
    {
      char* clear[]={"/usr/bin/clear",NULL};
      exec_w(clear[0],clear);
      return buf.b;
    }
    else if (c == KEY_ENTER) {
      if (buf.len >=0) {
        putchar('\n');
        return buf.b;
      }
    } 
    else if (c == ARROW_LEFT) {
            if(buf.cursor)
            {
              (buf.cursor)--;
              cursorbackward(1);
            }
        } 
    else if (c == ARROW_RIGHT) {
            if(buf.cursor < buf.len){
              (buf.cursor)++;
              cursorforward(1);
            }
        }

    else if (c == ARROW_UP) {
      hist++;
      if(hist>0){
        for(itr=0;itr<prevStringLength;itr++){
          deletechar(&buf);
        }
        tempChar =printI(&HISTSTACK, hist);
        prevStringLength = strlen(tempChar);
        for(itr=0;itr<prevStringLength;itr++){
          insertchar(&buf,tempChar[itr]);
        }
      }


    }

    else if (c == ARROW_DOWN) {
      hist--;
      if(hist>0){
      for(itr=0;itr<prevStringLength;itr++){
          deletechar(&buf);
        }
        tempChar =printI(&HISTSTACK, hist);
        prevStringLength = strlen(tempChar);
        for(itr=0;itr<prevStringLength;itr++){
          insertchar(&buf,tempChar[itr]);
        }
      } 
    }

    else if(CTRL_KEY(c)=='\x5e'){putchar('^');putchar('C');putchar('\n'); return buf.b; }
    else if(CTRL_KEY(c)=='\x1a'){}
    else if (!iscntrl(c) && c < 128) {     
      insertchar(&buf,c);
      //basically overwriting to replace with true insertion
    }

    
  }
}



