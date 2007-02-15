//-----------------------------------------------------------------------------
// File: XBResource.cpp
//
// Desc: Loads resources from an XPR (Xbox Packed Resource) file.  
//
// Hist: 03.12.01 - New for April XDK release
//       10.15.02 - Modifed to register resources in a more flexible way
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "platform/PlatformPCH.h"

#include "Layers/Media.h"
#include "Layers/DiscErrorHandler/DiscError.h"

#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#include <xgraphics.h>
#include <stdio.h>
#include "xbutil.h"
#include "xbresource.h"

#include "platform/Xbox/dxdmanager.h"



//-----------------------------------------------------------------------------
// Magic values to identify XPR files
//-----------------------------------------------------------------------------
#define XPR0_MAGIC_VALUE 0x30525058
#define XPR1_MAGIC_VALUE 0x31525058




//-----------------------------------------------------------------------------
// Name: XBResource_SizeOf()
// Desc: Determines the byte size of a D3DResource
//-----------------------------------------------------------------------------
DWORD XBResource_SizeOf( LPDIRECT3DRESOURCE8 pResource )
{
    switch( pResource->GetType() )
    {
        case D3DRTYPE_TEXTURE:       return sizeof(D3DTexture);
        case D3DRTYPE_VOLUMETEXTURE: return sizeof(D3DVolumeTexture);
        case D3DRTYPE_CUBETEXTURE:   return sizeof(D3DCubeTexture);
        case D3DRTYPE_VERTEXBUFFER:  return sizeof(D3DVertexBuffer);
        case D3DRTYPE_INDEXBUFFER:   return sizeof(D3DIndexBuffer);
        case D3DRTYPE_PALETTE:       return sizeof(D3DPalette);
    }
    return 0;
}




