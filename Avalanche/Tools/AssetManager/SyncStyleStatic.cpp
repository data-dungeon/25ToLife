// SyncStyleStatic.cpp : implementation file
//

#include "stdafx.h"
#include "assetmanager.h"
#include "SyncStyleStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SyncStyleStatic

SyncStyleStatic::SyncStyleStatic()
{
	m_syncStyle = -1;
}

SyncStyleStatic::~SyncStyleStatic()
{
}


BEGIN_MESSAGE_MAP(SyncStyleStatic, CStatic)
	//{{AFX_MSG_MAP(SyncStyleStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SyncStyleStatic message handlers


static int leftSyncStyleBitmaps[SyncTreeManager::SYNC_STYLE_NUM_STYLES] = 
{
	IDR_SYNC_STYLE_PATTERN01, // SYNC_STYLE_2_WAY
	IDR_SYNC_STYLE_PATTERN02, // SYNC_STYLE_AUGMENT_LEFT
	IDR_SYNC_STYLE_PATTERN03, // SYNC_STYLE_AUGMENT_RIGHT
	IDR_SYNC_STYLE_PATTERN04, // SYNC_STYLE_REFRESH_LEFT
	IDR_SYNC_STYLE_PATTERN05, // SYNC_STYLE_REFRESH_RIGHT
	IDR_SYNC_STYLE_PATTERN06, // SYNC_STYLE_UPDATE_LEFT
	IDR_SYNC_STYLE_PATTERN07, // SYNC_STYLE_UPDATE_RIGHT
	IDR_SYNC_STYLE_PATTERN08, // SYNC_STYLE_UPDATE_AND_PRUNE_LEFT
	IDR_SYNC_STYLE_PATTERN09, // SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT
	IDR_SYNC_STYLE_PATTERN10, // SYNC_STYLE_FORCE_LEFT
	IDR_SYNC_STYLE_PATTERN11, // SYNC_STYLE_FORCE_RIGHT
};

static int tileSyncStyleBitmaps[SyncTreeManager::SYNC_STYLE_NUM_STYLES] = 
{
	IDR_SYNC_STYLE_PATTERN01T, // SYNC_STYLE_2_WAY
	IDR_SYNC_STYLE_PATTERN02T, // SYNC_STYLE_AUGMENT_LEFT
	IDR_SYNC_STYLE_PATTERN03T, // SYNC_STYLE_AUGMENT_RIGHT
	IDR_SYNC_STYLE_PATTERN04T, // SYNC_STYLE_REFRESH_LEFT
	IDR_SYNC_STYLE_PATTERN05T, // SYNC_STYLE_REFRESH_RIGHT
	IDR_SYNC_STYLE_PATTERN06T, // SYNC_STYLE_UPDATE_LEFT
	IDR_SYNC_STYLE_PATTERN07T, // SYNC_STYLE_UPDATE_RIGHT
	IDR_SYNC_STYLE_PATTERN08T, // SYNC_STYLE_UPDATE_AND_PRUNE_LEFT
	IDR_SYNC_STYLE_PATTERN09T, // SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT
	IDR_SYNC_STYLE_PATTERN10T, // SYNC_STYLE_FORCE_LEFT
	IDR_SYNC_STYLE_PATTERN11T, // SYNC_STYLE_FORCE_RIGHT
};

void SyncStyleStatic::SetSyncStyle(int syncStyle)
{
	if (syncStyle != m_syncStyle)
	{
		m_syncStyle = syncStyle;
		Invalidate();
	}
}

