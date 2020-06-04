#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include "common.h"

void usage(){
	printf("Usage: mv SOURCE DEST\n");
	printf("   or: mv SOURCE DIRECTORY\n");
	printf("Rename SOURCE to DEST, or move SOURCE to DIRECTORY.\n");
}

int validPath(char* path){
	if(!path) return -1;
	
	int depthCnt = 0;

	if(*path != '/'){
		char *CWD = getenv("myCWD");
		if(!CWD){
			fprintf(stderr, "mv: Failed to verify current working directory\n");
			return -1;
		}

		depthCnt = charCount(CWD, '/');
	}

	char pathCopy[strlen(path)+1];
	strcpy(pathCopy, path);

	char* elem = strtok(pathCopy, "/");	
	while(elem){
		if(strcmp(elem, "..")==0){
			depthCnt--;
			if(depthCnt < 1) return -1;
		}else if(strcmp(elem,".")==0){}
		else{
			depthCnt++;
		}

		elem = strtok(NULL,"/");
	}


	return depthCnt;
}


int handleMV(char* source, char* dest){
	
	if(!source || !dest) return EXIT_FAILURE;



	if(validPath(source) < 1){
		fprintf(stderr, "mv: Source cannot be above root\n");
		return EXIT_FAILURE;
	}

	if(validPath(dest) < 1){
		fprintf(stderr, "mv: Destination cannot be above root\n");
		return EXIT_FAILURE;
	}

	//printf("%c\n", *(dest+strlen(dest)-1));
	/*
	struct stat tryDirect;
	if(stat(dest, &tryDirect) < 0){
		fprintf(stderr, "mv: Could not identify destination\n");
		return EXIT_FAILURE;
	}


	//handle path end as directory
	if(S_ISDIR(tryDirect.st_mode)){


	}*/

/*
	if(*(dest+strlen(dest)-1) == '/'){
		printf("Directory\n");
	}else{
		printf("%c\n", *(dest+strlen(dest)));
	}
*/


	if(access(dest, W_OK) < 0){
		fprintf(stderr, "mv: Permission denied for destination file: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	


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


	if(handleMV(argv[1], argv[2]) == EXIT_FAILURE){
		fprintf(stderr, "mv: cannot move '%s' to '%s'\n", argv[1], argv[2]);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}