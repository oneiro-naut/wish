INCDIR =./include
CC=gcc
LD=gcc
INCFLAGS=-I$(INCDIR)
CFLAGS=-std=gnu99

SRCDIR=./src
OBJDIR=./obj
BINDIR=./bin
LIBDIR =./lib

LIBS=-lreadLine

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJLIST = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
SRCLIST=

$(BINDIR)/wish: $(OBJLIST)
	mkdir -p $(BINDIR)
	$(LD) -g $(CFLAGS) $(INCFLAGS) $^ $(LIBS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) -g $(CFLAGS) $(INCFLAGS) -c $^ -o $@

clean:
	rm bin/* obj/*