HBITMAP SyncStyleStatic::CreateAlphaMixedBitmap(HDC hDC,int resourceID,COLORREF bkColor,BITMAPINFO *pBmi)
{
	PLAnyPicDecoder decoder;
	PLWinBmp image;
	decoder.MakeBmpFromResource(AfxGetInstanceHandle(),resourceID,&image,32);
	
	BYTE bkRed = GetRValue(bkColor);
	BYTE bkGreen = GetGValue(bkColor);
	BYTE bkBlue = GetBValue(bkColor);
	
	int width = image.GetWidth();
	int height = image.GetHeight();
	int size = width*height*4;
	
	int x,y;
	BYTE ** pLineArray = image.GetLineArray();
	BYTE *pData = new BYTE[size];
	BYTE *pDest = pData;
	BYTE *pLine;
	
	for (y=0; y<height; y++)
	{
		pLine = pLineArray[y];
		
		// Iterate through the pixels
		for (x=0; x<width; x++)
		{
			float alphaFactor = ((float)pLine[PL_RGBA_ALPHA])/((float)0xff);
			pDest[PL_RGBA_RED] = (BYTE)((pLine[PL_RGBA_RED] * alphaFactor)+(bkRed*(1.0f-alphaFactor)));
			pDest[PL_RGBA_GREEN] = (BYTE)((pLine[PL_RGBA_GREEN] * alphaFactor)+(bkGreen*(1.0f-alphaFactor)));
			pDest[PL_RGBA_BLUE] = (BYTE)((pLine[PL_RGBA_BLUE] * alphaFactor)+(bkBlue*(1.0f-alphaFactor)));
			pDest[PL_RGBA_ALPHA] = 0;
			pDest+=4;
			pLine+=4;
		}
	}
	
	// zero the memory for the bitmap info
	ZeroMemory(pBmi, sizeof(BITMAPINFO));
	
	// setup bitmap info 
	// set the bitmap width and height to 60% of the width and height of each of the three horizontal areas. Later on, the blending will occur in the center of each of the three areas.
	pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBmi->bmiHeader.biWidth = width;
	pBmi->bmiHeader.biHeight = height;
	pBmi->bmiHeader.biPlanes = 1;
	pBmi->bmiHeader.biBitCount = 32;         // four 8-bit components
	pBmi->bmiHeader.biCompression = BI_RGB;
	pBmi->bmiHeader.biSizeImage = 0;
	
	HBITMAP hBitmap = ::CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)pBmi, (LONG)CBM_INIT, pData, (LPBITMAPINFO)pBmi,  DIB_RGB_COLORS ); 
	
	delete pData;
	
	return hBitmap;
}


void SyncStyleStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (m_syncStyle==-1)
		return;

	CRect myRect;
	GetClientRect(&myRect);

	dc.IntersectClipRect( &myRect );

	COLORREF bkColor = GetSysColor(COLOR_3DFACE);

	BITMAPINFO leftBmi;  // bitmap header
	HBITMAP hLeftBitmap = CreateAlphaMixedBitmap(dc,leftSyncStyleBitmaps[m_syncStyle],bkColor,&leftBmi);
	
	CDC leftBitmapDC;
	leftBitmapDC.CreateCompatibleDC(&dc);
	HGDIOBJ hOldLeftObj = leftBitmapDC.SelectObject(hLeftBitmap);

	BITMAPINFO tileBmi;  // bitmap header
	HBITMAP hTileBitmap = CreateAlphaMixedBitmap(dc,tileSyncStyleBitmaps[m_syncStyle],bkColor,&tileBmi);
	
	CDC tileBitmapDC;
	tileBitmapDC.CreateCompatibleDC(&dc);
	HGDIOBJ hOldTileObj = tileBitmapDC.SelectObject(hTileBitmap);

	int x,y;
	for (y=myRect.top;y<=myRect.bottom;y+=leftBmi.bmiHeader.biWidth)
	{
		::BitBlt(dc,myRect.left,y,leftBmi.bmiHeader.biWidth,leftBmi.bmiHeader.biHeight,
									leftBitmapDC, 0, 0, SRCCOPY);
	}
	for (x=myRect.left+leftBmi.bmiHeader.biWidth;x<=myRect.right;x+=tileBmi.bmiHeader.biWidth)
	{
		for (y=myRect.top;y<=myRect.bottom;y+=tileBmi.bmiHeader.biHeight)
		{
			::BitBlt(dc,x,y,tileBmi.bmiHeader.biWidth,tileBmi.bmiHeader.biHeight,
									tileBitmapDC, 0, 0, SRCCOPY);
		}
	}

	leftBitmapDC.SelectObject(hOldLeftObj);
	tileBitmapDC.SelectObject(hOldTileObj);

	DeleteObject(hLeftBitmap);
	DeleteObject(hTileBitmap);
}
