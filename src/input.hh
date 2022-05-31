#ifndef SWMM_INPUT_H
#define SWMM_INPUT_H

#include "swmm6_int.hh"

#include "provider.hh"

#include <string>
#include <tuple>
#include <variant>

namespace swmm
{

struct InputObjectReaderProps
{
    swmm6_uid uid;
    const char* name;
    const char* kind;
};

struct InputObjectReader
{
    virtual ~InputObjectReader() = default;
    virtual bool next() = 0;

    virtual int readParams(ParamPack& values) = 0;
};

struct InputObjectConstructorProps
{
    swmm6_uid uid;
    std::string name;
    std::string kind;
};

struct InputCursor
{
    virtual std::pair<bool, InputObjectConstructorProps> next() = 0;
};

struct Input
{
    virtual ~Input() = default;
    virtual InputCursor* openNodeCursor(const char* scenario) = 0;
    //virtual InputCursor* openLinkCursor(const char* scenario) = 0;

    virtual InputObjectReader* openReader(const char* kind, ParamDefPack& params) = 0;

    static Input* open(const char* input, const swmm6_io_module* input_module);
};

} // namespace swmm

#endif
