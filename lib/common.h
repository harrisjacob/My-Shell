char* myConcat(char* s1, char* s2){
	if(!s1 || !s2 ) return NULL;
	char* conc;
	if(!(conc = malloc(strlen(s1)+strlen(s2)+1))){
		fprintf(stderr, "Concatenation allocation failed\n");
		return NULL;
	}

	if(sprintf(conc,"%s%s", s1, s2) < 0){
		fprintf(stderr, "Concatentation build failed\n");
		free(conc);
		return NULL;
	}

	return conc;
}

char* preFormatPath(char* s1){
	char slash[] = "/";
	char* fPath;
	if(!(fPath = myConcat(slash, s1))){
		fprintf(stderr, "Couldn't preappend path character\n");
		return NULL;
	}
	return fPath;
}


char* addToPath(char* basePath, char* pathAdd){
	if(!basePath || !pathAdd) return NULL;
	char* newPath;
	if(!(newPath = malloc(strlen(basePath)+strlen(pathAdd)+2))){
		fprintf(stderr, "Path append allocation failed\n");
		return NULL;
	}
	if(strlen(basePath) == 1){
		if(sprintf(newPath,"/%s", pathAdd)<0){
			fprintf(stderr, "Path append build failed\n");
			free(newPath);
			return NULL;
		}
	}else{
		if(sprintf(newPath,"%s/%s", basePath, pathAdd)<0){
			fprintf(stderr, "Path append build failed\n");
			free(newPath);
			return NULL;
		}
	}

	return newPath;
}

int setDirectory(char* localPath, char* uPath){ //local path is aliased, uPath is actual path
	
	if(!localPath || !uPath) return -1;
	if(chdir(uPath) < 0){
		fprintf(stderr, "-MyShell: cd: %s\n", strerror(errno));
		return -1;
	}

	char myCWD[] = "myCWD";
	if(setenv(myCWD, localPath, 1)<0){
		fprintf(stderr, "Change directory failed aliased path: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

void trimToChar(char *trimStr, char x, int depth){
	//trimToChar searches from the end of the character array trimStr for the character x up until depth is reached
	//When found the x character is replaced with the null character and the function exits
	int j;
	char* strptr = trimStr+strlen(trimStr)-1;
	for(j = 0; j<depth; j++){
		if(*strptr-- == x){
			*++strptr = '\0';
			break;
		}
	}

	if(strlen(trimStr)==0){
		*trimStr = '/';
		*++trimStr = '\0';
	}
}

//FindTailChar searches search from the end to find the first occurance of x, ignoring the first depth characters
char* findTailChar(char *search, char x, int depth){
	char* strptr = search+strlen(search)- 1 - depth;
	while(search!=strptr){
		if(*strptr-- == x) return ++strptr;
	}
	return search;
}

//returns the count of the char thisChar in search
int charCount(char* search, char thisChar){
	int cCount=0;
	while(*search!='\0'){
		if(*search++ == thisChar){
			cCount++;
		}
	}
	return cCount;
}

char* allocPath(char* u_arg){
	if(!u_arg) return NULL;
	char* ret_Alloc;
	long path_max, pathLen;
	
	if(*u_arg == '/'){
		
		if((path_max = pathconf("/", _PC_PATH_MAX)) < 0){
			fprintf(stderr, "Failed to get path length: %s\n", strerror(errno));
			return NULL;
		}
		char* root;
		if(!(root =  getenv("myRoot"))){
			fprintf(stderr, "Could not access root directory\n");
		}

		if((pathLen = strlen(root) + strlen(u_arg)) >= path_max) return NULL;
		if(!(ret_Alloc = malloc(pathLen + 1))) return NULL;
		strcpy(ret_Alloc, root);
		strcat(ret_Alloc, u_arg);
	}else{
	
		if((path_max = pathconf(".", _PC_PATH_MAX)) < 0){
			fprintf(stderr, "Failed to get path length: %s\n", strerror(errno));
			return NULL;
		}

		if((pathLen = strlen(u_arg)) >= path_max) return NULL;
		if(!(ret_Alloc = malloc(pathLen + 1))) return NULL;
		strcpy(ret_Alloc, u_arg);
	}

	return ret_Alloc;

}