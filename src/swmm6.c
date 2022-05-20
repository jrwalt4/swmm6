#include "swmm6_int.h"

#include "hash.h"
#include "input.h"
#include "provider.h"
#include "simulation.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct swmm6
{
  swmm6_input* ioInput;
  swmm6_output* ioOutput;

  Hash* vProviders;

  Hash* vSimulations;
  int nSimulations;
};

swmm6_provider* swmmFindProvider(swmm6* prj, const char* sModuleName)
{
  if(sModuleName == NULL) {
    return NULL;
  }
  swmm6_provider* prv = (swmm6_provider*) hash_get(prj->vProviders, sModuleName);
  /*
  for(int index = 0 ; index < prj->nProviders ; index++) {
    if(strcmp(prj->vProviders[index]->sKind, sModuleName) == 0) {
      return prj->vProviders[index];
    }
  }
  */
  return prv;
}

int swmm6_create_provider(swmm6* prj, swmm6_provider* prv)
{

  const char* key = hash_set(prj->vProviders, prv->sKind, prv);
  if(key == NULL) {
    return SWMM_ERROR;
  }
  /*
  swmm6_provider** module_array = realloc(prj->vProviders, sizeof(prv) * (prj->nProviders + 1));

  if(module_array == NULL) {
    return SWMM_ERROR;
  }
  module_array[prj->nProviders] = prv;
  prj->vProviders = module_array;
  prj->nProviders++;
  */
  return SWMM_OK;
}

int swmm6_open(const char* inpName, swmm6** pPrj, const char* inpMod)
{
  swmm6* prj;
  swmm6_input* pInp;
  int rc = inputOpen(inpName, inpMod, &pInp);
  if(rc) {
    return rc;
  }
  prj = calloc(1,sizeof(*prj));
  if(prj == NULL) {
    return SWMM_ERROR;
  }
  prj->ioInput = pInp;
  puts("Creating providers");
  prj->vProviders   = hash_create();
  if(prj->vProviders == NULL) {
    rc = SWMM_ERROR;
    goto cleanup_prj;
  }
  puts("Creating simulations");
  prj->vSimulations = hash_create();
  if(prj->vSimulations == NULL) {
    rc = SWMM_ERROR;
    goto cleanup_providers;
    // hash_destroy(prj->vProviders);
    // free(prj);
    // return SWMM_ERROR;
  }
  rc = swmmCreateBuiltinNodeProviders(prj);
  if(rc) {
    goto cleanup_simulations;
    // free(prj);
    // return rc;
  }
  *pPrj = prj;
  return SWMM_OK;
cleanup_simulations:
  hash_destroy(prj->vSimulations);
cleanup_providers:
  hash_destroy(prj->vProviders);
cleanup_prj:
  free(prj);
  return SWMM_ERROR;
}

int swmmReadCursor(swmm6* prj, swmm6_input_cursor* cur, swmm6_provider* prv, swmm6_simulation* sim)
{
  (void) prj;
  //(void) cur;
  (void) prv;
  (void) sim;
  int rc = inputNext(cur);
  while (rc == SWMM_ROW) {
    printf("Reading uid: %i\n", inputReadInt(cur, 0));
    swmm6_object* obj;
    rc = providerReadCursor(prv, cur, &obj);
    if(rc) {
      return rc;
    }
    puts("destroying object");
    swmm6_object_destroy(obj);
    rc = inputNext(cur);
  }
  if(rc == SWMM_DONE) {
    return SWMM_OK;
  }
  return rc;
}

int swmm6_open_simulation(const char* scenario, swmm6* prj, swmm6_simulation** pSim, char** zErr)
{
  (void) zErr;
  int rc;
  swmm6_input* inp = prj->ioInput;
  swmm6_simulation* sim;
  swmm6_scenario_info* info;
  swmm6_input_cursor* cur;
  swmm6_provider* prv;

  rc = simulationOpen(scenario, prj, &sim);
  if(rc) {
    goto open_simulation_fail;
  }
  rc = inputDescribeScenario(scenario, inp, &info);
  if(rc) {
    goto open_scenario_fail;
    /*
    simulationClose(sim);
    *pSim = NULL;
    return rc;
    */
  }
  puts("Reading scenario");
  for(int i = 0 ; i < info->nQueries ; i++) {
    prv = swmmFindProvider(prj, info->aProviders[i]);
    if(prv == NULL) {
      rc = SWMM_NOTFOUND;
      goto cursor_fail;
    }
    rc = inputOpenCursor(inp, info->aQueries[i], prv, &cur);
    if(rc) {
      goto cursor_fail;
    }
    rc = swmmReadCursor(prj, cur, prv, sim);
    // close before next iteration, and before failure check
    inputCloseCursor(cur);
    if(rc) {
      goto read_cursor_fail;
    }
  }
  *pSim = sim;
  return SWMM_OK;

read_cursor_fail:
  inputCloseCursor(cur);
cursor_fail:
  inputReleaseScenario(inp, info);
open_scenario_fail:
  simulationClose(sim);
  *pSim = NULL;
open_simulation_fail:
  return rc;
}
/*
int swmmCloseProviders(swmm6* prj)
{
  int i;
  int rc = SWMM_OK;
  int prv_rc = SWMM_OK;
  swmm6_provider* prv;
  for(i = 0 ; i < prj->nProviders ; i++) {
    prv = prj->vProviders[i];
    prv_rc = providerClose(prv);
    if(prv_rc) {
      // log that there was an issue, use as function return
      rc = prv_rc;
    }
  }
  return rc;
}
*/
int swmmCloseSimulations(swmm6* prj)
{
  // int i;
  int rc = SWMM_OK;
  int sim_rc = SWMM_OK;
  HashIter sim_it = hash_iterator(prj->vSimulations);
  while(hash_next(&sim_it)) {
    sim_rc = simulationClose(hash_get(prj->vSimulations, sim_it.key));
    if(sim_rc) {
      // log that there was an issue, use as function return
      rc = sim_rc;
    }
  }
  /*
  for(i = 0 ; i < prj->nSimulations ; i++) {
    sim_rc = simulationClose(prj->vSimulations[i]);
    if(sim_rc) {
      // log that there was an issue, use as function return
      rc = sim_rc;
    }
  }
  */
  return rc;
}

int swmm6_close(swmm6* prj)
{
  int rc = SWMM_OK;
  //rc = swmmCloseProviders(prj);
  puts("Closing providers");
  hash_destroy(prj->vProviders);

  puts("Closing simulations");
  rc = swmmCloseSimulations(prj);
  puts("Destroying simulation hash");
  hash_destroy(prj->vSimulations);

  puts("Closing input");
  rc = inputClose(prj->ioInput);

  free(prj);

  return rc;
}
