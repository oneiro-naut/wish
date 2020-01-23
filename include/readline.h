#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define KEY_ESCAPE  0x001b
#define KEY_ENTER   0x000a//all of these are unicodes

#define cursorforward(x) printf("\033[%dC", (x))
#define cursorbackward(x) printf("\033[%dD", (x))
#define backspc() printf("\b \b")
#define CTRL_KEY(k) ((k) & 0x1f) //masking unmasking//ASCII table is special


static size_t buffersize;
static struct termios term, oterm;



enum editorKey {
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};



/*** line buffer ***/

typedef struct lbuf {
  char *b;
  int len;
  int size;
  int cursor;
}lbuf;

#define LBUF_INIT {NULL,0,128,0}

void initreadLine(size_t bufsize);
char *readLine();
void die(const char *s);

static int render_lbuf(lbuf *buf);
static void insertchar(lbuf *v_buf,char c);
static void deletechar(lbuf* v_buf);
static int getch(char* c,int vmin,int vtime);
static int editorReadKey();
extern int enableRawMode();
extern void disableRawMode();

