////////////////////////////////////////////////////////////////////////////
//
// ObjectHelper
//
// Helper functions for objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ObjectHelper.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:58p
 * Updated in $/Avalanche/tools/Tupper3d
 * export vertex colors (from apply vc modifier)
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_OBJECT_HELPER_H
#define TUPPER3D_OBJECT_HELPER_H

class TupObjectPool;
class TupModifierPool;

#ifndef TUPPER3D_TUP_VERTEX_H
#include "TupVertex.h"
#endif

class TupObjectHelper
{
public:
	static int FindBaseObjectRef(TupObjectPool &tupObjectPool,int objectRef);
	static void FindVertexColors(TupObjectPool &tupObjectPool,int objectRef,
										  TupModifierPool &tupModifierPool,
										  TupArray<TupVertex3f> &colorVerts,TupArray<int> &colorIndices,
										  TupArray<float> &alphaVerts,TupArray<int> &alphaIndices,
										  bool &bMixedColors);
	static bool FindSkin(TupObjectPool &tupObjectPool,int objectRef,TupModifierPool &tupModifierPool,
										  TupperwareAggregate **pSkinModifierAgg,TupperwareAggregate **pSkinModifierContextAgg);
	static void FindFaceAttributes(TupObjectPool &tupObjectPool,int objectRef,
											 TupModifierPool &tupModifierPool,
											 TupArray<TupString> &faceAttrVerts,TupArray<int> &faceAttrIndices);
	static TupperwareAggregate *TupObjectHelper::FindBaseMesh(TupObjectPool &tupObjectPool,int objectRef);
};

#endif
