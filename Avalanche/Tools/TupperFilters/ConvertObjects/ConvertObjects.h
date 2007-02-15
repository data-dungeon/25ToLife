////////////////////////////////////////////////////////////////////////////
//
// ConvertObjects
//
// Class to handle the Make YUp Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ConvertObjects.h $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 7/29/03    Time: 2:58p
 * Created in $/Avalanche/tools/TupperFilters/ConvertObjects
 */

////////////////////////////////////////////////////////////////////////////

#ifndef CONVERT_OBJECTS_H
#define CONVERT_OBJECTS_H

class TupperwareAggregate;
class TupperChainLogBase;
class TupNodePool;

class ConvertObjectsFilter : public FilterPlugin
{
public:
	ConvertObjectsFilter(TupperChainLogBase &log) : FilterPlugin(log) {}

	int GetTotalPasses(void) const;
	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);
	void ValidateMeshes(TupObjectPool &tupObjectPool);
	bool ValidateMesh(TupPolyMesh &tupPolyMesh);
	bool ValidateMesh(TupTriMesh &tupTriMesh);

};

#endif
