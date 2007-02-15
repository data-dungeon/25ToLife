////////////////////////////////////////////////////////////////////////////
//
// AnnotateFilter
//
// Class to handle the Annotate Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "AnnotatePch.h"
#include "Annotate.h"
#include "ProcessTup.h"

// so I don't need to link in the collide library
#include <Collide/obbStatics.h> 

AnnotateFilter::AnnotateFilter(TupperChainLogBase &log) :
		FilterPlugin(log),m_bForceDynamicTextures(false),m_bBuildPartitions(false),m_bOptimizeUVs(false),m_bClearTwoSidedFaceFlags(false),
		m_bRemovePinkVertexColors(false),
		m_platform("undefined"),m_receiveShadowNormalThreshold(-1.0f),
		m_bAdjustVertexColorGamma(false),m_redVertexColorGamma(1.0f),m_greenVertexColorGamma(1.0f),m_blueVertexColorGamma(1.0f),
		m_bGlobalMipMapping(false),m_bGlobalMipMapCrossFade(false),m_globalNumMipMapSubMaps(0),m_globalMipMapDistance(100.0f),
		m_bLoadAuxMapAttributes(false),m_bSaveAuxImageAttributes(false),m_bSetTransparentAndObscuredStageFacesAsHidden(true),
      m_bForceBonesToAttachPoints(false),m_bRotateTallImages(true),m_bRemapDuplicateImages(true)
{
}

int AnnotateFilter::GetTotalPasses(void) const
{
	return 2;
}

void AnnotateFilter::GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	ProcessTup processTup(options,dataManager,false);
	try
	{
		processTup.GetFileDependencies(filePathArray);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"Annotate Filter - GetFileDependencies\r\n  %s",(const char *)(e)));
	}
}


void AnnotateFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	ProcessTup processTup(options,dataManager,true);
	try
	{
		processTup.SetForceDynamicTextures(m_bForceDynamicTextures);
		processTup.SetBuildPartitions(m_bBuildPartitions);
		processTup.SetOptimizeUVs(m_bOptimizeUVs);
		processTup.SetPlatform(m_platform);
		processTup.SetReceiveShadowNormalThreshold(m_receiveShadowNormalThreshold);
		processTup.SetAdjustVertexColorGamma(m_bAdjustVertexColorGamma);
		processTup.SetRedVertexColorGamma(m_redVertexColorGamma);
		processTup.SetGreenVertexColorGamma(m_greenVertexColorGamma);
		processTup.SetBlueVertexColorGamma(m_blueVertexColorGamma);
		processTup.SetClearTwoSidedFaceFlags(m_bClearTwoSidedFaceFlags);
		processTup.SetRemovePickVertexColors(m_bRemovePinkVertexColors);
		processTup.SetGlobalMipMapping(m_bGlobalMipMapping);
		processTup.SetGlobalMipMapCrossFade(m_bGlobalMipMapCrossFade);
		processTup.SetGlobalNumMipMapSubMaps(m_globalNumMipMapSubMaps);
		processTup.SetGlobalMipMapDistance(m_globalMipMapDistance);
		processTup.SetLoadAuxMapAttributes(m_bLoadAuxMapAttributes);
		processTup.SetSaveAuxImageAttributes(m_bSaveAuxImageAttributes);
		processTup.SetTransparentAndObscuredStageFacesAsHiddenValue(m_bSetTransparentAndObscuredStageFacesAsHidden);
		processTup.SetForceBonesToAttachPoints(m_bForceBonesToAttachPoints);
      processTup.SetRotateTallImages(m_bRotateTallImages);
      processTup.SetRemapDuplicateImages(m_bRemapDuplicateImages);

		processTup.Process(pass);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"Annotate Filter - Process Pass %d\r\n  %s",pass,(const char *)(e)));
	}
}

char *AnnotateFilter::m_readDependencyNames[NUM_READ_DEPENDENCIES] = 
{
	"Bounding_ObjectBounding", // OBJECT_BOUNDING
	"Bounding_NodeBounding", // NODE_BOUNDING
	"Bounding_NodeOBB", // NODE_OBB
};

