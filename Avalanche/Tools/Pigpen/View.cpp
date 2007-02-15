/************************************/
/*                                  */
/* View.cpp                         */
/* Pigpen particle tool  12/07/99   */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* particle list view               */
/*                                  */
/************************************/

/* the main view windows for the particle tool are all list views, so unify them to make it easier to
update them all at the same time. the only bad thing is that the constructor relies on a global variable to
determine which type of view should be created, since it is called through the doc template mechanism and i can't
specify a parameter (at least i don't know how). the list view (CListView) style allows you to have columns */

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

/******************* local variables *********************/

/******************* externs *****************************/

extern int nRequestedViewType;

/******************* class functions *********************/

/////////////////////////////////////////////////////////////////////////////
// CDocView

IMPLEMENT_DYNCREATE(CDocView, CListView)

BEGIN_MESSAGE_MAP(CDocView, CListView)
	//{{AFX_MSG_MAP(CDocView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_NEW_EFFECT, OnNew)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDITA, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_COMMAND(ID_EFFECT_DIALOG, OnEdit0)
	ON_COMMAND(ID_EDIT_CUT, OnCut)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_COMMAND(ID_MOVE_TO_TOP, OnMoveToTop)
	ON_COMMAND(ID_MOVE_TO_BOTTOM, OnMoveToBottom)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_COMMAND(ID_NEW_PARTICLE, OnNew)
	ON_COMMAND(ID_NEW_GRAPHIC, OnNew)
	ON_COMMAND(ID_NEW_EMITTER, OnNew)
	ON_COMMAND(ID_NEW_SWOOSH, OnNew)
	ON_COMMAND(ID_NEW_SWOOSH_EMITTER, OnNew)
	ON_COMMAND(ID_PARTICLE_DIALOG, OnEdit0)
	ON_COMMAND(ID_GRAPHIC_DIALOG, OnEdit0)
	ON_COMMAND(ID_EMITTER_DIALOG, OnEdit0)
	ON_COMMAND(ID_SWOOSH_DIALOG, OnEdit0)
	ON_COMMAND(ID_SWOOSH_EMITTER_DIALOG, OnEdit0)
	ON_COMMAND(ID_PREVIEW, OnPreview)
	ON_COMMAND(ID_FINDASSOCIATES, OnFindAssociates)
	ON_COMMAND(ID_REPAIR_CLEARSTAGE, OnRepairClearStage)
	ON_COMMAND(ID_REPAIR_CLEAREMITTERMODEL, OnRepairClearEmitterModel)
	ON_COMMAND(ID_CLOSE_ALLBUTEFFECTS, OnCloseAllButEffects)
	ON_COMMAND(ID_CLOSE_ALLBUTGRAPHICS, OnCloseAllButGraphics)
	ON_COMMAND(ID_CLOSE_ALLBUTPARTICLES, OnCloseAllButParticles)
	ON_COMMAND(ID_CLOSE_ALLBUTEMITTERS, OnCloseAllButEmitters)
	ON_COMMAND(ID_CLOSE_ALLBUTSWOOSHES, OnCloseAllButSwooshes)
	ON_COMMAND(ID_CLOSE_ALLBUTSWOOSHEMITTERS, OnCloseAllButSwooshEmitters)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_ALLBUTEFFECTS, OnUpdateCloseAllButEffects)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_ALLBUTGRAPHICS, OnUpdateCloseAllButGraphics)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_ALLBUTPARTICLES, OnUpdateCloseAllButParticles)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_ALLBUTEMITTERS, OnUpdateCloseAllButEmitters)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_ALLBUTSWOOSHES, OnUpdateCloseAllButSwooshes)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_ALLBUTSWOOSHEMITTERS, OnUpdateCloseAllButSwooshEmitters)
	//}}AFX_MSG_MAP
	ON_NOTIFY_RANGE(LVN_COLUMNCLICK,0,0xffff,OnColumnClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocView construction/destruction

CDocView::CDocView()
{
	m_dwDefaultStyle |= LVS_REPORT | LVS_EDITLABELS;	// we want the report style list view, editable labels

	/* no child windows yet */

	for (int i = 0; i < MAX_EDIT_WINDOWS_PER_VIEW; i++)
		m_pEditWindow[i] = NULL;

	/* no clipboard format */

	m_nClipboardFormat = 0;
	m_nClipboardRecordSize = 0;

	/* keep track of status for text cut and paste */

	m_labelEditActive = false;
}

CDocView::~CDocView()
{
}

/* from help: "Called by the framework after the view is first attached to the document, but before the
view is initially displayed. The default implementation of this function calls the OnUpdate member function
with no hint information (that is, using the default values of 0 for the lHint parameter and NULL for the
pHint parameter). Override this function to perform any one-time initialization that requires information
about the document"

use this opportunity to add columns to the list view */

void CDocView::OnInitialUpdate()
{
	/* what flavor? because the view is constructed as part of the doc template process, i use a global */
	/* variable to pass this */

	m_nType = nRequestedViewType;	// checked in OnInitialUpdate, below
//m_nType = PARTICLES;	// for development of particles dialog
	nRequestedViewType = EFFECTS;	// back to default

	/* first call the base class function */

	CListView::OnInitialUpdate();

	/* add columns to list view */

	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	switch(m_nType)
	{
	case EFFECTS:
//HDS_FULLDRAG?
		List.InsertColumn(EFFECT_NAME_COLUMN,						"Name", LVCFMT_LEFT, 150);
		List.InsertColumn(EFFECT_PART1_COLUMN,						"Channel 1", LVCFMT_LEFT, 120);
		List.InsertColumn(EFFECT_PART2_COLUMN,						"Channel 2", LVCFMT_LEFT, 120);
		List.InsertColumn(EFFECT_PART3_COLUMN,						"Channel 3", LVCFMT_LEFT, 120);
		List.InsertColumn(EFFECT_PART4_COLUMN,						"Channel 4", LVCFMT_LEFT, 120);
		List.InsertColumn(EFFECT_PART5_COLUMN,						"Channel 5", LVCFMT_LEFT, 120);
		List.InsertColumn(EFFECT_ASSOCIATED_COLUMN,				"Assoc.", LVCFMT_LEFT, 50);
		List.InsertColumn(EFFECT_USED_COUNT_COLUMN,				"Used", LVCFMT_LEFT, 50);
		break;
	case PARTICLES:
		List.InsertColumn(PARTICLE_NAME_COLUMN,					"Name", LVCFMT_LEFT, 150);
		List.InsertColumn(PARTICLE_GRAPHIC_COLUMN,				"Graphic", LVCFMT_LEFT, 150);
		List.InsertColumn(PARTICLE_ASSOCIATED_COLUMN,			"Assoc.", LVCFMT_LEFT, 50);
		List.InsertColumn(PARTICLE_USED_COUNT_COLUMN,			"Used", LVCFMT_LEFT, 50);
		break;
	case GRAPHICS:
		List.InsertColumn(GRAPHIC_NAME_COLUMN,						"Name", LVCFMT_LEFT, 150);
		List.InsertColumn(GRAPHIC_FRAMES_COLUMN,					"Frames", LVCFMT_LEFT, 75);
		List.InsertColumn(GRAPHIC_SIZE_COLUMN,						"Size", LVCFMT_LEFT, 100);
		List.InsertColumn(GRAPHIC_IMPORT_PATH_COLUMN,			"Import path", LVCFMT_LEFT, 250);
		List.InsertColumn(GRAPHIC_USED_COUNT_COLUMN,				"Used", LVCFMT_LEFT, 50);
		List.InsertColumn(GRAPHIC_EXPORT_SIZE_COLUMN,			"Export", LVCFMT_LEFT, 75);
		break;
	case EMITTERS:
		List.InsertColumn(EMITTER_NAME_COLUMN,						"Name", LVCFMT_LEFT, 150);
		List.InsertColumn(EMITTER_EFFECT_COLUMN,					"Effect", LVCFMT_LEFT, 120);
		List.InsertColumn(EMITTER_ASSOCIATED_COLUMN,				"Assoc.", LVCFMT_LEFT, 50);
		List.InsertColumn(EMITTER_USED_COUNT_COLUMN,				"Used", LVCFMT_LEFT, 50);
		break;
	case SWOOSHES:
		List.InsertColumn(SWOOSH_NAME_COLUMN,						"Name", LVCFMT_LEFT, 150);
		List.InsertColumn(SWOOSH_GRAPHIC_COLUMN,					"Graphic", LVCFMT_LEFT, 150);
		List.InsertColumn(SWOOSH_ASSOCIATED_COLUMN,				"Assoc.", LVCFMT_LEFT, 50);
		List.InsertColumn(SWOOSH_USED_COUNT_COLUMN,				"Used", LVCFMT_LEFT, 50);
		break;
	case SWOOSH_EMITTERS:
		List.InsertColumn(SWOOSH_EMITTER_NAME_COLUMN,			"Name", LVCFMT_LEFT, 150);
		List.InsertColumn(SWOOSH_EMITTER_SWOOSH_COLUMN,			"Swoosh", LVCFMT_LEFT, 150);
		List.InsertColumn(SWOOSH_EMITTER_ASSOCIATED_COLUMN,	"Assoc.", LVCFMT_LEFT, 50);
		List.InsertColumn(SWOOSH_EMITTER_USED_COUNT_COLUMN,	"Used", LVCFMT_LEFT, 50);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	/* register clipboard format for this view */

	switch(m_nType)
	{
	case EMITTERS:
		m_nClipboardFormat = RegisterClipboardFormat("PigpenEmitter");
		ASSERT(m_nClipboardFormat != 0);	// if this is 0, RegisterClipboardFormat failed
		m_nClipboardRecordSize = sizeof(CEmitter);
		break;
	case EFFECTS:
		m_nClipboardFormat = RegisterClipboardFormat("PigpenEffect");
		ASSERT(m_nClipboardFormat != 0);	// if this is 0, RegisterClipboardFormat failed
		m_nClipboardRecordSize = sizeof(CEffect);
		break;
	case PARTICLES:
		m_nClipboardFormat = RegisterClipboardFormat("PigpenParticle");
		ASSERT(m_nClipboardFormat != 0);	// if this is 0, RegisterClipboardFormat failed
		m_nClipboardRecordSize = sizeof(CParticle) + CParticle::GetSurfaceCount() * sizeof(CSpawn);
		break;
	case GRAPHICS:
		// no clipboard functions for graphic images yet
		break;
	case SWOOSHES:
		m_nClipboardFormat = RegisterClipboardFormat("PigpenSwoosh");
		ASSERT(m_nClipboardFormat != 0);	// if this is 0, RegisterClipboardFormat failed
		m_nClipboardRecordSize = sizeof(CSwoosh);
		break;
	case SWOOSH_EMITTERS:
		m_nClipboardFormat = RegisterClipboardFormat("PigpenSwooshEmitter");
		ASSERT(m_nClipboardFormat != 0);	// if this is 0, RegisterClipboardFormat failed
		m_nClipboardRecordSize = sizeof(CPigpenSwooshEmitter);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	/* mark as open in doc */

	CPigpenDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->ViewOpen(this, m_nType);

	/* show document contents */

	OnUpdate(NULL, 0, NULL);

	/* size window as requested on file load */

	if (PigpenApp.GetRestoreWindow())
	{
		/* coords are relative to main app window's client area */

		CRect Rect = PigpenApp.GetRestoreWindowRect();
		GetParentFrame()->SetWindowPos(&wndTop, Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, 0);
//yuch-- scroll setting restore does not work-- i can force the scroll bars to move to the correct place, but it
// doesn't change the view of the window contents. there has to be another way...even if i set the scroll bar
// positions here, they get reset when the window is initially drawn. i was able to force the scroll bars to move by
// calling SetScrollInfo after the first loop in OpenLoadedWindows, but that didn't scroll the visible data...
		SCROLLINFO *pScrollInfo = PigpenApp.GetRestoreWindowHorzScroll();
		SetScrollInfo(SB_HORZ, pScrollInfo, TRUE);
		pScrollInfo = PigpenApp.GetRestoreWindowVertScroll();
		SetScrollInfo(SB_VERT, pScrollInfo, TRUE);
		PigpenApp.SetRestoreWindow(FALSE);	// this is a one-shot
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDocView operations

/* add or refresh item (effect, particle, etc) from doc's list to view */

void CDocView::ItemToView(int nIndex, bool bAdd)
{
	/* add data to end of view */

	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	int nListIndex;
	if (bAdd)
		nListIndex = List.GetItemCount();		// add item to end of existing list
	else
	{
		ASSERT(nIndex < List.GetItemCount());	// refresh item in place
		nListIndex = nIndex;
	}
	switch(m_nType)
	{
	case EFFECTS:
		{
			CEffect *pEffect = GetDocument()->GetEffect(nIndex);
			if (bAdd)
				List.InsertItem(nListIndex, pEffect->GetName());
			else
				List.SetItemText(nListIndex, EFFECT_NAME_COLUMN, pEffect->GetName());
			for (int i = EFFECT_PART1_COLUMN - EFFECT_PART1_COLUMN; i <= EFFECT_PART5_COLUMN - EFFECT_PART1_COLUMN; i++)
			{
				ASSERT(i < MAX_CHANNELS_PER_EFFECT);	// there aren't as many columns in the view window as there are channels
				if (pEffect->ChannelIsSound(i))
					List.SetItemText(nListIndex, EFFECT_PART1_COLUMN + i, pEffect->GetChannelSoundName(i));
				else
				{
					CParticle *pParticle = pEffect->GetChannelParticle(i);
					List.SetItemText(nListIndex, EFFECT_PART1_COLUMN + i, pParticle == NULL ? "None" : pParticle->GetName());
				}
			}
			List.SetItemText(nListIndex, EFFECT_ASSOCIATED_COLUMN, pEffect->IsAssociated() ? "    x" : "");
			List.SetItemText(nListIndex, EFFECT_USED_COUNT_COLUMN, (pEffect->GetUsedCount() == -1) ? "" : ToCString(pEffect->GetUsedCount()));
		}
		break;
	case PARTICLES:
		{
			CParticle *pParticle = GetDocument()->GetParticle(nIndex);
			if (bAdd)
				List.InsertItem(nListIndex, pParticle->GetName());
			else
				List.SetItemText(nListIndex, PARTICLE_NAME_COLUMN, pParticle->GetName());
			List.SetItemText(nListIndex, PARTICLE_GRAPHIC_COLUMN, pParticle->GetGraphicName());
			List.SetItemText(nListIndex, PARTICLE_ASSOCIATED_COLUMN, pParticle->IsAssociated() ? "    x" : "");
			List.SetItemText(nListIndex, PARTICLE_USED_COUNT_COLUMN, (pParticle->GetUsedCount() == -1) ? "" : ToCString(pParticle->GetUsedCount()));
		}
		break;
	case GRAPHICS:
		{
			CGraphic *pGraphic = GetDocument()->GetGraphic(nIndex);
			if (bAdd)
				List.InsertItem(nListIndex, pGraphic->GetName());
			else
				List.SetItemText(nListIndex, GRAPHIC_NAME_COLUMN, pGraphic->GetName());
			List.SetItemText(nListIndex, GRAPHIC_FRAMES_COLUMN, ToCString(pGraphic->GetImageCount()));
			List.SetItemText(nListIndex, GRAPHIC_SIZE_COLUMN, ToCString(pGraphic->GetWidth()) + " x " + ToCString(pGraphic->GetHeight()));
			List.SetItemText(nListIndex, GRAPHIC_IMPORT_PATH_COLUMN, pGraphic->GetBaseSourceFile());
			List.SetItemText(nListIndex, GRAPHIC_USED_COUNT_COLUMN, (pGraphic->GetUsedCount() == -1) ? "" : ToCString(pGraphic->GetUsedCount()));
			List.SetItemText(nListIndex, GRAPHIC_EXPORT_SIZE_COLUMN, (pGraphic->GetTextureSetSize() == -1) ? "" : ToCString(pGraphic->GetTextureSetSize()));
		}
		break;
	case EMITTERS:
		{
			CEmitter *pEmitter = GetDocument()->GetEmitter(nIndex);
			if (bAdd)
				List.InsertItem(nListIndex, pEmitter->GetName());
			else
				List.SetItemText(nListIndex, EMITTER_NAME_COLUMN, pEmitter->GetName());
			CEffect *pEffect = pEmitter->GetEffect();
			List.SetItemText(nListIndex, EMITTER_EFFECT_COLUMN, pEffect == NULL ? "None" : pEffect->GetName());
			List.SetItemText(nListIndex, EMITTER_ASSOCIATED_COLUMN, pEmitter->IsAssociated() ? "    x" : "");
			List.SetItemText(nListIndex, EMITTER_USED_COUNT_COLUMN, (pEmitter->GetUsedCount() == -1) ? "" : ToCString(pEmitter->GetUsedCount()));
		}
		break;
	case SWOOSHES:
		{
			CSwoosh *pSwoosh = GetDocument()->GetSwoosh(nIndex);
			if (bAdd)
				List.InsertItem(nListIndex, pSwoosh->GetName());
			else
				List.SetItemText(nListIndex, SWOOSH_NAME_COLUMN, pSwoosh->GetName());
			List.SetItemText(nListIndex, SWOOSH_GRAPHIC_COLUMN, pSwoosh->GetGraphicName());
			List.SetItemText(nListIndex, SWOOSH_ASSOCIATED_COLUMN, pSwoosh->IsAssociated() ? "    x" : "");
			List.SetItemText(nListIndex, SWOOSH_USED_COUNT_COLUMN, (pSwoosh->GetUsedCount() == -1) ? "" : ToCString(pSwoosh->GetUsedCount()));
		}
		break;
	case SWOOSH_EMITTERS:
		{
			CPigpenSwooshEmitter *pSwooshEmitter = GetDocument()->GetSwooshEmitter(nIndex);
			if (bAdd)
				List.InsertItem(nListIndex, pSwooshEmitter->GetName());
			else
				List.SetItemText(nListIndex, SWOOSH_EMITTER_NAME_COLUMN, pSwooshEmitter->GetName());
			List.SetItemText(nListIndex, SWOOSH_EMITTER_SWOOSH_COLUMN, pSwooshEmitter->GetSwooshName());
			List.SetItemText(nListIndex, SWOOSH_EMITTER_ASSOCIATED_COLUMN, pSwooshEmitter->IsAssociated() ? "    x" : "");
			List.SetItemText(nListIndex, SWOOSH_EMITTER_USED_COUNT_COLUMN, (pSwooshEmitter->GetUsedCount() == -1) ? "" : ToCString(pSwooshEmitter->GetUsedCount()));
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

/* get the index of the first selected item, -1 if none selected */

int CDocView::GetSelectedItemIndex(void)
{
	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	POSITION pos = List.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return(-1);	// no item selected
   return(List.GetNextSelectedItem(pos));
}

/* update editing window (dialog or property sheet) when item selection changes */

void CDocView::UpdateEditWindow(void)
{
	for (int i = 0; i < MAX_EDIT_WINDOWS_PER_VIEW; i++)
	{
		if (m_pEditWindow[i])
		{
			/* get pointer to selected item, NULL if no item selected */

			CObject *pItem = NULL;
			int nIndex = GetSelectedItemIndex();
			if (nIndex != -1)
			{
				CPigpenDoc* pDoc = GetDocument();
				ASSERT_VALID(pDoc);
				pItem = pDoc->GetArrayItem(m_nType, nIndex);
			}

			/* update edit dialog contents, NULL item means clear it */

			switch(GetEditWindowType(i))
			{
			case EFFECT_DIALOG:
				((CEffectDialog *) m_pEditWindow[i])->ShowData((CEffect *) pItem);
				break;
			case PARTICLE_DIALOG:
				((CParticleSheet *) m_pEditWindow[i])->ShowData((CParticle *) pItem);
				break;
			case GRAPHIC_DIALOG:
				((CGraphicDialog *) m_pEditWindow[i])->ShowData((CGraphic *) pItem);
				break;
			case EMITTER_DIALOG:
				((CEmitterDialog *) m_pEditWindow[i])->ShowData((CEmitter *) pItem);
				break;
			case SWOOSH_DIALOG:
				((CSwooshSheet *) m_pEditWindow[i])->ShowData((CSwoosh *) pItem);
				break;
			case SWOOSH_EMITTER_DIALOG:
				((CSwooshEmitterDialog *) m_pEditWindow[i])->ShowData((CPigpenSwooshEmitter *) pItem);
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
	}
}

/* receive a message from edit window that is is closing */

void CDocView::EditWindowClosed(int nEditWindow)
{
	m_pEditWindow[nEditWindow] = NULL;
}

/* how many edit window open for this view? */

int CDocView::EditWindowOpenCount()
{
	int nCount = 0;
	for (int i = 0; i < MAX_EDIT_WINDOWS_PER_VIEW; i++)
	{
		if (m_pEditWindow[i])
			nCount++;
	}
	return(nCount);
}

/////////////////////////////////////////////////////////////////////////////
// CDocView diagnostics

#ifdef _DEBUG
void CDocView::AssertValid() const
{
	CListView::AssertValid();
}

void CDocView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CPigpenDoc* CDocView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPigpenDoc)));
	return (CPigpenDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocView message handlers

/* handle popup menu for list view */

void CDocView::OnContextMenu(CWnd* pWnd, CPoint Point) 
{
	static int nEditWindowID[MAX_DOC_DATA_TYPES][MAX_EDIT_WINDOWS_PER_VIEW] = {
		{ ID_EFFECT_DIALOG, -1},
		{ ID_PARTICLE_DIALOG, -1},
		{ ID_GRAPHIC_DIALOG, -1},
		{ ID_EMITTER_DIALOG, -1},
		{ ID_SWOOSH_DIALOG, -1},
		{ ID_SWOOSH_EMITTER_DIALOG, -1}};

	CMenu PopupMenu;
	if (PopupMenu.LoadMenu(IDR_POPUP_MENUS))
	{
		CMenu *pSubMenu = PopupMenu.GetSubMenu(m_nType);

		/* some menu items depend on number of selected items */

		int nCount = GetListCtrl().GetSelectedCount();
		pSubMenu->EnableMenuItem(ID_DELETE, (nCount != 0) ? MF_ENABLED : MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_RENAME, (nCount == 1) ? MF_ENABLED : MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_MOVE_TO_TOP, (nCount != 0) ? MF_ENABLED : MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_MOVE_TO_BOTTOM, (nCount != 0) ? MF_ENABLED : MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_PREVIEW, (nCount == 1) ? MF_ENABLED : MF_GRAYED);

		/* cut, copy, paste */

		pSubMenu->EnableMenuItem(ID_EDIT_CUT, (nCount != 0) ? MF_ENABLED : MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_EDIT_COPY, (nCount != 0) ? MF_ENABLED : MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_EDIT_PASTE, IsClipboardFormatAvailable(m_nClipboardFormat) ? MF_ENABLED : MF_GRAYED);

		/* associated dialog */

		for (int i = 0; i < MAX_EDIT_WINDOWS_PER_VIEW; i++)
		{
			if (nEditWindowID[m_nType][i] != -1)
				pSubMenu->CheckMenuItem(nEditWindowID[m_nType][i], (m_pEditWindow[i] == NULL) ? MF_UNCHECKED : MF_CHECKED);
		}

		/* views */

		CPigpenDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pSubMenu->CheckMenuItem(ID_VIEW_EFFECTS, (pDoc->GetView(EFFECTS) == NULL) ? MF_UNCHECKED : MF_CHECKED);
		pSubMenu->CheckMenuItem(ID_VIEW_PARTICLES, (pDoc->GetView(PARTICLES) == NULL) ? MF_UNCHECKED : MF_CHECKED);
		pSubMenu->CheckMenuItem(ID_VIEW_GRAPHICS, (pDoc->GetView(GRAPHICS) == NULL) ? MF_UNCHECKED : MF_CHECKED);
		pSubMenu->CheckMenuItem(ID_VIEW_EMITTERS, (pDoc->GetView(EMITTERS) == NULL) ? MF_UNCHECKED : MF_CHECKED);
		pSubMenu->CheckMenuItem(ID_VIEW_SWOOSHES, (pDoc->GetView(SWOOSHES) == NULL) ? MF_UNCHECKED : MF_CHECKED);
		pSubMenu->CheckMenuItem(ID_VIEW_SWOOSH_EMITTERS, (pDoc->GetView(SWOOSH_EMITTERS) == NULL) ? MF_UNCHECKED : MF_CHECKED);

		/* show the menu */

		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, this);
	}
}

/* add a new item to the document */

void CDocView::OnNew()
{
	NewDocument();
}

CObject *CDocView::NewDocument()
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CObject *pObject = pDoc->NewItem(m_nType);
	return(pObject);
}

/* called when a column header is clicked */

void CDocView::OnColumnClick(UINT id, NMHDR* pNotifyStruct, LRESULT* pResult)
{
	/* doc's array should be same length as list */

	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& List = GetListCtrl();
	ASSERT(List.GetItemCount() == pDoc->GetArraySize(m_nType));
	if (List.GetItemCount() < 2)
		return;	// no sort necessary

	/* sort array */

	pDoc->SortArray(m_nType, reinterpret_cast<NMLISTVIEW*>(pNotifyStruct)->iSubItem);
}

/* called when user starts to edit name of item in view window */

void CDocView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	/* keep track of status for text cut and paste */

	m_labelEditActive = true;

	/* ok */

	*pResult = 0;
}

/* called when user hits enter after editing name of item in view window */

void CDocView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* keep track of status for text cut and paste */

	m_labelEditActive = false;

	/* apply changes */

	LV_DISPINFO *pDispInfo = (LV_DISPINFO *) pNMHDR;
	LV_ITEM *pItem = &pDispInfo->item;
	if (pItem->pszText != NULL)
	{
		CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
		List.SetItemText(pItem->iItem, pItem->iSubItem, pItem->pszText);

		/* update name in doc, too */

		CPigpenDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->RenameItem(m_nType, pItem->iItem, pItem->pszText);
	}
	*pResult = 0;
}

/* toggle edit box 0 or 1 individually (each view may have multiple edit windows) */

void CDocView::OnEdit0()
{
	OnEdit(EDIT_WINDOW0);
}
void CDocView::OnEdit1()
{
	OnEdit(EDIT_WINDOW1);
}

/* return type of edit window associated with this view and index */

int CDocView::GetEditWindowType(int nEditWindow)
{
	switch(m_nType) 
	{
	case EFFECTS:
		switch(nEditWindow)
		{
		case EDIT_WINDOW0:
			return(EFFECT_DIALOG);
		default:
			return(NO_DIALOG);
		}
		break;
	case PARTICLES:
		switch(nEditWindow)
		{
		case EDIT_WINDOW0:
			return(PARTICLE_DIALOG);
		default:
			return(NO_DIALOG);
		}
		break;
	case GRAPHICS:
		switch(nEditWindow)
		{
		case EDIT_WINDOW0:
			return(GRAPHIC_DIALOG);
		default:
			return(NO_DIALOG);
		}
		break;
	case EMITTERS:
		switch(nEditWindow)
		{
		case EDIT_WINDOW0:
			return(EMITTER_DIALOG);
		default:
			return(NO_DIALOG);
		}
		break;
	case SWOOSHES:
		switch(nEditWindow)
		{
		case EDIT_WINDOW0:
			return(SWOOSH_DIALOG);
		default:
			return(NO_DIALOG);
		}
		break;
	case SWOOSH_EMITTERS:
		switch(nEditWindow)
		{
		case EDIT_WINDOW0:
			return(SWOOSH_EMITTER_DIALOG);
		default:
			return(NO_DIALOG);
		}
		break;
	default:
		ASSERT(FALSE);
		return(NO_DIALOG);
	}
}

/* force selection of a particular record */

void CDocView::FindAndSelectItem(CObject *pItemRecord)
{
	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	for (int i = 0; i < List.GetItemCount(); i++)
	{
		if (pItemRecord == GetDocument()->GetArrayItem(m_nType, i))
			List.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		else
			List.SetItemState(i, 0, LVIS_SELECTED);
	}
}

/* toggle edit dialog box */

void CDocView::OnEdit(int nEditWindow) 
{
	int nEditWindowType = GetEditWindowType(nEditWindow);
	if (nEditWindowType == NO_DIALOG)
		return;
	if (m_pEditWindow[nEditWindow] == NULL)
	{
		switch(nEditWindowType) 
		{
		case EFFECT_DIALOG:
			m_pEditWindow[nEditWindow] = (void *) new CEffectDialog(this, GetParentFrame());
			((CEffectDialog *) m_pEditWindow[nEditWindow])->Create();
			break;
		case PARTICLE_DIALOG:
			m_pEditWindow[nEditWindow] = (void *) new CParticleSheet(this, GetParentFrame());
			((CParticleSheet *) m_pEditWindow[nEditWindow])->Create();
			break;
		case GRAPHIC_DIALOG:
			m_pEditWindow[nEditWindow] = (void *) new CGraphicDialog(this, GetParentFrame());
			((CGraphicDialog *) m_pEditWindow[nEditWindow])->Create();
			break;
		case EMITTER_DIALOG:
			m_pEditWindow[nEditWindow] = (void *) new CEmitterDialog(this, GetParentFrame());
			((CEmitterDialog *) m_pEditWindow[nEditWindow])->Create();
			break;
		case SWOOSH_DIALOG:
			m_pEditWindow[nEditWindow] = (void *) new CSwooshSheet(this, GetParentFrame());
			((CSwooshSheet *) m_pEditWindow[nEditWindow])->Create();
			break;
		case SWOOSH_EMITTER_DIALOG:
			m_pEditWindow[nEditWindow] = (void *) new CSwooshEmitterDialog(this, GetParentFrame());
			((CSwooshEmitterDialog *) m_pEditWindow[nEditWindow])->Create();
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		UpdateEditWindow();

		/* size window as requested on file load */

		if (PigpenApp.GetRestoreWindow())
		{
			/* coords are in screen coords (modeless dialogs can be moved around all over the place). edit windows */
			/* are not sizeable, so get correct height and width to use (may changes as versions change) */

			CRect CurrentRect;
			((CWnd *) m_pEditWindow[nEditWindow])->GetWindowRect(&CurrentRect);
			CRect Rect = PigpenApp.GetRestoreWindowRect();
			((CWnd *) m_pEditWindow[nEditWindow])->SetWindowPos(&wndTop, Rect.left, Rect.top, CurrentRect.right - CurrentRect.left, CurrentRect.bottom - CurrentRect.top, 0);
			PigpenApp.SetRestoreWindow(FALSE);	// this is a one-shot
		}
	}
	else
	{
		switch(nEditWindowType) 
		{
		case EFFECT_DIALOG:
			((CEffectDialog *) m_pEditWindow[nEditWindow])->DestroyWindow();
			break;
		case PARTICLE_DIALOG:
			((CParticleSheet *) m_pEditWindow[nEditWindow])->DestroyWindow();
			break;
		case GRAPHIC_DIALOG:
			((CGraphicDialog *) m_pEditWindow[nEditWindow])->DestroyWindow();
			break;
		case EMITTER_DIALOG:
			((CEmitterDialog *) m_pEditWindow[nEditWindow])->DestroyWindow();
			break;
		case SWOOSH_DIALOG:
			((CSwooshSheet *) m_pEditWindow[nEditWindow])->DestroyWindow();
			break;
		case SWOOSH_EMITTER_DIALOG:
			((CSwooshEmitterDialog *) m_pEditWindow[nEditWindow])->DestroyWindow();
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		m_pEditWindow[nEditWindow] = NULL;
	}
}

/* cut the currently-selected item(s) to the clipboard */

void CDocView::OnCut() 
{
	DoCut();
}

BOOL CDocView::DoCut()
{
	/* text? */

	if (m_labelEditActive)
		return(CutText());

	/* copy to clipboard */

	if (!DoCopy())
		return(FALSE);

	/* copy was successful, delete items */

	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	pDoc->DeleteSelectedItems(m_nType, List);
	return(TRUE);
}

/* copy the currently-selected item(s) to the clipboard */

void CDocView::OnCopy() 
{
	DoCopy();
}

BOOL CDocView::DoCopy()
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	ASSERT(List.GetSelectedCount() != 0);	// must be some selected items to copy

	/* text? */

	if (m_labelEditActive)
		return(CopyText());

	/* we should have a valid clipboard format at this point */

	ASSERT(m_nClipboardFormat != 0);
	ASSERT(m_nClipboardRecordSize != 0);
	if (m_nClipboardFormat == 0)
		return(FALSE);

	/* collect selected items */

	int nIndexCount;
	int *pIndexArray = pDoc->CollectSelectedItems(m_nType, List, nIndexCount);
	ASSERT(pIndexArray);
	if (pIndexArray == NULL)
		return(FALSE);

	/* how much room do we need to hold data? */

	int nBufferSize = sizeof(int);	// room for record count
	nBufferSize += nIndexCount * m_nClipboardRecordSize;
//yuch-- when this includes all data necessary to move data between docs, remove bItemsDeleted check
// in OnDelete that invalidates clipboard data

	/* open clipboard */

	if (OpenClipboard() == 0)
		return(FALSE);

	/* empty clipboard-- these clears it out-- only one thing lives in the clipboard at once */

	EmptyClipboard();

	/* allocate memory to hold data */

	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, nBufferSize);
	if (hBuffer == NULL) 
	{ 
		ASSERT(FALSE);	// shouldn't happen
		CloseClipboard(); 
		return(FALSE);
	} 

	/* add data to buffer */

	char *pBuffer = (char *) GlobalLock(hBuffer);
	ASSERT(pBuffer);
	memcpy(pBuffer, &nIndexCount, sizeof(int));
	pBuffer += sizeof(int);
	for (int i = 0; i < nIndexCount; i++)
	{
		/* point to record, prepare for memcpy */

		CObject *pObject = pDoc->GetArrayItem(m_nType, pIndexArray[i]);
		switch(m_nType)
		{
		case EMITTERS:
			((CEmitter *) pObject)->PrepareForMemcpy();
			break;
		case EFFECTS:
			((CEffect *) pObject)->PrepareForMemcpy();
			break;
		case PARTICLES:
			((CParticle *) pObject)->PrepareForMemcpy();
			break;
		case SWOOSHES:
			((CSwoosh *) pObject)->PrepareForMemcpy();
			break;
		case SWOOSH_EMITTERS:
			((CPigpenSwooshEmitter *) pObject)->PrepareForMemcpy();
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* do the memcpy. the = operator might be better here, but it is problematic since the buffer memory */
		/* is not really a particle or an effect (or whatever) record. if we could call constructor on each */
		/* chunk of buffer memory so that it WAS a record of the right type, we might be able to get away */
		/* with just calling the = operator (wouldn't need the explicit PrepareForMemcpy/RestoreAfterMemcpy */
		/* calls since they are part of the = operator). however, the constructor for particles (for example) */
		/* does its own malloc's, etc, which create their own problems. the best solution, i think, is to */
		/* have the clipboard buffer represent an image of a serialization of the chosen records-- that's the */
		/* most complete and memory-independent data (e.g. no pointers). however, that would require substantial */
		/* work-- for one thing, we couldn't pre-calculate needed buffer size. for another, the serialization */
		/* process deals with pointers on a whole-database basis-- for example, effects have pointers to particles, */
		/* and those pointers are translated to indices with the expectation that all particle records will be saved */
		/* out to the same file as the effect records. but if we ever want to be able to copy/paste between files, */
		/* the pseudo-serialization method is the only thing that will work... */

		/*** DANGER! since we ARE using memcpy, the buffer will contain pointers (e.g. the effect record contains ***/
		/*** a CParticle pointer). this means that copying/pasting will only work within the same file, and that ***/
		/*** pasting will fail if you delete records between coying/pasting. it's not a happy thing right now ***/

		int nCopySize = 0;
		switch(m_nType)
		{
		case EMITTERS:
			{
				CEmitter *pEmitter = (CEmitter *) pObject;
				memcpy(pBuffer, pEmitter, sizeof(CEmitter));
				pBuffer += sizeof(CEmitter);
				nCopySize = sizeof(CEmitter);
			}
			break;
		case EFFECTS:
			{
				CEffect *pEffect = (CEffect *) pObject;
				memcpy(pBuffer, pEffect, sizeof(CEffect));
				pBuffer += sizeof(CEffect);
				nCopySize = sizeof(CEffect);
			}
			break;
		case PARTICLES:
			{
				CParticle *pParticle = (CParticle *) pObject;
				memcpy(pBuffer, pParticle, sizeof(CParticle));
				pBuffer += sizeof(CParticle);
				nCopySize += sizeof(CParticle);

				/* and now add each spawn record */

				for (int j = 0; j < pParticle->GetSpawnCount(); j++)
				{
					memcpy(pBuffer, pParticle->GetSpawn(j), sizeof(CSpawn));
					pBuffer += sizeof(CSpawn);
					nCopySize += sizeof(CSpawn);
				}
			}
			break;
		case SWOOSHES:
			{
				CSwoosh *pSwoosh = (CSwoosh *) pObject;
				memcpy(pBuffer, pSwoosh, sizeof(CSwoosh));
				pBuffer += sizeof(CSwoosh);
				nCopySize = sizeof(CSwoosh);
			}
			break;
		case SWOOSH_EMITTERS:
			{
				CPigpenSwooshEmitter *pSwooshEmitter = (CPigpenSwooshEmitter *) pObject;
				memcpy(pBuffer, pSwooshEmitter, sizeof(CPigpenSwooshEmitter));
				pBuffer += sizeof(CPigpenSwooshEmitter);
				nCopySize = sizeof(CPigpenSwooshEmitter);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		ASSERT(nCopySize == m_nClipboardRecordSize);	// if this hits, see comment in CPigpenDoc::CPigpenDoc, look for BROKEN

		/* restore after memcpy */

		switch(m_nType)
		{
		case EMITTERS:
			((CEmitter *) pObject)->RestoreAfterMemcpy();
			break;
		case EFFECTS:
			((CEffect *) pObject)->RestoreAfterMemcpy();
			break;
		case PARTICLES:
			((CParticle *) pObject)->RestoreAfterMemcpy();
			break;
		case SWOOSHES:
			((CSwoosh *) pObject)->RestoreAfterMemcpy();
			break;
		case SWOOSH_EMITTERS:
			((CPigpenSwooshEmitter *) pObject)->RestoreAfterMemcpy();
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	free(pIndexArray);	// done with selected item list
	GlobalUnlock(hBuffer);

	/* copy to clipboard, and close */

	SetClipboardData(m_nClipboardFormat, hBuffer);
	CloseClipboard();

	/* success */

	return(TRUE);
}

/* paste the item(s) in the clipboard to the view */

void CDocView::OnPaste() 
{
	DoPaste();
}

BOOL CDocView::DoPaste()
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/* text? */

	if (m_labelEditActive)
		return(PasteText());

	/* should have data available, since paste menu item is disabled if not */

	ASSERT(IsClipboardFormatAvailable(m_nClipboardFormat));

	/* open clipboard */

	if (OpenClipboard() == 0)
		return(FALSE);

	/* get data */

	HGLOBAL hBuffer = GetClipboardData(m_nClipboardFormat);
	if (hBuffer == NULL) 
	{ 
		ASSERT(FALSE);	// shouldn't happen
		CloseClipboard(); 
		return(FALSE);
	} 

	/* get buffer */

	char *pBuffer = (char *) GlobalLock(hBuffer);
	ASSERT(pBuffer);

	/* get item count */

	int nCount = *(int *) pBuffer;
	pBuffer += sizeof(int);

	/* paste each item */

	for (int i = 0; i < nCount; i++)
	{
		/* add a new item to the list */

		CObject *pObject = NewDocument();

		/* prepare for memcpy. we're not going to copy this one, but we're going to copy */
		/* over it, so the same sort of clean-up is necessary */

		switch(m_nType)
		{
		case EMITTERS:
			((CEmitter *) pObject)->PrepareForMemcpy();
			break;
		case EFFECTS:
			((CEffect *) pObject)->PrepareForMemcpy();
			break;
		case PARTICLES:
			((CParticle *) pObject)->PrepareForMemcpy();
			break;
		case SWOOSHES:
			((CSwoosh *) pObject)->PrepareForMemcpy();
			break;
		case SWOOSH_EMITTERS:
			((CPigpenSwooshEmitter *) pObject)->PrepareForMemcpy();
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* fill it in. see comments in OnCopy for why this is so ugly */

		int nCopySize = 0;
		switch(m_nType)
		{
		case EMITTERS:
			{
				CEmitter *pEmitter = (CEmitter *) pObject;
				memcpy(pEmitter, pBuffer, sizeof(CEmitter));
				pBuffer += sizeof(CEmitter);
				nCopySize = sizeof(CEmitter);
			}
			break;
		case EFFECTS:
			{
				CEffect *pEffect = (CEffect *) pObject;
				memcpy(pEffect, pBuffer, sizeof(CEffect));
				pBuffer += sizeof(CEffect);
				nCopySize = sizeof(CEffect);
			}
			break;
		case PARTICLES:
			{
				CParticle *pParticle = (CParticle *) pObject;

				/* particle record contains a CObArray of CSpawn records that has to be handled specially. this is similar */
				/* to the process in the CParticle = operator */

				char SaveSpawn[sizeof(CObArray)];	// can't make it a CObArray or it will destruct
				memcpy(&SaveSpawn, pParticle->GetSpawnArray(), sizeof(CObArray));	// saves pointers to spawn records, and memory malloc'd for CObArray itself

				/* do the memcpy */

				memcpy(pParticle, pBuffer, sizeof(CParticle));
				pBuffer += sizeof(CParticle);
				nCopySize = sizeof(CParticle);

				/* restore spawn record array */

				memcpy(pParticle->GetSpawnArray(), &SaveSpawn, sizeof(CObArray));

				/* copy spawn record contents over from buffer */

				for (int j = 0; j < pParticle->GetSpawnCount(); j++)
				{
					memcpy(pParticle->GetSpawn(j), pBuffer, sizeof(CSpawn));
					pBuffer += sizeof(CSpawn);
					nCopySize += sizeof(CSpawn);
				}
			}
			break;
		case SWOOSHES:
			{
				CSwoosh *pSwoosh = (CSwoosh *) pObject;
				memcpy(pSwoosh, pBuffer, sizeof(CSwoosh));
				pBuffer += sizeof(CSwoosh);
				nCopySize = sizeof(CSwoosh);
			}
			break;
		case SWOOSH_EMITTERS:
			{
				CPigpenSwooshEmitter *pSwooshEmitter = (CPigpenSwooshEmitter *) pObject;
				memcpy(pSwooshEmitter, pBuffer, sizeof(CPigpenSwooshEmitter));
				pBuffer += sizeof(CPigpenSwooshEmitter);
				nCopySize = sizeof(CPigpenSwooshEmitter);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		ASSERT(nCopySize == m_nClipboardRecordSize);

		/* restore after memcpy */

		switch(m_nType)
		{
		case EMITTERS:
			((CEmitter *) pObject)->RestoreAfterMemcpy();
			break;
		case EFFECTS:
			((CEffect *) pObject)->RestoreAfterMemcpy();
			break;
		case PARTICLES:
			((CParticle *) pObject)->RestoreAfterMemcpy();
			break;
		case SWOOSHES:
			((CSwoosh *) pObject)->RestoreAfterMemcpy();
			break;
		case SWOOSH_EMITTERS:
			((CPigpenSwooshEmitter *) pObject)->RestoreAfterMemcpy();
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* enforce unique name for new item */

		pDoc->EnforceUniqueName(pObject, m_nType);
	}

	/* clean up */

	GlobalUnlock(hBuffer);
	CloseClipboard();

	/* repaint view */

	OnUpdate(NULL, 0, NULL);

	/* success */
	return(TRUE);
}

/* copy/paste for text labels */

BOOL CDocView::CopyText()
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/* must be only one selected item */

	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	if (List.GetSelectedCount() != 1)
	{
		ASSERT(false);
		return(false);
	}

	/* open clipboard */

	if (OpenClipboard() == 0)
		return(false);

	/* empty clipboard-- these clears it out-- only one thing lives in the clipboard at once */

	EmptyClipboard();

	/* get selected text */

	CEdit *pEdit = List.GetEditControl();	// the active "editor" window
	if (pEdit == NULL)
	{
		ASSERT(false);	// shouldn't happen, since m_labelEditActive should have been checked already
		CloseClipboard(); 
		return(false);
	}
	char label[100];
	pEdit->GetWindowText(label, 100);
	ASSERT(strlen(label) < 100);
	int start;
	int end;
	pEdit->GetSel(start, end);
	char clipboardText[100];
	strcpy(clipboardText, &label[start]);
	clipboardText[end - start] = 0;

	/* allocate memory to hold data */

	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, strlen(clipboardText) + 1);
	if (hBuffer == NULL) 
	{ 
		ASSERT(false);	// shouldn't happen
		CloseClipboard(); 
		return(false);
	} 

	/* add data to buffer */

	char *pBuffer = (char *) GlobalLock(hBuffer);
	ASSERT(pBuffer);
	strcpy(pBuffer, clipboardText);
	GlobalUnlock(hBuffer);

	/* copy to clipboard, and close */

	SetClipboardData(CF_TEXT, hBuffer);
	CloseClipboard();
	return(true);
}
BOOL CDocView::CutText()
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/* must be only one selected item */

	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	if (List.GetSelectedCount() != 1)
	{
		ASSERT(false);
		return(false);
	}

	/* open clipboard */

	if (OpenClipboard() == 0)
		return(false);

	/* empty clipboard-- these clears it out-- only one thing lives in the clipboard at once */

	EmptyClipboard();

	/* get selected text */

	CEdit *pEdit = List.GetEditControl();	// the active "editor" window
	if (pEdit == NULL)
	{
		ASSERT(false);	// shouldn't happen, since m_labelEditActive should have been checked already
		CloseClipboard(); 
		return(false);
	}
	char label[100];
	pEdit->GetWindowText(label, 100);
	ASSERT(strlen(label) < 100);
	int start;
	int end;
	pEdit->GetSel(start, end);
	char clipboardText[100];
	strcpy(clipboardText, &label[start]);
	clipboardText[end - start] = 0;

	/* allocate memory to hold data */

	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, strlen(clipboardText) + 1);
	if (hBuffer == NULL) 
	{ 
		ASSERT(false);	// shouldn't happen
		CloseClipboard(); 
		return(false);
	} 

	/* add data to buffer */

	char *pBuffer = (char *) GlobalLock(hBuffer);
	ASSERT(pBuffer);
	strcpy(pBuffer, clipboardText);
	GlobalUnlock(hBuffer);

	/* copy to clipboard, and close */

	SetClipboardData(CF_TEXT, hBuffer);
	CloseClipboard();

	/* now update text that should be left */

	char newLabel[100] = "";
	if (start != 0)
	{
		strncpy(newLabel, label, start);
		newLabel[start] = 0;
	}
	if ((unsigned) end < strlen(label))
		strcat(newLabel, &label[end]);

	/* set new label */

	pEdit->SetWindowText(newLabel);
	return(true);
}
BOOL CDocView::PasteText()
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/* must be only one selected item */

	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	if (List.GetSelectedCount() != 1)
	{
		ASSERT(false);
		return(false);
	}

	/* open clipboard */

	if (OpenClipboard() == 0)
		return(false);

	/* get data-- may not be any */

	HGLOBAL hBuffer = GetClipboardData(CF_TEXT);
	if (hBuffer == NULL) 
	{ 
		CloseClipboard(); 
		return(false);
	} 

	/* get buffer */

	char *pBuffer = (char *) GlobalLock(hBuffer);
	ASSERT(pBuffer);

	/* get current text */

	CEdit *pEdit = List.GetEditControl();	// the active "editor" window
	if (pEdit == NULL)
	{
		ASSERT(false);	// shouldn't happen, since m_labelEditActive should have been checked already
		CloseClipboard(); 
		return(false);
	}
	char label[100];
	pEdit->GetWindowText(label, 100);	// current text
	ASSERT(strlen(label) < 100);
	int start;
	int end;
	pEdit->GetSel(start, end);

	/* sanity check */

#define NEW_LABEL_LENGTH 200
	if (strlen(pBuffer) + strlen(label) + 1 > NEW_LABEL_LENGTH)
	{
		CloseClipboard(); 
		return(false);
	}

	/* build new text */

	char newLabel[NEW_LABEL_LENGTH] = "";
	if (start != 0)
	{
		strncpy(newLabel, label, start);
		newLabel[start] = 0;
	}
	strcat(newLabel, pBuffer);
	if ((unsigned) end < strlen(label))
		strcat(newLabel, &label[end]);

	/* set new label */

	pEdit->SetWindowText(newLabel);

	/* clean up */

	GlobalUnlock(hBuffer);
	CloseClipboard();

	/* success */

	return(true);
}

/* delete the currently-selected item(s) */

void CDocView::OnDelete() 
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	bool bItemsDeleted = pDoc->DeleteSelectedItems(m_nType, GetListCtrl());

	/* temporarily, deleting items from view may invalidate clipboard contents. this is because */
	/* clipboard contents do not currently contain all data necessary to recreate entry-- for */
	/* example, particle clipboard entry does not include graphic/image records. so deleting */
	/* a graphic record invalidates a particle clipboard entry. eventually this will change, since */
	/* we will have to have full data to copy/paste between docs */

	if (bItemsDeleted)
	{
		/* since this is a temporary band-aid, just clear clipboard when anything is deleted */

		OpenClipboard();
		EmptyClipboard();
		CloseClipboard();
	}
}

/* called when user selects rename from pop-up menu. only one item should be selected */

void CDocView::OnRename() 
{
	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	ASSERT(List.GetSelectedCount() == 1);	// should only get here through pop-up menu; rename enabled when one item selected	
	POSITION pos = List.GetFirstSelectedItemPosition();
	ASSERT(pos != NULL);
	List.EditLabel(List.GetNextSelectedItem(pos));
}

/* move selected item(s) to top, bottom of list */

void CDocView::OnMoveToTop() 
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->MoveSelectedItemsToTop(m_nType, GetListCtrl());
}
void CDocView::OnMoveToBottom() 
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->MoveSelectedItemsToBottom(m_nType, GetListCtrl());
}

/* preview selected item */

void CDocView::OnPreview()
{
	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	ASSERT(List.GetSelectedCount() == 1);	// should only get here through pop-up menu; rename enabled when one item selected	
	CObject *pItem = NULL;
	int nIndex = GetSelectedItemIndex();
	ASSERT(nIndex != -1);
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pItem = pDoc->GetArrayItem(m_nType, nIndex);
	switch(m_nType)
	{
	case EFFECTS:
		Preview__Create(pDoc, NULL, (CEffect *) pItem, NULL, NULL, NULL);
		break;
	case PARTICLES:
		Preview__Create(pDoc, NULL, NULL, (CParticle *) pItem, NULL, NULL);
		break;
	case EMITTERS:
		Preview__Create(pDoc, (CEmitter *) pItem, NULL, NULL, NULL, NULL);
		break;
	case SWOOSHES:
		Preview__Create(pDoc, NULL, NULL, NULL, (CSwoosh *) pItem, NULL);
		break;
	case SWOOSH_EMITTERS:
		Preview__Create(pDoc, NULL, NULL, NULL, NULL, (CPigpenSwooshEmitter *) pItem);
		break;
	default:
		ASSERT(FALSE);
	}
}

/* find associated items-- follow links through the database */

void CDocView::OnFindAssociates()
{
	/* get item being queried */

	CListCtrl& List = GetListCtrl();	// get CListCtrl associated with this view
	ASSERT(List.GetSelectedCount() == 1);	// should only get here through pop-up menu; rename enabled when one item selected	
	CObject *pItem = NULL;
	int nIndex = GetSelectedItemIndex();
	ASSERT(nIndex != -1);
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pItem = pDoc->GetArrayItem(m_nType, nIndex);

	/* this is a job for the document */

	pDoc->FindAssociates(pItem, m_nType);
}

/* clear stage and emitter model, mainly for switch to dx, but also useful if you trapped with a bad stage somehow, since the
stage name is saved in the .pig file */
void CDocView::OnRepairClearStage()
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	static CString EmptyString = "";
	pDoc->SetStageFile(EmptyString);
}
void CDocView::OnRepairClearEmitterModel()
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	static CString EmptyString = "";
	pDoc->SetEmitterModelFile(EmptyString);
}

/* for convenience, commands to close all but one window */

void CDocView::OnCloseAllButEffects()
{
	CloseAllBut(EFFECTS);
}
void CDocView::OnCloseAllButGraphics()
{
	CloseAllBut(GRAPHICS);
}
void CDocView::OnCloseAllButParticles()
{
	CloseAllBut(PARTICLES);
}
void CDocView::OnCloseAllButEmitters()
{
	CloseAllBut(EMITTERS);
}
void CDocView::OnCloseAllButSwooshes()
{
	CloseAllBut(SWOOSHES);
}
void CDocView::OnCloseAllButSwooshEmitters()
{
	CloseAllBut(SWOOSH_EMITTERS);
}
void CDocView::CloseAllBut(int nEditWindowType)
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->GetView(nEditWindowType) == NULL)
	{
		ASSERT(false);
		return;
	}
	for (int nType = 0; nType < MAX_DOC_DATA_TYPES; nType++)
	{
		if (nType == nEditWindowType)
			continue;	// this is one the not to close

		/* destroy window that contains view */

		if (pDoc->GetView(nType))
		{
			CWnd *pWnd = pDoc->GetView(nType)->GetParentFrame();
			pWnd->DestroyWindow();
			pDoc->ViewClosed(nType);
		}
	}
}
void CDocView::OnUpdateCloseAllButEffects(CCmdUI* pCmdUI)
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->GetView(EFFECTS) != NULL);
}
void CDocView::OnUpdateCloseAllButGraphics(CCmdUI* pCmdUI)
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->GetView(GRAPHICS) != NULL);
}
void CDocView::OnUpdateCloseAllButParticles(CCmdUI* pCmdUI)
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->GetView(PARTICLES) != NULL);
}
void CDocView::OnUpdateCloseAllButEmitters(CCmdUI* pCmdUI)
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->GetView(EMITTERS) != NULL);
}
void CDocView::OnUpdateCloseAllButSwooshes(CCmdUI* pCmdUI)
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->GetView(SWOOSHES) != NULL);
}
void CDocView::OnUpdateCloseAllButSwooshEmitters(CCmdUI* pCmdUI)
{
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->GetView(SWOOSH_EMITTERS) != NULL);
}

