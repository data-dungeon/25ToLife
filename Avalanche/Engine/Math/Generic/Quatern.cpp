/* --------------------------------------------------
 * File    : QUATERN.C
 * Created : Thursday February 11 1998 10:25:00
 * Descript: Floating Point Quaternion Library
 * --------------------------------------------------*/

/******************* includes ****************************/

#include "Math/MathPCH.h"





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
void qatIdentity(ts_Quatvw *pQuat)
{
	pQuat->V.Clear();
	pQuat->W = 1.0f;
}

#ifndef EXCLUDE_GENERIC_QUATERNION_NORMALIZE
bool qatNormalize(ts_Quatvw *pQuat)
{
	float	fMagnitude;

	fMagnitude = Math::Sqrt(pQuat->V.LengthSquared() + (pQuat->W * pQuat->W));
	if ((fMagnitude < 0.001f) && (fMagnitude > -0.001f))
		return(FALSE);

	fMagnitude = Math::Inverse(fMagnitude);
	pQuat->V *= fMagnitude;
	pQuat->W   *= fMagnitude;
	return(TRUE);
}
#endif

#define NEW_ALGORITHM
#ifndef NEW_ALGORITHM
//I think this is from Game Developer Magazine...
void qatFromMat(const Graphics4x4 *pMatrix, ts_Quatvw *pQuat)
{
	bool quatNorm = qatNormalize(pQuat);

	if (quatNorm == FALSE) // maybe add error handling??
	{
		ASSERT(FALSE);
		return;
	}
	else
	{
		float  trace, s, q[4];
		int    i;

		trace = pMatrix->r00 + pMatrix->r11 + pMatrix->r22;  // check the diagonal

		if (trace > 0.0f)
		{
			s = mthSQRT(trace + 1.0f);
			pQuat->W = s / 2.0f;
			s = 0.5f / s;

			pQuat->V.x((pMatrix->r21 - pMatrix->r12) * s);
			pQuat->V.y((pMatrix->r02 - pMatrix->r20) * s);
			pQuat->V.z((pMatrix->r10 - pMatrix->r01) * s);
		}
		else
		{
			// diagonal is negative
    		i = 0;
			if (pMatrix->r11 > pMatrix->r00)
				i = 1;
			if ((pMatrix->r22 > pMatrix->r00) && (pMatrix->r22 > pMatrix->r11))
				i = 2;

			switch(i)
			{
			case 0:
				s = mthSQRT(pMatrix->r00 - pMatrix->r11 - pMatrix->r22 + 1.0f);
				break;
			case 1:
				s = mthSQRT(pMatrix->r11 - pMatrix->r22 - pMatrix->r00 + 1.0f);
				break;
			case 2:
				s = mthSQRT(pMatrix->r22 - pMatrix->r00 - pMatrix->r11 + 1.0f);
				break;
			default:
				ASSERT(FALSE);
			}
			q[i] = s * 0.5f;

			if (s != 0.0f)
				s = 0.5f / s;

xxx	//i am putting this here so that the compile will break if this routine is reactivated
//yuch-- this routine is currently dead. if it ever comes back alive, compare it to the other qatFromMat routine
// below, which use '+' instead of '-' for the first two equations in each case statement. this code also
// appears to be COLUMN major for the first part (trace > 0.0)
			switch(i)
			{
			case 0:
				q[1] = (pMatrix->r01 - pMatrix->r10) * s;
				q[2] = (pMatrix->r02 - pMatrix->r20) * s;
				q[3] = (pMatrix->r12 - pMatrix->r21) * s;
				break;
			case 1:
				q[0] = (pMatrix->r10 - pMatrix->r01) * s;
				q[2] = (pMatrix->r12 - pMatrix->r21) * s;
				q[3] = (pMatrix->r20 - pMatrix->r02) * s;
				break;
			case 2:
				q[0] = (pMatrix->r20 - pMatrix->r02) * s;
				q[1] = (pMatrix->r21 - pMatrix->r12) * s;
				q[3] = (pMatrix->r01 - pMatrix->r10) * s;
				break;
			}

			pQuat->V.x = q[0];
			pQuat->V.y = q[1];
			pQuat->V.z = q[2];
			pQuat->W =   q[3];
		}
	}
}

