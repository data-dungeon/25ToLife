/***********************************/
/*                                 */
/* Doc.cpp                         */
/* Pigpen particle tool  12/07/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle tool document defn     */
/*                                 */
/***********************************/

/* a "document" is basically a database that can be loaded, modified, saved. in this case it is basically
a list of effect and particle records */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************* defines *****************************/

/******************* structures **************************/

/* for sorting effect list-- i want to maintain list's previous order as much as possible, so record
pre-sort list index for each record to be used as a tie-breaker */

typedef struct
{
	CObject *pObject;
	unsigned long ulListIndex;
}
ts_SortRecord;

/******************* global variables ********************/

/* for letting CDocView constructor know what kind of view we want */

int nRequestedViewType = EFFECTS;	// default

/******************* local variables *********************/

/* a common UpdateAllViews hint-- only change current view */

int nNoChangeOtherViews[MAX_DOC_DATA_TYPES] = {
		NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW  | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW,	// effects
		NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW   | NO_CHANGE_GRAPHICS_VIEW  | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW,	// particles
		NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW   | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW,	// graphics
		NO_CHANGE_EFFECTS_VIEW  | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW  | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW,	// emitters
		NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW   | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW,	// swooshes
		NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW   | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW,			// swoosh emitters
};

int nChannelIndex;	// for use by EffectParticleCompare sort routine

/******************* externs *****************************/

/******************* class functions *********************/

/////////////////////////////////////////////////////////////////////////////
// CPigpenDoc

IMPLEMENT_DYNCREATE(CPigpenDoc, CDocument)

