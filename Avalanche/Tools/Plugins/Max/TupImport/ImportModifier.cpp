////////////////////////////////////////////////////////////////////////////
//
// ImportModifier
//
// Class to handle the modifiers
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportModifier.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 */

#include "TupImportPch.h"
#include "TupImport.h"
#include "applyvc.h"
#include "vertexpaint.h"
#include "holdvc.h"
#include "bonemoddata.h"
#include "../UIDisplay/CurrAttribBase.h"
#include "../perFaceDataMod/modData.h"
#include "../Cloth/ClothModData.h"

#define PERFACEDATAMOD_CLASS_ID	Class_ID(0x4d29c57b, 0x112abd22)
#define CLOTH_CLASS_ID	Class_ID(0x697e6651, 0xcdfbd77)


int TupImport::AddModifier(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext)
{
	if (!pModifier) return -1;
	int index = m_pModifierPoolPtrs->GetIndex(pModifier);
	TupperwareAggregate *pModifierAgg = NULL; // start with a null (don't use if already set it up)
	if (index==-1) // if it is not in the list
	{
		index = m_pModifierPoolPtrs->Add(pModifier);
		pModifierAgg = m_pModifierPool->AddModifier(index);
	}
	ModifierType modifierType = GetModifierType(pModifier);
	switch (modifierType)
	{
		case MODIFIER_APPLY_VC:
			{
				// if we just created this
				if (pModifierAgg)
				{
					TupModifier tupModifier(pModifierAgg);
					tupModifier.SetModifierType(TupModifier::APPLY_VC_MODIFIER);
					AddApplyVCModifier(pModifierAgg,pModifier);
				}
				// do this every time (even if the modifier was created before
				AddApplyVCModifierContext(pModifier,pModifierContextAgg,pModifierContext);
			}
			break;
		case MODIFIER_VERTEX_PAINT:
			{
				// if we just created this
				if (pModifierAgg)
				{
					TupModifier tupModifier(pModifierAgg);
					tupModifier.SetModifierType(TupModifier::VERTEX_PAINT_MODIFIER);
					AddVertexPaintModifier(pModifierAgg,pModifier);
				}
				// do this every time (even if the modifier was created before
				AddVertexPaintModifierContext(pModifier,pModifierContextAgg,pModifierContext);
			}
			break;
		case MODIFIER_HOLD_VC:
			{
				// if we just created this
				if (pModifierAgg)
				{
					TupModifier tupModifier(pModifierAgg);
					tupModifier.SetModifierType(TupModifier::HOLD_VC_MODIFIER);
					AddHoldVCModifier(pModifierAgg,pModifier);
				}
				// do this every time (even if the modifier was created before
				AddHoldVCModifierContext(pModifier,pModifierContextAgg,pModifierContext);
			}
			break;
		case MODIFIER_FACE_ATTRIBUTE:
			{
				// if we just created this
				if (pModifierAgg)
				{
					TupModifier tupModifier(pModifierAgg);
					tupModifier.SetModifierType(TupModifier::FACE_ATTRIBUTE_MODIFIER);
					AddFaceAttributesModifier(pModifierAgg,pModifier);
				}
				// do this every time (even if the modifier was created before
				AddFaceAttributesModifierContext(pModifier,pModifierContextAgg,pModifierContext);
			}
			break;
		case MODIFIER_SKIN:
			{
				// if we just created this
				if (pModifierAgg)
				{
					TupModifier tupModifier(pModifierAgg);
					tupModifier.SetModifierType(TupModifier::SKIN_MODIFIER);
					AddSkinModifier(pModifierAgg,pModifier);
				}
				// do this every time (even if the modifier was created before
				AddSkinModifierContext(pModifier,pModifierContextAgg,pModifierContext);
			}
			break;
		case MODIFIER_CLOTH:
			{
				// if we just created this
				if (pModifierAgg)
				{
					TupModifier tupModifier(pModifierAgg);
					tupModifier.SetModifierType(TupModifier::CLOTH_MODIFIER);
					AddClothModifier(pModifierAgg,pModifier);
				}
				// do this every time (even if the modifier was created before
				AddClothModifierContext(pModifier,pModifierContextAgg,pModifierContext);
			}
			break;
		case MODIFIER_UNKNOWN:
			{
				// if we just created this
				if (pModifierAgg)
				{
					TupModifier tupModifier(pModifierAgg);
					tupModifier.SetModifierType(TupModifier::UNKNOWN);
					AddUnknownModifier(pModifierAgg,pModifier);
				}
				// do this every time (even if the modifier was created before
				AddUnknownModifierContext(pModifier,pModifierContextAgg,pModifierContext);
			}
			break;
	}
	return index;
}