void qatToRotMat(const ts_Quatvw *pQuat, Cartesian4x4 *pMatrix)
{
	bool quatNorm = qatNormalize(pQuat);

	if (quatNorm == FALSE) // maybe add error handling??
	{
		ASSERT( 0 );
		return;
	}
	else
	{
		float twoXW, twoYW, twoZW, twoXX, twoYY, twoZZ, twoYZ, twoXY, twoXZ, twoX, twoY, twoZ;

		// calculate coefficients
		twoX  = pQuat->V.x() + pQuat->V.x();
		twoY  = pQuat->V.y() + pQuat->V.y();
		twoZ  = pQuat->V.z() + pQuat->V.z();
		twoXX = pQuat->V.x() * twoX;
		twoXY = pQuat->V.x() * twoY;
		twoXZ = pQuat->V.x() * twoZ;
		twoYY = pQuat->V.y() * twoY;
		twoYZ = pQuat->V.y() * twoZ;
		twoZZ = pQuat->V.z() * twoZ;
		twoXW = pQuat->W   * twoX;
		twoYW = pQuat->W   * twoY;
		twoZW = pQuat->W   * twoZ;

		// fill in matrix
		pMatrix->r00 = 1.0f - (twoYY + twoZZ);
		pMatrix->r10 = twoXY - twoZW;
		pMatrix->r20 = twoXZ + twoYW;
		pMatrix->tx  = 0.0f;
		pMatrix->r01 = twoXY + twoZW;
		pMatrix->r11 = 1.0f - (twoXX + twoZZ);
		pMatrix->r21 = twoYZ - twoXW;
		pMatrix->ty  = 0.0f;
		pMatrix->r02 = twoXZ - twoYW;
		pMatrix->r12 = twoYZ + twoXW;
		pMatrix->r22 = 1.0f - (twoXX + twoYY);
		pMatrix->tz  = 0.0f;
		pMatrix->u0  = 0.0f;
		pMatrix->u1  = 0.0f;
		pMatrix->u2  = 0.0f;
		pMatrix->w   = 1.0f;
	}
}

void qatSlerp(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult)
{
	bool quatFromNorm = qatNormalize(pFrom);
	bool quatToNorm = qatNormalize(pTo);

	if (quatFromNorm == FALSE || quatToNorm == FALSE) // maybe add error handling??
	{
		ASSERT( 0 );
		return;
	}
	else
	{
		float delta = 0.001f;
		float to1[4];
		float omega, cosom, sinom, scale0, scale1;

		// calc cosine
		cosom = (pFrom->V.x * pTo->V.x) + (pFrom->V.y * pTo->V.y) + (pFrom->V.z * pTo->V.z) + (pFrom->W * pTo->W);

		// adjust signs (if necessary)
		if ( cosom < 0.0f )
		{
			cosom = - cosom;
			to1[0] = - pTo->V.x();
			to1[1] = - pTo->V.y();
			to1[2] = - pTo->V.z();
			to1[3] = - pTo->W;
		}
		else
		{
			to1[0] = pTo->V.x();
			to1[1] = pTo->V.y();
			to1[2] = pTo->V.z();
			to1[3] = pTo->W;
		}

		// calculate coefficients

		if (1.0f - cosom > delta )
		{
			// standard case (slerp)
			omega = (float) mthACOS(D_R(cosom));
			sinom = (float) Math::SinDeg(omega);
		  	scale0 = (float) Math::SinDeg(((1.0f - t) * omega)) / sinom;
		   scale1 = (float) Math::SinDeg(t * omega) / sinom;
		}
		else
		{
			// "from" and "to" quaternions are very close
			//  ... so we can do a linear interpolation
			scale0 = 1.0f - t;
			scale1 = t;
		}

		// calculate final values
		pResult->V.x((scale0 * pFrom->V.x) + (scale1 * to1[0]));
		pResult->V.y((scale0 * pFrom->V.y) + (scale1 * to1[1]));
		pResult->V.z((scale0 * pFrom->V.z) + (scale1 * to1[2]));
		pResult->W   = (scale0 * pFrom->W)   + (scale1 * to1[3]);
	}
}
#endif //NEW_ALGORITHM

