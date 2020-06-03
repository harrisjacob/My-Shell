#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<errno.h>

void usage(){
	printf("Usage: touch FILE\n");
	printf("Update the access and modification times of each FILE to the current time.\n");
	printf("\nA FILE argument that does not exist is created empty.\n");
}

int main(int argc, char* argv[]){

	if(strcmp(argv[1], "--help")==0){
		usage();
		return EXIT_SUCCESS;
	}

	int fd;
	if(argc < 2){
		fprintf(stderr, "touch: missing file operand\n");
		fprintf(stderr, "Try 'touch --help' for more information.\n");
		return EXIT_FAILURE;
	}

	if((fd = open(argv[1], O_CREAT, 00777))<0){
		fprintf(stderr, "touch: Could not create file %s: %s\n", argv[1], strerror(errno));
		return EXIT_FAILURE;
	}

	if(close(fd)<0){
		fprintf(stderr, "touch: Could not close file %s: %s\n", argv[1], strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}