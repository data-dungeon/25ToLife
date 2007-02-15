///////////////////////////////////////////////////////////////////////////
//
//  Platform.c
//
//  Platform Specific Module.
//
//  Nate Robins, September 2000.
//
//  Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef DIRECTX
#pragma message( "WARNING: DIRECTX should be defined when compiling this module!" )
#endif


////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

#include "Display/DisplayPCH.h"
#include "Display/DirectX/DXPublished.h"
#include "Display/DirectX/DXInit.h"
#ifdef DIRECTX_PC
#include <shlobj.h>
#endif

//djr: hack to get DirectX to display at full destop resolution
//#define DESKTOP_RES
#ifdef _DEBUG
#undef DESKTOP_RES
#endif


////////////////////////////////////////////////////////////////////////////
//  Pragmas
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Types
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////


#ifdef DIRECTX_PC
#define INI_FILE_LOCATION	"\\25 to life\\"
#define INI_FILE_NAME		"settings.ini"
#endif

LPDIRECT3D8					DirectX_D3D;
LPDIRECT3DDEVICE8			DirectX_Device;
D3DCOLOR						DirectX_ClearColor;
D3DPRESENT_PARAMETERS	DirectX_PresentParams; 

bool							DirectX_Fullscreen;
char							_DirectX_Buffer[ 1024 ];

#ifdef _XBOX
struct
{
	int width;
	int height;
	int format;
	int multisampletype;

} DirectX_Options;

int DirectX_Formats[] = 
{
	D3DFMT_A8R8G8B8,		// 0
	D3DFMT_X8R8G8B8,		// 1
	D3DFMT_R5G6B5,			// 2
	D3DFMT_X1R5G5B5,		// 3
	D3DFMT_LIN_A8R8G8B8,	// 4
	D3DFMT_LIN_X8R8G8B8,	// 5
	D3DFMT_LIN_R5G6B5,	// 6
	D3DFMT_LIN_X1R5G5B5,	// 7
};

int DirectX_MultiSampleTypes[] = 
{
	D3DMULTISAMPLE_NONE,													//  0
	D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR,					//  1
	D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX,				//  2
	D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR,	//  3
	D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,		//  4
	D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR,					//  5
	D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN,				//  6
	D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR,					//  7
	D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,				//  8
	D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN,				//  9
	D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN,				// 10
};
#else
DXInitSettings			UserSettings;
#endif

// Xbox specific Aspect Ratio Change
#define ASPECT_10X11
//#define XBOX_60HZ
#define XBOX_30HZ
//#define	XBOX_24BIT_FP			// use 24-bit precision instead of 53.

//#define XBOX_1080I
#define XBOX_720P
#define XBOX_480P

// This, my friend, is how ugly babies are born
#if defined(_XBOX)
bool g_allowHDTV = true;
#endif

////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void Platform__Init( void )
{
	DrawArray__Initialize( );
}

////////////////////////////////////////////////////////////////////////////

int Platform__Framebuffer( HWND hWnd, int Color, int Depth, int Stencil, int Alpha, int Doublebuffer )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	DirectX_D3D = Direct3DCreate8( D3D_SDK_VERSION );
	assert( DirectX_D3D != 0 );
	
