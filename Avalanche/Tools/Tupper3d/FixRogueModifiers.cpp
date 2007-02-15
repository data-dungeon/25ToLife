////////////////////////////////////////////////////////////////////////////
//
// TupFixRogueModifiers
//
// Repair any problems with modifiers
//
////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "FixRogueModifiers.h"
#include "ObjectHelper.h"
#include "ObjectPool.h"
#include "object.h"
#include "TriMesh.h"
#include "TriMeshMap.h"
#include "derived.h"
#include "derivedmodifier.h"
#include "modifierpool.h"
#include "modifier.h"
#include "applyvcmodifier.h"
#include "skinModifier.h"
#include "faceattributesmodifier.h"
#include "faceattributesmodifiercontext.h"
#include "VertexPaintModifier.h"
#include "VertexPaintModifierContext.h"
#include "HoldVCModifier.h"
#include "SkinModifier.h"
#include "SkinModifierContext.h"
#include "SkinModifierContextVertexPool.h"
#include "ClothModifierContext.h"


TupFixRogueModifiers::TupFixRogueModifiers(TupObjectPool *pTupObjectPool,TupModifierPool *pTupModifierPool) : 
	m_pTupObjectPool(pTupObjectPool),
	m_pTupModifierPool(pTupModifierPool)
{
}

// go through and make sure all the modifiers conform to the meshes beneath them
void TupFixRogueModifiers::Process(void)
{
	int numObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		if (!pObjectAgg)
			continue;
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::DERIVED)
		{
			TupperwareAggregate *pBaseMeshAgg = TupObjectHelper::FindBaseMesh(*m_pTupObjectPool,objectIndex);
			if (!pBaseMeshAgg)
				continue;
			TupObject tupBaseMeshObject(pBaseMeshAgg);
			assert(tupBaseMeshObject.GetObjectType() == TupObject::TRI_MESH); // this better be a trimesh
			TupTriMesh tupBaseMesh(pBaseMeshAgg);

			TupDerived tupDerived(pObjectAgg);
			int numModifierSlots = tupDerived.GetNumModifierSlots();
			// call on all the modifiers
			for (int modifierIndex=0;modifierIndex<numModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
					int modifierReference;
					tupDerivedModifier.GetModifierReference(modifierReference);
					TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
					FixRogueModifier(modifierReference,pModifierContextAgg,tupBaseMesh);
				}
			}
		}
	}
}

void TupFixRogueModifiers::FixRogueModifier(int modifierRef,TupperwareAggregate *pModifierContextAgg,TupTriMesh &tupBaseMesh)
{
	TupperwareAggregate *pTupModifierAgg = m_pTupModifierPool->GetModifier(modifierRef);
	assert(pTupModifierAgg);
	TupModifier tupModifier(pTupModifierAgg);

	switch(tupModifier.GetModifierType())
	{
	case TupModifier::APPLY_VC_MODIFIER:
		FixRogueApplyVCModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		break;
	case TupModifier::VERTEX_PAINT_MODIFIER:
		FixRogueVertexPaintModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		break;
	case TupModifier::HOLD_VC_MODIFIER:
		FixRogueHoldVCModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		break;
	case TupModifier::FACE_ATTRIBUTE_MODIFIER:
		FixRogueFaceAttributeModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		break;
	case TupModifier::SKIN_MODIFIER:
		FixRogueSkinModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		break;
	case TupModifier::CLOTH_MODIFIER:
		FixRogueClothModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		break;
	default:
		assert(false);
	}

}

