#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

int main(int argc, char *argv[]){

	char* dName;
	if(argc == 1) dName = ".";
	
	DIR *directory;
	if(!(directory = opendir(dName))){
		printf("%s: %s\n", argv[0], strerror(errno));
		return EXIT_FAILURE;
	}

	struct dirent *dStruct;
	int count = 0;
	while((dStruct = readdir(directory))){
		if(strcmp(dStruct->d_name,".") == 0|| strcmp(dStruct->d_name,"..")==0) continue;
		if(count > 0 )printf(" ");
		printf("%s ", dStruct->d_name);
		count+=1;
	}

	printf("\n");

	closedir(directory);

	return EXIT_SUCCESS;
}