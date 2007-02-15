////////////////////////////////////////////////////////////////////////////
//
// MeshManager
//
// Class to handle keeping track of the unique meshes
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "MeshManager.h"
#include "CRC.h"

static const float SCALEFACTOR = .01f;

// MeshManager::UniqueObjectData methods

MeshManager::UniqueObjectData::UniqueObjectData(void) :
	m_meshCRC(0),
	m_vertexColorCRC(0),
	m_bHasVertexColors(false),
	m_parentMeshIndex(-1),
	m_parentVertexColorIndex(-1),
	m_meshObjectRef(-1),
	m_vertexColorObjectRef(-1),
	m_meshMaterialRef(-1),
	m_shaderId(-1)
{
}

static void FixColor(Vector3 &newColor)
{
	if (newColor.x()<0.0f) newColor.x(0.0f);
	if (newColor.x()>1.0f) newColor.x(1.0f);
	if (newColor.y()<0.0f) newColor.y(0.0f);
	if (newColor.y()>1.0f) newColor.y(1.0f);
	if (newColor.z()<0.0f) newColor.z(0.0f);
	if (newColor.z()>1.0f) newColor.z(1.0f);
}

static bool IsOutsideEdge(int index1,int index2,int totalPoints)
{
	// check for adjacent indices

	if (index1+1==index2) // 1->2
	{
		return true;
	}
	else if (index1-1==index2) // 2->1
	{
		return true;
	}
	else if (index1==0 && (index2+1==totalPoints))  // 0->5 (if totalPoints was 6)
	{
		return true;
	}
	else if ((index1+1==totalPoints) && (index2==0)) // 5->0 (if totalPoints was 6)
	{
		return true;
	}
	return false;
}

static Vector3 ComputeFaceNormal(const Vector3 &point1,const Vector3 &point2,const Vector3 &point3)
{
	Vector3 faceNormal;
	faceNormal = Vector3::Cross(point3 - point2, point1 - point2);
	faceNormal.SafeNormalize();
	return faceNormal;
}

#ifdef VERIFY_FINITE_VALUES
static bool VerifyFiniteValues(float *pFloats,int nSize)
{
	bool bFound = false;
	for (int index=0;index<nSize;index++)
	{
		if (!_finite(pFloats[index]))
		{
			pFloats[index] = 10000.0f; // set to a reasonable large number
			bFound = true;
		}
	}
	return !bFound;
}
#endif



#ifdef POLYGON_EXPORT
// polygon version
void MeshManager::UniqueObjectData::InitMeshData(MeshManager::AddShadersInterface *pAddShadersInterface)
{
	MStatus status;
	MSpace::Space space = MSpace::kObject;

	MObject mComponent = MObject::kNullObj;

	Vector3 white;
	white.Set(1.0f,0.0f,1.0f);	// funky pink
	m_vertexColorVertices.Add(white);
	
	float opaque=1.0f;
	m_vertexAlphaVertices.Add(opaque);
	TupString dagPathString = m_dagPath.fullPathName().asChar();
	TupString bpDagPathString = m_PBDagPath.fullPathName().asChar();

	// Get PreBind data

	MFnDependencyNode fnPBNode(m_PBDagPath.node());
	MFnMesh fnPBMesh(m_PBDagPath, &status);
	if (status != MS::kSuccess)
	{
		fprintf(stderr,"Failure in MFnMesh initialization.\n");
		return;
	}

	// Vertex Table
	MItMeshVertex vtxIter(m_PBDagPath, mComponent, &status);
	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshVertex initialization.\n");
		return;
	}
	for ( ; !vtxIter.isDone(); vtxIter.next() )
	{
		MPoint point = vtxIter.position( space );
		Vector3 point1;
#ifdef MAKE_Z_UP
		point1.Set(point.x,-point.z,point.y); // convert from yup to zup 
#else
		point1.Set((float)point.x,(float)point.y,(float)point.z); // yup
#endif
		point1 *= SCALEFACTOR; // convert to meters
		m_positionVertices.Add(point1);
	}
	
	// Normal Table
	TupArray<int> destNormalIndexRemaps;
	if (fnPBMesh.numNormals())
	{
		MFloatVectorArray sourceNormals;
		fnPBMesh.getNormals( sourceNormals, space );
		for (int normalIndex=0; normalIndex < (int)sourceNormals.length(); normalIndex++ )
		{
			MFloatVector normal = sourceNormals[normalIndex];
			Vector3 normal1;
#ifdef MAKE_Z_UP
			normal1.Set(normal[0],-normal[2],normal[1]); // yup to zup
#else
			normal1.Set(normal[0],normal[1],normal[2]); // yup
#endif
			destNormalIndexRemaps.Add(m_normalVertices.Add(normal1));
		}
	}
	else
	{
		Vector3 dummyNormal;
#ifdef MAKE_Z_UP
		dummyNormal.Set(0.0f,0.0f,1.0f);
#else
		dummyNormal.Set(0.0f,1.0f,0.0f);
#endif
	}

	MItMeshPolygon polyIter(m_dagPath, mComponent, &status);
	
	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshPolygon initialization.\n");
		return;
	}

