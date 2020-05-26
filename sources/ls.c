#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdlib.h>
#include<errno.h>
#include<stdbool.h>
#include<string.h>


int main(int argc, char *argv[]){

	char dirPath[BUFSIZ];
	getcwd(dirPath, BUFSIZ);

	char* permission = "----------";
	bool lFlag, aFlag;


	lFlag = aFlag = false;

	int opt = 0;

	while((opt = getopt(argc, argv, "la"))!= -1){
		switch(opt){
			case 'l':
				lFlag = true;
				break;
			case 'a':
				aFlag = true;
				break;
			default:
				printf("Unknown flag %c\n", opt);
				break;
		}
	}

	DIR *directory;
	if(!(directory = opendir(dirPath))){
		printf("%s: %s\n", argv[0], strerror(errno));
		return EXIT_FAILURE;
	}

	struct dirent *dStruct;
	while((dStruct = readdir(directory))){
		
		if(!aFlag){
			if(strcmp(dStruct->d_name,".") == 0 || strcmp(dStruct->d_name,"..")==0) continue;
			if((dStruct->d_name)[0] == '.') continue;
		}

		if(lFlag){
			struct stat myStat;
			char* preText = dirPath;
			char* itemPath = malloc(sizeof(dirPath)+sizeof(dStruct->d_name));
			sprintf(itemPath, "%s/%s", preText, dStruct->d_name);
			printf("%s\n", itemPath);
		

			free(itemPath);
		}

		printf("%s  ", dStruct->d_name);
	}

	printf("\n");

	closedir(directory);

	return EXIT_SUCCESS;
}