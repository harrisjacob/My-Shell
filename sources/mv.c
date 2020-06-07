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

int handleMV(char* source, char* dest){
	
	if(!source || !dest) return EXIT_FAILURE;

	//int dIsDir = 0;
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

	//Check if inputs are directories

	if(pathIsDir(source)){
		free(CWD);
		return EXIT_FAILURE;  //Source must be a file
	}

	//Attempt to split source in two
	if(splitPath(&sFile, source)==EXIT_FAILURE){
		free(CWD);
		return EXIT_FAILURE;
	}



	int prefixSz;

	prefixSz = (sRoot) ? rootLen : CWDLen;

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


	/*
	if(strcmp(sPath, sFile) == 0){
		//If the source char allocs match, a relative file was passed
		//Reset sPath to path to file
		free(sPath);
		sPath = NULL;
		int prefixSz = (sRoot) ? rootLen : CWDLen;

		if(!(sPath = malloc(prefixSz + strlen(sFile) + 2))){
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

		strcat(sPath, sFile);
	}else{
		free(sPath);
		sPath = NULL;
		if(*source == '/'){
			if(!(sPath = malloc(strlen(root) + strlen(source) + 2))){
				free(sFile);
				free(CWD);
				return EXIT_FAILURE;
			}
			strcpy(sPath, root);
		
		}else{
			
			if(!(sPath = malloc(strlen(CWD) + strlen(source) + 2))){
				free(CWD);
				free(sFile);
				return EXIT_FAILURE;
			}
			strcpy(sPath, CWD);
			strcat(sPath, "/");
		}
		
		strcat(sPath, source);

	}
	*/

	printf("\nSource: %s\n", sPath);
	printf("SFile: %s\n", sFile);
	

	//Attempt to split dest in two
	if(splitPath(&dFile, dest)==EXIT_FAILURE){
		free(CWD);
		free(sPath);
		free(sFile);
		return EXIT_FAILURE;
	}

	int destIsDir = pathIsDir(dest);
	prefixSz = (dRoot)? strlen(root) : strlen(CWD);

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



	/*
	if(strcmp(dPath, dFile) == 0){
		if(destIsDir){
			
			free(dFile);
			dFile = NULL;
			if(!(dFile = strdup(sFile))){
				free(CWD);
				free(sPath);
				free(sFile);
				return EXIT_FAILURE;
			}

			free(dPath);
			dPath = NULL;

			prefixSz = (*dest == '/')? strlen(root) : strlen(CWD);

			if(!(dPath = malloc(prefixSz + strlen(dest) + strlen(dFile) + 3))){
				free(CWD);
				free(sPath);
				free(sFile);
				free(dFile);
			}
			if(*dest == '/'){
				strcpy(dPath, root);
			}else{
				strcpy(dPath, CWD);
				strcat(dPath, "/");
			}

			strcat(dPath, dest);
			strcat(dPath, "/");
			strcat(dPath, dFile);

		}else{
			free(dPath);
			dPath = NULL;
			if(!(dPath = malloc(strlen(CWD) + strlen(dFile) + 1))){
				free(dFile);
				free(CWD);
				free(sPath);
				free(sFile);
				return EXIT_FAILURE;
			}
			strcpy(dPath, CWD);
			strcat(dPath, "/");
			strcat(dPath, dFile);
		}
	}else{
		if(destIsDir){
			free(dFile);
			dFile = NULL;
			if(!(dFile = strdup(sFile))){
				free(CWD);
				free(sPath);
				free(sFile);
				return EXIT_FAILURE;
			}
			free(dPath);
			dPath = NULL;
			prefixSz = (*dest == '/') ? strlen(root) : strlen(CWD);
			if(!(dPath = malloc(prefixSz+strlen(dest)+strlen(dFile)+3))){
				free(CWD);
				free(sPath);
				free(sFile);
				free(dFile);
			}
			if(*dest == '/'){
				strcpy(dPath, root);
			}else{
				strcpy(dPath, CWD);
				strcat(dPath, "/");
			}
			
			strcat(dPath, dest);
			strcat(dPath,"/");
			strcat(dPath,dFile);


		}else{	
			free(dPath);
			dPath = NULL;
			prefixSz = (*dest == '/') ? strlen(root) : strlen(CWD);
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

		

	}
	*/
	printf("Dest File: %s\n", dFile);
	printf("Dest Path: %s\n\n", dPath);







	/*		free(dPath);
			printf("Dest case 3: Read split\n");
			free(dPath);
			dPath = NULL;
			printf("CWD ------ %s\n", CWD);
			printf("Root ----- %s\n", root);
			printf("dest ----- %s\n", dest);
			if(!(dPath = malloc(strlen(CWD) + strlen(dest) + 1))){
				free(dFile);
				free(CWD);
				free(sPath);
				free(sFile);
				return EXIT_FAILURE;
			}
	*/
	free(dFile);
	free(dPath);
	free(CWD);
	free(sPath);
	free(sFile);










	/*
	if((dIsDir = pathIsDir(dest))){
		if(*dest == '/'){

		}else{
			dPath = strdup(dest);
		}
		
	}*/

	/*
	//Check if source is a directory
	if((sIsDir = pathIsDir(source))) return EXIT_FAILURE;

	//Check if destination is a directory
	dIsDir = pathIsDir(dest);

	if(!dIsDir){
		//One of two cases:
		//Valid path but does not end in a directory. Need to check the write permissions of the parent and overwrite the existing file
		//Invalid dest path provided. Either the file name is new or there was an issue with parent directories.

		if(splitPath(&dPath, &dFile, dest)==EXIT_FAILURE) return EXIT_FAILURE;

	}


	//Source
	if(splitPath(&sPath, &sFile, source)==EXIT_FAILURE){
		free(dPath);
		free(dFile);
		return EXIT_FAILURE;
	}

	if(!dFile && !(dFile = strdup(sFile))){
		free(dPath);
		free(sPath);
		free(sFile);
	}


	char *CWD;
	if(!(CWD= getcwd(NULL, 0))){
		free(dPath);
	 	free(sPath);
	 	free(dFile);
	 	free(sFile);
		return EXIT_FAILURE;
	}

	int concatPathSz = strlen(sPath)+strlen(sFile)+2;
	int rootPathSz = strlen(CWD)+strlen(sFile)+2; 
	char finSPath[(concatPathSz > rootPathSz) ? concatPathSz : rootPathSz];
	//Check source permissions
	if(strcmp(sPath,sFile)==0){
		//Check write permissions of the CWD

		if(access(CWD, W_OK)!=0){
			
			free(dPath);
		 	free(sPath);
		 	free(dFile);
		 	free(sFile);
		 	free(CWD);
			return EXIT_FAILURE;
		}

		strcpy(finSPath,CWD);
		strcat(finSPath, "/");
		strcat(finSPath, sFile);

	}else{
		//Check that write permissions of sPath is ok
		if(access(sPath, F_OK)!=0 || access(sPath, W_OK)!=0){
			free(dPath);
		 	free(sPath);
		 	free(dFile);
		 	free(sFile);
			return EXIT_FAILURE;
		}

		strcpy(finSPath,sPath);
		strcat(finSPath, "/");
		strcat(finSPath,sFile);


	}

	
	int dFileSize = strlen(dFile) + strlen(sFile) + 1;
	//Check destination permissions

	char finDPath[strlen(dPath)+dFileSize+6];
	char tempSwitch[strlen(dPath)+dFileSize+2];

	if(dIsDir){

		printf("dPath: %s\n", dPath);
		if(access(dPath, W_OK)!=0){
			free(dPath);
		 	free(sPath);
		 	free(dFile);
		 	free(sFile);
			free(CWD);
			return EXIT_FAILURE;
		}
		printf("dPath: %s\n", dPath);
		printf("dFile: %s\n", dFile);
		strcpy(finDPath, dPath);
		strcat(finDPath, "/");
		if(strcmp(dFile,"..") != 0  && strcmp(dFile, dPath)!=0){
			strcat(finDPath, dFile);
			strcat(finDPath, "/");
		}
		strcat(finDPath, sFile);
		strcat(finDPath, ".tmp");

		strcpy(tempSwitch, dPath);
		strcat(tempSwitch, "/");
		if(strcmp(dFile, "..")!=0 && strcmp(dFile, dPath)!=0){
			strcat(tempSwitch, dFile);
			strcat(tempSwitch, "/");
		}
		strcat(tempSwitch, sFile);
	
	}else{
		//Check write permissions of CWD
		if(access(CWD, W_OK)!=0){
			free(dPath);
		 	free(sPath);
		 	free(dFile);
		 	free(sFile);
			free(CWD);
			return EXIT_FAILURE;
		}

		strcpy(finDPath, dPath);
		strcat(finDPath, "/");
		strcat(finDPath, dFile);
		strcat(finDPath, ".tmp");

		strcpy(tempSwitch, dPath);
		strcat(tempSwitch, "/");
		strcat(tempSwitch, dFile);
	}
	free(CWD);
	free(sPath);
	free(sFile);
	free(dPath);
	free(dFile);


	printf("Source Path: %s\n", finSPath);
	printf("Destination Path: %s\n", finDPath);
	printf("Destination Rename: %s\n", tempSwitch);
	printf("Here\n");
	
	
	if(link(finSPath, finDPath)<0){
		printf("Fail 1\n");
		return EXIT_FAILURE;
	} 

	if(unlink(finSPath)<0){
		printf("Fail 2\n");
		return EXIT_FAILURE;
	}

	if(rename(finDPath, tempSwitch)<0){
		printf("Fail 3\n");
		return EXIT_FAILURE;
	}
	*/
	return EXIT_SUCCESS;
}




int main(int argc, char* argv[]){
	/*
	if(pathIsDir("/play")){
		printf("Directory\n");
	}else{
		printf("Not\n");
	}

	if(pathIsDir("/play/directory")){
		printf("Directory\n");
	}else{
		printf("Not\n");
	}

	if(pathIsDir("play")){
		printf("Directory\n");
	}else{
		printf("Not\n");
	}

	if(pathIsDir("play/directory")){
		printf("Directory\n");
	}else{
		printf("Not\n");
	}

	if(pathIsDir("/play/hello")){
		printf("Directory\n");
	}else{
		printf("Not\n");
	}

	if(pathIsDir("play/hello")){
		printf("Directory\n");
	}else{
		printf("Not\n");
	}

	if(pathIsDir("/play/goodbye")){
		printf("Directory\n");
	}else{
		printf("Not\n");
	}

	if(pathIsDir("play/goodbye")){
		printf("Directory\n");
	}else{
		printf("Not\n");
	}*/
	
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