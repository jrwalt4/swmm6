#ifndef SWMM_PROVIDER_H
#define SWMM_PROVIDER_H

#include "swmm6_int.hh"

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

struct ParamPack
{
  std::vector<std::variant<
        int,
        double,
        std::string
    >> _values;

  int length() const
  {
    return _values.size();
  }

  const std::string& get_text(int col) const;
  int get_int(int col) const;
  double get_real(int col) const;

  bool set_text(const char* val, int col);
  bool set_int(int val, int col);
  bool set_real(double val, int col);

  operator swmm6_param_pack*()
  {
    return reinterpret_cast<swmm6_param_pack*>(this);
  }

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
    virtual void read_params(Object& obj, ParamPack& values) = 0;
};

} // namespace swmm

#endif
