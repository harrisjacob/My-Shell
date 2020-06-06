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
int splitPath(char** pathAlc, char** fileAlc, char* baseP){

		char* split = findTailChar(baseP, '/', 1);
		int pathLen = split - baseP;
		int fileLen = baseP + strlen(baseP) - split;

		if(split == baseP){
			if(!(*pathAlc = strdup(baseP))) return EXIT_FAILURE;
			if(!(*fileAlc = strdup(baseP))){
				free(*pathAlc);
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}

		if(*baseP == '/'){

			char *aliasRoot;
			if(!(aliasRoot = getenv("myRoot"))) return EXIT_FAILURE;
			if(!(*pathAlc = malloc(strlen(aliasRoot)+ pathLen + 1))) return EXIT_FAILURE;
			strcpy(*pathAlc, aliasRoot);
			char* nullC = *pathAlc + strlen(*pathAlc);
			memcpy(nullC, baseP, pathLen);
			*(*pathAlc + strlen(aliasRoot)+pathLen) = '\0';

		}else{

			if(!(*pathAlc = malloc(pathLen + 1))) return EXIT_FAILURE;
			memcpy(*pathAlc, baseP, pathLen);
			*(*pathAlc + pathLen) = '\0';

		}

		if(!(*fileAlc = malloc(fileLen + 1))){
			free(*pathAlc);
			return EXIT_FAILURE;
		}

		strcpy(*fileAlc, split+1);


		//printf("Path: $%s$\n", *pathAlc);
		//printf("File: $%s$\n", *fileAlc);

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

	int sIsDir = 0;
	int dIsDir = 0;
	char *dPath, *dFile, *sPath, *sFile;
	dPath = dFile = sPath = sFile = NULL;
	


	if(validPath(source) < 0){
		fprintf(stderr, "mv: Source cannot be above root\n");
		return EXIT_FAILURE;
	}

	if(validPath(dest) < 0){
		fprintf(stderr, "mv: Destination cannot be above root\n");
		return EXIT_FAILURE;
	}

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



	/*
	if(*source == '/'){
		char *aliasRoot;
		if(!(aliasRoot = getenv("myRoot")))return EXIT_FAILURE;


		if(!(sourceDirect = malloc(strlen(aliasRoot)+strlen(source)+1))){
			return EXIT_FAILURE;
		}
		strcpy(sourceDirect, aliasRoot);
		strcat(sourceDirect, source);
		
		//char sourceRoot[strlen(aliasRoot)+strlen(source)+1];
		
		//strcpy(sourceRoot, aliasRoot);
		//strcat(sourceRoot, source);
		//trimToChar(sourceRoot, '/', strlen(sourceRoot));
		//sourceDirect = sourceRoot;
		
	}else{
		if(!(sourceDirect = strdup(source))) return EXIT_FAILURE;
		
		
		//char sourceCopy[strlen(source)+1];
		//strcpy(sourceCopy, source);
		//trimToChar(sourceCopy, '/', strlen(sourceCopy));
		//sourceDirect = sourceCopy;
		
	}
	trimToChar(sourceDirect, '/', strlen(sourceDirect));
	*/
/*
	printf("Source Path: %s\n", sPath);
	printf("Source File: %s\n", sFile);
	printf("Dest Path: %s\n", dPath);
	printf("Dest File: %s\n", dFile);
*/



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

	return EXIT_SUCCESS;
}




int main(int argc, char* argv[]){

	/*
	char baseP[] = "/hello/mr/jacob";
	char *pathAlc=NULL, *fileAlc=NULL;
	splitPath(pathAlc, fileAlc, baseP);
	printf("\n");
	char baseP2[] = "hello/mr/jacob";
	splitPath(pathAlc, fileAlc, baseP2);
	*/
	
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