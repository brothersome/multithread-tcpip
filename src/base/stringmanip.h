#ifndef __HEADER__stringmanip_
#define __HEADER_stringmanip_
#include <stdlib.h>
void reverse(char s[]);
char *strnchr(char *s, int c, int n);
void strlowercase(char *si);
void struppercase(char *si);
char *strcat_number(char *,int);
char *strcat_number_unsigned(char *,unsigned int);
int bf(char *si,char *soi,int maxlen,char *expr,char *sbuf,int len);
#ifdef __WIN32
	void itoa(int,char [],int);
#else
#ifndef itoa
	void itoa(int,char[]);
#endif
#endif
#endif
