////////////////////////////////////////////////////////////////////////////
//
// CompareMeshesManager
//
// Class to handle keeping track of the unique meshes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CompareMeshes.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 2:15p
 * Updated in $/Avalanche/tools/Plugins/Maya/avatools
 * compare meshes does not care about normals
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/25/03    Time: 2:03p
 * Updated in $/Avalanche/tools/Plugins/Maya/avatools
 * compare meshes start
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/24/03    Time: 2:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * always has vertex colors
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/22/03    Time: 10:51a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * impovement on handling boned models
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/16/03    Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * new consolidation code
 * start of getting prebound mesh
 */

#include "StdAfx.h"
#include "CompareMeshes.h"
#include "CRC.h"

static const float SCALEFACTOR = .01f;

// CompareMeshesManager::UniqueObjectData methods

CompareMeshesManager::UniqueObjectData::UniqueObjectData(void) :
	m_meshCRC(0),
	m_vertexColorCRC(0),
	m_bHasVertexColors(false),
	m_parentMeshIndex(-1),
	m_parentVertexColorIndex(-1)
{
}

static void FixColor(Vector &newColor)
{
	if (newColor.x<0.0f) newColor.x=0.0f;
	if (newColor.x>1.0f) newColor.x=1.0f;
	if (newColor.y<0.0f) newColor.y=0.0f;
	if (newColor.y>1.0f) newColor.y=1.0f;
	if (newColor.z<0.0f) newColor.z=0.0f;
	if (newColor.z>1.0f) newColor.z=1.0f;
}

void CompareMeshesManager::UniqueObjectData::InitMeshData(void)
{
	MStatus status;
	MSpace::Space space = MSpace::kObject;

	MObject mComponent = MObject::kNullObj;

	Vector white;
	white.Set(1.0f,0.0f,1.0f);	// funky pink
	m_vertexColorVertices.Add(white);
	
	float opaque=1.0f;
	m_vertexAlphaVertices.Add(opaque);
	TupString dagPathString = m_dagPath.fullPathName().asChar();
	TupString bpDagPathString = m_dagPath.fullPathName().asChar();

	// Get PreBind data

	MFnDependencyNode fnNode(m_dagPath.node());
	MFnMesh fnMesh(m_dagPath, &status);
	if (status != MS::kSuccess)
	{
		fprintf(stderr,"Failure in MFnMesh initialization.\n");
		return;
	}

	// Vertex Table
	MItMeshVertex vtxIter(m_dagPath, mComponent, &status);
	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshVertex initialization.\n");
		return;
	}
	for ( ; !vtxIter.isDone(); vtxIter.next() )
	{
		MPoint point = vtxIter.position( space );
		Vector point1;
		point1.Set(point.x,-point.z,point.y); // convert from yup to zup 
		point1 *= SCALEFACTOR;
		m_positionVertices.Add(point1);
	}
	

#ifdef COMPARE_NORMALS
	// Normal Table
	TupArray<int> destNormalIndexRemaps;
	if (fnMesh.numNormals())
	{
		MFloatVectorArray sourceNormals;
		fnMesh.getNormals( sourceNormals, space );
		for (int normalIndex=0; normalIndex < sourceNormals.length(); normalIndex++ )
		{
			MFloatVector normal = sourceNormals[normalIndex];
			Vector normal1;
			normal1.Set(normal[0],-normal[2],normal[1]); // yup to zup
			destNormalIndexRemaps.Add(m_normalVertices.Add(normal1));
		}
	}
	else
	{
		Vector dummyNormal;
		dummyNormal.Set(0.0f,0.0f,1.0f);
	}