void TupFixRogueModifiers::FixRogueApplyVCModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupApplyVCModifier tupApplyVCModifier(tupModifier);
	int *pColorIndices;
	int numColorIndices;
	tupApplyVCModifier.GetColorIndices(&pColorIndices,numColorIndices);
	int flags;
	tupApplyVCModifier.GetFlags(flags);

	int numFaces;
	int *pPositionIndices;
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);

	TupVertex3f *pModifierVerts;
	int numModifierVerts;
	tupApplyVCModifier.GetColorVerts((float **)&pModifierVerts,numModifierVerts);
	if (numModifierVerts==0)
	{
		float whiteColor[3] = {1.0f,1.0f,1.0f};
		tupApplyVCModifier.AddColorVerts(whiteColor,1);
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//			"Fixed Rogue Apply VC Modifier - Reference %s - No Colors used - Defaulting to white color",
//			tupModifier.GetBaseAggregate()->GetName());
	}

	int *pAlphaIndices = NULL;
	int numAlphaIndices = 0;
	float *pModifierAlphaVerts = NULL;
	int numModifierAlphaVerts = 0;
	if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
	{
		tupApplyVCModifier.GetAlphaIndices(&pAlphaIndices,numAlphaIndices);
		tupApplyVCModifier.GetAlphaVerts((float **)&pModifierAlphaVerts,numModifierAlphaVerts);
		if (numModifierAlphaVerts==0)
		{
			float opaque = 1.0f;
			tupApplyVCModifier.AddAlphaVerts(&opaque,1);
//			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//				"Fixed Rogue Apply VC Modifier - Reference %s - No Alphas used - Defaulting to opaque",
//				tupModifier.GetBaseAggregate()->GetName());
		}
	}

	// if this flag is set the indices are in sync with the position verts
	if (flags & TupApplyVCModifier::FLAGS_MIXED_COLORS)
	{
		int numPositionVerts;
		TupVertex3f *pPositionVerts;
		tupBaseMesh.GetPositionVerts((float **) &pPositionVerts,numPositionVerts);
		// if these are not the same then fix
		if (numColorIndices!=numPositionVerts)
		{
//			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//				"Fixed Rogue Apply VC Modifier - Reference %s Color Using %d Points Needs to Use %d Points",
//				tupModifier.GetBaseAggregate()->GetName(),numColorIndices,numPositionVerts);

			TupArray<int> newColorIndices;
			newColorIndices.SetSize(numPositionVerts);
			for (int posVertIndex=0;posVertIndex<numPositionVerts;posVertIndex++)
			{
				if (posVertIndex<numColorIndices)
				{
					newColorIndices[posVertIndex] = pColorIndices[posVertIndex];
				}
				else
				{
					newColorIndices[posVertIndex] = 0; // use index 0 for invalid entry
				}
			}
			tupApplyVCModifier.AddColorIndices(newColorIndices.GetData(),numPositionVerts);
		}
		if (pAlphaIndices && numAlphaIndices!=numPositionVerts)
		{
//			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//				"Fixed Rogue Apply VC Modifier - Reference %s Alpha Using %d Points Needs to Use %d Points",
//				tupModifier.GetBaseAggregate()->GetName(),numAlphaIndices,numPositionVerts);

			TupArray<int> newAlphaIndices;
			newAlphaIndices.SetSize(numPositionVerts);
			for (int posVertIndex=0;posVertIndex<numPositionVerts;posVertIndex++)
			{
				if (posVertIndex<numAlphaIndices)
				{
					newAlphaIndices[posVertIndex] = pAlphaIndices[posVertIndex];
				}
				else
				{
					newAlphaIndices[posVertIndex] = 0; // use index 0 for invalid entry
				}
			}
			tupApplyVCModifier.AddAlphaIndices(newAlphaIndices.GetData(),numPositionVerts);
		}
	}
	// indices are per point per face
	else
	{
		// if these are not in line
		if (numColorIndices!=(numFaces<<1)+numFaces)
		{
//			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//				"Fixed Rogue Apply VC Modifier - Reference %s Color Using %d Points Needs to Use %d Points",
//				tupModifier.GetBaseAggregate()->GetName(),numColorIndices,(numFaces<<1)+numFaces);

			TupArray<int> newColorIndices;
			newColorIndices.SetSize((numFaces<<1)+numFaces);

			// fill the array with valid values
			for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int baseFaceIndex = (faceIndex<<1)+faceIndex;

				for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
				{
					if (baseFaceIndex+facePointIndex<numColorIndices)
					{
						newColorIndices[baseFaceIndex+facePointIndex] = pColorIndices[baseFaceIndex+facePointIndex];
					}
					else
					{
						newColorIndices[baseFaceIndex+facePointIndex] = 0; // use index 0 if not enough entries
					}
				}
			}
			tupApplyVCModifier.AddColorIndices(newColorIndices.GetData(),(numFaces<<1)+numFaces);
		}
		if (pAlphaIndices && numAlphaIndices!=(numFaces<<1)+numFaces)
		{
//			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//				"Fixed Rogue Apply VC Modifier - Reference %s Alpha Using %d Points Needs to Use %d Points",
//				tupModifier.GetBaseAggregate()->GetName(),numAlphaIndices,(numFaces<<1)+numFaces);

			TupArray<int> newAlphaIndices;
			newAlphaIndices.SetSize((numFaces<<1)+numFaces);

			// fill the array with valid values
			for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int baseFaceIndex = (faceIndex<<1)+faceIndex;

				for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
				{
					if (baseFaceIndex+facePointIndex<numAlphaIndices)
					{
						newAlphaIndices[baseFaceIndex+facePointIndex] = pAlphaIndices[baseFaceIndex+facePointIndex];
					}
					else
					{
						newAlphaIndices[baseFaceIndex+facePointIndex] = 0; // use index 0 if not enough entries
					}
				}
			}
			tupApplyVCModifier.AddAlphaIndices(newAlphaIndices.GetData(),(numFaces<<1)+numFaces);
		}
	}
}

