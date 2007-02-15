/***********************************/
/*                                 */
/* Export.cpp                      */
/* Pigpen particle tool  1/5/00    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dbl file export                 */
/*                                 */
/***********************************/

/* export the database file for the particle effects */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



/******************* defines *****************************/

/* for speed (maybe, at least it avoids lots of malloc/free), allocate a buffer for all data records to place */
/* data in */

#define EXPORT_BUFFER_SIZE		1000

/******************* structures **************************/

/******************* local variables *********************/

static char ExportBuffer[EXPORT_BUFFER_SIZE];	// for data items to fill with export-ready data
static int nEffectCount;								// for data analysis
static int nParticleCount;								// for data analysis
static int nGraphicCount;								// for data analysis
static int nImageCount;									// for data analysis
static int nEmitterCount;								// for data analysis
static int nSwooshCount;								// for data analysis
static int nSwooshEmitterCount;						// for data analysis
static char ExportLogFilename[_MAX_PATH] = "";	// last export file created

/******************* externs *****************************/

/******************* local prototypes ********************/

static int AnalyzeGraphicImages(CObArray& Graphics);
static void SetGraphicExportSizes(CObArray& Graphics, u32 *pu32ImageSize);
static int WriteGraphicChunk(CObArray& Graphics);
static int WriteParticleChunk(CObArray& Particles, bool bPreview);
static int WriteEffectChunk(CObArray& Effects, bool bPreview);
static int WriteEmitterChunk(CObArray& Emitters, bool bPreview);
static int WriteSwooshChunk(CObArray& Swooshes, bool bPreview);
static int WriteSwooshEmitterChunk(CObArray& SwooshEmitters, bool bPreview);
static int WriteEmitterListChunk(CObArray& Emitters);
static int WriteEffectListChunk(CObArray& Effects);
static int WriteParticleListChunk(CObArray& Particles);
static int WriteSwooshListChunk(CObArray& Swooshes);
static int WriteSwooshEmitterListChunk(CObArray& SwooshEmitters);

/******************* functions ***************************/

/* bring up file dialog, export database. if pPreviewEmitter is non-NULL, only export one effect for preview
(should be fast), skip file dialog */

