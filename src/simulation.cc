#include "simulation.hh"

namespace swmm
{

const std::string& Simulation::scenario() const
{
  return _scenario;
}

Node& Simulation::get_node(swmm6_uid node_uid) const
{
  return *_nodes.at(node_uid);
}

bool Simulation::add_node(Node* node)
{
  return _nodes.emplace(node->uid, node).second;
}

Link& Simulation::get_link(swmm6_uid link_uid) const
{
  return *_links.at(link_uid);
}

bool Simulation::add_link(Link* link)
{
  return _links.emplace(link->uid, link).second;
}

SimulationStatus Simulation::get_status() const
{
  return _status;
}

}
