#ifndef _DIBBLE_H_
#define _DIBBLE_H_

//=============================================================================
// DibbleFile
//=============================================================================

class DibbleFile : public CTreeNode
{
public:
	DibbleFile( void );
	virtual ~DibbleFile();

	bool ProcessFile( char *pFilePath, void *pData, int DataSize );
	void Collapse( u32 size );
	bool Parse( void );
	void Destroy( void );

public:
	ts_DBFileHdr m_Header;
	tstring m_FilePath;
	int m_DataSize;
	u8 *m_pTrueData;
	u8 *m_pData;
};

#endif // _DIBBLE_H_