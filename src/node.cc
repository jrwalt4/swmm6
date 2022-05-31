#include "node.hh"

#include "input.hh"
#include "provider.hh"

using namespace std;

namespace swmm
{

class JunctionNode: public Node
{

  double _invert;
  double _rim;

public:
  using Node::Node;


  struct Provider: public ProviderBase
  {
    Provider(): ProviderBase(
      "JUNCTION",
      {{"INVERT", swmm6_param_type::REAL}, {"RIM", swmm6_param_type::REAL}}
    ) {}

    JunctionNode* create_object(swmm6_uid uid, const char* name) override
    {
      return new JunctionNode(uid, name);
    }
    void read_params(Object& obj, ParamPack& values) override
    {
      JunctionNode& jxn = dynamic_cast<JunctionNode&>(obj);
      jxn._invert = values.get_real(0);
      jxn._rim = values.get_real(1);
    }
  };

  double get_depth() override
  {
    return _rim - _invert;
  }

  double get_invert() override
  {
    return _invert;
  }
};

static JunctionNode::Provider junctionProvider;

int createBuiltinNodeProviders(swmm6& prj)
{
  return registerProvider(prj, junctionProvider);
}

} // namespace swmm
