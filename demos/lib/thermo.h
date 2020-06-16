//This library is based off of Brian Kernighan/Dennis Ritchie's "The C Programming Language 2nd Edition" storage allocator
#include<unistd.h>

typedef long Align;

#define MINOBT 1024

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

static Meta *morecore(unsigned);
void release(void*);

void *Obtain(unsigned r_bytes){

	Meta *cur, *prev;
	unsigned obtUnits;

	obtUnits = (r_bytes + sizeof(Meta) - 1) / sizeof(Meta) + 1;
	if((prev = freep) == NULL){
		base.s.ptr = freep = prev = &base;
		base.s.size = 0;
	}
	for(cur = prev->s.ptr ;; prev = cur, cur = cur->s.ptr){
		if(cur->s.size >= obtUnits){
			if(cur->s.size == obtUnits){
				prev->s.ptr = cur->s.ptr;
			}else{
				cur->s.size -= obtUnits;
				cur += cur->s.size;
				cur->s.size = obtUnits;
			}
			freep = prev;
			return (void *)(cur+1);
		}
		if(cur == freep){
			if((cur = morecore(obtUnits)) == NULL) return NULL;
		}

	}

}

static Meta *morecore(unsigned nu){
	char *cp;
	Meta* up;

	if(nu < MINOBT) nu = MINOBT;
	cp = sbrk(nu*sizeof(Meta));
	if(cp == (char *) -1) return NULL;
	up = (Meta*) cp;
	up->s.size = nu;
	release((void *)(up+1));
	return freep;
}

void release(void *ap){
	Meta *bp, *p;

	bp = (Meta*) ap - 1;
	for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr){
		if(p >= p->s.ptr && ( bp > p || bp < p->s.ptr )) break;
	}

	if(bp + bp->s.size == p->s.ptr){
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	}else{
		bp->s.ptr = p->s.ptr;
	}

	if(p + p->s.size == bp){
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	}else{
		p->s.ptr = bp;
	}
	freep = p;
}