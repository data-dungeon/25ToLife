/***********************************/
/*                                 */
/* Image.h                         */
/* Pigpen particle tool  12/17/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle image class            */
/*                                 */
/***********************************/

#ifndef __IMAGE_H
#define __IMAGE_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include <Platform/Basetype.h>

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in image serialization */

#define IMAGE_VERSION		0

/* flags */

/* supported image file types */

#define NO_TYPE	0
#define PCX_TYPE	1
#define TGA_TYPE	2
#define BMP_TYPE	3

/* read image failure codes */

#define READ_IMAGE_SUCCESS				0
#define READ_IMAGE_OPEN_ERROR			1
#define READ_IMAGE_MALLOC_ERROR		2
#define READ_IMAGE_READ_ERROR			3
#define READ_IMAGE_PROCESS_ERROR		4	// couldn't process image data
#define READ_IMAGE_SIZE_ERROR			5	// non-power-of-2 image size
#define READ_IMAGE_MATCH_SIZE_ERROR	6	// didn't match size of graphic's existing images

/******************* structures **************************/

/******************* class *******************************/

class CImage : public CObject
{
public:

	/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the */
	/* array records serialize themselves, in order-- slick */

	DECLARE_SERIAL(CImage)

// Constructor/destructor

	CImage();
	~CImage();

// Implementation

protected:

	/* keep serialization up-to-date! */

	CString m_SourceFile;				// originally selected image file name
	unsigned long m_ulVersion;
	unsigned long m_ulFlags;
	u8 *m_pBits;							// image data
	BITMAPINFO *m_pBMI;					// other image info, including palette
	int m_nType;							// image type, based on bits-per-pixel in BMI

	/* not serialized */
	
	HBITMAP m_hBitmap;					// handle to device-independent bitmap for display
	HBITMAP m_hAlphaBitmap;				// handle to device-independent bitmap for display (alpha mask)
	int m_nTempIndex;						// for image-import sorting and export purposes
	unsigned long m_ulLoadVersion;	// version number from serialization load, for possible fix-ups

	/* keep serialization up-to-date! */

public:

	/* simple routines */

	CString& GetSourceFile(void) { return(m_SourceFile); }
	int GetTempIndex(void) { ASSERT(m_nTempIndex != -1); return(m_nTempIndex); }
	void SetTempIndex(int nIndex) { m_nTempIndex = nIndex; }
	int GetHeight(void) { if (m_pBMI) return(m_pBMI->bmiHeader.biHeight); else return(0); }
	int GetWidth(void) { if (m_pBMI) return(m_pBMI->bmiHeader.biWidth); else return(0); }
	int GetImageSize(void) { if (m_pBits == NULL) return(0); else return(GetRowSize() * GetHeight()); }
	u8 *GetImageBits(void) { return(m_pBits); }
	void SetImageBits(u8 *pBits) { m_pBits = pBits; }
	BITMAPINFO *GetImageBMI(void) { return(m_pBMI); }
	int GetImageType(void) { return(m_nType); }
	int GetImageBitCount(void) { return(m_pBMI->bmiHeader.biBitCount); }
	unsigned long GetLoadVersion(void) { return(m_ulLoadVersion); }

	/* routines defined in .cpp */

	bool SetSourceFileAndIndex(CString& File, bool bNoIndex);
	int GetRowSize(void);
	int GetBMISize(void);
	char *GetImageTypeString(void);
	int ReadImageFile(int nHeight, int nWidth);
	void Paint(HDC hDC, RECT *pRect, int nBorderPixels, bool bShowAlphaMask);
	bool IsFeatheredAlpha(void);
	bool IsSingleBitAlpha(void);
	void LoadFixup(void);

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

// operations

public:

};

#endif // __IMAGE_H