void qatFromEuler(float roll, float pitch, float yaw, ts_Quatvw *pQuat)
{
	float halfRoll, halfPitch, halfYaw, cr, cp, cy, sr, sp, sy, cpcy, spsy;

	// calculate trig identities
	halfRoll = roll / 2.0f;
	halfPitch = pitch / 2.0f;
	halfYaw = yaw / 2.0f;

	cr = Math::CosDeg(halfRoll);
	cp = Math::CosDeg(halfPitch);
	cy = Math::CosDeg(halfYaw);

	sr = Math::SinDeg(halfRoll);
	sp = Math::SinDeg(halfPitch);
	sy = Math::SinDeg(halfYaw);

	cpcy = cp * cy;
	spsy = sp * sy;

	pQuat->V.x( (sr * cpcy) - (cr * spsy));
	pQuat->V.y( (cr * sp * cy) + (sr * cp * sy));
	pQuat->V.z( (cr * cp * sy) - (sr * sp * cy));
	pQuat->W   = (cr * cpcy) + (sr * spsy);
}

void qatFromAxisAngle(const Vector3 *pAxis, float angle, ts_Quatvw *pQuat)
{
	float halfAngle, sinAngle, cosAngle;

	halfAngle = angle / 2.0f;

	sinAngle = Math::SinDeg(halfAngle);
	cosAngle = Math::CosDeg(halfAngle);

	pQuat->V = *pAxis * sinAngle;
	pQuat->W = cosAngle;
}

#ifndef EXCLUDE_GENERIC_QUATERNION_ADD
void qatAdd(const ts_Quatvw *pA, const ts_Quatvw *pB, ts_Quatvw *pResult)
{
	pResult->V = pA->V + pB->V;
	pResult->W   = pA->W   + pB->W;
}
#endif

#ifndef EXCLUDE_GENERIC_QUATERNION_SUBTRACT
void qatSub(const ts_Quatvw *pA, const ts_Quatvw *pB, ts_Quatvw *pResult)
{
	pResult->V = pA->V - pB->V;
	pResult->W   = pA->W   - pB->W;
}
#endif

#ifndef EXCLUDE_GENERIC_QUATERNION_MULTIPLY
void qatMul(const ts_Quatvw *pA, const ts_Quatvw *pB, ts_Quatvw *pResult)
{
	float A, B, C, D, E, F, G, H;

	A = (pA->W     + pA->V.x()) * (pB->W     + pB->V.x());
	B = (pA->V.z() - pA->V.y()) * (pB->V.y() - pB->V.z());
	C = (pA->V.x() - pA->W    ) * (pB->V.y() + pB->V.z());
	D = (pA->V.y() + pA->V.z()) * (pB->V.x() - pB->W);
	E = (pA->V.x() + pA->V.z()) * (pB->V.x() + pB->V.y());
	F = (pA->V.x() - pA->V.z()) * (pB->V.x() - pB->V.y());
	G = (pA->W     + pA->V.y()) * (pB->W     - pB->V.z());
	H = (pA->W     - pA->V.y()) * (pB->W     + pB->V.z());

	pResult->V.Set(A - ((E + F + G + H) / 2.0f),
						((E - F + G - H) / 2.0f) - C,
						((E - F - G + H) / 2.0f) - D);
	pResult->W   = B + ((-E - F + G + H) / 2.0f);
}
#endif

void qatVectMul( const ts_Quatvw *pA, const Vector3 *pB, Vector3 *pResult)
{
	Vector3 d;

	d = pA->V.Cross( *pB );

	*pResult = *pB + ( 2.0f * ( ( pA->W * d ) + pA->V.Cross( d ) ) );
}

