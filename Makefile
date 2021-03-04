IDIR =include
CC=gcc
CFLAGS=-I$(IDIR)

CDIR=src
BDIR=bin
LDIR =lib

LIBS=-lreadLine

DEPS = $(wildcard $(IDIR)/*.h)

SRC = $(wildcard $(CDIR)/*.c)

wish: $(SRC)
	$(CC) -g -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

