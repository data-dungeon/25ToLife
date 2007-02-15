////////////////////////////////////////////////////////////////////////////
//
// ExportDXFilter
//
// Class to handle the ExportDX Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportDX.cpp $
 * 
 */

////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportDX.h"
#include "ExportDBL.h"

ExportDXFilter::ExportDXFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int ExportDXFilter::GetTotalPasses(void) const
{
	return 1;
}

void ExportDXFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
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
		throw(PluginException(ERROR_FATAL,"ExportDX Filter - Export\r\n  %s",(const char *)(e)));
	}
}

void ExportDXFilter::GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray)
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

void ExportDXFilter::CheckOptions(const FilterOptions &options)
{
}

char *ExportDXFilter::m_readDependencyNames[NUM_READ_DEPENDENCIES] = 
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
		"TriStrip_BonesPerStrip = 16", // FOUR_BONES_PER_STRIP
		"TriStrip_StripStageFaces = true", // STRIP_STAGE_FACES
};

Revision ExportDXFilter::m_readDependencyRevisions[NUM_READ_DEPENDENCIES] = 
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

int ExportDXFilter::GetTotalReadDependencies(int pass,const FilterOptions &options) const
{
	return NUM_READ_DEPENDENCIES;
}

const char *ExportDXFilter::GetReadDependencyKey(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyNames[index];
}

Revision ExportDXFilter::GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyRevisions[index];
}

