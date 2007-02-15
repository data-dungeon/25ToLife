#pragma once

class DBLFileData
{
public:
	ts_DBFileHdr m_fileHeader;
	int m_padSize;

	u8 *m_pDataBuffer;
	u32 m_dataBufferSize;

	DBLFileData();
	~DBLFileData();

	void FreeBuffers(void);

	void ReadFile(const char *pFilename);
};
