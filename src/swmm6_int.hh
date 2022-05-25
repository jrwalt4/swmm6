#ifndef SWMM6_INT_H
#define SWMM6_INT_H

#include "swmm6.h"
// pass SWMM_CORE value to the includes below
#define SWMM_CORE 1
#include "swmm6_ext.h"

#define SWMM_DEBUG 1
#define VIRTUAL_CHECK(vtable, method) if( vtable->method == NULL ) return SWMM_NOIMPL;
#define VIRTUAL_CALL(vtable, method, args...) \
VIRTUAL_CHECK(vtable, method)\
return vtable->method(args);

swmm6_provider* swmmFindProvider(swmm6* prj, const char* sModuleName);

/**
 * Called by node module to load builtin nodes
 * (JUNCTION, STORAGE, OUTFALL, etc.)
 */
int swmmCreateBuiltinNodeProviders(swmm6* prj);


/**
 * Called by link module to load builtin links
 * (CONDUIT, WEIR, PUMP, etc.)
 */
int swmmCreateBuiltinLinkProviders(swmm6* prj);

#endif //SWMM6_INT_H