// set up arrays for how the polygons are trianglulated
	int numPolygons = fnPBMesh.numPolygons();
	TupArray<TupArray<int> > polyTriangleIndices;
	polyTriangleIndices.SetSize(numPolygons);

	int polyFaceIndex;
	for (polyFaceIndex=0 ; !polyIter.isDone(); polyIter.next(),polyFaceIndex++)
	{
		TupArray<int> &triangleIndices = polyTriangleIndices[polyFaceIndex];
		
		MIntArray sourcePositionIndicies;
		status = polyIter.getVertices(sourcePositionIndicies);
		
		// get an array of integers that point to the polygon points for the triangulation (cannot rely on a random fan)
		int numTriangles;
		polyIter.numTriangles(numTriangles);
		MIntArray sourceTrianglePositionIndices;
		MPointArray sourceTrianglePoints;
		status = polyIter.getTriangles(sourceTrianglePoints,sourceTrianglePositionIndices,space);
		triangleIndices.SetSize(sourceTrianglePositionIndices.length());
		// do a linear search to find matching polygon point for each triangle point
		for (int trianglePointIndex=0;trianglePointIndex<(int)sourceTrianglePositionIndices.length();trianglePointIndex++)
		{
			bool bFoundPoint = false;
			int pointToFind = sourceTrianglePositionIndices[trianglePointIndex];
			for (int pointIndex=0;pointIndex<(int)sourcePositionIndicies.length();pointIndex++)
			{
				if (pointToFind == sourcePositionIndicies[pointIndex])
				{
					triangleIndices[trianglePointIndex] = pointIndex;
					bFoundPoint = true;
					break;
				}
			}
			if (!bFoundPoint)
			{
				triangleIndices[trianglePointIndex] = 0; // this should never happen
			}
		}
	}

	MItMeshPolygon polyPBIter(m_PBDagPath, mComponent, &status);

	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshPolygon initialization.\n");
		return;
	}

	// get position, normal, and face normal data
	for (polyFaceIndex = 0 ; !polyPBIter.isDone(); polyPBIter.next(),polyFaceIndex++ )
	{
		TupArray<int> &triangleIndices = polyTriangleIndices[polyFaceIndex];
		int numTriangles;
		polyPBIter.numTriangles(numTriangles);
		m_faceTriangleCounts.Add(numTriangles);

		MIntArray sourcePositionIndicies;
		status = polyPBIter.getVertices(sourcePositionIndicies);

		// face normal
		MVector sourceFaceNormal;
		polyPBIter.getNormal(sourceFaceNormal);
		Vector3 destFaceNormal;
#ifdef MAKE_Z_UP
		destFaceNormal.Set((float)sourceFaceNormal.x,(float)-sourceFaceNormal.z,(float)sourceFaceNormal.y);
#else
		destFaceNormal.Set((float)sourceFaceNormal.x,(float)sourceFaceNormal.y,(float)sourceFaceNormal.z);
#endif
		m_faceNormalIndices.Add(m_faceNormalVertices.Add(destFaceNormal));

		int numFacePoints = polyPBIter.polygonVertexCount();
		m_faceVertexCounts.Add(numFacePoints);

		int facePointIndex;
		for (facePointIndex=0;facePointIndex<numFacePoints;facePointIndex++)
		{
			// position
			m_positionIndices.Add(sourcePositionIndicies[facePointIndex]);

			// vertex normal
			if (fnPBMesh.numNormals())
			{
				m_normalIndices.Add(destNormalIndexRemaps[polyPBIter.normalIndex(facePointIndex)]);
			}
			else
			{
				m_normalIndices.Add(0);
			}
		}

		// what points in the poly each triangle uses
		int triangleIndex;
		int triangleOffset;
		for (triangleIndex=0,triangleOffset=0;triangleIndex<numTriangles;triangleIndex++,triangleOffset+=3)
		{
			m_trianglePointIndices.Add(triangleIndices[triangleOffset]);
			m_trianglePointIndices.Add(triangleIndices[triangleOffset+1]);
			m_trianglePointIndices.Add(triangleIndices[triangleOffset+2]);
		}
	}

	MFnDependencyNode fnNode(m_dagPath.node());
	MFnMesh fnMesh(m_dagPath, &status);
	if (status != MS::kSuccess)
	{
		fprintf(stderr,"Failure in MFnMesh initialization.\n");
		return;
	}

	MIntArray shaderIndices;
	MObjectArray shaders;
	shaders.clear();
	int instanceIndex = m_dagPath.instanceNumber();
	fnMesh.getConnectedShaders(instanceIndex,shaders,shaderIndices);

	// give back an id for the shaders and possibly remap the shaderIndices
	m_shaderId = pAddShadersInterface->AddShaders(m_dagPath,shaders,shaderIndices);

	//	Multiple UV Sets
	int numUVSets = fnMesh.numUVSets();
	
	MStringArray uvSetNames;
	fnMesh.getUVSetNames(uvSetNames);

	TupArray<MIntArray> polyUVCounts;
	TupArray<MIntArray> polyUVIds;
	TupArray<int> currentUVIDArray;

	m_UVVertices.SetSize(numUVSets);
	polyUVCounts.SetSize(numUVSets);
	polyUVIds.SetSize(numUVSets);
	m_UVIndices.SetSize(numUVSets);
	m_UVNames.SetSize(numUVSets);
	currentUVIDArray.SetSize(numUVSets);
	
	int uvSetIndex;
	for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		currentUVIDArray[uvSetIndex] = 0;

		m_UVNames[uvSetIndex] = uvSetNames[uvSetIndex].asChar();

		MFloatArray uArray,vArray;
		
		//		Get UV Table
		fnMesh.getUVs(uArray,vArray,&uvSetNames[uvSetIndex]);
		m_UVVertices[uvSetIndex].SetSize(uArray.length());

		for (int uvIndex = 0; uvIndex < (int)uArray.length(); uvIndex++)
			m_UVVertices[uvSetIndex][uvIndex].Set(uArray[uvIndex],1.0f-vArray[uvIndex],0.0f);
		
		fnMesh.getAssignedUVs(polyUVCounts[uvSetIndex],polyUVIds[uvSetIndex],&uvSetNames[uvSetIndex]);
	}
	
	polyIter.reset();

	// Faces
	int numFaces = 0;
	for (polyFaceIndex=0 ; !polyIter.isDone(); polyIter.next(),polyFaceIndex++ )
	{
		// face flags
		int faceFlags = 0;

		if (polyIter.isPlanar())
			faceFlags |= TupPolyMesh::FLAGS_FACE_PLANAR;

		if (polyIter.isConvex())
			faceFlags |= TupPolyMesh::FLAGS_FACE_CONVEX;

		if (polyIter.zeroArea())
			faceFlags |= TupPolyMesh::FLAGS_FACE_ZERO_AREA;

		if (polyIter.isHoled())
			faceFlags |= TupPolyMesh::FLAGS_FACE_HAS_HOLES;

		m_faceFlags.Add(faceFlags);

		// face material index
		m_faceMaterialIndices.Add(shaderIndices[polyFaceIndex]); // set material index

		int numFacePoints = polyIter.polygonVertexCount();

		int numVertexColors;
		polyIter.numColors(numVertexColors);
		MColorArray colors;
		if (numVertexColors > 0)
		{
			polyIter.getColors(colors);
			if (colors.length() != numFacePoints)
			{
				MGlobal::displayError("Vertex color problem.");
				numVertexColors=0;
			}
		}

		for (int facePointIndex = 0; facePointIndex < numFacePoints; facePointIndex++ )
		{
			// uv values
			for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
			{
				if (polyUVCounts[uvSetIndex][polyFaceIndex])
				{
					m_UVIndices[uvSetIndex].Add(polyUVIds[uvSetIndex][currentUVIDArray[uvSetIndex]+facePointIndex]);
				}
				else
				{
					m_UVIndices[uvSetIndex].Add(0);
				}
			}

			// vertex color values
			if (numVertexColors)
			{
				Vector3 newColor;
				newColor.Set(colors[facePointIndex].r,colors[facePointIndex].g,colors[facePointIndex].b);
				FixColor(newColor);
				m_vertexColorIndices.Add(m_vertexColorVertices.Add(newColor));
					
				float newAlpha = colors[facePointIndex].a;
				if (newAlpha<0.0f) newAlpha=0.0f;
				if (newAlpha>1.0f) newAlpha=1.0f;
				m_vertexAlphaIndices.Add(m_vertexAlphaVertices.Add(newAlpha));
			}
			else
			{
				m_vertexColorIndices.Add(0);
				m_vertexAlphaIndices.Add(0);
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

#ifdef VERIFY_FINITE_VALUES
	if (!VerifyFiniteValues((float *)m_positionVertices.GetData(),m_positionVertices.GetSize()*3))
		MGlobal::displayError(MString("Warning - ") + m_dagPath.fullPathName() + MString(" has one or more infinite position vertices"));
	if (!VerifyFiniteValues((float *)m_normalVertices.GetData(),m_normalVertices.GetSize()*3))
		MGlobal::displayError(MString("Warning - ") + m_dagPath.fullPathName() + MString(" has one or more infinite vertex normal vertices"));
	if (!VerifyFiniteValues((float *)m_faceNormalVertices.GetData(),m_faceNormalVertices.GetSize()*3))
		MGlobal::displayError(MString("Warning - ") + m_dagPath.fullPathName() + MString(" has one or more infinite face normal vertices"));

	for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		if (!VerifyFiniteValues((float *)m_UVVertices[uvSetIndex].GetData(),m_UVVertices[uvSetIndex].GetSize()*3))
			MGlobal::displayError(MString("Warning - ") + m_dagPath.fullPathName() + MString(" has one or more infinite uv vertices for uvset ") + MString(m_UVNames[uvSetIndex]));
	}
#endif

	CRC meshCRCGenerator;

	// compute hash value for the mesh (not using the vertex colors)
	meshCRCGenerator.AnalyzeData(m_positionVertices.GetData(),m_positionVertices.GetSize()*sizeof(Vector3));
	meshCRCGenerator.AnalyzeData(m_positionIndices.GetData(),m_positionIndices.GetSize()*sizeof(int));
#ifdef COMPARE_NORMALS
	meshCRCGenerator.AnalyzeData(m_normalVertices.GetData(),m_normalVertices.GetSize()*sizeof(Vector3));
	meshCRCGenerator.AnalyzeData(m_normalIndices.GetData(),m_normalIndices.GetSize()*sizeof(int));
	meshCRCGenerator.AnalyzeData(m_faceNormalVertices.GetData(),m_faceNormalVertices.GetSize()*sizeof(Vector3));
	meshCRCGenerator.AnalyzeData(m_faceNormalIndices.GetData(),m_faceNormalIndices.GetSize()*sizeof(int));
#endif
	meshCRCGenerator.AnalyzeData(m_faceFlags.GetData(),m_faceFlags.GetSize()*sizeof(int));
	meshCRCGenerator.AnalyzeData(m_faceVertexCounts.GetData(),m_faceVertexCounts.GetSize()*sizeof(int));
	meshCRCGenerator.AnalyzeData(m_faceTriangleCounts.GetData(),m_faceTriangleCounts.GetSize()*sizeof(int));
	meshCRCGenerator.AnalyzeData(m_trianglePointIndices.GetData(),m_trianglePointIndices.GetSize()*sizeof(int));
	meshCRCGenerator.AnalyzeData(m_faceMaterialIndices.GetData(),m_faceMaterialIndices.GetSize()*sizeof(int));

	for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		meshCRCGenerator.AnalyzeData(m_UVVertices[uvSetIndex].GetData(),m_UVVertices[uvSetIndex].GetSize()*sizeof(Vector3));
		meshCRCGenerator.AnalyzeData(m_UVIndices[uvSetIndex].GetData(),m_UVIndices[uvSetIndex].GetSize()*sizeof(int));
	}

	// include the shader id
	meshCRCGenerator.AnalyzeData(m_shaderId);

	m_meshCRC = meshCRCGenerator.GetResult();


	CRC vertexColorsCRCGenerator;

	// compute the hash value for the vertex colors
	m_bHasVertexColors = true; //(m_vertexColorVertices.GetSize()>1 || m_vertexAlphaVertices.GetSize()>1);

	vertexColorsCRCGenerator.AnalyzeData(m_vertexColorVertices.GetData(),m_vertexColorVertices.GetSize()*sizeof(Vector3));
	vertexColorsCRCGenerator.AnalyzeData(m_vertexAlphaVertices.GetData(),m_vertexAlphaVertices.GetSize()*sizeof(float));
	vertexColorsCRCGenerator.AnalyzeData(m_vertexColorIndices.GetData(),m_vertexColorIndices.GetSize()*sizeof(int));
	vertexColorsCRCGenerator.AnalyzeData(m_vertexAlphaIndices.GetData(),m_vertexAlphaIndices.GetSize()*sizeof(int));

	m_vertexColorCRC = vertexColorsCRCGenerator.GetResult();
}
#else
// triangle mesh version
void MeshManager::UniqueObjectData::InitMeshData(MeshManager::AddShadersInterface *pAddShadersInterface)
{
	MStatus status;
	MSpace::Space space = MSpace::kObject;

	MObject mComponent = MObject::kNullObj;

	Vector3 white;
	white.Set(1.0f,0.0f,1.0f);	// funky pink
	m_vertexColorVertices.Add(white);
	
	float opaque=1.0f;
	m_vertexAlphaVertices.Add(opaque);
	TupString dagPathString = m_dagPath.fullPathName().asChar();
	TupString bpDagPathString = m_PBDagPath.fullPathName().asChar();

	// Get PreBind data

	MFnDependencyNode fnPBNode(m_PBDagPath.node());
	MFnMesh fnPBMesh(m_PBDagPath, &status);
	if (status != MS::kSuccess)
	{
		fprintf(stderr,"Failure in MFnMesh initialization.\n");
		return;
	}

	// Vertex Table
	MItMeshVertex vtxIter(m_PBDagPath, mComponent, &status);
	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshVertex initialization.\n");
		return;
	}
	for ( ; !vtxIter.isDone(); vtxIter.next() )
	{
		MPoint point = vtxIter.position( space );
		Vector3 point1;
#ifdef MAKE_Z_UP
		point1.Set((float)point.x,(float)-point.z,(float)point.y); // convert from yup to zup 
#else
		point1.Set((float)point.x,(float)point.y,(float)point.z); // yup
#endif
		point1 *= SCALEFACTOR; // convert to meters
		m_positionVertices.Add(point1);
	}
	
	// Normal Table
	TupArray<int> destNormalIndexRemaps;
	if (fnPBMesh.numNormals())
	{
		MFloatVectorArray sourceNormals;
		fnPBMesh.getNormals( sourceNormals, space );
		for (int normalIndex=0; normalIndex < (int)sourceNormals.length(); normalIndex++ )
		{
			MFloatVector normal = sourceNormals[normalIndex];
			Vector3 normal1;
#ifdef MAKE_Z_UP
			normal1.Set((float)normal[0],(float)-normal[2],(float)normal[1]); // yup to zup
#else
			normal1.Set((float)normal[0],(float)normal[1],(float)normal[2]); // yup
#endif
			destNormalIndexRemaps.Add(m_normalVertices.Add(normal1));
		}
	}
	else
	{
		Vector3 dummyNormal;
#ifdef MAKE_Z_UP
		dummyNormal.Set(0.0f,0.0f,1.0f);
#else
		dummyNormal.Set(0.0f,1.0f,0.0f);
#endif
	}

	MItMeshPolygon polyIter(m_dagPath, mComponent, &status);
	
	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshPolygon initialization.\n");
		return;
	}

