////////////////////////////////////////////////////////////////////////////
//
// ExportPC
//
// Class to handle the ExportPC Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportPC.h $
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 1/21/03    Time: 2:58p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * dependencies
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 12:57p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * removed uv transforms
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * File Dependency checks
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 5/22/02    Time: 3:40p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * add dependencies for bones and limit on tristripping
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Model attributes dependency
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:32p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Surface Type strings now read from node attribute tree
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/03/02    Time: 2:36p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Paths Exporting
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Surface types chunk
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:56p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed to use Annotate filter
 * Added boundign and annotate helper references
 * Removed export pass
 * Added Dependencies
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:18p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Start of Tupperchain filter for the PC 
 * Currently got as far to output the SOM data (should not work yet)
 */

////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_PC_H
#define EXPORT_PC_H

class TupperwareAggregate;

class ExportPCFilter : public FilterPlugin
{
	enum ReadDependencies
	{
		OBJECT_MATERIAL_LISTS,
		OBJECT_SOM_STARTS,
		NODE_SOM_INDEXES,
		TEXTURE_CONTEXT_INDEXES,
		OBJECT_BOUNDING,
		NODE_BOUNDING,
		PARENT_OBJECT_REFERENCES,
		PARENT_NODE_REFERENCES,
		PROPAGATE_VERTEX_COLORS,
		PATH_INDEXES,
		TRI_STRIPS,
		MODEL_ATTRIBUTES,
		BONES_TO_MESH,
		FOUR_BONES_PER_STRIP,
		STRIP_STAGE_FACES,
		NUM_READ_DEPENDENCIES // last entry
	};

	static char *m_readDependencyNames[NUM_READ_DEPENDENCIES];
	static Revision m_readDependencyRevisions[NUM_READ_DEPENDENCIES];

public:
	ExportPCFilter(TupperChainLogBase &log);
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
