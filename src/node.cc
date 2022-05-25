#include "swmm6_int.hh"

#include "input.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double swmm6_node_getDepth(const swmm6_node* node)
{
  return node->node_methods->xGetDepth(node);
}

double swmm6_node_getInvert(const swmm6_node* node)
{
  return node->node_methods->xGetInvert(node);
}

typedef struct swmm6_junction_node
{
  swmm6_node node_base;
  double invert;
  double rim;
} swmm6_junction_node;

int junctionDestroy(swmm6_object* obj)
{
  free(obj->name);
  free(obj);
  return SWMM_OK;
}

double junctionGetDepth(const swmm6_node* node)
{
  const swmm6_junction_node* jxn = (const swmm6_junction_node*) node;
  return jxn->rim - jxn->invert;
}

double junctionGetInvert(const swmm6_node* node)
{
  return ((const swmm6_junction_node*) node)->invert;
}

swmm6_node_methods JunctionNodeMethods = {
  { junctionDestroy },
  junctionGetDepth,
  junctionGetInvert
};

/*
int JunctionProviderOpen(swmm6* prj, swmm6_provider** outPrv, void* userData)
{
  (void) prj;
  (void) userData;
  swmm6_provider* prv = malloc(sizeof(*prv));
  *outPrv = prv;
  prv->eType = NODE;
  prv->sKind = "JUNCTION";
  return SWMM_OK;
}
*/

int JunctionProviderResetBuilder(swmm6_builder* bldr)
{
  // builder base properties reset by call to providerResetBuilder,
  // only reset our subclass properties.
  swmm6_junction_builder* jxnBldr = (swmm6_junction_builder*)bldr;
  jxnBldr->invert = -1;
  jxnBldr->rim = -1;
  return SWMM_OK;
}

int JunctionProviderGetBuilder(swmm6_builder** outBldr)
{
  swmm6_junction_builder* bldr = (swmm6_junction_builder*) malloc(sizeof(*bldr));
  JunctionProviderResetBuilder((swmm6_builder*)bldr);
  *outBldr = (swmm6_builder*) bldr;
  return SWMM_OK;
}
/*
int JunctionProviderRead(swmm6_provider* prv, int param, const char* value, swmm6_builder* bldr)
{
  (void) prv;
  switch(param) {
    case 0:
      bldr->uid = atoi(value);
      break;
    case 1:
      ((JunctionBuilder*) bldr)->invert = atof(value);
      break;
    case 2:
      ((JunctionBuilder*) bldr)->rim = atof(value);
      break;
    default: return SWMM_ERROR;
  }
  return SWMM_OK;
}
*/

int JunctionProviderCreateObject(swmm6_builder* objBldr, swmm6_object** outNode)
{
  swmm6_junction_node* jxn = (swmm6_junction_node*) malloc(sizeof(*jxn));
  if(jxn == NULL) {
    return SWMM_ERROR;
  }
  swmm6_junction_builder* bldr = (swmm6_junction_builder*) objBldr;
  jxn->node_base.object_base.uid = bldr->builder_base.uid;
  jxn->node_base.object_base.name = bldr->builder_base.name;
  jxn->invert = bldr->invert;
  jxn->rim = bldr->rim;
  *outNode = (swmm6_object*) jxn;
  return SWMM_OK;
}

int JunctionProviderReleaseBuilder(swmm6_builder* bldr)
{
  free((swmm6_junction_builder*)bldr);
  return SWMM_OK;
}

int JunctionProviderReadCursor(swmm6_input_cursor* cur, swmm6_object** outNode)
{
  swmm6_junction_node* jxn = (swmm6_junction_node*) malloc(sizeof(*jxn));
  if(jxn == NULL) {
    return SWMM_ERROR;
  }
  jxn->node_base.object_base.uid = inputReadInt(cur, 0);
  jxn->node_base.object_base.name = strdup(inputReadText(cur, 1));
  jxn->invert = inputReadDouble(cur, 2);
  jxn->rim = inputReadDouble(cur, 3);
  *outNode = (swmm6_object*) jxn;
  return SWMM_OK;
}

static const char* JunctionParams[] = {"uid", "name", "invert", "rim"};
static swmm6_param_type JunctionParamTypes[] = { SWMM_INT, SWMM_TEXT, SWMM_REAL, SWMM_REAL};

static swmm6_provider JunctionProvider = {
  1,
  NODE,
  "JUNCTION",
  (swmm6_object_methods*) &JunctionNodeMethods,
  4,
  JunctionParams,
  JunctionParamTypes,
  //JunctionProviderOpen,
  JunctionProviderGetBuilder,
  //JunctionProviderRead,
  JunctionProviderCreateObject,
  JunctionProviderReleaseBuilder, // xReleaseBuilder
  //NULL // xClose
  JunctionProviderResetBuilder,
  JunctionProviderReadCursor
};

int swmmCreateBuiltinNodeProviders(swmm6* prj)
{
  int rc = SWMM_OK;
  rc = swmm6_create_provider(prj, &JunctionProvider);
  if(rc) {
    puts("error loading JunctionProvider");
  } else {
    puts("loaded JunctionProvider");
  }
  return rc;
}

