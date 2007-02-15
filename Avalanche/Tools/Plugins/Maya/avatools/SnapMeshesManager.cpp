#include "stdafx.h"
#include "snapMeshesManager.h"

#define BLIND_DATA_ID 956
#define BLIND_DATA_LONG_NAME "FaceAttributes"
#define BLIND_DATA_SHORT_NAME "fa1"
#define BLIND_DATA_FORMAT_NAME "int"


SnapMeshesManager::SnapMeshesManager(void) : m_positionTolerance(10.0), m_positionToleranceSq(100.0)
{
}

void SnapMeshesManager::SetPositionTolerance(float positionTolerance)
{
	m_positionTolerance = positionTolerance;
	m_positionToleranceSq = positionTolerance*positionTolerance;
}

MStatus SnapMeshesManager::AddInstancesForSnap(const MSelectionList &selectionList)
{
	MGlobal::executeCommand(MString("dgdirty -a"));

	MStatus status;

	if (selectionList.length()==0)
	{
		DISPLAY_ERROR("Nothing selected.");
		return MS::kFailure;
	}

	// last one is the base
	MDagPath lastDagPath;
	selectionList.getDagPath(selectionList.length()-1,lastDagPath);

	MDagPath baseTransformPath,baseMeshPath;
	int baseMeshChildIndex;
	if (!FindMesh(lastDagPath,baseTransformPath,baseMeshPath,baseMeshChildIndex))
	{
		DISPLAY_ERROR("Bad selection.");
		return MS::kFailure;
	}

	// count how many we are needing to do
	int numSubMeshes = 0;
	int selectionIndex;
	for (selectionIndex = 0; selectionIndex < (int)selectionList.length()-1; selectionIndex++)
	{
		MDagPath mDagPath;
		selectionList.getDagPath(selectionIndex,mDagPath);

		MDagPath subTransformPath,subMeshPath;
		int subMeshChildIndex;
		if (FindMesh(mDagPath,subTransformPath,subMeshPath,subMeshChildIndex))
		{
				numSubMeshes++;
		}
	}

	if (!numSubMeshes)
	{
		DISPLAY_ERROR("Bad selection.");
		return MS::kFailure;
	}

	selectionList.getDagPath(selectionList.length()-1,baseTransformPath);
	MString infoString = "Getting Mesh Data for base ";
	infoString += baseMeshPath.fullPathName();
	DISPLAY_INFO(infoString);
	status = GetMeshData(m_baseMeshData,baseTransformPath,baseMeshPath,baseMeshChildIndex);

	FATAL_ERROR(status,"Error Getting base mesh data.")
	
	TupMatrix4x4d worldToBaseMatrix;
	m_baseMeshData.m_localToWorldMatrix.Inverse(worldToBaseMatrix);

	m_subMeshDataArray.SetSize(numSubMeshes);

	int subInstanceIndex=0;
	for (selectionIndex = 0; selectionIndex < (int)selectionList.length()-1; selectionIndex++)
	{
		MDagPath mDagPath;
		selectionList.getDagPath(selectionIndex,mDagPath);

		MDagPath subTransformPath,subMeshPath;
		int subMeshChildIndex;
		if (FindMesh(mDagPath,subTransformPath,subMeshPath,subMeshChildIndex))
		{
			MeshData &subMeshData = m_subMeshDataArray[subInstanceIndex];
			MString infoString = "Getting Mesh Data for ";
			infoString += subMeshPath.fullPathName();
			DISPLAY_INFO(infoString);

			status = GetMeshData(subMeshData,subTransformPath,subMeshPath,subMeshChildIndex);

			FATAL_ERROR(status,"Error Getting sub mesh data.")

			if (!m_baseMeshData.m_localToWorldMatrix.Compare(subMeshData.m_localToWorldMatrix))
			{
				DISPLAY_INFO("Transforming into space of base node");
				TupMatrix4x4d localToBaseMatrix;
				// set up the local to base matrix
				worldToBaseMatrix.Multiply(subMeshData.m_localToWorldMatrix,localToBaseMatrix);
				// transform sub mesh to local coordinate system of base
				localToBaseMatrix.TransformVertices(subMeshData.m_positionVerts);
				localToBaseMatrix.TransformNormals(subMeshData.m_normalVerts);
				// make them have the same matrices
				subMeshData.m_localToWorldMatrix = m_baseMeshData.m_localToWorldMatrix;
			}
			subInstanceIndex++;
		}
	}
	return  MS::kSuccess;
}

MStatus SnapMeshesManager::AddInstancesForCopy(const MSelectionList &selectionList)
{
	MGlobal::executeCommand(MString("dgdirty -a"));

	MStatus status;

	if (selectionList.length()==0)
	{
		DISPLAY_ERROR("Nothing selected.");
		return MS::kFailure;
	}

	// count how many we are needing to do
	int numSubMeshes = 0;
	int selectionIndex;
	for (selectionIndex = 0; selectionIndex < (int)selectionList.length(); selectionIndex++)
	{
		MDagPath mDagPath;
		selectionList.getDagPath(selectionIndex,mDagPath);

		MDagPath subTransformPath,subMeshPath;
		int subMeshChildIndex;
		if (FindMesh(mDagPath,subTransformPath,subMeshPath,subMeshChildIndex))
		{
			numSubMeshes++;
		}
	}

	if (!numSubMeshes)
	{
		DISPLAY_ERROR("Bad selection.");
		return MS::kFailure;
	}

	m_subMeshDataArray.SetSize(numSubMeshes);

	int subInstanceIndex=0;
	for (selectionIndex = 0; selectionIndex < (int)selectionList.length(); selectionIndex++)
	{
		MDagPath mDagPath;
		selectionList.getDagPath(selectionIndex,mDagPath);

		MDagPath subTransformPath,subMeshPath;
		int subMeshChildIndex;

		if (FindMesh(mDagPath,subTransformPath,subMeshPath,subMeshChildIndex))
		{
			MeshData &subMeshData = m_subMeshDataArray[subInstanceIndex];
			MString infoString = "Getting Mesh Data for ";
			infoString += subMeshPath.fullPathName();
			DISPLAY_INFO(infoString);

			status = GetMeshData(subMeshData,subTransformPath,subMeshPath,subMeshChildIndex);

			FATAL_ERROR(status,"Error Getting sub mesh data.")

			subInstanceIndex++;
		}
	}
	return  MS::kSuccess;
}

MStatus SnapMeshesManager::ReorderFaces(void)
{
	MStatus status;
	status = ReorderFaces(m_baseMeshData);
	FATAL_ERROR(status,"Error reordering faces in base mesh.")

	for (int index = 0; index < m_subMeshDataArray.GetSize(); index++)
	{
		status = ReorderFaces(m_subMeshDataArray[index]);
		FATAL_ERROR(status,"Error reordering faces in sub mesh.")
	}
	return MS::kSuccess;
}

