/*
**
**  File:   Cholesky.h
**  Date:   May 2, 2000
**  By:     Bryant Collard
**  Desc:   Defines CCholesky, a static class that uses a Cholesky
**          decomposition to implement Gauss elimination to solve any
**          number of a special class of simultaneous linear equations.
**
**          In general, this class solves the equation:
**
**              Ax = b
**
**          where A is an n x n symmetric matrix, and x and b are vectors
**          of length n. This is done in two steps. First, A is decomposed
**          into a lower triangular matrix, L, with 1's on the diagonal and
**          a diagonal matrix, D, such that A = LDL^T (L^T is the transpose
**          of L). Because of their structures, A can be stored in an upper
**          triagular matrix and both L and D can be stored in a single upper
**          triagular matrix of the same dimensions as A. The second step
**          determines x from a given right-hand side b.
**
**          This method prevents pivoting to deal with numerical instability.
**          It is guaranteed to be stable only if A is positive-definite
**          (x^TAx > 0 for all non-zero vectors x); however, positive-
**          definitness is not checked.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef CHOLESKY_H
#define CHOLESKY_H

#include "platform/BaseType.h"

#define CHOLESKY_DEFAULT_TOLERANCE  1.0e-4f

class CCholesky
{
  public:
    static bool LDLtDecomp (float* io_coeff, const int i_rank,
            const float i_tolerance = CHOLESKY_DEFAULT_TOLERANCE);
    inline static bool LDLtDecomp (const float* i_coeff, float* o_ldlt,
            const int i_rank, const float i_tolerance = CHOLESKY_DEFAULT_TOLERANCE);

    static void LDLtSolve (const float* i_ldlt, float* io_var, const int i_rank);
    inline static void LDLtSolve (const float* i_ldlt, const float* i_rhs,
            float* o_result, const int i_rank);

    static bool Solve (float* io_coeff, float* io_var, const int i_rank,
            const float i_tolerance = CHOLESKY_DEFAULT_TOLERANCE);
    inline static bool Solve (const float* i_coeff, float* o_ldlt,
            const float* i_rhs, float* o_result, const int i_rank,
            const float i_tolerance = CHOLESKY_DEFAULT_TOLERANCE);
  private:
    inline static int Row(const int i_rowIndex, const int i_rank);
};

#define CHOLESKY_HPP
#include "mathUtil/Cholesky.hpp"

#endif // CHOLESKY_H