#ifdef _XBOX
	bool	vidmodeset = false;
	D3DDISPLAYMODE fdm;
	fdm.Width  = DirectX_Options.width;
	fdm.Height = DirectX_Options.height;
	fdm.Format = (D3DFORMAT)DirectX_Options.format;

	// Set up the structure used to create the D3DDevice.
	ZeroMemory( &DirectX_PresentParams, sizeof ( DirectX_PresentParams ) );
	
	// Support for different video modes if available on XBOX
	// NTSC / PAL
	DWORD	VidStandard = XGetVideoStandard();
	DWORD	VidType = XGetVideoFlags();

	DirectX_PresentParams.Flags = 0;

	if (VidStandard == XC_VIDEO_STANDARD_NTSC_M || VidStandard == XC_VIDEO_STANDARD_NTSC_J )	// USA
	{
		// Dashboard set to 1080i
		if (VidType & XC_VIDEO_FLAGS_HDTV_1080i)
		{
			#if defined (XBOX_1080I)
			fdm.Width  = 1920;
			fdm.Height = 1080;
			DisplayState.nScreenWidth = 1920;
			DisplayState.nScreenHeight = 1080;
			DirectX_PresentParams.Flags = D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_WIDESCREEN;
			vidmodeset = true;
			#endif
		}
		// Dashboard set to 720p
		if (VidType & XC_VIDEO_FLAGS_HDTV_720p)
		{
			#if defined (XBOX_720P)
			if (vidmodeset == false)
			{
				fdm.Width  = 1280;
				fdm.Height = 720;
				DisplayState.nScreenWidth = 1280;
				DisplayState.nScreenHeight = 720;
				DirectX_PresentParams.Flags = D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN;
				vidmodeset = true;
			}
			#endif
		}
		// Dashboard set to 480p
		if (VidType & XC_VIDEO_FLAGS_HDTV_480p)
		{
			#if defined (XBOX_480P)
			if (vidmodeset == false)
			{
				fdm.Width  = 640;
				fdm.Height = 480;
				DirectX_PresentParams.Flags = D3DPRESENTFLAG_PROGRESSIVE;

				if (VidType & XC_VIDEO_FLAGS_WIDESCREEN)
					DirectX_PresentParams.Flags |= D3DPRESENTFLAG_WIDESCREEN;

				#if defined (ASPECT_10X11)
				DirectX_PresentParams.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
				#endif

				vidmodeset = true;
			}
			#endif
		}
		
		// standard video mode if not set here
		if (vidmodeset == false)
		{
			fdm.Width  = 640;
			fdm.Height = 480;

			#if defined (ASPECT_10X11)
			DirectX_PresentParams.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
			vidmodeset = true;
			#endif
		}
	}
/*
	else if (VidStandard == XC_VIDEO_STANDARD_NTSC_J)	// Japan
	{
		if (vidmodeset == false)
		{
			#if defined (ASPECT_10X11)
			DirectX_PresentParams.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
			vidmodeset = true;
			#endif
		}
	}
*/
	else  // VidStandard == XC_VIDEO_STANDARD_PAL_I		// Europe
	{
		// Dashboard set to 6oHZ PAL
		if (VidType & XC_VIDEO_FLAGS_PAL_60Hz)
		{
			if (vidmodeset == false)
			{
				fdm.Width  = 640;
				fdm.Height = 480;
				DisplayState.nScreenWidth = 640;
				DisplayState.nScreenHeight = 480;
				DirectX_PresentParams.Flags |= D3DPRESENTFLAG_EMULATE_REFRESH_RATE;
				DirectX_PresentParams.FullScreen_RefreshRateInHz = 60;

				#if defined (ASPECT_10X11)
				DirectX_PresentParams.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
				#endif
	
				if (VidType & XC_VIDEO_FLAGS_WIDESCREEN)
					DirectX_PresentParams.Flags |= D3DPRESENTFLAG_WIDESCREEN;
				
				vidmodeset = true;
			}
		}
		else 		// 50hz regular PAL
		{
			if (vidmodeset == false)
			{
				fdm.Width  = 640;
				fdm.Height = 576;
				DisplayState.nScreenWidth = 640;
				DisplayState.nScreenHeight = 576;
				DirectX_PresentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

				#if defined (ASPECT_10X11)
				DirectX_PresentParams.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
				#endif

				if (VidType & XC_VIDEO_FLAGS_WIDESCREEN)
					DirectX_PresentParams.Flags |= D3DPRESENTFLAG_WIDESCREEN;

				vidmodeset = true;
			}
		}
	}

	DirectX_PresentParams.Windowed                        = FALSE;
		
	DirectX_PresentParams.BackBufferWidth                 = fdm.Width;
	DirectX_PresentParams.BackBufferHeight                = fdm.Height;
	DirectX_PresentParams.BackBufferFormat                = D3DFMT_A8R8G8B8;
	DirectX_PresentParams.BackBufferCount                 = 1;		
	if (DisplayState.nScreenWidth > 640)
		DirectX_PresentParams.MultiSampleType              = 0;
	else
		DirectX_PresentParams.MultiSampleType              = DirectX_Options.multisampletype;
	DirectX_PresentParams.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
		
	DirectX_PresentParams.EnableAutoDepthStencil          = TRUE;
	DirectX_PresentParams.AutoDepthStencilFormat          = D3DFMT_D24S8;
      
#if defined (PROFILE)			// profiling on XBox
	DirectX_PresentParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif

#if defined (XBOX_60HZ)
	DirectX_PresentParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE;
#endif

	// 30 hz
