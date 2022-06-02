#ifndef SWMM6_INT_H
#define SWMM6_INT_H

#include "swmm6.h"
// pass SWMM_CORE value to the includes below
#define SWMM_CORE 1
#include "swmm6_ext.h"

// #include "provider.hh"

#define SWMM_DEBUG 1
#define VIRTUAL_CHECK(vtable, method) if( vtable->method == NULL ) return SWMM_NOIMPL;
#define VIRTUAL_CALL(vtable, method, args...) \
VIRTUAL_CHECK(vtable, method)\
return vtable->method(args);

namespace swmm
{
struct ProviderBase;
int registerProvider(swmm6& prj, ProviderBase& prv);

/**
 * Called by link module to load builtin links
 * (CONDUIT, WEIR, PUMP, etc.)
 */
int createBuiltinLinkProviders(swmm6* prj);

} // namespace swmm

#endif //SWMM6_INT_H