char *AnnotateFilter::m_writeDependencyNamesPass1[NUM_WRITE_DEPENDENCIES_PASS1] = 
{
		"Annotate_OneMaterialPerMesh", // ONE_MATERIAL_PER_MESH
};

char *AnnotateFilter::m_writeDependencyNamesPass2[NUM_WRITE_DEPENDENCIES_PASS2] = 
{
		"Annotate_ObjectMaterialLists", // OBJECT_MATERIAL_LISTS
		"Annotate_ObjectSOMStarts", // OBJECT_SOM_STARTS
		"Annotate_InstanceIndexes", // INSTANCE_INDEXES
		"Annotate_NodeSomIndexes", // NODE_SOM_INDEXES
		"Annotate_TextureContextIndexes", // TEXTURE_CONTEXT_INDEXES
		"Annotate_ParentObjectReferences", // PARENT_OBJECT_REFERENCES
		"Annotate_ParentNodeReferences", // PARENT_NODE_REFERENCES
		"Annotate_PropagateVertexColors", // PROPAGATE_VERTEX_COLORS
		"Annotate_PathIndexes", // PATH_INDEXES
		"Annotate_ModelAttributes", // MODEL_ATTRIBUTES
		"Annotate_BonesToMesh", // BONES_TO_MESH
		"Annotate_CollisionLayer", // COLLISION_LAYER
		"Annotate_AddVertexColorRefsToMesh", // ADD_VERTEX_COLOR_REFS_TO_MESH
};

char *AnnotateFilter::m_writeDependencyNamesOptions[NUM_OPTIONAL_WRITE_DEPENDENCIES] = 
{
		"Annotate_DummyOptional", // DUMMY_OPTIONAL
};

char *AnnotateFilter::m_dependencyOptionNames[NUM_OPTIONAL_WRITE_DEPENDENCIES] = 
{
		"DummyOptional", // DUMMY_OPTIONAL
};

Revision AnnotateFilter::m_readDependencyRevisions[NUM_READ_DEPENDENCIES] = 
{
	Revision(1,0), // OBJECT_BOUNDING
	Revision(1,0), // NODE_BOUNDING
	Revision(1,0), // NODE_OBB
};

Revision AnnotateFilter::m_writeDependencyRevisionsPass1[NUM_WRITE_DEPENDENCIES_PASS1] = 
{
	Revision(1,0), // ONE_MATERIAL_PER_MESH
};

Revision AnnotateFilter::m_writeDependencyRevisionsPass2[NUM_WRITE_DEPENDENCIES_PASS2] = 
{
	Revision(1,0), // OBJECT_MATERIAL_LISTS
	Revision(1,0), // OBJECT_SOM_STARTS
	Revision(1,0), // INSTANCE_INDEXES
	Revision(1,0), // NODE_SOM_INDEXES
	Revision(1,0), // TEXTURE_CONTEXT_INDEXES
	Revision(1,0), // PARENT_OBJECT_REFERENCES
	Revision(1,0), // PARENT_NODE_REFERENCES
	Revision(1,0), // PROPAGATE_VERTEX_COLORS
	Revision(1,0), // PATH_INDEXES
	Revision(1,0), // MODEL_ATTRIBUTES
	Revision(1,0), // BONES_TO_MESH
	Revision(1,0), // COLLISION_LAYER
	Revision(1,0), // ADD_VERTEX_COLOR_REFS_TO_MESH
};

Revision AnnotateFilter::m_writeDependencyRevisionsOptions[NUM_OPTIONAL_WRITE_DEPENDENCIES] = 
{
	Revision(1,0), // DUMMY_OPTIONAL
};


int AnnotateFilter::GetTotalReadDependencies(int pass,const FilterOptions &options) const
{
	if (pass == 2)
		return NUM_READ_DEPENDENCIES;
	else return 0;
}

const char *AnnotateFilter::GetReadDependencyKey(int pass,const FilterOptions &options,int index) const
{
	assert(pass==2);
	return m_readDependencyNames[index];
}

Revision AnnotateFilter::GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	assert(pass==2);
	return m_readDependencyRevisions[index];
}