// set up arrays for how the polygons are trianglulated
	int numPolygons = fnPBMesh.numPolygons();
	TupArray<TupArray<int> > polyTriangleIndices;
	polyTriangleIndices.SetSize(numPolygons);

	int polyFaceIndex;
	for (polyFaceIndex=0 ; !polyIter.isDone(); polyIter.next(),polyFaceIndex++)
	{
		TupArray<int> &triangleIndices = polyTriangleIndices[polyFaceIndex];
		
		MIntArray sourcePositionIndicies;
		status = polyIter.getVertices(sourcePositionIndicies);
		
		// get an array of integers that point to the polygon points for the triangulation (cannot rely on a random fan)
		int numTriangles;
		polyIter.numTriangles(numTriangles);
		MIntArray sourceTrianglePositionIndices;
		MPointArray sourceTrianglePoints;
		status = polyIter.getTriangles(sourceTrianglePoints,sourceTrianglePositionIndices,space);
		triangleIndices.SetSize(sourceTrianglePositionIndices.length());
		// do a linear search to find matching polygon point for each triangle point
		for (int trianglePointIndex=0;trianglePointIndex<(int)sourceTrianglePositionIndices.length();trianglePointIndex++)
		{
			bool bFoundPoint = false;
			int pointToFind = sourceTrianglePositionIndices[trianglePointIndex];
			for (int pointIndex=0;pointIndex<(int)sourcePositionIndicies.length();pointIndex++)
			{
				if (pointToFind == sourcePositionIndicies[pointIndex])
				{
					triangleIndices[trianglePointIndex] = pointIndex;
					bFoundPoint = true;
					break;
				}
			}
			if (!bFoundPoint)
			{
				triangleIndices[trianglePointIndex] = 0; // this should never happen
			}
		}
	}

	MItMeshPolygon polyPBIter(m_PBDagPath, mComponent, &status);

	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshPolygon initialization.\n");
		return;
	}

	// get position, normal, and face normal data
	for (polyFaceIndex = 0 ; !polyPBIter.isDone(); polyPBIter.next(),polyFaceIndex++ )
	{
		TupArray<int> &triangleIndices = polyTriangleIndices[polyFaceIndex];
		int numTriangles;
		polyPBIter.numTriangles(numTriangles);

		MIntArray sourcePositionIndicies;
		status = polyPBIter.getVertices(sourcePositionIndicies);

		int triangleIndex;
		int triangleOffset;
		for (triangleIndex=0,triangleOffset=0;triangleIndex<numTriangles;triangleIndex++,triangleOffset+=3)
		{
			// position
			int polyIndex1 = triangleIndices[triangleOffset];
			int polyIndex2 = triangleIndices[triangleOffset+1];
			int polyIndex3 = triangleIndices[triangleOffset+2];
			int positionIndex1 = sourcePositionIndicies[polyIndex1];
			int positionIndex2 = sourcePositionIndicies[polyIndex2];
			int positionIndex3 = sourcePositionIndicies[polyIndex3];
			m_positionIndices.Add(positionIndex1);
			m_positionIndices.Add(positionIndex2);
			m_positionIndices.Add(positionIndex3);
			Vector3 destFaceNormal;
			destFaceNormal = ComputeFaceNormal(m_positionVertices[positionIndex1],m_positionVertices[positionIndex2],m_positionVertices[positionIndex3]);
			m_faceNormalIndices.Add(m_faceNormalVertices.Add(destFaceNormal));

			// normal
			if (fnPBMesh.numNormals())
			{
				m_normalIndices.Add(destNormalIndexRemaps[polyPBIter.normalIndex(polyIndex1)]);
				m_normalIndices.Add(destNormalIndexRemaps[polyPBIter.normalIndex(polyIndex2)]);
				m_normalIndices.Add(destNormalIndexRemaps[polyPBIter.normalIndex(polyIndex3)]);
			}
			else
			{
				m_normalIndices.Add(0);
				m_normalIndices.Add(0);
				m_normalIndices.Add(0);
			}
		}
	}

	MFnDependencyNode fnNode(m_dagPath.node());
	MFnMesh fnMesh(m_dagPath, &status);
	if (status != MS::kSuccess)
	{
		fprintf(stderr,"Failure in MFnMesh initialization.\n");
		return;
	}

	MIntArray shaderIndices;
	MObjectArray shaders;
	shaders.clear();
	int instanceIndex = m_dagPath.instanceNumber();
	fnMesh.getConnectedShaders(instanceIndex,shaders,shaderIndices);

	// give back an id for the shaders and possibly remap the shaderIndices
	m_shaderId = pAddShadersInterface->AddShaders(m_dagPath,shaders,shaderIndices);

	//	Multiple UV Sets
	int numUVSets = fnMesh.numUVSets();
	
	MStringArray uvSetNames;
	fnMesh.getUVSetNames(uvSetNames);

	TupArray<MIntArray> polyUVCounts;
	TupArray<MIntArray> polyUVIds;
	TupArray<int> currentUVIDArray;

	m_UVVertices.SetSize(numUVSets);
	polyUVCounts.SetSize(numUVSets);
	polyUVIds.SetSize(numUVSets);
	m_UVIndices.SetSize(numUVSets);
	m_UVNames.SetSize(numUVSets);
	currentUVIDArray.SetSize(numUVSets);
	
	int uvSetIndex;
	for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		currentUVIDArray[uvSetIndex] = 0;

		m_UVNames[uvSetIndex] = uvSetNames[uvSetIndex].asChar();

		MFloatArray uArray,vArray;
		
		//		Get UV Table
		fnMesh.getUVs(uArray,vArray,&uvSetNames[uvSetIndex]);
		m_UVVertices[uvSetIndex].SetSize(uArray.length());

		for (int uvIndex = 0; uvIndex < (int)uArray.length(); uvIndex++)
			m_UVVertices[uvSetIndex][uvIndex].Set(uArray[uvIndex],1.0f-vArray[uvIndex],0.0f);
		
		fnMesh.getAssignedUVs(polyUVCounts[uvSetIndex],polyUVIds[uvSetIndex],&uvSetNames[uvSetIndex]);
	}
	
	polyIter.reset();

	// Faces
	int numFaces = 0;
	for (polyFaceIndex=0 ; !polyIter.isDone(); polyIter.next(),polyFaceIndex++ )
	{
		int polyVertexCount = polyIter.polygonVertexCount();

		int numVertexColors;
		polyIter.numColors(numVertexColors);
		TupArray<int> polyColors;
		TupArray<int> polyAlphas;
		MColorArray colors;
		if (numVertexColors > 0)
		{
			polyIter.getColors(colors);
			if (colors.length() != polyVertexCount)
			{
				MGlobal::displayError("Vertex color problem.");
				numVertexColors=0;
			}
			else
			{
				polyColors.SetSize(polyVertexCount);
				polyAlphas.SetSize(polyVertexCount);
				for (int pointIndex = 0; pointIndex < polyVertexCount; pointIndex++ )
				{
					Vector3 newColor;
					newColor.Set(colors[pointIndex].r,colors[pointIndex].g,colors[pointIndex].b);
					FixColor(newColor);
					polyColors[pointIndex] = m_vertexColorVertices.Add(newColor);
					
					float newAlpha = colors[pointIndex].a;
					if (newAlpha<0.0f) newAlpha=0.0f;
					if (newAlpha>1.0f) newAlpha=1.0f;
					polyAlphas[pointIndex] = m_vertexAlphaVertices.Add(newAlpha);
				}
			}
		}

		TupArray<int> &triangleIndices = polyTriangleIndices[polyFaceIndex];
		int numTriangles;
		polyIter.numTriangles(numTriangles);
		
		int triangleIndex;
		int triangleOffset;
		for (triangleIndex=0,triangleOffset=0;triangleIndex<numTriangles;triangleIndex++,triangleOffset+=3)
		{
			// position
			int polyIndex1 = triangleIndices[triangleOffset];
			int polyIndex2 = triangleIndices[triangleOffset+1];
			int polyIndex3 = triangleIndices[triangleOffset+2];

			// face flags
			int faceFlags = 0;

			if (IsOutsideEdge(polyIndex1,polyIndex2,polyVertexCount))
				faceFlags |= TupTriMesh::FLAGS_EDGE_0_VISIBLITY;
			if (IsOutsideEdge(polyIndex2,polyIndex3,polyVertexCount))
				faceFlags |= TupTriMesh::FLAGS_EDGE_1_VISIBLITY;
			if (IsOutsideEdge(polyIndex3,polyIndex1,polyVertexCount))
				faceFlags |= TupTriMesh::FLAGS_EDGE_2_VISIBLITY;

			m_faceFlags.Add(faceFlags);
			
			// uv
			for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
			{
				if (polyUVCounts[uvSetIndex][polyFaceIndex])
				{
					m_UVIndices[uvSetIndex].Add(polyUVIds[uvSetIndex][currentUVIDArray[uvSetIndex]+polyIndex1]);
					m_UVIndices[uvSetIndex].Add(polyUVIds[uvSetIndex][currentUVIDArray[uvSetIndex]+polyIndex2]);
					m_UVIndices[uvSetIndex].Add(polyUVIds[uvSetIndex][currentUVIDArray[uvSetIndex]+polyIndex3]);
				}
				else
				{
					m_UVIndices[uvSetIndex].Add(0);
					m_UVIndices[uvSetIndex].Add(0);
					m_UVIndices[uvSetIndex].Add(0);
				}
			}
				
			m_faceMaterialIndices.Add(shaderIndices[polyFaceIndex]); // set material index
			m_faceShadingGroups.Add(1); // shading groups (use 1 for now)
			// vertex colors
			if (numVertexColors)
			{
				m_vertexColorIndices.Add(polyColors[polyIndex1]);
				m_vertexColorIndices.Add(polyColors[polyIndex2]);
				m_vertexColorIndices.Add(polyColors[polyIndex3]);
				
				m_vertexAlphaIndices.Add(polyAlphas[polyIndex1]);
				m_vertexAlphaIndices.Add(polyAlphas[polyIndex2]);
				m_vertexAlphaIndices.Add(polyAlphas[polyIndex3]);
			}
			else
			{
				m_vertexColorIndices.Add(0);
				m_vertexColorIndices.Add(0);
				m_vertexColorIndices.Add(0);
				
				m_vertexAlphaIndices.Add(0);
				m_vertexAlphaIndices.Add(0);
				m_vertexAlphaIndices.Add(0);
			}
			numFaces++;
		}

		// update id starts for uvs
		for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
		{
			if (polyUVCounts[uvSetIndex][polyFaceIndex])
			{
				currentUVIDArray[uvSetIndex]+=polyVertexCount;
			}
		}
  	}

