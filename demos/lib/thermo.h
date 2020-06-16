#include<stddef.h>

//This library is based off of Brian Kernighan/Dennis Ritchie's "The C Programming Language 2nd Edition" storage allocator


typedef long Align;

union meta {
	struct{
		union meta *ptr;
		unsigned size;
	} s;
	Align x;
};

typedef union meta Meta;

static Meta base;
static Meta *freep = NULL;

