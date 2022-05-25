#include "swmm6_int.hh"

int swmm6_object_destroy(swmm6_object* pObj)
{
  return pObj->object_methods->xDestroy(pObj);
}
