////////////////////////////////////////////////////////////////////////////
//
// makeobb
//
// Make an obb set from an instance
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: makeobb.cpp $
 * 
 * *****************  Version 34  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:44a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 33  *****************
 * User: Tyler Colbert Date: 6/12/03    Time: 6:19p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * fixed some vector3 stuff
 * 
 * *****************  Version 32  *****************
 * User: Tyler Colbert Date: 6/12/03    Time: 6:04p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * fixed some vector3 stuff
 * 
 * *****************  Version 31  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * 
 * *****************  Version 30  *****************
 * User: Food         Date: 4/28/03    Time: 3:23p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * 
 * *****************  Version 29  *****************
 * User: Food         Date: 3/12/03    Time: 6:07p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * 
 * *****************  Version 28  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * update for streaming system
 * 
 * *****************  Version 27  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 11:33a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * new collision attribute system (using image names)
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 7/23/02    Time: 2:40p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Keeper usage
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Added support for attrib tree
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * hybrid configuration
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 3:53p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * First Pass of Attributes
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 1:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * OBBWorld is now online... Just needs to get the node and face
 * attributes
 */

#include "Pch.h"

#include "makeobb.h"

/***************************************************************************/
/***************************************************************************/
MakeOBB::MakeOBB(TupNode &tupNode,TupTriMesh &tupTriMesh,TupperwareAggregate *pVertexColorMapAgg,TupArray<FaceCollisionManager::Attributes> &faceAttributeArray,TupArray<int> &faceAttributeIndices) :
	m_tupNode(tupNode),
	m_tupTriMesh(tupTriMesh),
	m_obbList(8, 4, false),
	m_pColorArray(NULL),
	m_pColorIndices(NULL),
	m_numColorFaces(0),
	m_faceAttributeIndices(faceAttributeIndices),
	m_faceAttributeArray(faceAttributeArray)
{
	AnnotateHelper::BuildVertexColors(pVertexColorMapAgg,(AnnotateHelper::Color **)&m_pColorArray,&m_pColorIndices,m_numColorFaces);
}

/***************************************************************************/
/***************************************************************************/
MakeOBB::~MakeOBB(void)
{
	delete [] m_pColorArray,
	delete [] m_pColorIndices;
}

/***************************************************************************/
// Make an instance into an OBB
/***************************************************************************/
int MakeOBB::Build(const Vector3 *shrinkage,float minDimension)
{
	char *pNodeName;
	m_tupNode.GetName(&pNodeName);
	// Get the transform of this node
	TupperwareAggregate *pTransformAgg = m_tupNode.GetTransform();
	assert(pTransformAgg);
	TupTransform tupTransform(pTransformAgg);
	TupTransform::TransformTypes transformType = tupTransform.GetTransformType();
	assert(transformType == TupTransform::STATIC_MATRIX);
	TupStaticMatrix tupStaticMatrix(pTransformAgg);
	float *pFloatList;
	tupStaticMatrix.GetMatrix(&pFloatList);
	assert(pFloatList);
	Graphics4x4 &localToWorld = *(Graphics4x4 *)pFloatList;
//	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Processing Instance %s Pos %f,%f,%f",pNodeName,localToWorld.tx,localToWorld.ty,localToWorld.tz);

	int numVerts = CountVertices();
	if (numVerts==0)
		return 0;

	// Build the transformed vertex list
	Vertex *pVertexArray = BuildVertexArray(numVerts, localToWorld); // sets numVerts to the number of unique numVerts
	assert(pVertexArray);
	// Make a spot to untransform vertices
	Vector3 *pUntransformedVertexArray = new Vector3[numVerts];

	// count the number of OBB's in this instance
	OBBType *pObbTypeArray = new OBBType[MAX_OBBSPERINSTANCE];
	int numObbs = CountOBBs(pVertexArray, numVerts, pObbTypeArray);
	assert(numObbs != 0);

	// Stip the instance matrix into an orientation
	DirCos3x3 orientation;
	StripOrientation(localToWorld, orientation);

	// Loop through the obb's
	int actualOBBs = 0;
	for (int i = 0 ; i < numObbs; i++)
	{
		// What is the centroid of the list?
		Vector3 center;
		if (!ComputeCentroid(pVertexArray, numVerts, center, pObbTypeArray[i]))
			continue;

		// "Untransform" the vertices
		int numUtVerts = UntransformVertices(pVertexArray,numVerts,orientation,center,pObbTypeArray[i],pUntransformedVertexArray);
		assert(numUtVerts != 0);

		// Now compute the AABB
		AABB aabb;
		ComputeAABB(pUntransformedVertexArray, numUtVerts, aabb);

		// Shift the aabb?
		Vector3 origin = (aabb.maxV + aabb.minV) * 0.5f;
		aabb.maxV -= origin;
		aabb.minV -= origin;
		Vector3 worigin = origin * orientation;
		center += worigin;

		// Shrink the obb?
		if (shrinkage)
		{
			aabb.minV = aabb.minV.Diag(*shrinkage);
			aabb.maxV = -aabb.minV;
		}

		// Are any of the dimensions too small?
		// Don't forget that minV = -maxV
		if (aabb.maxV.x( ) < minDimension)
		{
			aabb.maxV.x( minDimension );
			aabb.minV.x( -minDimension );
		}
		if (aabb.maxV.y( ) < minDimension)
		{
			aabb.maxV.y( minDimension );
			aabb.minV.y( -minDimension );
		}
		if (aabb.maxV.z( ) < minDimension)
		{
			aabb.maxV.z( minDimension );
			aabb.minV.z( -minDimension );
		}

		// Calculate average color for obb
		u16 color = ComputeColor(pVertexArray, numVerts, pObbTypeArray[i]);

		// Create a new obb
		OBBStaticInstance *obb = new OBBStaticInstance;

		// setup the innards
		obb->SetBoundingVolume(&aabb, false);
		obb->SetTransform(orientation, center);
		obb->SetMaterial(pObbTypeArray[i].m_surfaceType);
		obb->SetColor(color);

		// Add it
		if (m_obbList.Add(obb))
			++actualOBBs;
	}

	// delete the vertex lists!
	delete [] pVertexArray;
	delete [] pUntransformedVertexArray;
	delete [] pObbTypeArray;

	return(actualOBBs);
}

