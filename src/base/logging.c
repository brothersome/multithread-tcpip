
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#include "logging.h"
#include "stringmanip.h"
#include "timemanip.h"

FILE *fp_log = NULL;
FILE *fp_log_error = NULL;

void initialize_logging_normal(FILE *fp) {
	if (fp_log != NULL) fclose(fp_log);
	fp_log = fp;
}

void initialize_logging_error(FILE *fp) {
	if (fp_log_error != NULL) fclose(fp_log_error);
	fp_log_error = fp;
}

int add_to_log_error(char *filename,int line_number,char *s,...) {
	char *s2;
	char tot_line[1500];
	char *value_s;
	char line[80];
	int value_i;
	unsigned int value_ui;
	void *value_p;
	unsigned int now;
	double value_d;
	va_list ap;
	char null_s[]="unknown";
	int len;
	struct tm ts;
	time_t t;
	char time_s[42];
	char *so;
	so = tot_line;
	now = get_time_unsigned();
	if (fp_log_error==NULL) return 0; // Just do nothing
	if (s[1]==':' || s[1]=='\0')
	{
		tot_line[0]='\0';
		for(va_start(ap,s); *s && (so - tot_line) < 1024; s++) {
			switch(*s)
			{
			case 's':
				if (ap==NULL) strcat(so,null_s);
				else {
					value_s = va_arg(ap, char *);
					if (value_s==NULL || value_s < (char *)0x1000) strcat(so,null_s);
					else {
						len = strlen(value_s) + (so - tot_line);
						if (len < 1024) strcat(so,value_s);
						else {
							strncat(so,value_s,1024 - (so - tot_line));
							tot_line[1024]='\0';
							len = 1024;
						}
						s2 = so;
						while ((s2=strchr(s2,'\r'))!=NULL) (*s2)='$';
						s2 = so;
						while ((s2=strchr(s2,'\n'))!=NULL) (*s2)='$';
					}
				}
				so += strlen(so);
				break;
			case 'd':
				if (ap==NULL) strcat(so,null_s);
				else {
					value_i = va_arg(ap, int);
					strcat_number(so,value_i);
				}
				so += strlen(so);
				break;
			case 'p':
				if (ap==NULL)
				{
					strcat(so,null_s);
				}
				else {
					value_p = va_arg(ap, void *);
					if (value_p != NULL)
					{
						sprintf(line,"%p",value_p);
						strcat(so,line);
					}
					else {
						strcat(so,null_s);
					}
				}
				so += strlen(so);
				break;
			case 'u':
				if (ap==NULL)
				{
					strcat(so,null_s);
				}
				else {
					value_ui = va_arg(ap, int);
					strcat_number_unsigned(so,value_ui);
				}
				so += strlen(so);
				break;
			case 'f':
				if (ap==NULL)
				{
					strcat(tot_line,null_s);
				}
				else {
					value_d = va_arg(ap, double);
					sprintf(line,"%f",value_d);
					strcat(so,line);
				}
				so += strlen(so);
				break;
			case 'e':
				if (ap==NULL)
				{
					strcat(so,null_s);
				}
				else {
					value_d = va_arg(ap, double);
					sprintf(line,"%e",(double)value_d);
					strcat(so,line);
				}
				so += strlen(so);
				break;
			default:
				*so = *s;
				so++;
				*so = '\0';
			}
		}
		if (ap!=NULL) va_end(ap);
		s = tot_line;
	}
	t = now;
	gmtime_r(&t,&ts);
	strftime(time_s,41, "%a %Y-%m-%d %H:%M:%S",&ts);
	fprintf(fp_log_error,"%s %s %d %s\n",time_s,filename,line_number,s);
	fflush(fp_log_error);
	return 0;
}

int add_to_log(char *s,...)
{
	char *s2;
	char tot_line[1500];
	char *value_s;
	char line[80];
	int value_i;
	unsigned int value_ui;
	void *value_p;
	unsigned int now;
	double value_d;
	va_list ap;
	char null_s[]="unknown";
	int len;
	struct tm ts;
	time_t t;
	char time_s[42];
	char *so;
	so = tot_line;
	now = get_time_unsigned();
	if (fp_log==NULL) return 0; // Just do nothing
	if (s[1]==':' || s[1]=='\0')
	{
		tot_line[0]='\0';
		for(va_start(ap,s); *s && (so - tot_line) < 1024; s++) {
			switch(*s)
			{
			case 's':
				if (ap==NULL) strcat(so,null_s);
				else {
					value_s = va_arg(ap, char *);
					if (value_s==NULL || value_s < (char *)0x1000) strcat(so,null_s);
					else {
						len = strlen(value_s) + (so - tot_line);
						if (len < 1024) strcat(so,value_s);
						else {
							strncat(so,value_s,1024 - (so - tot_line));
							tot_line[1024]='\0';
							len = 1024;
						}
						s2 = so;
						while ((s2=strchr(s2,'\r'))!=NULL) (*s2)='$';
						s2 = so;
						while ((s2=strchr(s2,'\n'))!=NULL) (*s2)='$';
					}
				}
				so += strlen(so);
				break;
			case 'd':
				if (ap==NULL) strcat(so,null_s);
				else {
					value_i = va_arg(ap, int);
					strcat_number(so,value_i);
				}
				so += strlen(so);
				break;
			case 'p':
				if (ap==NULL)
				{
					strcat(so,null_s);
				}
				else {
					value_p = va_arg(ap, void *);
					if (value_p != NULL)
					{
						sprintf(line,"%p",value_p);
						strcat(so,line);
					}
					else {
						strcat(so,null_s);
					}
				}
				so += strlen(so);
				break;
			case 'u':
				if (ap==NULL)
				{
					strcat(so,null_s);
				}
				else {
					value_ui = va_arg(ap, int);
					strcat_number_unsigned(so,value_ui);
				}
				so += strlen(so);
				break;
			case 'f':
				if (ap==NULL)
				{
					strcat(tot_line,null_s);
				}
				else {
					value_d = va_arg(ap, double);
					sprintf(line,"%f",value_d);
					strcat(so,line);
				}
				so += strlen(so);
				break;
			case 'e':
				if (ap==NULL)
				{
					strcat(so,null_s);
				}
				else {
					value_d = va_arg(ap, double);
					sprintf(line,"%e",(double)value_d);
					strcat(so,line);
				}
				so += strlen(so);
				break;
			default:
				*so = *s;
				so++;
				*so = '\0';
			}
		}
		if (ap!=NULL) va_end(ap);
		s = tot_line;
	}
	t = now;
	gmtime_r(&t,&ts);
	strftime(time_s,41, "%a %Y-%m-%d %H:%M:%S",&ts);
	fprintf(fp_log,"%s %s\n",time_s,s);
	fflush(fp_log);
	return 0;
}

int end_logging()
{
#ifdef TESTMODE
	add_to_log("sds",__FILE__,__LINE__,"End");
#endif
	return __LINE__;
}
