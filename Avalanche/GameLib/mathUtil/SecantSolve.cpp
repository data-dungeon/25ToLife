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

/******************* includes ****************************/

#include "mathUtil/MathUtilPCH.h"





#define SMALL_NUMBER  1.0e-9f

bool CSecantSolve::Solve(float &io_solution, float i_inputDelta,
        float i_errorThreshold, int i_maxIterations)
{
    float prevResult, currentResult;
    if (!Evaluate(io_solution - i_inputDelta, prevResult))
        return (false);

    for (int i = 0; i < i_maxIterations; i++)
    {
        if (!Evaluate(io_solution, currentResult))
            return (false);
/*
 *      Trap straight horizontal line.
 */
        float limit = SMALL_NUMBER * Math::Abs(i_inputDelta * currentResult);
        if ((prevResult >= (currentResult - limit)) &&
                (prevResult <= (currentResult + limit)))
            return(false);

        i_inputDelta *= currentResult / (prevResult - currentResult);
        io_solution += i_inputDelta;
        if (Math::Abs(io_solution) > 1.0f)
        {
            if (Math::Abs(i_inputDelta / io_solution) <
                    i_errorThreshold)
                return (true);
        }
        else
        {
            if (Math::Abs(i_inputDelta) < i_errorThreshold)
                return (true);
        }
        prevResult = currentResult;
    }
    return (false);
}

bool CSecantSolve::Plot(float i_minInput, float i_maxInput, int i_numPoints,
        const char* i_fileName, const char* i_xName, const char* i_yName)
{
#ifdef WIN32
    if (!i_fileName || !i_fileName[0] || (i_numPoints < 1))
        return(false);

    FILE* fp = fopen(i_fileName, "w");
    if (fp == NULL)
        return(false);

    if (i_xName && i_xName[0])
        fprintf(fp, "%s\t", i_xName);
    else
        fprintf(fp, "Input\t");

    if (i_yName && i_yName[0])
        fprintf(fp, "%s\n", i_yName);
    else
        fprintf(fp, "Output\n");

    float delta;
    if (i_numPoints == 1)
        delta = 0.0f;
    else
        delta = (i_maxInput - i_minInput) / ((float)(i_numPoints - 1));

    for (int i = 0; i < i_numPoints; i++)
    {
        float output;
        if (!Evaluate(i_minInput, output))
        {
            fclose(fp);
            return(false);
        }
        fprintf(fp, "%f\t%f\n", i_minInput, output);
        i_minInput += delta;
    }
    fclose(fp);
    return(true);
#else
    return(false);
#endif
}