//***************************************************************************
// Get an obb (you own it now, it is gone from this list)
//***************************************************************************
OBBStaticInstance *MakeOBB::Get(void)
{
	int count = m_obbList.Count();
	if (count == 0)
		return(NULL);

	OBBStaticInstance *ret = m_obbList.Get(count - 1);
	m_obbList.Delete(count - 1);
	return(ret);
}

//***************************************************************************
// Strip the orientation from this 4x4
//***************************************************************************
void MakeOBB::StripOrientation(Graphics4x4 &localToWorld,DirCos3x3 &orientation)
{
#if 0
	// Copy in the 3x3
	orientation = *(Cartesian4x4*)&localToWorld;

	// What is the scale?
	Vector3 scale;
	scale.Set(Math::Sqrt(orientation.e00 * orientation.e00 + orientation.e01 * orientation.e01 + orientation.e02 * orientation.e02),
				 Math::Sqrt(orientation.e10 * orientation.e10 + orientation.e11 * orientation.e11 + orientation.e12 * orientation.e12),
				 Math::Sqrt(orientation.e20 * orientation.e20 + orientation.e21 * orientation.e21 + orientation.e22 * orientation.e22));

	// Remove the scaling from the matrix
	Vector3 invScale;
	invScale.Set(1.0f / scale.x(),
					 1.0f / scale.y(),
					 1.0f / scale.z());
	orientation.e00 *= invScale.x(); orientation.e01 *= invScale.x(); orientation.e02 *= invScale.x();
	orientation.e10 *= invScale.y(); orientation.e11 *= invScale.y(); orientation.e12 *= invScale.y();
	orientation.e20 *= invScale.z(); orientation.e21 *= invScale.z(); orientation.e22 *= invScale.z();
#else
	// Copy these into some nicer vectors
	Vector3 x, y, z;
	x = localToWorld.GetRow0( );
	y = localToWorld.GetRow1( );
	z = localToWorld.GetRow2( );

	// compute the scale
	Vector3 scale;
	scale.x( x.Length() );
	x.Normalize();

	// Figure out the XY shear and fix Y
	float shearXY = Vector3::Dot(x, y);
	y -= x * shearXY;
	scale.y( y.Length() );
	y.Normalize();
	shearXY /= scale.y( );

	// Figure out the XZ & YZ shears
 	float shearXZ = Vector3::Dot(x, z);
	z -= x * shearXZ;
	float shearYZ = Vector3::Dot(y, z);
	z -= y * shearYZ;

 	/* Next, get Z scale and normalize 3rd row. */
 	scale.z( z.Length() );
	z.Normalize();
	shearXZ /= scale.z( );
	shearYZ /= scale.z( );

 	// Check for a coordinate system flip.
	if (Vector3::Dot(x, Vector3::Cross(y, z)) < 0)
	{
		scale *= -1.0f;
		x *= -1.0f;
		y *= -1.0f;
		z *= -1.0f;
	}

	// Fix this bastard up!
	z = Vector3::Cross(x, y);
	z.Normalize();
	y = Vector3::Cross(z, x);

	orientation.SetRow0(x);
	orientation.SetRow1(y);
	orientation.SetRow2(z);
#endif
}

