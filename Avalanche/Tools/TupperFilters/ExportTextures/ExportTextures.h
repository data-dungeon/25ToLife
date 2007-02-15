////////////////////////////////////////////////////////////////////////////
//
// ExportTextures
//
// Class to handle the ExportTextures Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_TEXTURES_H
#define EXPORT_TEXTURES_H

class TupperwareAggregate;
class TupperChainLogBase;

class ExportTexturesFilter : public FilterPlugin
{
	TupString m_platform;
	bool m_bLoadSCFiles;
	bool m_bExportXBoxRDFFile;
	bool m_bExportTextureData;
	int m_scaleTextureWidth;
	int m_scaleTextureHeight;

public:
	ExportTexturesFilter(TupperChainLogBase &log) : FilterPlugin(log) {}

	int GetTotalPasses(void) const;
	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

	void GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray);

private:
	void CheckOptions(const FilterOptions &options);
};

#endif