int AnnotateFilter::GetTotalWriteDependencies(int pass,const FilterOptions &options) const
{
	switch (pass)
	{
		case 1:
			return NUM_WRITE_DEPENDENCIES_PASS1;
		case 2:
			{
				// check options here without changing the class
				int totalOptional = 0;
				for (int index = 0;index<NUM_OPTIONAL_WRITE_DEPENDENCIES;index++)
				{
					const char *pCommand = options.GetValue(m_dependencyOptionNames[index]);
					if (pCommand && !stricmp(pCommand,"true"))
					{
						totalOptional++;
					}
				}
				return totalOptional+NUM_WRITE_DEPENDENCIES_PASS2;
			}
		default:
			assert(false);
			return 0;
	}
}

const char *AnnotateFilter::GetWriteDependencyKey(int pass,const FilterOptions &options,int index) const
{
	int numDependencies = GetTotalWriteDependencies(pass,options);
	assert(index>=0 && index<numDependencies);

	switch (pass)
	{
		case 1:
			return m_writeDependencyNamesPass1[index];
		case 2:
			{
				// static ones (do not depend on options)
				if (index<NUM_WRITE_DEPENDENCIES_PASS2)
				{
					return m_writeDependencyNamesPass2[index];
				}
				// ones that depend on options
				else
				{
					int offset = index-NUM_WRITE_DEPENDENCIES_PASS2;
					int index = GetDependencyOption(options,offset);
					return m_writeDependencyNamesOptions[index];
				}
			}
		default:
			assert(false);
			return NULL;
	}
}

Revision AnnotateFilter::GetWriteDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	int numDependencies = GetTotalWriteDependencies(pass,options);
	assert(index>=0 && index<numDependencies);
	switch (pass)
	{
		case 1:
			return m_writeDependencyRevisionsPass1[index];
		case 2:
			{
				// static ones (do not depend on options)
				if (index<NUM_WRITE_DEPENDENCIES_PASS2)
				{
					return m_writeDependencyRevisionsPass2[index];
				}
				// ones that depend on options
				else
				{
					int offset = index-NUM_WRITE_DEPENDENCIES_PASS2;
					int index = GetDependencyOption(options,offset);
					return m_writeDependencyRevisionsOptions[index];
				}
			}
		default:
			assert(false);
			return Revision(0,0);
	}
}

int AnnotateFilter::GetDependencyOption(const FilterOptions &options,int offset) const
{
	for (int index = 0;index<NUM_OPTIONAL_WRITE_DEPENDENCIES;index++)
	{
		const char *pCommand = options.GetValue(m_dependencyOptionNames[index]);
		if (pCommand && !stricmp(pCommand,"true"))
		{
			if (offset==0)
				return index; // if this is the one we need then return the index
			offset--;
		}
	}
	assert(false);
	return 0;
}

