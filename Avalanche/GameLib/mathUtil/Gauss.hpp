/*
**
**  File:   Gauss.cpp
**  Date:   April 21, 2000
**  By:     Bryant Collard
**  Desc:   Implements inlined member functions of CGauss, a static class
**          that uses an LU decomposition implementation of Gauss
**          elimination (formally, Doolittle decomposition) to solve any
**          number of simultaneous linear equations and find inverses and
**          unscaled and scaled determinants of matricies of arbitrary rank.
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

#ifdef GAUSS_HPP
#undef GAUSS_HPP

#include <string.h>

/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    LUDecomp (static)
 *  Access:      public
 *  Description: Performs an LU decomposition on a matrix with partial
 *               pivoting based on scaled values.
 *  Arguments:
 *      i_coeff     - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. It is the matrix to be
 *                    decomposed.
 *      o_lu        - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. It contains the LU decomposition
 *                    of the input.
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
inline bool CGauss::LUDecomp (const float* i_coeff, float* o_lu, int* o_pivot,
        int &o_numPivots, float* o_scale, const int i_rank,
        const float i_tolerance)
{
    memcpy((void*)o_lu, (const void*)i_coeff, i_rank * i_rank * sizeof(float));
    return (LUDecomp (o_lu, o_pivot, o_numPivots, o_scale, i_rank,
            i_tolerance));
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
 *      i_rhs       - An array of length <i_rank>. The right-hand side of
 *                    the equation, b.
 *      o_result    - An array of length <i_rank>. It recieves the
 *                    solution, x.
 *      o_scratch   - An array of length <i_rank> used as scratch space.
 *                    (It ends up containing the vector d where Ld = b.)
 *      i_rank      - The rank of A.
 *  Return:      None
 *
 **********************************************************************/
inline void CGauss::LUSolve (const float* i_lu, const int* i_pivot,
        const float* i_rhs, float* o_result, float* o_scratch,
        const int i_rank)
{
    memcpy((void*)o_result, (const void*)i_rhs, i_rank * sizeof(float));
    LUSolve (i_lu, i_pivot, o_result, o_scratch, i_rank);
}

/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    Solve (static)
 *  Access:      public
 *  Description: Solves for x in the equation Ax = b.
 *  Arguments:
 *      i_coeff     - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. It is the matrix to be
 *                    decomposed.
 *      o_lu        - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. It ends up containing the LU
 *                    decomposition of the input.
 *      i_rhs       - An array of length <i_rank>. The right-hand side of
 *                    the equation, b.
 *      o_result    - An array of length <i_rank>. It recieves the
 *                    solution, x.
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
inline bool CGauss::Solve (const float* i_coeff, float* o_lu,
        const float* i_rhs, float* o_result, int* o_pivot, float* o_scratch,
        const int i_rank, const float i_tolerance)
{
    memcpy((void*)o_lu, (const void*)i_coeff, i_rank * i_rank * sizeof(float));
    memcpy((void*)o_result, (const void*)i_rhs, i_rank * sizeof(float));
    return (Solve (o_lu, o_result, o_pivot, o_scratch, i_rank, i_tolerance));
}

/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    Invert (static)
 *  Access:      public
 *  Description: Finds the inverse of a matrix.
 *  Arguments:
 *      i_mat       - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. The matrix to be inverted.
 *      o_result    - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. The inverse.
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
inline bool CGauss::Invert (const float* i_mat, float* o_result, int* o_pivot,
        float* o_scratch, const int i_rank, const float i_tolerance)
{
    memcpy((void*)o_result, (const void*)i_mat,
            i_rank * i_rank * sizeof(float));
    return (Invert (o_result, o_pivot, o_scratch, i_rank, i_tolerance));
}

/**********************************************************************
 *
 *  Class:       CGauss
 *  Function:    Determinant (static)
 *  Access:      public
 *  Description: Finds the determinant of a matrix.
 *  Arguments:
 *      i_mat       - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. The source matrix.
 *                    LU decompostion of the input.
 *      o_scratch   - A vector of length <i_rank> * <i_rank>. On output, it
 *                    ends up containing the LU decompostion of the input.
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
inline float CGauss::Determinant (const float* i_mat, float* o_scratch,
        int* o_pivot, float* o_scale, const int i_rank,  bool &o_error,
        const float i_tolerance)
{
    memcpy((void*)o_scratch, (const void*)i_mat,
            i_rank * i_rank * sizeof(float));
    return (Determinant (o_scratch, o_pivot, o_scale, i_rank, o_error,
            i_tolerance));
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
 *      i_mat       - A square matrix (stored in row order) of dimension
 *                    <i_rank> X <i_rank>. The source matrix.
 *                    LU decompostion of the input.
 *      o_scratch   - A vector of length <i_rank> * <i_rank>. On output, it
 *                    ends up containing the LU decompostion of the input.
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
inline float CGauss::ScaledDeterminant (const float* i_mat, float* o_scratch,
        int* o_pivot, float* o_scale, const int i_rank, bool &o_error,
        const float i_tolerance)
{
    memcpy((void*)o_scratch, (const void*)i_mat,
            i_rank * i_rank * sizeof(float));
    return (ScaledDeterminant (o_scratch, o_pivot, o_scale, i_rank, o_error,
            i_tolerance));
}

#endif // GAUSS_HPP
