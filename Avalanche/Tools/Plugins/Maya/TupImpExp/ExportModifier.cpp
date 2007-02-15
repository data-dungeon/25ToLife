////////////////////////////////////////////////////////////////////////////
//
// ExportModifier
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportModifier.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 7/24/03    Time: 10:30a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * better triangle output
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vector3 issues
 * assert issues
 * add audio clips
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/16/03    Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * new consolidation code
 * start of getting prebound mesh
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 1/28/03    Time: 12:09p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vertex alpha added to apply vc modifier
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/18/02   Time: 12:13p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting face attribute modifiers
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 12/10/02   Time: 2:18p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * add bones that are not part of joint clusters
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/05/02   Time: 4:17p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix bone name
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/15/02   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vertex color modifiers
 * able to disable object consolidation
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/06/02   Time: 3:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of mesh stuff
 */

#include "TupImpExpPch.h"
#include "TupExport.h"
#include "SkinManager.h"
#include "MeshManager.h"

static void FixColor(Vector3 &newColor)
{
	if (newColor.x()<0.0f) newColor.x(0.0f);
	if (newColor.x()>1.0f) newColor.x(1.0f);
	if (newColor.y()<0.0f) newColor.y(0.0f);
	if (newColor.y()>1.0f) newColor.y(1.0f);
	if (newColor.z()<0.0f) newColor.z(0.0f);
	if (newColor.z()>1.0f) newColor.z(1.0f);
}

int TupExport::AddVertexColorModifier(int meshManagerIndex,int objectRef)
{
	if (!m_pMeshManager->MeshHasVertexColors(meshManagerIndex))
		return objectRef;

	int derivedObjectRef = m_pObjectPool->AddObjectAggregate();
	TupperwareAggregate *pDerivedObjectAgg = m_pObjectPool->GetObjectAggregate(derivedObjectRef);
	TupObject tupObject(pDerivedObjectAgg);
	tupObject.SetObjectType(TupObject::DERIVED);

	TupDerived tupDerived(pDerivedObjectAgg);
	tupDerived.AddObjectReference(objectRef);

	TupperwareAggregate *pDerivedModifierAgg = tupDerived.AddModifier(0);
	TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
	tupDerivedModifier.AddModifierContext();

	int modifierReference = m_pModifierPool->AddModifier();
	TupperwareAggregate *pModifierAgg = m_pModifierPool->GetModifier(modifierReference);
	TupModifier tupModifier(pModifierAgg);
	tupModifier.SetModifierType(TupModifier::APPLY_VC_MODIFIER);
	
	TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
	tupApplyVCModifier.AddName("ApplyVC");
	
	tupApplyVCModifier.AddColorVerts((const float *)m_pMeshManager->GetMeshVertexColorVertices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshVertexColorVertices(meshManagerIndex).GetSize());
	tupApplyVCModifier.AddColorIndices(m_pMeshManager->GetMeshVertexColorIndices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshVertexColorIndices(meshManagerIndex).GetSize());
	
	int flags = TupApplyVCModifier::FLAGS_ENABLED;
	
	// if there is alpha to do
	if (m_pMeshManager->GetMeshVertexAlphaVertices(meshManagerIndex).GetSize()>1)
	{
		flags |= TupApplyVCModifier::FLAGS_HAS_ALPHA;
		tupApplyVCModifier.AddAlphaVerts((const float *)m_pMeshManager->GetMeshVertexAlphaVertices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshVertexAlphaVertices(meshManagerIndex).GetSize());
		tupApplyVCModifier.AddAlphaIndices(m_pMeshManager->GetMeshVertexAlphaIndices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshVertexAlphaIndices(meshManagerIndex).GetSize());
	}
	tupApplyVCModifier.AddFlags(flags);

	tupDerivedModifier.AddModifierReference(modifierReference);
	return derivedObjectRef;
}


class BoneWeights
{
public:
	TupArray<int> m_bones;
	TupArray<float> m_weights;
};

