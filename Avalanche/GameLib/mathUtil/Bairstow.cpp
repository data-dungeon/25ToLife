/*
**
**  File:   Bairstow.cpp
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

/******************* includes ****************************/

#include "mathUtil/MathUtilPCH.h"





#define ZERO_THRESHOLD  1.0e-10f

/**********************************************************************
 *
 *  Class:       CBairstow
 *  Function:    ParabolicRoots (static)
 *  Access:      public
 *  Description: Finds the parabolic (2nd order) roots of a polynomial.
 *               If the order of the source polynomial is odd, the final
 *               root returned includes a phantom zero root.
 *  Arguments:
 *      i_order               - The order of the polynomial to be solved.
 *      io_coeff              - The coefficients of the polynomial to be
 *                              solved stored in order from the constant
 *                              term. That is, the constant term is stored
 *                              in entry 0, the 1st order coefficient in
 *                              entry 1, etc. The memory is used in the
 *                              solution so the input contents of the array
 *                              are destroyed on return.
 *      o_roots               - An array that contains the parabolic roots
 *                              of the polynomial on return. If the order
 *                              of the polynomial is even, the dimension of
 *                              o_roots is i_order, if odd, i_order + 1.
 *                              The parabolic roots are expressed in the
 *                              form x^2-rx-s. They are saved in o_roots as;
 *                              r0, s0, r1, s1,... If the order of the
 *                              polynomial is odd, one of the linear roots
 *                              of the last parabolic root is zero, that is,
 *                              sn = 0.
 *      o_scratch             - An array of real values used in the solution.
 *                              It must be at least 2*(i_order+1) long.
 *      i_1stOrderSeed        - An initial guess for the first order term
 *                              of the first root found. A default value
 *                              is provided.
 *      i_0thOrderSeed        - An initial guess for the constant term
 *                              of the first root found. A default value
 *                              is provided.
 *      i_1stOrderErrorThresh - The allowed error for the first order term.
 *                              If the term is <1 and >-1, this is the
 *                              absolute error, otherwise, it is the
 *                              relative error. A default value is provided.
 *      i_0thOrderErrorThresh - The allowed error for the constant term.
 *                              If the term is <1 and >-1, this is the
 *                              absolute error, otherwise, it is the
 *                              relative error. A default value is provided.
 *      i_maxIterations       - The maximum number of iteration allowed for
 *                              the determination a any one root. A default
 *                              value is provided.
 *  Return:      If successful, the order of the polynomial solved. In
 *               general, it is i_order but it may be less if the highest
 *               order terms of the polynomial are zero. The return value
 *               is negative if the function failed. If it failed because
 *               the maximum number of iterations was reached without a
 *               solution, the return value is BAIRSTOW_ERROR_ITERATION_LIMIT.
 *               If failure is due to an ill-formed polynomial, it is
 *               BAIRSTOW_ERROR_ILLFORMED_POLY.
 *
 **********************************************************************/
