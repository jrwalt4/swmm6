#ifndef SWMM_INPUT_H
#define SWMM_INPUT_H

#include "swmm6_int.hh"

#include <string>
#include <variant>

namespace swmm
{

struct InputObjectCursor
{
    std::string provider;
    virtual ~InputObjectCursor() = default;
    virtual bool next() = 0;
    virtual int readInt(int col) = 0;
    virtual double readDouble(int col) = 0;
    virtual const std::string readText(int col) = 0;

    swmm6_input_cursor* as_extension();

    template <typename T>
    T read(int col);
};

struct InputScenarioCursor
{
    virtual bool next() = 0;
    virtual InputObjectCursor* openObjectCursor() = 0;
};

struct Input
{
    virtual ~Input() = default;
    virtual InputScenarioCursor* openScenario(const char* scenario) = 0;

    static Input* open(const char* input, const swmm6_io_module* input_module);
};

} // namespace swmm

#endif
