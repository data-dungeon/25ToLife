// XboxAutoInstall.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
#include <xtl.h>
#include "xbox\XBFont.h"
#include "xbox\xbinput.h"

#include <xfont.h>
#include "xbox/colorconversions.h"


//#define TEST_SELF_INSTALLER
#define TEST_NAME	"blah"

void CopyDirectory(char* Src, char *Dst);


// remapping drive code -- yick
typedef struct _UNICODE_STRING {
	USHORT	Length;
	USHORT	MaximumLength;
	PSTR		Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

extern "C" XBOXAPI DWORD WINAPI IoCreateSymbolicLink(
	PUNICODE_STRING SymbolicLinkName,
	PUNICODE_STRING DeviceName
);

extern "C" XBOXAPI DWORD WINAPI IoDeleteSymbolicLink(
	PUNICODE_STRING SymbolicLinkName
);

#define DriveE "\\??\\E:"
#define MountPartition "\\Device\\Harddisk0\\Partition1\\DEVKIT"

UNICODE_STRING SymbolicLinkName = {
	strlen(DriveE),
	strlen(DriveE)+1,
	DriveE
};

UNICODE_STRING DeviceName = {
	strlen(MountPartition),
	strlen(MountPartition)+1,
	MountPartition
};

CXBFont			g_Font;
XFONT*			g_pDefaultTrueTypeFont;  // Pointer to the Default Arial TrueTypeFont

CXBPackedResource  m_xpr;      // Packed resources for the app

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D8             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE8       g_pd3dDevice = NULL; // Our rendering device

// defines
#define	COLOR_RED     RGBA_TO_COLOR(255,0,0,255)
#define	COLOR_GREEN   RGBA_TO_COLOR(0,255,0,255)
#define	COLOR_BLUE    RGBA_TO_COLOR(0,0,255,255)
#define	COLOR_WHITE   RGBA_TO_COLOR(255,255,255,255)
#define	COLOR_YELLOW  RGBA_TO_COLOR(255,0,255,255)

XBGAMEPAD*             g_Gamepad;
XBGAMEPAD					g_Pad;

XDEVICE_PREALLOC_TYPE* g_InputDeviceTypes;
DWORD                g_dwNumInputDeviceTypes;


//-----------------------------------------------------------------------------

bool XBOX_InitGamepads()
{
	HRESULT	hr;

	g_dwNumInputDeviceTypes = 0;
	g_InputDeviceTypes      = NULL;

	XInitDevices( g_dwNumInputDeviceTypes, g_InputDeviceTypes );

	if( FAILED( hr = XBInput_CreateGamepads( &g_Gamepad) ) )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------

short	t[256];
WCHAR* AsciiToUnicode ( const char *s)
{
	int	i = 0;

	while (s[0] != 0)
	{
		t[i] = s[0];
		s++; 
		i++;
	}
	t[i] = 0;
	return (WCHAR*)t;
}

//-----------------------------------------------------------------------------

void ReadPads(void)
{
		XBInput_GetInput( g_Gamepad );

		ZeroMemory( &g_Pad, sizeof(g_Pad) );

		g_Pad.wButtons        |= g_Gamepad[0].wButtons;
		g_Pad.wPressedButtons |= g_Gamepad[0].wPressedButtons;
		g_Pad.wLastButtons    |= g_Gamepad[0].wLastButtons;

		for( DWORD b=0; b<8; b++ )
      {
			g_Pad.bAnalogButtons[b]        |= g_Gamepad[0].bAnalogButtons[b];
			g_Pad.bPressedAnalogButtons[b] |= g_Gamepad[0].bPressedAnalogButtons[b];
			g_Pad.bLastAnalogButtons[b]    |= g_Gamepad[0].bLastAnalogButtons[b];
		}

		g_Pad.fX1 += g_Gamepad[0].fX1;
		g_Pad.fY1 += g_Gamepad[0].fY1;
		g_Pad.fX2 += g_Gamepad[0].fX2;
		g_Pad.fY2 += g_Gamepad[0].fY2;

}


//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D()
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice.
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.BackBufferWidth        = 640;
    d3dpp.BackBufferHeight       = 480;
    d3dpp.BackBufferFormat       = D3DFMT_LIN_X8R8G8B8;
    d3dpp.BackBufferCount        = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE;

    // Create the Direct3D device.
    if( FAILED( g_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
        return E_FAIL;

    // Set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 200.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set the view matrix
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-7.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX  matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUp );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Updates the world for the next frame
//-----------------------------------------------------------------------------
VOID Update()
{
    
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID RenderBegin()
{
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
				              D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );


	g_pd3dDevice->BeginScene();
}

VOID RenderEnd()
{
	g_pd3dDevice->EndScene();
}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
char			Title[512];

void __cdecl main()
{
	int		start_copy = 0;
	WIN32_FIND_DATA wfd;
	char strFind[512]  = "d:\\*.xbe";
	char strFind2[512] = "d:\\*";
	HANDLE hFind;
	char			Src[512];
	char			Dst[512];
	char			FileName[512];
	int			CopyCount = 0;
	char			TitleFull[512];
	LD_LAUNCH_DASHBOARD	ld;

	// remap Drive E
	IoDeleteSymbolicLink(&SymbolicLinkName);
	IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);

    // Initialize Direct3D
    if( FAILED( InitD3D() ) )
        return;

 	 // load debug font
	if( FAILED( g_Font.Create( "d:\\Font.xpr" ) ) )
	{
	   OUTPUT_DEBUG_STRING( "Failed to load font.xpr" );
		return;
	}

	// Create the Default Arial TrueTypeFont
	if( FAILED( XFONT_OpenDefaultFont( &g_pDefaultTrueTypeFont ) ) )
	{
	   OUTPUT_DEBUG_STRING( "Failed to create font.xpr" );
		return;
	}

	XBOX_InitGamepads();

	// get the name of the xbe file and use that to create destination directory
	// this app is default.xbe.. the other xbe should be the one we need
	hFind = FindFirstFile( strFind, &wfd );

	int	title_found = 0;
	while(1)
	{
		#if defined (TEST_SELF_INSTALLER)
		if (strcmp(wfd.cFileName, "XboxAutoInstall.xbe"))
		#else
		if (strcmp(wfd.cFileName, "default.xbe"))
		#endif
		{
			int	len;
			title_found = 1;
			strcpy(Title, wfd.cFileName);
			len = strlen(Title);
			while( Title[len] != '.')
				len--;
			
			Title[len] = 0;
			break;
		}

		BOOL result = FindNextFile( hFind, &wfd );
		if (result == 0)
		{
			title_found = 0;
			break;
		}
	}

    while( TRUE )
    {

 		ReadPads();

        // Render the scene
		RenderBegin();
		g_Font.Begin();

		// title
		g_Font.DrawText(160,50, COLOR_WHITE, AsciiToUnicode("Avalanche Software Self Installer"),0,0);
		g_Font.DrawText(180,80, COLOR_GREEN, AsciiToUnicode("Press A Button to start copy"),0,0);
		g_Font.DrawText(180,110, COLOR_GREEN, AsciiToUnicode("Press B Button to run game"),0,0);

		if (title_found == 0)
         g_Font.DrawText(220,200, COLOR_RED, AsciiToUnicode("Title XBE not found"),0,0);
		else
		{
			if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] == 1)
			{
				if (start_copy == 0)
				{
					start_copy = 1;
					hFind = FindFirstFile( strFind2, &wfd );

					// create directory
					#if defined (TEST_SELF_INSTALLER)
					sprintf(Src, "e:\\%s", TEST_NAME);
					#else
					sprintf(Src, "e:\\%s", Title);
					#endif
					CreateDirectory(Src, NULL);

					// start copying the files
					sprintf(Src, "D:\\%s", wfd.cFileName);

					#if defined (TEST_SELF_INSTALLER)
					sprintf(Dst, "E:\\%s\\%s", TEST_NAME, wfd.cFileName);
					#else
					sprintf(Dst, "E:\\%s\\%s", Title, wfd.cFileName);
					#endif
				}
			}

			if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] == 1)
			{
				if (start_copy == 0)
				{
					DWORD	result = XLaunchNewImage("D:\\ttl.xbe", (PLAUNCH_DATA)&ld);
				}
			}

			if (start_copy)
			{
				// found title, so keep copying files..
		      g_Font.DrawText(190,200, COLOR_WHITE, AsciiToUnicode("Copying Files to Hard Disk"),0,0);
				sprintf(FileName, "Copying File %s", wfd.cFileName);									
		      g_Font.DrawText(190,240, COLOR_WHITE, AsciiToUnicode(FileName),0,0);
				sprintf(FileName, "Directory: %s", Title);									
		      g_Font.DrawText(190,270, COLOR_WHITE, AsciiToUnicode(FileName),0,0);
			}

		}

		g_Font.End();
		RenderEnd();

        // Present the backbuffer contents to the display
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		if (start_copy)
		{
			// is this the title xbe.. then rename it to default
			sprintf(TitleFull, "%s.xbe", Title);
			if (!strcmp(wfd.cFileName, TitleFull))
			{
				int	len = strlen(Dst);
				while (1)
				{
					if (Dst[len] == '\\')
					{
						len++;
						strcpy(&Dst[len], "default.xbe");
						break;
					}
					len--;

					if (len <=0)
						break;
				}
			}
			CopyFile(Src, Dst, false);
			SetFileAttributes(Dst, FILE_ATTRIBUTE_NORMAL);
			BOOL result = FindNextFile( hFind, &wfd );

			if (result == 0)
				break;

			sprintf(Src, "D:\\%s", wfd.cFileName);
			#if defined (TEST_SELF_INSTALLER)
			sprintf(Dst, "E:\\%s\\%s", TEST_NAME, wfd.cFileName);
			#else
			sprintf(Dst, "E:\\%s\\%s", Title, wfd.cFileName);
			#endif

			// is it a directory??? if so then we need to go into it and recurse the files again
			DWORD attr = GetFileAttributes(Src);

			// got a directory
			if (attr & FILE_ATTRIBUTE_DIRECTORY)
			{
				CreateDirectory(Dst, NULL);
				CopyDirectory(Src, wfd.cFileName);
			}
		}
    }

	 while(1)
	 {

  		ReadPads();

		RenderBegin();
		g_Font.Begin();

		// title
		g_Font.DrawText(160,50, COLOR_WHITE, AsciiToUnicode("Avalanche Software Self Installer"),0,0);
		g_Font.DrawText(240,300, COLOR_GREEN, AsciiToUnicode("Finished Copy"),0,0);
		g_Font.DrawText(160,330, COLOR_WHITE, AsciiToUnicode("Press A to return to dashboard"),0,0);
		g_Font.End();
		RenderEnd();

        // Present the backbuffer contents to the display
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] == 1)
		{
			ld.dwReason = XLD_LAUNCH_DASHBOARD_MAIN_MENU;
			ld.dwContext = 0;
			ld.dwParameter1 = 0;
			ld.dwParameter2 = 0;
			DWORD	result = XLaunchNewImage(NULL, (PLAUNCH_DATA)&ld);
		}
	 }
}

