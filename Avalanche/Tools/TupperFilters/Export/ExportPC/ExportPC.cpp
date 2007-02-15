////////////////////////////////////////////////////////////////////////////
//
// ExportPCFilter
//
// Class to handle the ExportPC Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportPC.cpp $
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:19p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * dependency on the bounding filter
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 1/21/03    Time: 2:58p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * dependencies
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 1:24p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * oops on revision
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 12:57p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * removed uv transforms
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * File Dependency checks
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 5/22/02    Time: 3:40p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * add dependencies for bones and limit on tristripping
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Model attributes dependency
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:32p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Surface Type strings now read from node attribute tree
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 4/03/02    Time: 2:36p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Paths Exporting
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * hybrid configuration
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 3/22/02    Time: 2:39p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Surface types chunk
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:56p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed to use Annotate filter
 * Added boundign and annotate helper references
 * Removed export pass
 * Added Dependencies
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed Exception Problem
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 4:25p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Added Collage Image Reader
 * Uses Paintlib (16 extensions)
 * Can read file and add width and height to tup
 * More to come...
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

#include "ExportPch.h"
#include "ExportPC.h"
#include "ExportDBL.h"

ExportPCFilter::ExportPCFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int ExportPCFilter::GetTotalPasses(void) const
{
	return 1;
}

void ExportPCFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	TupString outputFilename(dataManager.GetOutputFilename());
	FilePath::ChangeExtension(outputFilename,".DBL");
	ExportDBL exportDBL(outputFilename,options,dataManager);
	try
	{
		exportDBL.DoExport();
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"ExportPC Filter - Export\r\n  %s",(const char *)(e)));
	}
}

void ExportPCFilter::GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	TupString outputFilename(dataManager.GetOutputFilename());
	FilePath::ChangeExtension(outputFilename,".DBL");
	filePathArray.AddFilePath(outputFilename,FilePathArray::FILE_OUTPUT|FilePathArray::FILE_ACCESS_FATAL);
	char sceneVarFile[_MAX_PATH];
	FilePath::GetPath(outputFilename, sceneVarFile);
	strcat(sceneVarFile, "scene.var");
	filePathArray.AddFilePath(sceneVarFile,FilePathArray::FILE_OUTPUT|FilePathArray::FILE_ACCESS_FATAL);
}

void ExportPCFilter::CheckOptions(const FilterOptions &options)
{
}

char *ExportPCFilter::m_readDependencyNames[NUM_READ_DEPENDENCIES] = 
{
		"Annotate_ObjectMaterialLists", // OBJECT_MATERIAL_LISTS
		"Annotate_ObjectSOMStarts", // OBJECT_SOM_STARTS
		"Annotate_NodeSomIndexes", // NODE_SOM_INDEXES
		"Annotate_TextureContextIndexes", // TEXTURE_CONTEXT_INDEXES
		"Bounding_ObjectBounding", // OBJECT_BOUNDING
		"Bounding_NodeBounding", // NODE_BOUNDING
		"Annotate_ParentObjectReferences", // PARENT_OBJECT_REFERENCES
		"Annotate_ParentNodeReferences", // PARENT_NODE_REFERENCES
		"Annotate_PropagateVertexColors", // PROPAGATE_VERTEX_COLORS
		"Annotate_PathIndexes", // PATH_INDEXES
		"TriStrip_TriStripData", // TRI_STRIPS
		"Annotate_ModelAttributes", // MODEL_ATTRIBUTES
		"Annotate_BonesToMesh", // BONES_TO_MESH
		"TriStrip_BonesPerStrip = 4", // FOUR_BONES_PER_STRIP
		"TriStrip_StripStageFaces = true", // STRIP_STAGE_FACES
};

Revision ExportPCFilter::m_readDependencyRevisions[NUM_READ_DEPENDENCIES] = 
{
	Revision(1,0), // OBJECT_MATERIAL_LISTS
	Revision(1,0), // OBJECT_SOM_STARTS
	Revision(1,0), // NODE_SOM_INDEXES
	Revision(1,0), // TEXTURE_CONTEXT_INDEXES
	Revision(1,0), // OBJECT_BOUNDING
	Revision(1,0), // NODE_BOUNDING
	Revision(1,0), // PARENT_OBJECT_REFERENCES
	Revision(1,0), // PARENT_NODE_REFERENCES
	Revision(1,0), // PROPAGATE_VERTEX_COLORS
	Revision(1,0), // PATH_INDEXES
	Revision(1,0), // TRI_STRIPS
	Revision(1,0), // MODEL_ATTRIBUTES
	Revision(1,0), // BONES_TO_MESH
	Revision(1,0), // FOUR_BONES_PER_STRIP
	Revision(1,0), // STRIP_STAGE_FACES
};

int ExportPCFilter::GetTotalReadDependencies(int pass,const FilterOptions &options) const
{
	return NUM_READ_DEPENDENCIES;
}

const char *ExportPCFilter::GetReadDependencyKey(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyNames[index];
}

Revision ExportPCFilter::GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyRevisions[index];
}

