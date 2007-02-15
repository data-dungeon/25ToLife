//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
// XPRess:
//
// This tool takes a .DBL file and parses the textures
// to create an .XPR file for the xbox. 
//
// Author: Paul Blagay, additional code by Dan Silver
// Date: 5/19/04
//
// Command Line Options:
//
//		-usemapattrib		= take the texture type & mip info from map attrib file
//		-writetgas			= writes out the texture in the DBL to BMP files.
//		-compress			= uses DXT compression modes intelligently
//    -notextures			= remove texture chunk from output DBL file
//		-format <string>	= set texture format to use (D3D_define)
//		-o <filename>		= override output filename (default <inputfilename>.dbx)
//    -p <prefix>			= prefix for header file
//		-e <filename>		= error log
//		-verbose				= turn on screen info
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

#include "bundler/bundler.h"
#include "bundler/texture.h"
#include "bundler/vb.h"

// tupperware general
#include "tupperware/aggregate.h"
#include "tupperware/list.h"
#include "tupperware/scalar.h"
#include "tupperware/pool.h"
#include "tupperware/tuparray.h"
#include "tupperware/tupstring.h"
#include "tupperware/memorymanager.h"
#include "tupperware/keeper.h"
#include "TupperAttrib\KeyValue.h"
#include "TupperAttrib\AuxKeyValue.h"
#include "Tupperware/LoadSave.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "XPRess.h"
#include "DibbleFile.h"

// our singletons
XPRess			XPRessObj;
CBundler			bundler;
TupAuxKeyValue m_Keys;

XPRess_MapAttrib	MapAttrib[4096];		// max 4096 textures per dbl
int					g_NumTextures = 0;	// how many textures

//////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	// process our params
	for (int i = 1; i < argc; i++)
	{
		// remove textures from output DBL texture chunk
		if( lstrcmpi("notextures", &(argv[i][1])) == 0 )
		{
			XPRessObj.m_NoTextures = true;
		}
		if( lstrcmpi("compress", &(argv[i][1])) == 0 )
		{
			XPRessObj.m_Compress = true;
		}
		else if( lstrcmpi("o", &(argv[i][1])) == 0 )
		{
			XPRessObj.m_OverrideOutputName = true;
			strcpy (XPRessObj.m_DibbleOutputName, argv[i+1]);
		}
		else if( lstrcmpi("writetgas", &(argv[i][1])) == 0 )
		{
			XPRessObj.m_WriteBMPs = true;
		}
		else if( lstrcmpi("texturelist", &(argv[i][1])) == 0 )
		{
			XPRessObj.m_TextureList = true;
		}
		else if( lstrcmpi("usemapattrib", &(argv[i][1])) == 0 )
		{
			XPRessObj.m_UseMapAttrib = true;
			XPRessObj.m_RequestUseMapAttrib = true;
		}
		else if( lstrcmpi("format", &(argv[i][1])) == 0 )
		{
			XPRessObj.m_TextureFormat = true;
			strcpy (XPRessObj.m_FormatType, argv[i+1]);
		}
		else if( lstrcmpi("clean", &(argv[i][1])) == 0 )
		{
			XPRessObj.m_RunClean = true;
		}
		else if( lstrcmpi("ps2saturation", &(argv[i][1])) == 0 )
		{
			XPRessObj.m_PS2Saturation = true;
		}
	}

	if (argc == 1)
	{
		XPRessObj.InitBundler(argc, argv);
		return 0;
	}

	// save off dbl filename, and output name
	strcpy(XPRessObj.m_DibbleName, argv[1]);

	// create new save name if we didnt specify one on command line
	if (!XPRessObj.m_OverrideOutputName)
	{
		strcpy(XPRessObj.m_DibbleOutputName, XPRessObj.m_DibbleName);
		XPRessObj.m_DibbleOutputName[strlen(XPRessObj.m_DibbleOutputName)-1] = 'x';
	}

	// load the key file (if it doesnt exist, just do the defaults
	XPRessObj.LoadKeys();

	// load a .dbl file
	printf("Now Loading DBL to memory....\n\n");
	XPRessObj.LoadDBL();
	
	// process textures
	printf("Processing textures and creating .XPR file....\n\n");
	if (XPRessObj.m_PS2Saturation)
		printf("Using PS2 Saturation....\n\n");

	XPRessObj.ProcessTextures();

	// call bundler
	XPRessObj.InitBundler(argc, argv);
	XPRessObj.RunBundler(XPRessObj.m_NumTextures);
	printf("Textures processed....\n\n");

	// write out new dbl (if no textures was specified)
	if (XPRessObj.m_NoTextures)
	{
		printf("Creating new .DBL file without textures....\n\n");
		XPRessObj.SaveDBL();
	}

	// Clean up
	bundler.CloseFiles();
	if (XPRessObj.m_RunClean)
	{
		// Kill the key file
		if (XPRessObj.m_RequestUseMapAttrib)
		{
			char TempFile[512];
			strcpy(TempFile, XPRessObj.m_DibbleName);
			strcpy(&TempFile[strlen(TempFile)-3], "AuxImageAttr");
			#ifdef _DEBUG
			printf("Deleting \"%s\"\n", TempFile);
			#endif
			_unlink(TempFile);
		}

		// Kill & rename the dbl!
		if (XPRessObj.m_NoTextures)
		{
			_unlink(XPRessObj.m_DibbleName);
			#ifdef _DEBUG
			printf("Deleting \"%s\"\n", XPRessObj.m_DibbleName);
			#endif

			// Now rename the dbx
			rename(XPRessObj.m_DibbleOutputName, XPRessObj.m_DibbleName);
			#ifdef _DEBUG
			printf("Renaming \"%s\" to \"%s\"\n", XPRessObj.m_DibbleOutputName, XPRessObj.m_DibbleName);
			#endif
		}

		// Kill the h file
		char TempFile[512];
		strcpy(TempFile, XPRessObj.m_DibbleName);
		strcpy(&TempFile[strlen(TempFile)-3], "h");
		#ifdef _DEBUG
		printf("Deleting \"%s\"\n", TempFile);
		#endif
		_unlink(TempFile);
	}

	// if we asked for texture list, write it out now
	if (XPRessObj.m_TextureList)
	{
		char	tempf[256];
		strcpy(tempf, XPRessObj.m_DibbleName);
		strcpy(&tempf[strlen(tempf)-3], "lst");

		printf ("Writing out Texture List file\n");
		FILE*	fp = fopen(tempf, "wt");
		for (int j = 0; j < g_NumTextures; j++)
		{
			fprintf(fp, "Texture Index: %d	Texture Path: %s	Miplevels: %d\n", j, MapAttrib[j].Fname, MapAttrib[j].MipLevels);
		}
		fclose(fp);
	}
   
	return 0;
}

