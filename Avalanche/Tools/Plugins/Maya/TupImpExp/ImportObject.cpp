////////////////////////////////////////////////////////////////////////////
//
// ImportObject
//
// Tupperware import
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportObject.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/15/03    Time: 3:38p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * uv chooser connected
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/26/03    Time: 4:34p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * creating shaders
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export vertex colors (from apply vc modifier)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/13/03    Time: 4:52p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import nodes and polymeshes
 */

#include "TupImpExpPch.h"
#include "TupImport.h"
#include "ShaderManager.h"
#include "JointManager.h"

static const float SCALEFACTOR = 100.0f;

struct FaceVertexNormals
{
	int m_faceIndex;
	int m_normalIndex;
};

void TupImport::ImportObject(int objectRef,MObject parentObject)
{
	// get the vertex colors from a modifier
	TupArray<TupVertex3f> vertexColorVerts;
	TupArray<int> vertexColorIndices;
	TupArray<float> vertexAlphaVerts;
	TupArray<int> vertexAlphaIndices;
	bool bMixedColors;
	TupObjectHelper::FindVertexColors(*m_pObjectPool,objectRef,*m_pModifierPool,vertexColorVerts,vertexColorIndices,vertexAlphaVerts,vertexAlphaIndices,bMixedColors);

	TupArray<int> faceAttributesIndices;
	TupArray<TupString> faceAttributesVerts;
	TupObjectHelper::FindFaceAttributes(*m_pObjectPool,objectRef,*m_pModifierPool,faceAttributesVerts,faceAttributesIndices);

	int baseObjectRef = TupObjectHelper::FindBaseObjectRef(*m_pObjectPool,objectRef);
	TupperwareAggregate *pObjectAgg = m_pObjectPool->GetObjectAggregate(baseObjectRef);
	assert(pObjectAgg);

	TupObject tupObject(pObjectAgg);
	switch (tupObject.GetObjectType())
	{
		case TupObject::POLY_MESH:
			ImportPolyMesh(pObjectAgg,parentObject,
								vertexColorVerts,vertexColorIndices,vertexAlphaVerts,vertexAlphaIndices,bMixedColors,
								faceAttributesVerts,faceAttributesIndices);
			break;
		case TupObject::TRI_MESH:
			ImportTriMesh(pObjectAgg,parentObject,
							  vertexColorVerts,vertexColorIndices,vertexAlphaVerts,vertexAlphaIndices,bMixedColors,
							  faceAttributesVerts,faceAttributesIndices);
			break;
		case TupObject::SPLINE_SHAPE:
			ImportSplineShape(pObjectAgg,parentObject);
			break;
		case TupObject::BEZIER_SHAPE:
			ImportBezierShape(pObjectAgg,parentObject);
			break;
		case TupObject::LIGHT:
			ImportLight(pObjectAgg,parentObject);
			break;
		case TupObject::LOCATOR:
			ImportLocator(pObjectAgg,parentObject);
			break;
		case TupObject::BONE:
			// done in the node
			break;
	}
}

