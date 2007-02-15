// UpdateDuplicatesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "assetmanager.h"
#include "UpdateDuplicatesDialog.h"
#include "AssetManagerTreeCtrl.h"
#include "Registry.h"
#include "RegistryKeys.h"
#include "AssociatedIcon.h"
#include "PPTooltip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UpdateDuplicatesDialog dialog

#define TREE_DELETE_FILE_BK_COLOR RGB(255,210,210)
#define TREE_DEST_FILE_BK_COLOR RGB(249,235,255)
#define TREE_SOURCE_FILE_BK_COLOR RGB(237,255,255)

#define IMG_IND_IDX_EXCLUDE 0
#define IMG_IND_IDX_SOURCE  1
#define IMG_IND_IDX_DEST    2
#define IMG_IND_IDX_DELETE  3

UpdateDuplicatesDialog::UpdateDuplicatesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(UpdateDuplicatesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(UpdateDuplicatesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bClosing = false;

}

BOOL UpdateDuplicatesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	Registry registry(ASSET_MANAGER_KEY);
	DWORD regInt;
	
	m_dupText1.SetWindowText("");
	m_dupText2.SetWindowText("");
	
	CRect rectWindowDlg;
	GetWindowRect(&rectWindowDlg);
	rectWindowDlg.top = rectWindowDlg.bottom - 25;

	m_dialogRect.SetRectEmpty();
	if (registry.Get(UPDATE_DUPLICATES_DIALOG_RECT_TOP_FIELD,&regInt))
		m_dialogRect.top = (LONG)regInt;
	if (registry.Get(UPDATE_DUPLICATES_DIALOG_RECT_BOTTOM_FIELD,&regInt))
		m_dialogRect.bottom = (LONG)regInt;
	if (registry.Get(UPDATE_DUPLICATES_DIALOG_RECT_LEFT_FIELD,&regInt))
		m_dialogRect.left = (LONG)regInt;
	if (registry.Get(UPDATE_DUPLICATES_DIALOG_RECT_RIGHT_FIELD,&regInt))
		m_dialogRect.right = (LONG)regInt;

	// move window to last saved position
	CRect desktopRect;
	desktopRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	desktopRect.bottom = desktopRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
	desktopRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	desktopRect.right = desktopRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);

	if (m_dialogRect.IsRectEmpty())
	{
		GetWindowRect(&m_dialogRect);
	}
	// make sure the window is visible on the screen
	if (m_dialogRect.top<desktopRect.top)
	{
		m_dialogRect.bottom = desktopRect.top+m_dialogRect.Height();
		m_dialogRect.top = desktopRect.top;
	}
	if (m_dialogRect.right<desktopRect.left+554)
	{
		m_dialogRect.right = desktopRect.left+m_dialogRect.Width();
		m_dialogRect.left = desktopRect.left;
	}
	if (m_dialogRect.top>desktopRect.bottom-419)
	{
		m_dialogRect.top = desktopRect.bottom-m_dialogRect.Height();
		m_dialogRect.bottom = desktopRect.bottom;
	}
	if (m_dialogRect.left>desktopRect.right-554)
	{
		m_dialogRect.left = desktopRect.right-m_dialogRect.Width();
		m_dialogRect.right = desktopRect.right;
	}
	MoveWindow(&m_dialogRect);

	// Set up the tree control
	SetUpTreeControl();

	SetupToolTips();

	LoadTreeControl();

	UpdateStats();

	// Now update the window so it's displayed with child windows on top of it:
	ShowWindow(TRUE);
	UpdateWindow();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

//-----------------------------------------------------------------------------
// UpdateDuplicatesDialog::~UpdateDuplicatesDialog
// This method is the destructor for this dialog class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
UpdateDuplicatesDialog::~UpdateDuplicatesDialog()
{
	delete m_pDataTip;
	
	// Delete any member pointers:
	if (m_pTreeCtrl != NULL)
	{
		delete m_pTreeCtrl;
		m_pTreeCtrl = NULL;
	}
	if (m_pAssociatedIconManager != NULL)
	{
		delete m_pAssociatedIconManager;
		m_pAssociatedIconManager = NULL;
	}
}

void UpdateDuplicatesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UpdateDuplicatesDialog)
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
	DDX_Control(pDX, IDC_DUP_TEXT_2, m_dupText2);
	DDX_Control(pDX, IDC_DUP_TEXT_1, m_dupText1);
	DDX_Control(pDX, ID_UPDATE, m_updateButton);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(UpdateDuplicatesDialog, CDialog)
	//{{AFX_MSG_MAP(UpdateDuplicatesDialog)
	ON_BN_CLICKED(ID_UPDATE, OnUpdate)
	ON_COMMAND(ID_SET_DEFAULT, OnContextSetDefault)
	ON_COMMAND(ID_EXCLUDE, OnContextExclude)
	ON_COMMAND(ID_SOURCE_FILE, OnContextSourceFile)
	ON_COMMAND(ID_DEST_FILE, OnContextDestFile)
	ON_COMMAND(ID_DELETE_FILE, OnContextDeleteFile)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_SET_DEFAULT, OnUpdateSetDefault)
	ON_UPDATE_COMMAND_UI(ID_SOURCE_FILE, OnUpdateSourceFile)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE, OnUpdateExclude)
	ON_UPDATE_COMMAND_UI(ID_DEST_FILE, OnUpdateDestFile)
	ON_UPDATE_COMMAND_UI(ID_DELETE_FILE, OnUpdateDeleteFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UpdateDuplicatesDialog message handlers

void UpdateDuplicatesDialog::SetUpdateDuplicatesManager(UpdateDuplicatesManager *pUpdateDuplicatesManager)
{
	m_pUpdateDuplicatesManager = pUpdateDuplicatesManager;
}

void UpdateDuplicatesDialog::SetUpTreeControl()
{
	// Instantiate the tree control objects:
	m_pTreeCtrl = new CAssetManagerTreeCtrl(this);
	m_pAssociatedIconManager = new AssociatedIconManager;
	
	// Subclass the tree controls:
	m_pTreeCtrl->SubclassTreeCtrlId(IDC_DUP_TREE, this);

	// Set the background color:
	m_pTreeCtrl->SetDeleteTextBkColor(TREE_DELETE_FILE_BK_COLOR);
	m_pTreeCtrl->SetCopySourceTextBkColor(TREE_SOURCE_FILE_BK_COLOR);
	m_pTreeCtrl->SetCopyDestTextBkColor(TREE_DEST_FILE_BK_COLOR);
	
	m_pTreeCtrl->SetAssociatedIconManager(m_pAssociatedIconManager);
	
	// Set the tree and list styles:
	m_pTreeCtrl->SetTreeCtrlStyles(TVS_DISABLEDRAGDROP | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES,
		TVXS_COLUMNHEADER | TVXS_FLYBYTOOLTIPS | TVXS_MULTISEL,
		FALSE);
	m_pTreeCtrl->SetListCtrlStyleEx(LVXS_HILIGHTSUBITEMS | LVXS_LINESBETWEENCOLUMNS |
		LVXS_LINESBETWEENITEMS, FALSE);
	m_pTreeCtrl->SetListCtrlStyle(LVS_REPORT|LVS_NOLABELWRAP|LVS_SHAREIMAGELISTS|LVS_NOCOLUMNHEADER|LVS_NOSORTHEADER, FALSE);
	m_pTreeCtrl->SetRole(CAssetManagerTreeCtrl::LEFT_TREE);
	
	// Set the image lists for the left and right trees:
	HBITMAP hbmTreeControl = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_UPDATE_DUPLICATES), IMAGE_BITMAP,
		0, 0, LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);
	CBitmap bitmapTreeCtrls;
	bitmapTreeCtrls.Attach(hbmTreeControl);
	m_ilTreeCtrls.Create(23, 20, ILC_COLOR8, 8, 8);
	m_ilTreeCtrls.Add(&bitmapTreeCtrls, (CBitmap*) NULL);
	m_pTreeCtrl->SetImageList(&m_ilTreeCtrls, TVSIL_NORMAL);
}

void UpdateDuplicatesDialog::SetupToolTips(void)
{
	m_pDataTip = new CPPToolTip;
	m_pDataTip->Create(this);
	
	m_pDataTip->SetDelayTime(TTDT_AUTOPOP,0);
	m_pDataTip->SetDelayTime(TTDT_INITIAL,200);
	m_pDataTip->SetDelayTime(TTDT_RESHOW,200);

	int dataTipBehavior = PPTOOLTIP_MULTIPLE_SHOW | PPTOOLTIP_TRACKING_MOUSE | PPTOOLTIP_CLOSE_LEAVEWND;
	int dataTipStyle = PPTOOLTIP_SHADOW;
	m_pDataTip->SetBehaviour(dataTipBehavior);
	m_pDataTip->SetStyles(dataTipStyle);
	
	m_pDataTip->AddTool(m_pTreeCtrl,IDC_DUP_TREE);
	m_pDataTip->Activate(TRUE);
	m_pDataTip->SetNotify(TRUE);
	m_pDataTip->SetNotify(m_hWnd);

}

