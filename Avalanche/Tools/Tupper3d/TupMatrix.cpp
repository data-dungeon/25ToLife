////////////////////////////////////////////////////////////////////////////
//
// TupMatrix
//
// classes to handle matrices
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupMatrix.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/29/03    Time: 1:38p
 * Updated in $/Avalanche/tools/Tupper3d
 * tesselation code for 2d
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/11/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/08/03    Time: 4:59p
 * Updated in $/Avalanche/tools/Tupper3d
 */

#include "Tupper3dPch.h"
#include "TupMatrix.h"

TupMatrix4x4f TupMatrix4x4Utils::Convert(TupMatrix4x4d source)
{
	TupMatrix4x4f dest;

	dest._11 = (float)source._11;
	dest._21 = (float)source._21;
	dest._31 = (float)source._31;
	dest._41 = (float)source._41;

	dest._12 = (float)source._12;
	dest._22 = (float)source._22;
	dest._32 = (float)source._32;
	dest._42 = (float)source._42;

	dest._13 = (float)source._13;
	dest._23 = (float)source._23;
	dest._33 = (float)source._33;
	dest._43 = (float)source._43;

	dest._14 = (float)source._14;
	dest._24 = (float)source._24;
	dest._34 = (float)source._34;
	dest._44 = (float)source._44;
	return dest;
}

TupMatrix4x4d TupMatrix4x4Utils::Convert(TupMatrix4x4f source)
{
	TupMatrix4x4d dest;

	dest._11 = (double)source._11;
	dest._21 = (double)source._21;
	dest._31 = (double)source._31;
	dest._41 = (double)source._41;

	dest._12 = (double)source._12;
	dest._22 = (double)source._22;
	dest._32 = (double)source._32;
	dest._42 = (double)source._42;

	dest._13 = (double)source._13;
	dest._23 = (double)source._23;
	dest._33 = (double)source._33;
	dest._43 = (double)source._43;

	dest._14 = (double)source._14;
	dest._24 = (double)source._24;
	dest._34 = (double)source._34;
	dest._44 = (double)source._44;
	return dest;
}