void TupImport::ImportPolyMesh(TupperwareAggregate *pObjectAgg,MObject parentObject,
										 const TupArray<TupVertex3f> &colorVerts,const TupArray<int> &colorIndices,
										 const TupArray<float> &alphaVerts,const TupArray<int> &alphaIndices,bool bMixedColors,
										 const TupArray<TupString> &faceAttributesVerts,const TupArray<int> &faceAttributesIndices)
{
	TupPolyMesh tupPolyMesh(pObjectAgg);

	int numIndices;
	int numFaces;
	
	int numPositionVerts;
	TupVertex3f *pPositionVerts;
	int *pPositionIndices;
	
	int numNormalVerts;
	TupVertex3f *pNormalVerts;
	int *pNormalIndices;
	
	int *pFaceVertexCounts;
	
	int numFaceNormalVerts;
	TupVertex3f *pFaceNormalVerts;
	int *pFaceNormalIndices;
	
	int *pFaceFlags;
	int *pFaceMaterialRefs;
	
	tupPolyMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupPolyMesh.GetPositionIndices(&pPositionIndices,numIndices);
	tupPolyMesh.GetNormalVerts((float **)&pNormalVerts,numNormalVerts);
	tupPolyMesh.GetNormalIndices(&pNormalIndices,numIndices);
	tupPolyMesh.GetFaceVertexCounts(&pFaceVertexCounts,numFaces);
	tupPolyMesh.GetFaceNormalVerts((float **)&pFaceNormalVerts,numFaceNormalVerts);
	tupPolyMesh.GetFaceNormalIndices(&pFaceNormalIndices,numFaces);
	tupPolyMesh.GetFaceFlags(&pFaceFlags,numFaces);
	tupPolyMesh.GetFaceMaterialRefs(&pFaceMaterialRefs,numFaces);

	MFnMesh mfnMesh;

	MPointArray positionVerts;
	positionVerts.setLength(numPositionVerts);
	for (int posIndex=0;posIndex<numPositionVerts;posIndex++)
	{
		MPoint newPoint(pPositionVerts[posIndex].x*SCALEFACTOR,pPositionVerts[posIndex].y*SCALEFACTOR,pPositionVerts[posIndex].z*SCALEFACTOR);
		positionVerts[posIndex] = newPoint;
	}

	MIntArray faceVertexCounts(pFaceVertexCounts,numFaces);
	MIntArray positionIndices(pPositionIndices,numIndices);

	MStatus status;
	MObject polyMeshObject = mfnMesh.create( numPositionVerts, numFaces,positionVerts,faceVertexCounts,positionIndices,parentObject, &status );
	MFnDagNode mfnDagNode(polyMeshObject);
	MDagPath polyDagPath;
	mfnDagNode.getPath(polyDagPath);
	mfnMesh.setObject(polyDagPath);

	// set the dag path to this mesh if the joint manager needs it
	int skinIndex = m_pJointManager->GetSkinIndex(atoi(pObjectAgg->GetName()));
	if (skinIndex!=-1)
	{
		m_pJointManager->SetMeshDagPathForSkin(skinIndex,polyDagPath);
	}

	TupArray<TupArray<FaceVertexNormals> > positionVertsFaceNormals;
	positionVertsFaceNormals.SetSize(numPositionVerts);

	int polyIndex;
	int indicesIndex=0;
	// keep track of which faces are using which position as well as which normal
	for (polyIndex=0;polyIndex<numFaces;polyIndex++)
	{
		for (int vertIndex=0;vertIndex<pFaceVertexCounts[polyIndex];vertIndex++)
		{
			int positionIndex = pPositionIndices[indicesIndex];
			FaceVertexNormals faceVertexNormals;
			faceVertexNormals.m_faceIndex = polyIndex;
			faceVertexNormals.m_normalIndex = pNormalIndices[indicesIndex];
			positionVertsFaceNormals[positionIndex].Add(faceVertexNormals);
			indicesIndex++;
		}
	}

	MVectorArray sharedNormals;
	MIntArray sharedNormalPositionIndices;

	MVectorArray nonSharedNormals;
	MIntArray nonSharedNormalFaces;
	MIntArray nonSharedNormalPositionIndices;

	for (int positionIndex=0;positionIndex<numPositionVerts;positionIndex++)
	{
		int numFacesForVert = positionVertsFaceNormals[positionIndex].GetSize();
		if (numFacesForVert)
		{
			bool bSharedNormal = true;
			int firstFaceNormalIndex = positionVertsFaceNormals[positionIndex][0].m_normalIndex;
			for (int facePointIndex=1;facePointIndex<numFacesForVert;facePointIndex++)
			{
				if (positionVertsFaceNormals[positionIndex][facePointIndex].m_normalIndex != firstFaceNormalIndex)
				{
					bSharedNormal = false;
					break;
				}
			}
			if (bSharedNormal)
			{
				TupVertex3f &normal = pNormalVerts[firstFaceNormalIndex];
				MVector newVector(normal.x,normal.y,normal.z);
				sharedNormals.append(newVector);
				sharedNormalPositionIndices.append(positionIndex);
			}
			else
			{
				for (int facePointIndex=0;facePointIndex<numFacesForVert;facePointIndex++)
				{
					TupVertex3f &normal = pNormalVerts[positionVertsFaceNormals[positionIndex][facePointIndex].m_normalIndex];
					MVector newVector(normal.x,normal.y,normal.z);
					nonSharedNormals.append(newVector);
					nonSharedNormalFaces.append(positionVertsFaceNormals[positionIndex][facePointIndex].m_faceIndex);
					nonSharedNormalPositionIndices.append(positionIndex);
				}
			}
		}
	}
	int numSharedNormals = sharedNormals.length();
	int numNonSharedNormals = nonSharedNormals.length();

	// do the shared normals
	if (sharedNormals.length())
	{
		status = mfnMesh.setVertexNormals(sharedNormals, sharedNormalPositionIndices, MSpace::kTransform);
		if (status!=MStatus::kSuccess)
		{
			MGlobal::displayError("Failed to set shared vertex normals.");
		}
	}

	// do the non shared normals
	if (nonSharedNormals.length())
	{
		status = mfnMesh.setFaceVertexNormals(nonSharedNormals, nonSharedNormalFaces, nonSharedNormalPositionIndices, MSpace::kTransform);
		if (status!=MStatus::kSuccess)
		{
			MGlobal::displayError("Failed to set face vertex normals.");
		}
	}

	MIntArray faceList;
	faceList.setLength(numIndices);
	indicesIndex=0;
	for (int polyIndex=0;polyIndex<numFaces;polyIndex++)
	{
		for (int vertIndex=0;vertIndex<pFaceVertexCounts[polyIndex];vertIndex++)
		{
			faceList[indicesIndex] = polyIndex;
			indicesIndex++;
		}
	}

	// if this has colors
	if (colorVerts.GetSize())
	{
		if (bMixedColors) // inline with position
		{
			MColorArray colorArray;
			colorArray.setLength(numIndices);
			for (int vertexColorIndex=0;vertexColorIndex<numIndices;vertexColorIndex++)
			{
				const TupVertex3f &color = colorVerts[colorIndices[pPositionIndices[vertexColorIndex]]];
				float alpha = alphaVerts[alphaIndices[pPositionIndices[vertexColorIndex]]];
				MColor mColor(color.x,color.y,color.z,alpha);
				colorArray[vertexColorIndex] = mColor;
			}
			status = mfnMesh.setFaceVertexColors(colorArray,faceList,positionIndices);
			if (status!=MStatus::kSuccess)
			{
				MGlobal::displayError("Failed to set face vertex colors.");
			}
		}
		else // per point per face
		{
			MColorArray colorArray;
			colorArray.setLength(numIndices);
			for (int vertexColorIndex=0;vertexColorIndex<numIndices;vertexColorIndex++)
			{
				const TupVertex3f &color = colorVerts[colorIndices[vertexColorIndex]];
				float alpha = alphaVerts[alphaIndices[vertexColorIndex]];
				MColor mColor(color.x,color.y,color.z,alpha);
				colorArray[vertexColorIndex] = mColor;
			}
			status = mfnMesh.setFaceVertexColors(colorArray,faceList,positionIndices);
			if (status!=MStatus::kSuccess)
			{
				MGlobal::displayError("Failed to set face vertex colors.");
			}
		}
	}

	// set uv coordinates
	int numMeshMaps = tupPolyMesh.GetNumMeshMaps();
	for (int meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		TupperwareAggregate *pMeshMapAgg = tupPolyMesh.GetMeshMap(meshMapIndex);
		TupPolyMeshMap tupPolyMeshMap(pMeshMapAgg);
		int mapChannel = tupPolyMeshMap.GetMapChannel();
		if (mapChannel>=TupPolyMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
		{
			char *pUVSetName;
			tupPolyMeshMap.GetMapName(&pUVSetName);
			TupString uvSetName;
			if (pUVSetName)
			{
				uvSetName = pUVSetName;
				uvSetName.Replace(' ','_');
			}
			else
			{
				uvSetName.Format("map%d",mapChannel);
			}
			MString uvSet(uvSetName);
			if (mapChannel==TupPolyMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
			{
				status = mfnMesh.renameUVSet(MString("map1"),uvSet);
				status = mfnMesh.setCurrentUVSetName(uvSet);
				if (status != MStatus::kSuccess)
				{
					MGlobal::displayError("Failed set current uv set name.");
				}
			}
			else
			{
				status  = mfnMesh.createUVSet(uvSet);
				if (status != MStatus::kSuccess)
				{
					MGlobal::displayError("Failed to create uv set.");
				}
				status = mfnMesh.setCurrentUVSetName(uvSet);
				if (status != MStatus::kSuccess)
				{
					MGlobal::displayError("Failed set current uv set name.");
				}
			}

			TupVertex3f *pMapVerts;
			int numMapVerts;
			int *pMapIndices;
			int numMapIndices;

			tupPolyMeshMap.GetMapVerts((float**) &pMapVerts,numMapVerts);
			tupPolyMeshMap.GetMapIndices(&pMapIndices,numMapIndices);

			MFloatArray uArray;
			MFloatArray vArray;
			MIntArray uvIds(pMapIndices,numMapIndices);

			uArray.setLength(numMapVerts);
			vArray.setLength(numMapVerts);

			for (int uvIndex=0;uvIndex<numMapVerts;uvIndex++)
			{
				uArray[uvIndex] = pMapVerts[uvIndex].x;
				vArray[uvIndex] = 1.0f-pMapVerts[uvIndex].y;
			}
			status = mfnMesh.setUVs( uArray, vArray ,&uvSet);
			if (status!=MStatus::kSuccess)
			{
				MGlobal::displayError("Failed to set mesh uvs.");
			}
			status = mfnMesh.assignUVs( faceVertexCounts, uvIds ,&uvSet);
			if (status!=MStatus::kSuccess)
			{
				MGlobal::displayError("Failed to assign mesh uvs.");
			}
		}
	}
	MString defaultUVSet("map1");
	status = mfnMesh.setCurrentUVSetName(defaultUVSet);
	if (status != MStatus::kSuccess)
	{
		MGlobal::displayError("Failed set current uv set name.");
	}

	// face attributes
	if (faceAttributesVerts.GetSize())
	{
		MStringArray longNames,shortNames,formatNames;
		longNames.append( BLIND_DATA_LONG_NAME ); 
		shortNames.append( BLIND_DATA_SHORT_NAME ); 
		formatNames.append( BLIND_DATA_FORMAT_NAME ); 

		TupString strFullData;
		strFullData += BEGIN_FACE_BLOCK;
		strFullData += "\r";
		int numFaceAttributeVerts = faceAttributesVerts.GetSize();
		for (int vertIndex=0;vertIndex<numFaceAttributeVerts;vertIndex++)
		{
			strFullData += "[";
			strFullData += faceAttributesVerts[vertIndex];
			strFullData += "]\r";
		}
		strFullData += END_FACE_BLOCK;
		strFullData += "\r";

		AddNote(polyMeshObject,strFullData);						

		// place the ints on the mesh faces
		if (!mfnMesh.isBlindDataTypeUsed(BLIND_DATA_ID))
		{
			status = mfnMesh.createBlindDataType ( BLIND_DATA_ID,longNames,shortNames,formatNames) ;
		}
		int numIndices = faceAttributesIndices.GetSize();
		MIntArray polyArray;
		MIntArray polyDataArray;
		for (int faceIndex=0;faceIndex<numIndices;faceIndex++)
		{
			// if set to something
			if (faceAttributesIndices[faceIndex]!=0)
			{
				polyArray.append(faceIndex);
				polyDataArray.append(faceAttributesIndices[faceIndex]);
			}
		}
		if (polyArray.length()) // if any faces changed
		{
			status = mfnMesh.setIntBlindData( polyArray, MFn::kMeshPolygonComponent,BLIND_DATA_ID,BLIND_DATA_LONG_NAME,polyDataArray); 
		}
	}
#ifdef DEFER_MESH_SHADER_APPLICATION
	m_pShaderManager->AddMeshShaderData(polyDagPath,pFaceMaterialRefs,numFaces);
#else
	m_pShaderManager->SetUpMeshShaders(polyDagPath,pFaceMaterialRefs,numFaces);
#endif
}

static unsigned char TriStripColors[768] =
{
	0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 
	0x00, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xdc, 0xc0, 0xa6, 0xca, 0xf0, 0x2a, 0x3f, 
	0xaa, 0x2a, 0x3f, 0xff, 0x2a, 0x5f, 0x00, 0x2a, 0x5f, 0x55, 0x2a, 0x5f, 0xaa, 0x2a, 0x5f, 0xff, 
	0x2a, 0x7f, 0x00, 0x2a, 0x7f, 0x55, 0x2a, 0x7f, 0xaa, 0x2a, 0x7f, 0xff, 0x2a, 0x9f, 0x00, 0x2a, 
	0x9f, 0x55, 0x2a, 0x9f, 0xaa, 0x2a, 0x9f, 0xff, 0x2a, 0xbf, 0x00, 0x2a, 0xbf, 0x55, 0x2a, 0xbf, 
	0xaa, 0x2a, 0xbf, 0xff, 0x2a, 0xdf, 0x00, 0x2a, 0xdf, 0x55, 0x2a, 0xdf, 0xaa, 0x2a, 0xdf, 0xff, 
	0x2a, 0xff, 0x00, 0x2a, 0xff, 0x55, 0x2a, 0xff, 0xaa, 0x2a, 0xff, 0xff, 0x55, 0x00, 0x00, 0x55, 
	0x00, 0x55, 0x55, 0x00, 0xaa, 0x55, 0x00, 0xff, 0x55, 0x1f, 0x00, 0x55, 0x1f, 0x55, 0x55, 0x1f, 
	0xaa, 0x55, 0x1f, 0xff, 0x55, 0x3f, 0x00, 0x55, 0x3f, 0x55, 0x55, 0x3f, 0xaa, 0x55, 0x3f, 0xff, 
	0x55, 0x5f, 0x00, 0x55, 0x5f, 0x55, 0x55, 0x5f, 0xaa, 0x55, 0x5f, 0xff, 0x55, 0x7f, 0x00, 0x55, 
	0x7f, 0x55, 0x55, 0x7f, 0xaa, 0x55, 0x7f, 0xff, 0x55, 0x9f, 0x00, 0x55, 0x9f, 0x55, 0x55, 0x9f, 
	0xaa, 0x55, 0x9f, 0xff, 0x55, 0xbf, 0x00, 0x55, 0xbf, 0x55, 0x55, 0xbf, 0xaa, 0x55, 0xbf, 0xff, 
	0x55, 0xdf, 0x00, 0x55, 0xdf, 0x55, 0x55, 0xdf, 0xaa, 0x55, 0xdf, 0xff, 0x55, 0xff, 0x00, 0x55, 
	0xff, 0x55, 0x55, 0xff, 0xaa, 0x55, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x7f, 0x00, 0x55, 0x7f, 0x00, 
	0xaa, 0x7f, 0x00, 0xff, 0x7f, 0x1f, 0x00, 0x7f, 0x1f, 0x55, 0x7f, 0x1f, 0xaa, 0x7f, 0x1f, 0xff, 
	0x7f, 0x3f, 0x00, 0x7f, 0x3f, 0x55, 0x7f, 0x3f, 0xaa, 0x7f, 0x3f, 0xff, 0x7f, 0x5f, 0x00, 0x7f, 
	0x5f, 0x55, 0x7f, 0x5f, 0xaa, 0x7f, 0x5f, 0xff, 0x7f, 0x7f, 0x00, 0x7f, 0x7f, 0x55, 0x7f, 0x7f, 
	0xaa, 0x7f, 0x7f, 0xff, 0x7f, 0x9f, 0x00, 0x7f, 0x9f, 0x55, 0x7f, 0x9f, 0xaa, 0x7f, 0x9f, 0xff, 
	0x7f, 0xbf, 0x00, 0x7f, 0xbf, 0x55, 0x7f, 0xbf, 0xaa, 0x7f, 0xbf, 0xff, 0x7f, 0xdf, 0x00, 0x7f, 
	0xdf, 0x55, 0x7f, 0xdf, 0xaa, 0x7f, 0xdf, 0xff, 0x7f, 0xff, 0x00, 0x7f, 0xff, 0x55, 0x7f, 0xff, 
	0xaa, 0x7f, 0xff, 0xff, 0xaa, 0x00, 0x00, 0xaa, 0x00, 0x55, 0xaa, 0x00, 0xaa, 0xaa, 0x00, 0xff, 
	0xaa, 0x1f, 0x00, 0xaa, 0x1f, 0x55, 0xaa, 0x1f, 0xaa, 0xaa, 0x1f, 0xff, 0xaa, 0x3f, 0x00, 0xaa, 
	0x3f, 0x55, 0xaa, 0x3f, 0xaa, 0xaa, 0x3f, 0xff, 0xaa, 0x5f, 0x00, 0xaa, 0x5f, 0x55, 0xaa, 0x5f, 
	0xaa, 0xaa, 0x5f, 0xff, 0xaa, 0x7f, 0x00, 0xaa, 0x7f, 0x55, 0xaa, 0x7f, 0xaa, 0xaa, 0x7f, 0xff, 
	0xaa, 0x9f, 0x00, 0xaa, 0x9f, 0x55, 0xaa, 0x9f, 0xaa, 0xaa, 0x9f, 0xff, 0xaa, 0xbf, 0x00, 0xaa, 
	0xbf, 0x55, 0xaa, 0xbf, 0xaa, 0xaa, 0xbf, 0xff, 0xaa, 0xdf, 0x00, 0xaa, 0xdf, 0x55, 0xaa, 0xdf, 
	0xaa, 0xaa, 0xdf, 0xff, 0xaa, 0xff, 0x00, 0xaa, 0xff, 0x55, 0xaa, 0xff, 0xaa, 0xaa, 0xff, 0xff, 
	0xd4, 0x00, 0x00, 0xd4, 0x00, 0x55, 0xd4, 0x00, 0xaa, 0xd4, 0x00, 0xff, 0xd4, 0x1f, 0x00, 0xd4, 
	0x1f, 0x55, 0xd4, 0x1f, 0xaa, 0xd4, 0x1f, 0xff, 0xd4, 0x3f, 0x00, 0xd4, 0x3f, 0x55, 0xd4, 0x3f, 
	0xaa, 0xd4, 0x3f, 0xff, 0xd4, 0x5f, 0x00, 0xd4, 0x5f, 0x55, 0xd4, 0x5f, 0xaa, 0xd4, 0x5f, 0xff, 
	0xd4, 0x7f, 0x00, 0xd4, 0x7f, 0x55, 0xd4, 0x7f, 0xaa, 0xd4, 0x7f, 0xff, 0xd4, 0x9f, 0x00, 0xd4, 
	0x9f, 0x55, 0xd4, 0x9f, 0xaa, 0xd4, 0x9f, 0xff, 0xd4, 0xbf, 0x00, 0xd4, 0xbf, 0x55, 0xd4, 0xbf, 
	0xaa, 0xd4, 0xbf, 0xff, 0xd4, 0xdf, 0x00, 0xd4, 0xdf, 0x55, 0xd4, 0xdf, 0xaa, 0xd4, 0xdf, 0xff, 
	0xd4, 0xff, 0x00, 0xd4, 0xff, 0x55, 0xd4, 0xff, 0xaa, 0xd4, 0xff, 0xff, 0xff, 0x00, 0x55, 0xff, 
	0x00, 0xaa, 0xff, 0x1f, 0x00, 0xff, 0x1f, 0x55, 0xff, 0x1f, 0xaa, 0xff, 0x1f, 0xff, 0xff, 0x3f, 
	0x00, 0xff, 0x3f, 0x55, 0xff, 0x3f, 0xaa, 0xff, 0x3f, 0xff, 0xff, 0x5f, 0x00, 0xff, 0x5f, 0x55, 
	0xff, 0x5f, 0xaa, 0xff, 0x5f, 0xff, 0xff, 0x7f, 0x00, 0xff, 0x7f, 0x55, 0xff, 0x7f, 0xaa, 0xff, 
	0x7f, 0xff, 0xff, 0x9f, 0x00, 0xff, 0x9f, 0x55, 0xff, 0x9f, 0xaa, 0xff, 0x9f, 0xff, 0xff, 0xbf, 
	0x00, 0xff, 0xbf, 0x55, 0xff, 0xbf, 0xaa, 0xff, 0xbf, 0xff, 0xff, 0xdf, 0x00, 0xff, 0xdf, 0x55, 
	0xff, 0xdf, 0xaa, 0xff, 0xdf, 0xff, 0xff, 0xff, 0x55, 0xff, 0xff, 0xaa, 0xcc, 0xcc, 0xff, 0xff, 
	0xcc, 0xff, 0x33, 0xff, 0xff, 0x66, 0xff, 0xff, 0x99, 0xff, 0xff, 0xcc, 0xff, 0xff, 0x00, 0x7f, 
	0x00, 0x00, 0x7f, 0x55, 0x00, 0x7f, 0xaa, 0x00, 0x7f, 0xff, 0x00, 0x9f, 0x00, 0x00, 0x9f, 0x55, 
	0x00, 0x9f, 0xaa, 0x00, 0x9f, 0xff, 0x00, 0xbf, 0x00, 0x00, 0xbf, 0x55, 0x00, 0xbf, 0xaa, 0x00, 
	0xbf, 0xff, 0x00, 0xdf, 0x00, 0x00, 0xdf, 0x55, 0x00, 0xdf, 0xaa, 0x00, 0xdf, 0xff, 0x00, 0xff, 
	0x55, 0x00, 0xff, 0xaa, 0x2a, 0x00, 0x00, 0x2a, 0x00, 0x55, 0x2a, 0x00, 0xaa, 0x2a, 0x00, 0xff, 
	0x2a, 0x1f, 0x00, 0x2a, 0x1f, 0x55, 0x2a, 0x1f, 0xaa, 0x2a, 0x1f, 0xff, 0x2a, 0x3f, 0x00, 0x2a, 
	0x3f, 0x55, 0xff, 0xfb, 0xf0, 0xa0, 0xa0, 0xa4, 0x80, 0x80, 0x80, 0xff, 0x00, 0x00, 0x00, 0xff, 
	0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff
};

void TupImport::ImportTriMesh(TupperwareAggregate *pObjectAgg,MObject parentObject,
										 const TupArray<TupVertex3f> &colorVerts,const TupArray<int> &colorIndices,
										 const TupArray<float> &alphaVerts,const TupArray<int> &alphaIndices,bool bMixedColors,
										 const TupArray<TupString> &faceAttributesVerts,const TupArray<int> &faceAttributesIndices)
{
	TupTriMesh tupTriMesh(pObjectAgg);

	int numIndices;
	int numFaces;

	int numPositionVerts;
	TupVertex3f *pPositionVerts;
	int *pPositionIndices;

	int numNormalVerts;
	TupVertex3f *pNormalVerts;
	int *pNormalIndices;

	int numFaceNormalVerts;
	TupVertex3f *pFaceNormalVerts;
	int *pFaceNormalIndices;

	int *pFaceFlags;
	int *pFaceMaterialRefs;

	tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupTriMesh.GetPositionIndices(&pPositionIndices,numFaces);
	tupTriMesh.GetNormalVerts((float **)&pNormalVerts,numNormalVerts);
	tupTriMesh.GetNormalIndices(&pNormalIndices,numFaces);
	tupTriMesh.GetFaceNormalVerts((float **)&pFaceNormalVerts,numFaceNormalVerts);
	tupTriMesh.GetFaceNormalIndices(&pFaceNormalIndices,numFaces);
	tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);
	tupTriMesh.GetFaceMaterialRefs(&pFaceMaterialRefs,numFaces);
	numIndices = numFaces*3;

	MFnMesh mfnMesh;

	MPointArray positionVerts;
	positionVerts.setLength(numPositionVerts);
	for (int posIndex=0;posIndex<numPositionVerts;posIndex++)
	{
		MPoint newPoint(pPositionVerts[posIndex].x*SCALEFACTOR,pPositionVerts[posIndex].y*SCALEFACTOR,pPositionVerts[posIndex].z*SCALEFACTOR);
		positionVerts[posIndex] = newPoint;
	}

	// set up face vertex counts to be triangles
	MIntArray faceVertexCounts;
	faceVertexCounts.setLength(numFaces);
	for (int faceVertexCountIndex = 0; faceVertexCountIndex < numFaces; faceVertexCountIndex++)
	{
		faceVertexCounts[faceVertexCountIndex] = 3;
	}

	MIntArray positionIndices(pPositionIndices,numIndices);

	MStatus status;
	MObject polyMeshObject = mfnMesh.create( numPositionVerts, numFaces,positionVerts,faceVertexCounts,positionIndices,parentObject, &status );
	MFnDagNode mfnDagNode(polyMeshObject);
	MDagPath polyDagPath;
	mfnDagNode.getPath(polyDagPath);
	mfnMesh.setObject(polyDagPath);

	// set the dag path to this mesh if the joint manager needs it
	int skinIndex = m_pJointManager->GetSkinIndex(atoi(pObjectAgg->GetName()));
	if (skinIndex!=-1)
	{
		m_pJointManager->SetMeshDagPathForSkin(skinIndex,polyDagPath);
	}

	TupArray<TupArray<FaceVertexNormals> > positionVertsFaceNormals;
	positionVertsFaceNormals.SetSize(numPositionVerts);

	int faceIndex;
	int faceIndex3;
	// keep track of which faces are using which position as well as which normal
	for (faceIndex=0,faceIndex3=0;faceIndex<numFaces;faceIndex++,faceIndex3+=3)
	{
		for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
		{
			int positionIndex = pPositionIndices[faceIndex3+facePointIndex];
			FaceVertexNormals faceVertexNormals;
			faceVertexNormals.m_faceIndex = faceIndex;
			faceVertexNormals.m_normalIndex = pNormalIndices[faceIndex3+facePointIndex];
			positionVertsFaceNormals[positionIndex].Add(faceVertexNormals);
		}
	}

	MVectorArray sharedNormals;
	MIntArray sharedNormalPositionIndices;

	MVectorArray nonSharedNormals;
	MIntArray nonSharedNormalFaces;
	MIntArray nonSharedNormalPositionIndices;

	for (int positionIndex=0;positionIndex<numPositionVerts;positionIndex++)
	{
		int numFacesForVert = positionVertsFaceNormals[positionIndex].GetSize();
		if (numFacesForVert)
		{
			bool bSharedNormal = true;
			int firstFaceNormalIndex = positionVertsFaceNormals[positionIndex][0].m_normalIndex;
			for (int facePointIndex=1;facePointIndex<numFacesForVert;facePointIndex++)
			{
				if (positionVertsFaceNormals[positionIndex][facePointIndex].m_normalIndex != firstFaceNormalIndex)
				{
					bSharedNormal = false;
					break;
				}
			}
			if (bSharedNormal)
			{
				TupVertex3f &normal = pNormalVerts[firstFaceNormalIndex];
				MVector newVector(normal.x,normal.y,normal.z);
				sharedNormals.append(newVector);
				sharedNormalPositionIndices.append(positionIndex);
			}
			else
			{
				for (int facePointIndex=0;facePointIndex<numFacesForVert;facePointIndex++)
				{
					TupVertex3f &normal = pNormalVerts[positionVertsFaceNormals[positionIndex][facePointIndex].m_normalIndex];
					MVector newVector(normal.x,normal.y,normal.z);
					nonSharedNormals.append(newVector);
					nonSharedNormalFaces.append(positionVertsFaceNormals[positionIndex][facePointIndex].m_faceIndex);
					nonSharedNormalPositionIndices.append(positionIndex);
				}
			}
		}
	}
	int numSharedNormals = sharedNormals.length();
	int numNonSharedNormals = nonSharedNormals.length();

	// do the shared normals
	if (sharedNormals.length())
	{
		status = mfnMesh.setVertexNormals(sharedNormals, sharedNormalPositionIndices, MSpace::kTransform);
		if (status!=MStatus::kSuccess)
		{
			MGlobal::displayError("Failed to set shared vertex normals.");
		}
	}

	// do the non shared normals
	if (nonSharedNormals.length())
	{
		status = mfnMesh.setFaceVertexNormals(nonSharedNormals, nonSharedNormalFaces, nonSharedNormalPositionIndices, MSpace::kTransform);
		if (status!=MStatus::kSuccess)
		{
			MGlobal::displayError("Failed to set face vertex normals.");
		}
	}

	MIntArray faceList;
	faceList.setLength(numIndices);
	for (faceIndex=0,faceIndex3=0;faceIndex<numFaces;faceIndex++,faceIndex3+=3)
	{
		for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
		{
			faceList[faceIndex3+facePointIndex] = faceIndex;
		}
	}

	bool bSetColors = false;
	MColorArray colorArray;
	colorArray.setLength(numIndices);

	int vertexColorIndex;
	// set default colors
	MColor defaultColor(1.0,1.0f,1.0f,1.0f);
	for (vertexColorIndex=0;vertexColorIndex<numIndices;vertexColorIndex++)
	{
		colorArray[vertexColorIndex] = defaultColor;
	}

	// if this has colors
	if (colorVerts.GetSize())
	{
		if (bMixedColors) // inline with position
		{
			for (vertexColorIndex=0;vertexColorIndex<numIndices;vertexColorIndex++)
			{
				const TupVertex3f &color = colorVerts[colorIndices[pPositionIndices[vertexColorIndex]]];
				float alpha = alphaVerts[alphaIndices[pPositionIndices[vertexColorIndex]]];
				MColor mColor(color.x,color.y,color.z,alpha);
				colorArray[vertexColorIndex] = mColor;
			}
		}
		else // per point per face
		{
			for (vertexColorIndex=0;vertexColorIndex<numIndices;vertexColorIndex++)
			{
				const TupVertex3f &color = colorVerts[colorIndices[vertexColorIndex]];
				float alpha = alphaVerts[alphaIndices[vertexColorIndex]];
				MColor mColor(color.x,color.y,color.z,alpha);
				colorArray[vertexColorIndex] = mColor;
			}
		}
		bSetColors = true;
	}

#ifdef COLOR_TRI_STRIPS_ON_IMPORT
#ifdef COLOR_FACE_GROUPS
	TupperwareList *pConnectedFaceGroupList = pObjectAgg->FindListByKey("TriStrip_ConnectedFaceGroups");
	if (pConnectedFaceGroupList && pConnectedFaceGroupList->GetDataType()==TupperwareList::DATA_INT && pConnectedFaceGroupList->GetLength() == numFaces)
	{
		int *pConnectedFaceGroups = pConnectedFaceGroupList->GetAsInt();
		for (faceIndex=0,faceIndex3=0;faceIndex<numFaces;faceIndex++,faceIndex3+=3)
		{
			int groupIndex = pConnectedFaceGroups[faceIndex];
			int colorOffset = (groupIndex & 0xff)*3;
			float red = ((float)TriStripColors[colorOffset])/255.0f;
			float green = ((float)TriStripColors[colorOffset+1])/255.0f;
			float blue = ((float)TriStripColors[colorOffset+2])/255.0f;

			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				colorArray[faceIndex3+facePointIndex].r = red;
				colorArray[faceIndex3+facePointIndex].g = green;
				colorArray[faceIndex3+facePointIndex].b = blue;
			}
		}
	}
#else
	TupperwareAggregate *pTriStripAgg = tupTriMesh.GetTriangleStrips();
	if (pTriStripAgg)
	{
		TupTriangleStripPool stripPool(pTriStripAgg);
		int numStrips = stripPool.GetNumTriangleStrips();
		for (int stripIndex = 0; stripIndex < numStrips; stripIndex++)
		{
			int colorOffset = (stripIndex & 0xff)*3;
			float red = ((float)TriStripColors[colorOffset])/255.0f;
			float green = ((float)TriStripColors[colorOffset+1])/255.0f;
			float blue = ((float)TriStripColors[colorOffset+2])/255.0f;

			TupperwareAggregate *pStripAgg = stripPool.GetTriangleStrip(stripIndex);
			TupTriangleStrip tupStrip(pStripAgg);
			int *pFaceIndices;
			int numFaceIndices;
			tupStrip.GetFaceIndices(&pFaceIndices,numFaceIndices);
         int *pFacePointIndices;
         int numFacePointIndices;
         tupStrip.GetFacePointIndices(&pFacePointIndices,numFacePointIndices);
         int stripPointIndex;
			for (stripPointIndex = 0; stripPointIndex < numFaceIndices; stripPointIndex++)
			{
				int faceOffset = pFaceIndices[stripPointIndex]*3;

				for (int facePointIndex = 0; facePointIndex < 3; facePointIndex ++)
				{
					colorArray[faceOffset+facePointIndex].r = red;
					colorArray[faceOffset+facePointIndex].g = green;
					colorArray[faceOffset+facePointIndex].b = blue;
				}
			}
         for (stripPointIndex = 0; stripPointIndex < numFaceIndices; stripPointIndex++)
         {
            int faceOffset = pFaceIndices[stripPointIndex]*3;
            int givenFacePoint = pFacePointIndices[stripPointIndex];
            // mark first with inverse color
            if (stripPointIndex ==0 )
            {
               colorArray[faceOffset+givenFacePoint].r = 1.0f;
               colorArray[faceOffset+givenFacePoint].g = 1.0f;
               colorArray[faceOffset+givenFacePoint].b = 1.0f;
            }
            else // mark others in a sequence from dark to light
            {
               float factor = 0.7f+(float)(stripPointIndex&7)*0.05f;
               colorArray[faceOffset+givenFacePoint].r = red * factor;
               colorArray[faceOffset+givenFacePoint].g = green * factor;
               colorArray[faceOffset+givenFacePoint].b = blue * factor;
            }
         }
		}
		bSetColors = true;
	}
#endif
#endif

	if (bSetColors)
	{
		status = mfnMesh.setFaceVertexColors(colorArray,faceList,positionIndices);
		if (status!=MStatus::kSuccess)
		{
			MGlobal::displayError("Failed to set face vertex colors.");
		}
	}

	// set uv coordinates
	int numMeshMaps = tupTriMesh.GetNumMeshMaps();
	for (int meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
		TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
		int mapChannel = tupTriMeshMap.GetMapChannel();
		if (mapChannel>=TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
		{
			char *pUVSetName;
			tupTriMeshMap.GetMapName(&pUVSetName);
			TupString uvSetName;
			if (pUVSetName)
			{
				uvSetName = pUVSetName;
				uvSetName.Replace(' ','_');
			}
			else
			{
				uvSetName.Format("map%d",mapChannel);
			}
			MString uvSet(uvSetName);
			if (mapChannel==TupPolyMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
			{
				status = mfnMesh.renameUVSet(MString("map1"),uvSet);
				status = mfnMesh.setCurrentUVSetName(uvSet);
				if (status != MStatus::kSuccess)
				{
					MGlobal::displayError("Failed set current uv set name.");
				}
			}
			else
			{
				status  = mfnMesh.createUVSet(uvSet);
				if (status != MStatus::kSuccess)
				{
					MGlobal::displayError("Failed to create uv set.");
				}
				status = mfnMesh.setCurrentUVSetName(uvSet);
				if (status != MStatus::kSuccess)
				{
					MGlobal::displayError("Failed set current uv set name.");
				}
			}

			TupVertex3f *pMapVerts;
			int numMapVerts;
			int *pMapIndices;
			int numMapIndices;

			tupTriMeshMap.GetMapVerts((float**) &pMapVerts,numMapVerts);
			tupTriMeshMap.GetMapIndices(&pMapIndices,numMapIndices);
			numMapIndices *=3;

			MFloatArray uArray;
			MFloatArray vArray;
			MIntArray uvIds(pMapIndices,numMapIndices);

			uArray.setLength(numMapVerts);
			vArray.setLength(numMapVerts);

			for (int uvIndex=0;uvIndex<numMapVerts;uvIndex++)
			{
				uArray[uvIndex] = pMapVerts[uvIndex].x;
				vArray[uvIndex] = 1.0f-pMapVerts[uvIndex].y;
			}
			status = mfnMesh.setUVs( uArray, vArray ,&uvSet);
			if (status!=MStatus::kSuccess)
			{
				MGlobal::displayError("Failed to set mesh uvs.");
			}
			status = mfnMesh.assignUVs( faceVertexCounts, uvIds ,&uvSet);
			if (status!=MStatus::kSuccess)
			{
				MGlobal::displayError("Failed to assign mesh uvs.");
			}
		}
	}
	MString defaultUVSet("map1");
	status = mfnMesh.setCurrentUVSetName(defaultUVSet);
	if (status != MStatus::kSuccess)
	{
		MGlobal::displayError("Failed set current uv set name.");
	}

	// face attributes
	if (faceAttributesVerts.GetSize())
	{
		MStringArray longNames,shortNames,formatNames;
		longNames.append( BLIND_DATA_LONG_NAME ); 
		shortNames.append( BLIND_DATA_SHORT_NAME ); 
		formatNames.append( BLIND_DATA_FORMAT_NAME ); 

		TupString strFullData;
		strFullData += BEGIN_FACE_BLOCK;
		strFullData += "\r";
		int numFaceAttributeVerts = faceAttributesVerts.GetSize();
		for (int vertIndex=0;vertIndex<numFaceAttributeVerts;vertIndex++)
		{
			strFullData += "[";
			strFullData += faceAttributesVerts[vertIndex];
			strFullData += "]\r";
		}
		strFullData += END_FACE_BLOCK;
		strFullData += "\r";

		AddNote(polyMeshObject,strFullData);						

		// place the ints on the mesh faces
		if (!mfnMesh.isBlindDataTypeUsed(BLIND_DATA_ID))
		{
			status = mfnMesh.createBlindDataType ( BLIND_DATA_ID,longNames,shortNames,formatNames) ;
		}
		int numIndices = faceAttributesIndices.GetSize();
		MIntArray polyArray;
		MIntArray polyDataArray;
		for (int faceIndex=0;faceIndex<numIndices;faceIndex++)
		{
			// if set to something
			if (faceAttributesIndices[faceIndex]!=0)
			{
				polyArray.append(faceIndex);
				polyDataArray.append(faceAttributesIndices[faceIndex]);
			}
		}
		if (polyArray.length()) // if any faces changed
		{
			status = mfnMesh.setIntBlindData( polyArray, MFn::kMeshPolygonComponent,BLIND_DATA_ID,BLIND_DATA_LONG_NAME,polyDataArray); 
		}
	}
#ifdef DEFER_MESH_SHADER_APPLICATION
	m_pShaderManager->AddMeshShaderData(polyDagPath,pFaceMaterialRefs,numFaces);
#else
	m_pShaderManager->SetUpMeshShaders(polyDagPath,pFaceMaterialRefs,numFaces);
#endif
}


void TupImport::ImportSplineShape(TupperwareAggregate *pObjectAgg,MObject parentObject)
{
	TupSplineShape tupSplineShape(pObjectAgg);

	int numSplines = tupSplineShape.GetNumSplines();
	if (!numSplines)
		return;

	TupperwareAggregate *pSplineAgg = tupSplineShape.GetSpline(0);
	TupSpline tupSpline(pSplineAgg);

	int splineFlags;
	tupSpline.GetFlags(splineFlags);

	unsigned int degree;
	switch (splineFlags & TupSpline::FLAGS_DEGREE_MASK)
	{
		case TupSpline::FLAGS_DEGREE_ONE:
			degree = 1;
			break;
		case TupSpline::FLAGS_DEGREE_TWO:
			degree = 2;
			break;
		case TupSpline::FLAGS_DEGREE_THREE:
			degree = 3;
			break;
		case TupSpline::FLAGS_DEGREE_FOUR:
			degree = 4;
			break;
		case TupSpline::FLAGS_DEGREE_FIVE:
			degree = 5;
			break;
		case TupSpline::FLAGS_DEGREE_SIX:
			degree = 6;
			break;
		case TupSpline::FLAGS_DEGREE_SEVEN:
			degree = 7;
			break;
	}

	MFnNurbsCurve::Form agForm;

	switch (splineFlags & TupSpline::FLAGS_FORM_MASK)
	{
		case TupSpline::FLAGS_FORM_OPEN:
			agForm = MFnNurbsCurve::kOpen;
			break;
		case TupSpline::FLAGS_FORM_CLOSED:
			agForm = MFnNurbsCurve::kClosed;
			break;
		case TupSpline::FLAGS_FORM_PERIODIC:
			agForm = MFnNurbsCurve::kPeriodic;
			break;
	}

	MPointArray controlVertices;
	MDoubleArray knotSequences;

	int numCVs;
	Vector4 *pControlVertices;
	int numKnots;
	float *pKnots;

	tupSpline.GetControlPoints((float **)&pControlVertices,numCVs);
	tupSpline.GetKnots(&pKnots,numKnots);

	controlVertices.setLength(numCVs);

	for (int cvIndex = 0;cvIndex<numCVs;cvIndex++)
	{
		MPoint newCV(pControlVertices[cvIndex].x()*SCALEFACTOR,
						 pControlVertices[cvIndex].y()*SCALEFACTOR,
						 pControlVertices[cvIndex].z()*SCALEFACTOR,
						 pControlVertices[cvIndex].w()*SCALEFACTOR);
		controlVertices[cvIndex] = newCV;
	}

	knotSequences.setLength(numKnots);

	for (int knotIndex=0;knotIndex<numKnots;knotIndex++)
	{
		knotSequences[knotIndex] = pKnots[knotIndex];
	}

	MStatus status;
	MFnNurbsCurve fnCurve;
	fnCurve.create(controlVertices,knotSequences,degree,agForm,false,true,parentObject,&status);
	if (!status)
		return;
}

void TupImport::ImportBezierShape(TupperwareAggregate *pObjectAgg,MObject parentObject)
{
	TupBezierShape tupBezierShape(pObjectAgg);

	int numSplines = tupBezierShape.GetNumSplines();
	if (!numSplines)
		return;

	TupperwareAggregate *pSplineAgg = tupBezierShape.GetSpline(0);
	TupBezierSpline tupBezierSpline(pSplineAgg);

	int splineFlags;
	tupBezierSpline.GetFlags(splineFlags);

	unsigned int degree = 3;

	MFnNurbsCurve::Form agForm = MFnNurbsCurve::kOpen;

	bool bClosed = false;

	if (splineFlags & TupBezierSpline::FLAGS_CLOSED)
	{
		bClosed = true;
		agForm = MFnNurbsCurve::kPeriodic;
	}

	int numPoints;
	Vector3 *pPointLocations;
	Vector3 *pInVectors;
	Vector3 *pOutVectors;
	int *pPointFlags;

	tupBezierSpline.GetPointLocations((float**)&pPointLocations,numPoints);
	tupBezierSpline.GetInVectors((float**)&pInVectors,numPoints);
	tupBezierSpline.GetOutVectors((float**)&pOutVectors,numPoints);
	tupBezierSpline.GetPointFlags(&pPointFlags,numPoints);

	int numBetweenPoints = (numPoints>2) ? numPoints-2 : 0;

	int numSplinePoints = (numBetweenPoints*(degree+2))+4 + ((bClosed) ? 9 : 0);
	int numSpans = numSplinePoints - degree;
	int numKnots = numSpans + ((2*degree)-1);

	MPointArray controlVertices;
	MDoubleArray knotSequences;

	controlVertices.setLength(numSplinePoints);
	knotSequences.setLength(numKnots);

	if (!bClosed)
	{
		int cvIndex = 0;
		float knotValue = 0.000f;
		int knotIndex = 0;
		for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
		{
			MPoint newCV(pPointLocations[pointIndex].x()*SCALEFACTOR,
				pPointLocations[pointIndex].y()*SCALEFACTOR,
				pPointLocations[pointIndex].z()*SCALEFACTOR,
				1.0f);

			controlVertices[cvIndex++] = newCV;
			knotSequences[knotIndex++] = knotValue;

			// if start
			if (pointIndex==0)
			{
				// 3 knots (same value)
				knotSequences[knotIndex++] = knotValue;
				knotSequences[knotIndex++] = knotValue;

				// tangents
				MPoint newOutCV(pOutVectors[pointIndex].x()*SCALEFACTOR,
					pOutVectors[pointIndex].y()*SCALEFACTOR,
					pOutVectors[pointIndex].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newOutCV;

				MPoint newInCV(pInVectors[pointIndex+1].x()*SCALEFACTOR,
					pInVectors[pointIndex+1].y()*SCALEFACTOR,
					pInVectors[pointIndex+1].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newInCV;
				knotValue+=1.0f;
			}
			// if end
			else if (pointIndex+1==numPoints)
			{
				// 3 knots (same value)
				knotSequences[knotIndex++] = knotValue;
				knotSequences[knotIndex++] = knotValue;
			}
			// if inbetween point
			else 
			{
				// 3 points (same value) & 3 knots (incremented value)
				knotValue+=1.0f;
				controlVertices[cvIndex++] = newCV;
				knotSequences[knotIndex++] = knotValue;

				knotValue+=1.0f;
				controlVertices[cvIndex++] = newCV;
				knotSequences[knotIndex++] = knotValue;

				knotValue+=1.0f;
				// tangents
				MPoint newOutCV(pOutVectors[pointIndex].x()*SCALEFACTOR,
					pOutVectors[pointIndex].y()*SCALEFACTOR,
					pOutVectors[pointIndex].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newOutCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				MPoint newInCV(pInVectors[pointIndex+1].x()*SCALEFACTOR,
					pInVectors[pointIndex+1].y()*SCALEFACTOR,
					pInVectors[pointIndex+1].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newInCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;
			}
		}
	}
	else // bClosed
	{
		int cvIndex = 0;
		float knotValue = -2.000f;
		int knotIndex = 0;
		// if start and end are the same position don't use last point
		if (pPointLocations[numPoints-1]==pPointLocations[0])
		{
			numPoints--;
		}
		for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
		{
			MPoint newCV(pPointLocations[pointIndex].x()*SCALEFACTOR,
				pPointLocations[pointIndex].y()*SCALEFACTOR,
				pPointLocations[pointIndex].z()*SCALEFACTOR,
				1.0f);

			controlVertices[cvIndex++] = newCV;
			knotSequences[knotIndex++] = knotValue;

			// if start
			if (pointIndex==0)
			{
				// 3 knots (incremented)
				knotValue += 1.000f;
				knotSequences[knotIndex++] = knotValue;
				knotValue += 1.000f;
				knotSequences[knotIndex++] = knotValue;

				// tangents
				MPoint newOutCV(pOutVectors[pointIndex].x()*SCALEFACTOR,
					pOutVectors[pointIndex].y()*SCALEFACTOR,
					pOutVectors[pointIndex].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newOutCV;

				knotValue += 1.000f;
				knotSequences[knotIndex++] = knotValue;

				MPoint newInCV(pInVectors[pointIndex+1].x()*SCALEFACTOR,
					pInVectors[pointIndex+1].y()*SCALEFACTOR,
					pInVectors[pointIndex+1].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newInCV;
				knotValue += 1.000f;
				knotSequences[knotIndex++] = knotValue;

				knotValue+=1.0f;
			}
			// if end
			else if (pointIndex+1==numPoints)
			{
				knotValue+=1.000f;

				// 3 points (same)
				// 3 knots (incremented)
				controlVertices[cvIndex++] = newCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				controlVertices[cvIndex++] = newCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				// tangent out from last 
				MPoint newOutCV(pOutVectors[pointIndex].x()*SCALEFACTOR,
					pOutVectors[pointIndex].y()*SCALEFACTOR,
					pOutVectors[pointIndex].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newOutCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				// tangent in from first
				MPoint newInCV(pInVectors[0].x()*SCALEFACTOR,
					pInVectors[0].y()*SCALEFACTOR,
					pInVectors[0].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newInCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				// first point (3 times)
				MPoint newCV(pPointLocations[0].x()*SCALEFACTOR,
					pPointLocations[0].y()*SCALEFACTOR,
					pPointLocations[0].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				controlVertices[cvIndex++] = newCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				controlVertices[cvIndex++] = newCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				// tangent out from first 
				MPoint newOutFirstCV(pOutVectors[0].x()*SCALEFACTOR,
					pOutVectors[0].y()*SCALEFACTOR,
					pOutVectors[0].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newOutFirstCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				// tangent in from second
				MPoint newInSecondCV(pInVectors[1].x()*SCALEFACTOR,
					pInVectors[1].y()*SCALEFACTOR,
					pInVectors[1].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newInSecondCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

			}
			// if inbetween point
			else 
			{
				knotValue+=1.0f;

				// 3 points (same value)
				// 3 knots  (incremented)
				controlVertices[cvIndex++] = newCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				controlVertices[cvIndex++] = newCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				// tangents
				MPoint newOutCV(pOutVectors[pointIndex].x()*SCALEFACTOR,
					pOutVectors[pointIndex].y()*SCALEFACTOR,
					pOutVectors[pointIndex].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newOutCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;

				MPoint newInCV(pInVectors[pointIndex+1].x()*SCALEFACTOR,
					pInVectors[pointIndex+1].y()*SCALEFACTOR,
					pInVectors[pointIndex+1].z()*SCALEFACTOR,
					1.0f);

				controlVertices[cvIndex++] = newInCV;
				knotSequences[knotIndex++] = knotValue;
				knotValue+=1.0f;
			}
		}
	}

	MStatus status;
	MFnNurbsCurve fnCurve;
	fnCurve.create(controlVertices,knotSequences,degree,agForm,false,true,parentObject,&status);
	if (!status)
		return;
}

void TupImport::ImportLight(TupperwareAggregate *pObjectAgg,MObject parentObject)
{
	MStatus status;
	TupLight tupLight(pObjectAgg);

	int lightType;
	tupLight.GetLightType(lightType);

	MObject lightObject;

	MFnDagNode fnDagNode;

	switch (lightType)
	{
		case TupLight::LIGHT_OMNI:
			lightObject = fnDagNode.create(MString("pointLight"),parentObject,&status);
			break;
		case TupLight::LIGHT_TARGET_SPOT:
		case TupLight::LIGHT_FREE_SPOT:
		case TupLight::LIGHT_TARGET_DIRECT:
		case TupLight::LIGHT_FREE_DIRECT:
		case TupLight::LIGHT_AREA:
			lightObject = fnDagNode.create(MString("directionalLight"),parentObject,&status);
			break;
		case TupLight::LIGHT_AMBIENT:
			lightObject = fnDagNode.create(MString("ambientLight"),parentObject,&status);
			break;
	}
	if (!lightObject.isNull())
	{
		float *pLightColor;
		tupLight.GetLightColor(&pLightColor);
		float lightIntensity;
		tupLight.GetMultiplier(lightIntensity);
		MColor lightColor(pLightColor[0],pLightColor[1],pLightColor[2]);

		MFnLight mfnLight(lightObject);
		mfnLight.setColor(lightColor);
		mfnLight.setIntensity(lightIntensity);
	}
}

void TupImport::ImportLocator(TupperwareAggregate *pObjectAgg,MObject parentObject)
{
	MFnDagNode fnDagNode;
	MStatus status;
	fnDagNode.create(MString("locator"),parentObject,&status);
}

