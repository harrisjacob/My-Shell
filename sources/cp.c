#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

void usage(void);

int main(int argc, char* argv[]){
	
	if(argc < 3){
		usage();
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}


void usage(){
	printf("Usage: cp SOURCE DEST\n");
	printf("   or: cp SOURCE DIRECTORY\n");
	printf("Copy SOURCE to DEST or DIRECTORY\n");
}