// scan the classids of the modifier and return one of the known types or unknown
TupImport::ModifierType TupImport::GetModifierType(Modifier *pModifier)
{
	if (pModifier->ClassID() == APPLYVC_MOD_CLASS_ID) 
		return MODIFIER_APPLY_VC;
	else if (pModifier->ClassID() == VERTEXPAINT_CLASS_ID) 
		return MODIFIER_VERTEX_PAINT;
	else if (pModifier->ClassID() == HOLDVC_MOD_CLASS_ID) 
		return MODIFIER_HOLD_VC;
	else if (pModifier->ClassID() == PERFACEDATAMOD_CLASS_ID) 
		return MODIFIER_FACE_ATTRIBUTE;
	else if (pModifier->ClassID() == SKIN_CLASSID) 
		return MODIFIER_SKIN;
	else if (pModifier->ClassID() == CLOTH_CLASS_ID) 
		return MODIFIER_CLOTH;
	else
		return MODIFIER_UNKNOWN;
}

void TupImport::AddApplyVCModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifierBase)
{
	ApplyVCMod *pModifier = (ApplyVCMod *)pModifierBase;

	TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
	tupApplyVCModifier.AddName(pModifier->GetName());

	int flags = 0;
	if (pModifier->IsEnabled())
		flags |= TupApplyVCModifier::FLAGS_ENABLED;

	if (pModifier->mixedVertexColors.Count() > 0) 
	{
		flags |= TupApplyVCModifier::FLAGS_MIXED_COLORS;

		int numColors = pModifier->mixedVertexColors.Count();
		Keeper<Point3> colorVertexArray;
		colorVertexArray.Add(Point3(0.0,0.0,0.0)); // first entry is black
		int *colorIndexArray = new int[numColors];
		for (int i=0;i<numColors;i++)
		{
			colorIndexArray[i] = colorVertexArray.Add(*pModifier->mixedVertexColors[i]);
		}
		tupApplyVCModifier.AddColorVerts((float *)colorVertexArray.GetData(),colorVertexArray.GetSize());
		tupApplyVCModifier.AddColorIndices((int *)colorIndexArray,numColors);
		delete colorIndexArray;
	}
	else if (pModifier->faceColors.Count() > 0) 
	{
		int numFaces = pModifier->faceColors.Count();
		Keeper<Point3> colorVertexArray;
		colorVertexArray.Add(Point3(0.0,0.0,0.0)); // first entry is black
		TVFace *colorFaceArray = new TVFace[numFaces];
		for (int i=0;i<numFaces;i++)
		{
			colorFaceArray[i].t[0] = colorVertexArray.Add(pModifier->faceColors[i]->colors[0]);
			colorFaceArray[i].t[1] = colorVertexArray.Add(pModifier->faceColors[i]->colors[1]);
			colorFaceArray[i].t[2] = colorVertexArray.Add(pModifier->faceColors[i]->colors[2]);
		}
		tupApplyVCModifier.AddColorVerts((float *)colorVertexArray.GetData(),colorVertexArray.GetSize());
		tupApplyVCModifier.AddColorIndices((int *)colorFaceArray,numFaces*3);
		delete colorFaceArray;
	}
	tupApplyVCModifier.AddFlags(flags);
}

void TupImport::AddApplyVCModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext)
{
	// this modifier does not have any local data
}

void TupImport::AddVertexPaintModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifierBase)
{
	VertexPaint *pModifier = (VertexPaint *)pModifierBase;
	TupVertexPaintModifier tupVertexPaintModifier(pModifierAgg);
	tupVertexPaintModifier.AddName(pModifier->GetName());
	int flags = 0;
	if (pModifier->IsEnabled())
		flags |= TupVertexPaintModifier::FLAGS_ENABLED;
	tupVertexPaintModifier.AddFlags(flags);
}

