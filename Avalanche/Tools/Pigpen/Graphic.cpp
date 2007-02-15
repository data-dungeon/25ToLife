/***********************************/
/*                                 */
/* Graphic.cpp                     */
/* Pigpen particle tool  12/17/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle graphic class          */
/*                                 */
/***********************************/

/* a particle graphic is a set of one or more same-sized images */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



/******************* defines *****************************/

#define PS2_ALIGN_BYTES 4

/******************* structures **************************/

/******************* local variables *********************/

/******************* externs *****************************/

/******************* class functions *********************/

/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the
graphic records serialize themselves, in order-- slick */

IMPLEMENT_SERIAL(CGraphic, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CGraphic construction

CGraphic::CGraphic()
{
	m_ulVersion = GRAPHIC_VERSION;
	m_ulFlags = GRAPHIC_MAINTAIN_ASPECT_RATIO;

	m_Name = "New";
	m_BaseSourceFile = "None";
	m_Images.SetSize(0, 20);
	m_nGraphicDialogIndex = 0;
	m_nHeight = 0;
	m_nWidth = 0;
	m_fDefaultFPS = DEFAULT_FPS;
	m_fDefaultXSize = 1.0f;
	m_fDefaultYSize = 1.0f;
	m_nIconIndex = NO_ICON;
	m_nUsedCount = -1;	// unknown status until analysis is done
	m_nTextureSetSize = -1;	// unknown status until analysis is done
	m_nExportDataSize = 0;
}
CGraphic::~CGraphic()
{
	/* clean up images */

	DeleteArray(m_Images);
}

/////////////////////////////////////////////////////////////////////////////
// CGraphic serialization

void CGraphic::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == GRAPHIC_VERSION);
		ar << m_ulVersion;
	
		/* version 0 */

		ar << m_ulFlags;
		ar << m_Name;
		ar << m_BaseSourceFile;
		ar << m_nHeight;
		ar << m_nWidth;
		ar << m_fDefaultFPS;
		ar << m_fDefaultXSize;
		ar << m_fDefaultYSize;
		ar << m_nIconIndex;

		/* images */

		m_Images.Serialize(ar);
	}
	else
	{
		ar >> m_ulVersion;
		if (m_ulVersion > GRAPHIC_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading graphics.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
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
			ar >> m_Name;
			ar >> m_BaseSourceFile;
			ar >> m_nHeight;
			ar >> m_nWidth;
			ar >> m_fDefaultFPS;
			ar >> m_fDefaultXSize;
			ar >> m_fDefaultYSize;
			ar >> m_nIconIndex;

			/* images */

			m_Images.Serialize(ar);
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* clear associated flag on load-- it's temporary */

		m_ulFlags &= ~GRAPHIC_ASSOCIATED;

		/* now using latest version */

		m_ulLoadVersion = m_ulVersion;	// save version from load, for possible fix-ups
		m_ulVersion = GRAPHIC_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGraphic implementation

/* test if data object is associated with passed data object, for association testing. returns true if a new association is found */

int CGraphic::TestAssociate(CObject *pTestObject, int nType)
{
	/* do nothing if already marked as associated */

	if (IsAssociated())
		return(ASSOCIATE_NONE);

	/* should never be passed itself */

	ASSERT(this != pTestObject);

// since the association test branches out through all the data objects in the database, as soon as associate with a particle that uses
// a common graphic (e.g. fuzzball), then everything links to just about everything else, and it's not too useful. so don't detect
// association between particles and graphics. i DO allow the association to be detected in the other direction-- that is, if the seed
// object is a graphic, then any particles that use it will allow the association. otherwise finding association with a graphic would
// never find any results
#if 0
	/* type-specific association test */

	switch (nType)
	{
	case EFFECTS:
		break;
	case PARTICLES:
		/* does passed particle use this graphic? */

		if (((CParticle *) pTestObject)->GetGraphic() == this)
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_PARTICLE_USES_GRAPHIC);
		}
		break;
	case GRAPHICS:
		break;
	case EMITTERS:
		break;
	case SWOOSHES:
		/* does passed swoosh use this graphic? */

		if (((CSwoosh *) pTestObject)->GetGraphic() == this)
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_SWOOSH_USES_GRAPHIC);
		}
		break;
	case SWOOSH_EMITTERS:
		break;
	default:
		ASSERT(false);
		break;
	}
#endif //0

	/* no association */

	return(ASSOCIATE_NONE);
}

/////////////////////////////////////////////////////////////////////////////
// CGraphic operations

