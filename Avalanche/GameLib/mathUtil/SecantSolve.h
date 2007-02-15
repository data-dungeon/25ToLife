/*
**
**  File:   SecantSolve.h
**  Date:   September 5, 2000
**  By:     Bryant Collard
**  Desc:   Implements the secant method of solving equations.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef SECANT_SOLVE_H
#define SECANT_SOLVE_H

#include "platform/BaseType.h"

#define DEFAULT_INPUT_DELTA              0.01f
#define DEFAULT_ERROR_THRESHOLD          0.005f
#define DEFAULT_MAX_ITERATIONS           100

class CSecantSolve
{
  public:
    bool Solve(float &io_solution,
            float i_inputDelta = DEFAULT_INPUT_DELTA,
            float i_errorThreshold = DEFAULT_ERROR_THRESHOLD,
            int i_maxIterations = DEFAULT_MAX_ITERATIONS);
    virtual bool Plot(float i_minInput, float i_maxInput, int i_numPoints,
            const char* i_fileName, const char* i_xName, const char* i_yName);
  protected:
    virtual bool Evaluate(float i_input, float &o_result) = 0;
};

#endif // SECANT_SOLVE_H
