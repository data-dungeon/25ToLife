/***************************************************************************/
/***************************************************************************/

/******************* includes ****************************/

#include "mathUtil/MathUtilPCH.h"





/***************************************************************************/
/***************************************************************************/
void Eigen::Tridiagonal2(
float **mat,
float *diag,
float *subd)
{
	// matrix is already tridiagonal
	diag[0] = mat[0][0];
	diag[1] = mat[1][1];
	subd[0] = mat[0][1];
	subd[1] = 0.0f;
	mat[0][0] = 1.0f;
	mat[0][1] = 0.0f;
	mat[1][0] = 0.0f;
	mat[1][1] = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
void Eigen::Tridiagonal3(
float **mat,
float *diag,
float *subd)
{
	float m00 = mat[0][0];
	float m01 = mat[0][1];
	float m02 = mat[0][2];
	float m11 = mat[1][1];
	float m12 = mat[1][2];
	float m22 = mat[2][2];

	diag[0] = m00;
	subd[2] = 0.0f;
	if (m02 != 0.0f)
	{
		float length = Math::Sqrt(m01 * m01 + m02 * m02);
		float invLength = 1.0f / length;
		m01 *= invLength;
		m02 *= invLength;
		float q = 2.0f * m01 * m12 + m02 * (m22 - m11);
		diag[1] = m11 + m02 * q;
		diag[2] = m22 - m02 * q;
		subd[0] = length;
		subd[1] = m12 - m01 * q;
		mat[0][0] = 1.0f; mat[0][1] = 0.0f;  mat[0][2] = 0.0f;
		mat[1][0] = 0.0f; mat[1][1] = m01; mat[1][2] = m02;
		mat[2][0] = 0.0f; mat[2][1] = m02; mat[2][2] = -m01;
	}
	else
	{
		diag[1] = m11;
		diag[2] = m22;
		subd[0] = m01;
		subd[1] = m12;
		mat[0][0] = 1.0f; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
		mat[1][0] = 0.0f; mat[1][1] = 1.0f; mat[1][2] = 0.0f;
		mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = 1.0f;
	}
}

/***************************************************************************/
/***************************************************************************/
void Eigen::Tridiagonal4(float **mat, float *diag, float *subd)
{
	// save matrix M
	float m00 = mat[0][0];
	float m01 = mat[0][1];
	float m02 = mat[0][2];
	float m03 = mat[0][3];
	float m11 = mat[1][1];
	float m12 = mat[1][2];
	float m13 = mat[1][3];
	float m22 = mat[2][2];
	float m23 = mat[2][3];
	float m33 = mat[3][3];

	diag[0] = m00;
	subd[3] = 0.0f;

	mat[0][0] = 1.0f;
	mat[0][1] = 0.0f;
	mat[0][2] = 0.0f;
	mat[0][3] = 0.0f;
	mat[1][0] = 0.0f;
	mat[2][0] = 0.0f;
	mat[3][0] = 0.0f;

	float length, invLength;
	if (m02 != 0.0f || m03 != 0.0f)
	{
		float q11, q12, q13;
		float q21, q22, q23;
		float q31, q32, q33;

		// build column Q1
		length = Math::Sqrt(m01 * m01 + m02 * m02 + m03 * m03);
		invLength = 1.0f / length;
		q11 = m01 * invLength;
		q21 = m02 * invLength;
		q31 = m03 * invLength;

		subd[0] = length;

		// compute S*Q1
		float v0 = m11 * q11 + m12 * q21 + m13 * q31;
		float v1 = m12 * q11 + m22 * q21 + m23 * q31;
		float v2 = m13 * q11 + m23 * q21 + m33 * q31;

		diag[1] = q11 * v0 + q21 * v1 + q31 * v2;

		// build column Q3 = Q1 x (S * Q1)
		q13 = q21 * v2 - q31 * v1;
		q23 = q31 * v0 - q11 * v2;
		q33 = q11 * v1 - q21 * v0;
		length = Math::Sqrt(q13 * q13 + q23 * q23 + q33 * q33);
		if (length > 0.0f)
		{
			invLength = 1.0f / length;
			q13 *= invLength;
			q23 *= invLength;
			q33 *= invLength;

			// build column Q2 = Q3xQ1
			q12 = q23 * q31 - q33 * q21;
			q22 = q33 * q11 - q13 * q31;
			q32 = q13 * q21 - q23 * q11;

			v0 = q12 * m11 + q22 * m12 + q32 * m13;
			v1 = q12 * m12 + q22 * m22 + q32 * m23;
			v2 = q12 * m13 + q22 * m23 + q32 * m33;
			subd[1] = q11 * v0 + q21 * v1 + q31 * v2;
			diag[2] = q12 * v0 + q22 * v1 + q32 * v2;
			subd[2] = q13 * v0 + q23 * v1 + q33 * v2;

			v0 = q13 * m11 + q23 * m12 + q33 * m13;
			v1 = q13 * m12 + q23 * m22 + q33 * m23;
			v2 = q13 * m13 + q23 * m23 + q33 * m33;
			diag[3] = q13 * v0 + q23 * v1 + q33 * v2;
		}
		else
		{
			// S * Q1 parallel to Q1, choose any valid Q2 and Q3
			subd[1] = 0.0f;

			length = q21 * q21 + q31 * q31;
			if (length > 0.0f)
			{
				invLength = 1.0f / length;
				float tmp = q11 - 1.0f;
				q12 = -q21;
				q22 = 1.0f + tmp * q21 * q21 * invLength;
				q32 = tmp * q21 * q31 * invLength;

				q13 = -q31;
				q23 = q32;
				q33 = 1.0f + tmp * q31 * q31 * invLength;

				v0 = q12 * m11 + q22 * m12 + q32 * m13;
				v1 = q12 * m12 + q22 * m22 + q32 * m23;
				v2 = q12 * m13 + q22 * m23 + q32 * m33;
				diag[2] = q12 * v0 + q22 * v1 + q32 * v2;
				subd[2] = q13 * v0 + q23 * v1 + q33 * v2;

				v0 = q13 * m11 + q23 * m12 + q33 * m13;
				v1 = q13 * m12 + q23 * m22 + q33 * m23;
				v2 = q13 * m13 + q23 * m23 + q33 * m33;
				diag[3] = q13 * v0 + q23 * v1 + q33 * v2;
			}
			else
			{
				// Q1 =( + -1,0,0)
				q12 = 0.0f; q22 = 1.0f; q32 = 0.0f;
				q13 = 0.0f; q23 = 0.0f; q33 = 1.0f;

				diag[2] = m22;
				diag[3] = m33;
				subd[2] = m23;
			}
		}

		mat[1][1] = q11; mat[1][2] = q12; mat[1][3] = q13;
		mat[2][1] = q21; mat[2][2] = q22; mat[2][3] = q23;
		mat[3][1] = q31; mat[3][2] = q32; mat[3][3] = q33;
	}
	else
	{
		diag[1] = m11;
		subd[0] = m01;
		mat[1][1] = 1.0f;
		mat[2][1] = 0.0f;
		mat[3][1] = 0.0f;

		if (m13 != 0.0f)
		{
			length = Math::Sqrt(m12 * m12 + m13 * m13);
			invLength = 1.0f / length;
			m12 *= invLength;
			m13 *= invLength;
			float q = 2.0f * m12 * m23 + m13 * (m33 - m22);

			diag[2] = m22 + m13 * q;
			diag[3] = m33 - m13 * q;
			subd[1] = length;
			subd[2] = m23 - m12 * q;
			mat[1][2] = 0.0f;
			mat[1][3] = 0.0f;
			mat[2][2] = m12;
			mat[2][3] = m13;
			mat[3][2] = m13;
			mat[3][3] = -m12;
		}
		else
		{
			diag[2] = m22;
			diag[3] = m33;
			subd[1] = m12;
			subd[2] = m23;
			mat[1][2] = 0.0f;
			mat[1][3] = 0.0f;
			mat[2][2] = 1.0f;
			mat[2][3] = 0.0f;
			mat[3][2] = 0.0f;
			mat[3][3] = 1.0f;
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void Eigen::TridiagonalN(
int size,
float **mat,
float *diag,
float *subd)
{
	int i0, i1, i2, i3;
	for (i0 = size - 1, i3 = size - 2; i0 >= 1; i0--, i3--)
	{
		float h = 0.0f, scale = 0.0f;

		if (i3 > 0)
		{
			for (i2 = 0; i2 <= i3; i2++)
			scale += Math::Abs(mat[i0][i2]);
			if (scale == 0)
			{
				subd[i0] = mat[i0][i3];
			}
			else
			{
				float invScale = 1.0f / scale;
				for (i2 = 0; i2 <= i3; i2++)
				{
					mat[i0][i2] *= invScale;
					h += mat[i0][i2] * mat[i0][i2];
				}

				float f = mat[i0][i3];
				float g = Math::Sqrt(h);
				if (f > 0.0f)
					g = -g;
				subd[i0] = scale * g;
				h -= f * g;
				mat[i0][i3] = f - g;
				f = 0.0f;

				float invH = 1.0f / h;
				for (i1 = 0; i1 <= i3; i1++)
				{
					mat[i1][i0] = mat[i0][i1] * invH;
					g = 0.0f;
					for (i2 = 0; i2 <= i1; i2++)
						g += mat[i1][i2] * mat[i0][i2];
					for (i2 = i1 + 1; i2 <= i3; i2++)
						g += mat[i2][i1] * mat[i0][i2];
					subd[i1] = g * invH;
					f += subd[i1] * mat[i0][i1];
				}

				float halfFdivH = 0.5f * f * invH;
				for (i1 = 0; i1 <= i3; i1++)
				{
					f = mat[i0][i1];
					g = subd[i1] - halfFdivH * f;
					subd[i1] = g;
					for (i2 = 0; i2 <= i1; i2++)
					{
						mat[i1][i2] -= f*subd[i2] + g * mat[i0][i2];
					}
				}
			}
		}
		else
		{
			subd[i0] = mat[i0][i3];
		}

		diag[i0] = h;
	}

	diag[0] = subd[0] = 0.0f;
	for (i0 = 0, i3 = -1; i0 <= size - 1; i0++, i3++)
	{
		if (diag[i0] != 0.0f)
		{
			for (i1 = 0; i1 <= i3; i1++)
			{
				float sum = 0;
				for (i2 = 0; i2 <= i3; i2++)
					sum += mat[i0][i2] * mat[i2][i1];
				for (i2 = 0; i2 <= i3; i2++)
					mat[i2][i1] -= sum * mat[i2][i0];
			}
		}

		diag[i0] = mat[i0][i0];
		mat[i0][i0] = 1.0f;
		for (i1 = 0; i1 <= i3; i1++)
			mat[i1][i0] = mat[i0][i1] = 0.0f;
	}

	// re-ordering if Eigen::QLAlgorithm is used subsequently
	for (i0 = 1, i3 = 0; i0 < size; i0++, i3++)
		subd[i3] = subd[i0];
	subd[size - 1] = 0.0f;
}

/***************************************************************************/
/***************************************************************************/
bool Eigen::QLAlgorithm(
int size,
float *diag,
float *subd,
float **mat)
{
	const int maxIter = 32;

	for (int i0 = 0; i0 < size; i0++)
	{
		int i1;
		for (i1 = 0; i1 < maxIter; i1++)
		{
			int i2;
			for (i2 = i0; i2 <= size-2; i2++)
			{
				float tmp = Math::Abs(diag[i2]) + Math::Abs(diag[i2 + 1]);
				if (Math::Abs(subd[i2]) + tmp == tmp)
					break;
			}

			if (i2 == i0)
				break;

			float g =(diag[i0 + 1]-diag[i0]) / (2.0f * subd[i0]);
			float r = Math::Sqrt(g * g + 1.0f);
			if (g < 0.0f)
				g = diag[i2] - diag[i0] + subd[i0] / (g - r);
			else
				g = diag[i2] - diag[i0] + subd[i0] / (g + r);

			float sine = 1.0f, cosine = 1.0f, p = 0.0f;
			for (int i3 = i2-1; i3 >= i0; i3--)
			{
				float f = sine * subd[i3];
				float b = cosine * subd[i3];
				if (Math::Abs(f) >= Math::Abs(g))
				{
					cosine = g / f;
					r = Math::Sqrt(cosine * cosine + 1.0f);
					subd[i3 + 1] = f * r;
					sine = 1.0f / r;
					cosine *= sine;
				}
				else
				{
					sine = f / g;
					r = Math::Sqrt(sine * sine + 1.0f);
					subd[i3 + 1] = g * r;
					cosine = 1.0f / r;
					sine *= cosine;
				}

				g = diag[i3 + 1] - p;
				r = (diag[i3] - g) * sine + 2.0f * b * cosine;
				p = sine * r;
				diag[i3 + 1] = g + p;
				g = cosine * r - b;

				for (int i4 = 0; i4 < size; i4++)
				{
					f = mat[i4][i3 + 1];
					mat[i4][i3 + 1] = sine * mat[i4][i3] + cosine * f;
					mat[i4][i3] = cosine * mat[i4][i3] - sine * f;
				}
			}

			diag[i0] -= p;
			subd[i0] = g;
			subd[i2] = 0.0f;
		}

		if (i1 == maxIter)
			return(false);
	}

	return(true);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::DecreasingSort(
int size,
float *eigval,
float **eigvec)
{
	// sort eigenvalues in decreasing order, e[0] >= ... >= e[size-1]
	for (int i0 = 0, i1; i0 <= size - 2; i0++)
	{
		// locate maximum eigenvalue
		i1 = i0;
		float max = eigval[i1];
		int i2;
		for (i2 = i0 + 1; i2 < size; i2++)
		{
			if (eigval[i2] > max)
			{
				i1 = i2;
				max = eigval[i1];
			}
		}

		if (i1 != i0)
		{
			// swap eigenvalues
			eigval[i1] = eigval[i0];
			eigval[i0] = max;

			// swap eigenvectors
			for (i2 = 0; i2 < size; i2++)
			{
				float tmp = eigvec[i2][i0];
				eigvec[i2][i0] = eigvec[i2][i1];
				eigvec[i2][i1] = tmp;
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void Eigen::IncreasingSort(
int size,
float *eigval,
float **eigvec)
{
	// sort eigenvalues in increasing order, e[0] <= ... <= e[size-1]
	for (int i0 = 0, i1; i0 <= size - 2; i0++)
	{
		// locate minimum eigenvalue
		i1 = i0;
		float min = eigval[i1];
		int i2;
		for (i2 = i0 + 1; i2 < size; i2++)
		{
			if (eigval[i2] < min)
			{
				i1 = i2;
				min = eigval[i1];
			}
		}

		if (i1 != i0)
		{
			// swap eigenvalues
			eigval[i1] = eigval[i0];
			eigval[i0] = min;

			// swap eigenvectors
			for (i2 = 0; i2 < size; i2++)
			{
				float tmp = eigvec[i2][i0];
				eigvec[i2][i0] = eigvec[i2][i1];
				eigvec[i2][i1] = tmp;
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void Eigen::SetMatrix(float **mat)
{
	for (int row = 0; row < m_size; row++)
	{
		for (int col = 0; col < m_size; col++)
			m_mat[row][col] = mat[row][col];
	}
}

/***************************************************************************/
/***************************************************************************/
void Eigen::EigenStuff2(void)
{
	Tridiagonal2(m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::EigenStuff3(void)
{
	Tridiagonal3(m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::EigenStuff4(void)
{
	Tridiagonal4(m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::EigenStuffN(void)
{
	TridiagonalN(m_size, m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::EigenStuff(void)
{
	switch( m_size )
	{
		case 2:
			Tridiagonal2(m_mat, m_diag, m_subd);
			break;
		case 3:
			Tridiagonal3(m_mat, m_diag, m_subd);
			break;
		case 4:
			Tridiagonal4(m_mat, m_diag, m_subd);
			break;
		default:
			TridiagonalN(m_size, m_mat, m_diag, m_subd);
			break;
	}

	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::DecrSortEigenStuff2(void)
{
	Tridiagonal2(m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	DecreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::DecrSortEigenStuff3(void)
{
	Tridiagonal3(m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	DecreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::DecrSortEigenStuff4(void)
{
	Tridiagonal4(m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	DecreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::DecrSortEigenStuffN(void)
{
	TridiagonalN(m_size, m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	DecreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::DecrSortEigenStuff(void)
{
	switch( m_size )
	{
		case 2:
			Tridiagonal2(m_mat, m_diag, m_subd);
			break;
		case 3:
			Tridiagonal3(m_mat, m_diag, m_subd);
			break;
		case 4:
			Tridiagonal4(m_mat, m_diag, m_subd);
			break;
		default:
			TridiagonalN(m_size, m_mat, m_diag, m_subd);
			break;
	}

	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	DecreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::IncrSortEigenStuff2(void)
{
	Tridiagonal2(m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	IncreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::IncrSortEigenStuff3(void)
{
	Tridiagonal3(m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	IncreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::IncrSortEigenStuff4(void)
{
	Tridiagonal4(m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	IncreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::IncrSortEigenStuffN(void)
{
	TridiagonalN(m_size, m_mat, m_diag, m_subd);
	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	IncreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
void Eigen::IncrSortEigenStuff(void)
{
	switch( m_size )
	{
		case 2:
			Tridiagonal2(m_mat, m_diag, m_subd);
			break;
		case 3:
			Tridiagonal3(m_mat, m_diag, m_subd);
			break;
		case 4:
			Tridiagonal4(m_mat, m_diag, m_subd);
			break;
		default:
			TridiagonalN(m_size, m_mat, m_diag, m_subd);
			break;
	}

	QLAlgorithm(m_size, m_diag, m_subd, m_mat);
	IncreasingSort(m_size, m_diag, m_mat);
}

/***************************************************************************/
/***************************************************************************/
Eigen3::Eigen3(void)
{
	m_size = 3;
	m_mat = m_matptrdata;
	m_mat[0] = &m_matdata[0];
	m_mat[1] = &m_matdata[3];
	m_mat[2] = &m_matdata[6];
	m_diag = m_diagdata;
	m_subd = m_subddata;
}

/***************************************************************************/
/***************************************************************************/
EigenN::EigenN(int size)
{
	ASSERT(size >= 2);

	m_size = size;

	m_mat = new float *[m_size];
	for (int i = 0; i < m_size; i++)
		m_mat[i] = new float[m_size];

	m_diag = new float[m_size];
	m_subd = new float[m_size];
}

/***************************************************************************/
/***************************************************************************/
EigenN::~EigenN(void)
{
	delete [] m_subd;
	delete [] m_diag;
	for (int i = 0; i < m_size; i++)
		delete [] m_mat[i];
	delete [] m_mat;
}