void TupImport::AddVertexPaintModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext)
{
	VertexPaintData *pVertexPaintData = (VertexPaintData *)pModifierContext->localData;
	if (!pVertexPaintData) return; // should not be NULL

	TupVertexPaintModifierContext tupVertexPaintModifierContext(pModifierContextAgg);

	// pointers to hold allocated arrays
	Color *pColorTab;
	float *pAlphaTab;
	float *pMixFactorTab;
	DWORD *pFaceIndexTab;
	int *pPointIndexTab;

	int i;
	int totalColors,actualColors,colorIndex;

	// do vertex color channel
	if (pVertexPaintData->ChannelActive(0))
	{
		totalColors = pVertexPaintData->GetNumColors(0);
		// check through the list and count the actual colors used
		for (i=0,actualColors=0;i<totalColors;i++)
		{
			ColorData &colorData = pVertexPaintData->GetColorData(i,0);
			if (colorData.color != 0xffffffff)
				actualColors++;
		}
		if (actualColors)
		{
			pColorTab = new Color[actualColors];
			pMixFactorTab = new float[actualColors];
			pFaceIndexTab = new DWORD[actualColors];
			pPointIndexTab = new int[actualColors];

			for (i=0,colorIndex=0;i<totalColors;i++)
			{
				ColorData &colorData = pVertexPaintData->GetColorData(i,0);
				if (colorData.color != 0xffffffff)
				{
					pColorTab[colorIndex] = Color(colorData.color);
					pMixFactorTab[colorIndex] = colorData.bary;
					pFaceIndexTab[colorIndex] = colorData.fi;
					pPointIndexTab[colorIndex] = colorData.vi;
					colorIndex++;
				}
			}
			tupVertexPaintModifierContext.AddVertexColorChannelColorData((float *)pColorTab,actualColors);
			tupVertexPaintModifierContext.AddVertexColorChannelMixFactorData(pMixFactorTab,actualColors);
			tupVertexPaintModifierContext.AddVertexColorChannelFaceIndexData((int *)pFaceIndexTab,actualColors);
			tupVertexPaintModifierContext.AddVertexColorChannelPointIndexData(pPointIndexTab,actualColors);

			delete pColorTab;
			delete pMixFactorTab;
			delete pFaceIndexTab;
			delete pPointIndexTab;
		}
	}

	// do illumination color channel
	if (pVertexPaintData->ChannelActive(1))
	{
		totalColors = pVertexPaintData->GetNumColors(1);
		// check through the list and count the actual colors used
		for (i=0,actualColors=0;i<totalColors;i++)
		{
			ColorData &colorData = pVertexPaintData->GetColorData(i,1);
			if (colorData.color != 0xffffffff)
				actualColors++;
		}
		if (actualColors)
		{
			pColorTab = new Color[actualColors];
			pMixFactorTab = new float[actualColors];
			pFaceIndexTab = new DWORD[actualColors];
			pPointIndexTab = new int[actualColors];

			for (i=0,colorIndex=0;i<totalColors;i++)
			{
				ColorData &colorData = pVertexPaintData->GetColorData(i,1);
				if (colorData.color != 0xffffffff)
				{
					pColorTab[colorIndex] = Color(colorData.color);
					pMixFactorTab[colorIndex] = colorData.bary;
					pFaceIndexTab[colorIndex] = colorData.fi;
					pPointIndexTab[colorIndex] = colorData.vi;
					colorIndex++;
				}
			}
			tupVertexPaintModifierContext.AddIlluminationChannelColorData((float *)pColorTab,actualColors);
			tupVertexPaintModifierContext.AddIlluminationChannelMixFactorData(pMixFactorTab,actualColors);
			tupVertexPaintModifierContext.AddIlluminationChannelFaceIndexData((int *)pFaceIndexTab,actualColors);
			tupVertexPaintModifierContext.AddIlluminationChannelPointIndexData(pPointIndexTab,actualColors);

			delete pColorTab;
			delete pMixFactorTab;
			delete pFaceIndexTab;
			delete pPointIndexTab;
		}
	}

	// do alpha channel
	if (pVertexPaintData->ChannelActive(2))
	{
		totalColors = pVertexPaintData->GetNumColors(2);
		// check through the list and count the actual colors used
		for (i=0,actualColors=0;i<totalColors;i++)
		{
			AlphaData &alphaData = pVertexPaintData->GetAlphaData(i);
			if (alphaData.alpha != 1)
				actualColors++;
		}
		if (actualColors)
		{
			pAlphaTab = new float[actualColors];
			pMixFactorTab = new float[actualColors];
			pFaceIndexTab = new DWORD[actualColors];
			pPointIndexTab = new int[actualColors];

			for (i=0,colorIndex=0;i<totalColors;i++)
			{
				AlphaData &alphaData = pVertexPaintData->GetAlphaData(i);
				if (alphaData.alpha != 1)
				{
					pAlphaTab[colorIndex] = alphaData.alpha;
					pMixFactorTab[colorIndex] = alphaData.bary;
					pFaceIndexTab[colorIndex] = alphaData.fi;
					pPointIndexTab[colorIndex] = alphaData.vi;
					colorIndex++;
				}
			}
			tupVertexPaintModifierContext.AddAlphaChannelColorData(pAlphaTab,actualColors);
			tupVertexPaintModifierContext.AddAlphaChannelMixFactorData(pMixFactorTab,actualColors);
			tupVertexPaintModifierContext.AddAlphaChannelFaceIndexData((int *)pFaceIndexTab,actualColors);
			tupVertexPaintModifierContext.AddAlphaChannelPointIndexData(pPointIndexTab,actualColors);

			delete pAlphaTab;
			delete pMixFactorTab;
			delete pFaceIndexTab;
			delete pPointIndexTab;
		}
	}
}

