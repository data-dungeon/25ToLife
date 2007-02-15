/***********************************/
/*                                 */
/* Graphic.h                       */
/* Pigpen particle tool  12/17/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle graphic class          */
/*                                 */
/***********************************/

#ifndef __GRAPHIC_H
#define __GRAPHIC_H

/******************* includes ****************************/

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in graphic serialization */

#define GRAPHIC_VERSION		0

/* flags */

#define GRAPHIC_ASSOCIATED					0x00000001
#define GRAPHIC_SELECTED					0x00000002	// for serialization
#define GRAPHIC_MAINTAIN_ASPECT_RATIO	0x00000004
#define GRAPHIC_SHOW_ALPHA_MASK			0x00000008
#define GRAPHIC_NEW_ASSOCIATE				0x00000010
/*** always add to end of list-- don't reorder-- flags are serialized ***/

/* default animation speed */

#define DEFAULT_FPS	30

/* indicate that no icon has been chosen for graphic */

#define NO_ICON		-1

/******************* forward declarations ****************/

class CImage;

/******************* structures **************************/

/******************* class *******************************/

class CGraphic : public CObject
{
public:

	/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the */
	/* array records serialize themselves, in order-- slick */

	DECLARE_SERIAL(CGraphic)

// Constructor/destructor

	CGraphic();
	~CGraphic();

// Implementation

protected:

	/* keep serialization up-to-date! */

	CString m_Name;
	CString m_BaseSourceFile;			// originally selected image file name
	unsigned long m_ulVersion;
	unsigned long m_ulFlags;
	CObArray m_Images;
	int m_nHeight;
	int m_nWidth;
	int m_nIconIndex;						// index of image that is best representation of graphic
	float m_fDefaultFPS;					// default animation speed
	float m_fDefaultXSize;				// default x-size
	float m_fDefaultYSize;				// default y-size

	/* not serialized */

	int m_nGraphicDialogIndex;			// for image display in GraphicDialog
	int m_nTempIndex;						// for export
	int m_nExportStatus;					// used for export
	int m_nExportDataSize;				// used for preview
	int m_nUsedCount;						//	for use analysis
	int m_nTextureSetSize;				// for ps2 texture size analysis
	unsigned long m_ulLoadVersion;	// version number from serialization load, for possible fix-ups
	
	/* keep serialization up-to-date! */

public:

	/* simple routines */

	CString& GetName(void)								{ return(m_Name); }
	void SetName(char *pName)							{ m_Name = pName; }
	void SetName(CString& Name)						{ m_Name = Name; }
	int GetImageCount(void)								{ return(m_Images.GetSize()); }
	CImage *GetImage(int nIndex)						{ return((CImage *) m_Images.GetAt(nIndex)); }
	CString& GetBaseSourceFile(void)					{ return(m_BaseSourceFile); }
	int GetGraphicDialogIndex(void)					{ return(m_nGraphicDialogIndex); }
	void SetGraphicDialogIndex(int nIndex)			{ m_nGraphicDialogIndex = nIndex; }
	int GetIconIndex(void)								{ return(m_nIconIndex); }
	void SetIconIndex(int nIndex)						{ m_nIconIndex = nIndex; }
	CImage *GetIconImage(void)							{ if (m_nIconIndex == -1) return(NULL); else return(GetImage(m_nIconIndex)); }
	int GetHeight(void)									{ return(m_nHeight); }
	int GetWidth(void)									{ return(m_nWidth); }
	bool IsMaintainAspectRatio(void)					{ return((m_ulFlags & GRAPHIC_MAINTAIN_ASPECT_RATIO) != 0); }
	bool IsShowAlphaMask(void)							{ return((m_ulFlags & GRAPHIC_SHOW_ALPHA_MASK) != 0); }
	float GetDefaultFPS(void)							{ return(m_fDefaultFPS); }
	float GetDefaultXSize(void)						{ return(m_fDefaultXSize); }
	float GetDefaultYSize(void)						{ return(m_fDefaultYSize); }
	void SetTempIndex(int nIndex)						{ m_nTempIndex = nIndex; }
	int GetTempIndex(void)								{ return(m_nTempIndex); }
	void SetExportStatus(int nStatus)				{ m_nExportStatus = nStatus; }
	int GetExportStatus(void)							{ return(m_nExportStatus); }
	void SetExportDataSize(int nExportDataSize)	{ m_nExportDataSize = nExportDataSize; }
	int GetExportDataSize(void)						{ return(m_nExportDataSize); }
	void SetSelected(bool bSelected)					{ if (bSelected) m_ulFlags |= GRAPHIC_SELECTED; else m_ulFlags &= ~GRAPHIC_SELECTED; }
	bool GetSelected(void)								{ return((m_ulFlags & GRAPHIC_SELECTED) != 0); }
	void SetUsedCount(int nCount)						{ m_nUsedCount = nCount; }
	int GetUsedCount(void)								{ return(m_nUsedCount); }
	void IncUsedCount(void)								{ m_nUsedCount++; }
	void SetTextureSetSize(int nSize)				{ m_nTextureSetSize = nSize; }
	int GetTextureSetSize(void)						{ return(m_nTextureSetSize); }
	void IncTextureSetSize(int nSize)				{ m_nTextureSetSize += nSize; }
	unsigned long GetLoadVersion(void)				{ return(m_ulLoadVersion); }

	/* routines defined in .cpp */

	bool ImportImage(bool bReplace, bool bUpdate);
	void RemoveImage(int nIndex);
	int FillExportBuffer(char *pBuffer, int nMaxSize);
	int AnalyzeForExport(void);
	bool AlphaMaskOnly(void);
	bool AllImagesFeatheredAlpha(void);
	bool AllImagesSingleBitAlpha(void);
	void LoadFixup(void);

	/* association testing */

	bool IsAssociated()									{ return((m_ulFlags & GRAPHIC_ASSOCIATED) != 0); }
	bool IsNewAssociate()								{ return((m_ulFlags & GRAPHIC_NEW_ASSOCIATE) != 0); }
	void ClearAssociateFlags()							{ m_ulFlags &= ~(GRAPHIC_ASSOCIATED | GRAPHIC_NEW_ASSOCIATE); }
	void ClearNewAssociateFlag()						{ m_ulFlags &= ~GRAPHIC_NEW_ASSOCIATE; }
	void SetAssociateFlags()							{ m_ulFlags |= (GRAPHIC_ASSOCIATED | GRAPHIC_NEW_ASSOCIATE); }
	int TestAssociate(CObject *pTestObject, int nType);

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

	/* for more generic dialog processing, give access to addresses of data fields. data field id's are used */
	/* by dialog control lists */

	enum {	// because the enum is inside the class, it needs to be referenced as CGraphic::DATA_XXX
		DATA_FPS = 0,
		DATA_XSIZE,
		DATA_YSIZE,
		DATA_FLAGS,

		// update GetDataPointer, below, as you add more
	};

	void *GetDataPointer(int nDataID)
	{
		switch(nDataID)
		{
		case DATA_FPS:
			return(&m_fDefaultFPS);
		case DATA_XSIZE:
			return(&m_fDefaultXSize);
		case DATA_YSIZE:
			return(&m_fDefaultYSize);
		case DATA_FLAGS:
			return(&m_ulFlags);
		}
		ASSERT(FALSE);
		return(NULL);
	}

// operations

public:

};

#endif // __GRAPHIC_H