void TupFixRogueModifiers::FixRogueVertexPaintModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupVertexPaintModifierContext tupVertexPaintModifierContext(pTupModifierContext);
	TupVertex3f *pModifierColorData;
	float *pModifierMixFactorData;
	int *pModifierFaceIndexData;
	int *pModifierPointIndexData;
	int numModifierColors;
	for (int channel = 0;channel<3;channel++)
	{
		switch (channel)
		{
		case 0:
			tupVertexPaintModifierContext.GetVertexColorChannelColorData((float **)&pModifierColorData,numModifierColors);
			tupVertexPaintModifierContext.GetVertexColorChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
			tupVertexPaintModifierContext.GetVertexColorChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
			tupVertexPaintModifierContext.GetVertexColorChannelPointIndexData(&pModifierPointIndexData,numModifierColors);
			break;
		case 1:
			tupVertexPaintModifierContext.GetAlphaChannelColorData((float **)&pModifierColorData,numModifierColors);
			tupVertexPaintModifierContext.GetAlphaChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
			tupVertexPaintModifierContext.GetAlphaChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
			tupVertexPaintModifierContext.GetAlphaChannelPointIndexData(&pModifierPointIndexData,numModifierColors);
			break;
		case 2:
			tupVertexPaintModifierContext.GetIlluminationChannelColorData((float **)&pModifierColorData,numModifierColors);
			tupVertexPaintModifierContext.GetIlluminationChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
			tupVertexPaintModifierContext.GetIlluminationChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
			tupVertexPaintModifierContext.GetIlluminationChannelPointIndexData(&pModifierPointIndexData,numModifierColors);
			break;
		}
		if (numModifierColors)
		{
			int numFaces;
			int *pPositionIndices;
			tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);
			// add only valid entries
			TupArray<TupVertex3f> newColorArray;
			TupArray<float> newMixFactorArray;
			TupArray<int> newFaceIndexArray;
			TupArray<int> newPointIndexArray;

			int maxFaceIndex = 0;
			int maxPointIndex = 0;
			bool bFoundBadEntry = false;

			// fill the arrays with entries that are valid
			for (int colorIndex = 0;colorIndex<numModifierColors;colorIndex++)
			{
				TupVertex3f &color = pModifierColorData[colorIndex];
				float mixFactor = pModifierMixFactorData[colorIndex];
				int faceIndex = pModifierFaceIndexData[colorIndex];
				int pointIndex = pModifierPointIndexData[colorIndex];
				if (faceIndex<numFaces && pointIndex>=0 && pointIndex<=2)
				{
					newColorArray.Add(color);
					newMixFactorArray.Add(mixFactor);
					newFaceIndexArray.Add(faceIndex);
					newPointIndexArray.Add(pointIndex);
				}
				else
				{
					bFoundBadEntry = true;
					if (maxFaceIndex<faceIndex)
					{
						maxFaceIndex = faceIndex;
					}
					if (maxPointIndex<pointIndex)
					{
						maxPointIndex=pointIndex;
					}
				}
			}
			if (bFoundBadEntry)
			{
				switch (channel)
				{
				case 0:
//					PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//						"Fixed Rogue Vertex Paint Modifier (Color Channel) - Reference %s Using %d Faces & %d Points Needs to Use %d Faces & 0-2 Points",
//						tupModifier.GetBaseAggregate()->GetName(),maxFaceIndex,maxPointIndex,numFaces);

					tupVertexPaintModifierContext.AddVertexColorChannelColorData((float *)newColorArray.GetData(),newColorArray.GetSize());
					tupVertexPaintModifierContext.AddVertexColorChannelMixFactorData(newMixFactorArray.GetData(),newMixFactorArray.GetSize());
					tupVertexPaintModifierContext.AddVertexColorChannelFaceIndexData(newFaceIndexArray.GetData(),newFaceIndexArray.GetSize());
					tupVertexPaintModifierContext.AddVertexColorChannelPointIndexData(newPointIndexArray.GetData(),newPointIndexArray.GetSize());
					break;
				case 1:
//					PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//						"Fixed Rogue Vertex Paint Modifier (Alpha Channel) - Reference %s Using %d Faces & %d Points Needs to Use %d Faces & 0-2 Points",
//						tupModifier.GetBaseAggregate()->GetName(),maxFaceIndex,maxPointIndex,numFaces);

					tupVertexPaintModifierContext.AddAlphaChannelColorData((float *)newColorArray.GetData(),newColorArray.GetSize());
					tupVertexPaintModifierContext.AddAlphaChannelMixFactorData(newMixFactorArray.GetData(),newMixFactorArray.GetSize());
					tupVertexPaintModifierContext.AddAlphaChannelFaceIndexData(newFaceIndexArray.GetData(),newFaceIndexArray.GetSize());
					tupVertexPaintModifierContext.AddAlphaChannelPointIndexData(newPointIndexArray.GetData(),newPointIndexArray.GetSize());
					break;
				case 2:
//					PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//						"Fixed Rogue Vertex Paint Modifier (Illumination Channel) - Reference %s Using %d Faces & %d Points Needs to Use %d Faces & 0-2 Points",
//						tupModifier.GetBaseAggregate()->GetName(),maxFaceIndex,maxPointIndex,numFaces);

					tupVertexPaintModifierContext.AddIlluminationChannelColorData((float *)newColorArray.GetData(),newColorArray.GetSize());
					tupVertexPaintModifierContext.AddIlluminationChannelMixFactorData(newMixFactorArray.GetData(),newMixFactorArray.GetSize());
					tupVertexPaintModifierContext.AddIlluminationChannelFaceIndexData(newFaceIndexArray.GetData(),newFaceIndexArray.GetSize());
					tupVertexPaintModifierContext.AddIlluminationChannelPointIndexData(newPointIndexArray.GetData(),newPointIndexArray.GetSize());
					break;
				}
			}
		}
	}
}