MStatus SnapMeshesManager::SnapInstancesToBase(void)
{
	MStatus status;

	int numSubMeshes = m_subMeshDataArray.GetSize();

	for (int subMeshIndex = 0; subMeshIndex < numSubMeshes; subMeshIndex++)
	{
		MeshData &subMeshData = m_subMeshDataArray[subMeshIndex];

		MString infoString = "Snapping sub mesh ";
		infoString += subMeshData.m_meshDagPath.fullPathName();
		infoString += " To base mesh";
		DISPLAY_INFO(infoString);

		status = SnapMeshToBase(m_baseMeshData,subMeshData);
		FATAL_ERROR(status,"Error Snapping sub to base.")
	}
	return  MS::kSuccess;
}

MStatus SnapMeshesManager::AddToScene(void)
{
	MStatus status;
	// rebuild sub meshes

	MSelectionList newSelectionList;

	int numSubMeshes = m_subMeshDataArray.GetSize();
	for (int subMeshIndex=0;subMeshIndex<numSubMeshes;subMeshIndex++)
	{
		MeshData &subMeshData = m_subMeshDataArray[subMeshIndex];

		MString infoString = "Building new mesh data for ";
		infoString += subMeshData.m_meshDagPath.fullPathName();
		DISPLAY_INFO(infoString);

		status = CreateMeshFromSub(subMeshData);
		FATAL_ERROR(status,"Error building sub mesh.")

		if (subMeshData.m_facesToSelect.GetSize())
		{
			status = AddFacesToSelection(subMeshData,newSelectionList);
			FATAL_ERROR(status,"Error adding faces to selection.")
		}
	}

	// set it if there are any in the list
	if (newSelectionList.length())
	{
		MGlobal::setActiveSelectionList(newSelectionList);
	}

	return MS::kSuccess;
}

void SnapMeshesManager::ConvertMayaMatrix(const MMatrix &srcMatrix,TupMatrix4x4d &dstMatrix)
{
	dstMatrix._11 = srcMatrix.matrix[0][0];
	dstMatrix._21 = srcMatrix.matrix[0][1];
	dstMatrix._31 = srcMatrix.matrix[0][2];

	dstMatrix._12 = srcMatrix.matrix[1][0];
	dstMatrix._22 = srcMatrix.matrix[1][1];
	dstMatrix._32 = srcMatrix.matrix[1][2];

	dstMatrix._13 = srcMatrix.matrix[2][0];
	dstMatrix._23 = srcMatrix.matrix[2][1];
	dstMatrix._33 = srcMatrix.matrix[2][2];

	dstMatrix._14 = srcMatrix.matrix[3][0];
	dstMatrix._24 = srcMatrix.matrix[3][1];
	dstMatrix._34 = srcMatrix.matrix[3][2];

	dstMatrix._41 = dstMatrix._42 = dstMatrix._43 = 0.0;
	dstMatrix._44 = 1.0;
}

void SnapMeshesManager::ConvertMayaMatrix(const TupMatrix4x4d &dstMatrix,MMatrix &srcMatrix)
{
	srcMatrix.matrix[0][0] = 	dstMatrix._11;
	srcMatrix.matrix[0][1] = 	dstMatrix._21;
	srcMatrix.matrix[0][2] = 	dstMatrix._31;

	srcMatrix.matrix[1][0] = 	dstMatrix._12;
	srcMatrix.matrix[1][1] = 	dstMatrix._22;
	srcMatrix.matrix[1][2] = 	dstMatrix._32;

	srcMatrix.matrix[2][0] = 	dstMatrix._13;
	srcMatrix.matrix[2][1] = 	dstMatrix._23;
	srcMatrix.matrix[2][2] = 	dstMatrix._33;

	srcMatrix.matrix[3][0] = 	dstMatrix._14;
	srcMatrix.matrix[3][1] = 	dstMatrix._24;
	srcMatrix.matrix[3][2] = 	dstMatrix._34;

	srcMatrix.matrix[0][3] = srcMatrix.matrix[1][3] = srcMatrix.matrix[2][3] = 0.0;
	srcMatrix.matrix[3][3] = 1.0;
}


