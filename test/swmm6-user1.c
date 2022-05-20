#include "swmm6.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
  swmm6* prj;
  int rc = swmm6_open("", &prj, NULL);
  if(rc) {
    printf("Error: %i\n", rc);
  }
  swmm6_close(prj);
  return SWMM_OK;
}