void qatLerpBetter(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult)
{
	ts_Quatvw qTemp;
	float mag1, mag2;

	//accuracy enhancement described on page 365
#if 0 // TJC - vectorized
	mag1 = ((pFrom->V.x() + pTo->V.x()) * (pFrom->V.x() + pTo->V.x())) +
			 ((pFrom->V.y() + pTo->V.y()) * (pFrom->V.y() + pTo->V.y())) +
			 ((pFrom->V.z() + pTo->V.z()) * (pFrom->V.z() + pTo->V.z())) +
			 ((pFrom->W     + pTo->W    ) * (pFrom->W     + pTo->W    ));
	mag2 = ((pFrom->V.x() - pTo->V.x()) * (pFrom->V.x() - pTo->V.x())) +
			 ((pFrom->V.y() - pTo->V.y()) * (pFrom->V.y() - pTo->V.y())) +
			 ((pFrom->V.z() - pTo->V.z()) * (pFrom->V.z() - pTo->V.z())) +
			 ((pFrom->W     - pTo->W    ) * (pFrom->W     - pTo->W    ));
#else
	mag1 = (pFrom->V + pTo->V).LengthSquared() +
			 ((pFrom->W     + pTo->W    ) * (pFrom->W     + pTo->W    ));
	mag2 = (pFrom->V - pTo->V).LengthSquared() +
			 ((pFrom->W     - pTo->W    ) * (pFrom->W     - pTo->W    ));
#endif

	if (mag1 > mag2)
		qatLerp(pFrom, pTo, t, pResult);
	else
	{
		qTemp.V = -pTo->V;
		qTemp.W = -pTo->W  ;
		qatLerp(pFrom, &qTemp, t, pResult);
	}
}

void qatLerp(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult)
{
	bool quatFromNorm = qatNormalize(pFrom);
	bool quatToNorm = qatNormalize(pTo);

	if (quatFromNorm == FALSE || quatToNorm == FALSE) // maybe add error handling??
	{
		ASSERT( 0 );
		return;
	}
	else
	{
//		float delta = 0.001f;
//		float to1[4];
		Vector3 to1;
		float to1w;
		float cosom, scale0, scale1;

		// calc cosine
#if 0 // TJC - vectorized
		cosom = (pFrom->V.x() * pTo->V.x()) + (pFrom->V.y() * pTo->V.y()) + (pFrom->V.z() * pTo->V.z()) + (pFrom->W * pTo->W);
#else
		cosom = pFrom->V.Dot(pTo->V) + (pFrom->W * pTo->W);
#endif

		// adjust signs (if necessary)
		if ( cosom < 0.0f )
		{
			cosom = - cosom;
			to1  = - pTo->V;
			to1w = - pTo->W;
		}
		else
		{
			to1  = pTo->V;
			to1w = pTo->W;
		}

		// "from" and "to" quaternions are very close
		//  ... so we can do a linear interpolation
		scale0 = 1.0f - t;
		scale1 = t;

		// calculate final values
#if 0 // TJC - vectorized
		pResult->V.Set((scale0 * pFrom->V.x()) + (scale1 * to1[0]),
							(scale0 * pFrom->V.y()) + (scale1 * to1[1]),
							(scale0 * pFrom->V.z()) + (scale1 * to1[2]));
#else
		pResult->V = pFrom->V * scale0 + to1 * scale1;
#endif
		pResult->W   = (scale0 * pFrom->W)   + (scale1 * to1w);
	}
    return;
}

#ifdef NEW_ALGORITHM
//try the implemntation from Advanced Animation and Rendering Techniques, by Alan and Mark Watt
//at least it has no error returns, that is, it always return something.  The some of the above routines will do an error return.
#define EPSILON 0.00001f

