/*
**
**  File:   NumberRoot.cpp
**  Date:   August 14, 2000
**  By:     Bryant Collard
**  Desc:   Uses the Newton-Raphson method to find the integral root of
**          a real number.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

/******************* includes ****************************/

#include "mathUtil/MathUtilPCH.h"




bool CNumberRoot::Evaluate(float i_value, int i_power, float &o_root,
        bool &o_imag, float i_seed, float i_tolerance, int i_maxIter)
{
    if (i_power == 0)
    {
        o_root = 1.0f;
        o_imag = false;
        if ((i_value > 0.999999f) && (i_value < 1.000001f))
            return(true);
        return(false);
    }

    int posPower = Math::Abs(i_power);

    if ((i_seed < 1.0e-4f) && (i_seed > -1.0e-4f))
        i_seed += 1.0f;

    if (i_value < -1.0e-4f)
    {
        if (posPower & 0x1)
            o_imag = false;
        else
        {
            i_value = -i_value;
            o_imag = true;
        }
    }
    else if (i_value > 1.0e-4f)
        o_imag = false;
    else
    {
        o_root = 0.0f;
        o_imag = false;
        return(true);
    }

    int i, j;
    float y, der, error;
    for (i = 0; i < i_maxIter; i++)
    {
        y = i_seed;
        for (j = 1; j < posPower; j++)
            y *= i_seed;
        y -= i_value;

        der = (float)posPower;
        for (j = posPower - 1; j > 0; j--)
            der *= i_seed;

        o_root = i_seed - y / der;

        error = (o_root - i_seed) / o_root;
        if ((error < i_tolerance) && (error > -i_tolerance))
        {
            if (i_power < 0)
            {
                o_root = 1.0f / o_root;
                if (o_imag)
                    o_root = -o_root;
            }
            return(true);
        }

        i_seed = o_root;
    }

    return(false);
}
