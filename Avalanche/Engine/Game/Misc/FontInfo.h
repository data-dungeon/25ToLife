#ifndef FONTINFO_H
#define FONTINFO_H

#include "platform/BaseType.h"

// ---------------------------------------------------------------------------
//	FILE HEADER(S)

struct FontInfo
{
	u32 version;
	u32 entries;
	u32 widthTable[256];
};

// ---------------------------------------------------------------------------
//	CLASS: FontInfoFile

class FontInfoFile
{
	public:
		bool	Load(char *filename);
	
	public:
		inline int	GetCharCount() const;
		inline int	GetCharWidth(char c) const;

	protected:
		FontInfo	d_fontInfo;
};

// ===========================================================================
//	IMPLEMENTATION: FontInfoFile

// ---------------------------------------------------------------------------
//	ACCESSOR: GetCharCount

inline int FontInfoFile::GetCharCount() const
{
	return d_fontInfo.entries;
}

// ---------------------------------------------------------------------------
//	ACCESSOR: GetCharWidth

inline int FontInfoFile::GetCharWidth(char c) const
{
	return d_fontInfo.widthTable[c];
}

#endif
