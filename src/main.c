#include <stdio.h>
#include "def.h"
#include "initialize.h"
#include "global.h"

struct global_initialized_constants_type  global_initialized_constants;

int main(char *argv[],int argc) {
   int ret;
   ret = initialize(argv,argc);
   if (ret < 0) return 10 + ret * -1;
   return 0;
}

