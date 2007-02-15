//-----------------------------------------------------------------------------
// File: D3DEnumeration.h
//
// Desc: Enumerates D3D adapters, devices, modes, etc.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __D3D_ENUMERATOR_H
#define __D3D_ENUMERATOR_H

#include "DirectX.h"

//-----------------------------------------------------------------------------
// Name: CArrayList
// Desc: A growable array
//-----------------------------------------------------------------------------
class CArrayList
{
public:
	enum ArrayListType
	{
		AL_VALUE,       // entry data is copied into the list
		AL_REFERENCE,   // entry pointers are copied into the list
	};

protected:
    ArrayListType				m_ArrayListType;
    void *						m_pData;
    UINT							m_BytesPerEntry;
    UINT							m_NumEntries;
    UINT							m_NumEntriesAllocated;

public:
	CArrayList( ArrayListType Type, unsigned int bytesPerEntry = 0 );
	~CArrayList( void );
	
	void							clear( void )							{ m_NumEntries = 0; }

	unsigned int				count( void )							{ return m_NumEntries; }
	bool							contains( void * pEntryData );

	HRESULT						add( void * pEntry );
	void							remove( unsigned int entry );
   void *						getPtr( unsigned int Entry );
};

//-----------------------------------------------------------------------------
// Name: struct D3DAdapterInfo
// Desc: Info about a display adapter.
//-----------------------------------------------------------------------------
struct D3DAdapterInfo
{
	int							AdapterOrdinal;
	D3DADAPTER_IDENTIFIER8	AdapterIdentifier;
	CArrayList					DisplayModeList; 
	CArrayList 					DeviceInfoList; 

	D3DAdapterInfo( );
	~D3DAdapterInfo( void );

} ;


//-----------------------------------------------------------------------------
// Name: struct D3DDeviceInfo
// Desc: Info about a D3D device, including a list of D3DDeviceCombos (see below) 
//       that work with the device.
//-----------------------------------------------------------------------------
struct D3DDeviceInfo
{
	int							AdapterOrdinal;
	D3DDEVTYPE					DevType;
	D3DCAPS8						Caps;

	CArrayList 					DeviceComboList; // List of D3DDeviceCombo pointers
	
	D3DDeviceInfo( );
	~D3DDeviceInfo( void );
};



//-----------------------------------------------------------------------------
// Name: struct D3DDeviceCombo
// Desc: A combination of adapter format, back buffer format, and windowed/fullscreen 
//       that is compatible with a particular D3D device (and the app).
//-----------------------------------------------------------------------------
struct D3DDeviceCombo
{
	int							AdapterOrdinal;
	D3DDEVTYPE					DevType;
	D3DFORMAT					AdapterFormat;
	D3DFORMAT					BackBufferFormat;
	bool							IsWindowed;
    
	CArrayList 					DepthStencilFormatList;		// List of D3DFORMATs
	CArrayList 					MultiSampleTypeList;			// List of D3DMULTISAMPLE_TYPEs
	CArrayList 					DSMSConflictList;				// List of D3DDSMSConflicts
	CArrayList 					VertexProcessingTypeList;	// List of VertexProcessingTypes
	CArrayList 					PresentIntervalList;			// List of D3DPRESENT_INTERVALs

	D3DDeviceCombo( );
	~D3DDeviceCombo( void );
} ;

//-----------------------------------------------------------------------------
// Name: struct D3DDSMSConflict
// Desc: A depth/stencil buffer format that is incompatible with a
//       multisample type.
//-----------------------------------------------------------------------------
struct D3DDSMSConflict
{
	D3DFORMAT					DSFormat;
	D3DMULTISAMPLE_TYPE		MSType;
} ;


//-----------------------------------------------------------------------------
// Name: struct D3DBestMatch
// Desc: A set of DeviceCombo/DeviceInfo/Adapter
//-----------------------------------------------------------------------------
struct D3DBestMatch
{
	D3DAdapterInfo *			pBestAdapterInfo;
	D3DDeviceInfo *			pBestDeviceInfo;
	D3DDeviceCombo *			pBestDeviceCombo;
} ;

