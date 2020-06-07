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
	
	int depthCnt;

	if(*path != '/'){
		char *CWD = getenv("myCWD");
		if(!CWD){
			fprintf(stderr, "mv: Failed to verify current working directory\n");
			return -1;
		}

		depthCnt = charCount(CWD, '/');
		depthCnt += (strcmp(CWD,"/")==0) ? 0 : 1;
	}else{
		depthCnt = 1;
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

//splitPath attempts to split the filename from the path
int splitPath(char** fileAlc, char* baseP){

		if(strlen(baseP) == 1){
			if(!(*fileAlc = strdup(baseP))) return EXIT_FAILURE;
			return EXIT_SUCCESS;
		}

		char* split = findTailChar(baseP, '/', 1);
		int fileLen = baseP + strlen(baseP) - split;

		if(split == baseP){
			char* skipChar = baseP;
			if(*baseP == '/') skipChar+=1;
			if(!(*fileAlc = strdup(skipChar)))return EXIT_FAILURE;
			return EXIT_SUCCESS;
		}

		if(!(*fileAlc = malloc(fileLen + 1))) return EXIT_FAILURE;

		strcpy(*fileAlc, split+1);

		return EXIT_SUCCESS;
}

//0 is not dir, 1 is dir
int pathIsDir(char* path){
	if(*path == '/'){
		char* root, *fullPath;
		if(!(root = getenv("myRoot"))) return EXIT_FAILURE;
		if(!(fullPath = malloc(strlen(root)+ strlen(path) + 1))) return EXIT_FAILURE;

		strcpy(fullPath, root);
		strcat(fullPath, path);
	
		if(access(fullPath, F_OK)==0){
			struct stat tryPath;
			if(stat(fullPath, &tryPath)<0){
				free(fullPath);
				return 0;
			}
			if(S_ISDIR(tryPath.st_mode)){
				free(fullPath);
				return 1;
			}
		}
		free(fullPath);
	}else{
		if(access(path,F_OK)==0){
			struct stat tryPath;
			if(stat(path, &tryPath)<0) return 0;
			if(S_ISDIR(tryPath.st_mode)) return 1;
		}
	}
	
	return 0;
}

int parentAccess(char* path, int mode){

	char* parent = findTailChar(path, '/', 0);
	if(parent == path) return EXIT_FAILURE;
	
	int parentLen = parent - path;
	char parentPath[parentLen+ 1];
	strncpy(parentPath, path, parentLen);
	*(parentPath + parentLen) = '\0';

	return (access(parentPath, mode) == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}

int handleMV(char* source, char* dest){
	
	if(!source || !dest) return EXIT_FAILURE;

	char *dPath, *dFile, *sPath, *sFile;
	dPath = dFile = sPath = sFile = NULL;
	
	char *root, *CWD;


	if(validPath(source) < 0){
		fprintf(stderr, "mv: Source cannot be above root\n");
		return EXIT_FAILURE;
	}

	if(validPath(dest) < 0){
		fprintf(stderr, "mv: Destination cannot be above root\n");
		return EXIT_FAILURE;
	}

	//Get root and local CWD
	if(!(root = getenv("myRoot"))) return EXIT_FAILURE;
	if(!(CWD = getcwd(NULL, 0))) return EXIT_FAILURE;
	int rootLen = strlen(root);
	int CWDLen = strlen(CWD);

	int sRoot = (*source == '/') ? 1 : 0;
	int dRoot = (*dest == '/') ? 1 : 0;

	
	if(pathIsDir(source)){
		free(CWD);
		return EXIT_FAILURE;  //Source must be a file
	}


	if(splitPath(&sFile, source)==EXIT_FAILURE){
		free(CWD);
		return EXIT_FAILURE;
	}



	int prefixSz;

	prefixSz = (sRoot) ? rootLen : CWDLen;

	//Source
	if(!(sPath = malloc(prefixSz + strlen(source) + 2))){
		free(sFile);
		free(CWD);
		return EXIT_FAILURE;
	}
	if(sRoot){
		strcpy(sPath, root);
	}else{
		strcpy(sPath, CWD);
		strcat(sPath, "/");
	}
		
	strcat(sPath, source);

	
	//Destination
	if(splitPath(&dFile, dest)==EXIT_FAILURE){
		free(CWD);
		free(sPath);
		free(sFile);
		return EXIT_FAILURE;
	}

	int destIsDir = pathIsDir(dest);

	if(destIsDir){
		if(!(dFile = strdup(sFile))){
			free(CWD);
			free(sPath);
			free(sFile);
			return EXIT_FAILURE;
		}

		if(!(dPath = malloc(prefixSz + strlen(dest) + strlen(dFile) + 3))){
			free(CWD);
			free(sPath);
			free(sFile);
			free(dFile);
		}
		if(dRoot){
			strcpy(dPath, root);
		}else{
			strcpy(dPath, CWD);
			strcat(dPath, "/");
		}

		strcat(dPath, dest);
		strcat(dPath, "/");
		strcat(dPath, dFile);

	}else{
		
		if(!(dPath = malloc(prefixSz + strlen(dest) + 2))){
			free(dFile);
			free(CWD);
			free(sPath);
			free(sFile);
			return EXIT_FAILURE;
		}

		if(*dest == '/'){
			strcpy(dPath, root);
		}else{
			strcpy(dPath, CWD);
			strcat(dPath, "/");
		}
		
		strcat(dPath, dest);
	}

	//printf("\nSource File: %s\n", sFile);
	//printf("Source Path: %s\n", sPath);
	//printf("Dest File: %s\n", dFile);
	//printf("Dest Path: %s\n\n", dPath);


	if(parentAccess(sPath, W_OK) == EXIT_FAILURE || parentAccess(dPath, W_OK) == EXIT_FAILURE ){
		free(dFile);
		free(dPath);
		free(CWD);
		free(sPath);
		free(sFile);
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