/* --------------------------------------------------
 * File    : QUATERN.C
 * Created : Thursday February 11 1998 10:25:00
 * Descript: Floating Point Quaternion Library
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <m_trigfunc.h>
#include <m_macros.h>

//#include <mathplat.h>
//#include <libgte.h>
//#include <gtemac.h>

#include <lvector.h>

#include <quatern.h>

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
m_bool qatNormalize(ts_Quatvw *pQuat)
{
	float	magnitude;
	float	w,x,y,z;

	w = pQuat->W;
	x = pQuat->V[0];
	y = pQuat->V[1];
	z = pQuat->V[2];
	magnitude = (float) sqrt((w * w) + (x * x) + (y * y) + (z * z));

	if (magnitude < 0.001 && magnitude > -0.001)
	{
		return FALSE;
	}
	else
	{
		pQuat->W    = w / magnitude;
		pQuat->V[0] = x / magnitude;
		pQuat->V[1] = y / magnitude;
		pQuat->V[2] = z / magnitude;
	}

	return TRUE;
}





void qatFromMat(l_Matrix *pMat, ts_Quatvw *pQuat)
{
	m_bool quatNorm = qatNormalize(pQuat);

	if (quatNorm == FALSE) // maybe add error handling??
	{
		return;
	}
	else
	{

		float  trace, s, q[4];

		int    i, j, k;
		int next[3] = {1, 2, 0};

		trace = (*pMat)[0][0] + (*pMat)[1][1] + (*pMat)[2][2];  // check the diagonal

		 if (trace > 0.0f)
		{
			s = (float) sqrt(trace + 1.0f); // Do we want a sqrt macro function??
			pQuat->W = s / 2.0f;
			s = 0.5f / s;

			// Row major format!!!!!
			pQuat->V[0] = ((*pMat)[2][1] - (*pMat)[1][2]) * s;
			pQuat->V[1] = ((*pMat)[0][2] - (*pMat)[2][0]) * s;
			pQuat->V[2] = ((*pMat)[1][0] - (*pMat)[0][1]) * s;
		}
		else
		{			 // diagonal is negative
    		i = 0;
			  if ((*pMat)[1][1] > (*pMat)[0][0])
			{
				i = 1;
			}
			 if ((*pMat)[2][2] > (*pMat)[i][i])
			{
				i = 2;
			}

			j = next[i];
			k = next[j];
			s = (float) sqrt((*pMat)[i][i] - (*pMat)[j][j] - (*pMat)[k][k] + 1.0f);
			q[i] = s * 0.5f;

			if (s != 0.0f)
			{
				s = 0.5f / s;
			}

			q[3] = ((*pMat)[j][k] - (*pMat)[k][j]) * s;
			q[j] = ((*pMat)[i][j] - (*pMat)[j][i]) * s;
			q[k] = ((*pMat)[i][k] - (*pMat)[k][i]) * s;

			pQuat->V[0] = q[0];
			pQuat->V[1] = q[1];
			pQuat->V[2] = q[2];
			pQuat->W = q[3];
		}
	}
}

void qatToRotMat(ts_Quatvw *pQuat, l_Matrix *pMat)
{
	m_bool quatNorm = qatNormalize(pQuat);

	if (quatNorm == FALSE) // maybe add error handling??
	{
		return;
	}
	else
	{

		float twoXW, twoYW, twoZW, twoXX, twoYY, twoZZ, twoYZ, twoXY, twoXZ, twoX, twoY, twoZ;

		// calculate coefficients
		twoX = pQuat->V[0] + pQuat->V[0];
		twoY = pQuat->V[1] + pQuat->V[1];
		twoZ = pQuat->V[2] + pQuat->V[2];
		twoXX = pQuat->V[0] * twoX;
		twoXY = pQuat->V[0] * twoY;
		twoXZ = pQuat->V[0] * twoZ;
		twoYY = pQuat->V[1] * twoY;
		twoYZ = pQuat->V[1] * twoZ;
		twoZZ = pQuat->V[2] * twoZ;
		twoXW = pQuat->W * twoX;
		twoYW = pQuat->W * twoY;
		twoZW = pQuat->W * twoZ;

		// Row major format
		(*pMat)[0][0] = 1.0f - (twoYY + twoZZ);
		(*pMat)[1][0] = twoXY - twoZW;
		(*pMat)[2][0] = twoXZ + twoYW;
		(*pMat)[3][0] = 0.0f;
		(*pMat)[0][1] = twoXY + twoZW;
		(*pMat)[1][1] = 1.0f - (twoXX + twoZZ);
		(*pMat)[2][1] = twoYZ - twoXW;
		(*pMat)[3][1] = 0.0f;
		(*pMat)[0][2] = twoXZ - twoYW;
		(*pMat)[1][2] = twoYZ + twoXW;
		(*pMat)[2][2] = 1.0f - (twoXX + twoYY);
		(*pMat)[3][2] = 0.0f;
		(*pMat)[0][3] = 0.0f;
		(*pMat)[1][3] = 0.0f;
		(*pMat)[2][3] = 0.0f;
		(*pMat)[3][3] = 1.0f;
	}
}

void qatFromEuler(float roll, float pitch, float yaw, ts_Quatvw *pQuat)
{
	float halfRoll, halfPitch, halfYaw, cr, cp, cy, sr, sp, sy, cpcy, spsy;

	// calculate trig identities
	halfRoll = roll / 2.0f;
	halfPitch = pitch / 2.0f;
	halfYaw = yaw / 2.0f;

	cr = mthCos(D_R(halfRoll));
	cp = mthCos(D_R(halfPitch));
	cy = mthCos(D_R(halfYaw));

	sr = mthSin(D_R(halfRoll));
	sp = mthSin(D_R(halfPitch));
	sy = mthSin(D_R(halfYaw));

	cpcy = cp * cy;
	spsy = sp * sy;

	pQuat->W = (cr * cpcy) + (sr * spsy);
	pQuat->V[0] = (sr * cpcy) - (cr * spsy);
	pQuat->V[1] = (cr * sp * cy) + (sr * cp * sy);
	pQuat->V[2] = (cr * cp * sy) - (sr * sp * cy);
}

void qatFromAxisAngle(l_Vct3f *pAxis, t_angle angle, ts_Quatvw *pQuat)
{
	float halfAngle, sinAngle, cosAngle;

	halfAngle = angle / 2.0f;

	sinAngle = mthSin(D_R(halfAngle));
	cosAngle = mthCos(D_R(halfAngle));

	pQuat->V[0] = (*pAxis)[0] * sinAngle;
	pQuat->V[1] = (*pAxis)[1] * sinAngle;
	pQuat->V[2] = (*pAxis)[2] * sinAngle;
	pQuat->W = cosAngle;
}

void qatAdd(ts_Quatvw *pA, ts_Quatvw *pB, ts_Quatvw *pResult)
{
	pResult->V[0] = pA->V[0] + pB->V[0];
	pResult->V[1] = pA->V[1] + pB->V[1];
	pResult->V[2] = pA->V[2] + pB->V[2];
	pResult->W    = pA->W    + pB->W;
}

void qatSub(ts_Quatvw *pA, ts_Quatvw *pB, ts_Quatvw *pResult)
{
	pResult->V[0] = pA->V[0] - pB->V[0];
	pResult->V[1] = pA->V[1] - pB->V[1];
	pResult->V[2] = pA->V[2] - pB->V[2];
	pResult->W    = pA->W    - pB->W;
}

void qatMul(ts_Quatvw *pA, ts_Quatvw *pB, ts_Quatvw *pResult)
{
	float A, B, C, D, E, F, G, H;

	A = (pA->W    + pA->V[0]) * (pB->W    + pB->V[0]);
	B = (pA->V[2] - pA->V[1]) * (pB->V[1] - pB->V[2]);
	C = (pA->V[0] - pA->W)    * (pB->V[1] - pB->V[2]);
	D = (pA->V[1] + pA->V[2]) * (pB->V[0] - pB->W);
	E = (pA->V[0] + pA->V[2]) * (pB->V[0] + pB->V[1]);
	F = (pA->V[0] - pA->V[2]) * (pB->V[0] - pB->V[1]);
	G = (pA->W    + pA->V[1]) * (pB->W    - pB->V[2]);
	H = (pA->W    - pA->V[1]) * (pB->W    + pB->V[2]);

	pResult->W    = B + ((-E - F + G + H) / 2.0f);
	pResult->V[0] = A - ((E + F + G + H) / 2.0f);
	pResult->V[1] = ((E - F + G - H) / 2.0f) - C;
	pResult->V[2] = ((E - F - G + H) / 2.0f) - D;
}

void qatSlerp(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult)
{
	m_bool quatFromNorm = qatNormalize(pFrom);
	m_bool quatToNorm = qatNormalize(pTo);

	if (quatFromNorm == FALSE || quatToNorm == FALSE) // maybe add error handling??
	{
		return;
	}
	else
	{
		float delta = 0.001f;
		float to1[4];
		float omega, cosom, sinom, scale0, scale1;

		// calc cosine
		cosom = vctDot3f(&pFrom->V, &pTo->V);

		// adjust signs (if necessary)
		if ( cosom < 0.0 )
		{
			cosom = - cosom;
			to1[0] = - pTo->V[0];
			to1[1] = - pTo->V[1];
			to1[2] = - pTo->V[2];
			to1[3] = - pTo->W;
		}
		else
		{
			to1[0] = pTo->V[0];
			to1[1] = pTo->V[1];
			to1[2] = pTo->V[2];
			to1[3] = pTo->W;
		}

		// calculate coefficients

		if (1.0f - cosom > delta )
		{
			// standard case (slerp)
			omega = (float) mthACos(D_R(cosom));
			sinom = (float) mthSin(D_R(omega));
			scale0 = (float) mthSin(D_R((1.0f - t) * omega)) / sinom);
			scale1 = (float) mthSin(D_R(t * omega)) / sinom);
		}
		else
		{
			// "from" and "to" quaternions are very close
			//  ... so we can do a linear interpolation
			scale0 = 1.0f - t;
			scale1 = t;
		}

		// calculate final values
		pResult->V[0] = (scale0 * pFrom->V[0]) + (scale1 * to1[0]);
		pResult->V[1] = (scale0 * pFrom->V[1]) + (scale1 * to1[1]);
		pResult->V[2] = (scale0 * pFrom->V[2]) + (scale1 * to1[2]);
		pResult->W    = (scale0 * pFrom->W)    + (scale1 * to1[3]);
	}
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
