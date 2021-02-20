#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "def.h"
#include "configcontent.h"
#include "logging.h"
#include "basemem.h"
#include "applicationdata.h"

enum contentkind {
	CONTENT_CONFIG, CONTENT_STRING, CONTENT_INTEGER, CONTENT_DOUBLE, CONTENT_NOTHING
};

static int gReadConfigFileError = 0;

/*
 * Read configuration
 * 
 * 
 * 
 * */
char *get_from_config_content(struct configcontent *p,char *s)
{
	while (p!=NULL)
	{
		if (p->name!=NULL)
		{
			if (strcmp(p->name,s)==0) return p->u.value_s;
		}
		p = p->next;
	}
	return NULL;
}
struct configcontent *search_config_content(struct configcontent *p,char *si)
{
	while (p!=NULL)
	{
		if (p->name!=NULL)
		{
			if (strcmp(p->name,si)==0) return p;
		}
		p = p->next;
	}
	return NULL;
}
int process_line_config_file(struct configcontent *pi,char *line,int *counter_row)
{
	char *s,*s2,*sb;
	char *sign;
	int value_i;
	int minus = 0;
	char *name;
	double value_d;
	name = pi->name;
	s = line;
	while ((*s)==' ' || (*s)=='\t') s++;
	switch(*s)
	{
	case '\0': case '\n': case '#': case '!': case '\r':
		return 0; // Nothing happened
	case '"': case '=':
		return add_to_log_error(__FILE__,__LINE__,"sssd","Error 1 at for ",line," at line ",*counter_row);
	case '}':
		return 3; // Ending of content
	default:
		// Following a name but a = must exist
		if ((sign=strchr(s,'='))==NULL)
		{
			return add_to_log_error(__FILE__,__LINE__,"sssd","= expected at for ",line," at line ",*counter_row);
		}
		sb = s;
		s = sign - 1;
		while ((*s)==' ' || (*s)=='\t') s--;
		s++;
		(*s)='\0';
		switch(*(s-1))
		{
			case '"': case ':': 
				return add_to_log_error(__FILE__,__LINE__,"sssd","Unexpected sign in",line," at line ",*counter_row);
		}
		pi->name=strdup_hold(sb);
		name = pi->name;
		s = sign+1;
		while ((*s)==' ' || (*s)=='\t') s++;
		sb = s;
		switch(*s)
		{
		case '=': case ':':
			return add_to_log_error(__FILE__,__LINE__,"sssd","Error 2 at for",line," at line ",*counter_row);
		case '{':
			pi->kind = CONTENT_CONFIG;
			pi->u.content_p = NULL;
			return 2;
		case '"': // String content
			s++;
			while ((*s)!='\0' && (*s)!='"')
			{
				if ((*s)=='\\') s++;
				s++;
			}
			if ((*s)=='\0') return add_to_log_error(__FILE__,__LINE__,"!sssd","Unexpected EOL for ",line," at line ",*counter_row);
			// A " character
			sb++;
			(*s)='\0';
			pi->kind = CONTENT_STRING;
			if ((*name)=='%')
			{
				if ((pi->u.value_s = alloc_string(80))==NULL) return add_to_log_error(__FILE__,__LINE__,"Alloc error");
				strcpy(pi->u.value_s,sb);
			}
			else {
				if ((pi->u.value_s=strdup_hold(sb))==NULL) return add_to_log_error(__FILE__,__LINE__,"Alloc error");
			}
			return 1;
		default:
			if ((*s)=='-')
			{
				if (isdigit(*(s+1)))
				{
					s++;
					minus = 1;
				}
			}
			if (isdigit(*s))
			{
				sb = s;
				while (isdigit(*s)) s++;
				if ((*s)=='.')
				{
					s++;
					while (isdigit(*s)) s++;
					(*s)='\0';
					value_d = atof(sb);
					pi->kind=CONTENT_DOUBLE;
					if (minus) value_d *= -1;
					pi->u.value_d=value_d;
					
				}
				else {
					(*s)='\0';
					value_i=atoi(sb);
					pi->kind=CONTENT_INTEGER;
					if (minus) value_i *= -1;
					pi->u.value_i=value_i;
				}
				return 1;
			}
			else { // String Content until the end of line
				if ((s = strchr(sb,'\n'))!=NULL)   // Unix
				{
					if ((s2 = strchr(sb,'\r'))!=NULL) // DOS
					{
						if (s2<s) s = s2;
					}
					s--; // removing the character
				}
				else {
					if ((s = strchr(sb,'\r'))!=NULL) // MAC
					{
						s--;
					}
					else s = sb + (strlen(sb) -1);
				}
				while ((*s)==' ' || (*s)=='\t') s--;
				s++;
				(*s)='\0';
				pi->kind = CONTENT_STRING;
				if ((*name)=='%')
				{
					if ((pi->u.value_s = alloc_string(80))==NULL) return add_to_log_error(__FILE__,__LINE__,"Alloc error");
					strcpy(pi->u.value_s,sb);
				}
				else {
					if ((pi->u.value_s=strdup_hold(sb))==NULL) return add_to_log_error(__FILE__,__LINE__,"Alloc error");
				}
				return 1;
			}
		}
	}
	return 0;
}

