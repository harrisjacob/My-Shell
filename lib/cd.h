int changeDirect(const char* newPath){

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

		
	}

	return EXIT_SUCCESS;

}