bool ExportDBL(CPigpenDoc *pDoc, CObArray *pArrays, char *pExportFileName, CEmitter *pPreviewEmitter /*= NULL*/, CPigpenSwooshEmitter *pPreviewSwooshEmitter /*= NULL*/)
{
	/* may be a real export or an export for previewing */

	bool bPreview = ((pPreviewEmitter) || (pPreviewSwooshEmitter));

	/* pigpen relies on texture orientation for some purposes, like align to velocity. can't uv-reverse textures */

	bAllowUVReverse = false;

	/* skip file dialog if exporting an effect for preview */

	CString File;
	if (bPreview)
		File = PREVIEW_FILE_NAME;
	else if (pExportFileName)
		File = pExportFileName;	// export file name is preset for command line mode
	else
	{
		/* Get the data from the persistent strings */

		char InitialPath[_MAX_PATH];
		char Filename[_MAX_PATH];
		LoadPersistentString("Pigpen Export DBL", InitialPath, sizeof(InitialPath));
		DissectPath(InitialPath, InitialPath, Filename);

		/* file dialog */

		CFileDialog FileDialog(
			FALSE,					// BOOL bOpenFileDialog
			"dbl",					// LPCTSTR lpszDefExt = NULL
			Filename,				// LPCTSTR lpszFileName = NULL
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	// DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT-- HIDEREADONLY hides a dialog check box, not read only files
			"DBL Files (*.db*)|*.db*||",					// LPCTSTR lpszFilter = NULL, apparently can't have spaces around | symbols
			NULL);					// CWnd* pParentWnd = NULL
		FileDialog.m_ofn.lpstrInitialDir = InitialPath;	// set initial directory
		if (FileDialog.DoModal() != IDOK)
			return(FALSE);

		/* get file name */

		File = FileDialog.GetPathName();
		MakePersistentString("Pigpen Export DBL", File.GetBuffer(10));
	}

	/* check if destination file is write-protected-- bail if so */

	FILE *pFile = fopen(File.GetBuffer(10), "r");
	bool bFileExists = (pFile != NULL);
	if (bFileExists)
		fclose(pFile);
	if ((bFileExists) && (_access(File.GetBuffer(10), 2 ) == -1))
	{
		if (PigpenApp.IsCommandLineMode())
			PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_WRITE_PROTECTED);
		else
			AfxMessageBox("File is write protected.", MB_OK | MB_ICONEXCLAMATION);
		return(false);
	}

	/* initialize error system, open error log */

	if (InitPluginErrors("Export", File.GetBuffer(10)) == FALSE)
		return(FALSE);
	AddConditionalHeaderToErrorLog("Exporting file");

	/* show progress dialog */

	CProgressDialog *pProgressDialog = new CProgressDialog("Exporting", File, 13);	// last parameter is number of progress "chunks"
	pProgressDialog->Create(IDD_PROGRESS, PigpenApp.m_pMainWnd);

	/* open export log, save name for ID_VIEW_EXPORT_LOG menu item */

	char acDrive[_MAX_DRIVE];
	char acDir[_MAX_DIR];
	char acRoot[_MAX_FNAME];
	_splitpath(File.GetBuffer(10), acDrive, acDir, acRoot, NULL);
	strcat(acRoot, "Export");
	_makepath(ExportLogFilename, acDrive, acDir, acRoot, ".log");
	OpenLogFile(ExportLogFilename);
	WriteLogFile("Exporting Pigpen particle database...");

	/* create info lists needed for collecting data-- just texture list for pigpen */

	InfoLists.m_pTextureList = new CTextureList;

	/* keep going as long as iReturnCode == RETURN_SUCCESS */

	int iReturnCode = RETURN_SUCCESS;

	/* analyze data-- mark empty/unused items so they don't get exported. if nothing to do, bail */

	if (iReturnCode == RETURN_SUCCESS)
	{
		pProgressDialog->UpdateInfo("Analyzing data...");
		iReturnCode = AnalyzeDataForExport(pArrays, pPreviewEmitter, pPreviewSwooshEmitter);
	}
	pProgressDialog->UpdateProgress();

	/* analyze graphics for texture chunk creation */

	if (iReturnCode == RETURN_SUCCESS)
	{
		pProgressDialog->UpdateInfo("Analyzing graphic images...");
		iReturnCode = AnalyzeGraphicImages(pArrays[GRAPHICS]);
	}
	pProgressDialog->UpdateProgress();

	/* build up some text to add to the start of the .dbl. \x1a is ^z */

	CString acMessage;
	char acMessage2[1000];
	CString SourceFileName = pDoc->GetPathName();	// a hack, using pDoc->GetPathName().GetBuffer(10) doesn't work for cryptic (to me) reasons
	acMessage.Format("Created by Pigpen from source file:\n   %s\n", SourceFileName.GetBuffer(10));
	acMessage.Append("\nParticles:\n");
	for (int i = 0; i < pArrays[PARTICLES].GetSize(); i++)
	{
		CParticle *pParticle = (CParticle *) pArrays[PARTICLES].GetAt(i);
		if (pParticle->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		sprintf(acMessage2, "   %s\n", pParticle->GetName());
		acMessage.Append(acMessage2);
	}
	acMessage.Append("\nEffects:\n");
	for (i = 0; i < pArrays[EFFECTS].GetSize(); i++)
	{
		CEffect *pEffect = (CEffect *) pArrays[EFFECTS].GetAt(i);
		if (pEffect->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		sprintf(acMessage2, "   %s\n", pEffect->GetName());
		acMessage.Append(acMessage2);
	}
	acMessage.Append("\nEmitters:\n");
	for (i = 0; i < pArrays[EMITTERS].GetSize(); i++)
	{
		CEmitter *pEmitter = (CEmitter *) pArrays[EMITTERS].GetAt(i);
		if (pEmitter->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		sprintf(acMessage2, "   %s\n", pEmitter->GetName());
		acMessage.Append(acMessage2);
	}
	acMessage.Append("\nSwooshes:\n");
	for (i = 0; i < pArrays[SWOOSHES].GetSize(); i++)
	{
		CSwoosh *pSwoosh = (CSwoosh *) pArrays[SWOOSHES].GetAt(i);
		if (pSwoosh->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		sprintf(acMessage2, "   %s\n", pSwoosh->GetName());
		acMessage.Append(acMessage2);
	}
	acMessage.Append("\nSwoosh emitter:\n");
	for (i = 0; i < pArrays[SWOOSH_EMITTERS].GetSize(); i++)
	{
		CPigpenSwooshEmitter *pSwooshEmitter = (CPigpenSwooshEmitter *) pArrays[SWOOSH_EMITTERS].GetAt(i);
		if (pSwooshEmitter->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		sprintf(acMessage2, "   %s\n", pSwooshEmitter->GetName());
		acMessage.Append(acMessage2);
	}
	acMessage.Append("\x1a");	// ^z so text stops in dos prompt

	/* start dbl file. a lot of the dbl file stuff is in C */

	if (iReturnCode == RETURN_SUCCESS)
		iReturnCode = StartDBLFile(File.GetBuffer(10), acMessage.GetBuffer());

	/* texture chunk-- uses the export routines from ImportExport */

	if (iReturnCode == RETURN_SUCCESS)
	{
		/* we want to get ps2 texture sizes returned for filling in export field in graphics view */

		u32 *pu32ImageSize = (u32 *) malloc(InfoLists.GetTextureCount() * sizeof(u32));
		_ASSERT(pu32ImageSize);

		/* write texture chunk */

		pProgressDialog->UpdateInfo("Writing texture chunk...");
		WriteLogFileAndWindow("Writing texture chunk...");
		WriteLogFile("   Note: Reported texture dimensions are width x height");
		AddConditionalHeaderToErrorLog("Writing texture chunk");
		bDynamicTextureSet = TRUE;	// particles are currently treated as dynamic texture set on ps2
		iReturnCode = InfoLists.m_pTextureList->WriteTextureChunk(File.GetBuffer(10), pu32ImageSize, NULL);

		/* fill in graphic export size field */

		if (iReturnCode == RETURN_SUCCESS)
			SetGraphicExportSizes(pArrays[GRAPHICS], pu32ImageSize);
		free(pu32ImageSize);
	}
	pProgressDialog->UpdateProgress();

	/* graphic chunk-- just groups of texture indices */

//yuch-- unneeded if we continue putting image start/end indices in particle export record
//	if (iReturnCode == RETURN_SUCCESS)
//		iReturnCode = WriteGraphicChunk(pArrays[GRAPHICS]);

	/* particle chunk-- the big one (well, maybe the texture chunk's bigger?) */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing particle chunk...");
		pProgressDialog->UpdateInfo("Writing particle chunk...");
		iReturnCode = WriteParticleChunk(pArrays[PARTICLES], bPreview);
	}
	pProgressDialog->UpdateProgress();

	/* effect chunk-- the ones that actually get used to start a particle effect */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing effect chunk...");
		pProgressDialog->UpdateInfo("Writing effect chunk...");
		iReturnCode = WriteEffectChunk(pArrays[EFFECTS], bPreview);
	}
	pProgressDialog->UpdateProgress();

	/* emitter chunk-- can be useful to predefine some emitters, especially for things like tires */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing emitter chunk...");
		pProgressDialog->UpdateInfo("Writing emitter chunk...");
		iReturnCode = WriteEmitterChunk(pArrays[EMITTERS], bPreview);
	}
	pProgressDialog->UpdateProgress();

	/* swoosh chunk */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing swoosh chunk...");
		pProgressDialog->UpdateInfo("Writing swoosh chunk...");
		iReturnCode = WriteSwooshChunk(pArrays[SWOOSHES], bPreview);
	}
	pProgressDialog->UpdateProgress();

	/* swoosh emitter chunk */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing swoosh emitter chunk...");
		pProgressDialog->UpdateInfo("Writing swoosh emitter chunk...");
		iReturnCode = WriteSwooshEmitterChunk(pArrays[SWOOSH_EMITTERS], bPreview);
	}
	pProgressDialog->UpdateProgress();

	/* emitter names */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing emitter names chunk...");
		pProgressDialog->UpdateInfo("Writing emitter list...");
		iReturnCode = WriteEmitterListChunk(pArrays[EMITTERS]);
	}
	pProgressDialog->UpdateProgress();

	/* effect names */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing effect names chunk...");
		pProgressDialog->UpdateInfo("Writing effect list...");
		iReturnCode = WriteEffectListChunk(pArrays[EFFECTS]);
	}
	pProgressDialog->UpdateProgress();

	/* particle names */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing particle names chunk...");
		pProgressDialog->UpdateInfo("Writing particle list...");
		iReturnCode = WriteParticleListChunk(pArrays[PARTICLES]);
	}
	pProgressDialog->UpdateProgress();

	/* swoosh names */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing swoosh names chunk...");
		pProgressDialog->UpdateInfo("Writing swoosh list...");
		iReturnCode = WriteSwooshListChunk(pArrays[SWOOSHES]);
	}
	pProgressDialog->UpdateProgress();

	/* swoosh emitter names */

	if (iReturnCode == RETURN_SUCCESS)
	{
		WriteLogFile("Writing swoosh emitter names chunk...");
		pProgressDialog->UpdateInfo("Writing swoosh emitter list...");
		iReturnCode = WriteSwooshEmitterListChunk(pArrays[SWOOSH_EMITTERS]);
	}
	pProgressDialog->UpdateProgress();

	/* finish DBL database file */

	if (iReturnCode == RETURN_SUCCESS)
		iReturnCode = FinishDBLFile();

	/* clean up */

	CleanUpDBLFile();
	CleanUpTextureChunk();

	/* close progress dialog */

	pProgressDialog->DestroyWindow();
	delete pProgressDialog;

	/* show chunk summary, close export log file */

	WriteLogFile(" ");	// blank line, but WriteLogFile ignores ""
	WriteLogFile("Export Summary:");
	WriteLogFile(" ");	// blank line, but WriteLogFile ignores ""
	ChunkLog__ShowSummary(true);
	CloseLogFile();

	/* error? */

	ShowPluginErrors("Export");
	return(GetFatalErrorCount() == 0);
}

