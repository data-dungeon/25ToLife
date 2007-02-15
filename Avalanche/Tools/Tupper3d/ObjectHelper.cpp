////////////////////////////////////////////////////////////////////////////
//
// ObjectHelper
//
// Helper functions for objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ObjectHelper.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:58p
 * Updated in $/Avalanche/tools/Tupper3d
 * export vertex colors (from apply vc modifier)
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "ObjectHelper.h"
#include "ObjectPool.h"
#include "object.h"
#include "TriMesh.h"
#include "TriMeshMap.h"
#include "PolyMesh.h"
#include "PolyMeshMap.h"
#include "derived.h"
#include "derivedmodifier.h"
#include "modifierpool.h"
#include "modifier.h"
#include "applyvcmodifier.h"
#include "skinModifier.h"
#include "faceattributesmodifier.h"
#include "faceattributesmodifiercontext.h"

int TupObjectHelper::FindBaseObjectRef(TupObjectPool &tupObjectPool,int objectRef)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		// try the one I point to
		TupDerived tupDerived(pObjectAgg);
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		return FindBaseObjectRef(tupObjectPool,childObjectRef);
	}
	return objectRef;
}

void TupObjectHelper::FindVertexColors(TupObjectPool &tupObjectPool,int objectRef,
													TupModifierPool &tupModifierPool,
													TupArray<TupVertex3f> &colorVerts,TupArray<int> &colorIndices,
													TupArray<float> &alphaVerts,TupArray<int> &alphaIndices,
													bool &bMixedColors)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		// try the one I point to
		TupDerived tupDerived(pObjectAgg);

		int totalModifierSlots = tupDerived.GetNumModifierSlots();
		// if there is any modifiers
		if (totalModifierSlots)
		{
			// loop through the modifiers and see if any deal with the vertex colors
			for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
					int modifierReference;
					tupDerivedModifier.GetModifierReference(modifierReference);
					TupperwareAggregate *pModifierAgg = tupModifierPool.GetModifier(modifierReference);
					assert(pModifierAgg);
					TupModifier tupModifier(pModifierAgg);
					switch(tupModifier.GetModifierType())
					{
						case TupModifier::APPLY_VC_MODIFIER:
							{
								TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
								int flags;
								tupApplyVCModifier.GetFlags(flags);
								// enabled & not mixed colors
								if (flags&TupApplyVCModifier::FLAGS_ENABLED)
								{
									bMixedColors = (flags&TupApplyVCModifier::FLAGS_MIXED_COLORS) ? true : false;
									// get the modifier information
									int numColorVerts;
									int numColorIndices;
									TupVertex3f *pColorVerts;
									int *pColorIndices;
									tupApplyVCModifier.GetColorVerts((float **)&pColorVerts,numColorVerts);
									tupApplyVCModifier.GetColorIndices(&pColorIndices,numColorIndices);
									colorIndices.SetSize(numColorIndices);
									colorVerts.SetSize(numColorVerts);

									int index;
									for (index=0;index<numColorIndices;index++)
									{
										colorIndices[index] = pColorIndices[index];
									}
									for (index=0;index<numColorVerts;index++)
									{
										colorVerts[index] = pColorVerts[index];
									}
									if (flags&TupApplyVCModifier::FLAGS_HAS_ALPHA)
									{
										int numAlphaVerts;
										int numAlphaIndices;
										float *pAlphaVerts;
										int *pAlphaIndices;
										tupApplyVCModifier.GetAlphaVerts(&pAlphaVerts,numAlphaVerts);
										tupApplyVCModifier.GetAlphaIndices(&pAlphaIndices,numAlphaIndices);
										alphaIndices.SetSize(numAlphaIndices);
										alphaVerts.SetSize(numAlphaVerts);
										
										int index;
										for (index=0;index<numAlphaIndices;index++)
										{
											alphaIndices[index] = pAlphaIndices[index];
										}
										for (index=0;index<numAlphaVerts;index++)
										{
											alphaVerts[index] = pAlphaVerts[index];
										}
									}
									else
									{
										alphaIndices.SetSize(numColorIndices);
										alphaVerts.SetSize(1);
										alphaVerts[0] = 1.0f;
										
										int index;
										for (index=0;index<numColorIndices;index++)
										{
											alphaIndices[index] = 0;
										}
									}
									return; // found the colors
								}
							}
							break;
					}
				}
			}
		}
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		FindVertexColors(tupObjectPool,childObjectRef,tupModifierPool,colorVerts,colorIndices,alphaVerts,alphaIndices,bMixedColors);
	}
	else if (tupObject.GetObjectType()==TupObject::TRI_MESH)
	{
		bool bFoundColors = false;
		bool bFoundAlphas = false;
		TupTriMesh tupTriMesh(pObjectAgg);
		int numMeshMaps = tupTriMesh.GetNumMeshMaps();
		for (int meshMapIndex = 0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
			TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
			if (tupTriMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_VERTEX_COLOR)
			{
				// get the modifier information
				int numColorVerts;
				int numColorIndices;
				TupVertex3f *pColorVerts;
				int *pColorIndices;
				tupTriMeshMap.GetMapVerts((float **)&pColorVerts,numColorVerts);
				tupTriMeshMap.GetMapIndices(&pColorIndices,numColorIndices);
				colorIndices.SetSize(numColorIndices);
				colorVerts.SetSize(numColorVerts);

				int index;
				for (index=0;index<numColorIndices;index++)
				{
					colorIndices[index] = pColorIndices[index];
				}
				for (index=0;index<numColorVerts;index++)
				{
					colorVerts[index] = pColorVerts[index];
				}
				bFoundColors = true;
			}
			if (tupTriMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_ALPHA)
			{
				int numAlphaVerts;
				int numAlphaIndices;
				TupVertex3f *pAlphaVerts;
				int *pAlphaIndices;
				tupTriMeshMap.GetMapVerts((float **)&pAlphaVerts,numAlphaVerts);
				tupTriMeshMap.GetMapIndices(&pAlphaIndices,numAlphaIndices);
				alphaIndices.SetSize(numAlphaIndices);
				alphaVerts.SetSize(numAlphaVerts);

				int index;
				for (index=0;index<numAlphaIndices;index++)
				{
					alphaIndices[index] = pAlphaIndices[index];
				}
				for (index=0;index<numAlphaVerts;index++)
				{
					alphaVerts[index] = pAlphaVerts[index].x;
				}
				bFoundAlphas = true;
			}
		}
		if (bFoundColors && !bFoundAlphas)
		{
			int numAlphaIndices = colorIndices.GetSize();
			alphaIndices.SetSize(numAlphaIndices);
			alphaVerts.SetSize(1);
			alphaVerts[0] = 1.0f;

			int index;
			for (index=0;index<numAlphaIndices;index++)
			{
				alphaIndices[index] = 0;
			}
		}
		if (bFoundAlphas && !bFoundColors)
		{
			int numColorIndices = alphaIndices.GetSize();
			colorIndices.SetSize(numColorIndices);
			colorVerts.SetSize(1);
			colorVerts[0].x = 1.0f;
			colorVerts[0].y = 1.0f;
			colorVerts[0].z = 1.0f;

			int index;
			for (index=0;index<numColorIndices;index++)
			{
				colorIndices[index] = 0;
			}
		}
	}
	else if (tupObject.GetObjectType()==TupObject::POLY_MESH)
	{
		bool bFoundColors = false;
		bool bFoundAlphas = false;
		TupPolyMesh tupPolyMesh(pObjectAgg);
		int numMeshMaps = tupPolyMesh.GetNumMeshMaps();
		for (int meshMapIndex = 0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			TupperwareAggregate *pMeshMapAgg = tupPolyMesh.GetMeshMap(meshMapIndex);
			TupPolyMeshMap tupPolyMeshMap(pMeshMapAgg);
			if (tupPolyMeshMap.GetMapChannel()==TupPolyMeshMap::MAP_CHANNEL_VERTEX_COLOR)
			{
				// get the modifier information
				int numColorVerts;
				int numColorIndices;
				TupVertex3f *pColorVerts;
				int *pColorIndices;
				tupPolyMeshMap.GetMapVerts((float **)&pColorVerts,numColorVerts);
				tupPolyMeshMap.GetMapIndices(&pColorIndices,numColorIndices);
				colorIndices.SetSize(numColorIndices);
				colorVerts.SetSize(numColorVerts);

				int index;
				for (index=0;index<numColorIndices;index++)
				{
					colorIndices[index] = pColorIndices[index];
				}
				for (index=0;index<numColorVerts;index++)
				{
					colorVerts[index] = pColorVerts[index];
				}
				bFoundColors = true;
			}
			if (tupPolyMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_ALPHA)
			{
				int numAlphaVerts;
				int numAlphaIndices;
				TupVertex3f *pAlphaVerts;
				int *pAlphaIndices;
				tupPolyMeshMap.GetMapVerts((float **)&pAlphaVerts,numAlphaVerts);
				tupPolyMeshMap.GetMapIndices(&pAlphaIndices,numAlphaIndices);
				alphaIndices.SetSize(numAlphaIndices);
				alphaVerts.SetSize(numAlphaVerts);

				int index;
				for (index=0;index<numAlphaIndices;index++)
				{
					alphaIndices[index] = pAlphaIndices[index];
				}
				for (index=0;index<numAlphaVerts;index++)
				{
					alphaVerts[index] = pAlphaVerts[index].x;
				}
				bFoundAlphas = true;
			}
		}
		if (bFoundColors && !bFoundAlphas)
		{
			int numAlphaIndices = colorIndices.GetSize();
			alphaIndices.SetSize(numAlphaIndices);
			alphaVerts.SetSize(1);
			alphaVerts[0] = 1.0f;

			int index;
			for (index=0;index<numAlphaIndices;index++)
			{
				alphaIndices[index] = 0;
			}
		}
		if (bFoundAlphas && !bFoundColors)
		{
			int numColorIndices = alphaIndices.GetSize();
			colorIndices.SetSize(numColorIndices);
			colorVerts.SetSize(1);
			colorVerts[0].x = 1.0f;
			colorVerts[0].y = 1.0f;
			colorVerts[0].z = 1.0f;

			int index;
			for (index=0;index<numColorIndices;index++)
			{
				colorIndices[index] = 0;
			}
		}
	}
	else
	{
		colorVerts.RemoveAll();
		colorIndices.RemoveAll();
		alphaVerts.RemoveAll();
		alphaIndices.RemoveAll();
	}
}

