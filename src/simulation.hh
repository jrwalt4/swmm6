#ifndef SWMM_SIMULATION_H
#define SWMM_SIMULATION_H

#include "swmm6_int.hh"

#include "link.hh"
#include "node.hh"

#include <memory>
#include <string>
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

class Simulation
{
  swmm6& _project;
  std::string _scenario;
  std::unordered_map<swmm6_uid, std::unique_ptr<Node>> _nodes;
  std::unordered_map<swmm6_uid, std::unique_ptr<Link>> _links;
  SimulationStatus _status;

public:
  Simulation(swmm6& prj, const char* scenario):
    _project(prj),
    _scenario(scenario) {}

  /**
   * @brief `swmm6_simulation*` is an opaque handle to a swmm::Simulation
   *
   * @return swmm6_simulation*
   */
  operator swmm6_simulation*()
  {
    return reinterpret_cast<swmm6_simulation*>(this);
  }

  Node& get_node(swmm6_uid node_uid) const;
  bool add_node(Node* node);
  Link& get_link(swmm6_uid link_uid) const;
  bool add_link(Link* link);

  SimulationStatus get_status() const;
};

int simulationOpen(const char* scenario, swmm6* prj, swmm6_simulation** outSim);

int simulationClose(swmm6_simulation* sim);

int simulationAddObject(swmm6_simulation* sim);

} // namespace swmm

#endif
