//////////////////////////////////////////////////////////
// XPRviewer.cpp 
//
// Author: Paul Blagay
//////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <xtl.h>
#include "xbox\XBFont.h"
#include "xbox\xbinput.h"

#include <xfont.h>
#include "colorconversions.h"


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
DWORD                  g_dwNumInputDeviceTypes;

char	CurrentDir[256];	// dir
int	pos = 0;				// cursor pos
int	TexIndex = 0;		// texture to display
float	X = 200;
float Y = 100;
float	W = 256;
float	H = 256;
int	Format;
int	CurrentMip = 0;
float	MipBias = 0.0f;

float O_X = X;
float O_Y = Y;
float O_W = W;
float O_H = H;

#define YSCROLL	260
int	Y_Scroll = YSCROLL;

bool TextOn = true;

int	sizep2x = 2;
int	sizep2y = 2;

int	num_textures = 0;

int	BGToggle = 0;

char*	TextureFormats[] = {
	{ "32 bit bump / 32 bit with alpha" },
	{ "32 bit no alpha (888)" },
	{ "16 bit, no alpha (565)" },
	{ "16 bit, no alpha (555)" },
	{ "16 bit with alpha (1555)" },
	{ "8 bit alpha only" },
	{ "8 bit luminance only" },
	{ "16 bit alpha luminance" },
	{ "16 bit bump" },
	{ "16 bit bump with luminance" },
	{ "32 bit bump with luminance" },
	{ "32 bit bump / 32 bit with alpha" },
	{ "DXT1 compression" },
	{ "DXT2/3 compression" },
	{ "DXT3/3 compression" },
	{ "DXT4/5 compression" },
	{ "DXT4/5 compression" },
};

int	D3DTypes[] = { 6, 7, 5, 3, 2, 0x19, 0, 0x1a, 0x28, 0x27, 7, 0x3a, 0xc, 0xe, 0xe, 0xf, 0xf, -1};

//D3DFMT_A8R8G8B8|32 bit with alpha (8888)" = {}
//D3DFMT_X8R8G8B8|32 bit, no alpha (888)" = {}
//D3DFMT_R5G6B5|16 bit, no alpha (565)" = {}
//D3DFMT_X1R5G5B5|16 bit, no alpha (555)" = {}
//D3DFMT_A1R5G5B5|16 bit with alpha (1555)" = {}
//D3DFMT_A8|8 bit alpha only" = {}
//D3DFMT_L8|8 bit luminance only" = {}
//D3DFMT_A8L8|16 bit alpha luminance" = {}
//D3DFMT_V8U8|16 bit bump map" = {}
//D3DFMT_L6V5U5|16 bit bump map with luminance" = {}
//D3DFMT_X8L8V8U8|32 bit bump with luminance" = {}
//D3DFMT_Q8W8V8U8|32 bit bump" = {}

//--------------------------------------------------------------//

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
//	 d3dpp.MultiSampleType			= D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR ;
  
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
    D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 10.0f, 0.0f );
    D3DXMATRIX  matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUp );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}

bool XBOX_DrawSpriteTexture ( LPDIRECT3DTEXTURE8 Txt, float Width, float Height, float X, float Y, float Z)
{
 	g_pd3dDevice->SetTexture(0, Txt);
	
	// set texture stages
 	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

	g_pd3dDevice->SetRenderState( D3DRS_YUVENABLE, FALSE);

	// set render states
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
	
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	g_pd3dDevice->SetRenderState (D3DRS_COLORVERTEX, TRUE);

	// set shader
 	g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );

	// set verts for quad
	g_pd3dDevice->Begin( D3DPT_QUADLIST );
	g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 0.0f );
	g_pd3dDevice->SetVertexData4f( D3DVSDE_DIFFUSE, 0.0f,0,0,255);
	g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX,   (X - 0.5f), (Y - 0.5f), Z, 0);

	g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0, 0.0f ); //width
	g_pd3dDevice->SetVertexData4f( D3DVSDE_DIFFUSE, 0.0f,0,0,255);
	g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (X + Width) - 0.5f, (Y - 0.5f), Z, 0);

	g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0, 1.0 );
	g_pd3dDevice->SetVertexData4f( D3DVSDE_DIFFUSE, 0.0f,0,0,255);
	g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (X + Width) - 0.5f, (Y + Height) - 0.5f, Z, 0);
 
	g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 1.0 );
	g_pd3dDevice->SetVertexData4f( D3DVSDE_DIFFUSE, 0.0f,0,0,255);
	g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX,   (X - 0.5f), (Y + Height) - 0.5f, Z, 0);
	g_pd3dDevice->End();

	return true;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID RenderBegin()
{
	// Clear the backbuffer to a blue color
	if (BGToggle == 0)
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
			                  D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );
	}
	else
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
			                  D3DCOLOR_XRGB(0,200,0), 1.0f, 0L );
	}

	// Begin the scene
	g_pd3dDevice->BeginScene();

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&MipBias)));

}