void AnnotateFilter::CheckOptions(const FilterOptions &options)
{
	const char *pValue;
	pValue = options.GetValue("ForceDynamicTextures");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bForceDynamicTextures = true;
	}
	else
	{
		m_bForceDynamicTextures = false;
	}

	pValue = options.GetValue("BuildPartitions");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bBuildPartitions = true;
	}
	else
	{
		m_bBuildPartitions = false;
	}

	pValue = options.GetValue("OptimizeUVs");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bOptimizeUVs = true;
	}
	else
	{
		m_bOptimizeUVs = false;
	}

	pValue = options.GetValue("ClearTwoSidedFaceFlags");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bClearTwoSidedFaceFlags = true;
	}

	pValue = options.GetValue("RemovePickVertexColors");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bRemovePinkVertexColors = true;
	}

	pValue = options.GetValue("Platform");
	if (pValue)
	{
		m_platform = pValue;
	}

	pValue = options.GetValue("ReceiveShadowNormalThreshold");
	if (pValue)
	{
		float value = (float)atof(pValue);
		if (value<-1.0f)
			value = -1.0f;
		if (value>1.0f)
			value = 1.0f;
		m_receiveShadowNormalThreshold = value;
	}

	pValue = options.GetValue("AdjustVertexColorGamma");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bAdjustVertexColorGamma = true;

		pValue = options.GetValue("VertexColorGammaValue");
		if (pValue)
		{
			TupArray<TupString> stringArray;
			TokenizeString::BuildArray(pValue,",",stringArray);
			if (stringArray.GetSize()==3)
			{
				m_redVertexColorGamma = (float)atof(stringArray[0]);
				if (m_redVertexColorGamma<0.1f) m_redVertexColorGamma = 0.1f;
				if (m_redVertexColorGamma>10.0f) m_redVertexColorGamma = 10.0f;

				m_greenVertexColorGamma = (float)atof(stringArray[1]);
				if (m_greenVertexColorGamma<0.1f) m_greenVertexColorGamma = 0.1f;
				if (m_greenVertexColorGamma>10.0f) m_greenVertexColorGamma = 10.0f;

				m_blueVertexColorGamma = (float)atof(stringArray[2]);
				if (m_blueVertexColorGamma<0.1f) m_blueVertexColorGamma = 0.1f;
				if (m_blueVertexColorGamma>10.0f) m_blueVertexColorGamma = 10.0f;

				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Adjust Vertex Color Gamma Enabled with values of %1.4f,%1.4f,%1.4f",
					m_redVertexColorGamma,m_greenVertexColorGamma,m_blueVertexColorGamma);

			}
			else 
			{
				if (stringArray.GetSize()>0)
				{
					m_redVertexColorGamma = (float)atof(stringArray[0]);
					if (m_redVertexColorGamma<0.1f) m_redVertexColorGamma = 0.1f;
					if (m_redVertexColorGamma>10.0f) m_redVertexColorGamma = 10.0f;
					m_greenVertexColorGamma = m_redVertexColorGamma;
					m_blueVertexColorGamma = m_redVertexColorGamma;
				}
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Adjust Vertex Color Gamma Enabled with value of %1.4f",
					m_redVertexColorGamma);
			}
		}
	}
	else
	{
		m_bAdjustVertexColorGamma = false;
	}

	pValue = options.GetValue("GlobalMipMapping");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bGlobalMipMapping = true;

		pValue = options.GetValue("GlobalNumMipMapSubMaps");
		if (pValue)
		{
			m_globalNumMipMapSubMaps = atoi(pValue);
		}

		pValue = options.GetValue("GlobalMipMapDistance");
		if (pValue)
		{
			m_globalMipMapDistance = (float)atof(pValue);
		}

		pValue = options.GetValue("GlobalMipMapCrossFade");
		if (pValue && !stricmp(pValue,"true"))
		{
			m_bGlobalMipMapCrossFade = true;
		}
		else
		{
			m_bGlobalMipMapCrossFade = false;
		}
	}
	else
	{
		m_bGlobalMipMapping = false;
	}

	pValue = options.GetValue("LoadAuxMapAttributes");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bLoadAuxMapAttributes = true;
	}
	else
	{
		m_bLoadAuxMapAttributes = false;
	}

	pValue = options.GetValue("SaveAuxImageAttributes");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bSaveAuxImageAttributes = true;
	}
	else
	{
		m_bSaveAuxImageAttributes = false;
	}

	pValue = options.GetValue("SetTransparentAndObscuredStageFacesAsHidden");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bSetTransparentAndObscuredStageFacesAsHidden = true;
	}
	else if (pValue && !stricmp(pValue,"false"))
	{
		m_bSetTransparentAndObscuredStageFacesAsHidden = false;
	}

	pValue = options.GetValue("ForceBonesToAttachPoints");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bForceBonesToAttachPoints = true;
	}
	else if (pValue && !stricmp(pValue,"false"))
	{
		m_bForceBonesToAttachPoints = false;
	}

   pValue = options.GetValue("RotateTallImages");
   if (pValue && !stricmp(pValue,"true"))
   {
      m_bRotateTallImages = true;
   }
   else if (pValue && !stricmp(pValue,"false"))
   {
      m_bRotateTallImages = false;
   }

   pValue = options.GetValue("RemapDuplicateImages");
   if (pValue && !stricmp(pValue,"true"))
   {
      m_bRemapDuplicateImages = true;
   }
   else if (pValue && !stricmp(pValue,"false"))
   {
      m_bRemapDuplicateImages = false;
   }
}
