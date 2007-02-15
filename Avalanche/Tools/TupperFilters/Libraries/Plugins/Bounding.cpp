////////////////////////////////////////////////////////////////////////////
//
// Bounding
//
// Class to creating bounding boxes and spheres
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Bounding.cpp $
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:03a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 10  *****************
 * User: Tyler Colbert Date: 6/12/03    Time: 6:20p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * fixed some vector3 stuff
 * 
 * *****************  Version 9  *****************
 * User: Tyler Colbert Date: 6/12/03    Time: 6:06p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 8  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 4/28/03    Time: 3:30p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * new strip orientation code (from Jeff G)
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:25p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * new bounding functions for OBB
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * update for streaming system
 * 
 * *****************  Version 4  *****************
 * User: Rob Nelson   Date: 9/16/02    Time: 3:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 1:01p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Added bounding and annotate helper classes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 */

////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "Bounding.h"
#include "AnnotateHelper.h"

void Bounding::CalculateBoundingBox(const Vector3 *pVertices,int numVertices,Vector3 &min,Vector3 &max)
{
	Clear(min,max);
	for (int i=0;i<numVertices;i++)
	{
		const Vector3 &point=pVertices[i];
		AddToBoundingBox(min,max,point);
	}
}

void Bounding::CalculateBoundingSphere(const Vector3 *pVertices,int numVertices,const Vector3 &min,const Vector3 &max,float &radius,Vector3 &center)
{
	center.x( 0.5f * ( min.x( ) + max.x( ) ) );
	center.y( 0.5f * ( min.y( ) + max.y( ) ) );
	center.z( 0.5f * ( min.z( ) + max.z( ) ) );

	radius = 0.0f;
	Vector3 delta;
	float distanceSq;

	for (int i=0;i<numVertices;i++)
	{
		const Vector3 &point=pVertices[i];
		delta.x( point.x( ) - center.x( ) );
		delta.y( point.y( ) - center.y( ) );
		delta.z( point.z( ) - center.z( ) );
		distanceSq = Vector3::LengthSquared(delta); 
		if (distanceSq>radius)
			radius=distanceSq;
	}
	// only do the sqaure root once
	radius = Math::Sqrt(radius);
}

void Bounding::AddToBoundingBox(Vector3 &min,Vector3 &max,const Vector3 &add)
{
	if (add.x( ) < min.x( ) ) min.x( add.x( ) );
	if (add.y( ) < min.y( ) ) min.y( add.y( ) );
	if (add.z( ) < min.z( ) ) min.z( add.z( ) );
	if (add.x( ) > max.x( ) ) max.x( add.x( ) );
	if (add.y( ) > max.y( ) ) max.y( add.y( ) );
	if (add.z( ) > max.z( ) ) max.z( add.z( ) );
}

void Bounding::Clear(Vector3 &min,Vector3 &max)
{
	min.x( MAXFLOAT32 ); min.y( MAXFLOAT32 ); min.z( MAXFLOAT32 );
	max.x( -MAXFLOAT32 ); max.y( -MAXFLOAT32 ); max.z( -MAXFLOAT32 );
}

bool Bounding::ContainsPointXZ(const Vector3 &min,const Vector3 &max,const Vector3 &point)
{
	return((point.x( ) >= min.x( ) ) &&
			 (point.z( ) >= min.z( ) ) &&
			 (point.x( ) <= max.x( ) ) &&
			 (point.z( ) <= max.z( ) ) );
}

bool Bounding::IsEmpty(Vector3 &min,Vector3 &max)
{
	return(min.x() == MAXFLOAT32);
}

bool Bounding::ContainsPoint(const Vector3 &min,const Vector3 &max,const Vector3 &point)
{
	return(	( point.x( ) >= min.x( ) ) &&
				( point.y( ) >= min.y( ) ) &&
				( point.z( ) >= min.z( ) ) &&
				( point.x( ) <= max.x( ) ) &&
				( point.y( ) <= max.y( ) ) &&
				( point.z( ) <= max.z( ) ) );
}

// returns true if the coordinates specified by min2,max2 are contained completely within min1,max1
bool Bounding::ContainsBox(const Vector3 &min1,const Vector3 &max1,const Vector3 &min2,const Vector3 &max2)
{
	return( ContainsPoint( min1, max1, min2) && ContainsPoint( min1, max1, max2 ) );
}

// return a value 0-1 indiciating the % of the area of min2,max2 that is contained within min1,max1
// if a 0 is returned none of min2,max2 is within min1,max1
// if a 1 is returned all of min2,max2 is within min1,max2
float Bounding::VolumeInsideBox(const Vector3 &min1,const Vector3 &max1,const Vector3 &min2,const Vector3 &max2)
{
	float myArea = GetVolume(min2,max2);
	if (myArea>0.0f)
	{
		Vector3 intersectionMin,intersectionMax;
		// if I can get an intersecion
		if (GetIntersection(min1,max1,min2,max2,intersectionMin,intersectionMax))
		{
			float intersecionArea = GetVolume(intersectionMin,intersectionMax);
			return intersecionArea/myArea; // return ratio between the areas
		}
	}
	return 0.0f;
}

