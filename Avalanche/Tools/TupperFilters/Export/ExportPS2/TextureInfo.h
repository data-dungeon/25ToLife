////////////////////////////////////////////////////////////////////////////
//
// TextureInfo
//
// Handle any texture information for the PS2
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TextureInfo.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 10:05a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * mip map support
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/10/02    Time: 11:38a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * palette merging
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/06/02    Time: 3:38p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Using textureIDs rather than image indexes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/08/02    Time: 1:46p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Texture sets and vram locations update
 */

#ifndef TEXTURE_INFO_H
#define TEXTURE_INFO_H

class PS2TextureInfo
{
public:

	class AnalyzedTextureData
	{
	public:
		u32 m_textureBlock;
		u32 m_paletteBlock;
	};

private:
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupScene &m_tupScene;
	TupObjectPool &m_tupObjectPool;
	TupImagePool &m_tupImagePool;
	int m_currentPartition;

	TupArray<TupArray<AnalyzedTextureData> > m_textureDataArray;
	int m_totalTextures;
	TupArray<u32> m_VRAMBlockSizes;
	int m_numTextureSets;


	PS2TextureInfo(void); // this is not defined

public:

	PS2TextureInfo(const FilterOptions &options,FilterDataManager &dataManager,
		TupScene &tupScene,TupObjectPool &tupObjectPool,TupImagePool &tupImagePool,int currentPartition);
	~PS2TextureInfo();

	void AnalyzeTextures(void);
	int GetNumTextures(void) const;
	int GetNumMaps(int textureID) const;
	const AnalyzedTextureData &GetAnalyzedTextureData(int textureID,int mapIndex) const;
	int GetNumTextureSets(void) const;
	u32 GetTextureSetVRAMBlockSize(int setNum) const;
private:
	static u32 GetPS2Format(int formatFlags);
	void AllocateVRAMSpaceForTexture(TupImage &tupImage,int textureID);
	void AllocateVRAMSpaceForPalette(TupImage &tupImage,int textureID);
};


#endif