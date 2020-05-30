CC = gcc
CFLAGS = -Wall
PDIR = ./programs
SRCS = ./sources
_OBJ = ls
OBJ = $(patsubst %, $(PDIR)/%, $(_OBJ))
PROG = $(patsubst %, $(SRCS)/%.c, $(_OBJ))
LIBR = ./lib

shell: sources/shell.c $(OBJ)
	$(CC) $< -o $@ $(CFLAGS) -I $(LIBR)

$(OBJ): $(PROG)
	$(CC) $< -o $@ $(CFLAGS) -I $(LIBR)