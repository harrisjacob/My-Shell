#include <stdio.h>
#include <stdlib.h>
#include<errno.h>
#include<string.h>

int main(int argc, char* argv[]){
	const char* CWDpath = getenv("myCWD");
	if(!CWDpath){
		fprintf(stderr, "%s: Failed to read directory path: %s\n", argv[0], strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%s\n", CWDpath);
	return EXIT_SUCCESS;
}