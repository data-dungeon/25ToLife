////////////////////////////////////////////////////////////////////////////
//
// Annotate
//
// Class to handle the Annotate Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef ANNOTATE_H
#define ANNOTATE_H

class TupperwareAggregate;

class AnnotateFilter : public FilterPlugin
{
	enum ReadDependencies
	{
		OBJECT_BOUNDING,
		NODE_BOUNDING,
		NODE_OBB,
		NUM_READ_DEPENDENCIES // last entry
	};

	enum WriteDependenciesPass1
	{
		ONE_MATERIAL_PER_MESH,
		NUM_WRITE_DEPENDENCIES_PASS1 // last entry
	};

	enum WriteDependenciesPass2
	{
		OBJECT_MATERIAL_LISTS,
		OBJECT_SOM_STARTS,
		INSTANCE_INDEXES,
		NODE_SOM_INDEXES,
		TEXTURE_CONTEXT_INDEXES,
		PARENT_OBJECT_REFERENCES,
		PARENT_NODE_REFERENCES,
		PROPAGATE_VERTEX_COLORS,
		PATH_INDEXES,
		MODEL_ATTRIBUTES,
		BONES_TO_MESH,
		COLLISION_LAYER,
		ADD_VERTEX_COLOR_REFS_TO_MESH,
		NUM_WRITE_DEPENDENCIES_PASS2 // last entry
	};

	enum WriteDependenciesOptional
	{
		DUMMY_OPTIONAL,
		NUM_OPTIONAL_WRITE_DEPENDENCIES // last entry
	};

	static char *m_readDependencyNames[NUM_READ_DEPENDENCIES];
	static Revision m_readDependencyRevisions[NUM_READ_DEPENDENCIES];
	static char *m_writeDependencyNamesPass1[NUM_WRITE_DEPENDENCIES_PASS1];
	static Revision m_writeDependencyRevisionsPass1[NUM_WRITE_DEPENDENCIES_PASS1];
	static char *m_writeDependencyNamesPass2[NUM_WRITE_DEPENDENCIES_PASS2];
	static Revision m_writeDependencyRevisionsPass2[NUM_WRITE_DEPENDENCIES_PASS2];
	static char *m_writeDependencyNamesOptions[NUM_OPTIONAL_WRITE_DEPENDENCIES];
	static Revision m_writeDependencyRevisionsOptions[NUM_OPTIONAL_WRITE_DEPENDENCIES];
	static char *m_dependencyOptionNames[NUM_OPTIONAL_WRITE_DEPENDENCIES];

	bool m_bForceDynamicTextures;
	bool m_bBuildPartitions;
	bool m_bOptimizeUVs;
	bool m_bClearTwoSidedFaceFlags;
	bool m_bRemovePinkVertexColors;
	TupString m_platform;
	float m_receiveShadowNormalThreshold;
	bool m_bAdjustVertexColorGamma;
	float m_redVertexColorGamma;
	float m_greenVertexColorGamma;
	float m_blueVertexColorGamma;
	bool m_bGlobalMipMapping;
	bool m_bGlobalMipMapCrossFade;
	int m_globalNumMipMapSubMaps;
	float m_globalMipMapDistance;
	bool m_bLoadAuxMapAttributes;
	bool m_bSaveAuxImageAttributes;
	bool m_bSetTransparentAndObscuredStageFacesAsHidden;
	bool m_bForceBonesToAttachPoints;
   bool m_bRotateTallImages;
   bool m_bRemapDuplicateImages;

public:
	AnnotateFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray);

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

	int GetTotalWriteDependencies(int pass,const FilterOptions &options) const;
	const char *GetWriteDependencyKey(int pass,const FilterOptions &options,int index) const;
	Revision GetWriteDependencyRevision(int pass,const FilterOptions &options,int index) const;

	int GetTotalReadDependencies(int pass,const FilterOptions &options) const;
	const char *GetReadDependencyKey(int pass,const FilterOptions &options,int index) const;
	Revision GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const;

private:
	int GetDependencyOption(const FilterOptions &options,int offset) const;
	void CheckOptions(const FilterOptions &options);
};

#endif
