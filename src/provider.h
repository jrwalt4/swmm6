#ifndef SWMM_PROVIDER_H
#define SWMM_PROVIDER_H

#include "swmm6_int.h"
/*
int providerOpen(swmm6* prj, const char* prvModule, swmm6_provider** outPrv, void* userData);
*/
int providerGetBuilder(const swmm6_provider* prv, swmm6_builder** outBldr);
/*
int providerRead(swmm6_provider* prv, int param, const char* value, swmm6_builder* bldr);
*/
int providerCreateObject(const swmm6_provider* prv, swmm6_builder* bldr, swmm6_object** outObj);

int providerReleaseBuilder(const swmm6_provider* prv, swmm6_builder* bldr);
/*
int providerClose(swmm6_provider* prv);
*/

int providerResetBuilder(const swmm6_provider* prv, swmm6_builder* bldr);

int providerReadCursor(const swmm6_provider* prv, swmm6_input_cursor* cur, swmm6_object** outObj);

#endif
