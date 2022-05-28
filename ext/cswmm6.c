
#ifndef PY_SSIZE_T_CLEAN
#define PY_SSIZE_T_CLEAN
#endif /* PY_SSIZE_T_CLEAN */
#include <Python.h>

#include "swmm6_ext.h"

SWMM_EXT_INIT1

#if __cplusplus
extern "C"
#endif
int swmm6_pyswmm6_init(swmm6* prj, char** ppErrMsg, const swmm6_api_routines* api)
{
    int rc;
    SWMM_EXT_INIT2( api )
    return SWMM_OK;
}
