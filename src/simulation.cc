#include "simulation.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int simulationOpen(const char* scenario, swmm6* prj, swmm6_simulation** outSim)
{
  swmm6_simulation* sim = (swmm6_simulation*) malloc(sizeof(*sim));
  if(sim == NULL) {
    return SWMM_ERROR;
  }
  sim->project = prj;
  sim->status = SIM_OPEN;
  sim->scenario = strdup(scenario);
  if(sim->scenario == NULL) {
    return SWMM_ERROR;
  }
  *outSim = sim;
  return SWMM_OK;
}

int simulationClose(swmm6_simulation* sim)
{
  if(sim->status != SIM_FINISHED) {
    puts("simulation not complete");
  }
  free((char* )sim->scenario);
  free(sim->vNodes);
  free(sim->vLinks);
  free(sim);
  return SWMM_OK;
}

int swmm6_finish(swmm6_simulation* sim)
{
  sim->status = SIM_FINISHED;
  return SWMM_OK;
}
