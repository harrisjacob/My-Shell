int changeDirectory(const char* newPath){

	if(!newPath) return EXIT_FAILURE;
	if(strcmp(newPath, ".") == 0) return EXIT_SUCCESS;

	const char* oldCWD = getenv("myCWD");

	if(!oldCWD){
		fprintf(stderr, "cd: Failed to load current directory: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	if(strcmp(newPath, "..")==0){
		if(strcmp(oldCWD, "/")==0){
			return EXIT_SUCCESS;
		}
		//step up one directory
		
	}
	/*
	if(newPath[0] == '/'){
		//start at root

	}else{
		//relative path
	}
	*/

	return EXIT_SUCCESS;

}

/*

	if(chdir(path)<0){
		fprintf(stderr, "Chdir failed: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	printf("Looks like it passed\n");
*/

//int changeDirectoryUtil