void TupImport::AddHoldVCModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifierBase)
{
	HoldVCMod *pModifier = (HoldVCMod *)pModifierBase;

	TupHoldVCModifier tupHoldVCModifier(pModifierAgg);
	tupHoldVCModifier.AddName(pModifier->GetName());

	int flags = 0;
	if (pModifier->IsEnabled())
		flags |= TupApplyVCModifier::FLAGS_ENABLED;

	int numColors = pModifier->colorTable.Count();
	int numFaces = pModifier->faceTable.Count();

	Keeper<Color> colorVertexArray;
	colorVertexArray.Add(Point3(0.0,0.0,0.0)); // first entry is black
	TVFace *colorFaceArray = new TVFace[numFaces];
	// condense again (keep the first as black in case there is a problem later with the face counts not in sync)
	for (int i=0;i<numFaces;i++)
	{
		colorFaceArray[i].t[0] = colorVertexArray.Add(*pModifier->colorTable[pModifier->faceTable[i]->t[0]]);
		colorFaceArray[i].t[1] = colorVertexArray.Add(*pModifier->colorTable[pModifier->faceTable[i]->t[1]]);
		colorFaceArray[i].t[2] = colorVertexArray.Add(*pModifier->colorTable[pModifier->faceTable[i]->t[2]]);
	}
	tupHoldVCModifier.AddColorVerts((float *)colorVertexArray.GetData(),colorVertexArray.GetSize());
	tupHoldVCModifier.AddColorIndices((int *)colorFaceArray,numFaces);
	delete colorFaceArray;
	tupHoldVCModifier.AddFlags(flags);
}

void TupImport::AddHoldVCModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext)
{
	// this modifier does not have any local data
}

void TupImport::AddFaceAttributesModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifierBase)
{
	TupFaceAttributesModifier tupFaceAttributesModifier(pModifierAgg);
	tupFaceAttributesModifier.AddName(pModifierBase->GetName());
	int flags = 0;
	if (pModifierBase->IsEnabled())
		flags |= TupFaceAttributesModifier::FLAGS_ENABLED;
	tupFaceAttributesModifier.AddFlags(flags);
}

// provide a hash value function for a vector of ints
inline unsigned int GetHashValue(const TupArray<int>& entry)
{
	int size = entry.GetSize();
	if (size)
	{
		return HashValue::GetHashValue(&entry[0],sizeof(int)*size);
	}
	else
	{
		return -1;
	}
}

