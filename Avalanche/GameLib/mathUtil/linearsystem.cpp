/***************************************************************************/
/***************************************************************************/

/******************* includes ****************************/

#include "mathUtil/MathUtilPCH.h"





#define LINEAR_SYSTEM_DEFAULT_TOLERANCE 1e-06f

float LinearSystem::tolerance = LINEAR_SYSTEM_DEFAULT_TOLERANCE;

/***************************************************************************/
/***************************************************************************/
float **LinearSystem::NewMatrix(
int size)
{
	float **a = new float *[size];
	ASSERT(a);

	for (int row = 0; row < size; row++)
	{
		a[row] = new float[size];
		memset(a[row], 0, size * sizeof(float));
	}

	return(a);
}

/***************************************************************************/
/***************************************************************************/
void LinearSystem::DeleteMatrix(
int size,
float **a)
{
	for (int row = 0; row < size; row++)
		delete [] a[row];
	delete [] a;
}

/***************************************************************************/
/***************************************************************************/
float *LinearSystem::NewVector(
int size)
{
	float *b = new float[size];
	ASSERT(b);
	memset(b, 0 , size * sizeof(float));
	return(b);
}

/***************************************************************************/
/***************************************************************************/
void LinearSystem::DeleteVector(
int size,
float *b)
{
	delete [] b;
}

