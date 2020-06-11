#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

void usage(void);
char* allocPath(char*);


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

	printf("Source: %s\n", src);
	printf("Destination: %s\n", dst);

	free(src);
	free(dst);

	return EXIT_SUCCESS;
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