int TupExport::AddSkinModifier(MDagPath mDagPath,int objectRef)
{
	if (mDagPath.apiType() != MFn::kMesh)
		return objectRef;

	MFnDependencyNode fnNode(mDagPath.node());
	int objectPtr = (int)MayaHelper::GetMPtrBase(fnNode);

	// check if this mesh has any bones attached to it
	int skinIndex = m_pSkinManager->GetSkinIndex(objectPtr);
	if (skinIndex==-1)
		return objectRef;

	int derivedObjectRef = m_pObjectPool->AddObjectAggregate();
	TupperwareAggregate *pDerivedObjectAgg = m_pObjectPool->GetObjectAggregate(derivedObjectRef);
	TupObject tupObject(pDerivedObjectAgg);
	tupObject.SetObjectType(TupObject::DERIVED);

	TupDerived tupDerived(pDerivedObjectAgg);
	tupDerived.AddObjectReference(objectRef);

	TupperwareAggregate *pDerivedModifierAgg = tupDerived.AddModifier(0);
	TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);

	// modifier
	int modifierIndex = m_pModifierPool->AddModifier();
	tupDerivedModifier.AddModifierReference(modifierIndex);
	TupperwareAggregate *pModifierAgg = m_pModifierPool->GetModifier(modifierIndex);
	TupModifier tupModifier(pModifierAgg);
	tupModifier.SetModifierType(TupModifier::SKIN_MODIFIER);

	TupSkinModifier tupSkinModifier(pModifierAgg);
	tupSkinModifier.AddName("Skin");
	int flags = TupSkinModifier::FLAGS_ENABLED;
	tupSkinModifier.AddFlags(flags);

	int numBones = m_pSkinManager->GetNumBones(skinIndex);
	const Graphics4x4 &meshMatrix = m_pSkinManager->GetMeshMatrix(skinIndex);

	int maxVertexIndex = 0;
	int boneIndex;
	for (boneIndex=0;boneIndex<numBones;boneIndex++)
	{
		TupperwareAggregate *pBoneAgg = tupSkinModifier.AddBone(boneIndex);
		TupSkinModifierBone tupSkinModifierBone(pBoneAgg);
		const SkinManager::BoneConnections &boneConnections = m_pSkinManager->GetBoneConnections(skinIndex,boneIndex);
		TupString boneName(boneConnections.m_boneName);
		FixBoneName(boneName);
		tupSkinModifierBone.AddName(boneName);
		int flags = TupSkinModifierBone::ABSOLUTE_WEIGHTS;
		tupSkinModifierBone.AddFlags(flags);
		tupSkinModifierBone.AddNodeReference(boneConnections.m_boneNodeRef);
		tupSkinModifierBone.AddBoneInitObjectTM(&boneConnections.m_boneMatrix.r00);
		// figure out the total number of vertices used for the skin
		int numComponents = boneConnections.m_vertexIndices.GetSize();
		for (int componentIndex=0;componentIndex<numComponents;componentIndex++)
		{
			int vertexIndex = boneConnections.m_vertexIndices[componentIndex];
			if (vertexIndex>maxVertexIndex)
				maxVertexIndex = vertexIndex;
		}
	}

	int vertexSize = maxVertexIndex+1;

	TupArray<BoneWeights> vertexBoneWeights;
	vertexBoneWeights.SetSize(vertexSize);
	for (boneIndex=0;boneIndex<numBones;boneIndex++)
	{
		const SkinManager::BoneConnections &boneConnections = m_pSkinManager->GetBoneConnections(skinIndex,boneIndex);

		// add the bones and weights to the verts in the array
		int numComponents = boneConnections.m_vertexIndices.GetSize();
		for (int componentIndex=0;componentIndex<numComponents;componentIndex++)
		{
			int vertexIndex = boneConnections.m_vertexIndices[componentIndex];
			vertexBoneWeights[vertexIndex].m_bones.Add(boneIndex);
			vertexBoneWeights[vertexIndex].m_weights.Add(boneConnections.m_weights[componentIndex]);
		}
	}

	// modifier context
	TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.AddModifierContext();
	TupSkinModifierContext tupSkinModifierContext(pModifierContextAgg);
	TupperwareAggregate *pVertexPoolAgg = tupSkinModifierContext.AddVertexPool();
	TupSkinModifierContextVertexPool tupSkinModifierContextVertexPool(pVertexPoolAgg);

	tupSkinModifierContext.AddBaseTM(&meshMatrix.r00);

	for (int vertexIndex = 0;vertexIndex<vertexSize;vertexIndex++)
	{
		TupperwareAggregate *pVertexAgg = tupSkinModifierContextVertexPool.AddVertex(vertexIndex);
		BoneWeights &boneWeights = vertexBoneWeights[vertexIndex];
		if (boneWeights.m_bones.GetSize())
		{
			TupSkinModifierContextVertex tupSkinModifierContextVertex(pVertexAgg);
			tupSkinModifierContextVertex.AddBones(boneWeights.m_bones.GetData(),boneWeights.m_bones.GetSize());
			tupSkinModifierContextVertex.AddWeights(boneWeights.m_weights.GetData(),boneWeights.m_weights.GetSize());
		}
	}
	return derivedObjectRef;
}