void UpdateDuplicatesDialog::OnUpdate() 
{
	EndDialog(TRUE);
}

BOOL UpdateDuplicatesDialog::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pDataTip)
		m_pDataTip->RelayEvent(pMsg);
	
	return CDialog::PreTranslateMessage(pMsg);
}

//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnNotify
// This handler will be called by the framework to inform the dialog of a
// control that an event has occurred in or that the control requires some
// kind of information.
//
// Input:
//  WPARAM wParam -- Identifies the control that sends the message if the
//					 message is from a control. Otherwise, wParam is 0.
//  LPARAM lParam -- Structure that contains the notification code and
//					 additional information.
//  LRESULT* pResult -- Pointer to an LRESULT variable in which to store the
//						result code if the message is handled.
//
// Output: TRUE. 
//-----------------------------------------------------------------------------
BOOL UpdateDuplicatesDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// See what notification message is being sent:
	LPNMHDR  pNMHDR = (LPNMHDR)lParam;
	if (UDM_TOOLTIP_DISPLAY == pNMHDR->code)
	{
		static TCHAR passedText[256];
		// Set up some strings, etc.
		NM_PPTOOLTIP_DISPLAY *pNotify = (NM_PPTOOLTIP_DISPLAY*) pNMHDR;
		CString strTipText = "";

		// Get the control and use the text from it:
		UINT nIDControl = ::GetDlgCtrlID((HWND)pNotify->ti->hWnd);
		if (nIDControl != 0) // Will be zero on a separator
		{
			if (nIDControl==IDC_DUP_TREE)
			{
				strTipText = GetMouseOverTipText();
			}
			else
			{
				// Get our status and tip text:
				TCHAR szFullText[256];
				AfxLoadString(nIDControl, szFullText);
				strTipText = szFullText;
			}
		}

		// Copy the text into the correct place:
		pNotify->ti->sTooltip = strTipText;

		// Set the result:
		*pResult = 0;
	}
	return TRUE;
}

