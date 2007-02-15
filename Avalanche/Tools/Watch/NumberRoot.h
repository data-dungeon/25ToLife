/*
**
**  File:   NumberRoot.h
**  Date:   August 14, 2000
**  By:     Bryant Collard
**  Desc:   Uses the Newton-Raphson method to find the integral root of
**          a real number.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef NUMBER_ROOT_H
#define NUMBER_ROOT_H

#include "basetype.h"

#define NUMBER_ROOT_MAX_ITER  1000
#define NUMBER_ROOT_TOLERANCE 1.0e-6

class CNumberRoot
{
  public:
    static bool Evaluate(real i_value, int i_power, real &o_root,
            bool &o_imag, real i_seed = (real)1,
            real i_tolerance = (real)NUMBER_ROOT_TOLERANCE,
            int i_maxIter = NUMBER_ROOT_MAX_ITER);
};

#endif //  NUMBER_ROOT_H
