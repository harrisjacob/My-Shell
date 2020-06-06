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

//splitPath separates the base path (baseP) into allocated parent path and filename
//If basepath starts with a '/', the full (from actual root) is allocated
//On failure, splitPath should handle allocation cleanup
int splitPath(char* pathAlc, char* fileAlc, char* baseP){

		char* split = findTailChar(baseP, '/', 1);
		int pathLen = split - baseP;
		int fileLen = baseP + strlen(baseP) - split;

		if(split == baseP){
			if(!(pathAlc = strdup(baseP))) return EXIT_FAILURE;
			if(!(fileAlc = strdup(baseP))){
				free(pathAlc);
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}

		if(*baseP == '/'){

			char *aliasRoot;
			if(!(aliasRoot = getenv("myRoot"))) return EXIT_FAILURE;
			if(!(pathAlc = malloc(strlen(aliasRoot)+ pathLen + 1))) return EXIT_FAILURE;
			strcpy(pathAlc, aliasRoot);
			char* nullC = pathAlc + strlen(pathAlc);
			memcpy(nullC, baseP, pathLen);
			*(pathAlc + strlen(aliasRoot)+pathLen) = '\0';

		}else{

			if(!(pathAlc = malloc(pathLen + 1))) return EXIT_FAILURE;
			memcpy(pathAlc, baseP, pathLen);
			*(pathAlc + pathLen) = '\0';

		}

		if(!(fileAlc = malloc(fileLen + 1))){
			free(pathAlc);
			return EXIT_FAILURE;
		}

		strcpy(fileAlc, split+1);


		printf("Path: $%s$\n", pathAlc);
		printf("File: $%s$\n", fileAlc);

		free(fileAlc);
		free(pathAlc);
		return EXIT_SUCCESS;

}

int handleMV(char* source, char* dest){
	
	if(!source || !dest) return EXIT_FAILURE;

	int isDirectory = 0;
	char *destDirect, *sourceDirect, *fileName;
	


	if(validPath(source) < 0){
		fprintf(stderr, "mv: Source cannot be above root\n");
		return EXIT_FAILURE;
	}

	if(validPath(dest) < 0){
		fprintf(stderr, "mv: Destination cannot be above root\n");
		return EXIT_FAILURE;
	}


	//Destination 
	if(access(dest, F_OK)==0){
		struct stat tryDirect;
		if(stat(dest, &tryDirect)<0){
			return EXIT_FAILURE;
		}

		if(S_ISDIR(tryDirect.st_mode)){
			//Valid path and ends in a directory.  Need to check permission dest path and get file name from source
			isDirectory = 1;

			//Unnecesary allocation in context, but should be allocated because all other cases allocate
			if(!(destDirect = strdup(dest))) return EXIT_FAILURE;
			/*
			if(!(destDirect = malloc((strlen(dest)+1)*sizeof(char)))) return EXIT_FAILURE;
			memcpy(destDirect, dest, strlen(dest)+1);
			*/

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

		char *aliasRoot;
		if(!(aliasRoot = getenv("myRoot")))return EXIT_FAILURE;
		if(strSplit!=dest){

			int dPathSize = strSplit - dest + 1;
			if(*dest=='/'){
				if(!(destDirect = malloc(dPathSize+strlen(aliasRoot)+1))) return EXIT_FAILURE;
				strcpy(destDirect, aliasRoot);
				//memcpy(destDirect, aliasRoot, strlen(aliasRoot));
				strcat(destDirect, dest);
			}else{
				
				if(!(destDirect = malloc(dPathSize))) return EXIT_FAILURE;
				memcpy(destDirect, dest, strSplit - dest);
				*(destDirect + (strSplit - dest)) = '\0';
			}

			trimToChar(destDirect, '/', strlen(destDirect));
		}else{
			char *CWD;
			if(!(CWD = getenv("myCWD")))return EXIT_FAILURE;
			if(!(destDirect = malloc(strlen(CWD)+strlen(aliasRoot)+1)));
			strcpy(destDirect, aliasRoot);
			strcat(destDirect, CWD);
		}
		
		fileName = strSplit;
		fileName += (strSplit == dest) ? 0:1;
	}


	//Source
	if(*source == '/'){
		char *aliasRoot;
		if(!(aliasRoot = getenv("myRoot")))return EXIT_FAILURE;


		if(!(sourceDirect = malloc(strlen(aliasRoot)+strlen(source)+1))){
			return EXIT_FAILURE;
		}
		strcpy(sourceDirect, aliasRoot);
		strcat(sourceDirect, source);
		
		//char sourceRoot[strlen(aliasRoot)+strlen(source)+1];
		/*
		strcpy(sourceRoot, aliasRoot);
		strcat(sourceRoot, source);
		trimToChar(sourceRoot, '/', strlen(sourceRoot));
		sourceDirect = sourceRoot;
		*/
	}else{
		if(!(sourceDirect = strdup(source))) return EXIT_FAILURE;
		
		/*
		char sourceCopy[strlen(source)+1];
		strcpy(sourceCopy, source);
		trimToChar(sourceCopy, '/', strlen(sourceCopy));
		sourceDirect = sourceCopy;
		*/
	}
	trimToChar(sourceDirect, '/', strlen(sourceDirect));


	printf("\nDestination Directory: $%s$\n", destDirect);
	printf("Source Directory: $%s$\n", sourceDirect);
	printf("New filename: $%s$\n", fileName);
	free(destDirect);
	
	return EXIT_SUCCESS;
}




int main(int argc, char* argv[]){

	char baseP[] = "/hello/mr/jacob";
	char *pathAlc=NULL, *fileAlc=NULL;
	splitPath(pathAlc, fileAlc, baseP);
	printf("\n");
	char baseP2[] = "hello/mr/jacob";
	splitPath(pathAlc, fileAlc, baseP2);
/*	
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
*/
	return EXIT_SUCCESS;
}