void TupObjectHelper::FindFaceAttributes(TupObjectPool &tupObjectPool,int objectRef,
													  TupModifierPool &tupModifierPool,
													  TupArray<TupString> &faceAttrVerts,TupArray<int> &faceAttrIndices)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		// try the one I point to
		TupDerived tupDerived(pObjectAgg);

		int totalModifierSlots = tupDerived.GetNumModifierSlots();
		// if there is any modifiers
		if (totalModifierSlots)
		{
			// loop through the modifiers and see if any deal with the vertex colors
			for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
					int modifierReference;
					tupDerivedModifier.GetModifierReference(modifierReference);
					TupperwareAggregate *pModifierAgg = tupModifierPool.GetModifier(modifierReference);
					assert(pModifierAgg);
					TupModifier tupModifier(pModifierAgg);
					switch(tupModifier.GetModifierType())
					{
						case TupModifier::FACE_ATTRIBUTE_MODIFIER:
							{
								TupFaceAttributesModifier tupFaceAttributesModifier(pModifierAgg);
								TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
								assert(pModifierContextAgg);
								TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pModifierContextAgg);
								int flags;
								tupFaceAttributesModifier.GetFlags(flags);
								if (flags&TupFaceAttributesModifier::FLAGS_ENABLED)
								{
									// get the modifier information
									int numAttribIndices;
									int *pAttribIndices;
									tupFaceAttributesModifierContext.GetFaceAttributeVerts(faceAttrVerts);
									tupFaceAttributesModifierContext.GetFaceAttributeIndices(&pAttribIndices,numAttribIndices);
									faceAttrIndices.SetSize(numAttribIndices);

									int index;
									for (index=0;index<numAttribIndices;index++)
									{
										faceAttrIndices[index] = pAttribIndices[index];
									}
									return; // found the attributes
								}
							}
							break;
					}
				}
			}
		}
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		FindFaceAttributes(tupObjectPool,childObjectRef,tupModifierPool,faceAttrVerts,faceAttrIndices);
	}
	else // no attributes
	{
		faceAttrVerts.RemoveAll();
		faceAttrIndices.RemoveAll();
	}
}