#endif

	MItMeshPolygon polyPBIter(m_dagPath, mComponent, &status);

	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshPolygon initialization.\n");
		return;
	}

	// get position, normal, and face normal data
	int polyFaceIndex = 0;

	for ( ; !polyPBIter.isDone(); polyPBIter.next() )
	{
		int polyVertexCount = polyPBIter.polygonVertexCount();

		MIntArray sourcePositionIndicies;
		MFloatArray uarray,varray;

		status = polyPBIter.getVertices(sourcePositionIndicies);

		for (int pointIndex = 2; pointIndex < polyVertexCount; pointIndex++ )
		{
			// position
			m_positionIndices.Add(sourcePositionIndicies[0]);
			m_positionIndices.Add(sourcePositionIndicies[pointIndex-1]);
			m_positionIndices.Add(sourcePositionIndicies[pointIndex]);

#ifdef COMPARE_NORMALS
			// face normal
			MVector sourceFaceNormal;
			polyPBIter.getNormal(sourceFaceNormal,space);
			Vector destFaceNormal;
			destFaceNormal.Set(sourceFaceNormal.x,-sourceFaceNormal.z,sourceFaceNormal.y);
			m_faceNormalIndices.Add(m_faceNormalVertices.Add(destFaceNormal));

			// normal
			if (fnMesh.numNormals())
			{
				m_normalIndices.Add(destNormalIndexRemaps[polyPBIter.normalIndex(0)]);
				m_normalIndices.Add(destNormalIndexRemaps[polyPBIter.normalIndex(pointIndex-1)]);
				m_normalIndices.Add(destNormalIndexRemaps[polyPBIter.normalIndex(pointIndex)]);
			}
			else
			{
				m_normalIndices.Add(0);
				m_normalIndices.Add(0);
				m_normalIndices.Add(0);
			}
#endif
		}
	}

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
	currentUVIDArray.SetSize(numUVSets);
	
	int uvSetIndex;
	for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		currentUVIDArray[uvSetIndex] = 0;

		MFloatArray uArray,vArray;
		
		//		Get UV Table
		fnMesh.getUVs(uArray,vArray,&uvSetNames[uvSetIndex]);
		m_UVVertices[uvSetIndex].SetSize(uArray.length());

		for (int uvIndex = 0; uvIndex < (int)uArray.length(); uvIndex++)
			m_UVVertices[uvSetIndex][uvIndex].Set(uArray[uvIndex],1.0f-vArray[uvIndex],0.0f);
		
		fnMesh.getAssignedUVs(polyUVCounts[uvSetIndex],polyUVIds[uvSetIndex],&uvSetNames[uvSetIndex]);
	}
	
	MItMeshPolygon polyIter(m_dagPath, mComponent, &status);

	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshPolygon initialization.\n");
		return;
	}

	// Faces
	int numFaces = 0;
	polyFaceIndex = 0;
	for ( ; !polyIter.isDone(); polyIter.next() )
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
					Vector newColor;
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

		MFloatArray uarray,varray;

		for (int pointIndex = 2; pointIndex < polyVertexCount; pointIndex++ )
		{
			// uv
			for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
			{
				if (polyUVCounts[uvSetIndex][polyFaceIndex])
				{
					m_UVIndices[uvSetIndex].Add(polyUVIds[uvSetIndex][currentUVIDArray[uvSetIndex]+0]);
					m_UVIndices[uvSetIndex].Add(polyUVIds[uvSetIndex][currentUVIDArray[uvSetIndex]+pointIndex-1]);
					m_UVIndices[uvSetIndex].Add(polyUVIds[uvSetIndex][currentUVIDArray[uvSetIndex]+pointIndex]);
				}
				else
				{
					m_UVIndices[uvSetIndex].Add(0);
					m_UVIndices[uvSetIndex].Add(0);
					m_UVIndices[uvSetIndex].Add(0);
				}
			}
			// vertex colors
			if (numVertexColors)
			{
				m_vertexColorIndices.Add(polyColors[0]);
				m_vertexColorIndices.Add(polyColors[pointIndex-1]);
				m_vertexColorIndices.Add(polyColors[pointIndex]);
				
				m_vertexAlphaIndices.Add(polyAlphas[0]);
				m_vertexAlphaIndices.Add(polyAlphas[pointIndex-1]);
				m_vertexAlphaIndices.Add(polyAlphas[pointIndex]);
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
		polyFaceIndex++;
  	}

	CRC meshCRCGenerator;

	// compute hash value for the mesh (not using the vertex colors)
	meshCRCGenerator.AnalyzeData(m_positionVertices.GetData(),m_positionVertices.GetSize()*sizeof(Vector));
	meshCRCGenerator.AnalyzeData(m_positionIndices.GetData(),m_positionIndices.GetSize()*sizeof(int));
#ifdef COMPARE_NORMALS
	meshCRCGenerator.AnalyzeData(m_normalVertices.GetData(),m_normalVertices.GetSize()*sizeof(Vector));
	meshCRCGenerator.AnalyzeData(m_normalIndices.GetData(),m_normalIndices.GetSize()*sizeof(int));
	meshCRCGenerator.AnalyzeData(m_faceNormalVertices.GetData(),m_faceNormalVertices.GetSize()*sizeof(Vector));
	meshCRCGenerator.AnalyzeData(m_faceNormalIndices.GetData(),m_faceNormalIndices.GetSize()*sizeof(int));
#endif

	for (uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		meshCRCGenerator.AnalyzeData(m_UVVertices[uvSetIndex].GetData(),m_UVVertices[uvSetIndex].GetSize()*sizeof(Vector));
		meshCRCGenerator.AnalyzeData(m_UVIndices[uvSetIndex].GetData(),m_UVIndices[uvSetIndex].GetSize()*sizeof(int));
	}

	m_meshCRC = meshCRCGenerator.GetResult();


	CRC vertexColorsCRCGenerator;

	// compute the hash value for the vertex colors
	m_bHasVertexColors = true; //(m_vertexColorVertices.GetSize()>1 || m_vertexAlphaVertices.GetSize()>1);

	vertexColorsCRCGenerator.AnalyzeData(m_vertexColorVertices.GetData(),m_vertexColorVertices.GetSize()*sizeof(Vector));
	vertexColorsCRCGenerator.AnalyzeData(m_vertexAlphaVertices.GetData(),m_vertexAlphaVertices.GetSize()*sizeof(float));
	vertexColorsCRCGenerator.AnalyzeData(m_vertexColorIndices.GetData(),m_vertexColorIndices.GetSize()*sizeof(int));
	vertexColorsCRCGenerator.AnalyzeData(m_vertexAlphaIndices.GetData(),m_vertexAlphaIndices.GetSize()*sizeof(int));

	m_vertexColorCRC = vertexColorsCRCGenerator.GetResult();
}

