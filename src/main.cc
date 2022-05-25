#include "swmm6.h"
#include <stdio.h>

int main(int argc, char **argv) {
  char* inp;
  int rc;
  swmm6* prj;
  swmm6_simulation* sim;
  char* zErr;
  if(argc > 1) {
    inp = argv[1];
  } else {
    inp = ":memory:";
  }
  printf("Opening %s\n", inp);
  rc = swmm6_open(inp, &prj, NULL);
  if(rc) {
	  printf("Error Opening: %i\n", rc);
    return rc;
  }
  rc = swmm6_open_simulation("", prj, &sim, &zErr);
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
