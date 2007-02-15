/***********************************/
/*                                 */
/* Image.cpp                       */
/* Pigpen particle tool  12/17/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle image class            */
/*                                 */
/***********************************/

/* the images that make up a graphic set */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



/******************* defines *****************************/

/******************* structures **************************/

/******************* local variables *********************/

/******************* externs *****************************/

/******************* class functions *********************/

/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the
image records serialize themselves, in order-- slick */

IMPLEMENT_SERIAL(CImage, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CImage construction

CImage::CImage()
{
	m_ulVersion = IMAGE_VERSION;
	m_ulFlags = 0;
	m_SourceFile = "";

	/* image data */

	m_pBits = NULL;
	m_pBMI = NULL;
	m_hBitmap = NULL;
	m_hAlphaBitmap = NULL;
	m_nType = SOURCE_TYPE_UNKNOWN;
}
CImage::~CImage()
{
	if (m_pBits)
		free(m_pBits);
	if (m_pBMI)
		free(m_pBMI);
	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	if (m_hAlphaBitmap)
		DeleteObject(m_hAlphaBitmap);
}

/////////////////////////////////////////////////////////////////////////////
// CImage serialization

void CImage::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == IMAGE_VERSION);
		ar << m_ulVersion;
	
		/* version 0 */

		ar << m_ulFlags;
		ar << m_SourceFile;
		ar << m_nType;

		/* image bits */

		if (m_pBits == NULL)
			ar << (int) 0;
		else
		{
			int nBytes = GetImageSize();
			ar << nBytes;
			ar.Write(m_pBits, nBytes);
		}

		/* BMI */

		if (m_pBMI == NULL)
			ar << (int) 0;
		else
		{
			int nBytes = GetBMISize();
			ar << nBytes;
			ar.Write(m_pBMI, nBytes);
		}
	}
	else
	{
		ar >> m_ulVersion;
		if (m_ulVersion > IMAGE_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading images.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* as we add new versions, process latest version and then fall through to earlier versions */

		switch(m_ulVersion)
		{
		case 1:
			/* fall through */
		case 0:
			/* version 0 */

			ar >> m_ulFlags;
			ar >> m_SourceFile;
			ar >> m_nType;

			/* image bits */

			int nBytes;
			ASSERT(m_pBits == NULL);	// shouldn't be overwriting an existing image
			ar >> nBytes;
			if (nBytes != 0)
			{
				m_pBits = (u8 *) malloc(nBytes);
				ASSERT(m_pBits);
				ar.Read(m_pBits, nBytes);
			}

			/* BMI */

			ASSERT(m_pBMI == NULL);	// shouldn't be overwriting an existing image
			ar >> nBytes;
			if (nBytes != 0)
			{
				/* always malloc max size so i don't have to track how big each buffer is */

				m_pBMI = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
				ASSERT(m_pBMI);
				ar.Read(m_pBMI, nBytes);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* now using latest version */

		m_ulLoadVersion = m_ulVersion;	// save version from load, for possible fix-ups
		m_ulVersion = IMAGE_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImage implementation

char *CImage::GetImageTypeString(void)
{
	return(GetSourceImageType(m_nType));
}

/* set source file name and convert numbers at end of file to index for sorting purposes. error if no index */

bool CImage::SetSourceFileAndIndex(CString& File, bool bNoIndex)
{
	m_SourceFile = File;

	/* maybe no index */

	if (bNoIndex)
	{
		m_nTempIndex = 0;
		return(TRUE);
	}

	/* strip off extension */

	int nIndex = File.ReverseFind('.');
	ASSERT(nIndex != -1);	// we insist on an extension further upstream
	File.Delete(nIndex, File.GetLength() - nIndex);

	/* get index, record for sorting purposes in CGraphic::ImportImages */

	int nLength = 0;
	while ((File.GetAt(File.GetLength() - 1 - nLength) >= '0') && (File.GetAt(File.GetLength() - 1 - nLength) <= '9'))
		nLength++;
	if (nLength == 0)
		return(FALSE);
	CString Index = File.Right(nLength);
	m_nTempIndex = atoi(Index);

	/* sigh. if atoi fails it returns 0, but it also returns 0 for "0", obviously */

	if ((m_nTempIndex == 0) && (AllZeros(Index) == FALSE))
		return(FALSE);
	ASSERT(m_nTempIndex >= 0);	// duh, since we would ignore negative sign
	return(TRUE);
}

/* load in an image file. if nHeight and nWidth are non-zero, incoming image size must match. use existing image
reading in multigen plug-ins */

int CImage::ReadImageFile(int nHeight, int nWidth)
{
	/* open file */

	HANDLE hFile = CreateFile(m_SourceFile,
							 GENERIC_READ,
							 FILE_SHARE_READ,
							 NULL,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL);

	/* read it in */

	int nResult = ReadImage(hFile, &m_pBMI, &m_pBits);
	m_nType = nSourceImageType;
	CloseHandle(hFile);
	if( nResult == FALSE)
		return(READ_IMAGE_READ_ERROR);

	/* size check */

	if ((IsPowerOfTwo(m_pBMI->bmiHeader.biHeight) == FALSE) || (IsPowerOfTwo(m_pBMI->bmiHeader.biWidth) == FALSE))
		return(READ_IMAGE_SIZE_ERROR);
	if ((nHeight != 0) && ((m_pBMI->bmiHeader.biHeight != nHeight) || (m_pBMI->bmiHeader.biWidth != nWidth)))
		return(READ_IMAGE_MATCH_SIZE_ERROR);

	/* we are good to go */

	return(READ_IMAGE_SUCCESS);
}

/* get size in bytes of one row of image. windows insists on 4-byte alignment */

int CImage::GetRowSize(void)
{
	/* we know that dimensions are power of 2, for what it's worth */

	int nRowSize = GetWidth();
	if (nRowSize == 0)
		return(0);
	ASSERT(m_pBMI);	// if we have an image, we have to have a BMI too */
	switch(m_pBMI->bmiHeader.biBitCount)
	{
	case 4:
		nRowSize /= 2;
		if (nRowSize == 0)	// 1 pixel wide? ever happen?
			nRowSize = 1;
		break;
	case 8:
		break;
	case 24:
		nRowSize *= 3;
		break;
	case 32:
		nRowSize *= 4;
		break;
	default:
		ASSERT(FALSE);
		return(0);
	}

	/* pad to 4-byte-aligned (windows requires it) */

	return((nRowSize + 3) & ~3);
}

/* get size in bytes of BMI (bit map info), including palette if any */

int CImage::GetBMISize(void)
{
	if (m_pBMI == NULL)
		return(0);
	return(sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));	// always allow for 256 colors in palette
}

/* blit the bitmap onto the screen. nBorderPixels allows you to shrink blit rectangle to leave a border around
image */

void CImage::Paint(HDC hDC, RECT *pRect, int nBorderPixels, bool bShowAlphaMask)
{
	/* ok, this is ugly. mfc and CBitmap provides a lot of this functionality, but apparently not enough */
	/* to do what i want. so back to the GDI, which all deals with handles, etc. fortunately, GetSafeHwnd */
	/* let's me convert from CWnd to window handles... anyway, to display a bitmap, you create a BITMAP handle */
	/* via CreateDIBitmap. once the bitmap is created, it can be blitted to the screen by creating a */
	/* "compatible" display context, selecting the bitmap, and blitting */

	/* create device-independent bitmap if not created yet */

	ASSERT(m_pBMI);
	ASSERT(m_pBits);
	if (m_hBitmap == NULL)
	{
		m_hBitmap = CreateDIBitmap(
			hDC,							// HDC hdc,									// handle to device context
			&m_pBMI->bmiHeader,		// CONST BITMAPINFOHEADER *lpbmih,  // pointer to bitmap size and format data
			CBM_INIT,					// DWORD fdwInit,							// initialization flag
			(LPBYTE) m_pBits,			// CONST VOID *lpbInit,					// pointer to initialization data
			(LPBITMAPINFO) m_pBMI,	// CONST BITMAPINFO *lpbmi,			// pointer to bitmap color-format data
			DIB_RGB_COLORS);			// UINT fuUsage							// color-data usage
		ASSERT(m_hBitmap);
	}
	if (m_hAlphaBitmap == NULL)
	{
		/* have to create a bitmap that represents the alpha mask */

		void *pAlphaImage = malloc(m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biHeight * 4);	// 32-bit image
		int nSaveBitCount = m_pBMI->bmiHeader.biBitCount;
		if (nSaveBitCount < 32)
		{
			/* all-white image */

			memset(pAlphaImage, 0xff, m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biHeight * 4);
			m_pBMI->bmiHeader.biBitCount = 32;
		}
		else
		{
			/* make a gray scale image */

			memcpy(pAlphaImage, m_pBits, m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biHeight * 4);
			ts_bRGBA *pPixel = (ts_bRGBA *) pAlphaImage;
			for (int i = 0; i < m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biHeight; i++, pPixel++)
			{
				pPixel->R = pPixel->A;
				pPixel->G = pPixel->A;
				pPixel->B = pPixel->A;
			}
		}
		m_hAlphaBitmap = CreateDIBitmap(
			hDC,							// HDC hdc,									// handle to device context
			&m_pBMI->bmiHeader,		// CONST BITMAPINFOHEADER *lpbmih,  // pointer to bitmap size and format data
			CBM_INIT,					// DWORD fdwInit,							// initialization flag
			(LPBYTE) pAlphaImage,	// CONST VOID *lpbInit,					// pointer to initialization data
			(LPBITMAPINFO) m_pBMI,	// CONST BITMAPINFO *lpbmi,			// pointer to bitmap color-format data
			DIB_RGB_COLORS);			// UINT fuUsage							// color-data usage
		ASSERT(m_hAlphaBitmap);

		/* clean up */

		free(pAlphaImage);
		m_pBMI->bmiHeader.biBitCount = nSaveBitCount;
	}

	/* we will stretch to fit rect, but preserve aspect ratio */

	int nRectWidth = pRect->right - pRect->left - (nBorderPixels * 2);
	int nRectHeight = pRect->bottom - pRect->top - (nBorderPixels * 2);
	float fWidthRatio = (float) nRectWidth / (float) m_pBMI->bmiHeader.biWidth;
	float fHeightRatio = (float) nRectHeight / (float) m_pBMI->bmiHeader.biHeight;
	float fRatio = min(fWidthRatio, fHeightRatio);
	int nStretchHeight = (int) ((float) m_pBMI->bmiHeader.biHeight * fRatio);
	int nStretchWidth = (int) ((float) m_pBMI->bmiHeader.biWidth * fRatio);
	ASSERT((nStretchHeight == nRectHeight) || (nStretchWidth == nRectWidth));

	/* blit from bitmap to window */

	HDC hBitmapDC = CreateCompatibleDC(hDC);
	if (bShowAlphaMask)
		SelectObject(hBitmapDC, m_hAlphaBitmap);
	else
		SelectObject(hBitmapDC, m_hBitmap);
	StretchBlt(
		hDC,									// HDC hdcDest,		// handle to destination device context
		nBorderPixels,						// int nXOriginDest,	// x-coordinate of destination rectangle's upper-left corner
		nBorderPixels,						// int nYOriginDest,	// y-coordinate of destination rectangle's upper-left corner
		nStretchWidth,						// int nWidthDest,	// width of destination rectangle
		nStretchHeight,					// int nHeightDest,	// height of destination rectangle
		hBitmapDC,							// HDC hdcSrc,			// handle to source device context
		0,										// int nXOriginSrc,	// x-coordinate of source rectangle's upper-left corner
		0,										// int nYOriginSrc,  // y-coordinate of source rectangle's upper-left corner
		m_pBMI->bmiHeader.biWidth,		// int nWidthSrc,    // width of source rectangle
		m_pBMI->bmiHeader.biHeight,	// int nHeightSrc,   // height of source rectangle
		SRCCOPY);							// DWORD dwRop			// raster operation code
	DeleteDC(hBitmapDC);
}

/* return true if image contains feathered alpha */

bool CImage::IsFeatheredAlpha(void)
{
	if (m_pBMI->bmiHeader.biBitCount < 32)
		return(FALSE);

	/* look through alpha mask */

	ts_bRGBA *pPixel = (ts_bRGBA *) m_pBits;
	for (int i = 0; i < m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biHeight; i++, pPixel++)
	{
		if ((pPixel->A != 0) && (pPixel->A != 0xff))
			return(TRUE);	// feathered
	}
	return(FALSE);
}

/* return true if image contains single-bit alpha */

bool CImage::IsSingleBitAlpha(void)
{
	if (m_pBMI->bmiHeader.biBitCount < 32)
		return(FALSE);

	/* look through alpha mask */

	ts_bRGBA *pPixel = (ts_bRGBA *) m_pBits;
	for (int i = 0; i < m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biHeight; i++, pPixel++)
	{
		if ((pPixel->A != 0) && (pPixel->A != 0xff))
			return(FALSE);	// feathered, not single bit
	}
	return(TRUE);
}

/* do load-time fix-up on old versions of data. all pointers, etc have been resolved */

void CImage::LoadFixup(void)
{
	/* currently no fix-up required for image */
}

