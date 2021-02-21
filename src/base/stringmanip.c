#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "def.h"
#include "logging.h"
#include "stringmanip.h"

void itoa_unsigned(unsigned int n, char s[]);

void reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

char *strnchr(char *s, int c, int n)
{
	while (n--)
	{
		if (*s == c) return s;
		else {
			if ((*s)=='\0') return NULL;
			++s;
		}
	}
	return NULL;
}

void strlowercase(char *si)
{
	char *s;
	s = si;
	while ((*s)!='\0')
	{
		(*s) =  tolower((int)*s);
		s++;
	}
}
void struppercase(char *si)
{
	char *s;
	s = si;
	while ((*s)!='\0')
	{
		(*s) =  toupper((int)*s);
		s++;
	}
}

char *strcat_number(char *si,int number)
{
	char number_s[12];
#ifdef __WIN32
	itoa(number,number_s,10);
#else
	itoa(number,number_s);
#endif
	strcat(si,number_s);
	return si;
}
char *strcat_number_unsigned(char *si,unsigned int number)
{
	char number_s[12];
	itoa_unsigned(number,number_s);
	strcat(si,number_s);
	return si;
}

int bf(char *si,char *soi,int maxlen,char *expr,char *sbuf,int len)
{
	char *s;
	char *so;
	int level = 0;
	int level2;
	char *slevel[32];
	int done;
	so = soi;
	s = sbuf;
	while (*expr != '\0')
	{
		switch(*expr)
		{
		case '>':
			if (s > sbuf) s--;
			else s = sbuf + (len - 1);
			break;
		case '<':
			if (s - sbuf < len) s++;
			else s = sbuf;
			break;
		case '+':
			++(*s);
			break;
		case '-':
			--(*s);
			break;
		case '.':
			if (so - soi > maxlen - 1)
			{
				*so = '\0';
				return -1;
			}
			*so = *s;
			so++;
			break;
		case ',':
			if (*si == '\0') return -2;
			*s = *si;
			si++;
			break;
		case '[':	// While
			if (*s == '\0')
			{
				level2 = level;
				expr++;
				done = 0;
				while (!done)
				{
					switch(*expr)
					{
					case '[':
						level2++;
						expr++;
						break;
					case ']':
						if (level2 == level) done = 1;
						else {
							level2--;
							expr++;
						}
						break;
					case '\0':
						return -3;
					default:
						expr++;
					}
				}
			}
			else {
				slevel[level] = expr;
				level++;
			}
			break;
		case ']':	// End of while
			if (level == 0) return -4;
			level--;
			expr = slevel[level];
			break;
		}
		expr++;

	}
	return 0;
}

#ifndef __WIN32
#ifndef itoa
/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}
#endif
#endif
void itoa_unsigned(unsigned int n, char s[])
{
    int i;
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    s[i] = '\0';
    reverse(s);
}

int end_stringmanip()
{
#ifdef TESTMODE
	add_to_log("sds",__FILE__,__LINE__,"End");
#endif
	return __LINE__;
}