/* bring up file dialog, import images. return TRUE if images actually added to graphic (no errors).
bReplace = TRUE will delete existing images if no errors. bUpdate will skip the file dialog and try to
update from the import path recorded when the images were first imported. bUpdate == TRUE implies
bReplace == TRUE. the different modes correspond to the different buttons on the graphic dialog:
import images, replace images, and update images */

bool CGraphic::ImportImage(bool bReplace, bool bUpdate)
{
	CString File;
	CString WildcardFile;
	bool bNoIndex;
	if (bUpdate)
	{
		/* an update tries to go back to the import path and re-load the files */

		File = GetBaseSourceFile();
		WildcardFile = GetBaseSourceFile();
		bReplace = TRUE;

		/* bNoIndex is FALSE if this is a wildcard load */

		bNoIndex = (WildcardFile.Find('*') == -1);
	}
	else
	{
		/* Get the data from the persistent strings */

		char InitialPath[_MAX_PATH];
		char Filename[_MAX_PATH];
		LoadPersistentString("Pigpen Import Image", InitialPath, sizeof(InitialPath));
		DissectPath(InitialPath, InitialPath, Filename);

		/* file dialog */

		CFileDialog FileDialog(
			TRUE,						// BOOL bOpenFileDialog
			"tga",					// LPCTSTR lpszDefExt = NULL
			Filename,				// LPCTSTR lpszFileName = NULL
			OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,	// DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT-- HIDEREADONLY hides a dialog check box, not read only files
			"Image Files (*.tga, *.pcx, *.bmp)|*.tga;*.pcx;*.bmp||",		// LPCTSTR lpszFilter = NULL, apparently can't have spaces around | symbols
			NULL);					// CWnd* pParentWnd = NULL
		FileDialog.m_ofn.lpstrInitialDir = InitialPath;	// set initial directory
		if (FileDialog.DoModal() != IDOK)
			return(FALSE);

		/* get file name, save in registry */

		File = FileDialog.GetPathName();
		WildcardFile = File;
		MakePersistentString("Pigpen Import Image", File.GetBuffer(10));

		/* get file extension */

		int nType = NO_TYPE;
		CString Extension;
		int nIndex = WildcardFile.ReverseFind('.');
		if (nIndex != -1)
		{
			Extension = WildcardFile.Right(WildcardFile.GetLength() - nIndex);
			WildcardFile.Delete(nIndex, WildcardFile.GetLength() - nIndex);
			CString UppercaseExtension = Extension;
			UppercaseExtension.MakeUpper();
			if (UppercaseExtension == ".PCX")
				nType = PCX_TYPE;
			else if (UppercaseExtension == ".TGA")
				nType = TGA_TYPE;
			else if (UppercaseExtension == ".BMP")
				nType = BMP_TYPE;
		}
		if (nType == NO_TYPE)
		{
			AfxMessageBox("Unsupported graphic image file extension.\nSupported types are .TGA, .PCX, and .BMP", MB_OK|MB_ICONEXCLAMATION);
			return(FALSE);
		}

		/* strip off numbers at end, replace with "*". if no numbers, don't add "*" so that we only get */
		/* a single file */

		bNoIndex = TRUE;
		while ((WildcardFile.GetAt(WildcardFile.GetLength() - 1) >= '0') && (WildcardFile.GetAt(WildcardFile.GetLength() - 1) <= '9'))
		{
			WildcardFile.Delete(WildcardFile.GetLength() - 1, 1);
			bNoIndex = FALSE;
		}
		if (bNoIndex == FALSE)
			WildcardFile += "*";
		WildcardFile += Extension;
	}

	/* start error log */

	int nErrors = 0;
	CString ErrorText("Error(s) found while importing images:\n\n");

	/* build up a list of files */

	CObArray ImageList;
	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile(WildcardFile.GetBuffer(10), &wfd);
	if (h != INVALID_HANDLE_VALUE)	// it's possible to not find any images on an update command
	{
		do
		{
			/* create a new image, record file name, also set index based on numbers at end of file name */

			CImage *pImage = new CImage;
			CString FileName = wfd.cFileName;
			if (pImage->SetSourceFileAndIndex(FileName, bNoIndex) == TRUE)
				ImageList.Add(pImage);
			else
			{
				ErrorText += "   No index in image file name \"";
				ErrorText += wfd.cFileName;
				ErrorText += "\"\n";
				nErrors++;
				delete pImage;	// don't add it, and so avoid sort/load
			}
		} while(FindNextFile(h, &wfd));
	}

	/* sort file list */

	for (int i = 1; i < ImageList.GetSize(); i++)
	{
		/* bubble sort's good enough here */

		for (int j = i; j > 0; j--)
		{
			CImage *pImage1 = (CImage *) ImageList.GetAt(j);
			CImage *pImage2 = (CImage *) ImageList.GetAt(j - 1);
			if (pImage1->GetTempIndex() < pImage2->GetTempIndex())
			{
				ImageList.RemoveAt(j);
				ImageList.InsertAt(j - 1, pImage1);
			}
			else
				break;
		}
	}

	/* if no images loaded, don't insist on matching size */

	if ((GetImageCount() == 0) || (bReplace))
	{
		m_nHeight = 0;
		m_nWidth = 0;
	}

	/* load image files, look for errors */

	if (ImageList.GetSize() == 0)
	{
		ErrorText += "   No image files found.\n";
		nErrors++;
	}
	for (i = 0; i < ImageList.GetSize(); i++)
	{
		CImage *pImage = (CImage *) ImageList.GetAt(i);
		int nReturn;
		if ((nReturn = pImage->ReadImageFile(m_nHeight, m_nWidth)) != READ_IMAGE_SUCCESS)
		{
			switch (nReturn)
			{
			case READ_IMAGE_OPEN_ERROR:
				ErrorText += "   Error opening image file \"";
				break;
			case READ_IMAGE_READ_ERROR:
				ErrorText += "   Error reading image file \"";
				break;
			case READ_IMAGE_MALLOC_ERROR:
				ErrorText += "   Error buffering image file \"";
				break;
			case READ_IMAGE_PROCESS_ERROR:
				ErrorText += "   Error processing image file \"";
				break;
			case READ_IMAGE_SIZE_ERROR:
				ErrorText += "   Non-power-of-2 size for image file \"";
				break;
			case READ_IMAGE_MATCH_SIZE_ERROR:
				ErrorText += "   Image does not match size of existing images: \"";
				break;
			default:
				ASSERT(FALSE);
				ErrorText += "   Unknown error for image file \"";
			}
			ErrorText += pImage->GetSourceFile();
			ErrorText += "\"\n";
			nErrors++;
		}

		/* record image size for first valid file-- all images must be the same size */

		if ((nReturn == READ_IMAGE_SUCCESS) && (m_nHeight == 0))
		{
			m_nHeight = pImage->GetHeight();
			m_nWidth = pImage->GetWidth();
		}
	}

	/* if any errors, report, clean up, and bail */

	if (nErrors)
	{
		ErrorText += "\nNo images loaded.";
		AfxMessageBox(ErrorText.GetBuffer(10), MB_OK|MB_ICONEXCLAMATION);
		while (ImageList.GetSize() != 0)
		{
			delete ImageList.GetAt(0);
			ImageList.RemoveAt(0);
		}
		return(FALSE);
	}

	/* dialog to let user choose single/multiple/replace/append */

	CImportDialog ImportDialog(this, File, WildcardFile, ImageList.GetSize(), bReplace, bUpdate);	// pass graphic record so dialog box can fill in intelligently
	int nReturn = ImportDialog.DoModal();
	if (nReturn == IDCANCEL)
		return(FALSE);

	/* replace existing images? */

	if (nReturn & IMPORT_REPLACE_EXISTING)
		DeleteArray(m_Images);

	/* importing into an empty graphic? */

	bool bFirstImport = (m_Images.GetSize() == 0);

	/* record source, add new images to graphic's image list */

	if (nReturn & IMPORT_ALL)
	{
		m_BaseSourceFile = WildcardFile;
		for (i = 0; i < ImageList.GetSize(); i++)
		{
			CImage *pImage = (CImage *) ImageList.GetAt(i);
			m_Images.Add(pImage);
		}
	}
	else
	{
		ASSERT(nReturn & IMPORT_SINGLE);
		m_BaseSourceFile = File;
		for (i = 0; i < ImageList.GetSize(); i++)
		{
			CImage *pImage = (CImage *) ImageList.GetAt(i);
			if (pImage->GetSourceFile() == File.Right(pImage->GetSourceFile().GetLength()))
				m_Images.Add(pImage);
		}
	}

	/* adjust icon index if necessary */

	if ((m_nIconIndex == NO_ICON) || (m_nIconIndex >= GetImageCount()))
		m_nIconIndex = GetImageCount() / 2;

	/* if this is first import, set default size to match image aspect ratio */

	if (bFirstImport)
	{
		m_fDefaultXSize = m_fDefaultYSize = 1.0f;
		if (m_nHeight > m_nWidth)
			m_fDefaultXSize = (float) m_nWidth / float (m_nHeight);
		else if (m_nWidth > m_nHeight)
			m_fDefaultYSize = (float) m_nHeight / float (m_nWidth);

		/* set default state for show alpha mask-- some images come in as alpha mask only, it's useless to */
		/* see the white image as the default */

		m_ulFlags &= ~GRAPHIC_SHOW_ALPHA_MASK;
		if (AlphaMaskOnly())
			m_ulFlags |= GRAPHIC_SHOW_ALPHA_MASK;
	}

	/* doc has changed */

	return(TRUE);
}