int CBairstow::ParabolicRoots (int i_order, float* io_coeff, float* o_roots,
        float* o_scratch, float i_1stOrderSeed, float i_0thOrderSeed,
        float i_1stOrderErrorThresh, float i_0thOrderErrorThresh,
        int i_maxIterations)
{
/*
 *  Make sure the coefficient of the highest order term is non-zero.
 */
    for (; i_order > 0; i_order--)
    {
        if (Math::Abs(io_coeff[i_order]) > ZERO_THRESHOLD)
            break;
    }
/*
 *  Return an error if all coefficients of 1st order terms and higher are
 *  zero, that is, if the polynomial provided is actually a constant.
 */
    if (i_order <= 0)
        return (BAIRSTOW_ERROR_ILLFORMED_POLY);
/*
 *  Set the order of the polynomial being worked on to the order of the
 *  provided polynomial.
 */
    int order = i_order;
/*
 *  Find roots at zero. Count them and reduce the order of the polynomial
 *  accordingly.
 */
    int i;
    int zeroRoots = 0;
    while (Math::Abs(io_coeff[0]) <= ZERO_THRESHOLD)
    {
        zeroRoots++;
        for (i = 0; i < order; i++)
            io_coeff[i] = io_coeff[i + 1];
        order--;
    }
/*
 *  Setup term for polynomial reduction.
 */
    float* newCoeff = o_scratch;
    float* partDer = o_scratch + order + 1;
    float term1stOrder = i_1stOrderSeed;
    float term0thOrder = i_0thOrderSeed;
    int root = 0;
    int iter;
    float denom, delta1stOrder, delta0thOrder, error1stOrder;
/*
 *  Find pairs of roots until the order of the remaining polynomial is
 *  less than three.
 */
    while (order >= 3)
    {
/*
 *      Iterate to find the terms in the parabolic root.
 */
        for (iter = 0; iter < i_maxIterations; iter++)
        {
/*
 *          Find the proposed new coefficients and the partial derivatives
 *          of the new coefficients.
 */
            i = order - 1;
            newCoeff[order] = io_coeff[order];
            newCoeff[i] = io_coeff[i] + term1stOrder * newCoeff[order];
            partDer[order] = newCoeff[order];
            partDer[i] = newCoeff[i] + term1stOrder * partDer[order];
            for (i-- ; i >= 0; i--)
            {
                newCoeff[i] = io_coeff[i] + term1stOrder * newCoeff[i + 1] +
                        term0thOrder * newCoeff[i + 2];
                partDer[i] = newCoeff[i] + term1stOrder * partDer[i + 1] +
                        term0thOrder * partDer[i + 2];
            }
/*
 *          Find the new parabolic terms, if they can be found.
 */
            denom = partDer[2] * partDer[2] - partDer[3] * partDer[1];
            if (Math::Abs(denom) > ZERO_THRESHOLD)
            {
                delta1stOrder = (newCoeff[0] * partDer[3] -
                        newCoeff[1] * partDer[2]) / denom;
                delta0thOrder = (newCoeff[1] * partDer[1] -
                        newCoeff[0] * partDer[2]) / denom;
                term1stOrder += delta1stOrder;
                term0thOrder += delta0thOrder;
/*
 *              Estimate the error in the 1st order term. If the term is
 *              greater than 1 or less than -1, use the relative error;
 *              otherwise, use the absolute error.
 */
                if (Math::Abs(term1stOrder) > 1.0f)
                    error1stOrder = Math::Abs(delta1stOrder / term1stOrder);
                else
                    error1stOrder = Math::Abs(delta1stOrder);
/*
 *              If the 1st order error is small enough to stop, test the
 *              0th order error.
 */
                if (error1stOrder < i_1stOrderErrorThresh)
                {
                    if (Math::Abs(term0thOrder) > 1.0f)
                    {
                        if (Math::Abs(delta0thOrder / term0thOrder) <
                                i_0thOrderErrorThresh)
                            break;
                    }
                    else
                    {
                        if (Math::Abs(delta0thOrder) < i_0thOrderErrorThresh)
                            break;
                    }
                }
            }
/*
 *          If the equations cannot be solved, twiddle the parabolic terms and
 *          start over.
 */
            else
            {
                term1stOrder += 1.0f;
                term0thOrder += 1.0f;
                iter = -1;
            }
        }
/*
 *      Return failure if iteration limit was reached.
 */
        if (iter >= i_maxIterations)
            return (BAIRSTOW_ERROR_ITERATION_LIMIT);
/*
 *      Record the parabolic root terms.
 */
        o_roots[root++] = term1stOrder;
        o_roots[root++] = term0thOrder;
/*
 *      Reduce the polynomial.
 */
        order -= 2;
        for (i = 0; i <= order; i++)
        {
            io_coeff[i] = newCoeff[i + 2];
        }
    }
/*
 *  Process the reduced polynomial.
 */
    switch (order)
    {
/*
 *  If the order of the reduced polynomial is 1, effectively add a zero
 *  root. Reduce the number of zero roots to indicate that one was
 *  accounted for here.
 */
    case 1:
        o_roots[root++] = -io_coeff[0] / io_coeff[1];
        o_roots[root++] = 0.0f;
        zeroRoots--;
        break;
/*
 *  If the order of the reduced polynomial is 2, simply manipulate it
 *  to get a parabolic equation of the form: x^2 - rx - s = 0 and save
 *  the terms.
 */
    case 2:
        o_roots[root++] = -io_coeff[1] / io_coeff[2];
        o_roots[root++] = -io_coeff[0] / io_coeff[2];
        break;
    }
/*
 *  Store the zero roots.
 */
    while (zeroRoots > 0)
    {
        o_roots[root++] = 0.0f;
        o_roots[root++] = 0.0f;
        zeroRoots -= 2;
    }
/*
 *  Return the order of the polynomial that was processed. Note that
 *  if this value is odd, there is an extra zero root stored that must
 *  be ignored.
 */
    return (i_order);
}