//-----------------------------------------------------------------------------
// FaceAttrController::StripAllButRawData
// This method will take the incoming string and strip out everything but the
// actual raw attribute data substring.
//
// Input:
//  TupString& strToFix -- String to strip.
//
// Output: True if both of the begin and end block indicators were found, false
//		   otherwise.
//-----------------------------------------------------------------------------
static bool StripAllButRawData(TupString &strToFix)
{
	strToFix.Remove('\r');
	// Find and remove the begin block indicator:
	int beginIndex = strToFix.Find(BEGIN_FACE_BLOCK);
	if (beginIndex == -1)
	{
		return false;
	}
	int newLineIndex = strToFix.Find('\n',beginIndex);
	if (newLineIndex == -1)
	{
		return false;
	}
	
	// Find and remove the end block indicator:
	int endIndex = strToFix.Find(END_FACE_BLOCK,newLineIndex);
	if (endIndex == -1)
	{
		return false;
	}
	TupString tempString(strToFix.Mid(newLineIndex+1,endIndex-(newLineIndex+1)));
	strToFix = tempString;
	return true;
}


int TupExport::AddFaceAttributeModifier(MDagPath mDagPath,int objectRef)
{
	MStatus status;

	if (mDagPath.apiType() != MFn::kMesh)
		return objectRef;

	MFnDependencyNode fnNode(mDagPath.node());

	MFnMesh fnMesh(mDagPath, &status);
	if (status != MS::kSuccess)
		return objectRef;

	int numPolys = fnMesh.numPolygons();
	if (numPolys<=0)
		return objectRef;

	// set up the array of ints that deals with the set per poly
	TupArray<int> polyFaceSets;
	polyFaceSets.SetSize(numPolys);
	int polyIndex;
	for (polyIndex = 0; polyIndex<numPolys; polyIndex ++ ) 
	{ 
		polyFaceSets[polyIndex] = 0; // start them all at 0
	}

	TupArray<TupString> faceAttributeVerts; // holds all the strings for all the different sets
	TupString noteString;

	MPlug notesPlug = fnNode.findPlug("notes",&status);
	if (status)
	{
		MString tempString;
		notesPlug.getValue(tempString);	
		noteString = tempString.asChar();
		noteString.Replace("\r\n","\n");
		noteString.Replace("\r","\n");
	}
	StripAllButRawData(noteString);
	int pos=0;
	// break up single string using [] and add to the faceAttributeVerts
	while (pos<noteString.GetLength())
	{
		int startIndex = noteString.Find('[',pos);
		if (startIndex != -1)
		{
			int endIndex = noteString.Find(']',startIndex);
			if (endIndex != -1)
			{
				faceAttributeVerts.Add(noteString.Mid(startIndex+1,endIndex-(startIndex+1)));
				pos=endIndex+1;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	// if there is nothing then no attributes for this mesh
	if (faceAttributeVerts.GetSize()==0)
	{
		return objectRef;
	}

	int numSets = faceAttributeVerts.GetSize();
	
	// get the blind data off the mesh
	if (fnMesh.isBlindDataTypeUsed( BLIND_DATA_ID ) && 
		fnMesh.hasBlindData(MFn::kMeshPolygonComponent,BLIND_DATA_ID))
	{
		MIntArray compIDArray;
		MIntArray compIntDataArray;
		status = fnMesh.getIntBlindData( MFn::kMeshPolygonComponent, BLIND_DATA_ID,BLIND_DATA_LONG_NAME,compIDArray,compIntDataArray); 
		if (status == MStatus::kSuccess)
		{
			int numCompAttribs = compIntDataArray.length();
			for (int compIndex=0;compIndex<numCompAttribs;compIndex++)
			{
				int compID = compIDArray[compIndex];
				int setIndex = compIntDataArray[compIndex];
				// make sure the set is in range
				if (setIndex<0 || setIndex>=numSets)
					setIndex = 0;
				if (compID>=0&&compID<numPolys) // make sure the compID is in range
				{
					polyFaceSets[compID] = setIndex;
				}
			}
		}
		else // if no attributes
		{
			return objectRef;
		}
	}
	else // if no attributes
	{
		return objectRef;
	}

	MItMeshPolygon polyIter(mDagPath, MObject::kNullObj, &status);
	if (status != MS::kSuccess) 
	{
		return objectRef;
	}

	TupArray<int> faceAttributeIndices;
	
	// Faces
	polyIndex = 0;
	for (polyIndex=0 ; !polyIter.isDone(); polyIter.next(),polyIndex++ )
	{
#ifdef POLYGON_EXPORT
		int polyAttribIndex = polyFaceSets[polyIndex];
		faceAttributeIndices.Add(polyAttribIndex);
#else
		int numTriangles;
		polyIter.numTriangles(numTriangles);
		int polyAttribIndex = polyFaceSets[polyIndex];
		for (int triangleIndex=0;triangleIndex<numTriangles;triangleIndex++)
		{
			faceAttributeIndices.Add(polyAttribIndex);
		}
#endif
	}
	
	int derivedObjectRef = m_pObjectPool->AddObjectAggregate();
	TupperwareAggregate *pDerivedObjectAgg = m_pObjectPool->GetObjectAggregate(derivedObjectRef);
	TupObject tupObject(pDerivedObjectAgg);
	tupObject.SetObjectType(TupObject::DERIVED);

	TupDerived tupDerived(pDerivedObjectAgg);
	tupDerived.AddObjectReference(objectRef);

	TupperwareAggregate *pDerivedModifierAgg = tupDerived.AddModifier(0);
	TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);

	// modifier
	int modifierIndex = m_pModifierPool->AddModifier();
	tupDerivedModifier.AddModifierReference(modifierIndex);
	TupperwareAggregate *pModifierAgg = m_pModifierPool->GetModifier(modifierIndex);
	TupModifier tupModifier(pModifierAgg);
	tupModifier.SetModifierType(TupModifier::FACE_ATTRIBUTE_MODIFIER);

	TupFaceAttributesModifier tupFaceAttributesModifier(pModifierAgg);
	tupFaceAttributesModifier.AddName("FaceAttributes");
	int flags = TupSkinModifier::FLAGS_ENABLED;
	tupFaceAttributesModifier.AddFlags(flags);

	// modifier context
	TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.AddModifierContext();
	TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pModifierContextAgg);
	tupFaceAttributesModifierContext.AddFaceAttributeIndices(faceAttributeIndices.GetData(),faceAttributeIndices.GetSize());
	tupFaceAttributesModifierContext.AddFaceAttributeVerts(faceAttributeVerts);
	
	return derivedObjectRef;
}