bool CompareMeshesManager::UniqueObjectData::QuickCompare(MDagPath mDagPath)
{
	MStatus status;
	MFnMesh fnMyMesh(m_dagPath, &status);
	if (status != MS::kSuccess)
	{
		fprintf(stderr,"Failure in MFnMesh initialization.\n");
		return false;
	}

	MFnMesh fnTestMesh(mDagPath, &status);
	if (status != MS::kSuccess)
	{
		fprintf(stderr,"Failure in MFnMesh initialization.\n");
		return false;
	}

	if (fnMyMesh.numVertices()!=fnTestMesh.numVertices()) return false;
	if (fnMyMesh.numPolygons()!=fnTestMesh.numPolygons()) return false;
	if (fnMyMesh.numUVs()!=fnTestMesh.numUVs()) return false;
	if (fnMyMesh.numFaceVertices()!=fnTestMesh.numFaceVertices()) return false;
	if (fnMyMesh.numNormals()!=fnTestMesh.numNormals()) return false;
	return true;
}


void CompareMeshesManager::UniqueObjectData::FlushMesh(void)
{
	m_positionVertices.RemoveAll();
	m_positionIndices.RemoveAll();
#ifdef COMPARE_NORMALS
	m_normalVertices.RemoveAll();
	m_normalIndices.RemoveAll();
	m_faceNormalVertices.RemoveAll();
	m_faceNormalIndices.RemoveAll();
#endif
	m_UVVertices.RemoveAll();
	m_UVIndices.RemoveAll();
}

void CompareMeshesManager::UniqueObjectData::FlushVertexColors(void)
{
	m_vertexColorVertices.RemoveAll();
	m_vertexColorIndices.RemoveAll();
	m_vertexAlphaVertices.RemoveAll();
	m_vertexAlphaIndices.RemoveAll();
}


// CompareMeshesManager methods

void CompareMeshesManager::AddSourceMesh(MDagPath mDagPath)
{
	int objectPtr = (int)MTools::GetMPtrBase(mDagPath.node());
	
	int oldSize = m_uniqueObjectPtrs.GetSize();
	int objectPtrIndex = m_uniqueObjectPtrs.Add(objectPtr);
	// check if we have already done this object before
	if (objectPtrIndex!=oldSize)
	{
		return;
	}

	UniqueObjectData newObjectDataTemp;
	m_uniqueObjectDataArray.Add(newObjectDataTemp);
	UniqueObjectData &newObjectData = m_uniqueObjectDataArray[objectPtrIndex];
	newObjectData.m_dagPath = mDagPath;
	newObjectData.InitMeshData();

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
}

