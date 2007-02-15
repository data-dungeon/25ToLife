/***************************************************************************/
/***************************************************************************/
#ifndef LINEARSYSTEM_H
#define LINEARSYSTEM_H

#include "platform/BaseType.h"

class LinearSystem
{
public:
    LinearSystem(void) {}

    // 2x2 and 3x3 systems (avoids overhead of Gaussian elimination)
    static float &Tolerance(void);
    static bool Solve2(const float A[2][2], const float B[2], float X[2]);
    static bool Solve3(const float A[3][3], const float B[3], float X[3]);

    // convenience for allocation, memory is zeroed out
    static float **NewMatrix(int size);
    static void DeleteMatrix(int size, float **A);
    static float *NewVector(int size);
    static void DeleteVector(int size, float *B);

    static bool Inverse(int size, float **A);
    // Input:
    //     A[size][size], entries are A[row][col]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     A[size][size], inverse matrix

    static bool Solve(int size, float **A, float *B);
    // Input:
    //     A[size][size] coefficient matrix, entries are A[row][col]
    //     B[size] vector, entries are B[row]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     A[size][size] is inverse matrix
    //     B[size] is solution x to Ax = B

    static bool SolveTri(int size, float *A, float *B, float *C, float *R, float *U);
    // Input:
    //     Matrix is tridiagonal.
    //     Lower diagonal A[size-1]
    //     Main  diagonal B[size]
    //     Upper diagonal C[size-1]
    //     Right-hand side R[size]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     U[size] is solution

    static bool SolveConstTri(int size, float fA, float fB, float fC, float *R, float *U);
    // Input:
    //     Matrix is tridiagonal.
    //     Lower diagonal is constant, A
    //     Main  diagonal is constant, B
    //     Upper diagonal is constant, C
    //     Right-hand side Rr[size]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     U[size] is solution

    static bool SolveSymmetric(int size, float **A, float *B);
    // Input:
    //     A[size][size] symmetric matrix, entries are A[row][col]
    //     B[size] vector, entries are B[row]
    // Output:
    //     return value is TRUE if successful, FALSE if(nearly) singular
    //     decomposition A = L D L^t(diagonal terms of L are all 1)
    //         A[i][i] = entries of diagonal D
    //         A[i][j] for i > j = lower triangular part of L
    //     B[size] is solution to x to Ax = B

    static bool SymmetricInverse(int size, float **A, float **AInv);
    // Input:
    //     A[size][size], entries are A[row][col]
    // Output:
    //     return value is TRUE if successful, FALSE if algorithm failed
    //     AInv[size][size], inverse matrix

protected:
    // tolerance for 2x2 and 3x3 system solving
    static float tolerance;
};

/***************************************************************************/
/***************************************************************************/
inline float &LinearSystem::Tolerance(void)
{
    return tolerance;
}

#endif
