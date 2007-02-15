////////////////////////////////////////////////////////////////////////////
//
// ProcessTup
//
// Class to handle processing the tupperware data
//
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_TUP_H
#define PROCESS_TUP_H

class FilterOptions;
class FilterDataManager;

class TextureSettings
{
public:
	bool m_bBestFitFormat;
	int m_format;
	int m_width;
	int m_height;
};

class TextureEntry
{
public:
	TupperwareAggregate *m_pTextureAgg;
	TupString m_name;
	TupString m_filePathName;
	TextureSettings m_settings;
};

class ProcessTup
{
	enum Platform
	{
		PLATFORM_PC,
		PLATFORM_PS2,
		PLATFORM_GCN,
		PLATFORM_XBOX,
		PLATFORM_DX,
		PLATFORM_UNDEFINED,
	};

	bool m_bLoadSCFiles;
	bool m_bExportXBoxRDFFile;
	bool m_bExportTextureData;
	int m_scaleTextureWidth;
	int m_scaleTextureHeight;

	Platform m_platform;
	TupString m_platformString;

	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupString m_outputFileName;

	// these point into the tree
	TupperwareAggregate *m_pRootAgg;

	TupArray<TextureEntry> m_textureEntries;
	Keeper<TupString> m_textureEntryUniqueNames;

	ProcessTup(void); // this is not defined

public:
	ProcessTup(const FilterOptions &options,FilterDataManager &dataManager);
	~ProcessTup();
	void Process(void);
	void GetFileDependencies(FilePathArray &filePathArray);

	void SetPlatform(const char *pPlatform);
	void SetLoadSCFiles(bool bLoadSCFiles);
	void SetExportXBoxRDFFile(bool bExportXBoxRDFFile);
	void SetExportTextureData(bool bExportTextureData);
	void SetScaleTextureWidth(int scaleTextureWidth);
	void SetScaleTextureHeight(int scaleTextureHeight);

private:
	void GetTextureEntries(void);
	bool GetTextureEntry(TupperwareAggregate *pAgg,TextureEntry &entry,const TextureSettings &globalSettings);
	void UpdateSettings(TupperwareAtom *pAtom,TextureSettings &settings);
	bool UpdateFormat(const char *pFormat,bool &bBestFitFormat,int &format);

	// support for Dan Silver's front end tool
	void AddSCFileTextureEntries(bool bAddTexturesToRootAgg);
	bool GetSCFileTextureEntries(TupperwareAggregate *pAgg,TupArray<TextureEntry> &entries,const TextureSettings &globalSettings,bool bAddTexturesToRootAgg);

	bool ValidPlatform(const char *pPlatform);
	const char *GetFormatString(int formatFlags);
	void AddTextureData(void);
	void ChangeTextureFormats(void);
	void ChangeTextureSize(void);
	void RotateTallImages(void);
	void AddTextureInformation(void);
	void RemapDuplicateTextures(void);
	void AddTextureIDs(void);
	void MergeTexturePalettes(void);
	void MergeAnimatedImagePalettes(const char *pImageFileName,TupImageData &tupImageData,int &numPalettes,int &numMergedPalettes);
	void AnalyzePaletteColors(void);
	void AddSortedTexturesToLog(void);

	void ExportTextures(void);
	void WriteXBoxRDFFile(void);

	// in WriteTextures.cpp
	u32 WriteTextures(void);
	u32 WriteAnimatedTextures(void);

	// in WriteTexturePS2.cpp
	void WritePS2Texture(TextureEntry &textureEntry,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation);
	void SetPS2TextureRecord(TextureEntry &textureEntry,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation);

	// in WriteTextureGCN.cpp
	void WriteGCNTexture(TextureEntry &textureEntry,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation);
	void SetGCNTextureRecord(TextureEntry &textureEntry,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation);

	// in WriteTextureNames.cpp
	u32 WriteTextureNames(void);


};

#endif