CString UpdateDuplicatesDialog::GetMouseOverTipText(void)
{
	CString result = "";
	if (m_mouseOverData.m_type == MouseOverData::OVER_FILE)
	{
		// if pointing to the set itself
		if (m_mouseOverData.m_entryIndex==-1)
		{
			FILETIME oldestTime;
			FILETIME newestTime;
			int smallestSize;
			int largestSize;

			m_pUpdateDuplicatesManager->GetSetStats(m_mouseOverData.m_setIndex,oldestTime,newestTime,smallestSize,largestSize);
			const char *setName = m_pUpdateDuplicatesManager->GetDuplicateSetName(m_mouseOverData.m_setIndex);
			int numEntries = m_pUpdateDuplicatesManager->GetNumPathEntries(m_mouseOverData.m_setIndex);

			CString smallestSizeString;
			smallestSizeString.Format("%d",smallestSize);
			// insert the commas
			int end;
			end = smallestSizeString.GetLength()-3;
			while(end>0)
			{
				smallestSizeString.Insert(end,",");
				end-=3;
			}

			CString largestSizeString;
			largestSizeString.Format("%d",largestSize);
			// insert the commas
			end = largestSizeString.GetLength()-3;
			while(end>0)
			{
				largestSizeString.Insert(end,",");
				end-=3;
			}

			CTime oldestTimeObject(oldestTime);
			CString oldestTimeString = oldestTimeObject.Format("%A, %B %d, %Y %I:%M %p");
			CTime newestTimeObject(newestTime);
			CString newestTimeString = newestTimeObject.Format("%A, %B %d, %Y %I:%M %p");

			if (smallestSize==largestSize)
			{
				if (UpdateDuplicatesManager::CompareTimes(oldestTime,newestTime)==0)
				{
					result.Format("%s\n%d files found\nDate Modified: %s\nSize: %s Bytes",
						setName,numEntries,(const char *)oldestTimeString,(const char *)smallestSizeString);
				}
				else
				{
					result.Format("%s\n%d files found\nOldest Date Modified: %s\nNewest Date Modified: %s\nSize: %s Bytes",
						setName,numEntries,(const char *)oldestTimeString,(const char *)newestTimeString,(const char *)smallestSizeString);
				}
			}
			else
			{
				if (UpdateDuplicatesManager::CompareTimes(oldestTime,newestTime)==0)
				{
					result.Format("%s\n%d files found\nDate Modified: %s\nSmallest Size: %s Bytes\nLargest Size: %s Bytes",
						setName,numEntries,(const char *)oldestTimeString,(const char *)smallestSizeString,(const char *)largestSizeString);
				}
				else
				{
					result.Format("%s\n%d files found\nOldest Date Modified: %s\nNewest Date Modified: %s\nSmallest Size: %s Bytes\nLargestSize: %s Bytes",
						setName,numEntries,(const char *)oldestTimeString,(const char *)newestTimeString,(const char *)smallestSizeString,(const char *)largestSizeString);
				}
			}
		}
		else
		{
			CTime modifiedTime(m_pUpdateDuplicatesManager->GetPathEntryModifiedTime(m_mouseOverData.m_setIndex,m_mouseOverData.m_entryIndex));
			CString timeString = modifiedTime.Format("%A, %B %d, %Y %I:%M %p");
			TupString pathString = m_pUpdateDuplicatesManager->GetPathEntryString(m_mouseOverData.m_setIndex,m_mouseOverData.m_entryIndex);
			CString sizeString;
			sizeString.Format("%d",m_pUpdateDuplicatesManager->GetPathEntryNode(m_mouseOverData.m_setIndex,m_mouseOverData.m_entryIndex)->GetFileLength());
			// insert the commas
			int end = sizeString.GetLength()-3;
			while(end>0)
			{
				sizeString.Insert(end,",");
				end-=3;
			}
			result.Format(IDS_TIP_FILE,(const char *)pathString,(const char *)timeString,(const char *)sizeString);
		}
	}
	return result;
}

void UpdateDuplicatesDialog::TreeMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM hTreeItem;
	CPoint screenPoint(point);
	ClientToScreen(&screenPoint);

	MouseOverData mouseOverData;
	CPoint testPoint;
	testPoint = screenPoint;
	m_pTreeCtrl->ScreenToClient(&testPoint);
	hTreeItem = m_pTreeCtrl->HitTest(testPoint);
	if (hTreeItem)
	{
		SEC_DWORD sDWItemData = m_pTreeCtrl->GetItemData(hTreeItem);
		mouseOverData.m_setIndex = (sDWItemData>>16)&0xffff;
		mouseOverData.m_entryIndex = (sDWItemData&0xffff);
		if (mouseOverData.m_entryIndex==0xffff)
			mouseOverData.m_entryIndex = -1;
		mouseOverData.m_type = MouseOverData::OVER_FILE;
	}
	if (mouseOverData.m_type==MouseOverData::OVER_NONE)
	{
		if (m_mouseOverData.m_type!=MouseOverData::OVER_NONE)
		{
			m_pDataTip->Activate(FALSE);
		}
	}
	else
	{
		if (m_mouseOverData != mouseOverData)
		{
			m_mouseOverData = mouseOverData;
			m_pDataTip->Activate(FALSE);
			m_pDataTip->Activate(TRUE);
		}
	}
}

//-----------------------------------------------------------------------------
// CAssetManagerDlg::ShowContextMenu
// This method can be called to show the context menu for the left or right
// tree control.
//
// Input:
//	CPoint point -- Position of the cursor, in screen coordinates, at the time
//					of the mouse click.
// Output: None.
//-----------------------------------------------------------------------------
void UpdateDuplicatesDialog::ShowContextMenu(CPoint point)
{
	int validActionFlags = GetValidActions();
	UINT menuFlags;
	
	CMenu menu;
	menu.CreatePopupMenu();

	menuFlags = (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_SET_DEFAULT) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_SET_DEFAULT, "Set Default\tCtrl+Q");
	menuFlags = (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_EXCLUDE_FILE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_EXCLUDE, "Exclude\tCtrl+W");
	menuFlags = (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_SOURCE_FILE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_SOURCE_FILE, "Source File\tCtrl+R");
	menuFlags = (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_DEST_FILE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_DEST_FILE, "Destination File\tCtrl+E");
	menuFlags = (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_DELETE_FILE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
	menu.AppendMenu(menuFlags, ID_DELETE_FILE, "Delete File\tCtrl+Shift+D");
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		point.x, point.y, this);
}