MStatus SnapMeshesManager::GetMeshData(SnapMeshesManager::MeshData &meshData,const MDagPath &transformDagPath,const MDagPath &meshDagPath,int meshChildIndex)
{
	MStatus status;
	MSpace::Space space = MSpace::kObject;

	meshData.m_transformDagPath = transformDagPath;
	meshData.m_meshDagPath = meshDagPath;
	meshData.m_meshChildIndex = meshChildIndex;

	meshData.m_nodeName = transformDagPath.fullPathName().asChar();

	MMatrix myTM = transformDagPath.inclusiveMatrix();
	ConvertMayaMatrix(myTM,meshData.m_localToWorldMatrix);

	meshData.m_mayaObjectName = meshData.m_meshDagPath.fullPathName().asChar();

	MObject mComponent = MObject::kNullObj;

	MFnMesh fnMesh(meshData.m_meshDagPath, &status);

	FATAL_ERROR(status,"Failure in MFnMesh initialization.");

	MItMeshVertex vtxIter(meshData.m_meshDagPath, mComponent, &status);

	FATAL_ERROR(status,"Failure in MItMeshVertex initialization.");

	for ( ; !vtxIter.isDone(); vtxIter.next() )
	{
		MPoint point = vtxIter.position( space );
		TupVertex3d point1;
		point1.Set(point.x,point.y,point.z); 
		meshData.m_positionVerts.Add(point1);
	}
	
	// Normal Table
	if (fnMesh.numNormals())
	{
		MFloatVectorArray sourceNormals;
		fnMesh.getNormals( sourceNormals, space );
		for (int normalIndex=0; normalIndex < (int)sourceNormals.length(); normalIndex++ )
		{
			MFloatVector normal = sourceNormals[normalIndex];
			TupVertex3d normal1;
			normal1.Set(normal[0],normal[1],normal[2]);
			meshData.m_normalVerts.Add(normal1);
		}
	}
	else
	{
		TupVertex3d dummyNormal;
		dummyNormal.Set(0.0,1.0,0.0);
		meshData.m_normalVerts.Add(dummyNormal);
	}

	TupString meshPath = meshData.m_meshDagPath.fullPathName().asChar();
	TupString meshName = fnMesh.name().asChar();

	MItMeshPolygon polyIter(meshData.m_meshDagPath, mComponent, &status);

	FATAL_ERROR(status,"Failure in MItMeshPolygon initialization.");

// set up arrays for how the polygons are trianglulated
	int numPolygons = fnMesh.numPolygons();
	int polyFaceIndex;

	meshData.m_faceVertexCounts.SetSize(numPolygons);
	meshData.m_facePointOffsets.SetSize(numPolygons);
	meshData.m_faceAttributeIndices.SetSize(numPolygons);

	// get position, normal, and face normal data
	int facePointOffset=0;
	for (polyFaceIndex = 0 ; !polyIter.isDone(); polyIter.next(),polyFaceIndex++ )
	{
		MIntArray sourcePositionIndicies;
		status = polyIter.getVertices(sourcePositionIndicies);

		int numFacePoints = polyIter.polygonVertexCount();
		meshData.m_faceVertexCounts[polyFaceIndex] = numFacePoints;
		meshData.m_facePointOffsets[polyFaceIndex] = facePointOffset;
		meshData.m_faceAttributeIndices[polyFaceIndex] = -1; // no attributes (for now)

		int facePointIndex;
		for (facePointIndex=0;facePointIndex<numFacePoints;facePointIndex++)
		{
			// position
			meshData.m_positionIndices.Add(sourcePositionIndicies[facePointIndex]);

			// vertex normal
			if (fnMesh.numNormals())
			{
				meshData.m_normalIndices.Add(polyIter.normalIndex(facePointIndex));
			}
			else
			{
				meshData.m_normalIndices.Add(0);
			}
		}
		facePointOffset+=numFacePoints;
	}

	meshData.m_shaderVerts.clear();
	meshData.m_shaderIndices.clear();
	int instanceIndex = meshData.m_meshDagPath.instanceNumber();
	fnMesh.getConnectedShaders(instanceIndex,meshData.m_shaderVerts,meshData.m_shaderIndices);

	//	Multiple UV Sets
	int numUVSets = fnMesh.numUVSets();
	
	MStringArray uvSetNames;
	fnMesh.getUVSetNames(uvSetNames);

	meshData.m_meshMaps.SetSize(numUVSets);

	TupArray<MIntArray> polyUVCounts;
	TupArray<MIntArray> polyUVIds;
	TupArray<int> currentUVIDArray;

	polyUVCounts.SetSize(numUVSets);
	polyUVIds.SetSize(numUVSets);
	currentUVIDArray.SetSize(numUVSets);


	int uvSetIndex;
	for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		meshData.m_meshMaps[uvSetIndex].m_uvSetName = uvSetNames[uvSetIndex].asChar();
		currentUVIDArray[uvSetIndex] = 0;

		MFloatArray uArray,vArray;
		
		//		Get UV Table
		fnMesh.getUVs(uArray,vArray,&uvSetNames[uvSetIndex]);
		meshData.m_meshMaps[uvSetIndex].m_verts.SetSize(uArray.length());

		for (int uvIndex = 0; uvIndex < (int)uArray.length(); uvIndex++)
			meshData.m_meshMaps[uvSetIndex].m_verts[uvIndex].Set(uArray[uvIndex],vArray[uvIndex],0.0f);
		
		fnMesh.getAssignedUVs(polyUVCounts[uvSetIndex],polyUVIds[uvSetIndex],&uvSetNames[uvSetIndex]);
	}
	
	polyIter.reset();

	// Faces
	for (polyFaceIndex=0 ; !polyIter.isDone(); polyIter.next(),polyFaceIndex++ )
	{
		int numFacePoints = polyIter.polygonVertexCount();

		for (int facePointIndex = 0; facePointIndex < numFacePoints; facePointIndex++ )
		{
			// uv values
			for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
			{
				if (polyUVCounts[uvSetIndex][polyFaceIndex])
				{
					meshData.m_meshMaps[uvSetIndex].m_indices.Add(polyUVIds[uvSetIndex][currentUVIDArray[uvSetIndex]+facePointIndex]);
				}
				else
				{
					meshData.m_meshMaps[uvSetIndex].m_indices.Add(0);
				}
			}

			// vertex color values
			if (polyIter.hasColor(facePointIndex))
			{
				MColor color;
				polyIter.getColor(color,facePointIndex);
				TupVertex3d newColor;
				newColor.Set(color.r,color.g,color.b);
				meshData.m_vertexColorIndices.Add(meshData.m_vertexColorVerts.Add(newColor));
					
				double newAlpha = color.a;
				meshData.m_vertexAlphaIndices.Add(meshData.m_vertexAlphaVerts.Add((float)newAlpha));
			}
			else
			{
				meshData.m_vertexColorIndices.Add(-1);
				meshData.m_vertexAlphaIndices.Add(-1);
			}
		}

		// update id starts for uvs
		for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
		{
			if (polyUVCounts[uvSetIndex][polyFaceIndex])
			{
				currentUVIDArray[uvSetIndex]+=numFacePoints;
			}
		}
  	}

	MPlug notesPlug = fnMesh.findPlug("notes",&status);
	if (status)
	{
		MString tempString;
		notesPlug.getValue(tempString);	
		meshData.m_faceAttributeVerts = tempString.asChar();
	}

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

				// make sure the compID is in range
				if (compID>=0 && compID<numPolygons) 
				{
					meshData.m_faceAttributeIndices[compID] = setIndex;
				}
			}
		}
	}

	return MS::kSuccess; 
}