//////////////////////////////////////////////////////////
// load the key file (if it doesnt exist, just do the defaults

bool XPRess::LoadKeys()
{
	char			KeyFname[256];
	TupString	TupFname;

	// load the key tup
	strcpy(KeyFname, XPRessObj.m_DibbleName);
	strcpy(&KeyFname[strlen(KeyFname)-3], "AuxImageAttr");

	TupFname = KeyFname;

	TupperwareAggregate *pInMapAgg = new TupperwareAggregate;

	int result = TupperwareLoadSaveManager::Read(TupFname,&pInMapAgg);
	if (result)
	{
		printf("Using Texture Map Attrib file....\n\n");

		m_Keys.EntryNamesCaseSensitive(false);
		m_Keys.SetMaxEntryNameLength(31);
		m_Keys.LoadEntries(pInMapAgg);
		return true;
	}
	else
	{
		XPRessObj.m_UseMapAttrib = false;
		return false;
	}
}

//////////////////////////////////////////////////////////

XPRess::XPRess()
{
	m_NoTextures = false;
	m_NumTextures = 0;
	m_OverrideOutputName = false;
	m_pBuffer = 0;
	m_WriteBMPs = false;
	m_UseMapAttrib = false;
	m_TextureFormat = false;
	m_Compress = false;
	m_RequestUseMapAttrib = false;
	m_RunClean = false;
	m_PS2Saturation = false;
}

//////////////////////////////////////////////////////////

XPRess::~XPRess()
{
}

//////////////////////////////////////////////////////////