int UpdateDuplicatesDialog::GetValidActions(void)
{
	TupArray<int> setIndexArray;
	TupArray<int> entryIndexArray;
	GetSelectionEntries(setIndexArray,entryIndexArray);
	return m_pUpdateDuplicatesManager->GetValidActions(setIndexArray,entryIndexArray);
}

void UpdateDuplicatesDialog::OnContextSetDefault()
{
	ChangeAction(UpdateDuplicatesManager::ACTIONS_SET_DEFAULT);
}

void UpdateDuplicatesDialog::OnContextExclude()
{
	ChangeAction(UpdateDuplicatesManager::ACTIONS_EXCLUDE_FILE);
}

void UpdateDuplicatesDialog::OnContextSourceFile()
{
	ChangeAction(UpdateDuplicatesManager::ACTIONS_SOURCE_FILE);
}

void UpdateDuplicatesDialog::OnContextDestFile()
{
	ChangeAction(UpdateDuplicatesManager::ACTIONS_DEST_FILE);
}

void UpdateDuplicatesDialog::OnContextDeleteFile()
{
	ChangeAction(UpdateDuplicatesManager::ACTIONS_DELETE_FILE);
}

void UpdateDuplicatesDialog::GetSelectionEntries(TupArray<int> &setIndexArray,TupArray<int> &entryIndexArray)
{
	HTREEITEM hCurrItem = m_pTreeCtrl->GetFirstSelectedItem();
	while (hCurrItem != NULL)
	{
		SEC_DWORD sDWItemData = m_pTreeCtrl->GetItemData(hCurrItem);
		int setIndex = (sDWItemData>>16)&0xffff;
		int entryIndex = (sDWItemData&0xffff);
		if (entryIndex==0xffff)
			entryIndex = -1;
		setIndexArray.Add(setIndex);
		entryIndexArray.Add(entryIndex);
		
		hCurrItem = m_pTreeCtrl->GetNextSelectedItem(hCurrItem);
	}
}

void UpdateDuplicatesDialog::ChangeAction(UpdateDuplicatesManager::Actions action)
{
	TupArray<int> setIndexArray;
	TupArray<int> entryIndexArray;
	GetSelectionEntries(setIndexArray,entryIndexArray);
	m_pUpdateDuplicatesManager->SetAction(setIndexArray,entryIndexArray,action);
	UpdateTreeItems();
}

void UpdateDuplicatesDialog::LoadTreeControl(void)
{
	m_pTreeCtrl->SetAllowPaint(false);
	m_pTreeCtrl->DeleteAllItems();
	m_pAssociatedIconManager->ClearAll();

	m_pTreeCtrl->SetNodifyExpanded(false);

	int numSets = m_pUpdateDuplicatesManager->GetNumDuplicateSets();
	for (int setIndex=0;setIndex<numSets;setIndex++)
	{
		LoadTreeItem(setIndex);
	}
	m_pTreeCtrl->RecalcScrollBars();
	m_pTreeCtrl->SetAllowPaint(true);
}