MStatus SnapMeshesManager::SnapMeshToBase(const MeshData &baseMeshData,MeshData &subMeshData)
{
	int numBaseFaces = baseMeshData.m_faceVertexCounts.GetSize();
	int numBasePoints = baseMeshData.m_positionIndices.GetSize();
	int numSubFaces = subMeshData.m_faceVertexCounts.GetSize();
	int numSubPoints = subMeshData.m_positionIndices.GetSize();
	int faceIndex;
	int facePointIndex;
	int facePointOffset;

	TupArray<int> newPositionIndices;
	TupArray<int> newNormalIndices;
	TupArray<int> newVertexColorIndices;
	TupArray<int> newVertexAlphaIndices;
	TupArray<TupArray<int> > newMeshMapIndices;
	TupArray<TupVertex3d> newPositionVerts;
	TupArray<TupVertex3d> newNormalVerts;
	
	newPositionIndices.SetSize(numSubPoints);
	newNormalIndices.SetSize(numSubPoints);
	newVertexColorIndices.SetSize(numSubPoints);
	newVertexAlphaIndices.SetSize(numSubPoints);

	// make sure the mesh maps are the right size
	int meshMapIndex;
	int numSubMeshMaps = subMeshData.m_meshMaps.GetSize();
	newMeshMapIndices.SetSize(numSubMeshMaps);
	for (meshMapIndex=0;meshMapIndex<numSubMeshMaps;meshMapIndex++)
	{
		newMeshMapIndices[meshMapIndex].SetSize(numSubPoints);
	}

	// keep track of which faces are using which position verts
	TupArray<TupArray<int> > basePositionFaces;
	TupArray<TupArray<int> > basePositionFacePointIndices;
	basePositionFaces.SetSize(baseMeshData.m_positionVerts.GetSize());
	basePositionFacePointIndices.SetSize(baseMeshData.m_positionVerts.GetSize());
	int numPointsInFace;
	for (faceIndex=0,facePointOffset=0;faceIndex<numBaseFaces;faceIndex++,facePointOffset+=numPointsInFace)
	{
		numPointsInFace = baseMeshData.m_faceVertexCounts[faceIndex];

		for (facePointIndex=0;facePointIndex<numPointsInFace;facePointIndex++)
		{
			int positionIndex = baseMeshData.m_positionIndices[facePointOffset+facePointIndex];
			basePositionFaces[positionIndex].Add(faceIndex);
			basePositionFacePointIndices[positionIndex].Add(facePointIndex);
		}
	}

	int numSubMeshPositionVerts = subMeshData.m_positionVerts.GetSize();
	// this array keeps track of which position verts were added to the sub mesh (not from the base)
	TupArray<int> addedPosVertRemapTable;
	addedPosVertRemapTable.SetSize(numSubMeshPositionVerts);

	// find the base position verts that are close to the submesh position verts
	TupArray<TupArray<int> > closeBaseVertexTable;
	closeBaseVertexTable.SetSize(numSubMeshPositionVerts);
	int numAddedPosVertsStage1 = 0;
	int numLinkedFaces = 0;
	int subVertIndex;
	float lowerNoFittingDistanceSq=1.0e10f;
	float higherNoFittingDistanceSq=0.0f;
	for (subVertIndex=0;subVertIndex<numSubMeshPositionVerts;subVertIndex++)
	{
		TupVertex3d &subVertex = subMeshData.m_positionVerts[subVertIndex];
		int numBaseMeshPositionVerts = baseMeshData.m_positionVerts.GetSize();
		float smallestNoFittingDistanceSq=1.0e10f;
		bool bFoundMatch = false;
		for (int baseVertIndex=0;baseVertIndex<numBaseMeshPositionVerts;baseVertIndex++)
		{
			const TupVertex3d &baseVertex = baseMeshData.m_positionVerts[baseVertIndex];
			float distanceSq = (float)TupVertex3d::DistanceSq(subVertex,baseVertex);
			// keep track of ones that are within the threshold
			if (distanceSq<m_positionToleranceSq)
			{
				bFoundMatch = true;
				closeBaseVertexTable[subVertIndex].Add(baseVertIndex);
			}
			// we also keep track of the closest distance that didn't fit
			else if (distanceSq<smallestNoFittingDistanceSq)
			{
				smallestNoFittingDistanceSq = distanceSq;
			}
		}
		// if there are some verts close set that we have not added a vert
		if (bFoundMatch)
		{
			addedPosVertRemapTable[subVertIndex] = -1;
		}
		// if no verts close then add vert and set up remap
		else
		{
			newPositionVerts.Add(subVertex);
			addedPosVertRemapTable[subVertIndex] = newPositionVerts.GetSize()-1;
			numAddedPosVertsStage1++;
			// keep track of distances that didn't fit (for the whole mesh)
			if (smallestNoFittingDistanceSq<lowerNoFittingDistanceSq)
			{
				lowerNoFittingDistanceSq = smallestNoFittingDistanceSq;
			}
			if (smallestNoFittingDistanceSq>higherNoFittingDistanceSq)
			{
				higherNoFittingDistanceSq = smallestNoFittingDistanceSq;
			}
		}
	}

	int numSubMeshNormalVerts = subMeshData.m_normalVerts.GetSize();
	// this array keeps track of which normal verts were added to the sub mesh (not from the base)
	TupArray<int> addedNormalVertRemapTable;
	addedNormalVertRemapTable.SetSize(numSubMeshNormalVerts);
	for (int subNormalVertIndex=0;subNormalVertIndex<numSubMeshNormalVerts;subNormalVertIndex++)
	{
		addedNormalVertRemapTable[subNormalVertIndex] = -1;
	}


	// set up the array to keep track of which base pos verts are currently remapped into the sub pos verts (added from the base)
	TupArray<int> basePosVertRemapTable;
	int numBasePosVerts = baseMeshData.m_positionVerts.GetSize();
	basePosVertRemapTable.SetSize(numBasePosVerts);
	for (int basePosVertIndex=0;basePosVertIndex<numBasePosVerts;basePosVertIndex++)
	{
		basePosVertRemapTable[basePosVertIndex] = -1;
	}

	// set up the array to keep track of which base normal verts are currently remapped into the sub normal verts (added from the base)
	TupArray<int> baseNormalVertRemapTable;
	int numBaseNormalVerts = baseMeshData.m_normalVerts.GetSize();
	baseNormalVertRemapTable.SetSize(numBaseNormalVerts);
	for (int baseNormalVertIndex=0;baseNormalVertIndex<numBaseNormalVerts;baseNormalVertIndex++)
	{
		baseNormalVertRemapTable[baseNormalVertIndex] = -1;
	}


	// build a new set of data for the sub mesh using the base
	int numAddedPosVertsStage2 = 0;
	for (faceIndex=0,facePointOffset=0;faceIndex<numSubFaces;faceIndex++,facePointOffset+=numPointsInFace)
	{
		// try to find a face to link to from the base mesh
		int numMatchesFound = 0;
		int bestFace;
		float bestFaceDistance;
		int bestFaceRotation;

		numPointsInFace = subMeshData.m_faceVertexCounts[faceIndex];

		// build a list of faces to check against (uses all the faces close to the first point of the sub face)
		Keeper<int> closeBaseFaces;
		TupArray<int> &closeBaseVertices = closeBaseVertexTable[subMeshData.m_positionIndices[facePointOffset]];
		for (int closeIndex=0;closeIndex<closeBaseVertices.GetSize();closeIndex++)
		{
			TupArray<int> &baseFaces = basePositionFaces[closeBaseVertices[closeIndex]];
			for (int baseFacesIndex=0;baseFacesIndex<baseFaces.GetSize();baseFacesIndex++)
			{
				int baseFaceIndex = baseFaces[baseFacesIndex];
				// make sure the faces have the same number of points
				if (baseMeshData.m_faceVertexCounts[baseFaceIndex]==numPointsInFace)
				{
					closeBaseFaces.Add(baseFaces[baseFacesIndex]);
				}
			}
		}

		// we need to rotate as well to find links
		for (int faceRotation=0;faceRotation<numPointsInFace;faceRotation++)
		{
			int numCloseBaseFaces = closeBaseFaces.GetSize();
			for (int closeBaseFaceIndex=0;closeBaseFaceIndex<numCloseBaseFaces;closeBaseFaceIndex++)
			{
				int closeBaseFace = closeBaseFaces[closeBaseFaceIndex];
				int baseFacePointOffset = m_baseMeshData.m_facePointOffsets[closeBaseFace];
				bool bFoundMatch = true;
				double faceDistance = 0.0;
				for (facePointIndex=0;facePointIndex<numPointsInFace;facePointIndex++)
				{
					int rotatedFacePointIndex = facePointIndex+faceRotation;
					// handle wrapping
					if (rotatedFacePointIndex>=numPointsInFace)
						rotatedFacePointIndex-=numPointsInFace;

					// test rotated point in sub against base
					TupVertex3d &subVertex = subMeshData.m_positionVerts[subMeshData.m_positionIndices[facePointOffset+rotatedFacePointIndex]];
					const TupVertex3d &baseVertex = baseMeshData.m_positionVerts[baseMeshData.m_positionIndices[baseFacePointOffset+facePointIndex]];
					double distance = TupVertex3d::Distance(subVertex,baseVertex);
					// if too far apart then stop
					if (distance>=m_positionTolerance)
					{
						bFoundMatch = false;
						break;
					}
					faceDistance += distance;
				}
				if (bFoundMatch)
				{
					if (!numMatchesFound || faceDistance<bestFaceDistance)
					{
						numMatchesFound++;
						bestFace = closeBaseFace;
						bestFaceDistance = (float)faceDistance;
						bestFaceRotation = faceRotation;
					}
				}
			}
		}
		// if we found a face to link to 
		if (numMatchesFound)
		{
			numLinkedFaces++;
			int baseFacePointOffset = m_baseMeshData.m_facePointOffsets[bestFace];
			for (facePointIndex=0;facePointIndex<numPointsInFace;facePointIndex++)
			{
				int rotatedFacePointIndex = facePointIndex+bestFaceRotation;
				// handle wrapping
				if (rotatedFacePointIndex>=numPointsInFace)
					rotatedFacePointIndex-=numPointsInFace;

				int basePosVertIndex = baseMeshData.m_positionIndices[baseFacePointOffset+facePointIndex];
				// check to see if we have already dealt with this one before
				int newPositionVertIndex = basePosVertRemapTable[basePosVertIndex];
				if (newPositionVertIndex==-1)
				{
					// add a new vert to the sub mesh and keep track of it for next time
					// the position indices do not use rotation since they are taken from the base mesh 
					// and need to be the in the same rotation order
					newPositionVerts.Add(baseMeshData.m_positionVerts[basePosVertIndex]);
					newPositionVertIndex = newPositionVerts.GetSize()-1;
					basePosVertRemapTable[basePosVertIndex] = newPositionVertIndex;
				}
				newPositionIndices[facePointOffset+facePointIndex] = newPositionVertIndex;

				int baseNormalVertIndex = baseMeshData.m_normalIndices[baseFacePointOffset+facePointIndex];
				// check to see if we have already dealt with this one before
				int newNormalVertIndex = baseNormalVertRemapTable[baseNormalVertIndex];
				if (newNormalVertIndex==-1)
				{
					// add a new vert to the sub mesh and keep track of it for next time
					// the normal indices do not use rotation since they are taken from the base mesh 
					// and need to be the in the same rotation order
					newNormalVerts.Add(baseMeshData.m_normalVerts[baseNormalVertIndex]);
					newNormalVertIndex = newNormalVerts.GetSize()-1;
					baseNormalVertRemapTable[baseNormalVertIndex] = newNormalVertIndex;
				}
				newNormalIndices[facePointOffset+facePointIndex] = newNormalVertIndex;

				// these grab for the rotated point to the non rotated point
				newVertexColorIndices[facePointOffset+facePointIndex] = subMeshData.m_vertexColorIndices[facePointOffset+rotatedFacePointIndex];
				newVertexAlphaIndices[facePointOffset+facePointIndex] = subMeshData.m_vertexAlphaIndices[facePointOffset+rotatedFacePointIndex];
				for (meshMapIndex=0;meshMapIndex<numSubMeshMaps;meshMapIndex++)
				{
					newMeshMapIndices[meshMapIndex][facePointOffset+facePointIndex] = subMeshData.m_meshMaps[meshMapIndex].m_indices[facePointOffset+rotatedFacePointIndex];
				}
			}
		}
		// if no face to link to
		else
		{
			subMeshData.m_facesToSelect.Add(faceIndex); // keep track of these faces to select later
			for (facePointIndex=0;facePointIndex<numPointsInFace;facePointIndex++)
			{
				int subPosVertIndex = subMeshData.m_positionIndices[facePointOffset+facePointIndex];
				// check to see if we have already dealt with this one before
				int newPositionVertIndex = addedPosVertRemapTable[subPosVertIndex];
				if (newPositionVertIndex==-1)
				{
					// add a new vert to the sub mesh and keep track of it for next time
					newPositionVerts.Add(subMeshData.m_positionVerts[subPosVertIndex]);
					newPositionVertIndex = newPositionVerts.GetSize()-1;
					addedPosVertRemapTable[subPosVertIndex] = newPositionVertIndex;
					numAddedPosVertsStage2++;
				}
				newPositionIndices[facePointOffset+facePointIndex] = newPositionVertIndex;

				int subNormalVertIndex = subMeshData.m_normalIndices[facePointOffset+facePointIndex];
				// check to see if we have already dealt with this one before
				int newNormalVertIndex = addedNormalVertRemapTable[subNormalVertIndex];
				if (newNormalVertIndex==-1)
				{
					// add a new vert to the sub mesh and keep track of it for next time
					// the normal indices do not use rotation since they are taken from the base mesh 
					// and need to be the in the same rotation order
					newNormalVerts.Add(subMeshData.m_normalVerts[subNormalVertIndex]);
					newNormalVertIndex = newNormalVerts.GetSize()-1;
					addedNormalVertRemapTable[subNormalVertIndex] = newNormalVertIndex;
				}
				newNormalIndices[facePointOffset+facePointIndex] = newNormalVertIndex;

				newVertexColorIndices[facePointOffset+facePointIndex] = subMeshData.m_vertexColorIndices[facePointOffset+facePointIndex];
				newVertexAlphaIndices[facePointOffset+facePointIndex] = subMeshData.m_vertexAlphaIndices[facePointOffset+facePointIndex];
				for (meshMapIndex=0;meshMapIndex<numSubMeshMaps;meshMapIndex++)
				{
					newMeshMapIndices[meshMapIndex][facePointOffset+facePointIndex] = subMeshData.m_meshMaps[meshMapIndex].m_indices[facePointOffset+facePointIndex];
				}
			}
		}
	}
	TupString outputString;
	if (numAddedPosVertsStage1==0 && numAddedPosVertsStage2==0)
	{
		outputString.Format("    All %d position verts snapped to base mesh",numSubMeshPositionVerts);
		DISPLAY_INFO(MString(outputString));
	}
	else 
	{
		outputString.Format("    %d of %d position verts merged into base mesh",numSubMeshPositionVerts-(numAddedPosVertsStage1+numAddedPosVertsStage2),numSubMeshPositionVerts);
		DISPLAY_INFO(MString(outputString));
		if (numAddedPosVertsStage1)
		{
			outputString.Format("    lower no fitting distance = %f",TupVertex3Sqrt(lowerNoFittingDistanceSq));
			DISPLAY_INFO(MString(outputString));
			outputString.Format("    higher no fitting distance = %f",TupVertex3Sqrt(higherNoFittingDistanceSq));
			DISPLAY_INFO(MString(outputString));
		}
	}

	if (numLinkedFaces==numSubFaces)
	{
		outputString.Format("    All %d faces linked with base mesh",numLinkedFaces);
		DISPLAY_INFO(MString(outputString));
	}
	else
	{
		outputString.Format("    %d of %d faces linked with base mesh",numLinkedFaces,numSubFaces);
		DISPLAY_INFO(MString(outputString));
	}

	// copy data into mesh
	subMeshData.m_positionIndices = newPositionIndices;
	subMeshData.m_normalIndices = newNormalIndices;
	subMeshData.m_vertexColorIndices = newVertexColorIndices;
	subMeshData.m_vertexAlphaIndices = newVertexAlphaIndices;
	for (meshMapIndex=0;meshMapIndex<numSubMeshMaps;meshMapIndex++)
	{
		subMeshData.m_meshMaps[meshMapIndex].m_indices = newMeshMapIndices[meshMapIndex];
	}
	subMeshData.m_positionVerts = newPositionVerts;
	subMeshData.m_normalVerts = newNormalVerts;

	return MS::kSuccess;
}