struct configcontent *make_new_config_content(void)
{
	struct configcontent *pn;
	if ((pn = (struct configcontent *)hmalloc(sizeof(struct configcontent)))==NULL) return NULL;
	pn->kind=CONTENT_NOTHING;
	pn->next=NULL;
	pn->name=NULL;
	return pn;
}


int read_config_file_only(struct configcontent *p,FILE *fpi,int *counter_row)
{
	struct configcontent *pv=NULL;
	int counter=0;
	int result;
	char line[1024];
	while (fgets(line,1023,fpi)!=NULL)
	{
		(*counter_row) = (*counter_row) + 1;
		result = process_line_config_file(p,line,counter_row);

		switch(result)
		{
		case -1:
			return add_to_log_error(__FILE__,__LINE__,"Returning -1 from process_line_config_file in ReadConfigFileOnly");
		case 0: // nothing happened so next line
			break;
		case 1: // A integer of string is inserted, same level
			counter++;
			pv = p;
			if ((p = make_new_config_content())==NULL)
			{
				return add_to_log_error(__FILE__,__LINE__,"Alloc error");
			}
			pv->next=p;
			break;
		case 2: // A new level is inserted at the pn
			counter++;
			pv = p;
			if ((p = make_new_config_content())==NULL) return -1;
			if ((result=read_config_file_only(p,fpi,counter_row))<0)
			{
				add_to_log_error(__FILE__,__LINE__,"s:s","Returning",line);
				pv->u.content_p=p;
				return -1;
			}
			if (result > 0)
			{
				pv->u.content_p=p;
				if ((p = make_new_config_content())==NULL) return -1; // Returning of content
			}
			else pv->u.content_p = NULL;
			pv->next=p;
			break;
		case 3: // A level is ended
			if (pv!=NULL) pv->next = NULL;
			return counter;
		default: 
			return add_to_log_error(__FILE__,__LINE__,"!s:ssd","Returning",line," at row ",(*counter_row));
		}	
	}
	return counter;
}
/* There is 1 dummy name for the NULL content this name has the name NULL */
struct configcontent *ReadConfigFile(char *name)
{
	int result;
	struct configcontent *first_p;
	int counter_row=0;
	FILE *fpi;
	gReadConfigFileError = 0;
	if ((first_p = make_new_config_content())==NULL)
	{
		add_to_log_error(__FILE__,__LINE__,"Error: ReadConfigFile: no new configcontent");
		gReadConfigFileError = 2;
		return NULL;
	}
	if ((fpi = fopen(name,"r"))==NULL)
	{
		add_to_log_error(__FILE__,__LINE__,"!s-'s'","Error: ReadConfigFile: cannot open file for reading",name);
		gReadConfigFileError = 1; // No file
		return NULL;
	}
	result = read_config_file_only(first_p,fpi,&counter_row);
	fclose(fpi);
	if (result < 0)
	{
		add_to_log_error(__FILE__,__LINE__,"Error in ReadConfigFile for result");
		gReadConfigFileError = 3;
		return NULL;
	}

	return first_p;
}

char *change_config(char *si,struct configcontent *p)
{
	// returns value at level 0
	char s2[11];
	while (p!=NULL)
	{
		if (p->name!=NULL)
		{
			if (strcmp(p->name,si)==0)
			{
				if (p->kind == CONTENT_INTEGER)
				{
					itoa(p->u.value_i,s2,10);
					return strdup_hold(s2);
				}
				if (p->kind == CONTENT_STRING)
				{
#ifdef DBLSTRDUP
					return strdup_hold(p->u.value_s);
#else
					return p->u.value_s;
#endif
				}
			}
		}
		p = p->next;
	}
	return NULL;
}

struct configcontent *GetConfigEntryShort(struct configcontent *p,char *name)
{
	while (p!=NULL)
	{
		if (p->name!=NULL)
		{
			if (strcmp(p->name,name)==0) return p;
		}
		p = p->next;
	}
	return NULL;
}

// #define LOG_GetConfigurationEntry
struct configcontent *GetConfigurationEntry(struct configcontent *p,char *name,int mode)
{
	while (p!=NULL)
	{
		if (p->name!=NULL)
		{
			if (strcmp(p->name,name)==0)
			{
				if (p->kind==CONTENT_CONFIG || mode >= 100)
				{
					return p;
				}
			}
		}
		p = p->next;
	}
	if (mode ==0)
	{
		add_to_log("d s:s",__LINE__,"Step: GetConfigurationEntry NOT FOUND",name);
	}
	return NULL;
}

int end_readconfig()
{
#ifdef TESTMODE
	add_to_log("sds",__FILE__,__LINE__,"End");
#endif
	return __LINE__;
}