//***************************************************************************
//***************************************************************************
int MakeOBB::CountVertices(void)
{
	int numFaces;
	// get number of faces
	int *pPositionIndices;
	m_tupTriMesh.GetPositionIndices(&pPositionIndices,numFaces);

	if (numFaces == 0)
		return 0;

	if (m_faceAttributeIndices.GetSize()==1)
	{
		// all collision?
		if (m_faceAttributeArray[0].m_bCollision)
		{
			return numFaces*3;
		}
		// none collision?
		else
		{
			return 0;
		}
	}
	else // check the mesh
	{
		int numVerts = 0;
		// Do the polys
		for (int faceIndex=0,faceOffset=0;faceIndex<numFaces;faceIndex++,faceOffset+=3)
		{
			int attributeIndex = (faceIndex<m_faceAttributeIndices.GetSize()) ? faceIndex : 0;
			// check if this face has collision
			if (m_faceAttributeArray[m_faceAttributeIndices[attributeIndex]].m_bCollision)
			{
				numVerts +=3;
			}
		}
		return numVerts;
	}
}

//***************************************************************************
//***************************************************************************
MakeOBB::Vertex *MakeOBB::BuildVertexArray(int &numVerts,Graphics4x4 &localToWorld)
{
	assert(numVerts);

	int numFaces;
	// do positions
	int numPositionVerts;
	Vector3 *pPositionVerts;
	int *pPositionIndices;
	m_tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	m_tupTriMesh.GetPositionIndices(&pPositionIndices,numFaces);

	// do normals
	int numNormalVerts;
	Vector3 *pNormalVerts;
	int *pNormalIndices;
	m_tupTriMesh.GetNormalVerts((float **)&pNormalVerts,numNormalVerts);
	m_tupTriMesh.GetNormalIndices(&pNormalIndices,numFaces);

	if (numFaces == 0)
	{
		numVerts = 0;
		return(NULL);
	}

	if (m_faceAttributeIndices.GetSize()!=1 || m_faceAttributeArray[0].m_bCollision)
	{
		Keeper<Vertex> vertexKeeper;
		// Do the polys
		for (int faceIndex=0,faceOffset=0;faceIndex<numFaces;faceIndex++,faceOffset+=3)
		{
			int attributeIndex = (faceIndex<m_faceAttributeIndices.GetSize()) ? faceIndex : 0;
			if (m_faceAttributeArray[m_faceAttributeIndices[attributeIndex]].m_bCollision)
			{
				int colorIndex = (faceIndex<m_numColorFaces) ? faceOffset : 0;

				// Transform it
				for (int pointIndex=0;pointIndex<3;pointIndex++)
				{
					Vertex tempV;
					tempV.m_vertex = pPositionVerts[pPositionIndices[faceOffset+pointIndex]] * localToWorld;
					tempV.m_surfaceType = m_faceAttributeArray[m_faceAttributeIndices[attributeIndex]].m_surfaceType;
					tempV.m_obbValue = m_faceAttributeArray[m_faceAttributeIndices[attributeIndex]].m_obbValue;
					Vector3 normal = pNormalVerts[pNormalIndices[faceOffset+pointIndex]].BasisTransform(localToWorld);
					// if the normal is pointing generally up use the color
					if (normal.y() > 0.7f)
						tempV.m_color =	m_pColorArray[m_pColorIndices[colorIndex+pointIndex]];
					else
						memset(&tempV.m_color, 0, sizeof(ts_bRGBA));
					// keep track of the unique points
					vertexKeeper.Add(tempV);
				}
			}
		}
		int uniqueCount = vertexKeeper.GetSize();
		assert(uniqueCount<=numVerts);
		if (uniqueCount)
		{
			// allocate just what we need
			Vertex *pPopulatedVertexArray = new Vertex[uniqueCount];
			memcpy(pPopulatedVertexArray,vertexKeeper.GetData(),uniqueCount*sizeof(Vertex));
			// This our real unique vertex count
			numVerts = uniqueCount;
			// return the populated list
			return(pPopulatedVertexArray);
		}
	}

	numVerts = 0;
	return NULL;
}

//***************************************************************************
// How many OBB's of each type are there?
//***************************************************************************
int MakeOBB::CountOBBs(Vertex *pVertexArray,int numVerts,OBBType *pObbTypeArray)
{
	int numObbTypes = 0;
	for (int v = 0; v < numVerts; v++)
	{
		if (FindOBBType(pObbTypeArray, numObbTypes, pVertexArray[v].m_surfaceType, pVertexArray[v].m_obbValue) < 0)
		{
			assert(numObbTypes < MAX_OBBSPERINSTANCE);
			pObbTypeArray[numObbTypes].m_surfaceType = pVertexArray[v].m_surfaceType;
			pObbTypeArray[numObbTypes].m_obbValue = pVertexArray[v].m_obbValue;
			++numObbTypes;
		}
	}

	return(numObbTypes);
}

