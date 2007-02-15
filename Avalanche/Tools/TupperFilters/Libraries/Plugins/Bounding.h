////////////////////////////////////////////////////////////////////////////
//
// Bounding
//
// Class to creating bounding boxes and spheres
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Bounding.h $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:03a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 7  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
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

#ifndef TUPPERFILTERS_LIB_PLUGINS_BOUNDING_H
#define TUPPERFILTERS_LIB_PLUGINS_BOUNDING_H

//class Vector3;
class Graphics4x4;
class DirCos3x3;
class AABB;

class Bounding
{
public:
	static void CalculateBoundingBox(const Vector3 *pVertices,int numVertices,Vector3 &min,Vector3 &max);
	static void CalculateBoundingSphere(const Vector3 *pVertices,int numVertices,const Vector3 &min,const Vector3 &max,float &radius,Vector3 &center);
	static void AddToBoundingBox(Vector3 &min,Vector3 &max,const Vector3 &add);
	static void Clear(Vector3 &min,Vector3 &max);
	static bool ContainsPointXZ(const Vector3 &min,const Vector3 &max,const Vector3 &point);
	static bool IsEmpty(Vector3 &min,Vector3 &max);
	static bool ContainsPoint(const Vector3 &min,const Vector3 &max,const Vector3 &point);
	static bool ContainsBox(const Vector3 &min1,const Vector3 &max1,const Vector3 &min2,const Vector3 &max2);
	static float VolumeInsideBox(const Vector3 &min1,const Vector3 &max1,const Vector3 &min2,const Vector3 &max2);
	static float GetVolume(const Vector3 &min,const Vector3 &max);
	static bool GetIntersection(const Vector3 &min1,const Vector3 &max1,const Vector3 &min2,const Vector3 &max2,Vector3 &intersectionMin,Vector3 &intersectionMax);

	static void AddObjectBounding(TupTriMesh &tupTriMesh);
	static bool AddNodeBounding(TupNode &tupNode,TupObjectPool &tupObjectPool);

private:
	static void StripOrientation(Graphics4x4 &localToWorld,DirCos3x3 &orientation);
	static void ComputeCentroid(Vector3 *pVectorArray,int numVerts,Vector3 &centroid);
	static void UntransformVertices(Vector3 *pTransformedVerts,int numVerts,DirCos3x3 &orientation,Vector3 &center,Vector3 *pUnTransformedVerts);
};

#endif