float Bounding::GetVolume(const Vector3 &min,const Vector3 &max)
{
	return ( max.x( ) - min.x( ) ) * ( max.y( ) - min.y( ) ) * ( max.z( ) - min.z( ) );
}

// returns true if it the boxes intersect
bool Bounding::GetIntersection(const Vector3 &min1,const Vector3 &max1,const Vector3 &min2,const Vector3 &max2,Vector3 &intersectionMin,Vector3 &intersectionMax)
{
	if (ContainsPoint( min1, max1, min2) || ContainsPoint( min1, max1, max2 ) )
	{
		// pick the greatest min values
		intersectionMin.x( ( min1.x( ) > min2.x( ) ) ? min1.x( ) : min2.x( ) );
		intersectionMin.y( ( min1.y( ) > min2.y( ) ) ? min1.y( ) : min2.y( ) );
		intersectionMin.z( ( min1.z( ) > min2.z( ) ) ? min1.z( ) : min2.z( ) );
		// pick the smallest max values
		intersectionMax.x( ( max1.x( ) < max2.x( ) ) ? max1.x( ) : max2.x( ) );
		intersectionMax.y( ( max1.y( ) < max2.y( ) ) ? max1.y( ) : max2.y( ) );
		intersectionMax.z( ( max1.z( ) < max2.z( ) ) ? max1.z( ) : max2.z( ) );
		return true;
	}
	return false;
}												  

void Bounding::AddObjectBounding(TupTriMesh &tupTriMesh)
{
	TupperwareAggregate *pObjectAgg = tupTriMesh.GetBaseAggregate();

	// purge these out
	pObjectAgg->DeleteAtomsByKey("Bounding_BoundingBoxMin");
	pObjectAgg->DeleteAtomsByKey("Bounding_BoundingBoxMax");
	pObjectAgg->DeleteAtomsByKey("Bounding_BoundingSphereCenter");
	pObjectAgg->DeleteAtomsByKey("Bounding_BoundingSphereRadius");

	Vector3 *pPositionVerts;
	int numVerts;
	tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numVerts);
	Vector3 boundingBoxMin;
	Vector3 boundingBoxMax;
	
	Bounding::CalculateBoundingBox(pPositionVerts,numVerts,boundingBoxMin,boundingBoxMax);
	pObjectAgg->AddListFloat("Bounding_BoundingBoxMin",NULL,(float *)&boundingBoxMin,3);
	pObjectAgg->AddListFloat("Bounding_BoundingBoxMax",NULL,(float *)&boundingBoxMax,3);
	
	Vector3 boundingBoxCenter;
	boundingBoxCenter = (boundingBoxMin + boundingBoxMax) * 0.5f;
	
	Vector3 boundingSphereCenter;
	float boundingSphereRadius;
	Bounding::CalculateBoundingSphere(pPositionVerts,numVerts,boundingBoxMin,boundingBoxMax,boundingSphereRadius,boundingSphereCenter);
	pObjectAgg->AddListFloat("Bounding_BoundingSphereCenter",NULL,(float *)&boundingSphereCenter,3);
	pObjectAgg->AddScalarFloat("Bounding_BoundingSphereRadius",NULL,boundingSphereRadius);
}

