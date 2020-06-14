#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include "common.h"

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

	char* rmFile;
	if(!(rmFile = allocPath(argv[1]))){
		fprintf(stderr, "rm: Failed to resolve specified path\n");
		return EXIT_FAILURE;
	} 

	if(unlink(rmFile)<0){
		fprintf(stderr, "rm: Failed to remove '%s': %s\n", argv[1], strerror(errno));
		free(rmFile);
		return EXIT_FAILURE;
	}

	free(rmFile);
	return EXIT_SUCCESS;
}