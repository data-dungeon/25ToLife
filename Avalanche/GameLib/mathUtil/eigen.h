/***************************************************************************/
/***************************************************************************/
#ifndef EIGEN_H
#define EIGEN_H

#include "platform/BaseType.h"

class Eigen
{
public:
	Eigen(void) {}
	virtual ~Eigen(void) {}

	// set the matrix for eigensolving
	float &Matrix(int row, int col);
	void SetMatrix(float **mat);

	// get the results of eigensolving (eigenvectors are columns of matrix)
	float GetEigenvalue(int i) const;
	float GetEigenvector(int row, int col) const;
	float *GetEigenvalue(void);
	float **GetEigenvector(void);

	// solve eigensystem
	void EigenStuff2(void);
	void EigenStuff3(void);
	void EigenStuff4(void);
	void EigenStuffN(void);
	void EigenStuff (void);

	// solve eigensystem, use decreasing sort on eigenvalues
	void DecrSortEigenStuff2(void);
	void DecrSortEigenStuff3(void);
	void DecrSortEigenStuff4(void);
	void DecrSortEigenStuffN(void);
	void DecrSortEigenStuff (void);

	// solve eigensystem, use increasing sort on eigenvalues
	void IncrSortEigenStuff2(void);
	void IncrSortEigenStuff3(void);
	void IncrSortEigenStuff4(void);
	void IncrSortEigenStuffN(void);
	void IncrSortEigenStuff (void);

protected:
	int m_size;
	float **m_mat;
	float *m_diag;
	float *m_subd;

	// Householder reduction to tridiagonal form
	static void Tridiagonal2(float **mat, float *diag, float *subd);
	static void Tridiagonal3(float **mat, float *diag, float *subd);
	static void Tridiagonal4(float **mat, float *diag, float *subd);
	static void TridiagonalN(int size, float **mat, float *diag, float *subd);

	// QL algorithm with implicit shifting, applies to tridiagonal matrices
	static bool QLAlgorithm(int size, float *diag, float *subd, float **mat);

	// sort eigenvalues from largest to smallest
	static void DecreasingSort(int size, float *eigval, float **eigvec);

	// sort eigenvalues from smallest to largest
	static void IncreasingSort(int size, float *eigval, float **eigvec);
};

/***************************************************************************/
// A quick, fast 3 dimensional eigen
/***************************************************************************/
class Eigen3 : public Eigen
{
public:
	Eigen3(void);
	~Eigen3(void) {}

private:
	float *m_matptrdata[3];
	float m_matdata[9];
	float m_diagdata[3];
	float m_subddata[3];
};

/***************************************************************************/
// An n dimension eigen, much slower (uses heap)
/***************************************************************************/
class EigenN : public Eigen
{
public:
	EigenN(int size);
	~EigenN(void);
};

// inlines

#ifdef GCN
#include "mathUtil/eigen.hpp"
#else
#include "eigen.hpp"
#endif //GCN

#endif
