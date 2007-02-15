//-----------------------------------------------------------------------------
// Handles the DXD side of things for a DBL.
// Takes the XPR and XVD chunks and sets up the memory for DX etc.
//-----------------------------------------------------------------------------
#ifndef __DXDMANAGER_H
#define __DXDMANAGER_H

#include <xgraphics.h>
#include "xbresource.h"

enum {
	TYPE_XPR,
	TYPE_XVD,
};

class CDXD_Data
{
public:
	// Constructor/destructor
	CDXD_Data();
	~CDXD_Data();

	// Init
	void	Init(void);
	bool	InitSysmem(int Type, int SysmemSize, char* Sysmem );
	bool	LoadDXDData(const char* Filename, int Filesize);
	void	SetHeader( int Type, XPR_HEADER*	Hdr );
	bool	CreateResource(int Type, int Offset);
	void	SetDBLName( char* Fname );
	char*	GetDBLName(void);

	// vars
	bool						m_XPRresourceCreated;
	bool						m_XVDresourceCreated;
	char						m_Fname[48];

	// structs / classes
	CXBPackedResource		m_xprResources;
	CXBPackedResource		m_xvdResources;
	XPR_HEADER				m_xprHeader;
	XPR_HEADER				m_xvdHeader;
	BYTE*						m_pXprMem; 
	BYTE*						m_pXvdMem; 
};

#endif	// __DXDMANAGER_H
