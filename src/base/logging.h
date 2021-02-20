#ifndef __HEADER__logging_
#define __HEADER_logging_
#include <stdio.h>
void initialize_logging_normal(FILE *fp);
void initialize_logging_error(FILE *fp);
int add_to_log_error(char *filename,int line_number,char *s,...);
int add_to_log(char *s,...);
#endif