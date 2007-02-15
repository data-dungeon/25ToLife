////////////////////////////////////////////////////////////////////////////
//
// OBBWorld
//
// Class to handle the OBBWorld Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: OBBWorld.h $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 1/14/03    Time: 11:37a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * added collisionlayer dependency
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * File Dependency checks
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Added support for attrib tree
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/27/02    Time: 1:13p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Dependencies
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Surface Types file loading
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 1:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * OBBWorld is now online... Just needs to get the node and face
 * attributes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/14/02    Time: 9:49a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_TOOLA_H
#define TERRAIN_TOOLA_H

class TupperwareAggregate;
class OBBQuadTreeFactory;

class OBBWorldFilter : public FilterPlugin
{
	enum ReadDependencies
	{
		INSTANCE_INDEXES,
		OBJECT_BOUNDING,					
		NODE_BOUNDING,
		PARENT_OBJECT_REFERENCES,
		PARENT_NODE_REFERENCES,
		PROPAGATE_VERTEX_COLORS,
		COLLISION_LAYER,
		NUM_READ_DEPENDENCIES // last entry
	};

	static char *m_readDependencyNames[NUM_READ_DEPENDENCIES];
	static Revision m_readDependencyRevisions[NUM_READ_DEPENDENCIES];

	bool m_saveVisFile;
	float m_sectorSize;
	float m_minQuadrantSize;
	Vector3	m_shrinkBy;
	float m_minDimension;
	Stats m_stats;

public:
	OBBWorldFilter(TupperChainLogBase &log);
	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

	void GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray);

	int GetTotalReadDependencies(int pass,const FilterOptions &options) const;
	const char *GetReadDependencyKey(int pass,const FilterOptions &options,int index) const;
	Revision GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const;

private:
	void CheckOptions(const FilterOptions &options);
};

#endif
