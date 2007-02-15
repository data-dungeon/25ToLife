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

#include "stdafx.h"
#include "NumberRoot.h"
#include "basetype.h"

bool CNumberRoot::Evaluate(real i_value, int i_power, real &o_root,
        bool &o_imag, real i_seed, real i_tolerance, int i_maxIter)
{
    if (i_power == 0)
    {
        o_root = (real)1;
        o_imag = false;
        if ((i_value > (real)0.999999) && (i_value < (real)1.000001))
            return(true);
        return(false);
    }

    int posPower = (i_power < 0 ? -i_power : i_power);

    if ((i_seed < (real)1.0e-4) && (i_seed > (real)-1.0e-4))
        i_seed += (real)1;

    if (i_value < (real)-1.0e-4)
    {
        if (posPower & 0x1)
            o_imag = false;
        else
        {
            i_value = -i_value;
            o_imag = true;
        }
    }
    else if (i_value > (real)1.0e-4)
        o_imag = false;
    else
    {
        o_root = (real)0;
        o_imag = false;
        return(true);
    }

    int i, j;
    real y, der, error;
    for (i = 0; i < i_maxIter; i++)
    {
        y = i_seed;
        for (j = 1; j < posPower; j++)
            y *= i_seed;
        y -= i_value;

        der = (real)posPower;
        for (j = posPower - 1; j > 0; j--)
            der *= i_seed;

        o_root = i_seed - y / der;

        error = (o_root - i_seed) / o_root;
        if ((error < i_tolerance) && (error > -i_tolerance))
        {
            if (i_power < 0)
            {
                o_root = (real)1 / o_root;
                if (o_imag)
                    o_root = -o_root;
            }
            return(true);
        }

        i_seed = o_root;
    }

    return(false);
}
