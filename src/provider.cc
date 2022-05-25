#include "provider.hh"

#include <stdlib.h>
#include <string.h>

/*
int providerOpen(swmm6* prj, const char* sModuleName, swmm6_provider** outPrv, void* userData)
{
  swmm6_provider* prv;
  int rc;
  swmm6_provider_module* mod = swmmFindProviderModule(prj, sModuleName);
  if(mod == NULL) {
    return SWMM_NOTFOUND;
  }
  rc = mod->xOpen(prj, &prv, userData);
  if(rc) {
    return rc;
  }
  prv->methods = mod;
  *outPrv = prv;
  return SWMM_OK;
}
*/
int providerGetBuilder(const swmm6_provider* prv, swmm6_builder** outBldr)
{
  return prv->xGetBuilder(outBldr);
}
/*
int providerRead(swmm6_provider* prv, int param, const char* value, swmm6_builder* bldr)
{
  return prv->methods->xRead(prv, param, value, bldr);
}
*/
int providerCreateObject(const swmm6_provider* prv, swmm6_builder* bldr, swmm6_object** outObj)
{
  int rc = prv->xCreateObject(bldr, outObj);
  if(rc) {
    return rc;
  }
  (*outObj)->object_methods = prv->methods;
  return SWMM_OK;
}

int providerReleaseBuilder(const swmm6_provider* prv, swmm6_builder* bldr)
{
  return prv->xReleaseBuilder(bldr);
}
/*
int providerClose(swmm6_provider* prv)
{
  return prv->methods->xClose(prv);
}
*/

int providerResetBuilder(const swmm6_provider* prv, swmm6_builder* bldr)
{
  bldr->uid = SWMM_NOID;
  memset(bldr->name, 0, SWMM_NAME_BUFFER_SIZE);
  return prv->xResetBuilder(bldr);
}

int providerReadCursor(const swmm6_provider* prv, swmm6_input_cursor* cur, swmm6_object** outObj)
{
  int rc = prv->xReadCursor(cur, outObj);
  if(rc) {
    return rc;
  }
  (*outObj)->object_methods = prv->methods;
  return SWMM_OK;
}
