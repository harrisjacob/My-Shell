#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdbool.h>
#include<dirent.h>
#include "cd.h"
#include "common.h"

#define MAX_ARGS 10

int setMyENV(char* currProg);

int main(int argc, char *argv[]){

	if(setMyENV(argv[0]) == EXIT_FAILURE) return EXIT_FAILURE;

	while(true){
		
		char* program, *fullProgramPath;
		char* args[MAX_ARGS];
		char buff[BUFSIZ];
		int argIndex = 1;
		bool validProgram = false;
		
		printf("MyShell$ ");

		fgets(buff, BUFSIZ, stdin);

		if(!(program = strtok(buff, " \t\n")))continue;
		if(strcmp(program, "quit") == 0 || strcmp(program, "exit")==0) break;


		char *dirPath;
		long dirPathSize = pathconf(".", _PC_PATH_MAX);
		if(!(dirPath = malloc((size_t)dirPathSize))){
			fprintf(stderr,"%s: Memory allocation failed for CWD string\n", argv[0]);
			continue;
		}

		if(!(getcwd(dirPath, (size_t)dirPathSize))){
			fprintf(stderr,"%s: Failed to retrieve CWD path\n", argv[0]);
			free(dirPath);
			continue;
		}
		
		
		char* utilDirect;
		char progDir[] = "programs";

		if(!(utilDirect = addToPath(dirPath, progDir))){
			free(dirPath);
			continue;
		}
		free(dirPath);

		
		DIR *directory;
		if(!(directory = opendir(utilDirect))){
			fprintf(stderr, "%s: Couldn't open utility program directory: %s\n", argv[0], strerror(errno));
			free(utilDirect);
			continue;
		}

		struct dirent *oneDir, dStruct;
		while((oneDir = readdir(directory))){
			if(strcmp(program, oneDir->d_name) == 0){
				validProgram = true;
				break;
			}
		}

		if(strcmp(program, "cd") == 0) validProgram = true;

		if(!validProgram){
			printf("-MyShell: %s: command not found\n", program);
			free(utilDirect);
			closedir(directory);
			continue;
		}

		while(argIndex < MAX_ARGS-1 && (args[argIndex++] = strtok(NULL, " \t\n")));

		if(strcmp(program, "cd") == 0){
			changeDirectory(args[1]);
			free(utilDirect);
			closedir(directory);
			continue;
		}

		memcpy(&dStruct, oneDir, sizeof(struct dirent));
		closedir(directory);


		if(!(fullProgramPath = addToPath(utilDirect, dStruct.d_name))){
			free(utilDirect);
			continue;
		}
		free(utilDirect);

		args[0] = fullProgramPath;

		pid_t newPID;
		if((newPID = fork()) < 0){
			fprintf(stderr, "%s: Fork process failed: %s\n", argv[0], strerror(errno));
			return EXIT_FAILURE;
		}else if(newPID == 0){
			execvp(fullProgramPath, args);
			fprintf(stderr, "%s: exec failed to launch program: %s\n", argv[0], strerror(errno));
			return EXIT_FAILURE;
		}else{
			waitpid(newPID, NULL, 0);
		}
		free(fullProgramPath);
	}

	return EXIT_SUCCESS;
}

int setMyENV(char* currProg){

	char *dirPath;
	long dirPathSize = pathconf(".", _PC_PATH_MAX);
	if(!(dirPath = malloc((size_t)dirPathSize))){
		fprintf(stderr,"%s: Memory allocation failed for CWD string\n", currProg);
		return EXIT_FAILURE;
	}

	if(!(getcwd(dirPath, (size_t)dirPathSize))){
		fprintf(stderr,"%s: Failed to retrieve CWD path\n", currProg);
		free(dirPath);
		return EXIT_FAILURE;
	}

	const char envVar[] = "myRoot";
	const char myCWD[] = "myCWD";
	const char newRoot[] = "/";

	if(setenv(envVar, dirPath, 0)!=0){
		fprintf(stderr, "%s: Failed to write %s environment variable: %s\n", currProg, envVar, strerror(errno));
		return EXIT_FAILURE;
	}

	if(setenv(myCWD, newRoot, 0)!=0){
		fprintf(stderr, "%s: Failed to write %s environment variable: %s\n", currProg, myCWD, strerror(errno));
		return EXIT_FAILURE;
	}

	free(dirPath);

	if(!getenv(envVar)){
		fprintf(stderr, "%s: Check root environment variable failed\n", currProg);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}