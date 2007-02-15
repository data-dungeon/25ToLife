// Dan  3/24/2003  \DanLP105\Plugin\HXT\Ply\Hxt.cpp
//

#include "stdafx.h"
#include "BasicImagePlugin.h"
#include "HBitmapObject.h"
#include "HBitmapBuffer.h"

#include "PLAnyDec.h"
#include "..\win\paintlib\plwinbmp.h"

/////////////////////////////////////////////////////////////////////////////
// CMtrApp

BEGIN_MESSAGE_MAP(CItrApp, CWinApp)
	//{{AFX_MSG_MAP(CMtrApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItrApp construction

CItrApp::CItrApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHxtApp object

CItrApp g_theApp;

BOOL CItrApp::InitInstance() 
{
   SetRegistryKey( "Avalanche Software" );

   BuildGrayScalePalette();
	return CWinApp::InitInstance();
}

void CItrApp::BuildGrayScalePalette()
{
   for (int i=0; i<256; i++)
      m_grayscalepalette[i].Set(i, i, i, 255);
}

BOOL CItrApp::ImportImage(HBitmapObject *pBitmapObject, const char *szFileName)
{
   PLAnyPicDecoder decoder;

   PLWinBmp image;

	try {
		decoder.MakeBmpFromFile(szFileName, &image, 0); // read in native format
	}
	catch (PLTextException e) {
      String msg = (LPCTSTR)e;
      msg.Replace("\n", ".");
      THROW_FATAL_ERROR(msg);
   }

   int nWidth = image.GetWidth();
   int nHeight = image.GetHeight();
   BOOL bAlpha = image.HasAlpha();
   int nNumColors = image.GetNumColors();
   int nBPP = image.GetBitsPerPixel();

   pBitmapObject->Init( nWidth, nHeight );
   HBitmapBuffer *pBuffer;

   if (nBPP == 32){ // 32 bit
      if (bAlpha) {
         pBuffer = pBitmapObject->CreateImageBuffer(BDT_BYTE, BC_BGRA );
         BYTE **pLineArray = image.GetLineArray();
         for (int i=0; i < nHeight; i++)
            pBuffer->SetRow(i, pLineArray[i]);
      }
      else {//24 bit - The alpha is just 255
         pBuffer = pBitmapObject->CreateImageBuffer(BDT_BYTE, BC_BGR );
         BYTE **pLineArray = image.GetLineArray();
         for (int i=0; i < nHeight; i++) {
            RGBAByte *pSrcRow = (RGBAByte *)pLineArray[i];
            RGBByte *pDstRow = (RGBByte *)pBuffer->GetRow(i);
            for (int x=0; x<nWidth; x++)
               pDstRow[x].Set( pSrcRow[x].GetRed(), pSrcRow[x].GetGreen(), pSrcRow[x].GetBlue() ); 
         }
      }
   }
   if (nBPP == 8) { // Lets test for gray scale and keep it 8 bit otherwise lets convert to 24 bit
      if (memcmp(image.GetPalette(), &m_grayscalepalette, 256)==0) { // Gray Scale
         pBuffer = pBitmapObject->CreateImageBuffer(BDT_BYTE, BC_LUMINANCE );
         BYTE **pLineArray = image.GetLineArray();
         for (int i=0; i < nHeight; i++)
            pBuffer->SetRow(i, pLineArray[i]);
      }
      else { // 8 bit with palette, lets convert to 24 bit
         pBuffer = pBitmapObject->CreateImageBuffer(BDT_BYTE, BC_BGR );
         BYTE **pLineArray = image.GetLineArray();
         PLPixel32 *pPalette = image.GetPalette();
         for (int i=0; i < nHeight; i++) {
            BYTE *pSrcRow = pLineArray[i];
            RGBByte *pDstRow = (RGBByte *)pBuffer->GetRow(i);
            for (int x=0; x<nWidth; x++) {
               PLPixel32 pixel = pPalette[pSrcRow[x]];
               pDstRow[x].Set( pixel.GetR(), pixel.GetG(), pixel.GetB() );
            }
         }
      }
   }
   return TRUE;
}


