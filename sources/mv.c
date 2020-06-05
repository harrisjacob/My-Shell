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
		depthCnt += (strcmp(CWD,"/")==0) ? 0 : 1;
	}

	char pathCopy[strlen(path)+1];
	strcpy(pathCopy, path);

	char* elem = strtok(pathCopy, "/");	
	while(elem){
		if(strcmp(elem, "..")==0){
			if(--depthCnt < 1) return -1;
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

	int isDirectory = 0;
	char *destDirect, *fileName;

	if(validPath(source) < 0){
		fprintf(stderr, "mv: Source cannot be above root\n");
		return EXIT_FAILURE;
	}

	if(validPath(dest) < 0){
		fprintf(stderr, "mv: Destination cannot be above root\n");
		return EXIT_FAILURE;
	}

	if(access(dest, F_OK)==0){
		struct stat tryDirect;
		if(stat(dest, &tryDirect)<0){
			return EXIT_FAILURE;
		}

		if(S_ISDIR(tryDirect.st_mode)){
			//Valid path and ends in a directory.  Need to check permission dest path and get file name from source
			isDirectory = 1;

			//Unnecesary allocation in context, but should be allocated because all other cases allocate
			if(!(destDirect = malloc((strlen(dest)+1)*sizeof(char)))) return EXIT_FAILURE;
			memcpy(destDirect, dest, strlen(dest)+1);

			fileName = findTailChar(source, '/', 1);
			if(fileName!=source) fileName++; //If the character was found, it should not be included in the filename (skip '/' character)
		}
	}

	if(!isDirectory){
		//One of two cases:
		//Valid path but does not end in a directory. Need to check the write permissions of the parent and overwrite the existing file
		//Invalid dest path provided. Either the file name is new or there was an issue with parent directories.

		char* strSplit; // A pointer to the location of the split between new name and parent path
		strSplit = findTailChar(dest, '/', 1);

		if(strSplit!=dest){
			if(!(destDirect = malloc((strSplit - dest + 1)*sizeof(char)))) return EXIT_FAILURE;
			
			memcpy(destDirect, dest, strSplit - dest);
			*(destDirect + (strSplit - dest)) = '\0';

		}else{
			printf("StringSplit: %s\n", strSplit);
			printf("Dest: %s\n", dest);
			destDirect = NULL;
		}
		
		fileName = strSplit;
		fileName += (strSplit == dest) ? 0:1;

	}

	printf("Parent Directory: $%s$\n", destDirect);
	printf("New filename: $%s$\n", fileName);
	free(destDirect);
	
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