/***************************************************************************/
/***************************************************************************/
bool LinearSystem::Solve2(
const float a[2][2],
const float b[2],
float x[2])
{
	float det = a[0][0] * a[1][1] - a[0][1] * a[1][0];
	if (Math::Abs(det) < tolerance)
		return(false);

	float invDet = 1.0f / det;
	x[0] = (a[1][1] * b[0] - a[0][1] * b[1]) * invDet;
	x[1] = (a[0][0] * b[1] - a[1][0] * b[0]) * invDet;

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool LinearSystem::Solve3(
const float a[3][3],
const float b[3],
float x[3])
{
	float aInv[3][3];

	aInv[0][0] = a[1][1] * a[2][2] - a[1][2] * a[2][1];
	aInv[0][1] = a[0][2] * a[2][1] - a[0][1] * a[2][2];
	aInv[0][2] = a[0][1] * a[1][2] - a[0][2] * a[1][1];
	aInv[1][0] = a[1][2] * a[2][0] - a[1][0] * a[2][2];
	aInv[1][1] = a[0][0] * a[2][2] - a[0][2] * a[2][0];
	aInv[1][2] = a[0][2] * a[1][0] - a[0][0] * a[1][2];
	aInv[2][0] = a[1][0] * a[2][1] - a[1][1] * a[2][0];
	aInv[2][1] = a[0][1] * a[2][0] - a[0][0] * a[2][1];
	aInv[2][2] = a[0][0] * a[1][1] - a[0][1] * a[1][0];

	float det = a[0][0] * aInv[0][0] + a[0][1] * aInv[1][0] + a[0][2] * aInv[2][0];
	if (Math::Abs(det) < tolerance)
		return(false);

	float invDet = 1.0f / det;
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
			aInv[row][col] *= invDet;
	}

	x[0] = aInv[0][0] * b[0] + aInv[0][1] * b[1] + aInv[0][2] * b[2];
	x[1] = aInv[1][0] * b[0] + aInv[1][1] * b[1] + aInv[1][2] * b[2];
	x[2] = aInv[2][0] * b[0] + aInv[2][1] * b[1] + aInv[2][2] * b[2];

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool LinearSystem::Inverse(
int size,
float **a)
{
	int *colIndex = new int[size];
	ASSERT(colIndex);

	int *rowIndex = new int[size];
	ASSERT(rowIndex);

	bool *pivoted = new bool[size];
	ASSERT(pivoted);
	memset(pivoted, 0, size * sizeof(bool));

	int i1, i2, row = 0, col = 0;
	float save;

	// elimination by full pivoting
	for (int i0 = 0; i0 < size; i0++)
	{
		// search matrix (excluding pivoted rows) for maximum absolute entry
		float max = 0.0f;
		for (i1 = 0; i1 < size; i1++)
		{
			if (!pivoted[i1])
			{
				for (i2 = 0; i2 < size; i2++)
				{
					if (!pivoted[i2])
					{
						float absVal = Math::Abs(a[i1][i2]);
						if (absVal > max)
						{
							max = absVal;
							row = i1;
							col = i2;
						}
					}
				}
			}
		}

		if (max == 0.0f)
		{
			// matrix is not invertible
			delete [] colIndex;
			delete [] rowIndex;
			delete [] pivoted;
			return(false);
		}

		pivoted[col] = true;

		// swap rows so that A[col][col] contains the pivot entry
		if (row != col)
		{
			float *rowPtr = a[row];
			a[row] = a[col];
			a[col] = rowPtr;
		}

		// keep track of the permutations of the rows
		rowIndex[i0] = row;
		colIndex[i0] = col;

		// scale the row so that the pivot entry is 1
		float inv = 1.0f / a[col][col];
		a[col][col] = 1.0f;
		for (i2 = 0; i2 < size; i2++)
			a[col][i2] *= inv;

		// zero out the pivot column locations in the other rows
		for (i1 = 0; i1 < size; i1++)
		{
			if (i1 != col)
			{
				save = a[i1][col];
				a[i1][col] = 0.0f;
				for (i2 = 0; i2 < size; i2++)
					a[i1][i2] -= a[col][i2] * save;
			}
		}
	}

	// reorder rows so that A[][] stores the inverse of the original matrix
	for (i1 = size-1; i1 >= 0; i1--)
	{
		if ( rowIndex[i1] != colIndex[i1] )
		{
			for (i2 = 0; i2 < size; i2++)
			{
				save = a[i2][rowIndex[i1]];
				a[i2][rowIndex[i1]] = a[i2][colIndex[i1]];
				a[i2][colIndex[i1]] = save;
			}
		}
	}

	delete [] colIndex;
	delete [] rowIndex;
	delete [] pivoted;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool LinearSystem::Solve(
int size,
float **a,
float *b)
{
	int *colIndex = new int[size];
	ASSERT(colIndex);

	int *rowIndex = new int[size];
	ASSERT(rowIndex);

	bool *pivoted = new bool[size];
	ASSERT(pivoted);
	memset(pivoted, 0 ,size * sizeof(bool));

	int i1, i2, row = 0, col = 0;
	float save;

	// elimination by full pivoting
	for (int i0 = 0; i0 < size; i0++)
	{
		// search matrix (excluding pivoted rows) for maximum absolute entry
		float max = 0.0f;
		for (i1 = 0; i1 < size; i1++)
		{
			if (!pivoted[i1])
			{
				for (i2 = 0; i2 < size; i2++)
				{
					if (!pivoted[i2])
					{
						float absVal = Math::Abs(a[i1][i2]);
						if (absVal > max)
						{
							max = absVal;
							row = i1;
							col = i2;
						}
					}
				}
			}
		}

		if (max == 0.0f)
		{
			// matrix is not invertible
			delete [] colIndex;
			delete [] rowIndex;
			delete [] pivoted;
			return(false);
		}

		pivoted[col] = true;

		// swap rows so that A[col][col] contains the pivot entry
		if (row != col)
		{
			float *rowPtr = a[row];
			a[row] = a[col];
			a[col] = rowPtr;

			save = b[row];
			b[row] = b[col];
			b[col] = save;
		}

		// keep track of the permutations of the rows
		rowIndex[i0] = row;
		colIndex[i0] = col;

		// scale the row so that the pivot entry is 1
		float inv = 1.0f / a[col][col];
		a[col][col] = 1.0f;
		for (i2 = 0; i2 < size; i2++)
			a[col][i2] *= inv;
		b[col] *= inv;

		// zero out the pivot column locations in the other rows
		for (i1 = 0; i1 < size; i1++)
		{
			if (i1 != col)
			{
				save = a[i1][col];
				a[i1][col] = 0.0f;
				for (i2 = 0; i2 < size; i2++)
					a[i1][i2] -= a[col][i2] * save;
				b[i1] -= b[col] * save;
			}
		}
	}

	// reorder rows so that A[][] stores the inverse of the original matrix
	for (i1 = size-1; i1 >= 0; i1--)
	{
		if (rowIndex[i1] != colIndex[i1])
		{
			for (i2 = 0; i2 < size; i2++)
			{
				save = a[i2][rowIndex[i1]];
				a[i2][rowIndex[i1]] = a[i2][colIndex[i1]];
				a[i2][colIndex[i1]] = save;
			}
		}
	}

	delete [] colIndex;
	delete [] rowIndex;
	delete [] pivoted;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool LinearSystem::SolveTri(
int size,
float *a,
float *b,
float *c,
float *r,
float *u)
{
	if (b[0] == 0.0f)
		return(false);

	float *d = new float[size - 1];
	ASSERT(d);

	float e = b[0];
	float invE = 1.0f / e;
	u[0] = r[0] * invE;

	int i0, i1;
	for (i0 = 0, i1 = 1; i1 < size; i0++, i1++)
	{
		d[i0] = c[i0] * invE;
		e = b[i1] - a[i0] * d[i0];
		if (e == 0.0f)
		{
			delete [] d;
			return(false);
		}

		invE = 1.0f / e;
		u[i1] = (r[i1] - a[i0]*u[i0]) * invE;
	}

	for (i0 = size - 1, i1 = size - 2; i1 >= 0; i0--, i1--)
		u[i1] -= d[i1] * u[i0];

	delete [] d;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool LinearSystem::SolveConstTri(
int size,
float a,
float b,
float c,
float *r,
float *u)
{
	if (b == 0.0f)
		return(false);

	float *d = new float[size - 1];
	ASSERT(d);

	float e = b;
	float invE = 1.0f / e;
	u[0] = r[0] * invE;

	int i0, i1;
	for (i0 = 0, i1 = 1; i1 < size; i0++, i1++)
	{
		d[i0] =  c *invE;
		e = b - a * d[i0];
		if (e == 0.0f)
		{
			delete [] d;
			return(false);
		}

		invE = 1.0f / e;
		u[i1] = (r[i1] - a*u[i0])*invE;
	}

	for (i0 = size - 1, i1 = size - 2; i1 >= 0; i0--, i1--)
		u[i1] -= d[i1] * u[i0];

	delete [] d;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool LinearSystem::SolveSymmetric (
int size,
float **a,
float *b)
{
	// A = L D L^t decomposition with diagonal terms of L equal to 1.  The
	// algorithm stores D terms in A[i][i] and off-diagonal L terms in
	// A[i][j] for i > j.  (G. Golub and C. Van Loan, Matrix Computations)
	const float tolerance = LINEAR_SYSTEM_DEFAULT_TOLERANCE;

	int i0, i1;
	float *v = new float[size];
	ASSERT( v );

	for (i1 = 0; i1 < size; i1++)
	{
		for (i0 = 0; i0 < i1; i0++)
			v[i0] = a[i1][i0] * a[i0][i0];

		v[i1] = a[i1][i1];
		for (i0 = 0; i0 < i1; i0++)
			v[i1] -= a[i1][i0] * v[i0];

		a[i1][i1] = v[i1];
		if (Math::Abs(v[i1]) <= tolerance)
		{
			delete [] v;
			return(false);
		}

		float inv = 1.0f / v[i1];
		for (i0 = i1+1; i0 < size; i0++)
		{
			for (int i2 = 0; i2 < i1; i2++)
				a[i0][i1] -= a[i0][i2] * v[i2];
			a[i0][i1] *= inv;
		}
	}

	delete [] v;

	// Solve Ax = B.

	// Forward substitution:  Let z = DL^t x, then Lz = B.  Algorithm
	// stores z terms in B vector.
	for (i0 = 0; i0 < size; i0++)
	{
		for (i1 = 0; i1 < i0; i1++)
			b[i0] -= a[i0][i1] * b[i1];

	}

	// Diagonal division:  Let y = L^t x, then Dy = z.  Algorithm stores
	// y terms in B vector.
	for (i0 = 0; i0 < size; i0++)
	{
		if (Math::Abs(a[i0][i0]) <= tolerance)
			return(false);
		b[i0] /= a[i0][i0];
	}

	// Back substitution:  Solve L^t x = y.  Algorithm stores x terms in
	// B vector.
	for (i0 = size-2; i0 >= 0; i0--)
	{
		for (i1 = i0+1; i1 < size; i1++)
			b[i0] -= a[i1][i0] * b[i1];
	}

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool LinearSystem::SymmetricInverse(
int size,
float **a,
float **aInv)
{
	// Same algorithm as SolveSymmetric, but applied simultaneously to
	// columns of identity matrix.
	float *v = new float[size];
	ASSERT(v);

	int i0, i1;
	for (i0 = 0; i0 < size; i0++)
	{
		for (i1 = 0; i1 < size; i1++)
			aInv[i0][i1] = (i0 != i1 ? 0.0f : 1.0f);
	}

	for (i1 = 0; i1 < size; i1++)
	{
		for (i0 = 0; i0 < i1; i0++)
			v[i0] = a[i1][i0] * a[i0][i0];

		v[i1] = a[i1][i1];
		for (i0 = 0; i0 < i1; i0++)
			v[i1] -= a[i1][i0] * v[i0];

		a[i1][i1] = v[i1];
		for (i0 = i1+1; i0 < size; i0++)
		{
			for (int i2 = 0; i2 < i1; i2++)
				a[i0][i1] -= a[i0][i2] * v[i2];
		a[i0][i1] /= v[i1];
		}
	}
	delete [] v;

	for (int col = 0; col < size; col++)
	{
		// forward substitution
		for (i0 = 0; i0 < size; i0++)
		{
			for (i1 = 0; i1 < i0; i1++)
				aInv[i0][col] -= a[i0][i1] * aInv[i1][col];
		}

		// diagonal division
		const float tolerance = LINEAR_SYSTEM_DEFAULT_TOLERANCE;
		for (i0 = 0; i0 < size; i0++)
		{
			if (Math::Abs(a[i0][i0]) <= tolerance)
				return(false);
			aInv[i0][col] /= a[i0][i0];
		}

		// back substitution
		for (i0 = size - 2; i0 >= 0; i0--)
		{
			for (i1 = i0+1; i1 < size; i1++)
				aInv[i0][col] -= a[i1][i0] * aInv[i1][col];
		}
	}

	return(true);
}