bool TupObjectHelper::FindSkin(TupObjectPool &tupObjectPool,int objectRef,TupModifierPool &tupModifierPool,
									 TupperwareAggregate **pSkinModifierAgg,TupperwareAggregate **pSkinModifierContextAgg)
{
	*pSkinModifierAgg = NULL;
	*pSkinModifierContextAgg = NULL;

	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);

	// make sure valid object
	if (!pObjectAgg)
		return false;

	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupDerived tupDerived(pObjectAgg);
		int totalModifierSlots = tupDerived.GetNumModifierSlots();
		// if there is any modifiers
		if (totalModifierSlots)
		{
			bool foundValidModifier = false;
			// loop through the modifiers and see if any deal with the skin modifiers
			for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
					int modifierReference;
					tupDerivedModifier.GetModifierReference(modifierReference);
					TupperwareAggregate *pModifierAgg = tupModifierPool.GetModifier(modifierReference);
					TupModifier tupModifier(pModifierAgg);
					switch(tupModifier.GetModifierType())
					{
					case TupModifier::SKIN_MODIFIER:
						{
							TupSkinModifier tupSkinModifier(pModifierAgg);
							int flags;
							tupSkinModifier.GetFlags(flags);
							if (flags&TupSkinModifier::FLAGS_ENABLED)
							{
								*pSkinModifierAgg = pModifierAgg;
								*pSkinModifierContextAgg = tupDerivedModifier.GetModifierContext();
								return true;
							}
						}
					}
				}
			}
		}
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		return FindSkin(tupObjectPool,childObjectRef,tupModifierPool,pSkinModifierAgg,pSkinModifierContextAgg);
	}
	return false;
}

TupperwareAggregate *TupObjectHelper::FindBaseMesh(TupObjectPool &tupObjectPool,int objectRef)
{
	int baseRef = FindBaseObjectRef(tupObjectPool,objectRef);
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(baseRef);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::TRI_MESH)
	{
		return pObjectAgg;
	}
	else
	{
		return NULL;
	}
}
