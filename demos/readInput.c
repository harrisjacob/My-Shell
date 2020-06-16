#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<curses.h>

#define MAX_COMMAND 1024

int main(int argc, char* argv[]){

	WINDOW *win = initscr();
	nodelay(win, FALSE);
	cbreak();

	char userInput[MAX_COMMAND];
	char* curChar = userInput;
	printf("Command> ");
	int c;
	*(curChar + MAX_COMMAND - 1) = '\0';

	while((c = getch()) != EOF && c!='\n'){
		printf("%c", c);
		*curChar++ = c;
	}

	printf("\n%s\n", userInput);

	return EXIT_SUCCESS;
}