BEGIN_MESSAGE_MAP(CPigpenDoc, CDocument)
	//{{AFX_MSG_MAP(CPigpenDoc)
	ON_COMMAND(ID_VIEW_EFFECTS, OnViewEffects)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EFFECTS, OnUpdateViewEffects)
	ON_COMMAND(ID_VIEW_PARTICLES, OnViewParticles)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PARTICLES, OnUpdateViewParticles)
	ON_COMMAND(ID_VIEW_GRAPHICS, OnViewGraphics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRAPHICS, OnUpdateViewGraphics)
	ON_COMMAND(ID_VIEW_EMITTERS, OnViewEmitters)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EMITTERS, OnUpdateViewEmitters)
	ON_COMMAND(ID_VIEW_SWOOSHES, OnViewSwooshes)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SWOOSHES, OnUpdateViewSwooshes)
	ON_COMMAND(ID_VIEW_SWOOSH_EMITTERS, OnViewSwooshEmitters)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SWOOSH_EMITTERS, OnUpdateViewSwooshEmitters)
	ON_COMMAND(ID_VIEW_EXPORT_LOG, OnViewExportLog)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPORT_LOG, OnUpdateViewExportLog)
	ON_COMMAND(ID_FILE_EXPORT_PC, OnFileExportPC)
	ON_COMMAND(ID_FILE_EXPORT_PS2, OnFileExportPS2)
	ON_COMMAND(ID_FILE_EXPORT_GCN, OnFileExportGCN)
	ON_COMMAND(ID_FILE_ANALYZE, OnFileAnalyze)
	ON_COMMAND(ID_TEXTURE_ANALYZE, OnTextureAnalyze)
	ON_COMMAND(ID_FILE_MERGE, OnFileMerge)
	ON_COMMAND(ID_FILE_SETASSOCIATELOGFILE, OnSetAssociateLogFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPigpenDoc construction/destruction

CPigpenDoc::CPigpenDoc()
{
	/* versions */

	m_ulPigpenVersion = PIGPEN_VERSION;
	m_ulDocVersion = DOC_VERSION;

	/* no open views yet */

	for (int i = 0; i < MAX_DOC_DATA_TYPES; i++)
		m_pView[i] = NULL;

	/* no preview yet (this is used for updating particle behavior in real-time in particle preview) */

	m_pParticleSource = NULL;

	/* collision surface type list. index is currently selected item in particle collide dialog */
	/* surface type combo box. currently, all pre-defined surface types are active by definition-- */
	/* they can't be turned off, though no effect has to be specified */

	m_nSurfaceTypeIndex = 0;

	CSurface *pSurface;
	pSurface = new CSurface;
	pSurface->SetName("End of life");
	pSurface->SetFlags(SURFACE_PREDEFINED | SURFACE_END_OF_LIFE);
	AddSurface(pSurface);

	pSurface = new CSurface;
	pSurface->SetName("End of life underwater");
	pSurface->SetFlags(SURFACE_PREDEFINED | SURFACE_END_OF_LIFE);
	AddSurface(pSurface);

	pSurface = new CSurface;
	pSurface->SetName("Default collision");
	pSurface->SetFlags(SURFACE_PREDEFINED | SURFACE_DEFAULT_COLLISION);
	int nDefaultCollisionSurfaceIndex = GetSurfaceCount();
	AddSurface(pSurface);

	m_nPredefinedSurfaceCount = GetSurfaceCount();

	/* this is a bit of hack, but we insist that the end-of-life surfaces are the first ones in the predefined set */

	m_nEndOfLifeSurfaceCount = 0;
	while ((m_nEndOfLifeSurfaceCount < GetSurfaceCount()) && (GetSurface(m_nEndOfLifeSurfaceCount)->EndOfLife()))
		m_nEndOfLifeSurfaceCount++;
#ifdef _DEBUG
	for (i = m_nEndOfLifeSurfaceCount; i < GetSurfaceCount(); i++)
		ASSERT(GetSurface(i)->EndOfLife() == false);	// if this hits, end-of-life surfaces are not all grouoed at start of predefined surfaces
#endif //_DEBUG

	/* let particle constructor know about surface type count. i don't know a more elegant way to do this */

/*** ned says 4/13/05: this is BROKEN! each window goes through this code, and sets up a surface type list for itself. only the particle ***/
/*** view serializes the surface types. so if there are extra surface types, the particle view serializes them, and the static routine ***/
/*** CParticle::SetSurfaceCount gets called, and the clipboard record size gets set up (m_nClipboardRecordSize, set in CDocView::OnInitialUpdate) ***/
/*** but then another view is initialized and resets the surface type list to default ("end of life", etc), and CParticle::SetSurfaceCount ***/
/*** gets called again with the default list length. at this point, copying particles is broken because the record size calculated in ***/
/*** CDocView::DoCopy does not match the allocated record size. i guess that only the particle view should have a surface list, or maybe it ***/
/*** should be somewhere else entirely (what happens if you close the particle view and then re-open it? does the surface list get lost???). ***/
/*** as it is, there's no easy fix i can see because we still don't know what type of view this document is tied to (that comes later, with ***/
/*** the setting of nRequestedViewType in CPigpenDoc::Serialize and CPigpenDoc::OpenLoadedWindows). also, the surface type isn't correctly ***/
/*** implemented in the game engine anyway. so for now the solution is to not add new surface types. sorry. ***/
	CParticle::SetSurfaceCount(GetSurfaceCount());
	CParticle::SetPredefinedSurfaceCount(GetSurfaceCount());
	CParticle::SetEndOfLifeSurfaceCount(GetEndOfLifeSurfaceCount());
	CParticle::SetDefaultCollisionSurfaceIndex(nDefaultCollisionSurfaceIndex);

	/* preview prefs */

	m_StageFile = PigpenApp.GetDefaultStageFile();
	m_bDefaultStageFile = TRUE;
	m_EmitterModelFile = "";
	m_fEmitterModelXPos = 0.0f;
	m_fEmitterModelYPos = 0.0f;
	m_fEmitterModelZPos = 0.0f;
	m_fEmitterModelYRot = 0.0f;
	m_BackgroundColor = 0x808080;
	m_fInitCameraPosX = DEFAULT_CAMERA_POS_X;
	m_fInitCameraPosY = DEFAULT_CAMERA_POS_Y;
	m_fInitCameraPosZ = DEFAULT_CAMERA_POS_Z;
	m_fInitCameraRotX = 0.0f;
	m_fInitCameraRotY = 0.0f;

	/* no surface translation table */

	m_pnSurfaceTranslation = NULL;

	/* no associate log */

	m_pAssociateLog = NULL;
}

CPigpenDoc::~CPigpenDoc()
{
	/* free surface translation table */

	if (m_pnSurfaceTranslation)
		free(m_pnSurfaceTranslation);

	/* let preview know that doc has closed */

	Preview__DocClosed(this);
}

BOOL CPigpenDoc::OnNewDocument()
{
	m_ulPigpenVersion = PIGPEN_VERSION;
	m_ulDocVersion = DOC_VERSION;

//yuch-- this doesn't before CDocView::CDocView(), so m_nType has already sucked out contents of nRequestedViewType
//	/* effects view is default */
//
//	nRequestedViewType = EFFECTS;	// let CDocView constructor know what we want

	/* initialize list sizes */

	for (int i = 0; i < MAX_DOC_DATA_TYPES; i++)
		m_Arrays[i].SetSize(0, 20);

	/* let base class fire */

	if (!CDocument::OnNewDocument())
		return FALSE;

	/* done */

	return TRUE;
}

BOOL CPigpenDoc::OnOpenDocument( LPCTSTR lpszPathName ) 
{
	/* record doc being opened */

	PigpenApp.SetOpenedDoc(this);

	/* let base class fire */

	return(CDocument::OnOpenDocument(lpszPathName));
}

void CPigpenDoc::OnCloseDocument() 
{
	/* delete data */

	for (int i = 0; i < MAX_DOC_DATA_TYPES; i++)
		DeleteArray(m_Arrays[i]);

	/* let base class fire */

	CDocument::OnCloseDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CPigpenDoc serialization

void CPigpenDoc::Serialize(CArchive& ar)
{
	int i;

	/* get position of app window's client area for saving window locations */

//yuch-- this should work, but it ends up returning the top of the tool bar, not the empty client area
//	CPoint AppClientTopLeft(0, 0);
//	PigpenApp.m_pMainWnd->ClientToScreen(&AppClientTopLeft);
	CRect AppRect;
	PigpenApp.m_pMainWnd->GetWindowRect(&AppRect);
	CPoint AppClientTopLeft;
	AppClientTopLeft.x = AppRect.left + 6;	// left of client area seems to be 6 pixels in from left edge
	AppClientTopLeft.y = AppRect.top + 68;	// top of client area seems to be 66 pixels in from top edge

	/* serialize */

	if (ar.IsStoring())
	{
		/* translate pointers to indices in effect channel records */

		for (i = 0; i < GetEffectCount(); i++)
		{
			CEffect *pEffect = GetEffect(i);

			/* particle for each channel */

			for (int j = 0; j < MAX_CHANNELS_PER_EFFECT; j++)
			{
				pEffect->m_Channels[j].nParticleIndex = -1;
				CParticle *pParticle = pEffect->GetChannelParticle(j);
				if (pParticle)
				{
					for (int k = 0; k < GetParticleCount(); k++)
					{
						if (pParticle == GetParticle(k))	//yuch-- faster to pre-build an array of pointers?
						{
							pEffect->m_Channels[j].nParticleIndex = k;
							break;
						}
					}
					ASSERT(pEffect->m_Channels[j].nParticleIndex != -1);	// must find particle
				}
			}

			/* record status of record in list */

			int nState = 0;
			if (m_pView[EFFECTS])
				nState = m_pView[EFFECTS]->GetListCtrl().GetItemState(i, LVIS_SELECTED);
			pEffect->SetSelected((nState & LVIS_SELECTED) != 0);
		}

		/* translate pointers to indices in particle records */

		for (i = 0; i < GetParticleCount(); i++)
		{
			CParticle *pParticle = GetParticle(i);

			/* graphic */

			pParticle->SetGraphicIndex(-1);
			if (pParticle->GetGraphic() != NULL)
			{
				for (int j = 0; j < GetGraphicCount(); j++)
				{
					if (pParticle->GetGraphic() == GetGraphic(j))	//yuch-- faster to pre-build an array of pointers?
					{
						pParticle->SetGraphicIndex(j);
						break;
					}
				}
				ASSERT(pParticle->GetGraphicIndex() != -1);	// must find it
			}

			/* piggyback emitter */

			pParticle->SetPiggybackEmitterIndex(-1);
			if (pParticle->GetPiggybackEmitter() != NULL)
			{
				for (int j = 0; j < GetEmitterCount(); j++)
				{
					if (pParticle->GetPiggybackEmitter() == GetEmitter(j))	//yuch-- faster to pre-build an array of pointers?
					{
						pParticle->SetPiggybackEmitterIndex(j);
						break;
					}
				}
				ASSERT(pParticle->GetPiggybackEmitterIndex() != -1);	// must find it
			}

			/* spawn effect */

			for (int j = 0; j < pParticle->GetSpawnCount(); j++)
			{
				CSpawn *pSpawn = pParticle->GetSpawn(j);
				pSpawn->SetEffectIndex(-1);
				if (pSpawn->GetEffect())
				{
					for (int k = 0; k < GetEffectCount(); k++)
					{
						if (pSpawn->GetEffect() == GetEffect(k))	//yuch-- faster to pre-build an array of pointers?
						{
							pSpawn->SetEffectIndex(k);
							break;
						}
					}
					ASSERT(pSpawn->GetEffectIndex() != -1);	// must find it
				}
			}

			/* record status of record in list */

			int nState = 0;
			if (m_pView[PARTICLES])
				nState = m_pView[PARTICLES]->GetListCtrl().GetItemState(i, LVIS_SELECTED);
			pParticle->SetSelected((nState & LVIS_SELECTED) != 0);
		}

		/* translate pointers to indices in emitter records */

		for (i = 0; i < GetEmitterCount(); i++)
		{
			CEmitter *pEmitter = GetEmitter(i);
			pEmitter->SetEffectIndex(-1);
			if (pEmitter->GetEffect() != NULL)
			{
				for (int j = 0; j < GetEffectCount(); j++)
				{
					if (pEmitter->GetEffect() == GetEffect(j))	//yuch-- faster to pre-build an array of pointers?
					{
						pEmitter->SetEffectIndex(j);
						break;
					}
				}
				ASSERT(pEmitter->GetEffectIndex() != -1);	// must find it
			}

			/* record status of record in list */

			int nState = 0;
			if (m_pView[EMITTERS])
				nState = m_pView[EMITTERS]->GetListCtrl().GetItemState(i, LVIS_SELECTED);
			pEmitter->SetSelected((nState & LVIS_SELECTED) != 0);
		}

		/* process graphics view */

		for (i = 0; i < GetGraphicCount(); i++)
		{
			CGraphic *pGraphic = GetGraphic(i);

			/* record status of record in list */

			int nState = 0;
			if (m_pView[GRAPHICS])
				nState = m_pView[GRAPHICS]->GetListCtrl().GetItemState(i, LVIS_SELECTED);
			pGraphic->SetSelected((nState & LVIS_SELECTED) != 0);
		}

		/* translate pointers to indices in swoosh records */

		for (i = 0; i < GetSwooshCount(); i++)
		{
			CSwoosh *pSwoosh = GetSwoosh(i);

			/* graphic */

			pSwoosh->SetGraphicIndex(-1);
			if (pSwoosh->GetGraphic() != NULL)
			{
				for (int j = 0; j < GetGraphicCount(); j++)
				{
					if (pSwoosh->GetGraphic() == GetGraphic(j))	//yuch-- faster to pre-build an array of pointers?
					{
						pSwoosh->SetGraphicIndex(j);
						break;
					}
				}
				ASSERT(pSwoosh->GetGraphicIndex() != -1);	// must find it
			}

			/* record status of record in list */

			int nState = 0;
			if (m_pView[SWOOSHES])
				nState = m_pView[SWOOSHES]->GetListCtrl().GetItemState(i, LVIS_SELECTED);
			pSwoosh->SetSelected((nState & LVIS_SELECTED) != 0);
		}

		/* translate pointers to indices in swoosh emitter records */

		for (i = 0; i < GetSwooshEmitterCount(); i++)
		{
			CPigpenSwooshEmitter *pSwooshEmitter = GetSwooshEmitter(i);
			pSwooshEmitter->SetSwooshIndex(-1);
			if (pSwooshEmitter->GetSwoosh() != NULL)
			{
				for (int j = 0; j < GetSwooshCount(); j++)
				{
					if (pSwooshEmitter->GetSwoosh() == GetSwoosh(j))	//yuch-- faster to pre-build an array of pointers?
					{
						pSwooshEmitter->SetSwooshIndex(j);
						break;
					}
				}
				ASSERT(pSwooshEmitter->GetSwooshIndex() != -1);	// must find it
			}

			/* record status of record in list */

			int nState = 0;
			if (m_pView[SWOOSH_EMITTERS])
				nState = m_pView[SWOOSH_EMITTERS]->GetListCtrl().GetItemState(i, LVIS_SELECTED);
			pSwooshEmitter->SetSelected((nState & LVIS_SELECTED) != 0);
		}

		/* human-readable string */

		CString String;
		String.Format("Pigpen V%d (prefs V%d, effect V%d, particle V%d, graphic V%d, image V%d, surface V%d, swoosh V%d, swoosh emitter V%d)",
						  PIGPEN_VERSION, DOC_VERSION, EFFECT_VERSION, PARTICLE_VERSION, GRAPHIC_VERSION, IMAGE_VERSION, SURFACE_VERSION, SWOOSH_VERSION, SWOOSH_EMITTER_VERSION);
		ar << String;

		/* overall version id */

		ASSERT(m_ulPigpenVersion == PIGPEN_VERSION);
		ar << m_ulPigpenVersion;

		/* doc version */

		ASSERT(m_ulDocVersion == DOC_VERSION);
		ar << m_ulDocVersion;

		/* doc version 3-- collision surface types */

		ar << GetSurfaceCount();	// number of surface types stored
		for (i = 0; i < GetSurfaceCount(); i++)
			GetSurface(i)->Serialize(ar);
 
		/* doc version 2-- preview prefs */

		ar << m_StageFile;
		ar << m_bDefaultStageFile;
		ar << m_EmitterModelFile;
		ar << m_fEmitterModelXPos;
		ar << m_fEmitterModelYPos;
		ar << m_fEmitterModelZPos;
		ar << m_fEmitterModelYRot;
		ar << m_BackgroundColor;
		ar << m_fInitCameraPosX;
		ar << m_fInitCameraPosY;
		ar << m_fInitCameraPosZ;
		ar << m_fInitCameraRotX;
		ar << m_fInitCameraRotY;

		/* doc version 1-- layout and data */

//traverse z-order with GetTopWindow, GetNextWindow, then restore at load
// i thought this might work, but it looks like the different views aren't siblings? so GetNextWindow(GW_HWNDPREV)
// returns NULL the first time, so i can't tell which window is on top of which
//	   POSITION pos = GetFirstViewPosition();
//		CView* pView = GetNextView(pos);
//		CWnd *pWnd = (CWnd *) pView;
//		while (pWnd->GetNextWindow(GW_HWNDPREV))
//			pWnd = pWnd->GetNextWindow(GW_HWNDPREV);
//		int n = 0;
//		while (pWnd)
//		{
//			if (pWnd == (CWnd *) m_pView[EFFECTS])
//				n = 0;
//			if (pWnd == (CWnd *) m_pView[PARTICLES])
//				n = 1;
//			if (pWnd == (CWnd *) m_pView[GRAPHICS])
//				n = 2;
//			pWnd = pWnd->GetNextWindow();
//		}

		/* record open window layout */

		unsigned long ulDocDataTypes = MAX_DOC_DATA_TYPES;
		ar << ulDocDataTypes;
		for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
		{
			int nViewStatus;
			int nEditWindowCount;
			CRect Rect;
			nViewStatus = (m_pView[i] != NULL);
			ar << nViewStatus;
			if (nViewStatus)
			{
				/* view window open, record position */

				m_pView[i]->GetParent()->GetWindowRect(&Rect);	// parent "frame" of list view is really the window i want

				/* we want it relative to app window's client area (excluding frame, tool bar, etc) */

				Rect.left -= AppClientTopLeft.x;
				Rect.right -= AppClientTopLeft.x;
				Rect.top -= AppClientTopLeft.y;
				Rect.bottom -= AppClientTopLeft.y;
				ar << Rect;

				/* record scroll info */

				SCROLLINFO ScrollInfo;
				memset(&ScrollInfo, 0, sizeof(SCROLLINFO));
				m_pView[i]->GetScrollInfo(SB_HORZ, &ScrollInfo, SIF_ALL);
				ar.Write(&ScrollInfo, sizeof(SCROLLINFO));
				memset(&ScrollInfo, 0, sizeof(SCROLLINFO));
				m_pView[i]->GetScrollInfo(SB_VERT, &ScrollInfo, SIF_ALL);
				ar.Write(&ScrollInfo, sizeof(SCROLLINFO));

				/* edit windows open for this view? */

				nEditWindowCount = m_pView[i]->EditWindowOpenCount();
				ar << nEditWindowCount;
				for (int j = 0; j < MAX_EDIT_WINDOWS_PER_VIEW; j++)
				{
					if (m_pView[i]->GetEditWindow(j))
					{
						/* edit window open, record position */

						ar << j;
						m_pView[i]->GetEditWindow(j)->GetWindowRect(&Rect);

						/* modeless dialogs can be moved all over the screen-- they are not restricted */
						/* to the app's window. but when we re-open the file we still want to position */
						/* the edit window relative to the app's window */

						Rect.left -= AppClientTopLeft.x;
						Rect.right -= AppClientTopLeft.x;
						Rect.top -= AppClientTopLeft.y;
						Rect.bottom -= AppClientTopLeft.y;
						ar << Rect;
					}
				}
			}
		}
	
		/* doc data (each CObArray serializes itself) */

		for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
			m_Arrays[i].Serialize(ar);
	}
	else
	{
		/* we are loading a file */

		CRect WindowRect[MAX_DOC_DATA_TYPES];

		/* no views open */

		for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
			m_nViewOpen[i] = FALSE;

		/* human readable string. make sure this is a pigpen file */

		CString String;
		ar >> String;
		if (String.Left(6) != "Pigpen")
		{
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_PIG_FILE);
			else
				AfxMessageBox("This is not a Pigpen file.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* overall version id-- pretty useless */

		ar >> m_ulPigpenVersion;
		if (m_ulPigpenVersion > PIGPEN_VERSION)
		{
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("This executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* load doc contents. as we add new versions, process latest version and then fall through to earlier versions */

		ar >> m_ulDocVersion;
		if (m_ulDocVersion > DOC_VERSION)
		{
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("This executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}
		switch(m_ulDocVersion)
		{
		case 3:
			/* prefs version 3-- surface type list. pre-defined surface types are not saved out-- they are */
			/* created in constructor */

			ASSERT(GetSurfaceCount() == GetPredefinedSurfaceCount());	// should only be pre-defined surfaces in list so far
			int nSurfaceCount;
			ar >> nSurfaceCount;

			/* set up a translation table for surface types. i'm spending more time on this than i should, but */
			/* the goal is to be able to add a new pre-defined type without messing anything up. at file save */
			/* time, each particle has an array of spawn effects that matches the number of surface types. now */
			/* say i want to add a new pre-defined surface. this will end up meaning that the surface type */
			/* table will be one entry bigger than the one in the file being loaded, so i have to be able to */
			/* re-order the incoming spawn records. note that the surface type table can only grow, never */
			/* shrink-- it will always be at least as big as the surface type table in the file being loaded. */
			/* the call to CParticle::SetSurfaceCount below will make it so the CParticle constructor creates */
			/* <surface type count> spawn records per particle. this may be larger than the number of spawn */
			/* records contained in each particle in the file being loaded */

			if (m_pnSurfaceTranslation)
				free(m_pnSurfaceTranslation);
			m_pnSurfaceTranslation = (int *) malloc(nSurfaceCount * sizeof(int));

			/* load each surface type from disk file */

			for (i = 0; i < nSurfaceCount; i++)
			{
				CSurface *pSurface = new CSurface;
				pSurface->Serialize(ar);

				/* if this surface type exists in list, then ignore it-- translation table will take care */
				/* of making sure that spawn effects get into the correct spot */

				int nIndex = SurfaceExists(pSurface->GetName());
				if (nIndex == -1)
				{
					m_pnSurfaceTranslation[i] = m_SurfaceTypes.GetSize();
					AddSurface(pSurface);
				}
				else
				{
					m_pnSurfaceTranslation[i] = nIndex;	// index of already-existing surface type
					delete pSurface;
				}
			}

			/* let particle constructor know about surface type count. i don't know a more elegant way to do this */

			CParticle::SetSurfaceCount(GetSurfaceCount());

			/* let particle constructor know about surface translation table */

			CParticle::SetSurfaceTranslation(m_pnSurfaceTranslation);

			/* fall through */
		case 2:
			/* prefs version 2-- preview prefs */

			ar >> m_StageFile;
			ar >> m_bDefaultStageFile;
			if (m_bDefaultStageFile)
				m_StageFile = PigpenApp.GetDefaultStageFile();	// ignore archived name, use file set by app at execution start-- different machines may have pigpen executable in different places
			ar >> m_EmitterModelFile;
			ar >> m_fEmitterModelXPos;
			ar >> m_fEmitterModelYPos;
			ar >> m_fEmitterModelZPos;
			ar >> m_fEmitterModelYRot;
			ar >> m_BackgroundColor;
			ar >> m_fInitCameraPosX;
			ar >> m_fInitCameraPosY;
			ar >> m_fInitCameraPosZ;
			ar >> m_fInitCameraRotX;
			ar >> m_fInitCameraRotY;

			/* fall through */
		case 1:
			/* version 1 */

			unsigned long ulDocDataTypes;
			ar >> ulDocDataTypes;
			for (i = 0; i < (signed) ulDocDataTypes; i++)
			{
				ar >> m_nViewOpen[i];
				if (m_nViewOpen[i])
				{
					ar >> m_ViewRect[i];

					/* don't let them off-screen-- this is basically a panic mode for running the same */
					/* pigpen file in different resolutions */

					CRect ClientRect;
					PigpenApp.m_pMainWnd->GetClientRect(&ClientRect);
					if (m_ViewRect[i].left >= ClientRect.right)	// off right of window
						m_ViewRect[i].left = ClientRect.right / 8;
					if (m_ViewRect[i].right - m_ViewRect[i].left > ClientRect.right)	// too wide
						m_ViewRect[i].right = m_ViewRect[i].left + (ClientRect.right * 3 / 4);
					if (m_ViewRect[i].top >= ClientRect.bottom)	// off bottom of window
						m_ViewRect[i].top = ClientRect.bottom / 8;
					if (m_ViewRect[i].bottom - m_ViewRect[i].top > ClientRect.bottom)	// too tall
						m_ViewRect[i].bottom = m_ViewRect[i].top + (ClientRect.bottom * 3 / 4);

					/* load scroll info */

					ar.Read(&m_ViewHorzScroll[i], sizeof(SCROLLINFO));
					ar.Read(&m_ViewVertScroll[i], sizeof(SCROLLINFO));

					/* any edit window open for this view? */

					int nEditWindowCount;
					int nIndex;
					memset(m_nEditOpen[i], 0, MAX_EDIT_WINDOWS_PER_VIEW * sizeof(int));	// clear edit window status
					ar >> nEditWindowCount;
					for (int j = 0; j < nEditWindowCount; j++)
					{
						/* window index */

						ar >> nIndex;
						m_nEditOpen[i][nIndex] = TRUE;
						ar >> m_EditRect[i][nIndex];

						/* modeless dialogs can be moved all over the screen-- they are not restricted to the app's */
						/* window. but when we re-open the file we still want to position the edit window relative to */
						/* the app's window */

						m_EditRect[i][nIndex].left += AppClientTopLeft.x;
						m_EditRect[i][nIndex].right += AppClientTopLeft.x;
						m_EditRect[i][nIndex].top += AppClientTopLeft.y;
						m_EditRect[i][nIndex].bottom += AppClientTopLeft.y;

						/* modeless dialogs should be on-screen */

						if (m_EditRect[i][nIndex].left < 0)	// off left of screen 
							m_EditRect[i][nIndex].left = 0;
						if (m_EditRect[i][nIndex].top < 0)	// off top of screen
							m_EditRect[i][nIndex].top = 0;

						/* get size of screen */

						CRect DesktopRect;
						PigpenApp.m_pMainWnd->GetDesktopWindow()->GetWindowRect(&DesktopRect);
						if (m_EditRect[i][nIndex].left > DesktopRect.right)	// off right of screen 
							m_EditRect[i][nIndex].left = DesktopRect.right - 100;
						if (m_EditRect[i][nIndex].top > DesktopRect.bottom)	// off bottom of screen 
							m_EditRect[i][nIndex].top = DesktopRect.bottom - 100;
					}
				}
			}

			/* load doc data */

			for (i = 0; i < (signed) ulDocDataTypes; i++)
				m_Arrays[i].Serialize(ar);
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* translate indices to pointers in effect channel records */

		for (i = 0; i < GetEffectCount(); i++)
		{
			CEffect *pEffect = GetEffect(i);

			/* particle for each channel */

			for (int j = 0; j < MAX_CHANNELS_PER_EFFECT; j++)
			{
				if (pEffect->m_Channels[j].nParticleIndex == -1)
					pEffect->m_Channels[j].pParticle = NULL;
				else
					pEffect->m_Channels[j].pParticle = GetParticle(pEffect->m_Channels[j].nParticleIndex);
			}
		}

		/* translate indices to pointers in particle records */

		for (i = 0; i < GetParticleCount(); i++)
		{
			CParticle *pParticle = GetParticle(i);

			/* graphic */

			if (pParticle->GetGraphicIndex() == -1)
				pParticle->SetGraphic(NULL);
			else
				pParticle->SetGraphic(GetGraphic(pParticle->GetGraphicIndex()));

			/* piggyback emitter */

			if (pParticle->GetPiggybackEmitterIndex() == -1)
				pParticle->SetPiggybackEmitter(NULL);
			else
				pParticle->SetPiggybackEmitter(GetEmitter(pParticle->GetPiggybackEmitterIndex()));

			/* spawn effect */

			for (int j = 0; j < pParticle->GetSpawnCount(); j++)
			{
				CSpawn *pSpawn = pParticle->GetSpawn(j);
				if (pSpawn->GetEffectIndex() == -1)
					pSpawn->SetEffect(NULL);
				else
					pSpawn->SetEffect(GetEffect(pSpawn->GetEffectIndex()));
			}
		}

		/* translate indices to pointers in emitter records */

		for (i = 0; i < GetEmitterCount(); i++)
		{
			CEmitter *pEmitter = GetEmitter(i);
			if (pEmitter->GetEffectIndex() == -1)
				pEmitter->SetEffect(NULL);
			else
				pEmitter->SetEffect(GetEffect(pEmitter->GetEffectIndex()));
		}

		/* translate indices to pointers in swoosh records */

		for (i = 0; i < GetSwooshCount(); i++)
		{
			CSwoosh *pSwoosh = GetSwoosh(i);

			/* graphic */

			if (pSwoosh->GetGraphicIndex() == -1)
				pSwoosh->SetGraphic(NULL);
			else
				pSwoosh->SetGraphic(GetGraphic(pSwoosh->GetGraphicIndex()));
		}

		/* translate indices to pointers in swoosh emitter records */

		for (i = 0; i < GetSwooshEmitterCount(); i++)
		{
			CPigpenSwooshEmitter *pSwooshEmitter = GetSwooshEmitter(i);
			if (pSwooshEmitter->GetSwooshIndex() == -1)
				pSwooshEmitter->SetSwoosh(NULL);
			else
				pSwooshEmitter->SetSwoosh(GetSwoosh(pSwooshEmitter->GetSwooshIndex()));
		}

		/* possible fix-ups for old versions of data */

		for (i = 0; i < GetGraphicCount(); i++)
			GetGraphic(i)->LoadFixup();
		for (i = 0; i < GetParticleCount(); i++)
			GetParticle(i)->LoadFixup();
		for (i = 0; i < GetEffectCount(); i++)
			GetEffect(i)->LoadFixup();
		for (i = 0; i < GetEmitterCount(); i++)
			GetEmitter(i)->LoadFixup();
		for (i = 0; i < GetSwooshCount(); i++)
			GetSwoosh(i)->LoadFixup();
		for (i = 0; i < GetSwooshEmitterCount(); i++)
			GetSwooshEmitter(i)->LoadFixup();

		/* one view window is always opened automatically-- set up for it. rest of windows will be opened later. */
		/* window locations are ignored for merging */

		for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
		{
			if (m_nViewOpen[i])
			{
				nRequestedViewType = i;	// type of view to open
				PigpenApp.SetRestoreWindow(TRUE);
				PigpenApp.SetRestoreWindowRect(m_ViewRect[i]);
				PigpenApp.SetRestoreWindowHorzScroll(&m_ViewHorzScroll[i]);
				PigpenApp.SetRestoreWindowVertScroll(&m_ViewVertScroll[i]);
				m_nViewOpen[i] = FALSE;	// will be opened automatically, so don't worry about it in OpenLoadedWindows
				break;
			}
		}
		ASSERT(i != MAX_DOC_DATA_TYPES);	// must always be a view window open

		/* now using latest version */

		m_ulPigpenVersion = PIGPEN_VERSION;
		m_ulDocVersion = DOC_VERSION;
	}
}

/* open windows after file load. this is called by app after a file is loaded. the serialization routine records all
open windows. the first is loaded by the default doc open code */

void CPigpenDoc::OpenLoadedWindows(void)
{
	int i;
	int j;

	for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
	{
		if (m_nViewOpen[i])
		{
			nRequestedViewType = i;	// type of view to open
			PigpenApp.SetRestoreWindow(TRUE);
			PigpenApp.SetRestoreWindowRect(m_ViewRect[i]);
			PigpenApp.SetRestoreWindowHorzScroll(&m_ViewHorzScroll[i]);
			PigpenApp.SetRestoreWindowVertScroll(&m_ViewVertScroll[i]);
			ASSERT(m_pView[i] == NULL);	// shouldn't be open yet
			OnView(i);
		}
	}

	/* set selected items in view */

	for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
	{
		if (m_pView[i])
		{
			CListCtrl& List = m_pView[i]->GetListCtrl();	// get CListCtrl associated with this view
			bool bSelected;
			for (j = 0; j < List.GetItemCount(); j++)
			{
				switch (i)
				{
				case EFFECTS:
					bSelected = GetEffect(j)->GetSelected();
					break;
				case PARTICLES:
					bSelected = GetParticle(j)->GetSelected();
					break;
				case GRAPHICS:
					bSelected = GetGraphic(j)->GetSelected();
					break;
				case EMITTERS:
					bSelected = GetEmitter(j)->GetSelected();
					break;
				case SWOOSHES:
					bSelected = GetSwoosh(j)->GetSelected();
					break;
				case SWOOSH_EMITTERS:
					bSelected = GetSwooshEmitter(j)->GetSelected();
					break;
				}
				List.SetItemState(j, bSelected ? LVIS_SELECTED : 0, LVIS_SELECTED);
			}
		}
	}

	/* now go through and open edit windows if appropriate */

	for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
	{
		if (m_pView[i])
		{
			for (j = 0; j < MAX_EDIT_WINDOWS_PER_VIEW; j++)
			{
				if (m_nEditOpen[i][j])
				{
					PigpenApp.SetRestoreWindow(TRUE);
					PigpenApp.SetRestoreWindowRect(m_EditRect[i][j]);
					ASSERT(m_pView[i]->GetEditWindow(j) == NULL);	// shouldn't be open yet
					m_pView[i]->OnEdit(j);
				}
			}
		}
	}
}

/* check if a surface type name is already in the list */

int CPigpenDoc::SurfaceExists(CString &Name)
{
	/* look through existing surface types */

	for (int i = 0; i < GetSurfaceCount(); i++)
	{
		if (stricmp(GetSurface(i)->GetName().GetBuffer(10), Name.GetBuffer(10)) == 0)
			return(i);
	}

	/* not found */

	return(-1);
}

/* add/delete a surface to/from list */

void CPigpenDoc::AddSurface(CSurface *pSurface)
{
	m_SurfaceTypes.Add(pSurface);

	/* now add a spawn entry to all particles */

	for (int i = 0; i < GetParticleCount(); i++)
		GetParticle(i)->AddNewSpawn();

	/* and let particles know about new size */

	CParticle::SetSurfaceCount(GetSurfaceCount());
}
void CPigpenDoc::DeleteSurface()
{
	m_SurfaceTypes.RemoveAt(m_nSurfaceTypeIndex);

	/* now remove corresponding spawn entry from all particles */

	for (int i = 0; i < GetParticleCount(); i++)
		GetParticle(i)->DeleteSpawn(m_nSurfaceTypeIndex);

	/* make sure index is valid */

	if (m_nSurfaceTypeIndex >= GetSurfaceCount())
		m_nSurfaceTypeIndex = GetSurfaceCount() - 1;

	/* and let particles know about new size */

	CParticle::SetSurfaceCount(GetSurfaceCount());
}

/* preview prefs support */

void CPigpenDoc::SetStageFile(CString &File)
{
	if (m_StageFile != File)
	{
		m_StageFile = File;
		m_bDefaultStageFile = (m_StageFile.CompareNoCase(PigpenApp.GetDefaultStageFile()) == 0);
		SetModifiedFlag(MODIFIED_FLAG);
	}
}
void CPigpenDoc::SetCameraPos(Vector3CRef vCameraPos, Vector3CRef vCameraRot)
{
	if (m_fInitCameraPosX != vCameraPos.x())
	{
		m_fInitCameraPosX = vCameraPos.x();
		SetModifiedFlag(MODIFIED_FLAG);
	}
	if (m_fInitCameraPosY != vCameraPos.y())
	{
		m_fInitCameraPosY = vCameraPos.y();
		SetModifiedFlag(MODIFIED_FLAG);
	}
	if (m_fInitCameraPosZ != vCameraPos.z())
	{
		m_fInitCameraPosZ = vCameraPos.z();
		SetModifiedFlag(MODIFIED_FLAG);
	}
	if (m_fInitCameraRotX != vCameraRot.x())
	{
		m_fInitCameraRotX = vCameraRot.x();
		SetModifiedFlag(MODIFIED_FLAG);
	}
	if (m_fInitCameraRotY != vCameraRot.y())
	{
		m_fInitCameraRotY = vCameraRot.y();
		SetModifiedFlag(MODIFIED_FLAG);
	}
}
void CPigpenDoc::GetCameraPos(Vector3 *pvCameraPos, Vector3 *pvCameraRot)
{
	pvCameraPos->Set(m_fInitCameraPosX, m_fInitCameraPosY, m_fInitCameraPosZ);
	pvCameraRot->x(m_fInitCameraRotX);
	pvCameraRot->y(m_fInitCameraRotY);
}

/////////////////////////////////////////////////////////////////////////////
// CPigpenDoc diagnostics

#ifdef _DEBUG
void CPigpenDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPigpenDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPigpenDoc commands

/************************** qsort compare functions  *******************************/

int EffectNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CEffect *pEffect1 = (CEffect *) (pSort1->pObject);
	CEffect *pEffect2 = (CEffect *) (pSort2->pObject);
	int nCompare = pEffect1->GetName().CompareNoCase(pEffect2->GetName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(FALSE);	// names should be unique!
	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int EffectAssociatedCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CEffect *pEffect1 = (CEffect *) (pSort1->pObject);
	CEffect *pEffect2 = (CEffect *) (pSort2->pObject);
	if ((pEffect1->IsAssociated() == FALSE) && (pEffect2->IsAssociated()))
		return(1);	// record1 after record2
	else if ((pEffect1->IsAssociated()) && (pEffect2->IsAssociated() == FALSE))
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int EffectParticleCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CEffect *pEffect1 = (CEffect *) (pSort1->pObject);
	CEffect *pEffect2 = (CEffect *) (pSort2->pObject);
	
	/* get particle names for proper channel. nChannelIndex is set when sort routine is selected */

	CParticle *pParticle;
	pParticle = pEffect1->GetChannelParticle(nChannelIndex);
	CString ParticleName1 = "None";
	if (pParticle != NULL)
		ParticleName1 = pParticle->GetName();
	pParticle = pEffect2->GetChannelParticle(nChannelIndex);
	CString ParticleName2 = "None";
	if (pParticle != NULL)
		ParticleName2 = pParticle->GetName();
	
	/* compare particle names */

	int nCompare = ParticleName1.CompareNoCase(ParticleName2);
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int EffectUsedCountCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CEffect *pEffect1 = (CEffect *) (pSort1->pObject);
	CEffect *pEffect2 = (CEffect *) (pSort2->pObject);
	if (pEffect1->GetUsedCount() != pEffect2->GetUsedCount())
		return(pEffect2->GetUsedCount() - pEffect1->GetUsedCount());

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int ParticleNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CParticle *pParticle1 = (CParticle *) (pSort1->pObject);
	CParticle *pParticle2 = (CParticle *) (pSort2->pObject);
	int nCompare = pParticle1->GetName().CompareNoCase(pParticle2->GetName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(FALSE);	// names should be unique!
	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int ParticleGraphicNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CParticle *pParticle1 = (CParticle *) (pSort1->pObject);
	CParticle *pParticle2 = (CParticle *) (pSort2->pObject);
	int nCompare = pParticle1->GetGraphicName().CompareNoCase(pParticle2->GetGraphicName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int ParticleAssociatedCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CParticle *pParticle1 = (CParticle *) (pSort1->pObject);
	CParticle *pParticle2 = (CParticle *) (pSort2->pObject);
	if ((pParticle1->IsAssociated() == FALSE) && (pParticle2->IsAssociated()))
		return(1);	// record1 after record2
	else if ((pParticle1->IsAssociated()) && (pParticle2->IsAssociated() == FALSE))
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int ParticleUsedCountCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CParticle *pParticle1 = (CParticle *) (pSort1->pObject);
	CParticle *pParticle2 = (CParticle *) (pSort2->pObject);
	if (pParticle1->GetUsedCount() != pParticle2->GetUsedCount())
		return(pParticle2->GetUsedCount() - pParticle1->GetUsedCount());

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int GraphicNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CGraphic *pGraphic1 = (CGraphic *) (pSort1->pObject);
	CGraphic *pGraphic2 = (CGraphic *) (pSort2->pObject);
	int nCompare = pGraphic1->GetName().CompareNoCase(pGraphic2->GetName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(FALSE);	// names should be unique!
	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int GraphicFramesCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CGraphic *pGraphic1 = (CGraphic *) (pSort1->pObject);
	CGraphic *pGraphic2 = (CGraphic *) (pSort2->pObject);
	if (pGraphic1->GetImageCount() != pGraphic2->GetImageCount())
		return(pGraphic2->GetImageCount() - pGraphic1->GetImageCount());

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int GraphicUsedCountCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CGraphic *pGraphic1 = (CGraphic *) (pSort1->pObject);
	CGraphic *pGraphic2 = (CGraphic *) (pSort2->pObject);
	if (pGraphic1->GetUsedCount() != pGraphic2->GetUsedCount())
		return(pGraphic2->GetUsedCount() - pGraphic1->GetUsedCount());

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int GraphicTextureSetSizeCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CGraphic *pGraphic1 = (CGraphic *) (pSort1->pObject);
	CGraphic *pGraphic2 = (CGraphic *) (pSort2->pObject);
	if (pGraphic1->GetTextureSetSize() != pGraphic2->GetTextureSetSize())
		return(pGraphic2->GetTextureSetSize() - pGraphic1->GetTextureSetSize());

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int EmitterNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CEmitter *pEmitter1 = (CEmitter *) (pSort1->pObject);
	CEmitter *pEmitter2 = (CEmitter *) (pSort2->pObject);
	int nCompare = pEmitter1->GetName().CompareNoCase(pEmitter2->GetName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(FALSE);	// names should be unique!
	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int EmitterEffectNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CEmitter *pEmitter1 = (CEmitter *) (pSort1->pObject);
	CEmitter *pEmitter2 = (CEmitter *) (pSort2->pObject);
	int nCompare = pEmitter1->GetEffectName().CompareNoCase(pEmitter2->GetEffectName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int EmitterAssociatedCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CEmitter *pEmitter1 = (CEmitter *) (pSort1->pObject);
	CEmitter *pEmitter2 = (CEmitter *) (pSort2->pObject);
	if ((pEmitter1->IsAssociated() == FALSE) && (pEmitter2->IsAssociated()))
		return(1);	// record1 after record2
	else if ((pEmitter1->IsAssociated()) && (pEmitter2->IsAssociated() == FALSE))
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int EmitterUsedCountCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CEmitter *pEmitter1 = (CEmitter *) (pSort1->pObject);
	CEmitter *pEmitter2 = (CEmitter *) (pSort2->pObject);
	if (pEmitter1->GetUsedCount() != pEmitter2->GetUsedCount())
		return(pEmitter2->GetUsedCount() - pEmitter1->GetUsedCount());

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int SwooshNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CSwoosh *pSwoosh1 = (CSwoosh *) (pSort1->pObject);
	CSwoosh *pSwoosh2 = (CSwoosh *) (pSort2->pObject);
	int nCompare = pSwoosh1->GetName().CompareNoCase(pSwoosh2->GetName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(FALSE);	// names should be unique!
	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int SwooshGraphicNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CSwoosh *pSwoosh1 = (CSwoosh *) (pSort1->pObject);
	CSwoosh *pSwoosh2 = (CSwoosh *) (pSort2->pObject);
	int nCompare = pSwoosh1->GetGraphicName().CompareNoCase(pSwoosh2->GetGraphicName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int SwooshAssociatedCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CSwoosh *pSwoosh1 = (CSwoosh *) (pSort1->pObject);
	CSwoosh *pSwoosh2 = (CSwoosh *) (pSort2->pObject);
	if ((pSwoosh1->IsAssociated() == FALSE) && (pSwoosh2->IsAssociated()))
		return(1);	// record1 after record2
	else if ((pSwoosh1->IsAssociated()) && (pSwoosh2->IsAssociated() == FALSE))
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int SwooshUsedCountCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CSwoosh *pSwoosh1 = (CSwoosh *) (pSort1->pObject);
	CSwoosh *pSwoosh2 = (CSwoosh *) (pSort2->pObject);
	if (pSwoosh1->GetUsedCount() != pSwoosh2->GetUsedCount())
		return(pSwoosh2->GetUsedCount() - pSwoosh1->GetUsedCount());

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int SwooshEmitterNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CPigpenSwooshEmitter *pSwooshEmitter1 = (CPigpenSwooshEmitter *) (pSort1->pObject);
	CPigpenSwooshEmitter *pSwooshEmitter2 = (CPigpenSwooshEmitter *) (pSort2->pObject);
	int nCompare = pSwooshEmitter1->GetName().CompareNoCase(pSwooshEmitter2->GetName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(FALSE);	// names should be unique!
	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int SwooshEmitterSwooshNameCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CPigpenSwooshEmitter *pSwooshEmitter1 = (CPigpenSwooshEmitter *) (pSort1->pObject);
	CPigpenSwooshEmitter *pSwooshEmitter2 = (CPigpenSwooshEmitter *) (pSort2->pObject);
	int nCompare = pSwooshEmitter1->GetSwooshName().CompareNoCase(pSwooshEmitter2->GetSwooshName());
	if (nCompare > 0)
		return(1);	// record1 after record2
	else if (nCompare < 0)
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int SwooshEmitterAssociatedCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CPigpenSwooshEmitter *pSwooshEmitter1 = (CPigpenSwooshEmitter *) (pSort1->pObject);
	CPigpenSwooshEmitter *pSwooshEmitter2 = (CPigpenSwooshEmitter *) (pSort2->pObject);
	if ((pSwooshEmitter1->IsAssociated() == FALSE) && (pSwooshEmitter2->IsAssociated()))
		return(1);	// record1 after record2
	else if ((pSwooshEmitter1->IsAssociated()) && (pSwooshEmitter2->IsAssociated() == FALSE))
		return(-1);	// record2 after record1

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}
int SwooshEmitterUsedCountCompare(const void *arg1, const void *arg2)
{
	ts_SortRecord *pSort1 = (ts_SortRecord *) arg1;
	ts_SortRecord *pSort2 = (ts_SortRecord *) arg2;
	CPigpenSwooshEmitter *pSwooshEmitter1 = (CPigpenSwooshEmitter *) (pSort1->pObject);
	CPigpenSwooshEmitter *pSwooshEmitter2 = (CPigpenSwooshEmitter *) (pSort2->pObject);
	if (pSwooshEmitter1->GetUsedCount() != pSwooshEmitter2->GetUsedCount())
		return(pSwooshEmitter2->GetUsedCount() - pSwooshEmitter1->GetUsedCount());

	/* tie-breaker */

	ASSERT(pSort1->ulListIndex != pSort2->ulListIndex);	// pre-sort list indices should be unique!
	return(pSort1->ulListIndex - pSort2->ulListIndex);
}

/************************** basic CObArray processing *******************************/

/* add a new item to an array */

CObject *CPigpenDoc::NewItem(int nType)
{
	switch(nType)
	{
	case EFFECTS:
		m_Arrays[EFFECTS].Add(new CEffect);
		break;
	case PARTICLES:
		m_Arrays[PARTICLES].Add(new CParticle);
		break;
	case GRAPHICS:
		m_Arrays[GRAPHICS].Add(new CGraphic);
		break;
	case EMITTERS:
		m_Arrays[EMITTERS].Add(new CEmitter);
		break;
	case SWOOSHES:
		m_Arrays[SWOOSHES].Add(new CSwoosh);
		break;
	case SWOOSH_EMITTERS:
		m_Arrays[SWOOSH_EMITTERS].Add(new CPigpenSwooshEmitter);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	
	/* make sure name is unique */

	CObject *pObject = m_Arrays[nType].GetAt(m_Arrays[nType].GetSize() - 1);
	EnforceUniqueName(pObject, nType);

	/* doc has changed, update views */

	SetModifiedFlag(MODIFIED_FLAG);
	UpdateAllViews(NULL, nNoChangeOtherViews[nType]);

	/* return pointer to new object */

	return(pObject);
}

/* rename an item */

void CPigpenDoc::RenameItem(int nType, int nIndex, char *pName) 
{
	/* set name */

	int nHint = 0;
	switch(nType)
	{
	case EFFECTS:
		if (strlen(pName) != 0)	// name can't be empty
			GetEffect(nIndex)->SetName(pName);
		nHint = NO_REORDER_EMITTERS_VIEW | NO_REORDER_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW;	// renaming effect can change emitters view
		break;
	case PARTICLES:
		if (strlen(pName) != 0)	// name can't be empty
			GetParticle(nIndex)->SetName(pName);
		nHint = NO_CHANGE_EMITTERS_VIEW | NO_REORDER_EFFECTS_VIEW | NO_REORDER_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW;	// renaming particle can change effects view/dialog
		break;
	case GRAPHICS:
		if (strlen(pName) != 0)	// name can't be empty
			GetGraphic(nIndex)->SetName(pName);
		nHint = NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_REORDER_PARTICLES_VIEW | NO_REORDER_GRAPHICS_VIEW | NO_REORDER_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW;	// renaming graphic can change particles view/dialog
		break;
	case EMITTERS:
		if (strlen(pName) != 0)	// name can't be empty
			GetEmitter(nIndex)->SetName(pName);
		nHint = NO_REORDER_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW;
		break;
	case SWOOSHES:
		if (strlen(pName) != 0)	// name can't be empty
			GetSwoosh(nIndex)->SetName(pName);
		nHint = NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_REORDER_SWOOSHES_VIEW | NO_REORDER_SWOOSH_EMITTERS_VIEW;
		break;
	case SWOOSH_EMITTERS:
		if (strlen(pName) != 0)	// name can't be empty
			GetSwooshEmitter(nIndex)->SetName(pName);
		nHint = NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_REORDER_SWOOSH_EMITTERS_VIEW;	// renaming swoosh can change swoosh emitters view
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	/* make sure name is unique */

	EnforceUniqueName(m_Arrays[nType].GetAt(nIndex), nType);

	/* doc has changed, update views */

	SetModifiedFlag(MODIFIED_FLAG);
	UpdateAllViews(NULL, nHint);
}

/************************** generic list handling routines *******************************/

/* build a list of indices of selected items in list */

int *CPigpenDoc::CollectSelectedItems(int nType, CListCtrl& List, int& nIndexCount)
{
	/* may not be anything selected, since they may be using the hot key */

	POSITION pos = List.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return(NULL);
	
	/* build up list. it is important to note that the order of items in the view list is the same */
	/* as the order of items in the doc's list. it is important that this index list be monotonically */
	/* increasing */

	int *pIndexArray = (int *) malloc(GetArraySize(nType) * sizeof(int));
	nIndexCount = 0;
   while (pos)
	{
      pIndexArray[nIndexCount] = List.GetNextSelectedItem(pos);
		ASSERT((nIndexCount == 0) || (pIndexArray[nIndexCount] > pIndexArray[nIndexCount - 1]));
		nIndexCount++;
	}
	return(pIndexArray);	// it is caller's responsibility to free array
}

/* delete selected items in passed List from passed CObArray-- generalize this since it's a common thing */

bool CPigpenDoc::DeleteSelectedItems(int nType, CListCtrl& List)
{
	/* may not be anything selected, since they may be using the hot key */

	int nIndexCount;
	int *pIndexArray = CollectSelectedItems(nType, List, nIndexCount);
	if (pIndexArray == NULL)
		return(FALSE);	// nothing happened

	/* check for clearing references to about-to-be-deleted items */

	int nHint = 0;
	switch(nType)
	{
	case EFFECTS:
		ClearEffectReferences(pIndexArray, nIndexCount);
		nHint = NO_REORDER_EMITTERS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW;	// deleting effect(s) can change emitters view/dialog
		break;
	case PARTICLES:
		ClearParticleReferences(pIndexArray, nIndexCount);
		nHint = NO_REORDER_EFFECTS_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW;	// deleting particle(s) can change effects view/dialog
		break;
	case GRAPHICS:
		ClearGraphicReferences(pIndexArray, nIndexCount);
		nHint = NO_CHANGE_EFFECTS_VIEW | NO_REORDER_PARTICLES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW;	// deleting graphic(s) can change particles view/dialog
		break;
	case EMITTERS:
		nHint = NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW;
		break;
	case SWOOSHES:
		ClearSwooshReferences(pIndexArray, nIndexCount);
		nHint = NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_REORDER_SWOOSH_EMITTERS_VIEW;
		break;
	case SWOOSH_EMITTERS:
		nHint = NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW;
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	/* go backward through the list to delete properly */

	for (int i = nIndexCount - 1; i >= 0; i--)
	{
		/* delete from view list */

		List.DeleteItem(pIndexArray[i]);

		/* delete from CObArray and delete object itself */

		CObject *pObject = GetArrayItem(nType, pIndexArray[i]);
		m_Arrays[nType].RemoveAt(pIndexArray[i]);
		delete pObject;
	}
	free(pIndexArray);

	/* doc has changed, update views */

	SetModifiedFlag(MODIFIED_FLAG);
	UpdateAllViews(NULL, nHint);

	/* something was deleted */

	return(TRUE);
}

/* move selected items in passed List to top, bottom of CObarray */

void CPigpenDoc::MoveSelectedItemsToTop(int nType, CListCtrl& List)
{
	int nIndexCount;
	int *pIndexArray = CollectSelectedItems(nType, List, nIndexCount);
	if (pIndexArray)
	{
		for (int i = 0; i < nIndexCount; i++)
	   {
			/* delete from CObArray and reinsert at start of array */

			CObject *pObject = GetArrayItem(nType, pIndexArray[i]);
			m_Arrays[nType].RemoveAt(pIndexArray[i]);
			m_Arrays[nType].InsertAt(i, pObject);
		}
		free(pIndexArray);

		/* doc has changed, update views */

		SetModifiedFlag(MODIFIED_FLAG);
		UpdateAllViews(NULL, nNoChangeOtherViews[nType]);
   }
}
void CPigpenDoc::MoveSelectedItemsToBottom(int nType, CListCtrl& List)
{
	int nIndexCount;
	int *pIndexArray = CollectSelectedItems(nType, List, nIndexCount);
	if (pIndexArray)
	{
		for (int i = 0; i < nIndexCount; i++)
	   {
			/* delete from CObArray and add to end of array */

			CObject *pObject = GetArrayItem(nType, pIndexArray[i] - i);	// as items are pulled out location of subsequent items moves up
			m_Arrays[nType].RemoveAt(pIndexArray[i] - i);
			m_Arrays[nType].Add(pObject);
		}
		free(pIndexArray);

		/* doc has changed, update views */

		SetModifiedFlag(MODIFIED_FLAG);
		UpdateAllViews(NULL, nNoChangeOtherViews[nType]);
   }
}

/* sort an array. i want to maintain previously-sorted order as much as possible-- for example, if the effects
list is sorted on name and then on common status, i want the effects sorted by name within the common section.
record the pre-sorted list index so that i can use it as a tie-breaker */

void CPigpenDoc::SortArray(int nType, int nColumn)
{
	/* anything to do? */

	if (GetArraySize(nType) < 2)
		return;

	/* point to sort routine to use for data type/column */

	int (*SortRoutine)(const void *elem1, const void *elem2);
	switch(nType)
	{
	case EFFECTS:
		switch(nColumn)
		{
		case EFFECT_NAME_COLUMN:
			SortRoutine = EffectNameCompare;
			break;
		case EFFECT_PART1_COLUMN:
		case EFFECT_PART2_COLUMN:
		case EFFECT_PART3_COLUMN:
		case EFFECT_PART4_COLUMN:
		case EFFECT_PART5_COLUMN:
			SortRoutine = EffectParticleCompare;
			nChannelIndex = nColumn - EFFECT_PART1_COLUMN;
			break;
		case EFFECT_ASSOCIATED_COLUMN:
			SortRoutine = EffectAssociatedCompare;
			break;
		case EFFECT_USED_COUNT_COLUMN:
			SortRoutine = EffectUsedCountCompare;
			break;
		default:
			/* unsupported column type-- bail */

			ASSERT(FALSE);
			return;
		}
		break;
	case PARTICLES:
		switch(nColumn)
		{
		case PARTICLE_NAME_COLUMN:
			SortRoutine = ParticleNameCompare;
			break;
		case PARTICLE_GRAPHIC_COLUMN:
			SortRoutine = ParticleGraphicNameCompare;
			break;
		case PARTICLE_ASSOCIATED_COLUMN:
			SortRoutine = ParticleAssociatedCompare;
			break;
		case PARTICLE_USED_COUNT_COLUMN:
			SortRoutine = ParticleUsedCountCompare;
			break;
		default:
			/* unsupported column type-- bail */

			ASSERT(FALSE);
			return;
		}
		break;
	case GRAPHICS:
		switch(nColumn)
		{
		case GRAPHIC_NAME_COLUMN:
			SortRoutine = GraphicNameCompare;
			break;
		case GRAPHIC_FRAMES_COLUMN:
			SortRoutine = GraphicFramesCompare;
			break;
		case GRAPHIC_SIZE_COLUMN:
		case GRAPHIC_IMPORT_PATH_COLUMN:
			return;
		case GRAPHIC_USED_COUNT_COLUMN:
			SortRoutine = GraphicUsedCountCompare;
			break;
		case GRAPHIC_EXPORT_SIZE_COLUMN:
			SortRoutine = GraphicTextureSetSizeCompare;
			break;
		default:
			/* unsupported column type-- bail */

			ASSERT(FALSE);
			return;
		}
		break;
	case EMITTERS:
		switch(nColumn)
		{
		case EMITTER_NAME_COLUMN:
			SortRoutine = EmitterNameCompare;
			break;
		case EMITTER_EFFECT_COLUMN:
			SortRoutine = EmitterEffectNameCompare;
			break;
		case EMITTER_ASSOCIATED_COLUMN:
			SortRoutine = EmitterAssociatedCompare;
			break;
		case EMITTER_USED_COUNT_COLUMN:
			SortRoutine = EmitterUsedCountCompare;
			break;
		default:
			/* unsupported column type-- bail */

			ASSERT(FALSE);
			return;
		}
		break;
	case SWOOSHES:
		switch(nColumn)
		{
		case SWOOSH_NAME_COLUMN:
			SortRoutine = SwooshNameCompare;
			break;
		case SWOOSH_GRAPHIC_COLUMN:
			SortRoutine = SwooshGraphicNameCompare;
			break;
		case SWOOSH_ASSOCIATED_COLUMN:
			SortRoutine = SwooshAssociatedCompare;
			break;
		case SWOOSH_USED_COUNT_COLUMN:
			SortRoutine = SwooshUsedCountCompare;
			break;
		default:
			/* unsupported column type-- bail */

			ASSERT(FALSE);
			return;
		}
		break;
	case SWOOSH_EMITTERS:
		switch(nColumn)
		{
		case SWOOSH_EMITTER_NAME_COLUMN:
			SortRoutine = SwooshEmitterNameCompare;
			break;
		case SWOOSH_EMITTER_SWOOSH_COLUMN:
			SortRoutine = SwooshEmitterSwooshNameCompare;
			break;
		case SWOOSH_EMITTER_ASSOCIATED_COLUMN:
			SortRoutine = SwooshEmitterAssociatedCompare;
			break;
		case SWOOSH_EMITTER_USED_COUNT_COLUMN:
			SortRoutine = SwooshEmitterUsedCountCompare;
			break;
		default:
			/* unsupported column type-- bail */

			ASSERT(FALSE);
			return;
		}
		break;
	default:
		ASSERT(FALSE);
		return;
	}

	/* create an array of pointers to item records that can be qsorted */

	ts_SortRecord *pSortRecords = (ts_SortRecord *) malloc(GetArraySize(nType) * sizeof (ts_SortRecord));
	for (int i = 0; i < GetArraySize(nType); i++)
	{
		pSortRecords[i].pObject = GetArrayItem(nType, i);
		CEffect *pEffect = (CEffect *) pSortRecords[i].pObject;
		pSortRecords[i].ulListIndex = i;
	}

	/* qsort */

	qsort(pSortRecords, GetArraySize(nType), sizeof(ts_SortRecord),  SortRoutine);

	/* rebuild list to match new order */

	for (i = 0; i < GetArraySize(nType); i++)
		m_Arrays[nType].SetAt(i, pSortRecords[i].pObject);
	free(pSortRecords);

	/* doc has changed, update views */

	SetModifiedFlag(MODIFIED_FLAG);
	UpdateAllViews(NULL, nNoChangeOtherViews[nType]);
}

/************************** effect list processing ***************************************/

/* before deleting effect(s), clear possible references in emitters, particles */

void CPigpenDoc::ClearEffectReferences(int *pIndexArray, int nIndexCount)
{
	for (int i = 0; i < nIndexCount; i++)
	{
		CEffect *pEffect = GetEffect(pIndexArray[i]);

		/* first check emitters */

		for (int j = 0; j < GetEmitterCount(); j++)
		{
			CEmitter *pEmitter = GetEmitter(j);
			if (pEffect == pEmitter->GetEffect())
				pEmitter->SetEffect(NULL);
		}

		/* effects can be spawned by particles-- have to clear them, too */

		for (j = 0; j < GetParticleCount(); j++)
		{
			CParticle *pParticle = GetParticle(j);
			for (int k = 0; k < pParticle->GetSpawnCount(); k++)
			{
				if (pEffect == pParticle->GetSpawn(k)->GetEffect())
					pParticle->GetSpawn(k)->SetEffect(NULL);
			}
		}
	}
}

/* before deleting particle(s), clear possible references in effect channels */

void CPigpenDoc::ClearParticleReferences(int *pIndexArray, int nIndexCount)
{
	for (int i = 0; i < nIndexCount; i++)
	{
		CParticle *pParticle = GetParticle(pIndexArray[i]);
		for (int j = 0; j < GetEffectCount(); j++)
		{
			CEffect *pEffect = GetEffect(j);
			for (int k = 0; k < MAX_CHANNELS_PER_EFFECT; k++)
			{
				if (pParticle == pEffect->m_Channels[k].pParticle)
					pEffect->m_Channels[k].pParticle = NULL;
			}
		}
	}
}

/* before deleting graphic(s), clear possible references in particles/swooshes */

void CPigpenDoc::ClearGraphicReferences(int *pIndexArray, int nIndexCount)
{
	for (int i = 0; i < nIndexCount; i++)
	{
		CGraphic *pGraphic = GetGraphic(pIndexArray[i]);
		for (int j = 0; j < GetParticleCount(); j++)
		{
			CParticle *pParticle = GetParticle(j);
			if (pGraphic == pParticle->GetGraphic())
				pParticle->SetGraphic(NULL);
		}
		for (j = 0; j < GetSwooshCount(); j++)
		{
			CSwoosh *pSwoosh = GetSwoosh(j);
			if (pGraphic == pSwoosh->GetGraphic())
				pSwoosh->SetGraphic(NULL);
		}
	}
}

/* before deleting swoosh(es), clear possible references in swoosh emitters */

void CPigpenDoc::ClearSwooshReferences(int *pIndexArray, int nIndexCount)
{
	for (int i = 0; i < nIndexCount; i++)
	{
		CSwoosh *pSwoosh = GetSwoosh(pIndexArray[i]);
		for (int j = 0; j < GetSwooshEmitterCount(); j++)
		{
			CPigpenSwooshEmitter *pSwooshEmitter = GetSwooshEmitter(j);
			if (pSwoosh == pSwooshEmitter->GetSwoosh())
				pSwooshEmitter->SetSwoosh(NULL);
		}
	}
}

/* add a dummy effect to the effects list, for particle previewing */

CEffect *CPigpenDoc::AddDummyEffect(void)
{
	m_Arrays[EFFECTS].Add(new CEffect);
	int nIndex = GetEffectCount() - 1;
	CEffect *pEffect = GetEffect(nIndex);
	pEffect->SetName("PREVIEW DUMMY");
	return(pEffect);
}

/* delete last effect in list, don't signal change in doc. this is to support preview of a particle */

void CPigpenDoc::DeleteDummyEffect(void)
{
	/* remove from list, then delete item itself */

	int nIndex = GetEffectCount() - 1;
	CEffect *pEffect = GetEffect(nIndex);
	ASSERT(pEffect->GetName() == "PREVIEW DUMMY");
	m_Arrays[EFFECTS].RemoveAt(nIndex);
	delete pEffect;
}

/* add a dummy emitter to the emitters list, for particle previewing */

CEmitter *CPigpenDoc::AddDummyEmitter(void)
{
	m_Arrays[EMITTERS].Add(new CEmitter);
	int nIndex = GetEmitterCount() - 1;
	CEmitter *pEmitter = GetEmitter(nIndex);
	pEmitter->SetName("PREVIEW DUMMY");
	return(pEmitter);
}

/* delete last emitter in list, don't signal change in doc. this is to support preview of a particle */

void CPigpenDoc::DeleteDummyEmitter(void)
{
	/* remove from list, then delete item itself */

	int nIndex = GetEmitterCount() - 1;
	CEmitter *pEmitter = GetEmitter(nIndex);
	ASSERT(pEmitter->GetName() == "PREVIEW DUMMY");
	m_Arrays[EMITTERS].RemoveAt(nIndex);
	delete pEmitter;
}

/* add a dummy swoosh emitter to the swoosh emitters list, for swoosh previewing */

CPigpenSwooshEmitter *CPigpenDoc::AddDummySwooshEmitter(void)
{
	m_Arrays[SWOOSH_EMITTERS].Add(new CPigpenSwooshEmitter);
	int nIndex = GetSwooshEmitterCount() - 1;
	CPigpenSwooshEmitter *pSwooshEmitter = GetSwooshEmitter(nIndex);
	pSwooshEmitter->SetName("PREVIEW DUMMY");
	pSwooshEmitter->SetType(SWOOSH_EMITTER_PREVIEW);
	return(pSwooshEmitter);
}

/* delete last emitter in list, don't signal change in doc. this is to support preview of a particle */

void CPigpenDoc::DeleteDummySwooshEmitter(void)
{
	/* remove from list, then delete item itself */

	int nIndex = GetSwooshEmitterCount() - 1;
	CPigpenSwooshEmitter *pSwooshEmitter = GetSwooshEmitter(nIndex);
	ASSERT(pSwooshEmitter->GetName() == "PREVIEW DUMMY");
	m_Arrays[SWOOSH_EMITTERS].RemoveAt(nIndex);
	delete pSwooshEmitter;
}

/* make sure name of passed object is unique. this is called after pasting new records into a list, or after
user renames item */

void CPigpenDoc::EnforceUniqueName(CObject *pObject, int nType)
{
	/* get name to check */

	CString Name;
	switch(nType)
	{
	case EFFECTS:
		Name = ((CEffect *) pObject)->GetName();
		break;
	case PARTICLES:
		Name = ((CParticle *) pObject)->GetName();
		break;
	case GRAPHICS:
		Name = ((CGraphic *) pObject)->GetName();
		break;
	case EMITTERS:
		Name = ((CEmitter *) pObject)->GetName();
		break;
	case SWOOSHES:
		Name = ((CSwoosh *) pObject)->GetName();
		break;
	case SWOOSH_EMITTERS:
		Name = ((CPigpenSwooshEmitter *) pObject)->GetName();
		break;
	default:
		_ASSERT(FALSE);
		break;
	}

	/* is it a duplicate? */

	bool bDuplicateFound = TRUE;
	bool bNameChanged = FALSE;
	while (bDuplicateFound)
	{
		bDuplicateFound = FALSE;
		for (int i = 0; i < m_Arrays[nType].GetSize(); i++)
		{
			CString Name2;
			CObject *pObject2 = m_Arrays[nType].GetAt(i);
			if (pObject == pObject2)
				continue;	// don't check itself
			switch(nType)
			{
			case EFFECTS:
				Name2 = ((CEffect *) pObject2)->GetName();
				break;
			case PARTICLES:
				Name2 = ((CParticle *) pObject2)->GetName();
				break;
			case GRAPHICS:
				Name2 = ((CGraphic *) pObject2)->GetName();
				break;
			case EMITTERS:
				Name2 = ((CEmitter *) pObject2)->GetName();
				break;
			case SWOOSHES:
				Name2 = ((CSwoosh *) pObject2)->GetName();
				break;
			case SWOOSH_EMITTERS:
				Name2 = ((CPigpenSwooshEmitter *) pObject2)->GetName();
				break;
			default:
				_ASSERT(FALSE);
				break;
			}

			/* duplicate? */

			if (Name.CompareNoCase(Name2) == 0)
			{
				bDuplicateFound = TRUE;
				break;
			}
		}

		/* if found a duplicate, rename and try again */

		if (bDuplicateFound)
		{
			Name.TrimRight();	// trim trailing white space

			/* see if this name already has a (n) appended to it */

			bool bAppended = FALSE;
			int nAppendValue = 2;
			int nPos = Name.ReverseFind('(');
			if (nPos != -1)
			{
				/* ok, we have opening parentheses. all but last character should be numbers */

				bAppended = TRUE;	// assume it will pass the test
				for (int i = nPos + 1; i < Name.GetLength() - 1; i++)
				{
					if (isdigit(Name.GetAt(i)) == FALSE)
					{
						bAppended = FALSE;
						break;
					}
				}

				/* check for closing parenthesis */

				if (Name.GetAt(Name.GetLength() - 1) != ')')
					bAppended = FALSE;

				/* if already appended, bump value by one */

				if (bAppended)
				{
					nAppendValue = atoi(Name.GetBuffer(10) + nPos + 1);
					nAppendValue++;	// bump it by one
					if (nPos == 0)
						Name = "Unnamed";	// this would be weird-- no name, just a "(n)" */
					else
						Name = Name.Left(nPos - 1);	// trim off "(n)"
				}
			}

			/* append "(n)" to name */

			Name.TrimRight();
			Name = Name + " (" + ToCString(nAppendValue) + ")";
			bNameChanged = TRUE;
		}
	}

	/* back to object */

	if (bNameChanged)
	{
		switch(nType)
		{
		case EFFECTS:
			((CEffect *) pObject)->SetName(Name.GetBuffer(10));
			break;
		case PARTICLES:
			((CParticle *) pObject)->SetName(Name.GetBuffer(10));
			break;
		case GRAPHICS:
			((CGraphic *) pObject)->SetName(Name.GetBuffer(10));
			break;
		case EMITTERS:
			((CEmitter *) pObject)->SetName(Name.GetBuffer(10));
			break;
		case SWOOSHES:
			((CSwoosh *) pObject)->SetName(Name.GetBuffer(10));
			break;
		case SWOOSH_EMITTERS:
			((CPigpenSwooshEmitter *) pObject)->SetName(Name.GetBuffer(10));
			break;
		}
	}
}

/* given a seed object (particle, effect, etc), go through all data items and figure out if they are associated with the seed object. there are
two flags used here-- an associate flag (which is persistant, and tested when the view is drawn), and a new-associate flag, which is set when
an item is found to be an associate. this lets us loop through the items and test the associations again for each new associate. the process
ends when no more noew associates are found */

void CPigpenDoc::FindAssociates(CObject *pSeedObject, int nType)
{
	/* clear all flags */

	int i;
	for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
	{
		for (int j = 0; j < m_Arrays[i].GetSize(); j++)
		{
			CObject *pObject = m_Arrays[i].GetAt(j);
			switch(i)
			{
			case EFFECTS:
				((CEffect *) pObject)->ClearAssociateFlags();
				break;
			case PARTICLES:
				((CParticle *) pObject)->ClearAssociateFlags();
				break;
			case GRAPHICS:
				((CGraphic *) pObject)->ClearAssociateFlags();
				break;
			case EMITTERS:
				((CEmitter *) pObject)->ClearAssociateFlags();
				break;
			case SWOOSHES:
				((CSwoosh *) pObject)->ClearAssociateFlags();
				break;
			case SWOOSH_EMITTERS:
				((CPigpenSwooshEmitter *) pObject)->ClearAssociateFlags();
				break;
			default:
				ASSERT(false);
				break;
			}
		}
	}

	/* set associate flags for passed object */

	switch(nType)
	{
	case EFFECTS:
		((CEffect *) pSeedObject)->SetAssociateFlags();
		break;
	case PARTICLES:
		((CParticle *) pSeedObject)->SetAssociateFlags();
		break;
	case GRAPHICS:
		((CGraphic *) pSeedObject)->SetAssociateFlags();
		break;
	case EMITTERS:
		((CEmitter *) pSeedObject)->SetAssociateFlags();
		break;
	case SWOOSHES:
		((CSwoosh *) pSeedObject)->SetAssociateFlags();
		break;
	case SWOOSH_EMITTERS:
		((CPigpenSwooshEmitter *) pSeedObject)->SetAssociateFlags();
		break;
	default:
		ASSERT(false);
		break;
	}

	/* try to open log */

	char Filename[_MAX_PATH];
	LoadPersistentString("Pigpen Associate Log", Filename, sizeof(Filename));
	if (strlen(Filename) != 0)
	{
		m_pAssociateLog = fopen(Filename, "wt");
		if (m_pAssociateLog)
			fprintf(m_pAssociateLog, "Following association links:\n");
	}

	/* go through all data objects and look for new associates. if found, pass that object to all other objects looking to extend */
	/* the associate net */

	bool stillGoing = true;
	while (stillGoing)
	{
		/* assume this will be the last time */

		stillGoing = false;

		/* look for a new associate to pass on to all other data items */

		for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
		{
			for (int j = 0; j < m_Arrays[i].GetSize(); j++)
			{
				bool newAssociate = false;
				CObject *pObject = m_Arrays[i].GetAt(j);
				switch(i)
				{
				case EFFECTS:
					newAssociate = ((CEffect *) pObject)->IsNewAssociate();
					((CEffect *) pObject)->ClearNewAssociateFlag();						// if it's true, clear it now that we've found it. if false, no harm done
					break;
				case PARTICLES:
					newAssociate = ((CParticle *) pObject)->IsNewAssociate();
					((CParticle *) pObject)->ClearNewAssociateFlag();					// if it's true, clear it now that we've found it. if false, no harm done
					break;
				case GRAPHICS:
					newAssociate = ((CGraphic *) pObject)->IsNewAssociate();
					((CGraphic *) pObject)->ClearNewAssociateFlag();					// if it's true, clear it now that we've found it. if false, no harm done
					break;
				case EMITTERS:
					newAssociate = ((CEmitter *) pObject)->IsNewAssociate();
					((CEmitter *) pObject)->ClearNewAssociateFlag();					// if it's true, clear it now that we've found it. if false, no harm done
					break;
				case SWOOSHES:
					newAssociate = ((CSwoosh *) pObject)->IsNewAssociate();
					((CSwoosh *) pObject)->ClearNewAssociateFlag();						// if it's true, clear it now that we've found it. if false, no harm done
					break;
				case SWOOSH_EMITTERS:
					newAssociate = ((CPigpenSwooshEmitter *) pObject)->IsNewAssociate();
					((CPigpenSwooshEmitter *) pObject)->ClearNewAssociateFlag();	// if it's true, clear it now that we've found it. if false, no harm done
					break;
				default:
					ASSERT(false);
					break;
				}

				/* if we found one, pass it along to all other data items to see if they associate with it */

				if (newAssociate)
				{
					stillGoing = true;	// found a new associate, so keep looking for more that this new one may set
					TestAssociate(pObject, i);
				}
			}
		}
	}

	/* alternate log file form */

	if (m_pAssociateLog)
	{
		for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
		{
			bool headingShown = false;
			for (int j = 0; j < m_Arrays[i].GetSize(); j++)
			{
				bool writeToLog = false;
				char logHeading[200];
				char logMessage[200];
				CObject *pObject = m_Arrays[i].GetAt(j);
				switch(i)
				{
				case EFFECTS:
					writeToLog = ((CEffect *) pObject)->IsAssociated();
					strcpy(logHeading, "Effects:");
					sprintf(logMessage, "%s", ((CEffect *) pObject)->GetName());
					break;
				case PARTICLES:
					writeToLog = ((CParticle *) pObject)->IsAssociated();
					strcpy(logHeading, "Particles:");
					sprintf(logMessage, "%s", ((CParticle *) pObject)->GetName());
					break;
				case GRAPHICS:
					writeToLog = ((CGraphic *) pObject)->IsAssociated();
					strcpy(logHeading, "Graphics:");
					sprintf(logMessage, "%s", ((CGraphic *) pObject)->GetName());
					break;
				case EMITTERS:
					writeToLog = ((CEmitter *) pObject)->IsAssociated();
					strcpy(logHeading, "Emitters:");
					sprintf(logMessage, "%s", ((CEmitter *) pObject)->GetName());
					break;
				case SWOOSHES:
					writeToLog = ((CSwoosh *) pObject)->IsAssociated();
					strcpy(logHeading, "Swooshes:");
					sprintf(logMessage, "%s", ((CSwoosh *) pObject)->GetName());
					break;
				case SWOOSH_EMITTERS:
					writeToLog = ((CPigpenSwooshEmitter *) pObject)->IsAssociated();
					strcpy(logHeading, "Swoosh emitters:");
					sprintf(logMessage, "%s", ((CPigpenSwooshEmitter *) pObject)->GetName());
					break;
				default:
					ASSERT(false);
					break;
				}

				/* maybe write to log */

				if (writeToLog)
				{
					if (headingShown == false)
					{
						fprintf(m_pAssociateLog, "\n%s\n\n", logHeading);
						headingShown = true;
					}
					fprintf(m_pAssociateLog, "   %s\n", logMessage);
				}
			}
		}
	}

	/* close log */

	if (m_pAssociateLog)
	{
		fclose(m_pAssociateLog);
		m_pAssociateLog = NULL;
	}

	/* redraw all views to show newly-calculated association status */

	UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);
}

/* test if passed object is associated with any other object in the database */

void CPigpenDoc::TestAssociate(CObject *pTestObject, int nType)
{
	bool foundOne = false;

	/* loop through all other data objects */

	for (int i = 0; i < MAX_DOC_DATA_TYPES; i++)
	{
		for (int j = 0; j < m_Arrays[i].GetSize(); j++)
		{
			int associateState = ASSOCIATE_NONE;
			CObject *pObject = m_Arrays[i].GetAt(j);
			switch(i)
			{
			case EFFECTS:
				associateState = ((CEffect *) pObject)->TestAssociate(pTestObject, nType);	// returns true if new association is found
				break;
			case PARTICLES:
				associateState = ((CParticle *) pObject)->TestAssociate(pTestObject, nType);	// returns true if new association is found
				break;
			case GRAPHICS:
				associateState = ((CGraphic *) pObject)->TestAssociate(pTestObject, nType);	// returns true if new association is found
				break;
			case EMITTERS:
				associateState = ((CEmitter *) pObject)->TestAssociate(pTestObject, nType);	// returns true if new association is found
				break;
			case SWOOSHES:
				associateState = ((CSwoosh *) pObject)->TestAssociate(pTestObject, nType);	// returns true if new association is found
				break;
			case SWOOSH_EMITTERS:
				associateState = ((CPigpenSwooshEmitter *) pObject)->TestAssociate(pTestObject, nType);	// returns true if new association is found
				break;
			default:
				ASSERT(false);
				break;
			}

			/* maybe write to log */

			if ((associateState != ASSOCIATE_NONE) && (m_pAssociateLog))
			{
				/* formatting-- put a separator before each new test object */

				if (foundOne == false)
				{
					fprintf(m_pAssociateLog, "\n");
					foundOne = true;
				}

				/* print a suitable message to log file */

				switch(associateState)
				{
				case ASSOCIATE_EMITTER_USES_EFFECT:
					ASSERT(nType == EMITTERS);
					ASSERT(i == EFFECTS);
					fprintf(m_pAssociateLog, "   Emitter \"%s\" uses effect \"%s\"\n", ((CEmitter *) pTestObject)->GetName(), ((CEffect *) pObject)->GetName());
					break;
				case ASSOCIATE_EFFECT_USED_BY_EMITTER:
					ASSERT(nType == EFFECTS);
					ASSERT(i == EMITTERS);
					fprintf(m_pAssociateLog, "   Effect \"%s\" used by emitter \"%s\"\n", ((CEffect *) pTestObject)->GetName(), ((CEmitter *) pObject)->GetName());
					break;
				case ASSOCIATE_EFFECT_USES_PARTICLE:
					ASSERT(nType == EFFECTS);
					ASSERT(i == PARTICLES);
					fprintf(m_pAssociateLog, "   Effect \"%s\" uses particle \"%s\"\n", ((CEffect *) pTestObject)->GetName(), ((CParticle *) pObject)->GetName());
					break;
				case ASSOCIATE_PARTICLE_USED_BY_EFFECT:
					ASSERT(nType == PARTICLES);
					ASSERT(i == EFFECTS);
					fprintf(m_pAssociateLog, "   Particle \"%s\" used by effect \"%s\"\n", ((CParticle *) pTestObject)->GetName(), ((CEffect *) pObject)->GetName());
					break;
				case ASSOCIATE_PARTICLE_PIGGYBACKS_EMITTER:
					ASSERT(nType == PARTICLES);
					ASSERT(i == EMITTERS);
					fprintf(m_pAssociateLog, "   Particle \"%s\" piggybacks emitter \"%s\"\n", ((CParticle *) pTestObject)->GetName(), ((CEmitter *) pObject)->GetName());
					break;
				case ASSOCIATE_EMITTER_PIGGYBACKED_BY_PARTICLE:
					ASSERT(nType == EMITTERS);
					ASSERT(i == PARTICLES);
					fprintf(m_pAssociateLog, "   Emitter \"%s\" piggybacked by particle \"%s\"\n", ((CEmitter *) pTestObject)->GetName(), ((CParticle *) pObject)->GetName());
					break;
				case ASSOCIATE_PARTICLE_SPAWNS_EFFECT:
					ASSERT(nType == PARTICLES);
					ASSERT(i == EFFECTS);
					fprintf(m_pAssociateLog, "   Particle \"%s\" spawns effect \"%s\"\n", ((CParticle *) pTestObject)->GetName(), ((CEffect *) pObject)->GetName());
					break;
				case ASSOCIATE_EFFECT_SPAWNED_BY_PARTICLE:
					ASSERT(nType == EFFECTS);
					ASSERT(i == PARTICLES);
					fprintf(m_pAssociateLog, "   Effect \"%s\" spawned by particle \"%s\"\n", ((CEffect *) pTestObject)->GetName(), ((CParticle *) pObject)->GetName());
					break;
				case ASSOCIATE_PARTICLE_USES_GRAPHIC:
					ASSERT(nType == PARTICLES);
					ASSERT(i == GRAPHICS);
					fprintf(m_pAssociateLog, "   Particle \"%s\" uses graphic \"%s\"\n", ((CParticle *) pTestObject)->GetName(), ((CGraphic *) pObject)->GetName());
					break;
				case ASSOCIATE_GRAPHIC_USED_BY_PARTICLE:
					ASSERT(nType == GRAPHICS);
					ASSERT(i == PARTICLES);
					fprintf(m_pAssociateLog, "   Graphic \"%s\" used by particle \"%s\"\n", ((CGraphic *) pTestObject)->GetName(), ((CParticle *) pObject)->GetName());
					break;
				case ASSOCIATE_SWOOSH_USES_GRAPHIC:
					ASSERT(nType == SWOOSHES);
					ASSERT(i == GRAPHICS);
					fprintf(m_pAssociateLog, "   Swoosh \"%s\" uses graphic \"%s\"\n", ((CSwoosh *) pTestObject)->GetName(), ((CGraphic *) pObject)->GetName());
					break;
				case ASSOCIATE_GRAPHIC_USED_BY_SWOOSH:
					ASSERT(nType == GRAPHICS);
					ASSERT(i == SWOOSHES);
					fprintf(m_pAssociateLog, "   Graphic \"%s\" used by swoosh \"%s\"\n", ((CGraphic *) pTestObject)->GetName(), ((CSwoosh *) pObject)->GetName());
					break;
				case ASSOCIATE_SWOOSH_EMITTER_USES_SWOOSH:
					ASSERT(nType == SWOOSH_EMITTERS);
					ASSERT(i == SWOOSHES);
					fprintf(m_pAssociateLog, "   Swoosh emitter \"%s\" uses swoosh \"%s\"\n", ((CPigpenSwooshEmitter *) pTestObject)->GetName(), ((CSwoosh *) pObject)->GetName());
					break;
				case ASSOCIATE_SWOOSH_USED_BY_SWOOSH_EMITTER:
					ASSERT(nType == SWOOSHES);
					ASSERT(i == SWOOSH_EMITTERS);
					fprintf(m_pAssociateLog, "   Swoosh \"%s\" used by swoosh emitter \"%s\"\n", ((CSwoosh *) pTestObject)->GetName(), ((CPigpenSwooshEmitter *) pObject)->GetName());
					break;
				default:
					ASSERT(false);
					break;
				}
			}
		}
	}
}

/************************** particle list processing ***************************************/

/************************** view tracking *****************************************/

/* show/hide a list view */

void CPigpenDoc::OnView(int nType) 
{
	if (m_pView[nType] == NULL)
	{
		/* create the correct view for a new document and then attach it to existing document. this seems */
		/* kind of an end-around, but it was the only sample i could find of multiple views of different */
		/* types of a single document, other than using a splitter window */

		nRequestedViewType = nType;	// let CDocView constructor know what we want
		CDocument *pNewDoc = PigpenApp.pDocTemplate->OpenDocumentFile(NULL);

	   /* detach the view from the newly created doc */

		POSITION pos = pNewDoc->GetFirstViewPosition() ;
		m_pView[nType] = (CDocView *) pNewDoc->GetNextView(pos);
		pNewDoc->RemoveView(m_pView[nType]);	// closes new document, since this is its only view

		/* attach it to the open document */

		AddView(m_pView[nType]);

		/* fill in window by calling for an OnUpdate to that view */

		UpdateAllViews(NULL, nNoChangeOtherViews[nType]);
	}
	else
	{
		/* destroy window that contains view */

		CWnd *pWnd = m_pView[nType]->GetParentFrame();
		pWnd->DestroyWindow();
		ViewClosed(nType);
	}
}

/************************** message handlers ***************************************/

void CPigpenDoc::OnUpdateViewEffects(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pView[EFFECTS] != NULL);
}
void CPigpenDoc::OnUpdateViewParticles(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pView[PARTICLES] != NULL);
}
void CPigpenDoc::OnUpdateViewGraphics(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pView[GRAPHICS] != NULL);
}
void CPigpenDoc::OnUpdateViewEmitters(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pView[EMITTERS] != NULL);
}
void CPigpenDoc::OnUpdateViewSwooshes(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pView[SWOOSHES] != NULL);
}
void CPigpenDoc::OnUpdateViewSwooshEmitters(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pView[SWOOSH_EMITTERS] != NULL);
}

void CPigpenDoc::OnViewExportLog(void) 
{
	ASSERT(ValidExportLogFile());	// menu item should not be enabled unless export log is valid
	char Command[200];
	sprintf(Command, "NOTEPAD.EXE %s", GetExportLogFilename());
	WinExec(Command, SW_SHOW);
}
void CPigpenDoc::OnUpdateViewExportLog(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ValidExportLogFile());
}

/* export a .dbl file */

void CPigpenDoc::ExportDBLFile(int nMode, char *pExportFileName /*= NULL*/)
{
	switch(nMode)
	{
	default:
		ASSERT(FALSE);
		// and fall through
	case EXPORT_PC:
		nExportMode = EXPORT_PS2;		// this is sort of the default
		nHackExportMode = EXPORT_PC;	//yuch-- fix this
		break;
	case EXPORT_XBOX:
	case EXPORT_PS2:
		nExportMode = EXPORT_PS2;		// this is sort of the default
		nHackExportMode = EXPORT_PS2;	//yuch-- fix this
		break;
	case EXPORT_GAMECUBE:
		nExportMode = EXPORT_PS2;
		nHackExportMode = EXPORT_GAMECUBE;	//yuch-- fix this
		break;
	}
	ExportDBL(this, m_Arrays, pExportFileName);
}

/* export a .dbl file, pc-style */

void CPigpenDoc::OnFileExportPC() 
{
	ExportDBLFile(EXPORT_PC);
	UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);
}

/* export a .dbl file, ps2-style */

void CPigpenDoc::OnFileExportPS2() 
{
	ExportDBLFile(EXPORT_PS2);
	UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);
}

/* export a .dbl file, gamecube-style (mostly endian swapping, some texture swizzling) */

void CPigpenDoc::OnFileExportGCN() 
{
	ExportDBLFile(EXPORT_GAMECUBE);
	UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);
}

/* update use counts */

void CPigpenDoc::OnFileAnalyze() 
{
	AnalyzeDataForExport(m_Arrays, NULL, NULL);
	UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);
}

/* analyze texture set sizes */

void CPigpenDoc::OnTextureAnalyze()
{
	if (GetGraphicCount() != 0)
	{
		AnalyzeTextureSizes(m_Arrays[GRAPHICS]);
		UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);
	}
}

/* export another file into current open document */

void CPigpenDoc::OnFileMerge() 
{
	int i;

	/* get last file name from registry */

	char InitialPath[_MAX_PATH];
	char Filename[_MAX_PATH];
	LoadPersistentString("Pigpen Merge", InitialPath, sizeof(InitialPath));
	DissectPath(InitialPath, InitialPath, Filename);

	/* file dialog */

	CFileDialog FileDialog(
		TRUE,						// BOOL bOpenFileDialog
		"pig",					// LPCTSTR lpszDefExt = NULL
		Filename,				// LPCTSTR lpszFileName = NULL
		OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,	// DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT-- HIDEREADONLY hides a dialog check box, not read only files
		"Pigpen Files (*.pig)|*.pig||",		// LPCTSTR lpszFilter = NULL, apparently can't have spaces around | symbols
		NULL);					// CWnd* pParentWnd = NULL
	FileDialog.m_ofn.lpstrInitialDir = InitialPath;	// set initial directory
	if (FileDialog.DoModal() != IDOK)
		return;

	/* get file name, save in registry */

	CString newName = FileDialog.GetPathName();
	MakePersistentString("Pigpen Merge", newName.GetBuffer(10));

	/* open file */

	CFile File(newName.GetBuffer(10), CFile::modeRead|CFile::shareDenyWrite);
	if (File.GetLength() == 0)
	{
		File.Close();
		return;
	}

	/* make copies of current arrays-- move entries */

	CObArray SaveArrays[MAX_DOC_DATA_TYPES];
	for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
	{
		SaveArrays[i].InsertAt(0, &m_Arrays[i]);
		m_Arrays[i].RemoveAll();
	}

	/* set up archive and call serialization. this reads in the file as if it were the only data in the document */

	CArchive Archive(&File, CArchive::load | CArchive::bNoFlushOnDelete);
	Archive.m_pDocument = this;
	Serialize(Archive);
	Archive.Close();
	File.Close();

	/* ignore any window location info generated by serialization */

	PigpenApp.SetRestoreWindow(FALSE);

	/* add original arrays into newly loaded arrays */

	for (i = 0; i < MAX_DOC_DATA_TYPES; i++)
	{
		int nOrigArraySize = SaveArrays[i].GetSize();
		m_Arrays[i].InsertAt(0, &SaveArrays[i]);

		/* do unique name check on new items */

		for (int j = nOrigArraySize; j < m_Arrays[i].GetSize(); j++)
		{
			CObject *pObject = m_Arrays[i].GetAt(j);
			EnforceUniqueName(pObject, i);
		}
	}

	/* doc has changed, update views */

	SetModifiedFlag(MODIFIED_FLAG);
	UpdateAllViews(NULL, 0);
}

/* set the associate log file name */

void CPigpenDoc::OnSetAssociateLogFile()
{
	/* Get the data from the persistent strings */

	char InitialPath[_MAX_PATH];
	char Filename[_MAX_PATH] = "";
	LoadPersistentString("Pigpen Associate Log", InitialPath, sizeof(InitialPath));
	if (strlen(InitialPath) != 0)
		DissectPath(InitialPath, InitialPath, Filename);

	/* file dialog */

	CFileDialog FileDialog(
		true,						// BOOL bOpenFileDialog
		"log",					// LPCTSTR lpszDefExt = NULL
		Filename,				// LPCTSTR lpszFileName = NULL
		OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,		// DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT-- HIDEREADONLY hides a dialog check box, not read only files
		"Log Files (*.log)|*.log||",					// LPCTSTR lpszFilter = NULL, apparently can't have spaces around | symbols
		NULL);					// CWnd* pParentWnd = NULL
	FileDialog.m_ofn.lpstrInitialDir = InitialPath;	// set initial directory
	if (FileDialog.DoModal() != IDOK)
		return;

	/* remember file name */

	MakePersistentString("Pigpen Associate Log", FileDialog.GetPathName().GetBuffer(10));
}