/* go through database looking for empty/unused items-- they don't get exported. if pPreviewEmitter is
non-NULL, we are exporting for preview, so only include effect that is called by emitter (and possibly spawned
effects) */

int AnalyzeDataForExport(CObArray *pArrays, CEmitter *pPreviewEmitter, CPigpenSwooshEmitter *pPreviewSwooshEmitter)
{
	/* note what we're doing for error log */

	AddConditionalHeaderToErrorLog("Analyzing data");

	/* may be a real export or an export for previewing */

	bool bPreview = ((pPreviewEmitter) || (pPreviewSwooshEmitter));

	/* clear export status of all data items */

	for (int i = 0; i < MAX_DOC_DATA_TYPES; i++)
	{
		for (int j = 0; j < pArrays[i].GetSize(); j++)
		{
			switch(i)
			{
			case EFFECTS:
				{
					CEffect *pEffect = (CEffect *) pArrays[i].GetAt(j);
					pEffect->SetExportStatus(EXPORT_STATUS_UNKNOWN);
					pEffect->SetTempIndex(-1);
					pEffect->SetUsedCount(0);
				}
				break;
			case PARTICLES:
				{
					CParticle *pParticle = (CParticle *) pArrays[i].GetAt(j);
					pParticle->SetExportStatus(EXPORT_STATUS_UNKNOWN);
					pParticle->SetTempIndex(-1);
					pParticle->SetUsedCount(0);
				}
				break;
			case GRAPHICS:
				{
					CGraphic *pGraphic = (CGraphic *) pArrays[i].GetAt(j);
					pGraphic->SetExportStatus(EXPORT_STATUS_UNKNOWN);
					pGraphic->SetTempIndex(-1);
					pGraphic->SetUsedCount(0);
					pGraphic->SetTextureSetSize(-1);	// if graphic not exported, we won't know size

					/* clear image indices too */

					for (int k = 0; k < pGraphic->GetImageCount(); k++)
						pGraphic->GetImage(k)->SetTempIndex(-1);
				}
				break;
			case EMITTERS:
				{
					CEmitter *pEmitter = (CEmitter *) pArrays[i].GetAt(j);
					pEmitter->SetExportStatus(EXPORT_STATUS_UNKNOWN);
					pEmitter->SetTempIndex(-1);
					pEmitter->SetUsedCount(0);
				}
				break;
			case SWOOSHES:
				{
					CSwoosh *pSwoosh = (CSwoosh *) pArrays[i].GetAt(j);
					pSwoosh->SetExportStatus(EXPORT_STATUS_UNKNOWN);
					pSwoosh->SetTempIndex(-1);
					pSwoosh->SetUsedCount(0);
				}
				break;
			case SWOOSH_EMITTERS:
				{
					CPigpenSwooshEmitter *pSwooshEmitter = (CPigpenSwooshEmitter *) pArrays[i].GetAt(j);
					pSwooshEmitter->SetExportStatus(EXPORT_STATUS_UNKNOWN);
					pSwooshEmitter->SetTempIndex(-1);
					pSwooshEmitter->SetUsedCount(0);
				}
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
	}

	/* in preview mode, analyze the single preview emitter and let everything trickle down. this is special */
	/* behavior for preview mode-- normally the emitter AnalyzeForExport does not trickle down */

	if (bPreview)
	{
		if (pPreviewEmitter)
			pPreviewEmitter->AnalyzeForExport(true);
		if (pPreviewSwooshEmitter)
			pPreviewSwooshEmitter->AnalyzeForExport(true);
	}
	else
	{
		/* go through effects, let analysis trickle down */

		for (i = 0; i < pArrays[EFFECTS].GetSize(); i++)
		{
			CEffect *pEffect = (CEffect *) pArrays[EFFECTS].GetAt(i);
			pEffect->AnalyzeForExport(false);
		}

		/* emitters are separate-- non-empty effects will be exported whether they are referred to by */
		/* an emitter or not (since they can be triggered individually). so no trickle down action for emitters. */
		/*** do this after effects, since emitters need to know whether effects are valid for export ***/

		for (i = 0; i < pArrays[EMITTERS].GetSize(); i++)
		{
			CEmitter *pEmitter = (CEmitter *) pArrays[EMITTERS].GetAt(i);
			pEmitter->AnalyzeForExport(false);
		}

		/* swooshes and swoosh emitters. swooshes can be started independently, so no worry about trickle down */
		/* from the emitters, but the emitters have to be analyzed after the swooshes since they need to know */
		/* whether the swooshes are valid for export */

		for (i = 0; i < pArrays[SWOOSHES].GetSize(); i++)
		{
			CSwoosh *pSwoosh = (CSwoosh *) pArrays[SWOOSHES].GetAt(i);
			pSwoosh->AnalyzeForExport(false);
		}
		for (i = 0; i < pArrays[SWOOSH_EMITTERS].GetSize(); i++)
		{
			CPigpenSwooshEmitter *pSwooshEmitter = (CPigpenSwooshEmitter *) pArrays[SWOOSH_EMITTERS].GetAt(i);
			pSwooshEmitter->AnalyzeForExport(false);
		}
	}

	/* assign sequential indices to valid data items */

	nEffectCount = 0;
	nParticleCount = 0;
	nGraphicCount = 0;
	nImageCount = 0;
	nEmitterCount = 0;
	nSwooshCount = 0;
	nSwooshEmitterCount = 0;
	for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
	{
		for (int j = 0; j < pArrays[i].GetSize(); j++)
		{
			switch(i)
			{
			case EFFECTS:
				{
					CEffect *pEffect = (CEffect *) pArrays[i].GetAt(j);
					if (pEffect->GetExportStatus() == EXPORT_STATUS_VALID)
						pEffect->SetTempIndex(nEffectCount++);
				}
				break;
			case PARTICLES:
				{
					CParticle *pParticle = (CParticle *) pArrays[i].GetAt(j);
					if (pParticle->GetExportStatus() == EXPORT_STATUS_VALID)
						pParticle->SetTempIndex(nParticleCount++);
				}
				break;
			case GRAPHICS:
				{
					CGraphic *pGraphic = (CGraphic *) pArrays[i].GetAt(j);
					if (pGraphic->GetExportStatus() == EXPORT_STATUS_VALID)
					{
						pGraphic->SetTempIndex(nGraphicCount++);
//yuch-- image temp indices are set in AnalyzeGraphicImages, when they are added to the hash table so that
// there are no duplicates. the possibility exists that graphics will use the same images in different orders,
// at which pont this will break. we'll have to supply a translation table...
						for (int k = 0; k < pGraphic->GetImageCount(); k++)
						{
//							CImage *pImage = pGraphic->GetImage(k);
//							pImage->SetTempIndex(nImageCount++);
CImage *pImage = pGraphic->GetImage(k);
pImage->SetTempIndex(-1);
nImageCount++;	// to satisfy check below
						}
					}
				}
				break;
			case EMITTERS:
				{
					CEmitter *pEmitter = (CEmitter *) pArrays[i].GetAt(j);
					if (pEmitter->GetExportStatus() == EXPORT_STATUS_VALID)
						pEmitter->SetTempIndex(nEmitterCount++);
				}
				break;
			case SWOOSHES:
				{
					CSwoosh *pSwoosh = (CSwoosh *) pArrays[i].GetAt(j);
					if (pSwoosh->GetExportStatus() == EXPORT_STATUS_VALID)
						pSwoosh->SetTempIndex(nSwooshCount++);
				}
				break;
			case SWOOSH_EMITTERS:
				{
					CPigpenSwooshEmitter *pSwooshEmitter = (CPigpenSwooshEmitter *) pArrays[i].GetAt(j);
					if (pSwooshEmitter->GetExportStatus() == EXPORT_STATUS_VALID)
						pSwooshEmitter->SetTempIndex(nSwooshEmitterCount++);
				}
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
	}

	/* check for an empty/invalid database. ok for emitter or swoosh count to be 0. if swoosh count */
	/* is non-0, ok for everything else to be 0 */

	if (nSwooshCount == 0)
	{
		/* if no swooshes, then there must be effects. it is possible that there are no graphics, if */
		/* all particles use special graphics like front buffer */

		if (nEffectCount == 0)
			HANDLE_ERROR(RETURN_NO_EFFECTS);
		if (nParticleCount == 0)
			HANDLE_ERROR(RETURN_NO_PARTICLES);
	}

	/* success */

	return(RETURN_SUCCESS);
}

/* analyze textures used in graphics */

static int AnalyzeGraphicImages(CObArray& Graphics)
{
	int iReturnCode;
	int nImageIndex = 0;
	char acPathName[MAX_PATH];
	char *pPathName;

	/* create a hash table to put textures in */

	if ((iReturnCode = InfoLists.m_pTextureList->InitHashTable()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* go through all graphics and output all image file names */

	for (int i = 0; i < Graphics.GetSize(); i++)
	{
		/* this graphic valid for export? */

		CGraphic *pGraphic = (CGraphic *) Graphics.GetAt(i);
		if (pGraphic->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;

		/* get path name */

		strcpy(acPathName, pGraphic->GetBaseSourceFile().GetBuffer(10));

		/* translate forward slashes to backslashes */

		pPathName = acPathName;
		while (*pPathName != 0)
		{
			if (*pPathName == '/')
				*pPathName = '\\';
			pPathName++;
		}

		/* get rid of wildcard file root */

		pPathName = acPathName + strlen(acPathName) - 1;
		while ((pPathName >= acPathName) && (*pPathName != '\\') && (*pPathName != ':'))
		{
			*pPathName = 0;
			pPathName--;
		}

		/* skip any drive letter designation */

		pPathName = strstr(acPathName, ":");
		if (pPathName == NULL)
			pPathName = acPathName;
		else
			pPathName++;	// point to character after colon

		/* go through images for this graphic */

		for (int j = 0; j < pGraphic->GetImageCount(); j++)
		{
			CImage *pImage = pGraphic->GetImage(j);

			/* register by entire path name */

			char acFileName[_MAX_PATH];
			sprintf(acFileName, "%s%s", pPathName, pImage->GetSourceFile().GetBuffer(10));

			/* add to hash table */

			int nIndex;
			if ((iReturnCode = InfoLists.m_pTextureList->AddToHashTable(acFileName, NULL, -1, TEXTURE_USE_COLORMAP, &nIndex)) != RETURN_SUCCESS)
				return(iReturnCode);

			/* set actual index in texture set for reference by graphics in CGraphic::FillExportBuffer */

			pImage->SetTempIndex(nIndex);
//WriteLogFile("graphic %d, image %s, temp index %d", pGraphic->GetName().GetBuffer(10), pImage->GetSourceFile().GetBuffer(10), nIndex);
		}
	}

	/* TextureInfo list is complete-- grab ordered buffer, free hash table */

	if ((iReturnCode = InfoLists.m_pTextureList->BuildInfoBuffer()) != RETURN_SUCCESS)
		return(iReturnCode);
	InfoLists.m_pTextureList->FreeHashTable();

	/* pre-load images */

	for (i = 0; i < Graphics.GetSize(); i++)
	{
		/* this graphic valid for export? */

		CGraphic *pGraphic = (CGraphic *) Graphics.GetAt(i);
		if (pGraphic->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;

		/* go through images for this graphic */

		for (int j = 0; j < pGraphic->GetImageCount(); j++)
		{
			CImage *pImage = pGraphic->GetImage(j);
			CTextureInfo *pTextureInfo = InfoLists.GetTextureInfo(pImage->GetTempIndex());

			/* image has already been loaded, so don't have to reload it. but make a copy of the bitmap */
			/* since the export process may change it (e.g. convert from 32-bit rgba to paletted) */

			if (pTextureInfo->IsImageLoaded() == FALSE)
			{
				pTextureInfo->ConvertImage.BMIRecord.pBMI = (BITMAPINFO *) malloc(pImage->GetBMISize());
				ASSERT(pTextureInfo->ConvertImage.BMIRecord.pBMI);
				memcpy(pTextureInfo->ConvertImage.BMIRecord.pBMI, pImage->GetImageBMI(), pImage->GetBMISize());
				pTextureInfo->ConvertImage.BMIRecord.pBMBits = (u8 *) malloc(pImage->GetImageSize());
				memcpy(pTextureInfo->ConvertImage.BMIRecord.pBMBits, pImage->GetImageBits(), pImage->GetImageSize());
				ASSERT(pTextureInfo->ConvertImage.BMIRecord.pBMBits);
				pTextureInfo->m_nSourceImageType = pImage->GetImageType();
				pTextureInfo->SetImageLoaded(TRUE);
			}
		}
	}

	/* confirm that we've pre-loaded all images */

#ifdef _DEBUG
	for (i = 0; i < InfoLists.GetTextureCount(); i++)
		ASSERT(InfoLists.GetTextureInfo(i)->IsImageLoaded());
#endif //_DEBUG

	/* load and convert textures */

	if ((iReturnCode = InfoLists.m_pTextureList->ImportAll()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* ok */

	return(RETURN_SUCCESS);
}	

/* set export data size field in graphics. pu32ImageSize buffer gets filled via call to WriteTextureChunk */

static void SetGraphicExportSizes(CObArray& Graphics, u32 *pu32ImageSize)
{
	/* set export texture set sizes in view */

	_ASSERT(pu32ImageSize);
	for (int i = 0; i < Graphics.GetSize(); i++)
	{
		/* this graphic valid for export? */

		CGraphic *pGraphic = (CGraphic *) Graphics.GetAt(i);
		if (pGraphic->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;

		/* go through images for this graphic */

		pGraphic->SetTextureSetSize(0);
		for (int j = 0; j < pGraphic->GetImageCount(); j++)
		{
			CImage *pImage = pGraphic->GetImage(j);
			_ASSERT(pImage->GetTempIndex() < InfoLists.GetTextureCount());
			pGraphic->IncTextureSetSize(pu32ImageSize[pImage->GetTempIndex()]);
		}
	}
}

/* write the graphic chunk-- a graphic at this point is just a list of texture indices */

//yuch-- unneeded if we continue putting image start/end indices in particle export record
#if 0
static int WriteGraphicChunk(CObArray& Graphics)
{
	int iReturnCode;

	/* note what we're doing for error log */

	AddConditionalHeaderToErrorLog("Writing graphic chunk");

	/* start graphic chunk */

	if ((iReturnCode = StartChunk(DB_PARTICLE_GRAPHIC, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nGraphicCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out graphics */

	for (int i = 0; i < Graphics.GetSize(); i++)
	{
		/* valid for export? */

		CGraphic *pGraphic = (CGraphic *) Graphics.GetAt(i);
		if (pGraphic->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		AddConditionalRecordToErrorLog(pGraphic->GetName().GetBuffer(10));

		/* mark new record list entry */

		RecordListEntry();

		/* build data */

		int nRecordSize = pGraphic->FillExportBuffer(ExportBuffer, EXPORT_BUFFER_SIZE);
		if (nRecordSize == 0)
			HANDLE_ERROR(RETURN_GRAPHIC_EXPORT_BUFFER_ERROR);

		/* write data */

		if (DBLUtil__WriteData(ExportBuffer, nRecordSize, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* finish up chunk */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* success */

	return(RETURN_SUCCESS);
}
#endif //0

/* write the particle chunk-- particles refer to a graphic, and have a bunch of data about how the particle behaves */

static int WriteParticleChunk(CObArray& Particles, bool bPreview)
{
	int iReturnCode;

	/* maybe nothing to do */

	if (nParticleCount == 0)
		return(RETURN_SUCCESS);

	/* note what we're doing for error log */

	AddConditionalHeaderToErrorLog("Writing particle chunk");

	/* start particle chunk */

	if ((iReturnCode = StartChunk(DB_PARTICLE_SOURCE, DB_PARTICLE_SOURCE_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nParticleCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out particles */

	for (int i = 0; i < Particles.GetSize(); i++)
	{
		/* valid for export? */

		CParticle *pParticle = (CParticle *) Particles.GetAt(i);
		if (pParticle->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		AddConditionalRecordToErrorLog(pParticle->GetName().GetBuffer(10));

		/* mark new record list entry */

		RecordListEntry();

		/* build data */

		int nRecordSize = pParticle->FillExportBuffer(ExportBuffer, EXPORT_BUFFER_SIZE, bPreview);
		if (nRecordSize == 0)
			HANDLE_ERROR(RETURN_PARTICLE_EXPORT_BUFFER_ERROR);

		/* write data */

		if (DBLUtil__WriteData(ExportBuffer, nRecordSize, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* finish up chunk */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* success */

	return(RETURN_SUCCESS);
}

/* write the effect chunk-- effects have one or more channels, a channel is usually an particle */

static int WriteEffectChunk(CObArray& Effects, bool bPreview)
{
	int iReturnCode;

	/* maybe nothing to do */

	if (nEffectCount == 0)
		return(RETURN_SUCCESS);

	/* note what we're doing for error log */

	AddConditionalHeaderToErrorLog("Writing effect chunk");

	/* start effect chunk */

	if ((iReturnCode = StartChunk(DB_PARTICLE_EFFECT, DB_PARTICLE_EFFECT_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nEffectCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out effects */

	for (int i = 0; i < Effects.GetSize(); i++)
	{
		/* valid for export? */

		CEffect *pEffect = (CEffect *) Effects.GetAt(i);
		if (pEffect->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		AddConditionalRecordToErrorLog(pEffect->GetName().GetBuffer(10));

		/* mark new record list entry */

		RecordListEntry();

		/* build data */

		int nRecordSize = pEffect->FillExportBuffer(ExportBuffer, EXPORT_BUFFER_SIZE, bPreview);
		if (nRecordSize == 0)
			HANDLE_ERROR(RETURN_EFFECT_EXPORT_BUFFER_ERROR);

		/* write data */

		if (DBLUtil__WriteData(ExportBuffer, nRecordSize, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* finish up chunk */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* success */

	return(RETURN_SUCCESS);
}

/* write the emitter chunk-- emitters emit effects */

static int WriteEmitterChunk(CObArray& Emitters, bool bPreview)
{
	int iReturnCode;

	/* maybe nothing to do */

	if (nEmitterCount == 0)
		return(RETURN_SUCCESS);

	/* note what we're doing for error log */

	AddConditionalHeaderToErrorLog("Writing emitter chunk");

	/* start effect chunk */

	if ((iReturnCode = StartChunk(DB_PARTICLE_EMITTER, DB_PARTICLE_EMITTER_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nEmitterCount, RECORD_LIST_SAME_SIZE_RECORDS, sizeof(ts_EmitterSource))) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out emitters */

	for (int i = 0; i < Emitters.GetSize(); i++)
	{
		/* valid for export? */

		CEmitter *pEmitter = (CEmitter *) Emitters.GetAt(i);
		if (pEmitter->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		AddConditionalRecordToErrorLog(pEmitter->GetName().GetBuffer(10));

		/* mark new record list entry */

		RecordListEntry();

		/* build data */

		int nRecordSize = pEmitter->FillExportBuffer(ExportBuffer, EXPORT_BUFFER_SIZE, bPreview);
		if (nRecordSize == 0)
			HANDLE_ERROR(RETURN_EMITTER_EXPORT_BUFFER_ERROR);

		/* write data */

		if (DBLUtil__WriteData(ExportBuffer, nRecordSize, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* finish up chunk */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* success */

	return(RETURN_SUCCESS);
}

/* write the swoosh chunk-- swooshes refer to a graphic, and have a bunch of data about how the swoosh behaves */

static int WriteSwooshChunk(CObArray& Swooshes, bool bPreview)
{
	int iReturnCode;

	/* maybe nothing to do */

	if (nSwooshCount == 0)
		return(RETURN_SUCCESS);

	/* note what we're doing for error log */

	AddConditionalHeaderToErrorLog("Writing swoosh chunk");

	/* start swoosh chunk */

	if ((iReturnCode = StartChunk(DB_SWOOSH_SOURCE, DB_SWOOSH_SOURCE_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nSwooshCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out swooshes */

	for (int i = 0; i < Swooshes.GetSize(); i++)
	{
		/* valid for export? */

		CSwoosh *pSwoosh = (CSwoosh *) Swooshes.GetAt(i);
		if (pSwoosh->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		AddConditionalRecordToErrorLog(pSwoosh->GetName().GetBuffer(10));

		/* mark new record list entry */

		RecordListEntry();

		/* build data */

		int nRecordSize = pSwoosh->FillExportBuffer(ExportBuffer, EXPORT_BUFFER_SIZE, bPreview);
		if (nRecordSize == 0)
			HANDLE_ERROR(RETURN_SWOOSH_EXPORT_BUFFER_ERROR);

		/* write data */

		if (DBLUtil__WriteData(ExportBuffer, nRecordSize, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* finish up chunk */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* success */

	return(RETURN_SUCCESS);
}

/* write the swoosh emitter chunk-- swoosh emitters refer to a swoosh, and some more data to control swoosh emission */

static int WriteSwooshEmitterChunk(CObArray& SwooshEmitters, bool bPreview)
{
	int iReturnCode;

	/* maybe nothing to do */

	if (nSwooshEmitterCount == 0)
		return(RETURN_SUCCESS);

	/* note what we're doing for error log */

	AddConditionalHeaderToErrorLog("Writing swoosh emitter chunk");

	/* start swoosh emitter chunk */

	if ((iReturnCode = StartChunk(DB_SWOOSH_EMITTER, DB_SWOOSH_EMITTER_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nSwooshEmitterCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out swoosh emitters */

	for (int i = 0; i < SwooshEmitters.GetSize(); i++)
	{
		/* valid for export? */

		CPigpenSwooshEmitter *pSwooshEmitter = (CPigpenSwooshEmitter *) SwooshEmitters.GetAt(i);
		if (pSwooshEmitter->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;
		AddConditionalRecordToErrorLog(pSwooshEmitter->GetName().GetBuffer(10));

		/* mark new record list entry */

		RecordListEntry();

		/* build data */

		int nRecordSize = pSwooshEmitter->FillExportBuffer(ExportBuffer, EXPORT_BUFFER_SIZE, bPreview);
		if (nRecordSize == 0)
			HANDLE_ERROR(RETURN_SWOOSH_EMITTER_EXPORT_BUFFER_ERROR);

		/* write data */

		if (DBLUtil__WriteData(ExportBuffer, nRecordSize, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* finish up chunk */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* success */

	return(RETURN_SUCCESS);
}

/* access routines for export log file name */

bool ValidExportLogFile(void)
{
	return(strlen(ExportLogFilename) != 0);
}
char *GetExportLogFilename(void)
{
	return(ExportLogFilename);
}

/* export just a texture chunk for texture size analysis */

void AnalyzeTextureSizes(CObArray& Graphics)
{
	/* show a wait cursor (automatically clears when we exit routine) */

	CWaitCursor WaitCursor;

	/* we want to know about all graphics */

	for (int i = 0; i < Graphics.GetSize(); i++)
	{
		CGraphic *pGraphic = (CGraphic *) Graphics.GetAt(i);
		pGraphic->SetExportStatus(EXPORT_STATUS_VALID);
	}

	/* keep going as long as iReturnCode == RETURN_SUCCESS */

	int iReturnCode = RETURN_SUCCESS;

	/* analyze all image files into a hash table */

	if (iReturnCode == RETURN_SUCCESS)
		iReturnCode = AnalyzeGraphicImages(Graphics);

	/* start a dummy dbl file */

	if (iReturnCode == RETURN_SUCCESS)
		iReturnCode = StartDBLFile("temp.dbl");

	/* write ps2 texture chunk, fill in sizes */

	if (iReturnCode == RETURN_SUCCESS)
	{
		/* we want to get ps2 texture sizes returned for filling in export field in graphics view */

		u32 *pu32ImageSize = (u32 *) malloc(InfoLists.GetTextureCount() * sizeof(u32));
		_ASSERT(pu32ImageSize);

		/* write texture chunk */

		nExportMode = EXPORT_PS2;
		iReturnCode = InfoLists.m_pTextureList->WriteTextureChunk("temp.dbl", pu32ImageSize, NULL);

		/* fill in graphic export size field */

		if (iReturnCode == RETURN_SUCCESS)
			SetGraphicExportSizes(Graphics, pu32ImageSize);
		free(pu32ImageSize);
	}

	/* erase dummy file */

	remove("temp.dbl");
}

/* write out emitter names as strings in a chunk */

static int WriteEmitterListChunk(CObArray& Emitters)
{
	int iReturnCode;

	/* anything to do? */

	if (nEmitterCount == 0)
		return(RETURN_SUCCESS);

	/* start chunk */

	if ((iReturnCode = StartChunk(DB_EMITTER_LIST, DB_EMITTER_LIST_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

 	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nEmitterCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out emitter names */

	int nCount = 0;
	for (int i = 0; i < Emitters.GetSize(); i++)
	{
		/* valid for export? */

		CEmitter *pEmitter = (CEmitter *) Emitters.GetAt(i);
		if (pEmitter->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;

// now just using name, no need to "define-ify" it
//		/* build name */
//
//		char Label[200];
//		char *pLabel = Label;
//		sprintf(Label, "EMITTER_%s", pEmitter->GetName().GetBuffer(10));
//		while (*pLabel != 0)
//		{
//			*pLabel = toupper(*pLabel);
//			if (((*pLabel < '0') || (*pLabel > '9')) && ((*pLabel < 'A') || (*pLabel > 'Z')))
//				*pLabel = '_';
//			pLabel++;
//		}

		/* record in log */

		WriteLogFile("   %d: %s", nCount, pEmitter->GetName());
		nCount++;

		/* mark new record list entry */

		RecordListEntry();

		/* write data-- include terminating 0 */

		if (DBLUtil__WriteData(pEmitter->GetName().GetBuffer(10), strlen(pEmitter->GetName().GetBuffer(10)) + 1, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* chunk is done */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* ok */

	return(RETURN_SUCCESS);
}

/* write out effect names as strings in a chunk */

static int WriteEffectListChunk(CObArray& Effects)
{
	int iReturnCode;

	/* anything to do? */

	if (nEffectCount == 0)
		return(RETURN_SUCCESS);

	/* start chunk */

	if ((iReturnCode = StartChunk(DB_EFFECT_LIST, DB_EFFECT_LIST_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nEffectCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out effect names */

	int nCount = 0;
	for (int i = 0; i < Effects.GetSize(); i++)
	{
		/* valid for export? */

		CEffect *pEffect = (CEffect *) Effects.GetAt(i);
		if (pEffect->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;

// now just using name, no need to "define-ify" it
//		/* build name */
//
//		char Label[200];
//		char *pLabel = Label;
//		sprintf(Label, "EFFECT_%s", pEffect->GetName().GetBuffer(10));
//		while (*pLabel != 0)
//		{
//			*pLabel = toupper(*pLabel);
//			if (((*pLabel < '0') || (*pLabel > '9')) && ((*pLabel < 'A') || (*pLabel > 'Z')))
//				*pLabel = '_';
//			pLabel++;
//		}

		/* record in log */

		WriteLogFile("   %d: %s", nCount, pEffect->GetName());
		nCount++;

		/* mark new record list entry */

		RecordListEntry();

		/* write data-- include terminating 0 */

		if (DBLUtil__WriteData(pEffect->GetName().GetBuffer(10), strlen(pEffect->GetName().GetBuffer(10)) + 1, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* chunk is done */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* ok */

	return(RETURN_SUCCESS);
}

/* write out particle names as strings in a chunk */

static int WriteParticleListChunk(CObArray& Particles)
{
	int iReturnCode;

	/* anything to do? */

	if (nParticleCount == 0)
		return(RETURN_SUCCESS);

	/* start chunk */

	if ((iReturnCode = StartChunk(DB_PARTICLE_LIST, DB_PARTICLE_LIST_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nParticleCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out particle names */

	int nCount = 0;
	for (int i = 0; i < Particles.GetSize(); i++)
	{
		/* valid for export? */

		CParticle *pParticle = (CParticle *) Particles.GetAt(i);
		if (pParticle->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;

		/* record in log */

		WriteLogFile("   %d: %s", nCount, pParticle->GetName());
		nCount++;

		/* mark new record list entry */

		RecordListEntry();

		/* write data-- include terminating 0 */

		if (DBLUtil__WriteData(pParticle->GetName().GetBuffer(10), strlen(pParticle->GetName().GetBuffer(10)) + 1, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* chunk is done */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* ok */

	return(RETURN_SUCCESS);
}

/* write out swoosh names as strings in a chunk */

static int WriteSwooshListChunk(CObArray& Swooshes)
{
	int iReturnCode;

	/* anything to do? */

	if (nSwooshCount == 0)
		return(RETURN_SUCCESS);

	/* start chunk */

	if ((iReturnCode = StartChunk(DB_SWOOSH_LIST, DB_SWOOSH_LIST_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nSwooshCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out swoosh names */

	int nCount = 0;
	for (int i = 0; i < Swooshes.GetSize(); i++)
	{
		/* valid for export? */

		CSwoosh *pSwoosh = (CSwoosh *) Swooshes.GetAt(i);
		if (pSwoosh->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;

		/* record in log */

		WriteLogFile("   %d: %s", nCount, pSwoosh->GetName());
		nCount++;

		/* mark new record list entry */

		RecordListEntry();

		/* write data-- include terminating 0 */

		if (DBLUtil__WriteData(pSwoosh->GetName().GetBuffer(10), strlen(pSwoosh->GetName().GetBuffer(10)) + 1, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* chunk is done */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* ok */

	return(RETURN_SUCCESS);
}

/* write out swoosh emitter names as strings in a chunk */

static int WriteSwooshEmitterListChunk(CObArray& SwooshEmitters)
{
	int iReturnCode;

	/* anything to do? */

	if (nSwooshEmitterCount == 0)
		return(RETURN_SUCCESS);

	/* start chunk */

	if ((iReturnCode = StartChunk(DB_SWOOSH_EMITTER_LIST, DB_SWOOSH_EMITTER_LIST_VERSION, 0, "1000")) != RETURN_SUCCESS)
		return(iReturnCode);

	/* start record list-- writes out dummy data, to be replaced later */

	if ((iReturnCode = StartRecordList(nSwooshEmitterCount, RECORD_LIST_HAS_OFFSET_TABLE, 0)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* write out swoosh emitter names */

	int nCount = 0;
	for (int i = 0; i < SwooshEmitters.GetSize(); i++)
	{
		/* valid for export? */

		CPigpenSwooshEmitter *pSwooshEmitter = (CPigpenSwooshEmitter *) SwooshEmitters.GetAt(i);
		if (pSwooshEmitter->GetExportStatus() != EXPORT_STATUS_VALID)
			continue;

		/* record in log */

		WriteLogFile("   %d: %s", nCount, pSwooshEmitter->GetName());
		nCount++;

		/* mark new record list entry */

		RecordListEntry();

		/* write data-- include terminating 0 */

		if (DBLUtil__WriteData(pSwooshEmitter->GetName().GetBuffer(10), strlen(pSwooshEmitter->GetName().GetBuffer(10)) + 1, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_DATA_WRITE_FAILED);

		/* alignment */

		if ((iReturnCode = AlignFile()) != RETURN_SUCCESS)
			return(iReturnCode);
	}

	/* finish record list */

	if ((iReturnCode = FinishRecordList()) != RETURN_SUCCESS)
		return(iReturnCode);

	/* chunk is done */

	if ((iReturnCode = FinishChunk(FALSE)) != RETURN_SUCCESS)
		return(iReturnCode);

	/* ok */

	return(RETURN_SUCCESS);
}

