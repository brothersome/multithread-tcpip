#ifndef __HEADER__handleparams_
#define __HEADER_handleparams_
char *strdup_hold(char *);
char *alloc_string(int len);
void *hmalloc(int len);
int add_application_data_param(char *key,char *value);
int add_application_data_param_no_alloc(char *key,char *value);
int initialize_basemem();
#endif