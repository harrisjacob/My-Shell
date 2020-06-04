#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include "common.h"

void usage(){
	printf("Usage: mv SOURCE DEST\n");
	printf("   or: mv SOURCE DIRECTORY\n");
	printf("Rename SOURCE to DEST, or move SOURCE to DIRECTORY.\n");
}

int handleMV(char* source, char* dest){
	if(!source || !dest) return EXIT_FAILURE;

		//Need to resolve full path
	if(access(dest, W_OK|X_OK) < 0){
		fprintf(stderr, "mv: Permission denied for destination file: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}else{
		printf("Success!\n");
	}
	/*
	if(dest[0] == '/'){
		//check from root

	}else{
		//strtok do while?



	}

	//Source needs 

*/
	return EXIT_SUCCESS;
}




int main(int argc, char* argv[]){

	if(argc < 2){
		usage();
		return EXIT_FAILURE;
	}

	if(strcmp(argv[1], "--help") == 0){
		usage();
		return EXIT_SUCCESS;
	}


	return handleMV(argv[1], argv[2]);

}