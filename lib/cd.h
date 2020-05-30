

int changeDirectory(char* propPath){

	if(!propPath) return EXIT_FAILURE;
	if(strcmp(propPath, ".") == 0) return EXIT_SUCCESS;

	char* oldCWD = getenv("myCWD");

	if(!oldCWD){
		fprintf(stderr, "cd: Failed to load current directory: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	if(strcmp(propPath, "..")==0){
		if(strcmp(oldCWD, "/")==0){
			return EXIT_SUCCESS;
		}
		//step up one directory
	}
	
	trimToChar(propPath, '/', 1);

	char* newPath, *prePath;

	if(propPath[0] == '/'){
		//start at root
	}else{
		if(strcmp(oldCWD,"/")!=0){
			if(!(prePath = preFormatPath(propPath))){
				return EXIT_FAILURE;
			}

			if(!(newPath = addToPath(oldCWD, prePath))){
				free(prePath);
				return EXIT_FAILURE;
			}
			free(prePath);
		}else{
			if(!(newPath = addToPath(oldCWD, propPath))){
				return EXIT_FAILURE;
			}
		}

		if(setDirectory(newPath, propPath) < 0){
			free(newPath);
			return EXIT_FAILURE;
		}

		free(newPath);
	}

	
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