#ifdef VERIFY_FINITE_VALUES
	if (!VerifyFiniteValues((float *)m_positionVertices.GetData(),m_positionVertices.GetSize()*3))
		MGlobal::displayError(MString("Warning - ") + m_dagPath.fullPathName() + MString(" has one or more infinite position vertices"));
	if (!VerifyFiniteValues((float *)m_normalVertices.GetData(),m_normalVertices.GetSize()*3))
		MGlobal::displayError(MString("Warning - ") + m_dagPath.fullPathName() + MString(" has one or more infinite vertex normal vertices"));
	if (!VerifyFiniteValues((float *)m_faceNormalVertices.GetData(),m_faceNormalVertices.GetSize()*3))
		MGlobal::displayError(MString("Warning - ") + m_dagPath.fullPathName() + MString(" has one or more infinite face normal vertices"));

	for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		if (!VerifyFiniteValues((float *)m_UVVertices[uvSetIndex].GetData(),m_UVVertices[uvSetIndex].GetSize()*3))
			MGlobal::displayError(MString("Warning - ") + m_dagPath.fullPathName() + MString(" has one or more infinite uv vertices for uvset ") + MString(m_UVNames[uvSetIndex]));
	}
#endif

	CRC meshCRCGenerator;

	// compute hash value for the mesh (not using the vertex colors)
	meshCRCGenerator.AnalyzeData(m_positionVertices.GetData(),m_positionVertices.GetSize()*sizeof(Vector3));
	meshCRCGenerator.AnalyzeData(m_positionIndices.GetData(),m_positionIndices.GetSize()*sizeof(int));
