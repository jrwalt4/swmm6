#include "object.hh"

namespace swmm
{
ExtensionObject::~ExtensionObject()
{
  _module->xDestroy(_obj);
}

} // namespace swmm

