#pragma once

#include "XPRFileTypes.h"

class XPRFileData
{
public:
	bool m_bFileRead;

	XPRFileHeader m_fileHeader;

	u8 *m_pHeaderBuffer;
	u32 m_headerBufferSize;

	u8 *m_pVideoBuffer;
	u32 m_videoBufferSize;

	XPRFileData();
	~XPRFileData();

	void FreeBuffers(void);

	void ReadFile(const char *pFilename);
};