#ifdef COMPARE_NORMALS
	meshCRCGenerator.AnalyzeData(m_normalVertices.GetData(),m_normalVertices.GetSize()*sizeof(Vector3));
	meshCRCGenerator.AnalyzeData(m_normalIndices.GetData(),m_normalIndices.GetSize()*sizeof(int));
	meshCRCGenerator.AnalyzeData(m_faceNormalVertices.GetData(),m_faceNormalVertices.GetSize()*sizeof(Vector3));
	meshCRCGenerator.AnalyzeData(m_faceNormalIndices.GetData(),m_faceNormalIndices.GetSize()*sizeof(int));
#endif
	meshCRCGenerator.AnalyzeData(m_faceFlags.GetData(),m_faceFlags.GetSize()*sizeof(int));
	meshCRCGenerator.AnalyzeData(m_faceShadingGroups.GetData(),m_faceShadingGroups.GetSize()*sizeof(int));
	meshCRCGenerator.AnalyzeData(m_faceMaterialIndices.GetData(),m_faceMaterialIndices.GetSize()*sizeof(int));

	for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		meshCRCGenerator.AnalyzeData(m_UVVertices[uvSetIndex].GetData(),m_UVVertices[uvSetIndex].GetSize()*sizeof(Vector3));
		meshCRCGenerator.AnalyzeData(m_UVIndices[uvSetIndex].GetData(),m_UVIndices[uvSetIndex].GetSize()*sizeof(int));
	}

	// include the shader id
	meshCRCGenerator.AnalyzeData(m_shaderId);

	m_meshCRC = meshCRCGenerator.GetResult();

	CRC vertexColorsCRCGenerator;

	// compute the hash value for the vertex colors
	m_bHasVertexColors = true; //(m_vertexColorVertices.GetSize()>1 || m_vertexAlphaVertices.GetSize()>1);

	vertexColorsCRCGenerator.AnalyzeData(m_vertexColorVertices.GetData(),m_vertexColorVertices.GetSize()*sizeof(Vector3));
	vertexColorsCRCGenerator.AnalyzeData(m_vertexAlphaVertices.GetData(),m_vertexAlphaVertices.GetSize()*sizeof(float));
	vertexColorsCRCGenerator.AnalyzeData(m_vertexColorIndices.GetData(),m_vertexColorIndices.GetSize()*sizeof(int));
	vertexColorsCRCGenerator.AnalyzeData(m_vertexAlphaIndices.GetData(),m_vertexAlphaIndices.GetSize()*sizeof(int));

	m_vertexColorCRC = vertexColorsCRCGenerator.GetResult();
}
#endif


