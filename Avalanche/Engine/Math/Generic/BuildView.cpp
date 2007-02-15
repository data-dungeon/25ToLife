/*************************************/
/*                                   */
/*   BuildView.cpp                   */
/*   new game engine  03/20/01       */
/*   ned martin  avalanche software  */
/*   misc. math function             */
/*                                   */
/*************************************/

#include "Math/MathPCH.h"

//============================================================================

void BuildCTM(Vector3CRef pEye, Vector3CRef pFocal, Vector3CRef pUp, Graphics4x4 *pMatrix)
{
	Vector3 U;
	Vector3 V;
	Vector3 N;

	// Compute N.
	N = pEye - pFocal;

	// Eyepoint and focal point coindident.
	if (!N.SafeNormalize())
	{
		// Just use the up vector as V.
		V = pUp;

		// Up vector zero length.
		if (!V.SafeNormalize())
		{
			U.Set(1.0f, 0.0f, 0.0f);
			V.Set(0.0f, 1.0f, 0.0f);
			N.Set(0.0f, 0.0f, 1.0f);
		}

		// Make N perpendicular to the "most stable" principle axis.
		else
		{
			if (Math::Abs(V.X()) < Math::Abs(V.Y()))
			{
				if (Math::Abs(V.X()) < Math::Abs(V.Z()))
					N.Set(0.0f, -V.Z(), V.Y());
				else
					N.Set(-V.Y(), V.X(), 0.0f);
			}
			else if (Math::Abs(V.Y()) < Math::Abs(V.Z()))
				N.Set(V.Z(), 0.0f, -V.X());
			else
				N.Set(-V.Y(), V.X(), 0.0f);
			N.Normalize();
			U = V.Cross(N);
		}
	}
	else
	{
		U = Vector3::Cross(pUp, N);

		// N and up vector are aligned or up vector is zero length. Use "most
		// stable" axis as the up vector.
		if (!U.SafeNormalize())
		{
			if (Math::Abs(N.X()) < Math::Abs(N.Y()))
			{
				if (Math::Abs(N.X()) < Math::Abs(N.Z()))
					U.Set(0.0f, -N.Z(), N.Y());
				else
					U.Set(-N.Y(), N.X(), 0.0f);
			}
			else if (Math::Abs(N.Y()) < Math::Abs(N.Z()))
				U.Set(N.Z(), 0.0f, -N.X());
			else
				U.Set(-N.Y(), N.X(), 0.0f);
			U.Normalize();
		}
		V = N.Cross(U);
	}


	pMatrix->r00 = U.x();
	pMatrix->r10 = U.y();
	pMatrix->r20 = U.z();

	pMatrix->r01 = V.x();
	pMatrix->r11 = V.y();
	pMatrix->r21 = V.z();

	pMatrix->r02 = N.x();
	pMatrix->r12 = N.y();
	pMatrix->r22 = N.z();

	pMatrix->tx = -(U.x() * pEye.x()) - (U.y() * pEye.y()) - (U.z() * pEye.z());
	pMatrix->ty = -(V.x() * pEye.x()) - (V.y() * pEye.y()) - (V.z() * pEye.z());
	pMatrix->tz = -(N.x() * pEye.x()) - (N.y() * pEye.y()) - (N.z() * pEye.z());

	pMatrix->e03 = 0.0f;
	pMatrix->e13 = 0.0f;
	pMatrix->e23 = 0.0f;
	pMatrix->w = 1.0f;
}

//============================================================================
