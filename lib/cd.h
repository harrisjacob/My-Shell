int changeDirectoryUtil(char* directory, char* startLocAlias, char* startLocActual){ //Directory is the location to go, startLoc is the 
	
	if(!startLocAlias || !startLocActual) return EXIT_FAILURE;

	if(strcmp(directory, ".") == 0) return EXIT_SUCCESS;	//User wants to stay in current directory

	/*
	char* startLocAliasCopy = NULL;
	memcpy(startLocAliasCopy, startLocAlias, strlen(startLocAlias)+1);

	char* startLocActualCopy = NULL;
	memcpy(startLocActualCopy, startLocActual, strlen(startLocActual)+1);
	*/

	if(strcmp(directory,"..")==0){ 							//User wants to go to parent directory

		if(strcmp(startLocAlias, "/")==0){				//User is at aliased root and should not change directory
			//free(startLocAliasCopy);
			//free(startLocActualCopy);	
			return EXIT_SUCCESS;
		}

		trimToChar(startLocAlias,'/', strlen(startLocAlias));	//Trim aliased path by one directory
		trimToChar(startLocActual,'/', strlen(startLocActual));	//Trim actual path by one directory

		if(setDirectory(startLocAlias, startLocActual) < 0){					//Attempt to set the directory
			//free(startLocAliasCopy);
			//free(startLocActualCopy);
			return EXIT_FAILURE; 				//Directory wasn't set for some reason, failed call will cause path reset
		}

	}else{													//User wants to go to child directory
		char *modAliasPath, *modActualPath;
		if(!(modAliasPath = addToPath(startLocAlias, directory))){		//Append child directory to alias path
			fprintf(stderr, "cd: Failed to append path (alias)\n");
			//free(startLocAliasCopy);
			//free(startLocActualCopy);
			return EXIT_FAILURE;
		}

		if(!(modActualPath = addToPath(startLocActual, directory))){	//Append child directory to actual path
			fprintf(stderr, "cd: Failed to append path (actual)\n");
			free(modAliasPath);
			//free(startLocAliasCopy);
			//free(startLocActualCopy);
			return EXIT_FAILURE;
		}

		if(setDirectory(modAliasPath, modActualPath) < 0){					//Attempt to set the directory
			free(modAliasPath);
			free(modActualPath);
			//free(startLocAliasCopy);
			//free(startLocActualCopy);
			return EXIT_FAILURE; 				//Directory wasn't set for some reason, failed call will cause path reset
		}


		free(modAliasPath);
		free(modActualPath);
	}

	//free(startLocAliasCopy);
	//free(startLocActualCopy);
	return EXIT_SUCCESS;
}

int changeDirectory(char* propPath){

	char *startAliasLoc, *startActualLoc, *currentAliasCWD, *currentActualCWD;
	char foo[] = "/";

	char* temp = getenv("myCWD");
	if(!temp){
		fprintf(stderr, "cd: Failed to load current directory: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	char* prevAliasCWD;
	if(!(prevAliasCWD = malloc(strlen(temp)+1))){
		fprintf(stderr,"Memory allocation failed for safety alias directory\n");
		return EXIT_FAILURE;
	}

	memcpy(prevAliasCWD, temp, strlen(temp)+1);

	char *prevActualCWD = getcwd(NULL, 0);

	if(!prevActualCWD){
		fprintf(stderr, "cd: Failed to load (actual) current directory: %s\n", strerror(errno));
		free(prevAliasCWD);
		return EXIT_FAILURE;
	}

	char* breakup = strtok(propPath,"/ \n\t");

	do{
		currentAliasCWD = getenv("myCWD");

		if(!currentAliasCWD){
			fprintf(stderr, "cd: Failed to load current directory: %s\n", strerror(errno));
			free(prevAliasCWD);
			free(prevActualCWD);
			return EXIT_FAILURE;
		}

		currentActualCWD = getcwd(NULL, 0);

		if(!currentActualCWD){
			fprintf(stderr, "cd: Failed to load (actual) current directory: %s\n", strerror(errno));
			free(prevAliasCWD);
			free(prevActualCWD);
			return EXIT_FAILURE;
		}

		if(propPath[0]=='/'){
			startActualLoc = getenv("myRoot");
			if(!startActualLoc){
				fprintf(stderr, "cd: Failed to load (aliased) root: %s\n", strerror(errno));
				free(prevAliasCWD);
				free(prevActualCWD);
				free(currentActualCWD);
				return EXIT_FAILURE;
			}

			startAliasLoc = foo;
		}else{
			startAliasLoc = currentAliasCWD;
			startActualLoc = currentActualCWD;
		}

		//Needs to get updated starting alias and actual
		if(changeDirectoryUtil(breakup, startAliasLoc, startActualLoc) == EXIT_FAILURE){
			setDirectory(prevAliasCWD, prevActualCWD);
			free(prevAliasCWD);
			free(prevActualCWD);
			free(currentActualCWD);
			return EXIT_FAILURE;
		}

		free(currentActualCWD);

	}while((breakup = strtok(NULL,"/ \n\t")));

	free(prevAliasCWD);
	free(prevActualCWD);
	return EXIT_SUCCESS;
}