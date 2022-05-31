#ifndef SWMM_PROVIDER_H
#define SWMM_PROVIDER_H

#include "swmm6_int.hh"

#include "input.hh"
#include "object.hh"

#include <string>
#include <variant>
#include <vector>

namespace swmm
{

struct ParamDef
{
    std::string param_name;
    swmm6_param_type param_type;

    ParamDef() = default;
    ParamDef(swmm6_param_def& def): param_name(std::string{def.sName}), param_type(def.eType) {}
    ParamDef(const char* pname, swmm6_param_type ptype):
        param_name(std::string{pname}),
        param_type(ptype) {}
};

class ParamValue
{
    ParamDef _def;
    std::variant<
        swmm6_uid,
        int,
        double,
        std::string
    > _value;

public:
    const std::string& as_text() const;
    int as_int() const;
    double as_real() const;
};

struct ProviderBase
{
    std::string name;
    std::vector<ParamDef> params;

    ProviderBase(const char* prv_name, int n_params): name(std::string{prv_name}), params(n_params) {}
    ProviderBase(const char* prv_name, std::initializer_list<ParamDef> def_list):
        name(std::string{prv_name}),
        params(def_list) {}


    virtual Object* create_object(swmm6_uid uid, const char* name) = 0;
    virtual void read_params(Object& obj, std::vector<ParamValue>& values) = 0;
};

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

  void read_params(Object& obj, std::vector<ParamValue>& values) override
  {
    ExtensionObject& ext = dynamic_cast<ExtensionObject&>(obj);
    _module->xReadParams(ext._obj, reinterpret_cast<swmm6_param_value*>(values.data()));
  }
};

} // namespace swmm

#endif
