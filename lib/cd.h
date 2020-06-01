int changeDirectoryUtil(char* directory, char* startLocAlias, char* startLocActual){ //Directory is the location to go, startLoc is the 
	
	if(!startLocAlias || !startLocActual) return EXIT_FAILURE;

	if(strcmp(directory, ".") == 0) return EXIT_SUCCESS;	//User wants to stay in current directory

	char* startLocAliasCopy = NULL;
	memcpy(startLocAliasCopy, startLocAlias, strlen(startLocAlias)+1);

	char* startLocActualCopy = NULL;
	memcpy(startLocActualCopy, startLocActual, strlen(startLocActual)+1);


	if(strcmp(directory,"..")==0){ 							//User wants to go to parent directory

		if(strcmp(startLocAliasCopy, "/")==0){				//User is at aliased root and should not change directory
			free(startLocAliasCopy);
			free(startLocActualCopy);	
			return EXIT_SUCCESS;
		}

		trimToChar(startLocAliasCopy,'/', strlen(startLocAliasCopy));	//Trim aliased path by one directory
		trimToChar(startLocActualCopy,'/', strlen(startLocActualCopy));	//Trim actual path by one directory

		if(setDirectory(startLocAliasCopy, startLocActualCopy) < 0){					//Attempt to set the directory
			free(startLocAliasCopy);
			free(startLocActualCopy);
			return EXIT_FAILURE; 				//Directory wasn't set for some reason, failed call will cause path reset
		}

	}else{													//User wants to go to child directory
		char *modAliasPath, *modActualPath;
		if(!(modAliasPath = addToPath(startLocAliasCopy, directory))){		//Append child directory to alias path
			fprintf(stderr, "cd: Failed to append path (alias)\n");
			free(startLocAliasCopy);
			free(startLocActualCopy);
			return EXIT_FAILURE;
		}

		if(!(modActualPath = addToPath(startLocActualCopy, directory))){	//Append child directory to actual path
			fprintf(stderr, "cd: Failed to append path (actual)\n");
			free(modAliasPath);
			free(startLocAliasCopy);
			free(startLocActualCopy);
			return EXIT_FAILURE;
		}

		if(setDirectory(modAliasPath, modActualPath) < 0){					//Attempt to set the directory
			free(modAliasPath);
			free(modActualPath);
			free(startLocAliasCopy);
			free(startLocActualCopy);
			return EXIT_FAILURE; 				//Directory wasn't set for some reason, failed call will cause path reset
		}


		free(modAliasPath);
		free(modActualPath);
	}

	free(startLocAliasCopy);
	free(startLocActualCopy);
	return EXIT_SUCCESS;
}

/*

	char* actualCWD = getcwd(NULL, 0);
	if(!actualCWD){
		fprintf(stderr, "cd: Failed to load current directory (actual): %s\n", strerror(errno));
		free(startLocCopy);
		return EXIT_FAILURE;
	}

	if(strcmp(directory, "..")==0){				//User wants to travel to parent directory
		if(strcmp(startLocCopy,"/") == 0){
			free(startLocCopy);
			return EXIT_SUCCESS;				//Use should not be able to go be aliased root
		}

		//Trim the alias path by one directory
		trimToChar(startLocCopy, '/', strlen(startLocCopy));

		//Trim the actual path by one directory
		
		trimToChar(actualCWD, '/', strlen(actualCWD));

		//Attempt to set the new directory
		if(setDirectory(startLocCopy, actualCWD) < 0){
			free(actualCWD);
			free(startLocCopy);
			return EXIT_FAILURE; 				//Directory wasn't set for some reason, failed call will cause path reset
		}


	}else{										//User wants to go to child directory
		//Use startLoccopy and actualCWD
	}
	free(actualCWD);
	free(startLocCopy);
	return EXIT_SUCCESS;
*/



	/*
	if(strcmp(directory, "..")==0){
		if(strcmp(startLocCopy, "/")==0){
			free(startLocCopy);
			return EXIT_SUCCESS;
		}
		
		trimToChar(startLocCopy, '/', strlen(oldCpy));
		
		char* actualCWD = getcwd(NULL, 0);
		trimToChar(actualCWD, '/', strlen(actualCWD));

		if(setDirectory(oldCpy, actualCWD) < 0){
			free(actualCWD);
			free(oldCpy);
			return EXIT_FAILURE;
		}

		free(actualCWD);
	}
	*/

int changeDirectory(char* propPath){

	char *startAliasLoc, *startActualLoc;

	char *prevAliasCWD = getenv("myCWD");

	if(!prevAliasCWD){
		fprintf(stderr, "cd: Failed to load current directory: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	char *prevActualCWD = getcwd(NULL, 0);

	if(!prevActualCWD){
		fprintf(stderr, "cd: Failed to laod (actual) current directory: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	if(propPath[0]=='/'){
		startActualLoc = getenv("myRoot");
		if(!startActualLoc){
			fprintf(stderr, "cd: Failed to load (aliased) root: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		char foo[] = "/";
		startAliasLoc = foo;
	}else{
		startAliasLoc = prevAliasCWD;
		startActualLoc = prevActualCWD;
	}

	char* breakup;
	breakup = strtok(propPath,"/ \n\t");

	do{
		//Needs to get updated starting alias and actual
		if(changeDirectoryUtil(breakup, startAliasLoc, startActualLoc) == EXIT_FAILURE){
			setDirectory(prevAliasCWD, prevActualCWD);
			free(prevActualCWD);
			return EXIT_FAILURE;
		}
	}while((breakup = strtok(NULL,"/ \n\t")));

	free(prevActualCWD);
	return EXIT_SUCCESS;
}
	/*

	do{
		if(changeDirectoryUtil(breakup, oldCWD) == EXIT_FAILURE){
			revertDirectory(oldCWD, actualCWD);
			free(actualCWD);
			return EXIT_FAILURE;
		}

	}while((breakup = strtok(NULL,"/ \n\t")));

	free(actualCWD);
	*/
	
	/*
	if(!propPath) return EXIT_FAILURE;
	
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
	*/