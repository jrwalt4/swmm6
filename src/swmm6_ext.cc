#include "swmm6_int.hh"

#include "input.hh"
#include "node.hh"
#include "object.hh"
#include "provider.hh"

#include <unordered_map>
#include <utility>

namespace swmm
{

template<class OBJ>
struct ExtensionObject: public OBJ
{
  swmm6_ext_object* _obj;
  swmm6_ext_module* _module;
  ExtensionObject(swmm6_ext_object* obj):
    OBJ(obj->uid, obj->name),
    _obj(obj),
    _module(obj->mod) {}

  ~ExtensionObject()
  {
    _module->xDestroy(_obj);
  }
};

template<class OBJ>
class ExtensionProvider: public ProviderBase
{
protected:
  swmm6_ext_module* _module;
public:
  ExtensionProvider(swmm6_ext_module* prv):
    ProviderBase(prv->sName, prv->nParams),
    _module(prv) {
      for(int i = 0 ; i < prv->nParams ; i++) {
        params[i] = prv->vParams[i];
      }
    }

  int read_params(Object& obj, ParamPack& values) override
  {
    ExtensionObject<OBJ>& ext = dynamic_cast<ExtensionObject<OBJ>&>(obj);
    return _module->xReadParams(ext._obj, values);
  }
};

class ExtensionNode: public ExtensionObject<Node>
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

  struct Provider: public ExtensionProvider<Node>
  {
    Provider(swmm6_node_module* mod): ExtensionProvider((swmm6_ext_module*) mod) {}

    Object* create_object(swmm6_uid uid, const char* name) override
    {
      return new ExtensionNode(((swmm6_ext_module*)_module)->xCreateObject(uid, name));
    }
  };
};

static std::unordered_map<std::string, ExtensionNode::Provider> nodeExtProviders;

int swmm6_create_node_module(swmm6* prj, swmm6_node_module* mod)
{
  auto [node, success] = nodeExtProviders.emplace(
    std::string{mod->xModule.sName},
    mod
  );
  if(success) {
    ExtensionNode::Provider& prv = (*node).second;
    return registerProvider(*prj, prv);
  }
  return SWMM_ERROR;
}
} // namespace swmm
