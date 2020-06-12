#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

void usage(void);
char* allocPath(char*);
int makeCopy(char*, char*, char*);


int main(int argc, char* argv[]){
	
	if(argc < 3){
		usage();
		return EXIT_FAILURE;
	}

	char *src, *dst;

	if(!(src = allocPath(argv[1]))) return EXIT_FAILURE;
	if(!(dst = allocPath(argv[2]))){
		free(src);
		return EXIT_FAILURE;
	} 

	return makeCopy(src, dst, argv[1]);
}


void usage(){
	printf("Usage: cp SOURCE DEST\n");
	printf("   or: cp SOURCE DIRECTORY\n");
	printf("Copy SOURCE to DEST or DIRECTORY\n");
}

char* allocPath(char* u_arg){
	if(!u_arg) return NULL;
	char* ret_Alloc;
	long path_max, pathLen;
	
	if(*u_arg == '/'){
		
		if((path_max = pathconf("/", _PC_PATH_MAX)) < 0){
			fprintf(stderr, "cp: Failed to get path length: %s\n", strerror(errno));
			return NULL;
		}
		char* root;
		if(!(root =  getenv("myRoot"))){
			fprintf(stderr, "cp: Could not access root directory\n");
		}

		if((pathLen = strlen(root) + strlen(u_arg)) >= path_max) return NULL;
		if(!(ret_Alloc = malloc(pathLen + 1))) return NULL;
		strcpy(ret_Alloc, root);
		strcat(ret_Alloc, u_arg);
	}else{
	
		if((path_max = pathconf(".", _PC_PATH_MAX)) < 0){
			fprintf(stderr, "cp: Failed to get path length: %s\n", strerror(errno));
			return NULL;
		}

		if((pathLen = strlen(u_arg)) >= path_max) return NULL;
		if(!(ret_Alloc = malloc(pathLen + 1))) return NULL;
		strcpy(ret_Alloc, u_arg);
	}

	return ret_Alloc;

}

int makeCopy(char* src, char* dst, char* src_name){
	int src_fd, dir_fd, dst_fd;

	if((src_fd = open(src, O_RDONLY)) < 0){
		fprintf(stderr, "cp: Failed to read source: %s\n", strerror(errno));
		free(src);
		free(dst);
		return EXIT_FAILURE;
	}

	struct stat srcStat;
	if(fstat(src_fd, &srcStat) < 0){
		fprintf(stderr, "cp: Failed to get source information: %s\n", strerror(errno));
		free(src);
		free(dst);
		return EXIT_FAILURE;
	}

	if(!S_ISREG(srcStat.st_mode)){
		fprintf(stderr, "cp: Current cp version only supports copying regular files\n");
		free(src);
		free(dst);
		return EXIT_FAILURE;
	}

	mode_t perm = srcStat.st_mode & 0777;

	if((dir_fd = open(dst, O_DIRECTORY)) < 0){
		if((dst_fd = creat(dst, perm)) < 0){
			fprintf(stderr, "cp: Failed to create destination file: %s\n", strerror(errno));
			free(src);
			free(dst);
			return EXIT_FAILURE;
		}
	}else{
		if((dst_fd = openat(dir_fd, src_name, O_CREAT|O_WRONLY|O_TRUNC, perm)) < 0){
			fprintf(stderr, "cp: Failed to create destination file: %s\n", strerror(errno));
			free(src);
			free(dst);
			return EXIT_FAILURE;
		}
	}

	char buff[BUFSIZ];
	ssize_t byteCount;

	while((byteCount = read(src_fd, buff, BUFSIZ)) > 0){
		if(write(dst_fd, buff, byteCount) != byteCount){
			fprintf(stderr, "cp: Failed to copy source file: %s\n", strerror(errno));
			free(src);
			free(dst);
			return EXIT_FAILURE;
		}
	}


	free(src);
	free(dst);
	return EXIT_SUCCESS;
}
