/*
**
**  File:   Gauss.cpp
**  Date:   April 21, 2000
**  By:     Bryant Collard
**  Desc:   Implements CGauss, a static class that uses an LU decomposition
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

/******************* includes ****************************/

#include "mathUtil/MathUtilPCH.h"





/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    LUDecomp (static)
 *  Access:      public
 *  Description: Performs an LU decomposition on a matrix with partial
 *               pivoting based on scaled values.
 *  Arguments:
 *      io_coeff    - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. On input, it is the matrix to
 *                    be decomposed, on output, it is the decomposition.
 *      o_pivot     - An array of length <i_rank> that receives pivoting
 *                    information.
 *      o_numPivots - The number of pivots performed during decomposition.
 *      o_scale     - An array of length <i_rank> that is basically
 *                    scratch space but ends up containing the scale
 *                    factors used in pivoting.
 *      i_rank      - The rank of <io_coeff>.
 *      i_tolerance - An ill-conditioning tolerance. If any scaled entry
 *                    on the diagonal of the U matrix falls below
 *                    <i_tolerance>, the matrix is deemed ill-conditioned.
 *  Return:      true if well-conditioned, false if ill-conditioned.
 *
 **********************************************************************/
bool CGauss::LUDecomp (float* io_coeff, int* o_pivot, int &o_numPivots,
        float* o_scale, const int i_rank, const float i_tolerance)
{
    int ia, ib, j, index, currentRow, processRow, lastRow;
    float factor, max;
/*
 *  Initialize the pivot and scale tables.
 */
    for (ia = 0, j = 0; ia < i_rank; ia++)
    {
        o_pivot[ia] = ia;
/*
 *      Find the largest absolute value in the row.
 */
        o_scale[ia] = Math::Abs(io_coeff[j]);
        index = j + i_rank;
        for (j++; j < index; j++)
        {
            factor = Math::Abs(io_coeff[j]);
            if (factor > o_scale[ia])
                o_scale[ia] = factor;
        }
    }
/*
 *  Initialize the number of pivots.
 */
    o_numPivots = 0;
/*
 *  Process each row.
 */
    lastRow = i_rank - 1;
    for (ia = 0; ia < lastRow; ia++)
    {
/*
 *      Pivot.
 *
 *      Seed the pivot search with values from the current row and column.
 */
        currentRow = o_pivot[ia];
        max = Math::Abs(io_coeff[currentRow * i_rank + ia] /
                o_scale[currentRow]);
        index = ia;
/*
 *      Find the largest scale element in the current column of each
 *      unprocessed row.
 */
        for (ib = ia + 1; ib < i_rank; ib++)
        {
            processRow = o_pivot[ib];
            factor = Math::Abs(io_coeff[processRow * i_rank + ia] /
                    o_scale[processRow]);
            if (factor > max)
            {
                max = factor;
                index = ib;
            }
        }
/*
 *      Check for ill-conditioning.
 */
        if (max < i_tolerance)
            return (false);
/*
 *      Swap entries in the pivot table.
 */
        if (index != ia)
        {
            currentRow = o_pivot[index];
            o_pivot[index] = o_pivot[ia];
            o_pivot[ia] = currentRow;
            o_numPivots++;
        }
/*
 *      Subtract a factor times the current row from each of the remaining
 *      rows to eleminate the current column. Store the factor for each row
 *      in the current column of the row.
 */
        currentRow *= i_rank;
        for (ib = ia + 1; ib < i_rank; ib++)
        {
            processRow = o_pivot[ib] * i_rank;
            io_coeff[processRow + ia] /= io_coeff[currentRow + ia];
            factor = io_coeff[processRow + ia];
            for (j = ia + 1; j < i_rank; j++)
                io_coeff[processRow + j] -= factor * io_coeff[currentRow + j];
        }
    }
/*
 *  The remaining row has only one element. Check it for ill-conditioning.
 */
    currentRow = o_pivot[lastRow];
    if (Math::Abs(io_coeff[currentRow * i_rank + ia] / o_scale[currentRow]) <
            i_tolerance)
        return (false);
/*
 *  The decomposition was performed successfully.
 */
    return (true);
}

/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    LUSolve (static)
 *  Access:      public
 *  Description: Given an LU decomposition of a matrix A, and a right-
 *               hand side vector b, this function solves for x in the
 *               equation Ax = b.
 *  Arguments:
 *      i_lu        - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank> containing the LU decomposition.
 *      i_pivot     - An array of length <i_rank> that contains the
 *                    pivoting information generated in the decomposition
 *                    phase.
 *      io_var      - An array of length <i_rank>. On input, it contains
 *                    the right-hand side of the equation, b. On output,
 *                    it contains the solution, x.
 *      o_scratch   - An array of length <i_rank> used as scratch space.
 *                    (It ends up containing the vector d where Ld = b.)
 *      i_rank      - The rank of A.
 *  Return:      None
 *
 **********************************************************************/
