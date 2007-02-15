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

#include "platform/BaseType.h"

#define NUMBER_ROOT_MAX_ITER  1000
#define NUMBER_ROOT_TOLERANCE 1.0e-6f

class CNumberRoot
{
  public:
    static bool Evaluate(float i_value, int i_power, float &o_root,
            bool &o_imag, float i_seed = 1.0f,
            float i_tolerance = NUMBER_ROOT_TOLERANCE,
            int i_maxIter = NUMBER_ROOT_MAX_ITER);
};

#endif //  NUMBER_ROOT_H
