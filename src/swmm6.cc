#include "swmm6_int.hh"

#include "error.hh"
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
using namespace swmm;

struct swmm6
{
private:
  unique_ptr<Input> _input;
  swmm6_output* _output;

  unordered_map<string, unique_ptr<Provider>> _providers;

  unordered_map<string, unique_ptr<Simulation>> _simulations;
public:
  explicit swmm6(Input* inp): _input(inp) {}

  Input& get_input() const noexcept {
    return *_input;
  }



  bool register_provider(Provider* prv) {
    return std::get<1>(_providers.insert( { string{prv->sKind}, prv }));
  }
};

bool registerProvider(swmm6* prj, Provider* prv)
{
  prj->regsiter

}

int swmm6_create_provider(swmm6* prj, swmm6_provider* prv)
{
  bool success = prj->create_provider(prv);
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
  return swmm6_open_with(input, pPrj, NULL);
}

int swmm6_open_with(const char* inpName, swmm6** pPrj, const swmm6_io_module* io)
{
  Input* inp;
  try {
    inp = Input::open(inpName, io);
    if(inp == nullptr) {
      return SWMM_ERROR;
    }
  } catch (IoError& err) {
    return err.code();
  }
  swmm6* prj = new swmm6(inp);
  if(prj == NULL) {
    return SWMM_ERROR;
  }

  int rc = createBuiltinNodeProviders(prj);
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
  Input& inp = prj->get_input();
  Simulation* sim;
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
