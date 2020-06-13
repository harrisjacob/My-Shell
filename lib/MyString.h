char* strcpy_b(char* buff, char* src){
	while(*buff++ = src++);
	return src;
}

//Prone to buffer overflow but 
char* strjoin_p(char* buff, size_t size, char* first, char* last){
	if(!buff || !first || !last) return NULL;
	strcpy_b(buff, first); 

}