#include "simulation.hh"

namespace swmm
{

Node& Simulation::get_node(swmm6_uid node_uid) const
{
  return *_nodes.at(node_uid);
}

Link& Simulation::get_link(swmm6_uid link_uid) const
{
  return *_links.at(link_uid);
}

SimulationStatus Simulation::get_status() const
{
  return _status;
}

}
