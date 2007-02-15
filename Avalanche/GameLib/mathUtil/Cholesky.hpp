/*
**
**  File:   Cholesky.cpp
**  Date:   April 21, 2000
**  By:     Bryant Collard
**  Desc:   Implements inlined member functions of CCholesky, a static class
**          that uses a Cholesky decomposition to implement Gauss elimination
**          to solve any number of a special class of simultaneous linear
**          equations.
**
**          The core functions in the class usually provide solutions in
**          place, that is, the result is stored in what was the input.
**          These inlined functions leave the inputs unchanged, storing
**          the outputs in provided arguments.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifdef CHOLESKY_HPP
#undef CHOLESKY_HPP

#include <string.h>

/**********************************************************************
 *
 *  Class:       CCholesky
 *  Function:    LDLtDecomp (static)
 *  Access:      public
 *  Description: Performs an LDL^T decomposition on a symmetric matrix.
 *  Arguments:
 *      i_coeff     - The upper triangle (stored in row order) of an
 *                    <i_rank> X <i_rank> square matrix. For example, if
 *                    <i_rank> were 4, the following figure illustrates
 *                    the contents of <io_coeff> where the numbers are
 *                    indicies into <io_coeff> and x represents an element
 *                    of the matrix that is not stored in <io_coeff>.
 *
 *                                     0 1 2 3
 *                                     x 4 5 6
 *                                     x x 7 8
 *                                     x x x 9
 *
 *                    It is the matrix to be decomposed.
 *      o_ldlt      - An array that receives the LDL^T decomposition.
 *      i_rank      - The rank of the input matrix.
 *      i_tolerance - An ill-conditioning tolerance. If element of D
 *                    falls below <i_tolerance>, the matrix is deemed
 *                    ill-conditioned.
 *  Return:      true if well-conditioned, false if ill-conditioned.
 *
 **********************************************************************/
inline bool CCholesky::LDLtDecomp (const float* i_coeff, float* o_ldlt,
        const int i_rank, const float i_tolerance)
{
    memcpy((void*)o_ldlt, (const void*)i_coeff,
            ((i_rank * (i_rank + 1)) >> 1) * sizeof(float));
    return (LDLtDecomp (o_ldlt, i_rank, i_tolerance));
}

/**********************************************************************
 *
 *  Class:       CCholesky
 *  Function:    LDLtSolve (static)
 *  Access:      public
 *  Description: Given an LDL^T decomposition of a matrix A, and a right-
 *               hand side vector b, this function solves for x in the
 *               equation Ax = b.
 *  Arguments:
 *      i_lu        - An upper-triangular matrix (stored in row order)
 *                    containing the LDL^T decomposition.
 *      i_rhs       - An array of length <i_rank>. The right-hand side of
 *                    the equation, b.
 *      o_result    - An array of length <i_rank>. It recieves the
 *                    solution, x.
 *      i_rank      - The rank of A.
 *  Return:      None
 *
 **********************************************************************/
inline void CCholesky::LDLtSolve (const float* i_ldlt, const float* i_rhs,
        float* o_result, const int i_rank)
{
    memcpy((void*)o_result, (const void*)i_rhs, i_rank * sizeof(float));
    LDLtSolve (i_ldlt, o_result, i_rank);
}

/**********************************************************************
 *
 *  Class:       CCholesky
 *  Function:    Solve (static)
 *  Access:      public
 *  Description: Solves for x in the equation Ax = b.
 *  Arguments:
 *      i_coeff     - The upper triangle (stored in row order) of an
 *                    <i_rank> X <i_rank> square matrix. For example, if
 *                    <i_rank> were 4, the following figure illustrates
 *                    the contents of <io_coeff> where the numbers are
 *                    indicies into <io_coeff> and x represents an element
 *                    of the matrix that is not stored in <io_coeff>.
 *
 *                                     0 1 2 3
 *                                     x 4 5 6
 *                                     x x 7 8
 *                                     x x x 9
 *
 *                    It is the matrix to be decomposed.
 *      o_ldlt      - An array that ends up receiving the LDL^T decomposition.
 *      i_rhs       - An array of length <i_rank>. The right-hand side of
 *                    the equation, b.
 *      o_result    - An array of length <i_rank>. It recieves the
 *                    solution, x.
 *      i_rank      - The rank of A.
 *      i_tolerance - An ill-conditioning tolerance. If element of D
 *                    falls below <i_tolerance>, the matrix is deemed
 *                    ill-conditioned.
 *  Return:      true if well-conditioned, false if ill-conditioned.
 *
 **********************************************************************/
inline bool CCholesky::Solve (const float* i_coeff, float* o_ldlt,
        const float* i_rhs, float* o_result, const int i_rank,
        const float i_tolerance)
{
    memcpy((void*)o_ldlt, (const void*)i_coeff,
            ((i_rank * (i_rank + 1)) >> 1) * sizeof(float));
    memcpy((void*)o_result, (const void*)i_rhs, i_rank * sizeof(float));
    return (Solve (o_ldlt, o_result, i_rank, i_tolerance));
}

/**********************************************************************
 *
 *  Class:       CCholesky
 *  Function:    Row (static)
 *  Access:      private
 *  Description: Finds the "virtual index" of the beginning of a row
 *               in an upper-triangular matrix. For example, if the
 *               rank of the whole matrix were 4, the following figure
 *               illustrates how the upper-triangular matrix is stored
 *               where the plain numbers are indicies into the array and
 *               and the numbers in ()'s are virtual indicies.
 *
 *                                 0   1   2   3
 *                                (3)  4   5   6
 *                                (5) (6)  7   8
 *                                (6) (7) (8)  9
 *
 *  Arguments:
 *      i_rowIndex - The row of interest.
 *      i_rank     - The rank of the matrix.
 *  Return:      The virtual index of the beginning of the row.
 *
 **********************************************************************/
inline int CCholesky::Row(const int i_rowIndex, const int i_rank)
{
    return ((i_rowIndex * ((i_rank << 1) - i_rowIndex - 1)) >> 1);
}

#endif // CHOLESKY_HPP
