////////////////////////////////////////////////////////////////////////////
//
// CompareTupObjects
//
// Class to deal with comparing tup objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CompareTupObjects.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/20/03    Time: 4:11p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * moved map and material comparison to separate class
 * fixed memory leak
 */

#ifndef COMPARE_TUP_OBJECTS
#define COMPARE_TUP_OBJECTS

class CompareTupObjects
{
public:
	static unsigned long GetMaterialCRC(TupperwareAggregate *pMaterialAgg);
	static bool CompareMaterials(TupperwareAggregate *pMaterial1Agg,TupperwareAggregate *pMaterial2Agg);

	static unsigned long GetMapCRC(TupperwareAggregate *pMapAgg);
	static bool CompareMaps(TupperwareAggregate *pMap1Agg,TupperwareAggregate *pMap2Agg);

private:
	static bool CompareFloats(float *pFloat1,float *pFloat2,int total);
};

#endif