void CGauss::LUSolve (const float* i_lu, const int* i_pivot, float* io_var,
        float* o_scratch, const int i_rank)
{
    int i, j, processRow, index;
    float sum;
/*
 *  Forward substitute to get D from LD = B.
 */
    index = i_pivot[0];
    o_scratch[index] = io_var[index];
    for (i = 1; i < i_rank; i++)
    {
        index = i_pivot[i];
        processRow = index * i_rank;
        sum = io_var[index];
        for (j = 0; j < i; j++)
        {
            sum -= i_lu[processRow + j] * o_scratch[i_pivot[j]];
        }
        o_scratch[index] = sum;
    }
/*
 *  Backward substitute to get X from UX = D.
 */
    i = i_rank - 1;
    index = i_pivot[i];
    io_var[i] = o_scratch[index] / i_lu[index * i_rank + i];
    for (i--; i >= 0; i--)
    {
        index = i_pivot[i];
        processRow = index * i_rank;
        sum = 0.0f;
        for (j = i + 1; j < i_rank; j++)
        {
            sum += i_lu[processRow + j] * io_var[j];
        }
        io_var[i] = (o_scratch[index] - sum) / i_lu[processRow + i];
    }
}

/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    Solve (static)
 *  Access:      public
 *  Description: Solves for x in the equation Ax = b.
 *  Arguments:
 *      io_coeff    - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. On input, it is the matrix to
 *                    be decomposed, on output, it ends up containing the
 *                    LU decompostion of A.
 *      io_var      - An array of length <i_rank>. On input, it contains
 *                    the right-hand side of the equation, b. On output,
 *                    it contains the solution, x.
 *      o_pivot     - An array of length <i_rank> that receives pivoting
 *                    information.
 *      o_scratch   - An array of length <i_rank> used as scratch space.
 *                    (It ends up containing the vector d where Ld = b.)
 *      i_rank      - The rank of A.
 *      i_tolerance - An ill-conditioning tolerance. If any scaled entry
 *                    on the diagonal of the U matrix falls below
 *                    <i_tolerance>, the matrix is deemed ill-conditioned.
 *  Return:      true if well-conditioned, false if ill-conditioned.
 *
 **********************************************************************/
bool CGauss::Solve (float* io_coeff, float* io_var, int* o_pivot,
        float* o_scratch, const int i_rank, const float i_tolerance)
{
    int numPivots;
/*
 *  Perform LU decomposition on the matrix.
 */
    if (LUDecomp(io_coeff, o_pivot, numPivots, o_scratch, i_rank, i_tolerance))
    {
/*
 *      Solve for the given right-hand side and return success.
 */
        LUSolve (io_coeff, o_pivot, io_var, o_scratch, i_rank);
        return (true);
    }
/*
 *  Fail if the matrix is ill-conditioned.
 */
    return (false);
}

/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    Invert (static)
 *  Access:      public
 *  Description: Finds the inverse of a matrix.
 *  Arguments:
 *      io_mat      - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. On input, it is the matrix to
 *                    be inverted, on output, it is the inverse.
 *      o_pivot     - An array of length <i_rank> that receives pivoting
 *                    information.
 *      o_scratch   - An array of length [(<i_rank> + 1) * <i_rank>] used
 *                    as scratch space.
 *      i_rank      - The rank of <io_mat>.
 *      i_tolerance - An ill-conditioning tolerance. If any scaled entry
 *                    on the diagonal of the U matrix falls below
 *                    <i_tolerance>, the matrix is deemed ill-conditioned.
 *  Return:      true if well-conditioned, false if ill-conditioned.
 *
 **********************************************************************/
bool CGauss::Invert (float* io_mat, int* o_pivot, float* o_scratch,
        const int i_rank, const float i_tolerance)
{
    int i, j, row, index;
/*
 *  Perform LU decomposition on the matrix. Fail if the matrix is
 *  ill-conditioned.
 */
    if (!LUDecomp (io_mat, o_pivot, i, o_scratch, i_rank, i_tolerance))
        return (false);
/*
 *  Each column in the inverse is the solution to the equations given
 *  a vector with all zeros except for a one in the row of interest.
 */
    float* scratch2 = o_scratch + (i_rank * i_rank);
    for (i = 0, row = 0; i < i_rank; i++, row += i_rank)
    {
        index = row + i_rank;
        for (j = row; j < index; j++)
            o_scratch[j] = 0.0f;
        o_scratch[row + i] = 1.0f;
        LUSolve (io_mat, o_pivot, o_scratch + row, scratch2, i_rank);
    }
/*
 *  The scratch matrix ends up dontaining the transpose of the inverse.
 *  Transpose the scratch matrix into the resulting matrix to get the
 *  true inverse.
 */
    for (row = 0, i = 0; row < i_rank; row++)
    {
        for (index = 0, j = row; index < i_rank; index++, j += i_rank, i++)
        {
            io_mat[i] = o_scratch[j];
        }
    }
/*
 *  Return success.
 */
    return (true);
}

