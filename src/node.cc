#include "node.hh"

#include "nodes/junction.hh"

using namespace std;
namespace swmm
{

static JunctionNode::Provider junctionProvider;

int createBuiltinNodeProviders(swmm6& prj)
{
  return registerProvider(prj, junctionProvider);
}

} // namespace swmm