/**********************************************************************
 *
 *  Class:       CBairstow
 *  Function:    SolveParabolic (static)
 *  Access:      public
 *  Description: Finds the (1st order) roots of 2nd order polynomial
 *               of the form x^2-rx-s.
 *  Arguments:
 *    i_1stOrderTerm - The first order term of the polynomial (r in
 *                     x^2-rx-s).
 *    i_0thOrderTerm - The constant term of the polynomial (s in
 *                     x^2-rx-s).
 *    o_root1        - If the roots are real, one of the roots. If the
 *                     roots are complex, the real term.
 *    o_root2        - If the roots are real, one of the roots. (If at least
 *                     one of the real roots is zero, it will be returned
 *                     in this term). If the roots are complex, the
 *                     imaginary term.
 *    i_findComplex  - A flag. If true, complex roots will be found, if
 *                     false, they will not be found. The root values
 *                     returned will be garbage.
 *  Return:      True if the roots are real, false if complex.
 *
 **********************************************************************/
bool CBairstow::SolveParabolic (float i_1stOrderTerm, float i_0thOrderTerm,
        float &o_root1, float &o_root2, bool i_findComplex)
{
    if (Math::Abs(i_0thOrderTerm) < ZERO_THRESHOLD)
    {
        o_root1 = i_1stOrderTerm;
        o_root2 = 0.0f;
        return (true);
    }

    float discriminant = i_1stOrderTerm * i_1stOrderTerm +
            4.0f * i_0thOrderTerm;

    if (discriminant < 0.0f)
    {
        if (i_findComplex)
        {
            o_root1 = 0.5f * i_1stOrderTerm;
            o_root2 = 0.5f * Math::Sqrt(-discriminant);
        }
        return (false);
    }

    o_root2 = Math::Sqrt(discriminant);
    o_root1 = 0.5f * (i_1stOrderTerm + o_root2);
    o_root2 = 0.5f * (i_1stOrderTerm - o_root2);
    return (true);
}

/**********************************************************************
 *
 *  Class:       CBairstow
 *  Function:    Roots (static)
 *  Access:      public
 *  Description: Finds the roots of a polynomial.
 *  Arguments:
 *      i_order               - The order of the polynomial to be solved.
 *      io_coeff              - The coefficients of the polynomial to be
 *                              solved stored in order from the constant
 *                              term. That is, the constant term is stored
 *                              in entry 0, the 1st order coefficient in
 *                              entry 1, etc. The memory is used in the
 *                              solution so the input contents of the array
 *                              are destroyed on return.
 *      o_realRoots           - An array that, on return, contains the
 *                              real terms of the roots of the polynomial.
 *                              If the order of the polynomial is even, the
 *                              dimension of this array is i_order, if odd,
 *                              i_order + 1.
 *      o_imaginaryRoots      - An array that, on return, contains the
 *                              imaginary terms of the roots of the polynomial.
 *                              If the order of the polynomial is even, the
 *                              dimension of this array is i_order, if odd,
 *                              i_order + 1. The corresponding entries in
 *                              o_realRoots and o_imaginaryRoots together form
 *                              the roots of the polynomial. If a root is
 *                              real, the imaginary term is zero.
 *      o_scratch             - An array of real values used in the solution.
 *                              It must be at least 2*(i_order+1) long.
 *      i_1stOrderSeed        - An initial guess for the first order term
 *                              of the first root found. A default value
 *                              is provided.
 *      i_0thOrderSeed        - An initial guess for the constant term
 *                              of the first root found. A default value
 *                              is provided.
 *      i_1stOrderErrorThresh - The allowed error for the first order term.
 *                              If the term is <1 and >-1, this is the
 *                              absolute error, otherwise, it is the
 *                              relative error. A default value is provided.
 *      i_0thOrderErrorThresh - The allowed error for the constant term.
 *                              If the term is <1 and >-1, this is the
 *                              absolute error, otherwise, it is the
 *                              relative error. A default value is provided.
 *      i_maxIterations       - The maximum number of iteration allowed for
 *                              the determination a any one root. A default
 *                              value is provided.
 *  Return:      If successful, the order of the polynomial solved. In
 *               general, it is i_order but it may be less if the highest
 *               order terms of the polynomial are zero. The return value
 *               is negative if the function failed. If it failed because
 *               the maximum number of iterations was reached without a
 *               solution, the return value is BAIRSTOW_ERROR_ITERATION_LIMIT.
 *               If failure is due to an ill-formed polynomial, it is
 *               BAIRSTOW_ERROR_ILLFORMED_POLY.
 *
 **********************************************************************/