/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    Determinant (static)
 *  Access:      public
 *  Description: Finds the determinant of a matrix.
 *  Arguments:
 *      io_mat      - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. On input, it is the source
 *                    matrix, on output, it ends up containing the
 *                    LU decompostion of the input.
 *      o_pivot     - An array of length <i_rank> that receives pivoting
 *                    information.
 *      o_scale     - An array of length <i_rank> that is basically
 *                    scratch space but ends up containing the scale
 *                    factors used in pivoting.
 *      i_rank      - The rank of <io_mat>.
 *      o_error     - A flag indcating the conditioning of the input
 *                    matrix. On return, it is true if the input matrix
 *                    is well-conditioned, false if it is ill-conditioned.
 *      i_tolerance - An ill-conditioning tolerance. If any scaled entry
 *                    on the diagonal of the U matrix falls below
 *                    <i_tolerance>, the matrix is deemed ill-conditioned.
 *  Return:      The determinant of the input matrix. If the matrix is
 *               ill-conditioned, zero is returned.
 *
 **********************************************************************/
float CGauss::Determinant (float* io_mat, int* o_pivot, float* o_scale,
        const int i_rank, bool &o_error, const float i_tolerance)
{
    int numPivots;
/*
 *  Perform LU decomposition on the matrix. Fail if the matrix is
 *  ill-conditioned.
 */
    if (!LUDecomp (io_mat, o_pivot, numPivots, o_scale, i_rank, i_tolerance))
    {
        o_error = true;
        return (0.0f);
    }
/*
 *  The matrix is not ill-conditioned.
 */
    o_error = false;
/*
 *  Multiply the diagonals together.
 */
    float result = io_mat[o_pivot[0] * i_rank];
    for (int i = 1; i < i_rank; i++)
        result *= io_mat[o_pivot[i] * i_rank + i];
/*
 *  The determinant changes sign for each pivot. If an odd number of pivots
 *  were performed, return the negative of the diagonal product; otherwise,
 *  return the pruduct.
 */
    if (numPivots & 0x1)
        return (-result);
    else
        return (result);
}


/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    ScaledDeterminant (static)
 *  Access:      public
 *  Description: Finds the scaled determinant of a matrix. This is found
 *               by first scaling each row in the matrix by the maximum
 *               absolute value in the row, then finding the determinant.
 *               This scaled determinant provides a better measure of the
 *               conditioning of a matrix than the normal determinant.
 *  Arguments:
 *      io_mat      - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. On input, it is the source
 *                    matrix, on output, it ends up containing the
 *                    LU decompostion of the input.
 *      o_pivot     - An array of length <i_rank> that receives pivoting
 *                    information.
 *      o_scale     - An array of length <i_rank> that is basically
 *                    scratch space but ends up containing the scale
 *                    factors used in pivoting and scaling the matrix.
 *      i_rank      - The rank of <io_mat>.
 *      o_error     - A flag indcating the conditioning of the input
 *                    matrix. On return, it is true if the input matrix
 *                    is well-conditioned, false if it is ill-conditioned.
 *      i_tolerance - An ill-conditioning tolerance. If any scaled entry
 *                    on the diagonal of the U matrix falls below
 *                    <i_tolerance>, the matrix is deemed ill-conditioned.
 *  Return:      The scaled determinant of the input matrix. If the matrix
 *               is ill-conditioned, zero is returned.
 *
 **********************************************************************/
float CGauss::ScaledDeterminant (float* io_mat, int* o_pivot, float* o_scale,
        const int i_rank, bool &o_error, const float i_tolerance)
{
    int numPivots;
/*
 *  Perform LU decomposition on the matrix. Fail if the matrix is
 *  ill-conditioned.
 */
    if (!LUDecomp (io_mat, o_pivot, numPivots, o_scale, i_rank, i_tolerance))
    {
        o_error = true;
        return (0.0f);
    }
/*
 *  The matrix is not ill-conditioned.
 */
    o_error = false;
/*
 *  Multiply the diagonals together, devided by the scale factor for each row.
 */
    int row = o_pivot[0];
    float result = io_mat[row * i_rank] / o_scale[row];
    for (int i = 1; i < i_rank; i++)
    {
        row = o_pivot[i];
        result *= io_mat[row * i_rank + i] / o_scale[row];
    }
/*
 *  The determinant changes sign for each pivot. If an odd number of pivots
 *  were performed, return the negative of the diagonal product; otherwise,
 *  return the pruduct.
 */
    if (numPivots & 0x1)
        return (-result);
    else
        return (result);
}
