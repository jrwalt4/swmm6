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

class ExtensionNode: public ExtensionObject, public Node
{
public:
  using ExtensionObject::ExtensionObject;

  double get_depth() override
  {
    return ((swmm6_node_module*)_module)->xGetDepth((swmm6_ext_node*) _obj);
  }

  double get_invert() override
  {
    return ((swmm6_node_module*)_module)->xGetInvert((swmm6_ext_node*) _obj);
  }

  struct Provider: public ExtensionProvider
  {
    Provider(swmm6_node_module* mod): ExtensionProvider((swmm6_ext_module*) mod) {}

    Object* create_object(swmm6_uid uid, const char* name) override
    {
      return new ExtensionNode(((swmm6_ext_module*)_module)->xCreateObject(uid, name));
    }
  };
};

static JunctionNode::Provider junctionProvider;

int createBuiltinNodeProviders(swmm6& prj)
{
  return registerProvider(prj, junctionProvider);
}

} // namespace swmm

#include <unordered_map>
#include <utility>

static std::unordered_map<std::string, swmm::ExtensionNode::Provider> nodeExtProviders;

int swmm6_create_node_module(swmm6* prj, swmm6_node_module* mod)
{
  auto [node, success] = nodeExtProviders.emplace(
    std::piecewise_construct,
    std::string{mod->xModule.sName},
    mod
  );
  if(success) {
    swmm::ExtensionNode::Provider& prv = (*node).second;
    return swmm::registerProvider(*prj, prv);
  }
  return SWMM_ERROR;
}

