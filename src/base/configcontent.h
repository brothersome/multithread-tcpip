#ifndef __HEADER__configcontent_
#define __HEADER_configcontent_

union value_u {
	int value_i;
	unsigned int value_ui;
	int *values;
	unsigned int *values_ui;
	double value_d;
	long value_l;
	unsigned long value_ul;
	char *value_s;
	char value_chars[POINTERLENGTH];
	void *value_p;
};

struct configcontent {
	char *name;
	char kind;
	union u_configcontent {
		int value_i;
		char *value_s;
		double value_d;
		struct configcontent *content_p;
	}u;
	struct configcontent *next; // looping this way is faster
};

#endif