#if defined (XBOX_30HZ)
	DirectX_PresentParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#endif

	// Create the Direct3D device. Here we are using the default adapter (most
	// systems only have one, unless they have multiple graphics hardware cards
	// installed) and requesting the HAL (which is saying we want the hardware
	// device rather than a software one). Software vertex processing is 
	// specified since we know it will work on all cards. On cards that support 
	// hardware vertex processing, though, we would see a big performance gain 
	// by specifying hardware vertex processing.
	HRESULT result = IDirect3D8_CreateDevice( DirectX_D3D, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			hWnd,	D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX_PresentParams, &DirectX_Device );
	assert( result == D3D_OK );
#else	
	UserSettings.RequiredD3DDevType = D3DDEVTYPE_HAL;

	DXInit			UserInit( UserSettings );

	DirectX_Device = UserInit.createDevice( hWnd, *DirectX_D3D );
	DirectX_PresentParams = UserInit.dxCreateParams;
	assert( DirectX_Device != NULL );
#endif

	IDirect3DDevice8_Clear( DirectX_Device, 0, NULL, D3DCLEAR_TARGET, 0xFF000000, 1.0f, 0 );
	IDirect3DDevice8_Present( DirectX_Device, NULL, NULL, NULL, NULL );

	// set Xbox FP precision to 24 bits instead of 53..
#if defined (XBOX_24BIT_FP)
	_controlfp(_PC_24, _MCW_PC);
#endif

	return 1;
}

////////////////////////////////////////////////////////////////////////////
#ifndef _XBOX
void Platform__ProcessCommandLine(const char * pCommandLine )
{
	char * pDXOption = 0;
#ifndef DIRECTX_PC
	
	pDXOption = strstr( pCommandLine, "-dxadapter" );
	if( pDXOption )
		UserSettings.Adapter = atoi( pDXOption + strlen( "-dxadapter " ) );

	pDXOption = strstr( pCommandLine, "-dxfullscreen" );

	if( pDXOption )
	{
		UserSettings.Fullscreen = true;
		UserSettings.Windowed = false;

	}
	else
	{
		UserSettings.Windowed = true;
		UserSettings.Fullscreen = false;
	}

	pDXOption = strstr( pCommandLine, "-dxadapterres" );

	if( pDXOption )
		UserSettings.MatchAdapterResolution = true;
	else
	{
		pDXOption = strstr( pCommandLine, "-dxresolution" );

		if( pDXOption )
		{
			char * pResPtr = pDXOption + strlen( "-dxresolution " );

			unsigned int width = 0;
			unsigned int height = 0;

			width = (unsigned int) strtod( pResPtr, & pResPtr );
			
			if( pResPtr  &&  (*pResPtr == 'x') )
			{
				pResPtr++;
	
				height = (unsigned int) strtod( pResPtr, & pResPtr );
			}

			if( width  &&  height )
			{
				UserSettings.Width = width;
				UserSettings.Height = height;
				UserSettings.MatchAdapterResolution = false;
			}
		}
	}	

	pDXOption = strstr( pCommandLine, "-dxshaderdebugging" );

	if( pDXOption )
		UserSettings.ShaderDebugging = true;

#else // DIRECTX_PC
	// build our path to our ini file	
	char iniFileLocation[MAX_PATH + sizeof(INI_FILE_NAME)];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, iniFileLocation);
	ASSERT(result == S_OK);
	strcat(iniFileLocation, INI_FILE_LOCATION);
	strcat(iniFileLocation, INI_FILE_NAME);

	// grab our values from the ini file
	// first fullscreen
	char optionValue[64];
	DWORD ret = GetPrivateProfileString("Settings", "Full Screen", "false", optionValue, sizeof(optionValue), iniFileLocation);
	if( !strcmp(optionValue, "True") )
	{
		UserSettings.Fullscreen = true;
		UserSettings.Windowed = false;

	}
	else
	{
		UserSettings.Windowed = true;
		UserSettings.Fullscreen = false;
	}

	// then resolution
	ret = GetPrivateProfileString("Settings", "Display Width", "800", optionValue, sizeof(optionValue), iniFileLocation);
	if (ret)
	{
			unsigned int width = 0;
			width = atoi(optionValue);
			if( width  )
			{
				UserSettings.Width = width;
			}
	}
	ret = GetPrivateProfileString("Settings", "Display Height", "600", optionValue, sizeof(optionValue), iniFileLocation);
	if (ret)
	{
			unsigned int height = 0;
			height = atoi(optionValue);
			if( height )
			{
				UserSettings.Height= height;
			}
	}
	// then aniso and antialiasing settings
	ret = GetPrivateProfileString("Settings", "Anisotropic Filtering", "0", optionValue, sizeof(optionValue), iniFileLocation);
	if ( ret )
	{
		unsigned int anisoVal = atoi( optionValue );
		if( anisoVal >= 0 ) UserSettings.AnisotropicLevel = anisoVal;
	}

	ret = GetPrivateProfileString("Settings", "Antialiasing", "0", optionValue, sizeof(optionValue), iniFileLocation);
	if ( ret )
	{
		unsigned int antialasVal = atoi( optionValue );
		if ( antialasVal >= 0 ) UserSettings.AntialiasingLevel = antialasVal;
	}

