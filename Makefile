CC = gcc
CFLAGS = -Wall
PDIR = ./programs
SRCS = ./sources
_OBJ = ls
OBJ = $(patsubst %, $(PDIR)/%, $(_OBJ))
PROG = $(patsubst %, $(SRCS)/%.c, $(_OBJ))

shell: sources/shell.c $(OBJ)
	$(CC) $< -o $@ $(CFLAGS)

$(OBJ): $(PROG)
	$(CC) $< -o $@ $(CFLAGS)