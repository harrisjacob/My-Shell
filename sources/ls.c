#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<dirent.h>
#include<stdlib.h>
#include<errno.h>
#include<stdbool.h>
#include<string.h>

void usage();

void trimDate(char** myDate);

struct dirItem{
	char* perm;
	long int num_Links;
	char* userName;
	char* groupName;
	size_t f_size;
	char* fileDate;
	char* fileName;
	struct dirItem* nextItem;
};

void printEntries(struct dirItem*);

void freeEntries(struct dirItem*);

int main(int argc, char *argv[]){

	char* dirPath;
	char permission[11];
	bool lFlag, aFlag, res;
	char* readPath = NULL;
	struct dirItem *newNode, *headNode, *temp;
	headNode = temp = NULL;

	lFlag = aFlag = res = false;

	int opt = 0;

	while((opt = getopt(argc, argv, "-hla"))!= -1){
		switch(opt){
			case 'h':
				usage();
				return EXIT_SUCCESS;
			case 'l':
				lFlag = true;
				break;
			case 'a':
				aFlag = true;
				break;
			case 1:
				if(!readPath) readPath = optarg;
				break;
			default:
				fprintf(stderr, "Try 'ls -h' for more information\n");
				return EXIT_FAILURE;
		}
	}

	
	if(readPath){
		if(*readPath == '/'){
			char* root;
			if(!(root =  getenv("myRoot"))){
				fprintf(stderr, "%s: Could not access root directory\n", argv[0]);
			}
			if(!(dirPath = malloc(strlen(root)+strlen(readPath)+1))){
				return EXIT_FAILURE;
			}
			strcpy(dirPath, root);
			strcat(dirPath, argv[1]);
		}else{
			if(!(dirPath = strdup(readPath))){
				fprintf(stderr, "%s: Allocation failed for specified directory\n", argv[0]);
				return EXIT_FAILURE;
			}
		}
	}else{
		if(!(dirPath = getcwd(NULL, 0))){
			fprintf(stderr, "%s: Failed to fetch current working directory: %s\n", argv[0], strerror(errno));
			return EXIT_FAILURE;
		}
	}
	


	DIR *directory;
	if(!(directory = opendir(dirPath))){
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		free(dirPath);
		return EXIT_FAILURE;
	}

	if(lFlag){
		struct stat dirStat;
		int dirBlocks;
		if(stat(dirPath, &dirStat)<0){
			fprintf(stderr, "%s: stat failed on directory: %s\n", argv[0], strerror(errno));
			dirBlocks = -1;
		}else{
			dirBlocks = dirStat.st_blocks;
		}
		printf("total %i\n", dirBlocks);
	}

	struct dirent *dStruct;
	while((dStruct = readdir(directory))){
		
		if(!aFlag){
			if(strcmp(dStruct->d_name,".") == 0 || strcmp(dStruct->d_name,"..")==0) continue;
			if((dStruct->d_name)[0] == '.') continue;
		}

		res = true;
		
		if(lFlag){
			struct stat myStat;
			char* preText = dirPath;
			char* itemPath;
			if(!(itemPath = malloc(sizeof(dirPath)+sizeof(dStruct->d_name)))){
				fprintf(stderr, "%s: Memory allocation failed for %s directory string\n", argv[0], argv[0]);
				continue;
			} 
			
			if(sprintf(itemPath, "%s/%s", preText, dStruct->d_name) < 0){
				fprintf(stderr, "%s: path could not be resolved for %s\n", argv[0], dStruct->d_name);
				free(itemPath);
				continue;
			}

			if(stat(itemPath, &myStat)<0){
				fprintf(stderr, "%s: stat failed on %s: %s\n", argv[0], dStruct->d_name, strerror(errno));
				free(itemPath);
				continue;
			}

			free(itemPath);

			if(!(newNode = malloc(sizeof(struct dirItem)))){
				fprintf(stderr, "%s: Couldn't allocate directory entry struture for %s: %s\n", argv[0], dStruct->d_name, strerror(errno));
				continue;
			}

			if(!headNode){
				headNode = newNode;
			}else{
				temp->nextItem = newNode;
			}

			switch(myStat.st_mode & S_IFMT){
				case S_IFBLK: permission[0] = 'b'; break;
				case S_IFCHR: permission[0] = 'c'; break;
				case S_IFDIR: permission[0] = 'd'; break;
				case S_IFIFO: permission[0] = 'p'; break;
				case S_IFLNK: permission[0] = 'l'; break;
				case S_IFREG: permission[0] = '-'; break;
				default: permission[0] = '-'; break;
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
			
			newNode->perm = strdup(permission);
			newNode->num_Links = myStat.st_nlink;

			struct passwd* userPass;
			char* username = (!(userPass = getpwuid(myStat.st_uid))) ? "" : userPass->pw_name;

			newNode->userName = strdup(username);

			struct group* groupID;
			char* groupName = (!(groupID = getgrgid(myStat.st_gid))) ? "" : groupID->gr_name;

			newNode->groupName = strdup(groupName);
			newNode->f_size = myStat.st_size;

			char* myDate = ctime(&myStat.st_mtime);
			trimDate(&myDate);

			newNode->fileDate = strdup(myDate);
			newNode->fileName = strdup(dStruct->d_name);
			
			temp = newNode;
			//printf("%s %li %11s %11s %4zi %s %s\n", permission, myStat.st_nlink, username, groupName, myStat.st_size, myDate, dStruct->d_name);
		}else{
			printf("%s  ", dStruct->d_name);
		}
	}

	if(headNode){
		printEntries(headNode);
		freeEntries(headNode);
	}

	printf((!lFlag && res) ? "\n" : "");
	
	closedir(directory);
	free(dirPath);
	return EXIT_SUCCESS;
}

//Trims the week day, seconds, and year from the specified date
void trimDate(char** myDate){
	*(myDate)+=4; //Trim off day of week
	
	int i, cCount = 0;
	for(i=0;cCount<2;i++){
		if((*myDate)[i] == ':') cCount+=1;
	}

	(*myDate)[i-1] = '\0'; //Trim at 2nd colon
}

void usage(){
	printf("Usage: ls [OPTION]\n");
	printf("List information about the FILEs\n");
	printf("Entries are sorted alphabetically\n\n");
	printf("Supported flags:\n");
	printf("  -a\tdo not ignore entries starting with .\n");
	printf("  -h\thelp (more information)\n");
	printf("  -l\tuse long listing format\n");
}

void printEntries(struct dirItem* d){
	while(d){
		printf("%s %li %11s %11s %4zi %s %s\n", d->perm, d->num_Links, d->userName, d->groupName, d->f_size, d->fileDate, d->fileName);
		d = d->nextItem;
	}
}

void freeEntries(struct dirItem* d){
	struct dirItem *temp;
	while(d){
		temp = d->nextItem;
		free(d);
		d = temp;
	}
}