//-----------------------------------------------------------------------------
// Name: CXBPackedResource()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBPackedResource::CXBPackedResource()
{
    m_pSysMemData       = NULL;
    m_dwSysMemDataSize  = 0L;
    m_pVidMemData       = NULL;
    m_dwVidMemDataSize  = 0L;
    m_pResourceTags     = NULL;
    m_dwNumResourceTags = 0L;
	 m_numTextures = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CXBPackedResource()
// Desc: Destructor
//-----------------------------------------------------------------------------
CXBPackedResource::~CXBPackedResource()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: GetData()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
VOID* CXBPackedResource::GetData( const CHAR* strName ) const
{
    if( NULL==m_pResourceTags || NULL==strName )
        return NULL;

    for( DWORD i=0; m_pResourceTags[i].strName; i++ )
    {
        if( !_stricmp( strName, m_pResourceTags[i].strName ) )
            return &m_pSysMemData[m_pResourceTags[i].dwOffset];
    }

    return NULL;
}

//-----------------------------------------------------------------------------
// setup the system memory and video memory
HRESULT CXBPackedResource::AllocateMemory( int SysmemSize, int VidmemSize, char* Sysmem )
{
	// Allocate memory
	m_pSysMemData = new BYTE[SysmemSize];
	assert(m_pSysMemData);
	if ( m_pSysMemData == NULL )
		return E_OUTOFMEMORY;

	// Compute memory requirements
	m_dwSysMemDataSize = SysmemSize;
	m_dwVidMemDataSize = VidmemSize;

	m_dwVidMemDataSize = (m_dwVidMemDataSize + (XBOX_DVD_SECTOR_SIZE-1)) & ~(XBOX_DVD_SECTOR_SIZE-1);
	m_pVidMemData = (BYTE*)D3D_AllocContiguousMemory( m_dwVidMemDataSize , D3DTEXTURE_ALIGNMENT );
	assert(m_pVidMemData);
	if ( m_pVidMemData == NULL )
		return E_OUTOFMEMORY;

	// copy sysmem to object
	DWORD sysMemRead = SysmemSize;
	memcpy (m_pSysMemData, Sysmem, SysmemSize);
	return S_OK;
}

//-----------------------------------------------------------------------------

HRESULT CXBPackedResource::AllocateSystemMemory( int SysmemSize )
{
	// Allocate memory
	m_pSysMemData = new BYTE[SysmemSize];
	assert(m_pSysMemData);
	if ( m_pSysMemData == NULL )
		return E_OUTOFMEMORY;

	// Compute memory requirements
	m_dwSysMemDataSize = SysmemSize;
	return S_OK;
}

//-----------------------------------------------------------------------------

HRESULT CXBPackedResource::AllocateD3DMemory( int VidmemSize )
{
	m_dwVidMemDataSize = VidmemSize;
	m_dwVidMemDataSize = (m_dwVidMemDataSize + (XBOX_DVD_SECTOR_SIZE-1)) & ~(XBOX_DVD_SECTOR_SIZE-1);
	m_pVidMemData = (BYTE*)D3D_AllocContiguousMemory( m_dwVidMemDataSize , D3DTEXTURE_ALIGNMENT );
	assert(m_pVidMemData);
	if ( m_pVidMemData == NULL )
		return E_OUTOFMEMORY;

	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT CXBPackedResource::CopySysMem (char* Sysmem)
{
	memcpy (m_pSysMemData, Sysmem, m_dwSysMemDataSize);
	return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT CXBPackedResource::SetVidMemPtr (BYTE* Ptr)
{
	m_pVidMemData = Ptr;
	return S_OK;
}

BYTE*	 CXBPackedResource::GetVidMemPtr(void)
{
	return (m_pVidMemData);
}

//-----------------------------------------------------------------------------
// Load the DX Chunk to memory
//-----------------------------------------------------------------------------
HRESULT CXBPackedResource::LoadDXData( const char* Fname, int DXDSize, BYTE* Buff )
{
	t_Error error;
	MediaFile*	Mf;
	bool	err;
	Mf = Media::Open(Fname, &error);

	if( Mf == 0)
	{
		ASSERT( Mf );
		#ifdef CDROM
		DiscErrorHandler::DisplayError((char*)Fname,DiscErrorHandler::OPEN_FILE_FAILED);
		#endif
		return E_FAIL;
	}

	// read DX data
	err = Mf->Read(Buff, DXDSize, &error);

//pab	if (err)
//pab		return E_FAIL;

	// Done with the file
	Mf->Close();
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Create() using header loaded earlier
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
HRESULT CXBPackedResource::CreateDXD( DWORD Type, DWORD dwNumResourceTags, char* Fname )
{
	BOOL bHasResourceOffsetsTable = FALSE;

	if (Type == TYPE_XPR)
	{
		if (m_xprh.dwMagic == XPR0_MAGIC_VALUE )
			bHasResourceOffsetsTable = FALSE;
		else 
		{
			bHasResourceOffsetsTable = FALSE;

			if (m_xprh.dwMagic < 0 || m_xprh.dwMagic > 4096)
				return E_INVALIDARG;
			m_numTextures = m_xprh.dwMagic;
		}
	}
	else
	{
		if( m_xprh.dwMagic == XPR0_MAGIC_VALUE )
			bHasResourceOffsetsTable = false;
		else if( m_xprh.dwMagic == XPR1_MAGIC_VALUE )
			bHasResourceOffsetsTable = true;
		else
			ASSERT( 0 );
	}

	// Extract resource table from the header data
	if( bHasResourceOffsetsTable )
	{
		m_dwNumResourceTags = *(DWORD*)(m_pSysMemData+0);
		m_pResourceTags     = (XBRESOURCE*)(m_pSysMemData+4);

		// Patch up the resource strings
		for( DWORD i=0; i<m_dwNumResourceTags; i++ )
			m_pResourceTags[i].strName = (CHAR*)( m_pSysMemData + (DWORD)m_pResourceTags[i].strName );
	}

	m_pResourceFilename = strdup( Fname );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
HRESULT CXBPackedResource::Create( const CHAR* strFilename, DWORD dwNumResourceTags,
                                   XBRESOURCE* pResourceTags )
{
		BOOL bHasResourceOffsetsTable = FALSE;

	// Read in and verify the XPR magic header
	XPR_HEADER xprh;

	t_Error error;
	MediaFile*	Mf;
	bool	err;
	Mf = Media::Open(strFilename, &error);

	if( Mf == 0)
	{
		ASSERT( Mf );
#ifdef CDROM
		DiscErrorHandler::DisplayError((char*)strFilename,DiscErrorHandler::OPEN_FILE_FAILED);
#endif
		return E_FAIL;
	}

	// read in the header
	char	*Buff;
	Buff = (char*)memAlloc(XBOX_DVD_SECTOR_SIZE, 32);

	err = Mf->Read(Buff, XBOX_DVD_SECTOR_SIZE, &error);

	memcpy (&xprh, Buff, sizeof(XPR_HEADER));

	if( xprh.dwMagic == XPR0_MAGIC_VALUE )
	{
        bHasResourceOffsetsTable = FALSE;
   }
   else 
   {
		bHasResourceOffsetsTable = FALSE;

		 if (xprh.dwMagic < 0 || xprh.dwMagic > 4096)
		 {
			 Mf->Close();
			OUTPUT_DEBUG_STRING( "Invalid Xbox Packed Resource (.xpr) file" );	
   			return E_INVALIDARG;
		 }
		m_numTextures = xprh.dwMagic;
   }

     // Compute memory requirements
    m_dwSysMemDataSize = xprh.dwHeaderSize - sizeof(XPR_HEADER);
    m_dwVidMemDataSize = xprh.dwTotalSize - xprh.dwHeaderSize;

    // Allocate memory
    m_pSysMemData = new BYTE[m_dwSysMemDataSize];
	 assert(m_pSysMemData);
	 if ( m_pSysMemData == NULL )
		 return E_OUTOFMEMORY;
	 m_dwVidMemDataSize = (m_dwVidMemDataSize + (XBOX_DVD_SECTOR_SIZE-1)) & ~(XBOX_DVD_SECTOR_SIZE-1);
    m_pVidMemData = (BYTE*)D3D_AllocContiguousMemory( m_dwVidMemDataSize , D3DTEXTURE_ALIGNMENT );
	 assert(m_pVidMemData);
	 if ( m_pVidMemData == NULL )
		 return E_OUTOFMEMORY;

	DWORD sysMemRead = 2048 - sizeof(XPR_HEADER);
	DWORD sysMemCopy = m_dwSysMemDataSize > sysMemRead ? sysMemRead : m_dwSysMemDataSize;

	memcpy( m_pSysMemData, (char *) (Buff + sizeof(XPR_HEADER)), sysMemCopy );
	memFree(Buff);

	if( m_dwSysMemDataSize > sysMemRead )
	{
		ASSERT( ((m_dwSysMemDataSize - sysMemCopy) % 2048) == 0 );

		err = Mf->Read( m_pSysMemData + sysMemCopy, m_dwSysMemDataSize - sysMemCopy, & error );
		ASSERT( err );
	}

// 	 err = Mf->Read(m_pSysMemData, m_dwSysMemDataSize, &error);
	 err = Mf->Read(m_pVidMemData, m_dwVidMemDataSize, &error);

    // Done with the file
	 Mf->Close();
    
    // Extract resource table from the header data
    if( bHasResourceOffsetsTable )
    {
        m_dwNumResourceTags = *(DWORD*)(m_pSysMemData+0);
        m_pResourceTags     = (XBRESOURCE*)(m_pSysMemData+4);

        // Patch up the resource strings
        for( DWORD i=0; i<m_dwNumResourceTags; i++ )
            m_pResourceTags[i].strName = (CHAR*)( m_pSysMemData + (DWORD)m_pResourceTags[i].strName );
    }

    // Use user-supplied number of resources and the resource tags
    if( dwNumResourceTags !=0 || pResourceTags != NULL )
    {
        m_pResourceTags     = pResourceTags;
        m_dwNumResourceTags = dwNumResourceTags;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateFromMemory()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
HRESULT CXBPackedResource::CreateFromMemory( char* Buffer, DWORD dwNumResourceTags,
                                   XBRESOURCE* pResourceTags )
{
    BOOL bHasResourceOffsetsTable = FALSE;

	 m_numTextures = 0;

    // Read in and verify the XPR magic header
    XPR_HEADER xprh;
	 memcpy (&xprh, Buffer, sizeof(XPR_HEADER));

	 if( xprh.dwMagic == XPR0_MAGIC_VALUE )
    {
        bHasResourceOffsetsTable = FALSE;
    }
    else 
    {
		bHasResourceOffsetsTable = FALSE;

		 if (xprh.dwMagic < 0 || xprh.dwMagic > 4096)
		 {
				OUTPUT_DEBUG_STRING( "Invalid Xbox Packed Resource (.xpr) file" );	
				return E_INVALIDARG;
		 }
			m_numTextures = xprh.dwMagic;
    }

     // Compute memory requirements
    m_dwSysMemDataSize = xprh.dwHeaderSize - sizeof(XPR_HEADER);
    m_dwVidMemDataSize = xprh.dwTotalSize - xprh.dwHeaderSize;

    // Allocate memory
    m_pSysMemData = new BYTE[m_dwSysMemDataSize];
	 assert(m_pSysMemData);
	 if ( m_pSysMemData == NULL )
		 return E_OUTOFMEMORY;
    m_pVidMemData = (BYTE*)D3D_AllocContiguousMemory( m_dwVidMemDataSize, D3DTEXTURE_ALIGNMENT );
	 assert(m_pVidMemData);
	 if ( m_pVidMemData == NULL )
		 return E_OUTOFMEMORY;

    // Read in the data from the file
	 memcpy(m_pSysMemData, Buffer + sizeof (XPR_HEADER), m_dwSysMemDataSize);
	 memcpy(m_pVidMemData, Buffer + sizeof (XPR_HEADER) + m_dwSysMemDataSize, m_dwVidMemDataSize);
    
    // Extract resource table from the header data
    if( bHasResourceOffsetsTable )
    {
        m_dwNumResourceTags = *(DWORD*)(m_pSysMemData+0);
        m_pResourceTags     = (XBRESOURCE*)(m_pSysMemData+4);

        // Patch up the resource strings
        for( DWORD i=0; i<m_dwNumResourceTags; i++ )
            m_pResourceTags[i].strName = (CHAR*)( m_pSysMemData + (DWORD)m_pResourceTags[i].strName );
    }

    // Use user-supplied number of resources and the resource tags
    if( dwNumResourceTags !=0 || pResourceTags != NULL )
    {
        m_pResourceTags     = pResourceTags;
        m_dwNumResourceTags = dwNumResourceTags;
    }

    return S_OK;
}

 // Functions to retrieve resources by their offset
 VOID* CXBPackedResource::GetData( DWORD dwOffset ) const
{ 
	return &m_pSysMemData[dwOffset]; 
}

//-----------------------------------------------------------------------------
// Name: GetResourceTags()
// Desc: Retrieves the resource tags
//-----------------------------------------------------------------------------
HRESULT CXBPackedResource::GetResourceTags( DWORD* pdwNumResourceTags,
                                            XBRESOURCE** ppResourceTags )
{
    if( pdwNumResourceTags )
        (*pdwNumResourceTags) = m_dwNumResourceTags;

    if( ppResourceTags )
        (*ppResourceTags) = m_pResourceTags;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Cleans up the packed resource data
//-----------------------------------------------------------------------------
VOID CXBPackedResource::Destroy() 
{
    if( m_pSysMemData != NULL )
        delete[] m_pSysMemData;
    m_pSysMemData      = NULL;
    m_dwSysMemDataSize = 0L;
    
    if( m_pVidMemData != NULL )
        D3D_FreeContiguousMemory( m_pVidMemData );
    m_pVidMemData      = NULL;
    m_dwVidMemDataSize = 0L;
    
    m_pResourceTags     = NULL;
    m_dwNumResourceTags = 0L;
}



