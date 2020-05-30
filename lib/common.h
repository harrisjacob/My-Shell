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


char* addToPath(char* basePath, char* pathAdd){
	if(!basePath || !pathAdd) return NULL;
	char* newPath;
	if(!(newPath = malloc(strlen(basePath)+strlen(pathAdd)+2))){
		fprintf(stderr, "Path append allocation failed\n");
		return NULL;
	}

	if(sprintf(newPath,"%s/%s", basePath, pathAdd)<0){
		fprintf(stderr, "Path append build failed\n");
		free(newPath);
		return NULL;
	}

	return newPath;
}