void UpdateDuplicatesDialog::LoadTreeItem(int setIndex)
{
	const char *pSetName = m_pUpdateDuplicatesManager->GetDuplicateSetName(setIndex);
	int setFlags = m_pUpdateDuplicatesManager->GetDuplicateSetFlags(setIndex);

	bool bEnabled = false;

	if ((setFlags & UpdateDuplicatesManager::DUPLICATE_SET_FLAGS_ACTION_MASK) == UpdateDuplicatesManager::DUPLICATE_SET_FLAGS_ACTION_ENABLE)
		bEnabled = true;

	char extString[_MAX_EXT];
	_splitpath(pSetName,NULL,NULL,NULL,extString);
	
	int associatedIconIndex = m_pAssociatedIconManager->GetIconIndex(extString);

	int stateFlags = bEnabled ? TVIS_EXPANDED : 0;
	int stateMask = TVIS_EXPANDED;

	HTREEITEM hItemNewSet = m_pTreeCtrl->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE | TVIF_PARAM | TVIF_HANDLE | TVIF_STATE,
		pSetName, associatedIconIndex, associatedIconIndex, stateFlags, stateMask,
		(LPARAM) ((setIndex<<16)|0xffff),
		TVI_ROOT, TVI_LAST);

	m_pUpdateDuplicatesManager->SetDuplicateSetUserData(setIndex,(void *)hItemNewSet);

	TV_ITEM item;
	TV_ITEM_EX itemEx;
	
	item.mask = TVIF_HANDLE;
	item.hItem = hItemNewSet;
	
	itemEx.maskEx = TVIF_EX_STATEEX;
	itemEx.stateEx = TVIS_EX_ASSOCIATED_ICON;
	if (!bEnabled)
		itemEx.stateEx |= TVIS_EX_EXCLUDED;

	itemEx.stateExMask = TVIS_EX_ASSOCIATED_ICON|
								 TVIS_EX_FROZEN|
								 TVIS_EX_EXCLUDED|
								 TVIS_EX_TO_DELETE|
								 TVIS_EX_COPY_SOURCE|
								 TVIS_EX_COPY_DEST;
		
	m_pTreeCtrl->SetItemEx( &item, &itemEx );

	// add the sub entries for the set
	int numEntries = m_pUpdateDuplicatesManager->GetNumPathEntries(setIndex);
	for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
	{
		const char *pEntryName = m_pUpdateDuplicatesManager->GetPathEntryString(setIndex,entryIndex);
		int entryFlags = m_pUpdateDuplicatesManager->GetPathEntryFlags(setIndex,entryIndex);

		TV_ITEM item;
		TV_ITEM_EX itemEx;
		
		item.mask = TVIF_HANDLE;
		
		itemEx.maskEx = TVIF_EX_STATEEX;
		itemEx.stateEx = 0;

		int nIndImage;

		int firstFileAdd =  (entryFlags & UpdateDuplicatesManager::PATH_ENTRY_FIRST_FILE) ? 4 : 0;

		switch (entryFlags & UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_MASK)
		{
		case UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_EXCLUDE_FILE:
			itemEx.stateEx |= TVIS_EX_EXCLUDED;
			nIndImage = IMG_IND_IDX_EXCLUDE+firstFileAdd;
			break;
		case UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_DEST_FILE:
			itemEx.stateEx |= TVIS_EX_COPY_DEST;
			nIndImage = IMG_IND_IDX_DEST+firstFileAdd;
			break;
		case UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE:
			itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
			nIndImage = IMG_IND_IDX_SOURCE+firstFileAdd;
			break;
		case UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_DELETE_FILE:
			itemEx.stateEx |= TVIS_EX_TO_DELETE;
			nIndImage = IMG_IND_IDX_DELETE+firstFileAdd;
			break;
		}

		HTREEITEM hItemNewEntry = m_pTreeCtrl->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE | TVIF_PARAM | TVIF_HANDLE | TVIF_STATE,
			pEntryName, nIndImage, nIndImage, 0, 0,
			(LPARAM) ((setIndex<<16)|entryIndex),
			hItemNewSet, TVI_LAST);

		m_pUpdateDuplicatesManager->SetPathEntryUserData(setIndex,entryIndex,(void *)hItemNewEntry);

		item.hItem = hItemNewEntry;
		
		itemEx.stateExMask = TVIS_EX_ASSOCIATED_ICON|
								 TVIS_EX_FROZEN|
								 TVIS_EX_EXCLUDED|
								 TVIS_EX_TO_DELETE|
								 TVIS_EX_COPY_SOURCE|
								 TVIS_EX_COPY_DEST;
		
		m_pTreeCtrl->SetItemEx( &item, &itemEx );
	}
}