int XPRess::InitBundler(int argc, char* argv[])
{
    // It's important to return an error message since otherwise Visual Studio
    // continues with the build, and it's easy to not realize that bundler failed.
    // It's also important to print a VC.Net compatible error message.
    const int kFailed = 10;
    HRESULT hr;

	 // bundler
	hr = bundler.Initialize( argc, argv );

	 if( hr == E_INVALIDARG )
    {
        bundler.PrintUsage();
        return kFailed;
    }
    else if( hr == E_OUTOFMEMORY )
    {
        bundler.ErrorMsg( "Not enough memory.\n" );
        return kFailed;
    }
    else if( FAILED(hr) )
    {
        bundler.ErrorMsg( "D3D Initialization failed.\n" );
        return kFailed;
    }

	return 0;
}

//////////////////////////////////////////////////////////

CVertexBuffer	vb(&bundler);
CTexture2D		tex(&bundler);

int XPRess::RunBundler(int NumTextures)
{
   HRESULT hr;
	DWORD cbHeader, cbData;
	BOOL Result;
	DibbleFile* pDibble;


	pDibble = dm.FindDibble( XPRessObj.m_DibbleName );

	if( !pDibble )
		return S_FALSE;

	// loop thru all texture and add to the xpr...
	for (int i = 0; i < NumTextures; i++)
	{
		Result = WriteTGAToMemory( pDibble->m_ppTextures[i] );

		if (XPRessObj.m_WriteBMPs)
		{
			char TheFileName[256];

			sprintf( TheFileName, "Texture%d.tga", i );

			HANDLE hFile = CreateFile( TheFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

			if( hFile )
			{
				FileWrite( hFile, m_pBuffer, m_BufferSize );
				CloseHandle( hFile );
			}
		}

		// if we are using a map file, get the map attribs and use those
		// instead of the command line

		tex.m_dwLevels = MapAttrib[i].MipLevels;

		if (XPRessObj.m_UseMapAttrib)
		{
			// get texture format
			if (MapAttrib[i].TextureFormat != 0)
			{
				if (!strcmp(MapAttrib[i].TextureFormat, "BestFit"))
				{
					switch (MapAttrib[i].CompressionType)
					{
					case	1:
						lstrcpyA( tex.m_strFormat, "D3DFMT_DXT1" );
						break;
					case	2:
						lstrcpyA( tex.m_strFormat, "D3DFMT_DXT2" );
						break;
					case	3:
						lstrcpyA( tex.m_strFormat, "D3DFMT_DXT3" );
						break;
					case	4:
						lstrcpyA( tex.m_strFormat, "D3DFMT_DXT4" );
						break;
					case	5:
						lstrcpyA( tex.m_strFormat, "D3DFMT_DXT5" );
						break;
					default:
						lstrcpyA( tex.m_strFormat, "D3DFMT_A8R8G8B8" );
						break;
					}
					tex.m_nFormat = tex.FormatFromString( tex.m_strFormat );
				}
				else
				{
					if (MapAttrib[i].TextureFormat != 0)
					{
						lstrcpyA( tex.m_strFormat, MapAttrib[i].TextureFormat );
						tex.m_nFormat = tex.FormatFromString( tex.m_strFormat );
					}
					else
					{
						lstrcpyA( tex.m_strFormat, "D3DFMT_A8R8G8B8" );
						tex.m_nFormat = tex.FormatFromString( tex.m_strFormat );
					}
				}
			}
			else
			{
				lstrcpyA( tex.m_strFormat, "D3DFMT_A8R8G8B8" );
				tex.m_nFormat = tex.FormatFromString( tex.m_strFormat );
			}

			// get filter type
			// get two sided.. 

		}
		else
		{
			if (XPRessObj.m_Compress == false)
			{
				// set format from commandline
				if (XPRessObj.m_TextureFormat)
				{
					lstrcpyA( tex.m_strFormat, XPRessObj.m_FormatType );
					tex.m_nFormat = tex.FormatFromString( tex.m_strFormat );
				}
				else	// default format
				{
		//			lstrcpyA( tex.m_strFormat, "D3DFMT_DXT1" );
					lstrcpyA( tex.m_strFormat, "D3DFMT_A8R8G8B8" );
					tex.m_nFormat = tex.FormatFromString( tex.m_strFormat );
				}
			}
			else		// compress intelligently
			{	
				switch (MapAttrib[i].CompressionType)
				{
				case	1:
					lstrcpyA( tex.m_strFormat, "D3DFMT_DXT1" );
					break;
				case	2:
					lstrcpyA( tex.m_strFormat, "D3DFMT_DXT2" );
					break;
				case	3:
					lstrcpyA( tex.m_strFormat, "D3DFMT_DXT3" );
					break;
				case	4:
					lstrcpyA( tex.m_strFormat, "D3DFMT_DXT4" );
					break;
				case	5:
					lstrcpyA( tex.m_strFormat, "D3DFMT_DXT5" );
					break;
				default:
					lstrcpyA( tex.m_strFormat, "D3DFMT_A8R8G8B8" );
					break;
				}
				tex.m_nFormat = tex.FormatFromString( tex.m_strFormat );
			}
		}

		// write this texture out to the XPR
		hr = tex.SaveToBundle( &cbHeader, &cbData, (char*)m_pBuffer, m_BufferSize );
		if( FAILED(hr) )
			return hr;

		if( !bundler.m_bQuiet )
		{
			printf( "Texture: Wrote %s out in format %s (%d bytes)\n", 
						tex.m_strSource, tex.m_strFormat, cbData );
			printf( "     %d x %d, %d levels\n\n", tex.m_dwWidth, tex.m_dwHeight, tex.m_dwLevels );
		}

		bundler.m_pResources[bundler.m_dwNumResources++].dwOffset = bundler.m_cbHeader;
		bundler.m_cbHeader += cbHeader;
	}

	bundler.HandleEOFToken();

	return S_OK;
}

//////////////////////////////////////////////////////////
// loads the dbl file to memory. Then parses through it
// and converts all the images to 'bmp' in memory to be
// later parsed by the bundler code

void XPRess::LoadDBL()
{
	dm.LoadDibble(XPRessObj.m_DibbleName);
}

//////////////////////////////////////////////////////////
// removes the texture data from the dbl, and fixes any
// chunk header data that needs to be fixed. Then saves
// out a new .dbl without textures.

void XPRess::SaveDBL()
{
	// remove the textures from the dbl

	// save the new dbl out
}

BOOL XPRess::WriteBMPToMemory( BITMAP2 *pbm )
{
	if( !pbm )
		return FALSE;

	int Width = pbm->Width;
	int Height = pbm->Height;
	int BPP = 32;

	HBITMAP hbm = CreateDIB( Width, Height, BPP );

	if( !hbm )
		return FALSE;

	BITMAP2 bmDst = GetBitmap2Struct( hbm, FALSE );

	BitmapToBitmap( &bmDst, 0, 0, Width, Height, pbm, 0, 0, Width, Height );

	BOOL bResult = FALSE;
	HDC hdc = GetDC( NULL );
	BITMAPINFO *pbmi = CreateBitmapHeader( hbm );

	if( pbmi )
	{
		if( pbmi->bmiHeader.biClrUsed > 0 )
			GdiGetColorTable( hbm, pbmi->bmiColors, pbmi->bmiHeader.biClrUsed );

		DWORD dwWidthBytes = pbmi->bmiHeader.biSizeImage / pbmi->bmiHeader.biHeight;
		BYTE *pBits = (BYTE *) GlobalAlloc( GMEM_FIXED, dwWidthBytes );

		if( pBits )
		{
			BITMAPFILEHEADER bf;

			int x1 = 0;
			int x2 = x1 + sizeof(bf);
			int x3 = x2 + GetBitmapHeaderSize(pbmi);

			int SizeOfImage = dwWidthBytes*pbmi->bmiHeader.biHeight;
			int SizeOfBuffer = x3 + SizeOfImage; 

			m_BufferSize = SizeOfBuffer;

			if( m_pBuffer )
				delete[] m_pBuffer;

			BYTE* pBuffer = new BYTE[SizeOfBuffer];

			if( pBuffer )
			{
				m_pBuffer = pBuffer;

				bf.bfType		= 0x4d42;
				bf.bfSize		= SizeOfBuffer;
				bf.bfReserved1	= 0;
				bf.bfReserved2	= 0;
				bf.bfOffBits	= x3;

				memcpy( &pBuffer[x1], &bf, sizeof(bf) );
				memcpy( &pBuffer[x2], pbmi, GetBitmapHeaderSize(pbmi) );

				for( int y = 0; y < pbmi->bmiHeader.biHeight; y++ )
					GetDIBits( hdc, hbm, y, 1, &pBuffer[x3+y*dwWidthBytes], pbmi, DIB_RGB_COLORS );
			}
		}
		pbmi = DeleteBitmapHeader( pbmi );
	}

	ReleaseDC( NULL, hdc );

	return bResult;
}

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	BYTE tgaImageIDLength;
	BYTE tgaColorMapType;
	BYTE tgaImageType;
	WORD tgaColorMapBegin;
	WORD tgaColorMapLength;
	BYTE tgaColorMapBPP;
	WORD tgaOriginX;
	WORD tgaOriginY;
	WORD tgaImageWidth;
	WORD tgaImageHeight;
	BYTE tgaImageBPP;
	BYTE tgaDescriptor;
}
TGAINFOHEADER, *LPTGAINFOHEADER;

