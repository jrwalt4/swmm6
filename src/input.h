#ifndef SWMM_INPUT_H
#define SWMM_INPUT_H

#include "swmm6_int.h"

int inputOpen(const char* sName, const char* sModule, swmm6_input** outInput);

int inputDescribeScenario(const char* scenario, swmm6_input* inp, swmm6_scenario_info** info);

int inputReleaseScenario(swmm6_input* inp, swmm6_scenario_info* info);

int inputOpenCursor(swmm6_input* inp, const char* query, swmm6_provider* prv, swmm6_input_cursor** outCursor);

int inputNext(swmm6_input_cursor* cur);

/* cursor reading methods */
int inputReadInt(swmm6_input_cursor* cur, int col);
double inputReadDouble(swmm6_input_cursor* cur, int col);
const char* inputReadText(swmm6_input_cursor* cur, int col);

int inputCloseCursor(swmm6_input_cursor* cursor);

int inputClose(swmm6_input* pInput);

#endif