#ifndef EXCLUDE_GENERIC_QUATERNION_FROM_MATRIX
void qatFromMat(const Graphics4x4 *pMatrix, ts_Quatvw *pQuat)
{
//djr	floadouble tr,s;
	float tr,s;//djr: it looks like these don't really need to be doubles
	int i;

	tr = pMatrix->r00 + pMatrix->r11 + pMatrix->r22;
	if (tr > 0.0f)
	{
		s = Math::Sqrt(tr + 1.0f);
		pQuat->W = s * 0.5f;
		s = 0.5f / s;

		pQuat->V.Set((pMatrix->r12 - pMatrix->r21) * s,
						 (pMatrix->r20 - pMatrix->r02) * s,
						 (pMatrix->r01 - pMatrix->r10) * s);
	}
	else
	{
		/* find largest value on diagonal */

		i = 0;
		if (pMatrix->r11 > pMatrix->r00)
			i = 1;
		if ((pMatrix->r22 > pMatrix->r00) && (pMatrix->r22 > pMatrix->r11))
			i = 2;

		switch(i)
		{
		case 0:
			s = Math::Sqrt(pMatrix->r00 - pMatrix->r11 - pMatrix->r22 + 1.0f);
			pQuat->V.x( s * 0.5f);
			ASSERT(s != 0.0f);
			s = 0.5f / s;
			pQuat->V.y( (pMatrix->r01 + pMatrix->r10) * s);
			pQuat->V.z( (pMatrix->r02 + pMatrix->r20) * s);
			pQuat->W   = (pMatrix->r12 - pMatrix->r21) * s;
			break;
		case 1:
			s = Math::Sqrt(pMatrix->r11 - pMatrix->r22 - pMatrix->r00 + 1.0f);
			pQuat->V.y( s * 0.5f);
			ASSERT(s != 0.0f);
			s = 0.5f / s;
			pQuat->V.x( (pMatrix->r10 + pMatrix->r01) * s);
			pQuat->V.z( (pMatrix->r12 + pMatrix->r21) * s);
			pQuat->W   = (pMatrix->r20 - pMatrix->r02) * s;
			break;
		case 2:
			s = Math::Sqrt(pMatrix->r22 - pMatrix->r00 - pMatrix->r11 + 1.0f);
			pQuat->V.z( s * 0.5f);
			ASSERT(s != 0.0f);
			s = 0.5f / s;
			pQuat->V.x( (pMatrix->r20 + pMatrix->r02) * s);
			pQuat->V.y( (pMatrix->r21 + pMatrix->r12) * s);
			pQuat->W   = (pMatrix->r01 - pMatrix->r10) * s;
			break;
		default:
			ASSERT(FALSE);
		}
	}
}
#endif

#ifndef EXCLUDE_GENERIC_QUATERNION_TO_MATRIX
void qatToRotMat(const ts_Quatvw *pQuat, Cartesian4x4 *pMatrix)
{

//djr	double s,zz;
	float s,zz;//djr: these don't need to be doubles, do they?

	bool quatNorm = qatNormalize((ts_Quatvw*) pQuat);

	if (quatNorm == FALSE) // BHY: Needed for strict GAMECUBE FPU Check
	{
		pMatrix->Identity();
		return;
	}

	s = 2.0f / (pQuat->V.LengthSquared() + (pQuat->W * pQuat->W));

	Vector3 vs = pQuat->V * s;
	Vector3 wvs = vs * pQuat->W;
	Vector3 xvs = vs * pQuat->V.x();
	Vector3 yvs = vs * pQuat->V.y();

	zz = pQuat->V.z() * vs.z();

	pMatrix->r00 = 1.0f - (yvs.y() + zz);
	pMatrix->r01 = xvs.y() + wvs.z();
	pMatrix->r02 = xvs.z() - wvs.y();

	pMatrix->r10 = xvs.y() - wvs.z();
	pMatrix->r11 = 1.0f - (xvs.x() + zz);
	pMatrix->r12 = yvs.z() + wvs.x();

	pMatrix->r20 = xvs.z() + wvs.y();
	pMatrix->r21 = yvs.z() - wvs.x();
	pMatrix->r22 = 1.0f - (xvs.x() + yvs.y());

	pMatrix->u0 = 0.f; pMatrix->u1 = 0.f; pMatrix->u2 = 0.f; pMatrix->w = 1.f;
	pMatrix->tx = 0.f; pMatrix->ty = 0.f; pMatrix->tz = 0.f;
}
#endif