void MeshManager::UniqueObjectData::FlushMesh(void)
{
	m_positionVertices.RemoveAll();
	m_positionIndices.RemoveAll();
	m_normalVertices.RemoveAll();
	m_normalIndices.RemoveAll();
	m_faceNormalVertices.RemoveAll();
	m_faceNormalIndices.RemoveAll();
	m_faceFlags.RemoveAll();
#ifdef POLYGON_EXPORT
	m_faceVertexCounts.RemoveAll();
	m_faceTriangleCounts.RemoveAll();
	m_trianglePointIndices.RemoveAll();
#else
	m_faceShadingGroups.RemoveAll();
#endif
	m_faceMaterialIndices.RemoveAll();
	m_UVVertices.RemoveAll();
	m_UVIndices.RemoveAll();
}

void MeshManager::UniqueObjectData::FlushVertexColors(void)
{
	m_vertexColorVertices.RemoveAll();
	m_vertexColorIndices.RemoveAll();
	m_vertexAlphaVertices.RemoveAll();
	m_vertexAlphaIndices.RemoveAll();
}


// MeshManager methods

int MeshManager::AddMesh(MDagPath mDagPath,MDagPath mPBDagPath)
{
	MFnDependencyNode fnNode(mDagPath.node());
	int objectPtr = (int)MayaHelper::GetMPtrBase(fnNode);
	
	int oldSize = m_uniqueObjectPtrs.GetSize();
	int objectPtrIndex = m_uniqueObjectPtrs.Add(objectPtr);
	// check if we have already done this object before
	if (objectPtrIndex!=oldSize)
	{
		return objectPtrIndex;
	}

	UniqueObjectData newObjectDataTemp;
	m_uniqueObjectDataArray.Add(newObjectDataTemp);
	UniqueObjectData &newObjectData = m_uniqueObjectDataArray[objectPtrIndex];
	newObjectData.m_dagPath = mDagPath;
	newObjectData.m_PBDagPath = mPBDagPath;
	newObjectData.InitMeshData(m_pAddShadersInterface);

	int numMeshesToCheck = objectPtrIndex;
	int parentMeshIndex;
	// try to find a parent mesh
	for (parentMeshIndex=0;parentMeshIndex<numMeshesToCheck;parentMeshIndex++)
	{
		UniqueObjectData &parentMesh = m_uniqueObjectDataArray[parentMeshIndex];
		// if this is pointing to another mesh skip
		if (parentMesh.m_parentMeshIndex!=-1)
			continue;
		if (CompareMeshes(newObjectData,parentMesh))
		{
			newObjectData.m_parentMeshIndex = parentMeshIndex;
			newObjectData.FlushMesh(); // since we are pointing to a parent we don't need this stuff any more
			break;
		}
	}
	// if we found a parent mesh
	if (newObjectData.m_parentMeshIndex!=-1)
	{
		if (newObjectData.m_bHasVertexColors)
		{
			// try to find a parent vertex colors mesh
			for (int parentVertexColorsIndex=0;parentVertexColorsIndex<numMeshesToCheck;parentVertexColorsIndex++)
			{
				UniqueObjectData &parentMesh = m_uniqueObjectDataArray[parentVertexColorsIndex];
				// if pointing to the parent or a mesh that points to the parent
				if ((parentVertexColorsIndex==newObjectData.m_parentMeshIndex ||
					 parentMesh.m_parentMeshIndex == newObjectData.m_parentMeshIndex) &&
					 parentMesh.m_bHasVertexColors)
				{
					if (CompareVertexColors(newObjectData,parentMesh))
					{
						newObjectData.m_parentVertexColorIndex = parentVertexColorsIndex;
						newObjectData.FlushVertexColors(); // since we are pointing to a parent we don't need this stuff any more
						break;
					}
				}
			}
		}
	}
	return objectPtrIndex;
}

