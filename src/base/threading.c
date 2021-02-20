#include "def.h"
#include "applicationdata.h"

struct application_data *m_application_data;

int initialize_threads(struct application_data *p) {
	m_application_data = p;
	return 0;
}