void TupFixRogueModifiers::FixRogueHoldVCModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupHoldVCModifier tupHoldVCModifier(tupModifier);
	int *pColorIndices;
	int numColorIndices;
	tupHoldVCModifier.GetColorIndices(&pColorIndices,numColorIndices);

	int numFaces;
	int *pPositionIndices;
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);

	// if these are not in line
	if (numColorIndices!=(numFaces<<1)+numFaces)
	{
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//			"Fixed Rogue Hold VC Modifier - Reference %s Using %d Indices Needs to Use %d Indices",
//			tupModifier.GetBaseAggregate()->GetName(),numColorIndices,(numFaces<<1)+numFaces);

		TupArray<int> newColorIndices;
		newColorIndices.SetSize((numFaces<<1)+numFaces);

		// fill the array with valid values
		for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			int baseFaceIndex = (faceIndex<<1)+faceIndex;

			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				if (baseFaceIndex+facePointIndex<numColorIndices)
				{
					newColorIndices[baseFaceIndex+facePointIndex] = pColorIndices[baseFaceIndex+facePointIndex];
				}
				else
				{
					newColorIndices[baseFaceIndex+facePointIndex] = 0; // use index 0 if not enough entries
				}
			}
		}
		tupHoldVCModifier.AddColorIndices(newColorIndices.GetData(),(numFaces<<1)+numFaces);
	}
}

void TupFixRogueModifiers::FixRogueFaceAttributeModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pTupModifierContext);
	int *pAttributeIndices;
	int numAttributeIndices;
	tupFaceAttributesModifierContext.GetFaceAttributeIndices(&pAttributeIndices,numAttributeIndices);

	int numFaces;
	int *pPositionIndices;
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);

	// if these are not in line
	if (numAttributeIndices!=numFaces)
	{

//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//			"Fixed Rogue Face Attribute Modifier - Reference %s Using %d Faces Needs to Use %d Faces",
//			tupModifier.GetBaseAggregate()->GetName(),numAttributeIndices,numFaces);

		TupArray<int> newAttributeIndices;
		newAttributeIndices.SetSize(numFaces);

		// fill the array with valid values
		for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			if (faceIndex<numAttributeIndices)
			{
				newAttributeIndices[faceIndex] = pAttributeIndices[faceIndex];
			}
			else
			{
				newAttributeIndices[faceIndex] = 0; // use index 0 if not enough entries
			}
		}
		tupFaceAttributesModifierContext.AddFaceAttributeIndices(newAttributeIndices.GetData(),numFaces);
	}
}

