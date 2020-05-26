CC = gcc

all: sources/ls.c
	$(CC) $^ -o programs/ls -Wall