void TupImport::AddFaceAttributesModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext)
{
	CModData *pModData = (CModData *)pModifierContext->localData;
	if (!pModData) return; // should not be NULL

	TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pModifierContextAgg);
	const KEYVAL_VECTOR &KeyValVect = pModData->GetKeyValVect();
	const BITARRAY_VECTOR &FSelVect = pModData->GetFSelVect();

	Keeper<TupArray<int> > keyValIndexKeeper;

	TupArray<int> dummyValIndexArray;
	keyValIndexKeeper.Add(dummyValIndexArray); // add one that does not have any indexes

	int numBitArrays = FSelVect.size();
	int numKeyValuePairs = KeyValVect.size();
	assert(numBitArrays>=numKeyValuePairs);
	if (numKeyValuePairs)
	{
		int numFaces = FSelVect[0].GetSize();
		if (numFaces)
		{
			int *pFaceIndices = new int[numFaces];
			for (int faceIndex = 0;faceIndex<numFaces;faceIndex++)
			{
				TupArray<int> keyValIndexArray;
				// add indexes according to bits set in the bit arrays (at faceIndex)
				for (int bitArrayIndex = 0;bitArrayIndex<numKeyValuePairs;bitArrayIndex++)
				{
					if (FSelVect[bitArrayIndex][faceIndex]) // if this face has the bit set in the bit array
					{
						// make sure that it is not a empty entry
						if (!KeyValVect[bitArrayIndex].m_strKeyname.empty() && 
							!KeyValVect[bitArrayIndex].m_strValue.empty())
						{
							keyValIndexArray.Add(bitArrayIndex); // add to array
						}
					}
				}
				pFaceIndices[faceIndex] = keyValIndexKeeper.Add(keyValIndexArray);
			}
			TupArray<TupString> keyValStringArray;
			int numKeeperEntries = keyValIndexKeeper.GetSize();
			for (int keeperIndex = 0;keeperIndex<numKeeperEntries;keeperIndex++)
			{
				TupString keyValString;
				int numKeyValueIndexes = keyValIndexKeeper[keeperIndex].GetSize();
				for (int keyValueIndex = 0;keyValueIndex<numKeyValueIndexes;keyValueIndex++)
				{
					int index1 = (keyValIndexKeeper[keeperIndex])[keyValueIndex];
					int index2 = keyValIndexKeeper[keeperIndex][keyValueIndex];
					const TupString &key = KeyValVect[keyValIndexKeeper[keeperIndex][keyValueIndex]].m_strKeyname.c_str();
					keyValString += KeyValVect[keyValIndexKeeper[keeperIndex][keyValueIndex]].m_strKeyname.c_str();
					keyValString += "=";
					keyValString += KeyValVect[keyValIndexKeeper[keeperIndex][keyValueIndex]].m_strValue.c_str();
					keyValString += "\n";
				}
				keyValStringArray.Add(keyValString);
			}
			tupFaceAttributesModifierContext.AddFaceAttributeIndices(pFaceIndices,numFaces);
			tupFaceAttributesModifierContext.AddFaceAttributeVerts(keyValStringArray);
			delete pFaceIndices;
		}
		else // no faces
		{
			// default to saving one face with no attributes
			int tempIndex[1] = { 0 };
			TupArray<TupString> keyValStringArray;
			keyValStringArray.Add(TupString(""));
			tupFaceAttributesModifierContext.AddFaceAttributeIndices(tempIndex,1);
			tupFaceAttributesModifierContext.AddFaceAttributeVerts(keyValStringArray);
		}
	}
	else // no bit arrays
	{
		// default to saving one face with no attributes
		int tempIndex[1] = { 0 };
		TupArray<TupString> keyValStringArray;
		keyValStringArray.Add(TupString(""));
		tupFaceAttributesModifierContext.AddFaceAttributeIndices(tempIndex,1);
		tupFaceAttributesModifierContext.AddFaceAttributeVerts(keyValStringArray);
	}
}