typedef struct
{
	DWORD tgaExtensionOffset;
	DWORD tgaDeveloperOffset;
	BYTE tgaSignature[18];
}
TGAFILEFOOTER, *LPTGAFILEFOOTER;

#pragma pack(pop)

typedef struct
{
	BYTE	tgaBlue;
	BYTE	tgaGreen;
	BYTE	tgaRed;
}
TGATRIPLE;

typedef struct
{
	BYTE	tgaBlue;
	BYTE	tgaGreen;
	BYTE	tgaRed;
	BYTE	tgaAlpha;
}
TGAQUAD;

BOOL XPRess::WriteTGAToMemory( BITMAP2* pbm )
{
	TGAINFOHEADER header;

	header.tgaImageIDLength = 0;
	header.tgaColorMapType = 0;
	header.tgaImageType = 2;
	header.tgaColorMapBegin = 0;
	header.tgaColorMapLength = 0;
	header.tgaColorMapBPP = 0;
	header.tgaOriginX = 0;
	header.tgaOriginY = 0;
	header.tgaImageWidth = pbm->Width;
	header.tgaImageHeight = pbm->Height;
	header.tgaImageBPP = 32;
	header.tgaDescriptor = 34;	// bit field: 00 (reserved) 00 (origin lower-left) 0100 (8-bit alpha);

	if( header.tgaImageBPP <= 8 )
	{
		header.tgaColorMapType = 1;
		header.tgaImageType = 1;
		header.tgaColorMapBegin = 0;
		header.tgaColorMapLength = 1 << header.tgaImageBPP;
		header.tgaColorMapBPP = 32;
		header.tgaDescriptor = 0;
	}

	DWORD dwHeaderSize = sizeof(header);
	DWORD dwScanlineSize = (header.tgaImageWidth * header.tgaImageBPP + 7) / 8;
	DWORD dwImageSize = dwScanlineSize * header.tgaImageHeight;
	DWORD dwFileSize = dwHeaderSize + dwImageSize;
	DWORD dwFilePos = dwHeaderSize;

	BYTE* pBuffer = new BYTE[dwFileSize];

	if( !pBuffer )
		return FALSE;

	if( m_pBuffer )
		delete[] m_pBuffer;

	m_pBuffer = pBuffer;
	m_BufferSize = dwFileSize;

	memcpy( pBuffer, &header, sizeof(header) );

//	if( !FileWrite( hFile, &header, sizeof( header ) ) )
//		return FALSE;

	BeginBitmap( 0, pbm );

	TGAQUAD *scanline;
	BOOL bResult = FALSE;
	int x, y;

	if( header.tgaImageBPP == 32 )
	{
		COLORREF c;

		for( y = 0; y < header.tgaImageHeight; y++ )
		{
			scanline = (TGAQUAD*) &pBuffer[dwFilePos];

			for( x = 0; x < header.tgaImageWidth; x++ )
			{
				c = GetPixelU( 0, x, y );
				scanline[x].tgaAlpha = (BYTE) (c >> 24);
				scanline[x].tgaRed   = (BYTE) (c >> 16);
				scanline[x].tgaGreen = (BYTE) (c >>  8);
				scanline[x].tgaBlue  = (BYTE) (c >>  0);
			}

			dwFilePos += dwScanlineSize;
		}
	}

	FinishBitmap(0);

	return bResult;
}

//////////////////////////////////////////////////////////
// sets up the texture data into some form that the bundler
// can understand.. a simple BMP seems to be the easist
// solution

void XPRess::ProcessTextures()
{
	DibbleFile* pDibble = dm.FindDibble(XPRessObj.m_DibbleName);

	if( pDibble )
		m_NumTextures = pDibble->m_TextureCount;

	return;
}


