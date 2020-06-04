CC = gcc
CFLAGS = -Wall
PDIR = ./programs
SRCS = ./sources
_OBJ = ls pwd touch rm mv
OBJ = $(patsubst %, $(PDIR)/%, $(_OBJ))
LIBR = ./lib
HEAD = lib/cd.h lib/common.h

all: shell $(OBJ)

shell: sources/shell.c $(HEAD)
	$(CC) $< -o $@ $(CFLAGS) -I $(LIBR)

$(PDIR)/%: $(SRCS)/%.c
	$(CC) $< -o $@ $(CFLAGS) -I $(LIBR)