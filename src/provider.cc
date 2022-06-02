#include "provider.hh"

#include <algorithm>
#include <iterator>

using namespace std;

namespace swmm
{

using ParamType = swmm6_param_type;

ParamPack::ParamPack(ParamDefPack& param_def)
{
  transform(
    param_def.begin(),
    param_def.end(),
    back_inserter(_values),
    [](ParamDef& def) -> value_type {
      switch(def.param_type) {
        case ParamType::UID:
        case ParamType::INT: return int();
        case ParamType::REAL: return double();
        case ParamType::TEXT: return "";
        case ParamType::UNIT: return int();
      }
    }
  );
}

const std::string& ParamPack::get_text(int col) const
{
  return std::get<std::string>(_values[col]);
}

int ParamPack::get_int(int col) const
{
  return std::get<int>(_values[col]);
}

double ParamPack::get_real(int col) const
{
  return std::get<double>(_values[col]);
}


bool ParamPack::set_text(const char* val, int col)
{
  std::string& s_val = _values[col].emplace<std::string>(val);
  if(s_val.compare(val) == 0) {
    return true;
  }
  return false;
}
bool ParamPack::set_int(int val, int col)
{
  int i_val = _values[col].emplace<int>(val);
  if(i_val == val) {
    return true;
  }
  return false;
}
bool ParamPack::set_real(double val, int col)
{
  double d_val = _values[col].emplace<double>(val);
  if(d_val == val) {
    return true;
  }
  return false;
}

}
