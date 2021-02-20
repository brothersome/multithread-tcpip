#include "defconstants.h"

#define MEMORY_Q SMALL
#define ADDRESSSIZE ADDRESSSIZE_64BIT

#ifndef UNUSED
#define UNUSED(x) ( (void)(x) )
#endif

#ifdef _WIN32
#ifndef __WIN32
#define __WIN32
#endif
#endif

#define MAXFILENAME 260

#define ERRSTR add_to_log("s:d:s",__FILE__,__LINE__,"Err")

#define WITH_INHERITANCE

#define POINTERLENGTH ADDRESSSIZE

#define TESTMODE 1
