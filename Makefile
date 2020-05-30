CC = gcc
CFLAGS = -Wall
PDIR = ./programs
SRCS = ./sources
_OBJ = ls pwd
OBJ = $(patsubst %, $(PDIR)/%, $(_OBJ))
LIBR = ./lib

all: shell $(OBJ)

shell: sources/shell.c
	$(CC) $< -o $@ $(CFLAGS) -I $(LIBR)

$(PDIR)/%: $(SRCS)/%.c
	$(CC) $< -o $@ $(CFLAGS) -I $(LIBR)