#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdbool.h>
#include<dirent.h>

#define MAX_ARGS 10

int main(int argc, char *argv[]){

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
		
		/*
		char dirPath[BUFSIZ];
		getcwd(dirPath, BUFSIZ);
		*/

		
		char* utilDirect;
		if(!(utilDirect = malloc(dirPathSize+strlen("/programs")+1))){;
			free(dirPath);
			fprintf(stderr, "%s: Memory allocation failed for utility string\n", argv[0]);
			continue;
		}
		
		

		if(sprintf(utilDirect, "%s/programs", dirPath) < 0){
			fprintf(stderr, "%s: path could not be resolved for utility programs directory\n", argv[0]);
			free(utilDirect);
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

		memcpy(&dStruct, oneDir, sizeof(struct dirent));
		closedir(directory);

		if(!validProgram){
			printf("-MyShell: %s: command not found\n", program);
			free(utilDirect);
			continue;
		}

		//Read in the remaining arguments
		while(argIndex < MAX_ARGS  && (args[argIndex++] = strtok(NULL, " \t\n")));


		if(!(fullProgramPath = malloc(sizeof(utilDirect)+sizeof(dStruct.d_name)))){
			fprintf(stderr, "%s: Memory allocation failed for program path\n", argv[0]);
			free(utilDirect);
			continue;
		}

		if(sprintf(fullProgramPath, "%s/%s", utilDirect, dStruct.d_name) < 0){
			fprintf(stderr, "%s: path could not be resolved for %s\n", argv[0], dStruct.d_name);
			free(utilDirect);
			free(fullProgramPath);
			continue;
		}
		
		free(utilDirect);

		args[0] = fullProgramPath;

		int p = 0;
		while(args[p]){
			printf("%s\n", args[p++]);
		}

		free(fullProgramPath);
	}
/*
	pid_t newPID;
	if((newPID = fork()) < 0){
		fprintf(stderr, "%s: Fork process failed: %s\n", argv[0], strerror(errno));
		return EXIT_FAILURE;
	}else if(newPID == 0){
		printf("Child process!\n");
		return EXIT_SUCCESS;
	}else{
		waitpid(newPID, NULL, 0);
		printf("Parent process!\n");
	}
*/
	return EXIT_SUCCESS;
}

/*
	printf("Program: %s\n", program);
	int i = 0;
	while(args[i]){
		printf("%s\n", args[i++]);
	}
*/