bool Bounding::AddNodeBounding(TupNode &tupNode,TupObjectPool &tupObjectPool)
{
	TupperwareAggregate *pNodeAgg = tupNode.GetBaseAggregate();
	pNodeAgg->DeleteAtomsByKey("Bounding_BoundingBoxMin");
	pNodeAgg->DeleteAtomsByKey("Bounding_BoundingBoxMax");
	pNodeAgg->DeleteAtomsByKey("Bounding_BoundingSphereCenter");
	pNodeAgg->DeleteAtomsByKey("Bounding_BoundingSphereRadius");
	pNodeAgg->DeleteAtomsByKey("Bounding_OBBOrientation");
	pNodeAgg->DeleteAtomsByKey("Bounding_OBBCenter");
	pNodeAgg->DeleteAtomsByKey("Bounding_OBBExtents");

	TupperwareAggregate *pTransformAgg = tupNode.GetTransform();
	if (!pTransformAgg)
		return false;

	Graphics4x4 mLocalToWorld;
	TupTransform tupTransform(pTransformAgg);
	if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
	{
		TupStaticMatrix tupStaticMatrix(pTransformAgg);
		float *pMatrix;
		tupStaticMatrix.GetMatrix(&pMatrix);
		
		mLocalToWorld = Graphics4x4(
			pMatrix[0], pMatrix[1], pMatrix[2],
			pMatrix[4], pMatrix[5], pMatrix[6],
			pMatrix[8], pMatrix[9], pMatrix[10],
			pMatrix[12], pMatrix[13], pMatrix[14]);      // translation
	}
	else
	{
		mLocalToWorld.Identity(); // if we don't have a static matrix set to identity
	}
			
	int objectRef;	
	tupNode.GetObjectReference(objectRef);

	if (objectRef==-1)
		return false;

	TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(tupObjectPool,objectRef);
	if (!pBaseMeshAgg)
		return false;

	TupTriMesh tupTriMesh(pBaseMeshAgg);
	Vector3 *pPositionVerts;
	int numVerts;
	tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numVerts);

	if (!numVerts)
		return false;
	
	// create an array of verts to hold the transformed positions
	Vector3 *pTransformedVerts = new Vector3[numVerts];
	for (int vertexIndex=0;vertexIndex<numVerts;vertexIndex++)
	{
		pTransformedVerts[vertexIndex] = pPositionVerts[vertexIndex]*mLocalToWorld;
	}
	
	Vector3 boundingBoxMin;
	Vector3 boundingBoxMax;
	
	CalculateBoundingBox(pTransformedVerts,numVerts,boundingBoxMin,boundingBoxMax);
	pNodeAgg->AddListFloat("Bounding_BoundingBoxMin",NULL,(float *)&boundingBoxMin,3);
	pNodeAgg->AddListFloat("Bounding_BoundingBoxMax",NULL,(float *)&boundingBoxMax,3);
	
	Vector3 boundingSphereCenter;
	float boundingSphereRadius;
	Bounding::CalculateBoundingSphere(pTransformedVerts,numVerts,boundingBoxMin,boundingBoxMax,boundingSphereRadius,boundingSphereCenter);
	pNodeAgg->AddListFloat("Bounding_BoundingSphereCenter",NULL,(float *)&boundingSphereCenter,3);
	pNodeAgg->AddScalarFloat("Bounding_BoundingSphereRadius",NULL,boundingSphereRadius);

	// compute the obb for the node
	DirCos3x3 orientation;
	StripOrientation(mLocalToWorld, orientation);

	Vector3 center;
	ComputeCentroid(pTransformedVerts, numVerts, center);

	// "Untransform" the vertices
	Vector3 *pUntransformedVerts = new Vector3[numVerts];
	UntransformVertices(pTransformedVerts,numVerts,orientation,center,pUntransformedVerts);
	
	// Now compute the AABB
	Vector3 unTransformedBoxMin;
	Vector3 unTransformedBoxMax;
	CalculateBoundingBox(pUntransformedVerts,numVerts,unTransformedBoxMin,unTransformedBoxMax);
	
	// Shift the aabb?
	Vector3 origin = (unTransformedBoxMax + unTransformedBoxMin) * 0.5f;
	unTransformedBoxMax -= origin;
	unTransformedBoxMin -= origin;
	Vector3 worigin = origin * orientation;
	center += worigin;

	pNodeAgg->AddListFloat("Bounding_OBBOrientation",NULL,(float *)&orientation,9);
	pNodeAgg->AddListFloat("Bounding_OBBCenter",NULL,(float *)&center,3);
	pNodeAgg->AddListFloat("Bounding_OBBExtents",NULL,(float *)&unTransformedBoxMax,3);
	
	delete [] pTransformedVerts;
	delete [] pUntransformedVerts;

	return true;
}

//***************************************************************************
// Strip the orientation from this 4x4
//***************************************************************************
void Bounding::StripOrientation(Graphics4x4 &localToWorld,DirCos3x3 &orientation)
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
	scale.x( x.Length( ) );
	x.SafeNormalize();
	
	// Figure out the XY shear and fix Y
	float shearXY = Vector3::Dot(x, y);
	y -= x * shearXY;
	scale.y( y.Length( ) );
	y.SafeNormalize();
	shearXY /= scale.y( );
	
	// Figure out the XZ & YZ shears
	float shearXZ = Vector3::Dot(x, z);
	z -= x * shearXZ;
	float shearYZ = Vector3::Dot(y, z);
	z -= y * shearYZ;
	
	/* Next, get Z scale and normalize 3rd row. */
	scale.z( z.Length( ) );
	z.SafeNormalize();
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
	z.SafeNormalize();
	y = Vector3::Cross(z, x);
	
	orientation.SetRow0(x);
	orientation.SetRow1(y);
	orientation.SetRow2(z);
#endif
}

//***************************************************************************
//***************************************************************************
void Bounding::ComputeCentroid(Vector3 *pVectorArray,int numVerts,Vector3 &centroid)
{
	// Nothing yet
	centroid.Clear();
	for (int i = 0; i < numVerts; i++)
	{
		centroid += pVectorArray[i];
	}
	// Compute the centroid
	centroid /= (float)numVerts;
}

//***************************************************************************
//***************************************************************************
void Bounding::UntransformVertices(Vector3 *pTransformedVerts,int numVerts,DirCos3x3 &orientation,Vector3 &centroid,Vector3 *pUntransformedVerts)
{
	for (int i = 0; i < numVerts; i++)
	{
		pUntransformedVerts[i] = (pTransformedVerts[i] - centroid) / orientation;
	}
}