VOID RenderEnd()
{
	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

/////////////////////////////////////////////////////////

struct DirEntry
{
	char	DirectoryName[256];
	int	Type;						// 0 dir, 1 file
};

DirEntry	DirNames[1024];

int GrabDir( char* strFind, WIN32_FIND_DATA* wfd)
{
	HANDLE hFind;
	char	temp[256];

	int	i = 0;

	memset (DirNames,0, sizeof (DirNames));
	// get at dir struct..
	hFind = FindFirstFile( strFind, wfd );

	if (hFind == INVALID_HANDLE_VALUE)
		return (i);

	// go through rest of directorys
	do
	{
   
		strcpy(temp, strFind);
		temp[strlen(temp)-1] = 0;
		strcat(temp, wfd->cFileName);

		DWORD	attr = GetFileAttributes( temp);
		if (attr == FILE_ATTRIBUTE_DIRECTORY)
		{
			DirNames[i].Type = 1;
			strcpy (DirNames[i].DirectoryName, wfd->cFileName);
			i++;
		}
		else
		{
			if (!strcmp (&wfd->cFileName[strlen(wfd->cFileName)-4], ".xpr"))
			{
				DirNames[i].Type = 0;
				strcpy (DirNames[i].DirectoryName, wfd->cFileName);
				i++;
			}
		}
	} while( FindNextFile( hFind, wfd ) );

	FindClose(hFind);
	return (i);
}

//-----------------------------------------------------------------------------
// Show T: drive
// Allow user to pick an XPR to load
//-----------------------------------------------------------------------------
void DrawList(int NumFiles, int pos)
{
	int	color;

		// render it()...
		RenderBegin();

		g_Font.Begin();
		g_Font.SetScaleFactors(0.8f, 0.8f);
		g_Font.DrawText(300,50, COLOR_BLUE, AsciiToUnicode("XPR Viewer"),0,0);
		g_Font.DrawText(230,70, COLOR_WHITE, AsciiToUnicode("(c) Avalanche Software Inc."),0,0);
		g_Font.DrawText(257,93, COLOR_GREEN, AsciiToUnicode("Written by Paul Blagay"),0,0);
		for (int i=0; i<NumFiles; i++)
		{
			if (pos == i)
				color = COLOR_RED;
			else
				color  = COLOR_WHITE;
		
			// Dir - prepend with []
			if (DirNames[i].Type == 1)
			{
				char	temp[256];
   				sprintf (temp, "[%s]", DirNames[i].DirectoryName);
				if (( (Y_Scroll + (i*20)) > 160) && ( (Y_Scroll + (i*20)) < 360) )
					g_Font.DrawText(250,(float)(Y_Scroll + (i*20)),color,AsciiToUnicode(temp), 0,0);
			}
			else
			{
 				if (( (Y_Scroll + (i*20)) > 160) && ( (Y_Scroll + (i*20)) < 360) )
					g_Font.DrawText(250,(float)(Y_Scroll + (i*20)),color,AsciiToUnicode(DirNames[i].DirectoryName), 0,0);
			}
		}

		if (i == 0)
		{
			color  = COLOR_WHITE;
			g_Font.DrawText(250,(float)(Y_Scroll + (i*20)),color,AsciiToUnicode("<Empty>"), 0,0);
		}

		g_Font.End();

		RenderEnd();
}

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
// Show T: drive
// Allow user to pick an XPR to load
//-----------------------------------------------------------------------------

int DisplayDirectory()
{
	// get E drive directory listing
	char strFind[512] = "e:";
	WIN32_FIND_DATA wfd;
	int	NumFiles;

	// get current search dir
	if (strlen(CurrentDir) != 0)
	{
		if (!strcmp (CurrentDir, "\\*"))
		{
			CurrentDir[0] = NULL;
			strcat (strFind, "\\*");
		}
		else
			strcat (strFind, CurrentDir);
	}
	else
	{
		strcat (strFind, "\\*");
	}

	// get dir listing
	NumFiles = GrabDir(strFind, &wfd);

	while (1)
	{
		ReadPads();

		if (g_Pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
		{
			if (pos < NumFiles-1)
			{
				pos++;
				Y_Scroll -= 20;
			}
		}

		if (g_Pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
		{
			if (pos > 0)
			{
				Y_Scroll += 20;
				pos--;
			}
		}

		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] == 1)
		{
			if (pos < (NumFiles - 1))
			{
				pos+=10;
				Y_Scroll -= 20 * 10;

				if (pos > (NumFiles - 1))
				{
					pos = NumFiles - 1;
					Y_Scroll = YSCROLL - (20 * pos);
				}
			}
		}

		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] == 1)
		{
			if (pos > 0)
			{
				Y_Scroll += 20 * 10;
				pos-=10;
				if (pos < 0)
				{
					pos = 0;
					Y_Scroll = YSCROLL;
				}
			}
		}

		// A button

		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] == 1)
		{
			// is this file an XPR?? or a directory
			if (DirNames[pos].Type == 1)
			{
				if (!strcmp(CurrentDir, ""))
				{
					strcat (CurrentDir, "\\");
					strcat (CurrentDir, DirNames[pos].DirectoryName);
					strcat (CurrentDir, "\\*");
				}
				else
				{
					CurrentDir[strlen(CurrentDir)-2] = 0;
					strcat (CurrentDir, "\\");
					strcat (CurrentDir, DirNames[pos].DirectoryName);
					strcat (CurrentDir, "\\*");
				}
				pos = 0;
				Y_Scroll = YSCROLL;
				return (1);
				// change dir and relist
			}
			else
			{
				if (NumFiles != 0)
					return (0);
			}
		}

		// B button
		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] == 1)
		{
			// is this file an XPR?? or a directory
			if (strcmp(CurrentDir,""))
			{
				int ps;
				int len = strlen (CurrentDir);
				ps = len - 2;

				while ( strcmp(&CurrentDir[ps], "\\"))
				{
					CurrentDir[ps] = 0;
					ps--;
				}
				strcat (CurrentDir, "*");
				pos = 0;
				Y_Scroll = YSCROLL - (20 * pos);
				return (1);
			}
		}

		DrawList(NumFiles, pos);
		Sleep(60);
	}
}

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
// Name: main()
// Desc: The application's entry point
//-----------------------------------------------------------------------------