inline bool MeshManager::CompareArrays(const TupArray<int> &s1,const TupArray<int> &s2)
{
	int size = s1.GetSize();
	if (s2.GetSize() != size) return false;
	for (int index=0;index<size;index++)
	{
		if (s1[index]!=s2[index])
			return false;
	}
	return true;
}

inline bool MeshManager::CompareArrays(const TupArray<Vector3> &s1,const TupArray<Vector3> &s2)
{
	int size = s1.GetSize();
	if (s2.GetSize() != size) return false;
	for (int index=0;index<size;index++)
	{
		if (s1[index]!=s2[index])
			return false;
	}
	return true;
}

inline bool MeshManager::CompareArrays(const TupArray<float> &s1,const TupArray<float> &s2)
{
	int size = s1.GetSize();
	if (s2.GetSize() != size) return false;
	for (int index=0;index<size;index++)
	{
		if (s1[index]!=s2[index])
			return false;
	}
	return true;
}

inline bool MeshManager::CompareArrays(const TupArray<TupString> &s1,const TupArray<TupString> &s2)
{
	int size = s1.GetSize();
	if (s2.GetSize() != size) return false;
	for (int index=0;index<size;index++)
	{
		if (s1[index]!=s2[index])
			return false;
	}
	return true;
}


inline bool MeshManager::CompareArrays(const TupArray<TupArray<int> > &s1,const TupArray<TupArray<int> > &s2)
{
	int size = s1.GetSize();
	if (s2.GetSize() != size) return false;
	for (int index=0;index<size;index++)
	{
		if (!CompareArrays(s1[index],s2[index]))
			return false;
	}
	return true;
}

