/*
**
**  File:   Cholesky.cpp
**  Date:   May 2, 2000
**  By:     Bryant Collard
**  Desc:   Implements CCholesky, a static class that uses a modified
**          Cholesky decomposition to implement Gauss elimination to solve
**          any number of a special class of simultaneous linear equations.
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

/******************* includes ****************************/

#include "mathUtil/MathUtilPCH.h"





/**********************************************************************
 *
 *  Class:       CCholesky
 *  Function:    LDLtDecomp (static)
 *  Access:      public
 *  Description: Performs an LDL^T decomposition on a symmetric matrix.
 *  Arguments:
 *      io_coeff    - The upper triangle (stored in row order) of an
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
 *                    On input, it is the matrix to be decomposed, on
 *                    output, it is the decomposition.
 *      i_rank      - The rank of the input matrix.
 *      i_tolerance - An ill-conditioning tolerance. If element of D
 *                    falls below <i_tolerance>, the matrix is deemed
 *                    ill-conditioned.
 *  Return:      true if well-conditioned, false if ill-conditioned.
 *
 **********************************************************************/
bool CCholesky::LDLtDecomp (float* io_coeff, const int i_rank,
        const float i_tolerance)
{
    int i, j, k, index, row;
    float sum, scale;
/*
 *  Process each row of the matrix.
 */
    for (i = 0, index = 0; i < i_rank; i++)
    {
/*
 *      The first entry in the row is the diagonal.
 */
        sum = 0.0f;
        for (k = 0; k < i; k++)
        {
            row = Row(k, i_rank);
            sum += io_coeff[row + k] * io_coeff[row + i] * io_coeff[row + i];
        }
        io_coeff[index] -= sum;
/*
 *      Check for ill-conditioning.
 */
        if (Math::Abs(io_coeff[index]) < i_tolerance)
            return (false);
        scale = 1.0f / io_coeff[index];
/*
 *      Now process off-diagonal terms.
 */
        for (j = i + 1, index++; j < i_rank; j++, index++)
        {
            sum = 0.0f;
            for (k = 0; k < i; k++)
            {
                row = Row(k, i_rank);
                sum += io_coeff[row + k] * io_coeff[row + i] *
                        io_coeff[row + j];
            }
            io_coeff[index] = scale * (io_coeff[index] - sum);
        }
    }
/*
 *  Return success.
 */
    return (true);
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
 *      io_var      - An array of length <i_rank>. On input, it contains
 *                    the right-hand side of the equation, b. On output,
 *                    it contains the solution, x.
 *      i_rank      - The rank of A.
 *  Return:      None
 *
 **********************************************************************/
void CCholesky::LDLtSolve (const float* i_ldlt, float* io_var, const int i_rank)
{
    int i, k, row;
    float sum;
/*
 *  Forward substitute to get r from Lr = b.
 */
    for (i = 1; i < i_rank; i++)
    {
        sum = io_var[0] * i_ldlt[i];
        for (k = 1; k < i; k++)
        {
            sum += io_var[k] * i_ldlt[Row(k, i_rank) + i];
        }
        io_var[i] -= sum;
    }
/*
 *  Backward substitute to get x from DL^Tx = r.
 */
    for (i = i_rank - 1; i >= 0; i--)
    {
        row = Row(i, i_rank);
        sum = 0.0f;
        for (k = i + 1; k < i_rank; k++)
        {
            sum += i_ldlt[row + k] * io_var[k];
        }
        io_var[i] = (io_var[i] / i_ldlt[row + i]) - sum;
    }
}

/**********************************************************************
 *
 *  Class:       CCholesky
 *  Function:    Solve (static)
 *  Access:      public
 *  Description: Solves for x in the equation Ax = b.
 *  Arguments:
 *      io_coeff    - The upper triangle (stored in row order) of an
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
 *                    On input, it is the matrix to be decomposed, on
 *                    output, it ends up containing the LDL^T
 *                    decomposition of A.
 *      io_var      - An array of length <i_rank>. On input, it contains
 *                    the right-hand side of the equation, b. On output,
 *                    it contains the solution, x.
 *      i_rank      - The rank of A.
 *      i_tolerance - An ill-conditioning tolerance. If element of D
 *                    falls below <i_tolerance>, the matrix is deemed
 *                    ill-conditioned.
 *  Return:      true if well-conditioned, false if ill-conditioned.
 *
 **********************************************************************/
bool CCholesky::Solve (float* io_coeff, float* io_var, const int i_rank,
        const float i_tolerance)
{
/*
 *  Perform LDL^T decomposition on the matrix.
 */
    if (LDLtDecomp(io_coeff, i_rank, i_tolerance))
    {
/*
 *      Solve for the given right-hand side and return success.
 */
        LDLtSolve (io_coeff, io_var, i_rank);
        return (true);
    }
/*
 *  Fail if the matrix is ill-conditioned.
 */
    return (false);
}