void __cdecl main()
{
	char	temp[256];
	LPDIRECT3DTEXTURE8	Tex;
	D3DSURFACE_DESC		Desc;

	// remap Drive E
	IoDeleteSymbolicLink(&SymbolicLinkName);
	IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);

    // Initialize Direct3D
	 if( FAILED( InitD3D() ) )
	 {
	     OUTPUT_DEBUG_STRING( "Failed to Init D3d" );
        return;
	 }

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

	// init pads
	XBOX_InitGamepads();

restart:
	Y_Scroll = YSCROLL;

	// Display Directory listing.. choose XPR
	while (DisplayDirectory())
	{
	}

	// got an XPR... Load it
	if (!strcmp(CurrentDir, ""))
	{
		strcpy (CurrentDir, "e:\\");
		strcpy(temp, CurrentDir);
		strcat(temp, DirNames[pos].DirectoryName);
	}
	else
	{
		strcpy (temp, "e:");
		strcat(temp, CurrentDir);
		temp[strlen(temp)-1] = 0;
		strcat(temp, DirNames[pos].DirectoryName);
	}

	if (FAILED(m_xpr.Create( temp, 0 )))
	{			   
		OUTPUT_DEBUG_STRING( "Failed to load .XPR file" );
		TexIndex = 0;
		if (!strcmp(CurrentDir, "e:\\"))
		{
			CurrentDir[0] = 0;
		}
		goto	restart;
	}

	Tex = m_xpr.GetTexture(20UL * 0);
	Tex->GetLevelDesc(CurrentMip, &Desc);
	W = (float)Desc.Width;
	H = (float)Desc.Height;
	Format = Desc.Format;

	while( TRUE )
	{
		ReadPads();

		// reset
		if (g_Pad.wPressedButtons & XINPUT_GAMEPAD_START)
		{
			X = O_X;
			Y = O_Y;

			Tex = m_xpr.GetTexture(20UL * TexIndex);
			Tex->GetLevelDesc(CurrentMip, &Desc);
			W = (float)Desc.Width;
			H = (float)Desc.Height;
		}

		if (g_Pad.wPressedButtons & XINPUT_GAMEPAD_BACK)
		{
			if (TextOn == false)
				TextOn = true;
			else
				TextOn = false;
		}

		// choose tex
		if (g_Pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
		{
			if (TexIndex < num_textures - 1)
			{
				CurrentMip = 0;
				MipBias = 0.0f;
 				g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&MipBias)));

				TexIndex++;
				Tex = m_xpr.GetTexture(20UL * TexIndex);
				Tex->GetLevelDesc(CurrentMip, &Desc);
				W = (float)Desc.Width;
				H = (float)Desc.Height;
			}
		}

		if (g_Pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			if (TexIndex > 0)
			{
				CurrentMip = 0;
				MipBias = 0.0f;
 				g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&MipBias)));

				TexIndex--;
				Tex = m_xpr.GetTexture(20UL * TexIndex);
				Tex->GetLevelDesc(CurrentMip, &Desc);
				W = (float)Desc.Width;
				H = (float)Desc.Height;
			}
		}

		// go back to dashboard
		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] == 1 && g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
			LD_LAUNCH_DASHBOARD	ld;
			ld.dwReason = XLD_LAUNCH_DASHBOARD_MAIN_MENU;
			ld.dwContext = 0;
			ld.dwParameter1 = 0;
			ld.dwParameter2 = 0;
			DWORD	result = XLaunchNewImage(NULL, (PLAUNCH_DATA)&ld);
		}

		// change mip levels
		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] == 1)
		{
			Tex = m_xpr.GetTexture(20UL * TexIndex);
			int	Mips = Tex->GetLevelCount();

			if (CurrentMip < Mips-1)
			{
				CurrentMip++;
				MipBias = CurrentMip * 1.0f;
			}

 			g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&MipBias)));

			Tex->GetLevelDesc(CurrentMip, &Desc);
		}

		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] == 1)
		{
			if (CurrentMip > 0)
			{
				CurrentMip--;
				MipBias = CurrentMip * 1.0f;
			}

 			g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&MipBias)));

			Tex = m_xpr.GetTexture(20UL * TexIndex);
			Tex->GetLevelDesc(CurrentMip, &Desc);
		}

		// inc * 10
		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] == 1)
		{
			if (TexIndex + 10 < num_textures - 1)
			{
				CurrentMip = 0;
				MipBias = 0.0f;
 				g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&MipBias)));

				Tex = m_xpr.GetTexture(20UL * TexIndex);
				Tex->GetLevelDesc(CurrentMip, &Desc);
				W = (float)Desc.Width;
				H = (float)Desc.Height;

				TexIndex+=10;
			}
		}

		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] == 1)
		{
			if (TexIndex > 10)
			{
				CurrentMip = 0;
				MipBias = 0.0f;
 				g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&MipBias)));

				Tex = m_xpr.GetTexture(20UL * TexIndex);
				Tex->GetLevelDesc(CurrentMip, &Desc);
				W = (float)Desc.Width;
				H = (float)Desc.Height;
				TexIndex-=10;
			}
		}

		// Toggle background color
		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] == 1)
		{
			BGToggle = ~BGToggle;
		}

		// ^2 x
		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] == 1)
		{
			sizep2x *= 2;
			if (sizep2x > 1024)
				sizep2x = 2;
			W = (float)sizep2x;
		}

		// ^2 y
		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] == 1)
		{
			sizep2y *= 2;
			if (sizep2y > 1024)
				sizep2y = 2;
			H = (float)sizep2y;
		}

		// back out to dir
		if (g_Pad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] == 1)
		{
			if (!strcmp(CurrentDir, "e:\\"))
				CurrentDir[0] = 0;
			pos = 0;
			m_xpr.Destroy();		// clean up xpr..
			TexIndex = 0;
			BGToggle = 0;
			CurrentMip = 0;
			MipBias = 0.0f;
 			g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&MipBias)));
			goto	restart;
		}

		// sticks (pos)
		if (g_Pad.fX1 > 0.5)
			X+= (g_Pad.fX1 * 3.0f);

 		if (g_Pad.fX1 < -0.5)
			X+= (g_Pad.fX1 * 3.0f);

 		if (g_Pad.fY1 < -0.5)
			Y+= (-g_Pad.fY1 * 3.0f);

		if (g_Pad.fY1 > 0.5)
			Y+= (-g_Pad.fY1 * 3.0f);

		// sticks (size)
		if (g_Pad.fX2 > 0.5)
			W+= (g_Pad.fX2 * 3.0f);

 		if (g_Pad.fX2 < -0.5)
			W+= (g_Pad.fX2 * 3.0f);

 		if (g_Pad.fY2 < -0.5)
			H+= (-g_Pad.fY2 * 3.0f);

		if (g_Pad.fY2 > 0.5)
			H+= (-g_Pad.fY2 * 3.0f);

		RenderBegin();


		bool result = XBOX_DrawSpriteTexture(m_xpr.GetTexture(20UL * TexIndex), W,H,X,Y,0);
		if (result == false)
		{
			TexIndex--;
			bool result = XBOX_DrawSpriteTexture(m_xpr.GetTexture(20UL * TexIndex), W,H,X,Y,0);
		}

		if (TextOn)
		{

 			g_Font.Begin();

			Tex = m_xpr.GetTexture(20UL * TexIndex);
			Tex->GetLevelDesc(CurrentMip, &Desc);

			int	Mips = Tex->GetLevelCount();

 			sprintf (temp, "Texture Size X: %d", (int)Desc.Width);
			g_Font.DrawText(60,40, COLOR_WHITE, AsciiToUnicode(temp),0,0);
			sprintf (temp, "Texture Size Y: %d", (int)Desc.Height);
			g_Font.DrawText(60,60, COLOR_WHITE, AsciiToUnicode(temp),0,0);

			sprintf (temp, "Mip Levels: %d", Mips);
			g_Font.DrawText(450,40, COLOR_WHITE, AsciiToUnicode(temp),0,0);

			sprintf (temp, "Current Mip: %d", CurrentMip);
			g_Font.DrawText(450,60, COLOR_WHITE, AsciiToUnicode(temp),0,0);

			int j=0;
			while (1)
			{
				if (D3DTypes[j] == Desc.Format)
					break;

				if (D3DTypes[j] == -1)
					break;

				j++;
			}

 			sprintf (temp, "Format: %s", TextureFormats[j]);
			g_Font.DrawText(200,80, COLOR_WHITE, AsciiToUnicode(temp),0,0);

  			sprintf (temp, "Texture Index: %d", TexIndex);
			g_Font.DrawText(240,20, COLOR_WHITE, AsciiToUnicode(temp),0,0);
			
			sprintf (temp, "Display Size X: %d", (int)W);
			g_Font.DrawText(240,40, COLOR_WHITE, AsciiToUnicode(temp),0,0);
			sprintf (temp, "Display Size Y: %d", (int)H);
			g_Font.DrawText(240,60, COLOR_WHITE, AsciiToUnicode(temp),0,0);

 			g_Font.DrawText(50,360, COLOR_WHITE, AsciiToUnicode("BLACK + WHITE - Goto Dashboard          White: Mip+, Black: Mip-"),0,0);
			g_Font.DrawText(110,380, COLOR_WHITE, AsciiToUnicode("BACK - Toggle Text, START - Reset Initial Pos/Size"),0,0);
			g_Font.DrawText(150,400, COLOR_WHITE, AsciiToUnicode("X - Cycle ^2 Width, Y - Cycle ^2 Height "),0,0);
			g_Font.DrawText(130,420, COLOR_WHITE, AsciiToUnicode("A - Toggle BG Color   B - Return to Directory"),0,0);
			g_Font.End();
		}
		RenderEnd();
	}

	// should never get here
	g_Font.Destroy();
	return;
}

