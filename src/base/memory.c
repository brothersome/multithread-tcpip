#include <stdlib.h>
#include <string.h>

#include "def.h"
#include "logging.h"
#include "memory.h"

#define MAXMEMBUF 131072

#define VERSION 1.0.1
 
// #define LOG_mem_create
struct progmem *mem_create(void)
{
	char *sBuffer;
	struct progmem *pn;
	if ((sBuffer = (char *)malloc(MAXMEMBUF + 8))==NULL) return NULL;
	if ((pn = (struct progmem *)malloc(sizeof(struct progmem) + 8))==NULL) return NULL;
	memset(pn,0,sizeof(struct progmem));
	pn->buffer = sBuffer;
	return pn;
}
int mem_store(struct progmem *pm,void *pi)
{
	struct progmem *pn,*p;
	if (pm==NULL)
	{
		ERRSTR;
		add_to_log("Pointer null");
		return -1;
	}
	if ((pn = (struct progmem *)malloc(sizeof(struct progmem) + 8))==NULL) return -1;
	memset(pn,0,sizeof(struct progmem));
	pn->buffer = pi;
	pn->offset = MAXMEMBUF; // Not to allocate more --- With the end the program frees all buffers
	p = pm;
	while (p->next!=NULL) p = p->next;
	p->next = pn;
	return 0;
}
void *mem_get(struct progmem *pm,int iBytes)
{
	void *ret;
	int offset;
	struct progmem *pn,*p;
	
	if (pm==NULL)
	{
		ERRSTR;
		add_to_log("Pointer null");
		return NULL;
	}
	offset = pm->offset + iBytes;
#ifdef WITH_ALIGNMENT
	offset = (offset + 3) & 0xFFFFFFFFC; // Addresses must be able to be divided by 4
#endif

	if (offset > MAXMEMBUF)
	{
		p = pm;
		while (offset > MAXMEMBUF && p->next !=NULL)
		{
			p = p->next;
			offset = p->offset + iBytes;
#ifdef WITH_ALIGNMENT
			offset = (offset + 3) & 0xFFFFFFFFC; // Addresses must be able to be divided by 4
#endif
		}
		if (offset < MAXMEMBUF)
		{
			ret = p->buffer + p->offset;
			p->offset = offset;
		}
		else {
			if ((pn = mem_create())==NULL)
			{
				add_to_log("Alloc error");
				return NULL;
			}
			p->next = pn;
			ret = pn->buffer;
#ifdef WITH_ALIGNMENT
			pn->offset = (iBytes + 3) & 0xFFFFFFFFC;;
#else
			pn->offset = iBytes;
#endif
		}
	}
	else {
		ret = pm->buffer + pm->offset;
		pm->offset = offset;
	}
	return ret;
}

void mem_free(struct progmem *pm)
{
	struct progmem *p,*pv;
	p = pm;
	while (p!=NULL)
	{
		pv = p;
		p = p->next;
		if (pv->buffer!=NULL) free(pv->buffer);
		free(pv);
	}
}

void mem_nullify(struct progmem *pm)
{
	struct progmem *p;
	p = pm;
	while (p!=NULL)
	{
		p->offset = 0;
		p = p->next;
	}
}

char *mem_dup(struct progmem *pm,char *si)
{
	int iLen;
	char *sn;
	iLen = strlen(si) + 1;
	int offset;
	struct progmem *pn,*p; // Addresses can be everything
	if (pm==NULL) return NULL;
	offset = pm->offset + iLen;
#ifdef WITH_ALIGNMENT
	offset = (offset + 3) & 0xFFFFFFFFC; // Addresses must be able to be divided by 4
#endif

	if (offset > MAXMEMBUF)
	{
		p = pm;
		while (offset > MAXMEMBUF && p->next !=NULL)
		{
			p = p->next;
			offset = p->offset + iLen;
#ifdef WITH_ALIGNMENT
			offset = (offset + 3) & 0xFFFFFFFFC; // Addresses must be able to be divided by 4
#endif

		}
		if (offset < MAXMEMBUF)
		{
			sn = p->buffer + p->offset;
			p->offset = offset;
		}
		else {
			if ((pn = mem_create())==NULL) return NULL;
			p->next = pn;
			sn = pn->buffer;
#ifdef WITH_ALIGNMENT
			pn->offset = (iLen + 3) & 0xFFFFFFFFC; // Addresses must be able to be divided by 4
#else
			pn->offset = iLen;
#endif

		}
	}
	else {
		sn = pm->buffer + pm->offset;
		pm->offset = offset;
	}
	// if ((sn = (char *)mem_get(pm,iLen))==NULL) return NULL;
	strcpy(sn,si);
	return sn;
}

int end_memory()
{
#ifdef TESTMODE
	add_to_log("sds",__FILE__,__LINE__,"End");
#endif
	return __LINE__;
}