#include "provider.hh"

namespace swmm
{

const std::string& ParamValue::as_text() const
{
  return std::get<std::string>(_value);
}

int ParamValue::as_int() const
{
  return std::get<int>(_value);
}

double ParamValue::as_real() const
{
  return std::get<double>(_value);
}



}
