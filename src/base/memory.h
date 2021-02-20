#ifndef __HEADER__memory_
#define __HEADER_memory_

struct progmem {
	char *buffer;
	int offset;
	struct progmem *next;
};


struct progmem *mem_create(void);
int mem_store(struct progmem *pm,void *pi);
void *mem_get(struct progmem *pm,int iBytes);
void mem_free(struct progmem *pm);
void mem_nullify(struct progmem *pm);
char *mem_dup(struct progmem *pm,char *si);

#endif