//-----------------------------------------------------------------------------
// Name: class CD3DEnumeration
// Desc: Enumerates available D3D adapters, devices, modes, etc.
//-----------------------------------------------------------------------------
class D3DEnumerator
{
private:
	IDirect3D8 *				m_pD3D;
	CArrayList 					m_AllowedAdapterFormatList;				// list of D3DFORMATs
	CArrayList 					m_AllowedBackBufferFormatList;			// list of D3DFORMATs
	
private:
	HRESULT						enumerateDevices( D3DAdapterInfo & AdapterInfo, CArrayList & AdapterFormatList );
	HRESULT						enumerateDeviceCombos( D3DDeviceInfo & DeviceInfo, CArrayList & AdapterFormatList );

	void							buildDepthStencilFormatList( D3DDeviceCombo & DeviceCombo );
	void							buildMultiSampleTypeList( D3DDeviceCombo & DeviceCombo );
	void							buildDSMSConflictList( D3DDeviceCombo & DeviceCombo );
	void							buildVertexProcessingTypeList( D3DDeviceInfo & DeviceInfo, D3DDeviceCombo & DeviceCombo );
	void							buildPresentIntervalList( D3DDeviceInfo & DeviceInfo, D3DDeviceCombo & DeviceCombo );

public:
	enum VertexProcessingType
	{
	    SOFTWARE_VP,
		 MIXED_VP,
	    HARDWARE_VP,
		 PURE_HARDWARE_VP
	} ;

	typedef bool(* CONFIRMDEVICECALLBACK)( D3DCAPS8 & Caps, VertexProcessingType vertexProcessingType, 
													D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
	typedef bool(* CONFIRMADAPTERCALLBACK)( unsigned int adapterOrdinal, D3DADAPTER_IDENTIFIER8 & Identifier );

	CArrayList 					m_AdapterInfoList;

	// The following variables can be used to limit what modes, formats, 
	// etc. are enumerated.  Set them to the values you want before calling
	// Enumerate( ).

	CONFIRMDEVICECALLBACK	appConfirmDeviceCallback;
	CONFIRMADAPTERCALLBACK	appConfirmAdapterCallback;

	unsigned int				appMinFullscreenWidth;
	unsigned int				appMinFullscreenHeight;
	unsigned int				appMinColorChannelBits;			// min color bits per channel in adapter format
	unsigned int				appMinAlphaChannelBits;			// min alpha bits per pixel in back buffer format
	unsigned int				appMinDepthBits;
	unsigned int				appMinStencilBits;
	bool							appUsesDepthBuffer;
	bool							appRequiresHardwareVP;
	bool							appRequiresSoftwareVP;
	bool							appUsesPureDevice;
	bool							appUsesMixedVP;					// whether app can take advantage of mixed vp mode
	bool							appRequiresWindowed;
	bool							appRequiresFullscreen;

	D3DEnumerator( bool bFillAdapterFormats = true, bool bFillBackBufferFormats = true );
	~D3DEnumerator( );

	HRESULT						Enumerate( IDirect3D8 & D3D );

	bool							findBestWindowedMode( int iAdapter, bool bRequireHAL, bool bRequireREF, D3DBestMatch & BestDevice );
	bool							findBestFullscreenMode( int iAdapter, bool bRequireHAL, bool bRequireREF, bool bMatchAdapter, D3DBestMatch & BestDevice );

	static unsigned int		numColorChannelBits( D3DFORMAT fmt );
	static unsigned int		numAlphaChannelBits( D3DFORMAT fmt );
	static unsigned int		numDepthBits( D3DFORMAT fmt );
	static unsigned int		numStencilBits( D3DFORMAT fmt );

	void							addAdapterFormat( D3DFORMAT fmt );
	void							fillAdapterFormatList( void );
	void							clearAdapterFormatList( void );

	void							addBackBufferFormat( D3DFORMAT fmt );
	void							fillBackBufferFormatList( void );
	void							clearBackBufferFormatList( void );

} ;

#endif