/* called after window is destroyed */

void CDocView::PostNcDestroy() 
{
	/* if edit dialog is up, destroy it */

	for (int i = 0; i < MAX_EDIT_WINDOWS_PER_VIEW; i++)
	{
		if (m_pEditWindow[i])
			OnEdit(i);	// destroys edit window(s) when m_pEditWindow is non-NULL
	}

	/* mark view as closed in doc */

	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->ViewClosed(m_nType);

	/* call base class */

	CListView::PostNcDestroy();
}

/* notification that item is changing, like when user clicks on item or uses arrow keys to move from item to item.
when an item is highlighted, update the dialog window, if any */

void CDocView::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* as far as i can tell through debugging sessions, new state == 3 means highlighted */

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->uNewState == 3)
		UpdateEditWindow();
	*pResult = 0;
}

/* double click brings up edit dialog */

void CDocView::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* if no edit window up, bring it up */

	if ((EditWindowOpenCount() == 0) && (GetListCtrl().GetSelectedCount() != 0))
	{
		for (int i = 0; i < MAX_EDIT_WINDOWS_PER_VIEW; i++)
			OnEdit(i);	// bring up edit dialog
	}
	if (pResult)
		*pResult = 0;
}

/* doc has changed, rebuild view */

void CDocView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	static char *TypeTitle[MAX_DOC_DATA_TYPES] = {

		": Effects",
		": Particles",
		": Graphics",
		": Emitters",
		": Swooshes",
		": Swoosh emitters",
	};
	static int nNoChangeFlag[MAX_DOC_DATA_TYPES] = {
		NO_CHANGE_EFFECTS_VIEW,
		NO_CHANGE_PARTICLES_VIEW,
		NO_CHANGE_GRAPHICS_VIEW,
		NO_CHANGE_EMITTERS_VIEW,
		NO_CHANGE_SWOOSHES_VIEW,
		NO_CHANGE_SWOOSH_EMITTERS_VIEW,
	};
	static int nNoReorderFlag[MAX_DOC_DATA_TYPES] = {
		NO_REORDER_EFFECTS_VIEW,
		NO_REORDER_PARTICLES_VIEW,
		NO_REORDER_GRAPHICS_VIEW,
		NO_REORDER_EMITTERS_VIEW,
		NO_REORDER_SWOOSHES_VIEW,
		NO_REORDER_SWOOSH_EMITTERS_VIEW,
	};

	/* this view affected? */

	if (lHint & nNoChangeFlag[m_nType])
		return;

	/* if list not reordered, just refresh items in place. this preserves currently selected items */

	CListCtrl& List = GetListCtrl();
	CPigpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (lHint & nNoReorderFlag[m_nType])
	{
		for (int i = 0; i < pDoc->GetArraySize(m_nType); i++)
			ItemToView(i, FALSE);
	}
	else
	{
		/* reordered data-- clear list */

		List.DeleteAllItems();

		/* add each item to list */

		CPigpenDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		for (int i = 0; i < pDoc->GetArraySize(m_nType); i++)
			ItemToView(i, TRUE);
	}
	ASSERT(pDoc->GetArraySize(m_nType) == List.GetItemCount());	// list size should match doc's array size

	/* redraw all items */

	List.RedrawItems(0, List.GetItemCount() - 1);

	/* set window title */

	CWnd *pWnd = GetParentFrame();
	pWnd->SetWindowText(pDoc->GetTitle() + TypeTitle[m_nType]);

	/* update the associated editing window, if any */

	UpdateEditWindow();
}

