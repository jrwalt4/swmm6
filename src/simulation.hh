#ifndef SWMM_SIMULATION_H
#define SWMM_SIMULATION_H

#include "swmm6_int.hh"

typedef enum SimulationStatus
{
  SIM_OPEN,
  SIM_WORKING,
  SIM_DONE,
  SIM_FINISHED
} SimulationStatus;

struct swmm6_simulation
{
  swmm6* project;
  const char* scenario;
  swmm6_node** vNodes;
  swmm6_link** vLinks;
  int nNodes;
  int nLinks;
  SimulationStatus status;
};

int simulationOpen(const char* scenario, swmm6* prj, swmm6_simulation** outSim);

int simulationClose(swmm6_simulation* sim);

int simulationAddObject(swmm6_simulation* sim);

#endif
