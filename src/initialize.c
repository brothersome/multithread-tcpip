#include <stdio.h>
#include "initialize.h"
#include "global.h"
/*
 * initialize
 * Setting arguments in memory and checking the input
 * 
 * -p port
 * -f initialization file
 * -n number of max threads
 * -s script file
 * 
 */

extern struct global_initialized_constants_typ global_initialzed_contants;
int set_globals_to_default();

int initialize(char *argv[],int argc) {
   int ret;
   ret = set_globals_to_default();
   if (ret < 0) return ret;
   ret = overrule_initialization_with_arguments();
   if (ret < 0) return ret;
   return argc;
}

int set_globals_to_default() {
   global_initialized_constants_type *g;
   g = &global_initialized_constants;
   g->port = 8080;
   char *host = "localhost";
   char *hosts[] = (char *[])malloc(sizeof(char *));
   g->hosts[0] = host;
   g->index_default_host = 0;
   g->quantity_hosts = 1;
   g->max_handlers = 5;
   g->min_handlers = 1;
   return 0;
}

typedef enum NextArgParam = {
   NONE = 0,Next_arg_port,Next_arg_config_filename,Next_arg_threads,Next_arg_script_filename;
};

int overrule_initialization_with_arguments(char *argv[],int argc) {
   NextArgParam next_arg_param;
   next_arg_param = NONE;
   for (int i=1;i<argc;i++) {
	  if (next_arg_param == NONE) {
		 if (*argv[0] == '-') {
			if (argv[0][1] == '\0') {
			   fprintf(stderr,"Non option mentioned at parameter %d\n",i);
			   return -1;
			}
			if (argv[0][2] != '\0') {
			   fprintf(stderr,"Option mentioned too long at parameter %d\n",i);
			   return -1;
			}
			// pfns
			switch(argv[0][1]) {
			 case 'p':
			   next_arg_param = Next_arg_port;
			   break;
			 case 'f':
			   next_arg_param = Next_arg_config_filename;
			   break;
			 case 'n':
			   next_arg_param = Next_arg_threads;
			   break;
			 case 's':
			   next_arg_param = Next_arg_script_filename;
			   break;
			 default:
			   fprintf(stderr,"Unknown option at parameter %d\n",i);
			   return -1;
			}
		 }
	  } else {
		 switch (next_arg_param) {
		  case Next_arg_port:
			break;
		  case Next_arg_config_filename:
			break;
		  case Next_arg_threads:
			break;
		  case Next_arg_script_filename:
			break;
		  default:
			  fprintf(stderr,"PF01\n");
			return -1;
		 }
	  }
   }
}
