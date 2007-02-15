////////////////////////////////////////////////////////////////////////////
//
// PS2TextureInfo
//
// Handle any texture information for the PS2
//
////////////////////////////////////////////////////////////////////////////

#ifndef PS2TEXTURE_INFO_H
#define PS2TEXTURE_INFO_H

#ifndef PROCESS_TUP_H
#include "ProcessTup.h"
#endif

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
	TupArray<TextureEntry> &m_textureEntries;
	TupperwareAggregate *m_pRootAgg;

	TupArray<TupArray<AnalyzedTextureData> > m_textureDataArray;
	int m_totalTextures;
	u32 m_VRAMBlockSize;

	PS2TextureInfo(void); // this is not defined

public:

	PS2TextureInfo(TupArray<TextureEntry> &m_textureEntries,TupperwareAggregate *pRootAgg);
	~PS2TextureInfo();

	void AnalyzeTextures(void);
	int GetNumTextures(void) const;
	int GetNumMaps(int textureIndex) const;
	const AnalyzedTextureData &GetAnalyzedTextureData(int textureID,int mapIndex) const;
	u32 GetTextureSetVRAMBlockSize(void) const;
private:
	static u32 GetPS2Format(int formatFlags);
	void AllocateVRAMSpaceForTexture(TextureEntry &textureEntry,int textureIndex);
	void AllocateVRAMSpaceForPalette(TextureEntry &textureEntry,int textureIndex);
};


#endif