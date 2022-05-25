#ifndef SWMM_INPUT_H
#define SWMM_INPUT_H

#include "swmm6_int.hh"

#include <string>
#include <variant>

namespace swmm
{

struct InputCursor
{
    virtual ~InputCursor() = default;
    virtual bool next() = 0;
    virtual int readInt(int col) = 0;
    virtual double readDouble(int col) = 0;
    virtual const std::string readText(int col) = 0;

    template <typename T>
    T read(int col);
};

struct Input
{
    virtual ~Input() = default;
    virtual swmm6_scenario_info* describeScenario(const char* scenario) = 0;
    virtual InputCursor* openCursor(const char* query, const swmm6_scenario_info& info) = 0;
    virtual void releaseScenario(swmm6_scenario_info* info) = 0;
};

Input* openInput(const char* input, const char* input_module);

}

swmm::Input* inputOpen(const char* sName, const char* sModule);

int inputDescribeScenario(const char* scenario, swmm6_input* inp, swmm6_scenario_info** info);

int inputReleaseScenario(swmm6_input* inp, swmm6_scenario_info* info);

int inputOpenCursor(swmm6_input* inp, const char* query, const swmm6_scenario_info* info, swmm6_input_cursor** outCursor);

int inputNext(swmm6_input_cursor* cur);

/* cursor reading methods */
int inputReadInt(swmm6_input_cursor* cur, int col);
double inputReadDouble(swmm6_input_cursor* cur, int col);
const char* inputReadText(swmm6_input_cursor* cur, int col);

int inputCloseCursor(swmm6_input_cursor* cursor);

int inputClose(swmm6_input* pInput);

#endif
