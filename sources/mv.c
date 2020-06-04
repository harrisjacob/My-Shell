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

int validPath(char* path){
	if(!path) return -1;

	//char* elem;
	
	char *CWD = getenv("myCWD");
	if(!CWD){
		fprintf(stderr, "mv: Failed to check destination path\n");
		return -1;
	}

	int depthCnt = charCount(CWD, '/');
	printf("Depth: %d\n", depthCnt);
/*
	elem = strtok(CWD, "/");
	while(elem){
		depthCnt++;
		elem = strtok(NULL,"/");
	}
*/
	/*
	elem = strtok(path, "/");
	do{

	}while()
	*/
	free(CWD);
	return 0;
}


int handleMV(char* source, char* dest){
	if(!source || !dest) return EXIT_FAILURE;


	validPath(dest);

	if(access(dest, W_OK) < 0){
		fprintf(stderr, "mv: Permission denied for destination file: %s\n", strerror(errno));
		return EXIT_FAILURE;
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