void TupFixRogueModifiers::FixRogueSkinModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupSkinModifierContext tupSkinModifierContext(pTupModifierContext);
	TupperwareAggregate *tupVertexPoolAgg = tupSkinModifierContext.GetVertexPool();
	TupSkinModifierContextVertexPool tupVertexPool(tupVertexPoolAgg);
	int numVertexPoolEntries = tupVertexPool.GetNumVertices();

	int numPositionVerts;
	TupVertex3f *pPositionVerts;
	tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);

	if (numVertexPoolEntries!=numPositionVerts)
	{
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//			"Fixed Rogue Skin Modifier - Reference %s Using %d Points Needs to Use %d Points",
//			tupModifier.GetBaseAggregate()->GetName(),numVertexPoolEntries,numPositionVerts);

		TupperwareAggregate *pNewTupVertexPoolAgg = new TupperwareAggregate;
		TupSkinModifierContextVertexPool tupNewVertexPool(pNewTupVertexPoolAgg);

		for (int positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
		{
			TupperwareAggregate *tupNewVertexPoolEntryAgg = tupNewVertexPool.AddVertex(positionVertIndex);
			if (positionVertIndex<numVertexPoolEntries)
			{
				TupperwareAggregate *tupVertexPoolEntryAgg = tupVertexPool.GetVertex(positionVertIndex);
				// copy stuff to new vertex
				for (TupperwareIterator iter = tupVertexPoolEntryAgg->FindAll(); !iter.Finished(); iter.Next()) 
				{
					tupNewVertexPoolEntryAgg->Copy(iter.GetAsAtom());
				}
			}
		}
		// replace data
		TupperwareAggregate *pAddedVertexPool = tupSkinModifierContext.AddVertexPool();
		for (TupperwareIterator iter = pNewTupVertexPoolAgg->FindAll(); !iter.Finished(); iter.Next()) 
		{
			pAddedVertexPool->Copy(iter.GetAsAtom());
		}
		delete pNewTupVertexPoolAgg;
	}
}

void TupFixRogueModifiers::FixRogueClothModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupClothModifierContext tupClothModifierContext(pTupModifierContext);

	int numPositionVerts;
	TupVertex3f *pPositionVerts;
	tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);

	int *pClothPointFlags;
	int numClothPoints;
	tupClothModifierContext.GetPointFlags(&pClothPointFlags,numClothPoints);
	int *pConnections;
	int numConnections;
	tupClothModifierContext.GetConnectionPoints(&pConnections,numConnections);

	if (numClothPoints!=numPositionVerts)
	{
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
//			"Fixed Rogue Cloth Modifier - Reference %s Using %d Points Needs to Use %d Points",
//			tupModifier.GetBaseAggregate()->GetName(),numClothPoints,numPositionVerts);
		TupArray<int> newClothPointFlags;
		newClothPointFlags.SetSize(numPositionVerts);
		TupArray<int> newConnections;

		for (int positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
		{
			if (positionVertIndex<numClothPoints)
			{
				newClothPointFlags[positionVertIndex] = pClothPointFlags[positionVertIndex];
			}
			else
			{
				newClothPointFlags[positionVertIndex] = TupClothModifierContext::FLAGS_POINT_ACTIVE | TupClothModifierContext::FLAGS_POINT_ATTACHED;
			}
		}

		for (int connectionIndex=0;connectionIndex<numConnections;connectionIndex++)
		{
			// only copy valid connections
			if (pConnections[connectionIndex<<1]<positionVertIndex && pConnections[(connectionIndex<<1)+1]<positionVertIndex)
			{
				newConnections.Add(pConnections[connectionIndex<<1]);
				newConnections.Add(pConnections[(connectionIndex<<1)+1]);
			}
		}
		tupClothModifierContext.AddPointFlags(newClothPointFlags.GetData(),numPositionVerts);
		tupClothModifierContext.AddConnectionPoints(newConnections.GetData(),newConnections.GetSize()>>1);
	}
}