/* remove an image from the graphic */

void CGraphic::RemoveImage(int nIndex)
{
	ASSERT(nIndex < GetImageCount());
	CImage *pImage = GetImage(nIndex);
	delete m_Images.GetAt(nIndex);
	m_Images.RemoveAt(nIndex);
}

/* fill in data for export. return size of data, 0 if error or overflow */

//yuch-- needed? not if we just put starting/ending image indices in particle export record
int CGraphic::FillExportBuffer(char *pBuffer, int nMaxSize)
{
	int nBufferSize = 0;
	u16 *pu16Index = (u16 *) pBuffer;
	for (int i = 0; i < m_Images.GetSize(); i++, pu16Index++)
	{
		CImage *pImage = GetImage(i);
		if (nBufferSize + (signed) sizeof(u16) > nMaxSize)
		{
			ASSERT(FALSE);	// export buffer overflow-- increase EXPORT_BUFFER_SIZE in Export.h
			return(0);
		}
		*pu16Index = (u16) pImage->GetTempIndex();	// temp index set during texture list output
		nBufferSize += sizeof(u16);

		/* maybe endian swap it */

		u16 *pu16IndexSwapped = DBLUtil__SwapEndian(pu16Index);
		if (pu16IndexSwapped != pu16Index)
			memcpy(pu16Index, pu16IndexSwapped, sizeof(u16));	// if swapped, copy swapped version back into export buffer
	}

	/* record and return size of data that got filled in */

	SetExportDataSize(nBufferSize);
	return(GetExportDataSize());
}

