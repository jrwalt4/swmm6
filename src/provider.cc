#include "provider.hh"

namespace swmm
{

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
  try {
    std::string& s_val = _values[col].emplace<std::string>(val);
    if(s_val.compare(val) == 0) {
      return true;
    }
    return false;
  } catch (std::exception& ex) {

  }
  return false;
}
bool ParamPack::set_int(int val, int col)
{
  try {
    int i_val = _values[col].emplace<int>(val);
    if(i_val == val) {
      return true;
    }
    return false;
  } catch (std::exception& ex) {

  }
  return false;
}
bool ParamPack::set_real(double val, int col)
{

  try {
    int d_val = _values[col].emplace<double>(val);
    if(d_val == val) {
      return true;
    }
    return false;
  } catch (std::exception& ex) {

  }
  return false;
}

}
