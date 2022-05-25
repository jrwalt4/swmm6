#include "swmm6_int.hh"

#include "input.hh"
#include "provider.hh"
#include "simulation.hh"

#include <assert.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>

using namespace std;

struct swmm6
{
  unique_ptr<swmm::Input> ioInput;
  swmm6_output* ioOutput;

  unordered_map<string, swmm6_provider*> providers;

  unordered_map<string, swmm6_simulation*> vSimulations;

  explicit swmm6(swmm::Input* inp): ioInput(inp) {}

};

swmm6_provider* swmmFindProvider(swmm6* prj, const char* sModuleName)
{
  if(sModuleName == NULL) {
    return NULL;
  }
  swmm6_provider* prv = prj->providers.at(string(sModuleName));
  /*
  for(int index = 0 ; index < prj->nProviders ; index++) {
    if(strcmp(prj->providers[index]->sKind, sModuleName) == 0) {
      return prj->providers[index];
    }
  }
  */
  return prv;
}

int swmm6_create_provider(swmm6* prj, swmm6_provider* prv)
{
  string key{prv->sKind};
  pair<string, swmm6_provider*> kv(key, prv);
  bool success = prj->providers.insert( { std::string{prv->sKind} , prv } ).second;
  if(!success) {
    return SWMM_ERROR;
  }
  /*
  swmm6_provider** module_array = realloc(prj->providers, sizeof(prv) * (prj->nProviders + 1));

  if(module_array == NULL) {
    return SWMM_ERROR;
  }
  module_array[prj->nProviders] = prv;
  prj->providers = module_array;
  prj->nProviders++;
  */
  return SWMM_OK;
}

int swmm6_open(const char* input, swmm6** pPrj)
{

}

int swmm6_open_with(const char* inpName, swmm6** pPrj, const char* io)
{
  swmm6* prj = new swmm6(inputOpen(inpName, io));
  if(prj == NULL) {
    return SWMM_ERROR;
  }

  int rc = swmmCreateBuiltinNodeProviders(prj);
  if(rc) {
    delete prj;
    return rc;
  }
  *pPrj = prj;
  return SWMM_OK;
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
    rc = inputOpenCursor(inp, info->aQueries[i], info, &cur);
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
    prv = prj->providers[i];
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
  for(auto& key_val : prj->vSimulations)
  {
    sim_rc = simulationClose(key_val.second);
    if(sim_rc) {
      // log that there was an issue, use as function return
      rc = sim_rc;
    }
  }
  return rc;
}

int swmm6_close(swmm6* prj)
{
  int rc = SWMM_OK;
  puts("Closing simulations");
  rc = swmmCloseSimulations(prj);
  delete prj;

  return rc;
}
