//-----------------------------------------------------------------------------
// Handles the DXD side of things for a DBL.
// Takes the XPR and XVD chunks and sets up the memory for DX etc.
//-----------------------------------------------------------------------------


#include "platform/PlatformPCH.h"
#include <xgraphics.h>
#include "xbresource.h"
#include "platform/Xbox/dxdmanager.h"

#include "Layers/DiscErrorHandler/DiscError.h"

//-----------------------------------------------------------------------------
CDXD_Data::CDXD_Data()
{
	m_XPRresourceCreated = false;
	m_XVDresourceCreated = false;
	memset(&m_xprHeader, 0, sizeof (XPR_HEADER));
	memset(&m_xvdHeader, 0, sizeof (XPR_HEADER));
}

//-----------------------------------------------------------------------------
CDXD_Data::~CDXD_Data()
{
	// close textures
	if (m_XPRresourceCreated)
		m_xprResources.Destroy();

	// close verts
	if (m_XVDresourceCreated)
	{
		// need to make ptr null, since it uses same space as XPR
		m_xvdResources.SetVidMemPtr( NULL );
		m_xvdResources.Destroy();
	}
}

//-----------------------------------------------------------------------------
void CDXD_Data::Init()
{
}

//-----------------------------------------------------------------------------
void CDXD_Data::SetHeader( int Type, XPR_HEADER*	Hdr )
{
	switch (Type)
	{
		case	TYPE_XPR:
			m_xprHeader.dwHeaderSize	= Hdr->dwHeaderSize;
			m_xprHeader.dwMagic			= Hdr->dwMagic;
			m_xprHeader.dwTotalSize		= Hdr->dwTotalSize;
			break;
		case	TYPE_XVD:
			m_xvdHeader.dwHeaderSize	= Hdr->dwHeaderSize;
			m_xvdHeader.dwMagic			= Hdr->dwMagic;
			m_xvdHeader.dwTotalSize		= Hdr->dwTotalSize;
			break;
		default:
			ASSERT(0);
			break;
	}
}

//-----------------------------------------------------------------------------
void CDXD_Data::SetDBLName( char* Fname )
{
	ASSERT(strlen(Fname) < sizeof(m_Fname) - 1);
	strncpy (m_Fname, Fname, sizeof(m_Fname) - 1);
	m_Fname[sizeof(m_Fname) - 1] = '\0';
}

//-----------------------------------------------------------------------------
char* CDXD_Data::GetDBLName(void)
{
	return(m_Fname);
}

//-----------------------------------------------------------------------------
bool CDXD_Data::InitSysmem(int Type, int SysmemSize, char* Sysmem )
{
	HRESULT	hr;

	switch (Type)
	{
	case	TYPE_XPR:
		m_xprResources.m_xprh = m_xprHeader;
		hr = m_xprResources.AllocateSystemMemory( SysmemSize );
		if (hr != S_OK)
			return (false);
		m_xprResources.CopySysMem(Sysmem);
		break;

	case	TYPE_XVD:
		m_xvdResources.m_xprh = m_xvdHeader;
		hr = m_xvdResources.AllocateSystemMemory( SysmemSize );
		if (hr != S_OK)
			return (false);
		m_xvdResources.CopySysMem(Sysmem);
		break;

	default:
		return (false);
	}

	return (true);
}

//-----------------------------------------------------------------------------
bool CDXD_Data::CreateResource(int Type, int Offset)
{
	HRESULT	hr;

	switch(Type)
	{
	case	TYPE_XPR:
		m_xprResources.SetVidMemPtr( m_pXprMem );
		hr = m_xprResources.CreateDXD(TYPE_XPR, 0, m_Fname);
		if (hr != S_OK)
			return (false);
		m_XPRresourceCreated = true;
		break;

	case	TYPE_XVD:
		m_xvdResources.SetVidMemPtr( m_pXprMem + Offset );
		hr = m_xvdResources.CreateDXD(TYPE_XVD, 0, m_Fname);
		if (hr != S_OK)
			return (false);
		m_XVDresourceCreated = true;
		break;

	default:
		return (false);
	}

	return (true);
}

//-----------------------------------------------------------------------------
bool CDXD_Data::LoadDXDData(const char* Filename, int Filesize)
{
	t_Error		error;
	MediaFile*	Mf;
	bool			err;

	m_pXprMem = (BYTE*)D3D_AllocContiguousMemory( Filesize , D3DTEXTURE_ALIGNMENT );
	assert(m_pXprMem);
	if ( m_pXprMem == NULL )
		return (false);

	// open file
	Mf = Media::Open(Filename, &error);

	if( Mf == 0)
	{
		ASSERT( Mf );
		#ifdef CDROM
		DiscErrorHandler::DisplayError((char*)Filename,DiscErrorHandler::OPEN_FILE_FAILED);
		#endif
		return (false);
	}

	// read DX data
	err = Mf->Read(m_pXprMem, Filesize, &error);

	if (!err)
		return false;

	// Done with the file
	Mf->Close();

	return (true);
}