/* determine if graphic is valid for export. an invalid graphic has no images */

int CGraphic::AnalyzeForExport(void)
{
	if (GetExportStatus() == EXPORT_STATUS_UNKNOWN)
	{
		/* analyze data */

		if (m_Images.GetSize() != 0)
		{
			SetExportStatus(EXPORT_STATUS_VALID);
			IncUsedCount();
		}
		else
			SetExportStatus(EXPORT_STATUS_INVALID);
	}
	return(GetExportStatus());
}

/* look at all images and return false if any has an image that isn't all the same color */

bool CGraphic::AlphaMaskOnly(void)
{
	for (int i = 0; i < m_Images.GetSize(); i++)
	{
		CImage *pImage = GetImage(i);
		if (pImage->GetImageBitCount() < 32)
			return(FALSE);	// no alpha mask

		/* look through pixels */

		ts_bRGBA *pPixel = (ts_bRGBA *) pImage->GetImageBits();
		int nPixels = pImage->GetHeight() * pImage->GetWidth();
		ts_bRGBA BasePixel = *pPixel;
		for (int i = 0; i < nPixels; i++, pPixel++)
		{
			if ((pPixel->R != BasePixel.R) || (pPixel->G != BasePixel.G) || (pPixel->B != BasePixel.B))
				return(FALSE);
		}
	}

	/* no valid image, only alpha masks */

	return(TRUE);
}

/* check if all images in graphic are feathered alpha */

bool CGraphic::AllImagesFeatheredAlpha(void)
{
	for (int i = 0; i < m_Images.GetSize(); i++)
	{
		CImage *pImage = (CImage *) m_Images.GetAt(i);
		if (pImage->IsFeatheredAlpha() == FALSE)
			return(FALSE);
	}
	return(TRUE);
}

/* check if all images in graphic are single-bit alpha */

bool CGraphic::AllImagesSingleBitAlpha(void)
{
	for (int i = 0; i < m_Images.GetSize(); i++)
	{
		CImage *pImage = (CImage *) m_Images.GetAt(i);
		if (pImage->IsSingleBitAlpha() == FALSE)
			return(FALSE);
	}
	return(TRUE);
}

/* do load-time fix-up on old versions of data. all pointers, etc have been resolved */

void CGraphic::LoadFixup(void)
{
	/* currently no fix-up required for graphic */

	/* allow images to fix themselves up */

	for (int i = 0; i < m_Images.GetSize(); i++)
	{
		CImage *pImage = (CImage *) m_Images.GetAt(i);
		pImage->LoadFixup();
	}
}

