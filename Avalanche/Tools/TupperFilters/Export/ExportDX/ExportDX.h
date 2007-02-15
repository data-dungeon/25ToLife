////////////////////////////////////////////////////////////////////////////
//
// ExportDX
//
// Class to handle the ExportDX Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportDX.h $
 * 
 */

////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_DX_H
#define EXPORT_DX_H

class TupperwareAggregate;

class ExportDXFilter : public FilterPlugin
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
	ExportDXFilter(TupperChainLogBase &log);
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
