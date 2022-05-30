#ifndef SWMM_SIMULATION_H
#define SWMM_SIMULATION_H

#include "swmm6_int.hh"

#include "link.hh"
#include "node.hh"

#include <memory>
#include <unordered_map>

namespace swmm
{

typedef enum SimulationStatus
{
  SIM_OPEN,
  SIM_WORKING,
  SIM_DONE,
  SIM_FINISHED
} SimulationStatus;

class Simulation;

typedef Simulation swmm6_simulation;

class Simulation
{
  swmm6* project;
  const char* scenario;
  std::unordered_map<swmm6_uid, std::unique_ptr<Node>> _nodes;
  std::unordered_map<swmm6_uid, std::unique_ptr<Link>> _links;
  SimulationStatus status;

public:

  Node& get_node(swmm6_uid node_uid) const;
  Link& get_link(swmm6_uid link_uid) const;
};

int simulationOpen(const char* scenario, swmm6* prj, swmm6_simulation** outSim);

int simulationClose(swmm6_simulation* sim);

int simulationAddObject(swmm6_simulation* sim);

} // namespace swmm

#endif
