////////////////////////////////////////////////////////////////////////////
//
// TupVertex
//
// classes to handle vertices
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupVertex.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/05/03    Time: 12:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * no assert on 0 length vector
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/29/03    Time: 3:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * start of poly mesh and converters
 */

#include "Tupper3dPch.h"
#include "TupVertex.h"

//template class TupVertex3<float>;
//template class TupVertex3<double>;

double TupVertex3Sqrt(double value)
{
	return sqrt(value);
}


TupVertex3f TupVertex3Utils::Convert(TupVertex3d source)
{
	return TupVertex3f((float)source.x,(float)source.y,(float)source.z);
}

TupVertex3d TupVertex3Utils::Convert(TupVertex3f source)
{
	return TupVertex3d((double)source.x,(double)source.y,(double)source.z);
}

void TupVertex3Utils::Convert(TupArray<TupVertex3f> &dest,const TupArray<TupVertex3d> &source)
{
	int numElements = source.GetSize();
	dest.SetSize(numElements);
	for (int index=0;index<numElements;index++)
	{
		dest[index] = Convert(source[index]);
	}
}

void TupVertex3Utils::Convert(TupArray<TupVertex3d> &dest,const TupArray<TupVertex3f> &source)
{
	int numElements = source.GetSize();
	dest.SetSize(numElements);
	for (int index=0;index<numElements;index++)
	{
		dest[index] = Convert(source[index]);
	}
}