void UpdateDuplicatesDialog::UpdateTreeItems(void)
{
	UpdateStats();
	
	m_pTreeCtrl->SetAllowPaint(false);
	
	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	int numSets = m_pUpdateDuplicatesManager->GetNumDuplicateSets();
	for (int setIndex=0;setIndex<numSets;setIndex++)
	{
		SetTreeItem(setIndex);
	}
	
	m_pTreeCtrl->ReMeasureAllItems();
	m_pTreeCtrl->SetAllowPaint(true);
	m_pTreeCtrl->Invalidate();
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void UpdateDuplicatesDialog::UpdateStats(void)
{
	int numFilesToCopy;
	int numFilesToDelete;
	__int64 numBytesToCopy;
	m_pUpdateDuplicatesManager->GetStats(numFilesToCopy,numBytesToCopy,numFilesToDelete);
	CString sizeString;
	sizeString.Format("%I64d",numBytesToCopy);
	int end = sizeString.GetLength()-3;
	while(end>0)
	{
		sizeString.Insert(end,",");
		end-=3;
	}
	CString text1;
	text1.Format("%d File(s) to copy - %s Bytes Total",numFilesToCopy,(const char *)sizeString);
	CString text2;
	text2.Format("%d File(s) to Delete",numFilesToDelete);
	m_dupText1.SetWindowText(text1);
	m_dupText2.SetWindowText(text2);

	if (numFilesToCopy || numFilesToDelete)
	{
		m_updateButton.EnableWindow(TRUE);
	}
	else
	{
		m_updateButton.EnableWindow(FALSE);
	}
}

void UpdateDuplicatesDialog::SetTreeItem(int setIndex)
{
	TV_ITEM item;
	TV_ITEM_EX itemEx;

	int setFlags = m_pUpdateDuplicatesManager->GetDuplicateSetFlags(setIndex);
	
	bool bEnabled = false;
	if ((setFlags & UpdateDuplicatesManager::DUPLICATE_SET_FLAGS_ACTION_MASK) == UpdateDuplicatesManager::DUPLICATE_SET_FLAGS_ACTION_ENABLE)
		bEnabled = true;

	item.mask = TVIF_HANDLE;
	item.hItem = (HTREEITEM) m_pUpdateDuplicatesManager->GetDuplicateSetUserData(setIndex);
	
	itemEx.maskEx = TVIF_EX_STATEEX;
	itemEx.stateEx = 0;
	if (!bEnabled)
		itemEx.stateEx |= TVIS_EX_EXCLUDED;
	itemEx.stateExMask = TVIS_EX_EXCLUDED;
	
	m_pTreeCtrl->SetItemEx( &item, &itemEx );

	// add the sub entries for the set
	int numEntries = m_pUpdateDuplicatesManager->GetNumPathEntries(setIndex);
	for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
	{
		int entryFlags = m_pUpdateDuplicatesManager->GetPathEntryFlags(setIndex,entryIndex);
		
		item.mask = TVIF_HANDLE;
		item.hItem = (HTREEITEM) m_pUpdateDuplicatesManager->GetPathEntryUserData(setIndex,entryIndex);
		
		itemEx.maskEx = TVIF_EX_STATEEX;
		itemEx.stateEx = 0;

		int nIndImage;

		int firstFileAdd =  (entryFlags & UpdateDuplicatesManager::PATH_ENTRY_FIRST_FILE) ? 4 : 0;
		
		switch (entryFlags & UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_MASK)
		{
			case UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_EXCLUDE_FILE:
				itemEx.stateEx |= TVIS_EX_EXCLUDED;
				nIndImage = IMG_IND_IDX_EXCLUDE+firstFileAdd;
				break;
			case UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_DEST_FILE:
				itemEx.stateEx |= TVIS_EX_COPY_DEST;
				nIndImage = IMG_IND_IDX_DEST+firstFileAdd;
				break;
			case UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE:
				itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
				nIndImage = IMG_IND_IDX_SOURCE+firstFileAdd;
				break;
			case UpdateDuplicatesManager::PATH_ENTRY_FLAGS_ACTION_DELETE_FILE:
				itemEx.stateEx |= TVIS_EX_TO_DELETE;
				nIndImage = IMG_IND_IDX_DELETE+firstFileAdd;
				break;
		}
	
		itemEx.stateExMask = TVIS_EX_EXCLUDED|
									TVIS_EX_TO_DELETE|
									TVIS_EX_COPY_SOURCE|
									TVIS_EX_COPY_DEST;
		
		m_pTreeCtrl->SetItemEx( &item, &itemEx );

		item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		item.iImage = item.iSelectedImage = nIndImage;
		m_pTreeCtrl->SetItem( &item );
	}
}



void UpdateDuplicatesDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	CRect rect;
	GetClientRect(&rect);

	const int buttonTopY = 34;
	const int updateButtonX = 171;
	const int cancelButtonX = 86;
	const int text1Y = 35;
	const int text2Y = 17;
	const int textX = 175;
	const int treeX = 10;
	const int treeY = 39;

	// update button
	if (::IsWindow(m_updateButton.m_hWnd))
	{
		CRect myRect;
		m_updateButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_updateButton.MoveWindow(rect.right - updateButtonX, rect.bottom - buttonTopY,myRect.Width(), myRect.Height());
	}

	// cancel button
	if (::IsWindow(m_cancelButton.m_hWnd))
	{
		CRect myRect;
		m_cancelButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_cancelButton.MoveWindow(rect.right - cancelButtonX, rect.bottom - buttonTopY,myRect.Width(), myRect.Height());
	}

	// text 1
	if (::IsWindow(m_dupText1.m_hWnd))
	{
		CRect myRect;
		m_dupText1.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_dupText1.MoveWindow(myRect.left, rect.bottom - text1Y,(rect.right - textX)-myRect.left, myRect.Height());
	}

	// text 2
	if (::IsWindow(m_dupText2.m_hWnd))
	{
		CRect myRect;
		m_dupText2.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_dupText2.MoveWindow(myRect.left, rect.bottom - text2Y,(rect.right - textX)-myRect.left, myRect.Height());
	}

	// tree control
	CWnd *pCWnd = GetDlgItem(IDC_DUP_TREE);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		CRect myRect;
		pCWnd->GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		pCWnd->MoveWindow(myRect.left,myRect.top,(rect.right - treeX)-myRect.top, (rect.bottom-treeY)-myRect.left);
	}
	Invalidate(FALSE);
}