inline bool MeshManager::CompareArrays(const TupArray<TupArray<Vector3> > &s1,const TupArray<TupArray<Vector3> > &s2)
{
	int size = s1.GetSize();
	if (s2.GetSize() != size) return false;
	for (int index=0;index<size;index++)
	{
		if (!CompareArrays(s1[index],s2[index]))
			return false;
	}
	return true;
}

bool MeshManager::CompareMeshes(UniqueObjectData &mesh1,UniqueObjectData &mesh2)
{
	if (mesh1.m_meshCRC!=mesh2.m_meshCRC)
		return false;
	
	if (mesh1.m_shaderId != mesh2.m_shaderId) return false;
	if (!CompareArrays(mesh1.m_positionVertices,mesh2.m_positionVertices)) return false;
	if (!CompareArrays(mesh1.m_positionIndices,mesh2.m_positionIndices)) return false;
#ifdef COMPARE_NORMALS
	if (!CompareArrays(mesh1.m_normalVertices.GetArray(),mesh2.m_normalVertices.GetArray())) return false;
	if (!CompareArrays(mesh1.m_normalIndices,mesh2.m_normalIndices)) return false;
	if (!CompareArrays(mesh1.m_faceNormalVertices.GetArray(),mesh2.m_faceNormalVertices.GetArray())) return false;
	if (!CompareArrays(mesh1.m_faceNormalIndices,mesh2.m_faceNormalIndices)) return false;
#endif
	if (!CompareArrays(mesh1.m_faceFlags,mesh2.m_faceFlags)) return false;
#ifdef POLYGON_EXPORT
	if (!CompareArrays(mesh1.m_faceVertexCounts,mesh2.m_faceVertexCounts)) return false;
	if (!CompareArrays(mesh1.m_faceTriangleCounts,mesh2.m_faceTriangleCounts)) return false;
	if (!CompareArrays(mesh1.m_trianglePointIndices,mesh2.m_trianglePointIndices)) return false;
#else
	if (!CompareArrays(mesh1.m_faceShadingGroups,mesh2.m_faceShadingGroups)) return false;
#endif
	if (!CompareArrays(mesh1.m_faceMaterialIndices,mesh2.m_faceMaterialIndices)) return false;
	if (!CompareArrays(mesh1.m_UVVertices,mesh2.m_UVVertices)) return false;
	if (!CompareArrays(mesh1.m_UVIndices,mesh2.m_UVIndices)) return false;
	if (!CompareArrays(mesh1.m_UVNames,mesh2.m_UVNames)) return false;

	return true;
}

bool MeshManager::CompareVertexColors(UniqueObjectData &mesh1,UniqueObjectData &mesh2)
{
	if (mesh1.m_vertexColorCRC!=mesh2.m_vertexColorCRC)
		return false;

	if (!CompareArrays(mesh1.m_vertexColorVertices.GetArray(),mesh2.m_vertexColorVertices.GetArray())) return false;
	if (!CompareArrays(mesh1.m_vertexColorIndices,mesh2.m_vertexColorIndices)) return false;
	if (!CompareArrays(mesh1.m_vertexAlphaVertices.GetArray(),mesh2.m_vertexAlphaVertices.GetArray())) return false;
	if (!CompareArrays(mesh1.m_vertexAlphaIndices,mesh2.m_vertexAlphaIndices)) return false;

	return true;
}