void qatSlerpBetter(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult)
{
	ts_Quatvw qTemp;
	float mag1, mag2;

	//accuracy enhancement described on page 365
	
#if 0 // TJC - Vectorized
	mag1 = ((pFrom->V.x() + pTo->V.x()) * (pFrom->V.x() + pTo->V.x())) +
			 ((pFrom->V.y() + pTo->V.y()) * (pFrom->V.y() + pTo->V.y())) +
			 ((pFrom->V.z() + pTo->V.z()) * (pFrom->V.z() + pTo->V.z())) +
			 ((pFrom->W   + pTo->W  ) * (pFrom->W   + pTo->W  ));
	mag2 = ((pFrom->V.x() - pTo->V.x()) * (pFrom->V.x() - pTo->V.x())) +
			 ((pFrom->V.y() - pTo->V.y()) * (pFrom->V.y() - pTo->V.y())) +
			 ((pFrom->V.z() - pTo->V.z()) * (pFrom->V.z() - pTo->V.z())) +
			 ((pFrom->W   - pTo->W  ) * (pFrom->W   - pTo->W  ));
#else
	mag1 = (pFrom->V + pTo->V).LengthSquared() +
			 ((pFrom->W   + pTo->W  ) * (pFrom->W   + pTo->W  ));
	mag2 = (pFrom->V - pTo->V).LengthSquared() +
			 ((pFrom->W   - pTo->W  ) * (pFrom->W   - pTo->W  ));
#endif

	if (mag1 > mag2)
		qatSlerp(pFrom, pTo, t, pResult);
	else
	{
		qTemp.V = -pTo->V;
		qTemp.W   = -pTo->W  ;
		qatSlerp(pFrom, &qTemp, t, pResult);
	}
}


void qatSlerp(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult)
{
//	float *qt = (float *)pResult;
//	float *p = (float *)pFrom;
//	float *q = (float *)pTo;
	float omega, cosom, sinom, fScaleFrom, fScaleTo;

	cosom = pFrom->V.LengthSquared() + (pFrom->W * pTo->W);

	if ((1.0f + cosom) > EPSILON)
	{
		if ((1.0f - cosom) > EPSILON)
		{
			omega = Math::ArcCos(cosom);
			sinom = Math::Sin(omega);
			fScaleFrom = Math::Sin((1.0f - t) * omega) / sinom;
			fScaleTo = Math::Sin(t * omega) / sinom;
		}
		else
		{
			fScaleFrom = 1.0f - t;
			fScaleTo = t;
		}
#if 0 // TJC - vectorized
		pResult->V.Set((float) ((fScaleFrom * pFrom->V.x()) + (fScaleTo * pTo->V.x())),
							(float) ((fScaleFrom * pFrom->V.y()) + (fScaleTo * pTo->V.y())),
							(float) ((fScaleFrom * pFrom->V.z()) + (fScaleTo * pTo->V.z())));
#else
		pResult->V = pFrom->V * fScaleFrom + pTo->V * fScaleTo;
#endif
		pResult->W   = (float) ((fScaleFrom * pFrom->W  ) + (fScaleTo * pTo->W  ));
	}
	else
	{
		pResult->V.x( -pFrom->V.y());
		pResult->V.y( pFrom->V.x());
		pResult->V.z( -pFrom->W);
		pResult->W   = pFrom->V.z();
		fScaleFrom = Math::Sin((1.0f - t) * Math::HalfPi);
		fScaleTo = Math::Sin(t * Math::HalfPi);

#if 0 // TJC - Vectorized
		pResult->V.Set((float) ((fScaleFrom * pFrom->V.x()) + (fScaleTo * pResult->V.x())),
							(float) ((fScaleFrom * pFrom->V.y()) + (fScaleTo * pResult->V.y())),
							(float) ((fScaleFrom * pFrom->V.z()) + (fScaleTo * pResult->V.z())));
#else
		pResult->V = pFrom->V * fScaleFrom + pResult->V * fScaleTo;
#endif
//yuch-- loop in textbook is i = 0; i < 3, though it seems hard to believe
//		pResult->W   = (float) ((fScaleFrom * pFrom->W  ) + (fScaleTo * pResult->W  ));
	}
}

#endif NEW_ALGORITHM