void TupImport::AddSkinModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifierBase)
{
	TupSkinModifier tupSkinModifier(pModifierAgg);
	tupSkinModifier.AddName(pModifierBase->GetName());
	int flags = 0;
	if (pModifierBase->IsEnabled())
		flags |= TupSkinModifier::FLAGS_ENABLED;
	tupSkinModifier.AddFlags(flags);
																	
	ISkin *pISkin = (ISkin*)pModifierBase->GetInterface(I_SKIN);

	// count the number of valid bones
	int numBones = pISkin->GetNumBonesFlat();
	Tab<int> boneMap;
	boneMap.SetCount(numBones);
	int count=0;
	int boneIndex;
	for( boneIndex = 0 ; boneIndex < numBones ; boneIndex++ )
	{
		if(pISkin->GetBoneFlat(boneIndex) != NULL)
		{
			boneMap[count++] = boneIndex;
		}
	}
	boneMap.SetCount(count);
	numBones = count;

	for (boneIndex=0;boneIndex<numBones;boneIndex++)
	{
		TupperwareAggregate *pBoneAgg = tupSkinModifier.AddBone(boneIndex);
		TupSkinModifierBone tupSkinModifierBone(pBoneAgg);
		const char *pName = pISkin->GetBoneName(boneMap[boneIndex]);
		if (pName)
			tupSkinModifierBone.AddName(pName);
		DWORD properties = pISkin->GetBoneProperty(boneMap[boneIndex]);
		int flags = 0;
		if (properties&BONE_ABSOLUTE_FLAG)
			flags |= TupSkinModifierBone::ABSOLUTE_WEIGHTS;
		if (properties&BONE_SPLINE_FLAG)
			flags |= TupSkinModifierBone::IS_SPLINE;
		if (properties&BONE_SPLINECLOSED_FLAG)
			flags |= TupSkinModifierBone::IS_CLOSED_SPLINE;
		if (properties&BONE_DRAW_ENVELOPE_FLAG)
			flags |= TupSkinModifierBone::DRAW_ENVELOPE;
		if (properties&BONE_BONE_FLAG)
			flags |= TupSkinModifierBone::BONE_FLAG;
		if (properties&BONE_DEAD_FLAG)
			flags |= TupSkinModifierBone::BONE_DEAD;
		tupSkinModifierBone.AddFlags(flags);
		INode *pNode = pISkin->GetBoneFlat(boneMap[boneIndex]);

		TupperwareAggregate *pNodeAgg;
		int nodeReference = m_pNodePoolPtrs->GetIndex(pNode);
		// if this child is new to the pools
		if (nodeReference==-1)
		{
			nodeReference = m_pNodePoolPtrs->Add(pNode); // add to both pools
			pNodeAgg = m_pNodePool->AddNode(nodeReference);
			AddNode(-1,nodeReference,pNodeAgg,pNode,false); // this should never happen... if it does then we need some more work
			assert(false);
		}
		// if this child is already in the pools
		else
		{
			pNodeAgg = m_pNodePool->GetNode(nodeReference);
			assert(pNodeAgg);
		}
		tupSkinModifierBone.AddNodeReference(nodeReference);

		Matrix3 tm;
		Graphics4x4 matrix;
		if (pISkin->GetBoneInitTM(pNode,tm,true) != SKIN_OK)
			tm.IdentityMatrix();
		ConvertMaxMatrix(tm,matrix);
		tupSkinModifierBone.AddBoneInitObjectTM(&matrix.r00);
	}
}

void TupImport::AddSkinModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext)
{
	BoneModDataREV4 *pBoneModDataREV4 = (BoneModDataREV4 *)pModifierContext->localData;
	BoneModDataREV5 *pBoneModDataREV5 = (BoneModDataREV5 *)pModifierContext->localData;
	DWORD v = Get3DSMAXVersion();

	ISkinContextData *pModData;
	Graphics4x4 matrix;

	int r = GET_MAX_RELEASE(v);
	if (r >= 5000)
	{ 
		pModData = (ISkinContextData *)pBoneModDataREV5;
		ConvertMaxMatrix(pBoneModDataREV5->BaseTM,matrix);
	}
	else
	{
		pModData = (ISkinContextData *)pBoneModDataREV4;
		ConvertMaxMatrix(pBoneModDataREV4->BaseTM,matrix);
	}
	if (!pModData) return; // should not be NULL

	TupSkinModifierContext tupSkinModifierContext(pModifierContextAgg);
	TupperwareAggregate *pVertexPoolAgg = tupSkinModifierContext.AddVertexPool();
	TupSkinModifierContextVertexPool tupSkinModifierContextVertexPool(pVertexPoolAgg);

	tupSkinModifierContext.AddBaseTM(&matrix.r00);

	int totalVerts = pModData->GetNumPoints();
	for (int vertexIndex = 0;vertexIndex<totalVerts;vertexIndex++)
	{
		TupperwareAggregate *pVertexAgg = tupSkinModifierContextVertexPool.AddVertex(vertexIndex);
		int numBones = pModData->GetNumAssignedBones(vertexIndex);
		if (numBones)
		{
			// only count the bones that have a weight of >0
			int actualBoneCount = 0;
			for (int boneIndex=0;boneIndex<numBones;boneIndex++)
			{
				if (pModData->GetBoneWeight(vertexIndex,boneIndex)>0.0f)
					actualBoneCount++;
			}
			if (actualBoneCount)
			{
				int actualBoneIndex = 0;
				TupSkinModifierContextVertex tupSkinModifierContextVertex(pVertexAgg);
				int *pBones = new int[actualBoneCount];
				float *pWeights = new float[actualBoneCount];
				for (int boneIndex=0;boneIndex<numBones;boneIndex++)
				{
					if (pModData->GetBoneWeight(vertexIndex,boneIndex)>0.0f)
					{
						// only add when the weight is >0
						pBones[actualBoneIndex] = pModData->GetAssignedBone(vertexIndex,boneIndex);
						pWeights[actualBoneIndex] = pModData->GetBoneWeight(vertexIndex,boneIndex);
						actualBoneIndex++;
					}
				}
				tupSkinModifierContextVertex.AddBones(pBones,actualBoneCount);
				tupSkinModifierContextVertex.AddWeights(pWeights,actualBoneCount);
			}
		}
	}
}

