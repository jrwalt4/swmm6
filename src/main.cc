#include "swmm6.h"
#include <stdio.h>

int main(int argc, char **argv) {
  const char* inp;
  int rc;
  swmm6* prj;
  swmm6_simulation* sim;
  if(argc > 1) {
    inp = argv[1];
  } else {
    inp = ":memory:";
  }
  printf("Opening %s\n", inp);
  rc = swmm6_open(inp, &prj);
  if(rc) {
	  printf("Error Opening: %i\n", rc);
    return rc;
  }
  puts("Opening simulation");
  rc = swmm6_open_simulation("", prj, &sim);
  if(rc) {
    printf("Error opening simulation: %i\n", rc);
    return rc;
  }

  puts("Closing");
  rc = swmm6_close(prj);
  if(rc) {
	  printf("Error Closing: %i\n", rc);
    return rc;
  }
  puts("Success");
	return SWMM_OK;
}