void UpdateDuplicatesDialog::OnClose()
{
	CDialog::OnClose();

	// Save out the position of the app:
	Registry registry(ASSET_MANAGER_KEY);
	GetWindowRect(&m_dialogRect);
	registry.Set(UPDATE_DUPLICATES_DIALOG_RECT_TOP_FIELD,(DWORD)m_dialogRect.top);
	registry.Set(UPDATE_DUPLICATES_DIALOG_RECT_BOTTOM_FIELD,(DWORD)m_dialogRect.bottom);
	registry.Set(UPDATE_DUPLICATES_DIALOG_RECT_LEFT_FIELD,(DWORD)m_dialogRect.left);
	registry.Set(UPDATE_DUPLICATES_DIALOG_RECT_RIGHT_FIELD,(DWORD)m_dialogRect.right);
}

void UpdateDuplicatesDialog::OnOK()
{
	// We just want to absorb this message, thus keeping the app from quitting
	// when the user hits the enter key:
	SetFocus();
}

void UpdateDuplicatesDialog::OnCancel()
{
	CDialog::OnCancel();

	// Save out the position of the app:
	Registry registry(ASSET_MANAGER_KEY);
	GetWindowRect(&m_dialogRect);
	registry.Set(UPDATE_DUPLICATES_DIALOG_RECT_TOP_FIELD,(DWORD)m_dialogRect.top);
	registry.Set(UPDATE_DUPLICATES_DIALOG_RECT_BOTTOM_FIELD,(DWORD)m_dialogRect.bottom);
	registry.Set(UPDATE_DUPLICATES_DIALOG_RECT_LEFT_FIELD,(DWORD)m_dialogRect.left);
	registry.Set(UPDATE_DUPLICATES_DIALOG_RECT_RIGHT_FIELD,(DWORD)m_dialogRect.right);
}


void UpdateDuplicatesDialog::OnUpdateSetDefault(CCmdUI* pCmdUI) 
{
	int validActionFlags = GetValidActions();
	if (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_SET_DEFAULT)
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void UpdateDuplicatesDialog::OnUpdateSourceFile(CCmdUI* pCmdUI) 
{
	int validActionFlags = GetValidActions();
	if (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_SOURCE_FILE)
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void UpdateDuplicatesDialog::OnUpdateExclude(CCmdUI* pCmdUI) 
{
	int validActionFlags = GetValidActions();
	if (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_EXCLUDE_FILE)
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void UpdateDuplicatesDialog::OnUpdateDestFile(CCmdUI* pCmdUI) 
{
	int validActionFlags = GetValidActions();
	if (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_DEST_FILE)
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void UpdateDuplicatesDialog::OnUpdateDeleteFile(CCmdUI* pCmdUI) 
{
	int validActionFlags = GetValidActions();
	if (validActionFlags & UpdateDuplicatesManager::VALID_ACTIONS_DELETE_FILE)
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}
