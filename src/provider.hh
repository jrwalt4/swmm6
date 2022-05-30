#ifndef SWMM_PROVIDER_H
#define SWMM_PROVIDER_H

#include "swmm6_int.hh"

#include "input.hh"
#include "object.hh"

#include <string>

namespace swmm
{
struct Provider
{
    virtual Object* read_cursor(InputObjectCursor& cursor) = 0;

    static Provider* from_extension(swmm6_provider* prv);
};
} // namespace swmm
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
