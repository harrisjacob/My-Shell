#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdbool.h>
#include<dirent.h>
#include "common.h"
#include "cd.h"


#define MAX_ARGS 10

int setMyENV(char* currProg);

int main(int argc, char *argv[]){

	if(setMyENV(argv[0]) == EXIT_FAILURE) return EXIT_FAILURE;

	char *dirPath;
	long dirPathSize = pathconf(".", _PC_PATH_MAX);
	if(!(dirPath = malloc((size_t)dirPathSize))){
		fprintf(stderr,"%s: Memory allocation failed for CWD string\n", argv[0]);
		return EXIT_FAILURE;
	}

	if(!(getcwd(dirPath, (size_t)dirPathSize))){
		fprintf(stderr,"%s: Failed to retrieve CWD path\n", argv[0]);
		free(dirPath);
		return EXIT_FAILURE;
	}
		
	char* utilDirect;
	char progDir[] = "programs";

	if(!(utilDirect = addToPath(dirPath, progDir))){
		free(dirPath);
		return EXIT_FAILURE;
	}
	free(dirPath);

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

		DIR *directory;
		if(!(directory = opendir(utilDirect))){
			fprintf(stderr, "%s: Couldn't open utility program directory: %s\n", argv[0], strerror(errno));
			continue;
		}

		struct dirent *oneDir, dStruct;
		while((oneDir = readdir(directory))){
			if(strcmp(program, oneDir->d_name) == 0){
				validProgram = true;
				break;
			}
		}

		if(strcmp(program, "cd") == 0 || strcmp(program, "echo")==0) validProgram = true;

		if(!validProgram){
			printf("-MyShell: %s: command not found\n", program);
			closedir(directory);
			continue;
		}

		while(argIndex < MAX_ARGS-1 && (args[argIndex++] = strtok(NULL, " \t\n")));

		if(strcmp(program, "cd") == 0){
			changeDirectory(args[1]);
			closedir(directory);
			continue;
		}

		if(strcmp(program, "echo") == 0){
			printf("%s\n", (args[1]) ? args[1] : "");
			continue;
		}

		memcpy(&dStruct, oneDir, sizeof(struct dirent));
		closedir(directory);

		if(!(fullProgramPath = addToPath(utilDirect, dStruct.d_name))) continue;
		

		args[0] = fullProgramPath;

		pid_t newPID;
		if((newPID = fork()) < 0){
			fprintf(stderr, "%s: Fork process failed: %s\n", argv[0], strerror(errno));
			free(fullProgramPath);
			free(utilDirect);
			return EXIT_FAILURE;
		}else if(newPID == 0){
			execvp(fullProgramPath, args);
			fprintf(stderr, "%s: exec failed to launch program: %s\n", argv[0], strerror(errno));
			free(fullProgramPath);
			free(utilDirect);
			return EXIT_FAILURE;
		}else{
			waitpid(newPID, NULL, 0);
		}
		free(fullProgramPath);
	}

	free(utilDirect);
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

	if(setenv("myRoot", dirPath, 0)!=0){
		fprintf(stderr, "%s: Failed to write root location environment variable: %s\n", currProg,strerror(errno));
		return EXIT_FAILURE;
	}

	if(setenv("myCWD", "/", 0)!=0){
		fprintf(stderr, "%s: Failed to write root path environment variable: %s\n", currProg, strerror(errno));
		return EXIT_FAILURE;
	}

	free(dirPath);

	if(!getenv("myRoot")){
		fprintf(stderr, "%s: Check root environment variable failed\n", currProg);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}