void CopyDirectory(char* Src, char* Dirname)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	char		Fname[512];
	char		DstFname[512];
	char		Path[512];
	sprintf(Path, "%s\\*", Src);
	char		FileName[512];

	hFind = FindFirstFile(Path, &wfd);

	while (1)
	{
		// create fname;
		sprintf(Fname, "%s\\%s",Src, wfd.cFileName);

		DWORD attr = GetFileAttributes(Fname);

		if (attr & FILE_ATTRIBUTE_DIRECTORY)
		{
			// create dest dir
			#if defined (TEST_SELF_INSTALLER)
			sprintf(DstFname, "e:\\%s\\%s", TEST_NAME, &Fname[3]);
			#else
			sprintf(DstFname, "e:\\%s\\%s", Title, &Fname[3]);
			#endif
			CreateDirectory(DstFname, NULL);
			CopyDirectory(Fname, wfd.cFileName);
		}
		else
		{
			RenderBegin();
			g_Font.Begin();

			// title
	      g_Font.DrawText(190,200, COLOR_WHITE, AsciiToUnicode("Copying Files to Hard Disk"),0,0);
			sprintf(FileName, "Copying File %s", wfd.cFileName);									
		   g_Font.DrawText(190,240, COLOR_WHITE, AsciiToUnicode(FileName),0,0);
			g_Font.DrawText(160,50, COLOR_WHITE, AsciiToUnicode("Avalanche Software Self Installer"),0,0);
			g_Font.DrawText(180,80, COLOR_GREEN, AsciiToUnicode("Press A Button to start copy"),0,0);

			sprintf(FileName, "Directory: %s", Title);									
	      g_Font.DrawText(190,270, COLOR_WHITE, AsciiToUnicode(FileName),0,0);

			g_Font.End();
			RenderEnd();

        // Present the backbuffer contents to the display
			g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			// create dest name
			#if defined (TEST_SELF_INSTALLER)
			sprintf(DstFname, "e:\\%s\\%s", TEST_NAME, &Fname[3]);
			#else
			sprintf(DstFname, "e:\\%s\\%s", Title, &Fname[3]);
			#endif
			CopyFile(Fname, DstFname, false);
			SetFileAttributes(DstFname, FILE_ATTRIBUTE_NORMAL);
		}
		BOOL result = FindNextFile(hFind, &wfd);
		if (result == 0)
			break;
	}
	FindClose(hFind);
}