struct FaceVertexNormals
{
	int m_faceIndex;
	int m_normalIndex;
};

MStatus SnapMeshesManager::CreateMeshFromSub(MeshData &meshData)
{
	MStatus status;
	MSpace::Space space = MSpace::kObject;

	MMatrix myMatrix;
	ConvertMayaMatrix(meshData.m_localToWorldMatrix,myMatrix);
	MMatrix inverseParentMatrix = meshData.m_transformDagPath.exclusiveMatrixInverse();

	myMatrix = myMatrix*inverseParentMatrix;

	MFnTransform mfnTransform(meshData.m_transformDagPath.node());
	MTransformationMatrix localMatrix(myMatrix);
	mfnTransform.set(localMatrix);

	MFnMesh mfnMesh;

	int numPolygons = meshData.m_faceVertexCounts.GetSize();
	int numPositionVerts = meshData.m_positionVerts.GetSize();
	int numIndices = meshData.m_positionIndices.GetSize();
	MPointArray positionVerts;
	positionVerts.setLength(numPositionVerts);
	for (int posIndex=0;posIndex<numPositionVerts;posIndex++)
	{
		MPoint newPoint( meshData.m_positionVerts[posIndex].x,meshData.m_positionVerts[posIndex].y,meshData.m_positionVerts[posIndex].z);
		positionVerts[posIndex] = newPoint;
	}

	MIntArray faceVertexCounts(meshData.m_faceVertexCounts.GetData(),numPolygons);
	MIntArray positionIndices(meshData.m_positionIndices.GetData(),numIndices);

	MFnDagNode fnTransformDagNode(meshData.m_transformDagPath.node());
	MFnDagNode fnOldMeshDagNode(meshData.m_meshDagPath.node());
	MString meshName = fnOldMeshDagNode.name();

	if (meshData.m_meshDagPath.isInstanced())
	{
		status = fnTransformDagNode.removeChildAt(meshData.m_meshChildIndex); 
		FATAL_ERROR(status,"Error removing link to previous mesh");
	}
	else // delete if not instanced
	{
		MDGModifier dgModifier;
		MGlobal::executeCommand(MString("delete ")+meshData.m_meshDagPath.fullPathName());
		dgModifier.doIt();
	}

	DeleteIntermediateObjects(meshData.m_transformDagPath);

	MObject polyMeshObject = mfnMesh.create( numPositionVerts, numPolygons,positionVerts,faceVertexCounts,positionIndices,meshData.m_transformDagPath.node(), &status );

	FATAL_ERROR(status,"Error creating mesh");

	MFnDagNode mfnDagNode(polyMeshObject);
	mfnDagNode.setName(meshName);
	
	MDagPath polyDagPath;
	mfnDagNode.getPath(polyDagPath);
	mfnMesh.setObject(polyDagPath);
	meshData.m_meshDagPath = polyDagPath;

	TupArray<TupArray<FaceVertexNormals> > positionVertsFaceNormals;
	positionVertsFaceNormals.SetSize(numPositionVerts);

	int polyIndex;
	int indicesIndex=0;
	// keep track of which faces are using which position as well as which normal
	for (polyIndex=0;polyIndex<numPolygons;polyIndex++)
	{
		for (int vertIndex=0;vertIndex<faceVertexCounts[polyIndex];vertIndex++)
		{
			int positionIndex = positionIndices[indicesIndex];
			FaceVertexNormals faceVertexNormals;
			faceVertexNormals.m_faceIndex = polyIndex;
			faceVertexNormals.m_normalIndex = meshData.m_normalIndices[indicesIndex];
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
				TupVertex3d &normal = meshData.m_normalVerts[firstFaceNormalIndex];
				MVector newVector(normal.x,normal.y,normal.z);
				sharedNormals.append(newVector);
				sharedNormalPositionIndices.append(positionIndex);
			}
			else
			{
				for (int facePointIndex=0;facePointIndex<numFacesForVert;facePointIndex++)
				{
					TupVertex3d &normal = meshData.m_normalVerts[positionVertsFaceNormals[positionIndex][facePointIndex].m_normalIndex];
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

		FATAL_ERROR(status,"Failed to set shared vertex normals.");
	}

	// do the non shared normals
	if (nonSharedNormals.length())
	{
		status = mfnMesh.setFaceVertexNormals(nonSharedNormals, nonSharedNormalFaces, nonSharedNormalPositionIndices, MSpace::kTransform);

		FATAL_ERROR(status,"Failed to set face vertex normals.");
	}

	MIntArray faceList;
	faceList.setLength(numIndices);
	indicesIndex=0;
	for (polyIndex=0;polyIndex<numPolygons;polyIndex++)
	{
		for (int vertIndex=0;vertIndex<meshData.m_faceVertexCounts[polyIndex];vertIndex++)
		{
			faceList[indicesIndex] = polyIndex;
			indicesIndex++;
		}
	}

	// if this has colors
	if (meshData.m_vertexColorVerts.GetSize())
	{
		MColorArray colorArray;
		MIntArray colorFaceList;
		MIntArray colorPositionIndices;

		int vertexColorIndex=0;
		// keep track of which faces are using which position as well as which normal
		for (polyIndex=0;polyIndex<numPolygons;polyIndex++)
		{
			for (int vertIndex=0;vertIndex<faceVertexCounts[polyIndex];vertIndex++)
			{
				if (meshData.m_vertexColorIndices[vertexColorIndex]!=-1 &&
					 meshData.m_vertexAlphaIndices[vertexColorIndex]!=-1)
				{
					const TupVertex3d &color = meshData.m_vertexColorVerts[meshData.m_vertexColorIndices[vertexColorIndex]];
					float alpha = meshData.m_vertexAlphaVerts[meshData.m_vertexAlphaIndices[vertexColorIndex]];
					MColor mColor((float)color.x,(float)color.y,(float)color.z,(float)alpha);
					colorArray.append(mColor);
					colorFaceList.append(polyIndex);
					colorPositionIndices.append(positionIndices[vertexColorIndex]);
				}
				vertexColorIndex++;
			}
		}

		status = mfnMesh.setFaceVertexColors(colorArray,colorFaceList,colorPositionIndices);

		FATAL_ERROR(status,"Failed to set face vertex colors.");
	}

	// set uv coordinates
	int numMeshMaps = meshData.m_meshMaps.GetSize();
	for (int meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		MString uvSet(meshData.m_meshMaps[meshMapIndex].m_uvSetName);
		// if first one
		if (meshMapIndex==0)
		{
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

		int numMapIndices = meshData.m_meshMaps[meshMapIndex].m_indices.GetSize();
		int numMapVerts = meshData.m_meshMaps[meshMapIndex].m_verts.GetSize();

		MFloatArray uArray;
		MFloatArray vArray;
		MIntArray uvIds(meshData.m_meshMaps[meshMapIndex].m_indices.GetData(),meshData.m_meshMaps[meshMapIndex].m_indices.GetSize());
		MIntArray faceVertexCounts(meshData.m_faceVertexCounts.GetData(),meshData.m_faceVertexCounts.GetSize());

		uArray.setLength(numMapVerts);
		vArray.setLength(numMapVerts);

		for (int uvIndex=0;uvIndex<numMapVerts;uvIndex++)
		{
			uArray[uvIndex] = (float)meshData.m_meshMaps[meshMapIndex].m_verts[uvIndex].x;
			vArray[uvIndex] = (float)meshData.m_meshMaps[meshMapIndex].m_verts[uvIndex].y;
		}

		status = mfnMesh.setUVs( uArray, vArray ,&uvSet);
		if (status!=MStatus::kSuccess)
		{
			MGlobal::displayError("Failed to set mesh uvs.");
		}
		status = mfnMesh.assignUVs(faceVertexCounts, uvIds ,&uvSet);
		if (status!=MStatus::kSuccess)
		{
			MGlobal::displayError("Failed to assign mesh uvs.");
		}
	}
	MString defaultUVSet("map1");
	status = mfnMesh.setCurrentUVSetName(defaultUVSet);
	if (status != MStatus::kSuccess)
	{
		MGlobal::displayError("Failed set current uv set name.");
	}

	// face attributes
	if (meshData.m_faceAttributeVerts.GetLength())
	{
		MStringArray longNames,shortNames,formatNames;
		longNames.append( BLIND_DATA_LONG_NAME ); 
		shortNames.append( BLIND_DATA_SHORT_NAME ); 
		formatNames.append( BLIND_DATA_FORMAT_NAME ); 

		int numIndices = meshData.m_faceAttributeIndices.GetSize();
		MIntArray polyArray;
		MIntArray polyDataArray;
		for (int faceIndex=0;faceIndex<numIndices;faceIndex++)
		{
			// if set to something
			if (meshData.m_faceAttributeIndices[faceIndex]!=-1)
			{
				polyArray.append(faceIndex);
				polyDataArray.append(meshData.m_faceAttributeIndices[faceIndex]);
			}
		}
		if (polyArray.length()) // if any faces changed
		{
			AddNote(polyMeshObject,meshData.m_faceAttributeVerts);						

			// place the ints on the mesh faces
			if (!mfnMesh.isBlindDataTypeUsed(BLIND_DATA_ID))
			{
				status = mfnMesh.createBlindDataType ( BLIND_DATA_ID,longNames,shortNames,formatNames) ;
				if (status!=MStatus::kSuccess)
				{
					MGlobal::displayError("Failed to create the blind data.");
				}
			}
			status = mfnMesh.setIntBlindData( polyArray, MFn::kMeshPolygonComponent,BLIND_DATA_ID,BLIND_DATA_LONG_NAME,polyDataArray); 
			if (status!=MStatus::kSuccess)
			{
				MGlobal::displayError("Failed to set the blind data.");
			}
		}
	}

	TupArray<TupArray<int> > facesUsingShaders;
	facesUsingShaders.SetSize(meshData.m_shaderVerts.length());

	for (int faceIndex=0;faceIndex<numPolygons;faceIndex++)
	{
		int shaderIndex = meshData.m_shaderIndices[faceIndex];
		if (shaderIndex!=-1)
			facesUsingShaders[shaderIndex].Add(faceIndex);
	}

	MItMeshPolygon mItMeshPolygon(polyDagPath);

	int numShadingGroups = meshData.m_shaderVerts.length();
	int shadingGroupIndex;
	for (shadingGroupIndex=0;shadingGroupIndex<numShadingGroups;shadingGroupIndex++)
	{
		const TupArray<int> &facesUsingShader = facesUsingShaders[shadingGroupIndex];
		int numFacesToSet = facesUsingShader.GetSize();

		if (numFacesToSet)
		{
			MSelectionList polySelection;
			for (int faceToSetIndex=0;faceToSetIndex<numFacesToSet;faceToSetIndex++)
			{
				int prevIndex;
				mItMeshPolygon.setIndex(facesUsingShader[faceToSetIndex],prevIndex);
				MObject polyFace = mItMeshPolygon.polygon();
				
				status = polySelection.add(polyDagPath,polyFace);
				if (!status)
				{
					MGlobal::displayWarning("Could not add face to set.");
				}
			}
			
			MFnSet set(meshData.m_shaderVerts[shadingGroupIndex], &status);
			if (!status)
			{
				MGlobal::displayWarning("Could not create shader set.");
			}
			
			status = set.addMembers(polySelection);
			if (!status)
			{
				MGlobal::displayWarning(MString("Could not add poly members to shader set. ") + status.errorString());
			}
		}
	}
	return MS::kSuccess;
}

void SnapMeshesManager::AddNote(MObject object,const TupString &noteString)
{
	MDGModifier dgModifier;
	MStatus status;
	MString command;
	MFnDependencyNode fnNode(object);
	MFnDagNode fnDagNode(object);
	MDagPath mDagPath;
	fnDagNode.getPath(mDagPath);
	TupString pathString = mDagPath.fullPathName().asChar();
	
	MString note(noteString);
	
	const char *debug = note.asChar();
	
	MPlug plug = fnNode.findPlug("notes",&status);
	if (!status)
	{
		command = MString("addAttr -dt \"string\" -ln \"notes\" -sn \"nts\" ") + mDagPath.fullPathName();
		MGlobal::executeCommand(command);
		
		plug = fnNode.findPlug("notes",&status);
		if (!status)
			return;
	}
	
	status = plug.setValue(note);
	if (!status)
		return;
	
	dgModifier.doIt();
}

MStatus SnapMeshesManager::AddFacesToSelection(MeshData &meshData,MSelectionList &selectionList)
{
	MStatus status;
	MItMeshPolygon polyIter(meshData.m_meshDagPath,MObject::kNullObj,&status);
	FATAL_ERROR(status,"Error in wrapping MItMeshPolygon");

	int numFacesToSet = meshData.m_facesToSelect.GetSize();
	for (int index=0;index<numFacesToSet;index++)
	{
		int faceIndex = meshData.m_facesToSelect[index];
		int prevIndex;
		polyIter.setIndex(faceIndex,prevIndex);
		MObject faceComponent = polyIter.polygon();
		status = selectionList.add(meshData.m_meshDagPath,faceComponent);
		FATAL_ERROR(status,"Error adding face to selection");
	}
	return MS::kSuccess;
}

bool SnapMeshesManager::FindMesh(const MDagPath &selectedPath,MDagPath &transformPath,MDagPath &meshPath,int &childIndex)
{
	if (selectedPath.apiType()==MFn::kTransform)
	{
		transformPath = selectedPath;
		int numChildren = selectedPath.childCount();
		// find an object for the transform node (child)
		for (childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MObject childObject = selectedPath.child(childIndex);
			MDagPath childPath = selectedPath;
			childPath.push(childObject);

			MFnDagNode mDagNode(childPath);
			if (mDagNode.isIntermediateObject() || childPath.apiType()!=MFn::kMesh)
			{
				continue;
			}
			else
			{
				meshPath = childPath;
				return true;
			}
		}
	}
	else
	{
		MFnDagNode mDagNode(selectedPath);
		if (!mDagNode.isIntermediateObject() && selectedPath.apiType()==MFn::kMesh)
		{
			meshPath = selectedPath;
			MObject meshObject = selectedPath.node();
			transformPath = selectedPath;
			transformPath.pop();
			int numChildren = transformPath.childCount();
			// find an object for the transform node (child)
			for (childIndex = 0; childIndex < numChildren; childIndex++)
			{
				MObject childObject = transformPath.child(childIndex);
				if (meshObject==childObject)
				{
					return true;
				}
			}
		}
	}
	return false;
}

MStatus SnapMeshesManager::DeleteIntermediateObjects(MDagPath &transformPath)
{
	MDGModifier dgModifier;

	MStatus status;
	if (transformPath.apiType()==MFn::kTransform)
	{
		bool bFound = false;
		do
		{
			bFound = false;
			int numChildren = transformPath.childCount();
			// find an object for the transform node (child)
			for (int childIndex = 0; childIndex <numChildren; childIndex++)
			{
				MDagPath childPath = transformPath;
				childPath.push(transformPath.child(childIndex));

				MFnDagNode mDagNode(childPath);
				if (mDagNode.isIntermediateObject())
				{
					if (childPath.isInstanced())
					{
						MFnDagNode fnTransformDagNode(transformPath.node());
						status = fnTransformDagNode.removeChildAt(childIndex); 
						FATAL_ERROR(status,"Error removing link to previous mesh");
						bFound = true;
						break;
					}
					else // delete if not instanced
					{
						MGlobal::executeCommand(MString("delete ")+childPath.fullPathName());
						bFound = true;
						break;
					}
				}
			}
		}
		while (bFound);

	}
	dgModifier.doIt();
	return MStatus::kSuccess;
}

static int ComparePositions(const TupVertex3d &a,const TupVertex3d &b)
{
	if (a.x < b.x)
		return -1;
	else if (a.x > b.x)
		return 1;
	else if (a.y < b.y)
		return -1;
	else if (a.y > b.y)
		return 1;
	else if (a.z < b.z)
		return -1;
	else if (a.z > b.z)
		return 1;
	else
		return 0;
}

MStatus SnapMeshesManager::ReorderFaces(MeshData &meshData)
{
	int numFaces = meshData.m_faceVertexCounts.GetSize();
	int numPoints = meshData.m_positionIndices.GetSize();
	int faceIndex;
	int facePointIndex;
	int facePointOffset;

	TupArray<int> newPositionIndices;
	TupArray<int> newNormalIndices;
	TupArray<int> newVertexColorIndices;
	TupArray<int> newVertexAlphaIndices;
	TupArray<TupArray<int> > newMeshMapIndices;

	newPositionIndices.SetSize(numPoints);
	newNormalIndices.SetSize(numPoints);
	newVertexColorIndices.SetSize(numPoints);
	newVertexAlphaIndices.SetSize(numPoints);

	// make sure the mesh maps are the right size
	int meshMapIndex;
	int numMeshMaps = meshData.m_meshMaps.GetSize();
	newMeshMapIndices.SetSize(numMeshMaps);
	for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		newMeshMapIndices[meshMapIndex].SetSize(numPoints);
	}

	int numPointsInFace;
	for (faceIndex=0,facePointOffset=0;faceIndex<numFaces;faceIndex++,facePointOffset+=numPointsInFace)
	{
		numPointsInFace = meshData.m_faceVertexCounts[faceIndex];

		int bestFacePointIndex;
		TupVertex3d bestFacePosition;
		for (facePointIndex=0;facePointIndex<numPointsInFace;facePointIndex++)
		{
			int positionIndex = meshData.m_positionIndices[facePointOffset+facePointIndex];
			const TupVertex3d &position = meshData.m_positionVerts[positionIndex];
			if (facePointIndex == 0 || ComparePositions(position,bestFacePosition)<0)
			{
				bestFacePointIndex = facePointIndex;
				bestFacePosition = position;
			}
		}

		// copy offseted indices into new arrays
		for (facePointIndex=0;facePointIndex<numPointsInFace;facePointIndex++)
		{
			int sourceIndex = bestFacePointIndex+facePointIndex;
			if (sourceIndex>=numPointsInFace)
				sourceIndex -= numPointsInFace;
			newPositionIndices[facePointOffset+facePointIndex] = meshData.m_positionIndices[facePointOffset+sourceIndex];
			newNormalIndices[facePointOffset+facePointIndex] = meshData.m_normalIndices[facePointOffset+sourceIndex];
			newVertexColorIndices[facePointOffset+facePointIndex] = meshData.m_vertexColorIndices[facePointOffset+sourceIndex];
			newVertexAlphaIndices[facePointOffset+facePointIndex] = meshData.m_vertexAlphaIndices[facePointOffset+sourceIndex];
			for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
			{
				newMeshMapIndices[meshMapIndex][facePointOffset+facePointIndex] = meshData.m_meshMaps[meshMapIndex].m_indices[facePointOffset+sourceIndex];
			}
		}
	}
	meshData.m_positionIndices = newPositionIndices;
	meshData.m_normalIndices = newNormalIndices;
	meshData.m_vertexColorIndices = newVertexColorIndices;
	meshData.m_vertexAlphaIndices = newVertexAlphaIndices;
	for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		meshData.m_meshMaps[meshMapIndex].m_indices = newMeshMapIndices[meshMapIndex];
	}
	return MS::kSuccess;
}
