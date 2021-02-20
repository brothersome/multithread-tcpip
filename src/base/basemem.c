#include "def.h"
#include "applicationdata.h"
#include "memory.h"
#include "logging.h"

struct application_data *m_application_data;

char *strdup_hold(char *si) {
	return mem_dup(m_application_data->mem,si);
}

char *alloc_string(int len) {
	return mem_get(m_application_data->mem,len);
}

void *hmalloc(int len) {
	return mem_get(m_application_data->mem,len);
}

void add_application_data_param_to_application_data(struct application_data_param *adp_p) {
	struct application_data_param *p = m_application_data->params;
	if (p==NULL) {
		m_application_data->params = adp_p;
	} else {
		while (p->next != NULL) p = p->next;
		p->next = adp_p;
	}
}


int add_application_data_param(char *key,char *value) {
	struct application_data_param *p;
	if ((p = hmalloc(sizeof(struct application_data_param)))==NULL) return -1;
	if ((p->key = strdup_hold(key))==NULL) return -1;
	if ((p->value = strdup_hold(value))==NULL) return -1;
	p->next = NULL;
	add_application_data_param_to_application_data(p);
	return 0;
}

int add_application_data_param_no_alloc(char *key,char *value) {
	struct application_data_param *p;
	if ((p = hmalloc(sizeof(struct application_data_param)))==NULL) return -1;
	p->key = key;
	p->value = value;
	p->next = NULL;
	add_application_data_param_to_application_data(p);
	return 0;
}

int initialize_basemem(struct application_data *p) {
	m_application_data = p;
	p->add_application_data_param = add_application_data_param;
	p->add_application_data_param_no_alloc = add_application_data_param_no_alloc;
	return 0;
}

int end_basemem()
{
#ifdef TESTMODE
	add_to_log("sds",__FILE__,__LINE__,"End");
#endif
	return __LINE__;
}