#endif

}
#endif

////////////////////////////////////////////////////////////////////////////
void Platform__Resize( unsigned int width, unsigned int height )
{
	if( DirectX_Device == NULL )
		return;

	if( (DirectX_PresentParams.BackBufferWidth != width)  ||
	    (DirectX_PresentParams.BackBufferHeight != height) )
	{
		DirectX_PresentParams.BackBufferWidth = width;
		DirectX_PresentParams.BackBufferHeight = height;

		HRESULT result = DirectX_Device->Reset( & DirectX_PresentParams );
		ASSERT( result == D3D_OK );
	}
}

////////////////////////////////////////////////////////////////////////////

void Platform__Destroy( void )
{
	DrawArray__Terminate( );

	if( DirectX_Device != NULL )
	{
		ULONG useCount = IDirect3DDevice8_Release( DirectX_Device );
		ASSERT( useCount == 0 );

		DirectX_Device = NULL;
	}
	
	if( DirectX_D3D != NULL )
	{
		ULONG useCount = IDirect3D8_Release( DirectX_D3D );
		ASSERT( useCount == 0 );

		DirectX_D3D = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////

void DirectX__ClearColor( int r, int g, int b, int a )
{
    DirectX_ClearColor = D3DCOLOR_ARGB( a, r, g, b );
}

////////////////////////////////////////////////////////////////////////////

void DirectX__Fullscreen( bool fullscreen )
{
    DirectX_Fullscreen = fullscreen;
}

////////////////////////////////////////////////////////////////////////////

void DirectX__Debug( char* format, ... )
{
    int n;
    va_list args;
    
    va_start( args, format );
    n = _vsnprintf( _DirectX_Buffer, 1024, format, args );
    va_end( args );
    
    ASSERT( n != -1 );

    OutputDebugString( _DirectX_Buffer );
}

////////////////////////////////////////////////////////////////////////////

#ifdef _XBOX
void DirectX__Options( int width, int height, int format, int multisampletype )
{
	ASSERT(format >= 0 && format < sizeof(DirectX_Formats)/sizeof(DWORD));
	ASSERT(multisampletype >= 0 && multisampletype < sizeof(DirectX_MultiSampleTypes)/sizeof(DWORD));
	
	DirectX_Options.width = width;
	DirectX_Options.height = height;
	DirectX_Options.format = DirectX_Formats[format];
	DirectX_Options.multisampletype = DirectX_MultiSampleTypes[multisampletype];
}
#endif

////////////////////////////////////////////////////////////////////////////

float DirectX__FrameRate( void )
{
    int i;
    int f;
    int t;
    static int s = 0;
    static int frame = 0;
    static int times[ 8 ];

    t = GetTickCount();

    times[ frame % 8 ] = t - s;

    s = t;

    frame++;

    f = 0;
    for ( i = 0; i < 8; i++ )
    {
//        DirectX__Debug( "times[]: " );
//        DirectX__Debug( "%d %d %d %d ", times[  0 ], times[  1 ], times[  2 ], times[  3 ] );
//        DirectX__Debug( "%d %d %d %d ", times[  4 ], times[  5 ], times[  6 ], times[  7 ] );
//        DirectX__Debug( "%d %d %d %d ", times[  8 ], times[  9 ], times[ 10 ], times[ 11 ] );
//        DirectX__Debug( "%d %d %d %d ", times[ 12 ], times[ 13 ], times[ 14 ], times[ 15 ] );
//        DirectX__Debug( "\n" );

        f += times[ i ];
    }

    f /= 8;

    if ( f > 0 )
    {
        return ( 1000.0f / f );
    }
    else
    {
        return 0.0f;
    }
}

////////////////////////////////////////////////////////////////////////////
