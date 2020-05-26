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

	char permission[11] = "----------";
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
		
			if(stat(itemPath, &myStat)<0){
				printf("%s: Couldn't read %s: %s\n", argv[0], dStruct->d_name, strerror(errno));
				continue;
			}

			switch(myStat.st_mode & S_IFMT){
				case S_IFBLK: permission[0] = 'b'; break;
				case S_IFCHR: permission[0] = 'c'; break;
				case S_IFDIR: permission[0] = 'd'; break;
				case S_IFIFO: permission[0] = 'p'; break;
				case S_IFLNK: permission[0] = 'l'; break;
				case S_IFREG: permission[0] = '-'; break;
				default: printf("Unknown file type\n"); break;
			}
			
			permission[1] = (myStat.st_mode & S_IRUSR) ? 'r' : '-';
			permission[2] = (myStat.st_mode & S_IWUSR) ? 'w' : '-';
			permission[3] = (myStat.st_mode & S_IXUSR) ? 'x' : '-';
			permission[4] = (myStat.st_mode & S_IRGRP) ? 'r' : '-';
			permission[5] = (myStat.st_mode & S_IWGRP) ? 'w' : '-';
			permission[6] = (myStat.st_mode & S_IXGRP) ? 'x' : '-';
			permission[7] = (myStat.st_mode & S_IROTH) ? 'r' : '-';
			permission[8] = (myStat.st_mode & S_IWOTH) ? 'w' : '-';
			permission[9] = (myStat.st_mode & S_IXOTH) ? 'x' : '-';
			char* hello = permission;
			printf("%s\t%s\n", hello, dStruct->d_name);
			free(itemPath);
		}

		//printf("%s  ", dStruct->d_name);
	}

	printf("\n");

	closedir(directory);

	return EXIT_SUCCESS;
}