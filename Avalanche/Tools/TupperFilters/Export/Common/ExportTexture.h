////////////////////////////////////////////////////////////////////////////
//
// ExportTexture
//
// Export the textures
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_TEXTURE_H
#define EXPORT_TEXTURE_H

class FilterOptions;
class FilterDataManager;
class TupScene;
class TupImagePool;
struct ts_TextureChunkRecord;

class ExportTexture
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupScene &m_tupScene;
	TupObjectPool &m_tupObjectPool;
	TupImagePool &m_tupImagePool;

	int m_currentPartition;
	bool m_bExportTextureData;

	ExportTexture(void); // this is not defined

public:
	ExportTexture(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,
		TupScene &tupScene,TupObjectPool &tupObjectPool,TupImagePool &tupImagePool,
		int currentPartition,bool bExportTextureData);

	u32 WriteTextures(void);
	u32 WriteAnimatedTextures(void);
	void WriteXBoxRDFFile(void);
	u32 WriteTextureNames(void);

private:
	void WriteTexture(TupperwareAggregate *pImageAgg,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation);
	void SetTextureRecord(TupperwareAggregate *pImageAgg,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation);
#ifdef EXPORT_GCN
	void WriteGCNTexture(TupperwareAggregate *pImageAgg,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation);
	void SetGCNTextureRecord(TupperwareAggregate *pImageAgg,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation);
#endif // EXPORT_GCN
};


#endif