//***************************************************************************
// Find this OBBtype in the array
//***************************************************************************
int MakeOBB::FindOBBType(OBBType *pObbTypeArray,int numObbTypes,u8 material,u8 obbIndex)
{
	// Scan the list for a match
	for (int i = (int)numObbTypes - 1; i >= 0; i--)
	{
		if (pObbTypeArray[i].m_surfaceType == material && pObbTypeArray[i].m_obbValue == obbIndex)
			return(i);
	}

	// Didn't find it
	return(-1);
}


//***************************************************************************
//***************************************************************************
bool MakeOBB::ComputeCentroid(Vertex *pVertexArray,int numVerts,Vector3 &centroid,OBBType &type)
{
	// Nothing yet
	centroid.Clear();
	int matches = 0;

	// Scan the list
	for (int i = 0; i < numVerts; i++)
	{
		if (Match(type, pVertexArray[i]))
		{
			centroid += pVertexArray[i].m_vertex;
			++matches;
		}
	}

	// Did we get any matches?
	if (matches == 0)
		return(false);

	// Compute the centroid
	centroid /= (float)matches;
	return(true);
}

//***************************************************************************
//***************************************************************************
int MakeOBB::UntransformVertices(Vertex *pVertexArray,int numVerts,DirCos3x3 &orientation,Vector3 &centroid,OBBType &type,Vector3 *pUntransformedVertexArray)
{
	int matches = 0;
	for (int i = 0; i < numVerts; i++)
	{
		if (Match(type, pVertexArray[i]))
			pUntransformedVertexArray[matches++] = (pVertexArray[i].m_vertex - centroid) / orientation;
	}

	return(matches);
}

//***************************************************************************
//***************************************************************************
void MakeOBB::ComputeAABB(Vector3 *pUntransformedVertexArray,int numVerts,AABB &aabb)
{
	// duh
	if (numVerts == 0)
	{
		aabb.minV.Clear();
		aabb.maxV.Clear();
		return;
	}

	// compute by scanning
	aabb.minV = pUntransformedVertexArray[0];
	aabb.maxV = pUntransformedVertexArray[0];
	for (int v = 1; v < numVerts; v++)
	{
		aabb.minV = Vector3::Min(pUntransformedVertexArray[v], aabb.minV);
		aabb.maxV = Vector3::Max(pUntransformedVertexArray[v], aabb.maxV);

/*		if (pUntransformedVertexArray[v].x < aabb.minV.x)
			aabb.minV.x = pUntransformedVertexArray[v].x;
		if (pUntransformedVertexArray[v].x > aabb.maxV.x)
			aabb.maxV.x = pUntransformedVertexArray[v].x;
		if (pUntransformedVertexArray[v].y < aabb.minV.y)
			aabb.minV.y = pUntransformedVertexArray[v].y;
		if (pUntransformedVertexArray[v].y > aabb.maxV.y)
			aabb.maxV.y = pUntransformedVertexArray[v].y;
		if (pUntransformedVertexArray[v].z < aabb.minV.z)
			aabb.minV.z = pUntransformedVertexArray[v].z;
		if (pUntransformedVertexArray[v].z > aabb.maxV.z)
			aabb.maxV.z = pUntransformedVertexArray[v].z;*/
	}
}

//***************************************************************************
//***************************************************************************
u16 MakeOBB::ComputeColor(Vertex *pVertexArray,int numVerts,OBBType &type)
{
	// average all non-black vertices to get color (black vertices are from "non-standable" polys so they aren't considered)
	int nRed = 0;
	int nGreen = 0;
	int nBlue = 0;
	int nVerts = 0;

	// Scan the list
	for (int v = 0; v < numVerts; v++)
	{
		if (Match(type, pVertexArray[v]))
		{
			if ((pVertexArray[v].m_color.R != 0) || (pVertexArray[v].m_color.G != 0) || (pVertexArray[v].m_color.B != 0))
			{
				nRed += pVertexArray[v].m_color.R;
				nGreen += pVertexArray[v].m_color.G;
				nBlue += pVertexArray[v].m_color.B;
				nVerts++;
			}
		}
	}

	// Did we get any matches?
	if (nVerts == 0)
		return(0);	// return black

	// Compute the average
	nRed /= nVerts;
	nGreen /= nVerts;
	nBlue /= nVerts;
	u16 color = 0;
	color |= (((nRed >> 3) & 0x1f) << 0);
	color |= (((nGreen >> 2) & 0x3f) << 5);
	color |= (((nBlue >> 3) & 0x1f) << 11);
	return(color);
}



