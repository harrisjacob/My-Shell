#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>

void usage(){
	printf("Usage: rm [FILE]\n");
	printf("Remove (unlink) the FILE.\n");
}

int main(int argc, char* argv[]){

	if(argc < 2){
		printf("rm: mising operand\n");
		printf("Try 'rm --help' for more information.\n");
		return EXIT_FAILURE;
	}

	if(strcmp(argv[1], "--help")==0){
		usage();
		return EXIT_SUCCESS;
	}

	if(unlink(argv[1])<0){
		fprintf(stderr, "rm: cannot remove '%s': %s\n", argv[1], strerror(errno));
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}