void TupImport::AddClothModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifierBase)
{
	TupClothModifier tupClothModifier(pModifierAgg);
	tupClothModifier.AddName(pModifierBase->GetName());
	int flags = 0;
	if (pModifierBase->IsEnabled())
		flags |= TupClothModifier::FLAGS_ENABLED;
	tupClothModifier.AddFlags(flags);
}

void TupImport::AddClothModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext)
{
	ClothModData *pClothModData = (ClothModData *)pModifierContext->localData;
	if (!pClothModData) return; // should not be NULL

	TupClothModifierContext tupClothModifierContext(pModifierContextAgg);

	int numPoints = pClothModData->points.Count();
	TupArray<int> pointFlags;
	pointFlags.SetSize(numPoints);

	// set the flags for each point
	for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
	{
		int flags = 0;
		switch(pClothModData->points[pointIndex].state)
		{
			case POINTSTATE_INACTIVE:
				flags = 0;
				break;
			case POINTSTATE_FRONT:
				flags = TupClothModifierContext::FLAGS_POINT_ACTIVE;
				break;
			case POINTSTATE_ATTACHED:
				flags = TupClothModifierContext::FLAGS_POINT_ACTIVE | 
						TupClothModifierContext::FLAGS_POINT_ATTACHED;
				break;
			case POINTSTATE_BACK:
				flags = TupClothModifierContext::FLAGS_POINT_ACTIVE | 
						TupClothModifierContext::FLAGS_POINT_BACK;
				break;
		}
		pointFlags[pointIndex] = flags;
	}
	tupClothModifierContext.AddPointFlags(pointFlags.GetData(),numPoints);
	int numConnections = pClothModData->connections.Count();
	if (numConnections)
	{
		TupArray<int> connectionArray;
		connectionArray.SetSize(numConnections<<1);
		for (int connectionIndex=0;connectionIndex<numConnections;connectionIndex++)
		{
			connectionArray[connectionIndex<<1] = pClothModData->connections[connectionIndex].v1;
			connectionArray[(connectionIndex<<1)+1] = pClothModData->connections[connectionIndex].v2;
		}
		tupClothModifierContext.AddConnectionPoints(connectionArray.GetData(),numConnections);
	}
}

void TupImport::AddUnknownModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifier)
{
	TupUnknownModifier tupUnknownModifier(pModifierAgg);
	tupUnknownModifier.AddName(pModifier->GetName());

	int flags = 0;
	if (pModifier->IsEnabled())
		flags |= TupUnknownModifier::FLAGS_ENABLED;
	tupUnknownModifier.AddFlags(flags);
	TSTR className;
	pModifier->GetClassName(className);
	pModifierAgg->AddScalarString("ClassName",NULL,className);
	Class_ID classID = pModifier->ClassID();
	char string[40];
	sprintf(string,"0x%x",classID.PartA());
	pModifierAgg->AddScalarString("ClassID","PartA",string);
	sprintf(string,"0x%x",classID.PartB());
	pModifierAgg->AddScalarString("ClassID","PartB",string);
	SClass_ID superClassID = pModifier->SuperClassID();
	sprintf(string,"0x%x",superClassID);
	pModifierAgg->AddScalarString("SuperClassID",NULL,string);
}

void TupImport::AddUnknownModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext)
{
}