int CBairstow::Roots (int i_order, float* io_coeff, float* o_realRoots,
        float* o_imaginaryRoots, float* o_scratch, float i_1stOrderSeed,
        float i_0thOrderSeed, float i_1stOrderErrorThresh,
        float i_0thOrderErrorThresh, int i_maxIterations)
{
/*
 *  Find the parabolic roots.
 */
    int order = ParabolicRoots(i_order, io_coeff, o_realRoots, o_scratch,
            i_1stOrderSeed, i_0thOrderSeed, i_1stOrderErrorThresh,
            i_0thOrderErrorThresh, i_maxIterations);
/*
 *  Return if there was an error finding roots.
 */
    if (order < 0)
        return (order);
/*
 *  Solve each parabolic.
 */
    int i;
    for (i = 0; i < order; i += 2)
    {
        if (SolveParabolic(o_realRoots[i], o_realRoots[i + 1], o_realRoots[i],
                o_imaginaryRoots[i], true))
        {
            o_realRoots[i + 1] = o_imaginaryRoots[i];
            o_imaginaryRoots[i] = 0.0f;
            o_imaginaryRoots[i + 1] = 0.0f;
        }
        else
        {
            o_realRoots[i + 1] = o_realRoots[i];
            o_imaginaryRoots[i + 1] = -o_imaginaryRoots[i];
        }
    }
    return (order);
}

/**********************************************************************
 *
 *  Class:       CBairstow
 *  Function:    RealRoots (static)
 *  Access:      public
 *  Description: Finds the real roots of a polynomial.
 *  Arguments:
 *      i_order               - The order of the polynomial to be solved.
 *      io_coeff              - The coefficients of the polynomial to be
 *                              solved stored in order from the constant
 *                              term. That is, the constant term is stored
 *                              in entry 0, the 1st order coefficient in
 *                              entry 1, etc. The memory is used in the
 *                              solution so the input contents of the array
 *                              are destroyed on return.
 *      o_roots               - An array that, on return, contains the
 *                              real roots of the polynomial. If the order of
 *                              the polynomial is even, the dimension of this
 *                              array is i_order, if odd, i_order + 1.
 *      o_scratch             - An array of real values used in the solution.
 *                              It must be at least 2*(i_order+1) long.
 *      i_1stOrderSeed        - An initial guess for the first order term
 *                              of the first root found. A default value
 *                              is provided.
 *      i_0thOrderSeed        - An initial guess for the constant term
 *                              of the first root found. A default value
 *                              is provided.
 *      i_1stOrderErrorThresh - The allowed error for the first order term.
 *                              If the term is <1 and >-1, this is the
 *                              absolute error, otherwise, it is the
 *                              relative error. A default value is provided.
 *      i_0thOrderErrorThresh - The allowed error for the constant term.
 *                              If the term is <1 and >-1, this is the
 *                              absolute error, otherwise, it is the
 *                              relative error. A default value is provided.
 *      i_maxIterations       - The maximum number of iteration allowed for
 *                              the determination a any one root. A default
 *                              value is provided.
 *  Return:      If successful, the number of real roots found. The
 *               return value is negative if the function failed. If it
 *               failed because the maximum number of iterations was
 *               reached without a solution, the return value is
 *               BAIRSTOW_ERROR_ITERATION_LIMIT. If failure is due to an
 *               ill-formed polynomial, it is BAIRSTOW_ERROR_ILLFORMED_POLY.
 *
 **********************************************************************/
int CBairstow::RealRoots (int i_order, float* io_coeff, float* o_roots,
        float* o_scratch, float i_1stOrderSeed, float i_0thOrderSeed,
        float i_1stOrderErrorThresh, float i_0thOrderErrorThresh,
        int i_maxIterations)
{
/*
 *  Find the parabolic roots.
 */
    int order = ParabolicRoots(i_order, io_coeff, o_roots, o_scratch,
            i_1stOrderSeed, i_0thOrderSeed, i_1stOrderErrorThresh,
            i_0thOrderErrorThresh, i_maxIterations);
/*
 *  Return if there was an error finding roots.
 */
    if (order < 0)
        return (order);
/*
 *  Solve each parabolic that has real roots.
 */
    int i, root;
    for (i = 0, root = 0; i < order; i += 2)
    {
        if (SolveParabolic(o_roots[i], o_roots[i + 1], o_roots[root],
                o_roots[root + 1], false))
            root += 2;
    }
/*
 *  If the order is odd, the last parabolic roots will have a zero root
 *  and the second root found while solving the parabolic will be zero.
 *  It is there for convienience and should be ignored.
 */
    if (order & 0x1)
        return (root - 1);
    return (root);
}