bool CompareMeshesManager::CheckMesh(MDagPath mDagPath,bool bCheckVertexColors)
{
	int objectPtr = (int)MTools::GetMPtrBase(mDagPath.node());
	
	int objectPtrIndex = m_uniqueObjectPtrs.GetIndex(objectPtr);
	if (objectPtrIndex!=-1)
		return true; // if pointing to the same object as in the keeper

	int numMeshesToCheck = m_uniqueObjectPtrs.GetSize();
	
	int parentMeshIndex;
	// try to find a parent mesh
	for (parentMeshIndex=0;parentMeshIndex<numMeshesToCheck;parentMeshIndex++)
	{
		UniqueObjectData &parentMesh = m_uniqueObjectDataArray[parentMeshIndex];
		if (parentMesh.m_parentMeshIndex!=-1)
			continue;

		if (parentMesh.QuickCompare(mDagPath))
		{
			UniqueObjectData testObjectData;
			testObjectData.m_dagPath = mDagPath;
			testObjectData.InitMeshData();
			if (CompareMeshes(testObjectData,parentMesh))
			{
				if (bCheckVertexColors && parentMesh.m_bHasVertexColors)
				{
					// try to find a parent vertex colors mesh
					for (int parentVertexColorsIndex=0;parentVertexColorsIndex<numMeshesToCheck;parentVertexColorsIndex++)
					{
						UniqueObjectData &parentMesh = m_uniqueObjectDataArray[parentVertexColorsIndex];
						// if pointing to the parent or a mesh that points to the parent
						if ((parentVertexColorsIndex==parentMeshIndex ||
							parentMesh.m_parentMeshIndex == parentMeshIndex) &&
							parentMesh.m_bHasVertexColors)
						{
							if (CompareVertexColors(testObjectData,parentMesh))
							{
								return true;
							}
						}
					}
				}
				else
				{
					return true;
				}
			}
		}
	}
	return false;
}

inline bool CompareMeshesManager::CompareArrays(const TupArray<int> &s1,const TupArray<int> &s2)
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

inline bool CompareMeshesManager::CompareArrays(const TupArray<Vector> &s1,const TupArray<Vector> &s2)
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

inline bool CompareMeshesManager::CompareArrays(const TupArray<float> &s1,const TupArray<float> &s2)
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

inline bool CompareMeshesManager::CompareArrays(const TupArray<TupArray<int> > &s1,const TupArray<TupArray<int> > &s2)
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

inline bool CompareMeshesManager::CompareArrays(const TupArray<TupArray<Vector> > &s1,const TupArray<TupArray<Vector> > &s2)
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

bool CompareMeshesManager::CompareMeshes(UniqueObjectData &mesh1,UniqueObjectData &mesh2)
{
	if (mesh1.m_meshCRC!=mesh2.m_meshCRC)
		return false;
	
	if (!CompareArrays(mesh1.m_positionVertices,mesh2.m_positionVertices)) return false;
	if (!CompareArrays(mesh1.m_positionIndices,mesh2.m_positionIndices)) return false;
#ifdef COMPARE_NORMALS
	if (!CompareArrays(mesh1.m_normalVertices.GetArray(),mesh2.m_normalVertices.GetArray())) return false;
	if (!CompareArrays(mesh1.m_normalIndices,mesh2.m_normalIndices)) return false;
	if (!CompareArrays(mesh1.m_faceNormalVertices.GetArray(),mesh2.m_faceNormalVertices.GetArray())) return false;
	if (!CompareArrays(mesh1.m_faceNormalIndices,mesh2.m_faceNormalIndices)) return false;
#endif
	if (!CompareArrays(mesh1.m_UVVertices,mesh2.m_UVVertices)) return false;
	if (!CompareArrays(mesh1.m_UVIndices,mesh2.m_UVIndices)) return false;

	return true;
}

bool CompareMeshesManager::CompareVertexColors(UniqueObjectData &mesh1,UniqueObjectData &mesh2)
{
	if (mesh1.m_vertexColorCRC!=mesh2.m_vertexColorCRC)
		return false;

	if (!CompareArrays(mesh1.m_vertexColorVertices.GetArray(),mesh2.m_vertexColorVertices.GetArray())) return false;
	if (!CompareArrays(mesh1.m_vertexColorIndices,mesh2.m_vertexColorIndices)) return false;
	if (!CompareArrays(mesh1.m_vertexAlphaVertices.GetArray(),mesh2.m_vertexAlphaVertices.GetArray())) return false;
	if (!CompareArrays(mesh1.m_vertexAlphaIndices,mesh2.m_vertexAlphaIndices)) return false;

	return true;
}


