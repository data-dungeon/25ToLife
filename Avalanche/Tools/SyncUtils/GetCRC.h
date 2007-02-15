////////////////////////////////////////////////////////////////////////////
//
// GetCRC
//
// Class obtaining a CRC value from a file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: GetCRC.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * CRC checks
 */

#ifndef SYNC_UTILS_DLL_GET_CRC_H
#define SYNC_UTILS_DLL_GET_CRC_H

//-----------------------------------------------------------------------------
// SyncDBFactory class declaration:

class AFX_EXT_CLASS GetCRC
{
	DWORD *m_pCRC32Table;

public:

	GetCRC();
	~GetCRC();
	
	void Init(void);
	void Free(void);
	
	bool GetCRCFromFile(const char *pFilename, DWORD &crc32) const;
};

//-----------------------------------------------------------------------------

#endif

