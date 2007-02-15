/*
**
**  File:   Gauss.h
**  Date:   April 21, 2000
**  By:     Bryant Collard
**  Desc:   Defines CGauss, a static class that uses an LU decomposition
**          implementation of Gauss elimination (formally, Doolittle
**          decomposition) to solve any number of simultaneous linear
**          equations and find inverses and unscaled and scaled determinants
**          of matricies of arbitrary rank.
**
**          In general, this class solves the equation:
**
**              Ax = b
**
**          where A is an n x n matrix, and x and b are vectors of length n.
**          This is done in two steps. First, A is decomposed into a
**          lower triangular matrix, L, with 1's on the diagonal and an
**          upper triangular matrix, U, such that A = LU. Because of their
**          structure, both L and U can be stored in a single matrix of the
**          same dimensions as A. The second step determines x from a
**          given right-hand side b.
**
**          During the process, each row in A is effectively (although
**          not literally) scaled by the maximum absolute value in the
**          row and then the rows are (again, effectively) swapped, or
**          pivoted, using scaled values as criteria, to improve numerical
**          stability.
**
**          Note: Since the functions can deal with any sized matrix and
**          since dynamic memory allocation is avoided, each of the functions
**          require user supplied scratch space of the appropriate size.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef GAUSS_H
#define GAUSS_H

#include "platform/BaseType.h"

#define DEFAULT_TOLERANCE  1.0e-4f

class CGauss
{
  public:
    static bool LUDecomp (float* io_coeff, int* o_pivot, int &o_numPivots,
            float* o_scale, const int i_rank,
            const float i_tolerance = DEFAULT_TOLERANCE);
    inline static bool LUDecomp (const float* i_coeff, float* o_lu,
            int* o_pivot, int &o_numPivots, float* o_scale,
            const int i_rank, const float i_tolerance = DEFAULT_TOLERANCE);

    static void LUSolve (const float* i_lu, const int* i_pivot, float* io_var,
            float* o_scratch, const int i_rank);
    inline static void LUSolve (const float* i_lu, const int* i_pivot,
            const float* i_rhs, float* o_result, float* o_scratch,
            const int i_rank);

    static bool Solve (float* io_coeff, float* io_var, int* o_pivot,
            float* o_scratch, const int i_rank,
            const float i_tolerance = DEFAULT_TOLERANCE);
    inline static bool Solve (const float* i_coeff, float* o_lu,
            const float* i_rhs, float* o_result, int* o_pivot,
            float* o_scratch, const int i_rank,
            const float i_tolerance = DEFAULT_TOLERANCE);

    static bool Invert (float* io_mat, int* o_pivot, float* o_scratch,
            const int i_rank, const float i_tolerance = DEFAULT_TOLERANCE);
    inline static bool Invert (const float* i_mat, float* o_result,
            int* o_pivot, float* o_scratch, const int i_rank,
            const float i_tolerance = DEFAULT_TOLERANCE);

    static float Determinant (float* io_mat, int* o_pivot, float* o_scale,
            const int i_rank,  bool &o_error,
            const float i_tolerance = DEFAULT_TOLERANCE);
    inline static float Determinant (const float* i_mat, float* o_scratch,
            int* o_pivot, float* o_scale, const int i_rank, bool &o_error,
            const float i_tolerance = DEFAULT_TOLERANCE);

    static float ScaledDeterminant (float* io_mat, int* o_pivot,
            float* o_scale, const int i_rank, bool &o_error,
            const float i_tolerance = DEFAULT_TOLERANCE);
    inline static float ScaledDeterminant (const float* i_mat,
            float* o_scratch, int* o_pivot, float* o_scale, const int i_rank,
            bool &o_error, const float i_tolerance = DEFAULT_TOLERANCE);
};

#define GAUSS_HPP
#include "mathUtil/Gauss.hpp"

#endif // GAUSS_H
