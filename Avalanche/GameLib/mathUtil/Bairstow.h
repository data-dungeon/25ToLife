/*
**
**  File:   Bairstow.h
**  Date:   June 8, 2000
**  By:     Bryant Collard
**  Desc:   Defines CBairstow, a static class that uses Bairstow's method
**          to find the roots of a polynomial.
**
**          Bairstow's method uses synthetic division to divide a 3rd or
**          higher order polynomial by a quadradic factor x^2-rx-s. The
**          result is a polynomial with an order reduced by 2 and a 1st
**          order remainder polynomial. If the coefficients of the remainder
**          are zero, the quadratic factor x^2-rx-s is a root of the
**          original polynomial. The method iterates on r and s until a
**          zero remainder is found. The process repeats on the reduced
**          polynomial until it is less than 3rd order. See "Numerical
**          Methods for Engineers, 3rd Edition" by Chapra and Canale (1998,
**          WCB/McGraw-Hill) pp. 178-183 for details on the method.
**
**          Note: Since the functions can deal with a polynomial of any
**          order and since dynamic memory allocation is avoided, each of
**          the functions require user supplied scratch space of the
**          appropriate size.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef BAIRSTOW_H
#define BAIRSTOW_H

#include "platform/BaseType.h"

#define BAIRSTOW_ERROR_ITERATION_LIMIT   -1
#define BAIRSTOW_ERROR_ILLFORMED_POLY    -2

#define DEFAULT_1ST_ORDER_SEED           0.0f
#define DEFAULT_0TH_ORDER_SEED           0.0f
#define DEFAULT_1ST_ORDER_ERROR_THRESH   0.005f
#define DEFAULT_0TH_ORDER_ERROR_THRESH   0.005f
#define DEFAULT_MAX_ITERATIONS           100

class CBairstow
{
  public:
    static int ParabolicRoots (int i_order, float* io_coeff, float* o_roots,
            float* o_scratch,
            float i_1stOrderSeed = DEFAULT_1ST_ORDER_SEED,
            float i_0thOrderSeed = DEFAULT_0TH_ORDER_SEED,
            float i_1stOrderErrorThresh = DEFAULT_1ST_ORDER_ERROR_THRESH,
            float i_0thOrderErrorThresh = DEFAULT_0TH_ORDER_ERROR_THRESH,
            int i_maxIterations = DEFAULT_MAX_ITERATIONS);
    static bool SolveParabolic (float i_1stOrderTerm, float i_0thOrderTerm,
            float &o_root1, float &o_root2, bool i_findComplex = true);
    static int Roots (int i_order, float* io_coeff, float* o_floatRoots,
            float* o_imaginaryRoots, float* o_scratch,
            float i_1stOrderSeed = DEFAULT_1ST_ORDER_SEED,
            float i_0thOrderSeed = DEFAULT_0TH_ORDER_SEED,
            float i_1stOrderErrorThresh = DEFAULT_1ST_ORDER_ERROR_THRESH,
            float i_0thOrderErrorThresh = DEFAULT_0TH_ORDER_ERROR_THRESH,
            int i_maxIterations = DEFAULT_MAX_ITERATIONS);
    static int RealRoots (int i_order, float* io_coeff, float* o_roots,
            float* o_scratch,
            float i_1stOrderSeed = DEFAULT_1ST_ORDER_SEED,
            float i_0thOrderSeed = DEFAULT_0TH_ORDER_SEED,
            float i_1stOrderErrorThresh = DEFAULT_1ST_ORDER_ERROR_THRESH,
            float i_0thOrderErrorThresh = DEFAULT_0TH_ORDER_ERROR_THRESH,
            int i_maxIterations = DEFAULT_MAX_ITERATIONS);
};

#endif // BAIRSTOW_H
