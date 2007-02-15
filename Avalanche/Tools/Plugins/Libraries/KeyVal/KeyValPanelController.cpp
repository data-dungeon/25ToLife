//-----------------------------------------------------------------------------
//	File:		KeyValPanelController.cpp
//	Created:	April 29, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The KeyValPanelController class controlls all functionality
//			regarding the Key/Value panel dealing with objects, faces,
//			textures, etc.
//
//	History:	4/29/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "KeyValPCH.h"
#include "KeyValPanelController.h"
#include "KeyValCommunicatorBase.h"
#include "KeyValRegistry.h"
#include "KeyValFindDialog.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

// Treeview imagelist index defines:
#define IMG_INDEX_NONE									0
#define IMG_INDEX_FOLD_SHUT_NOCHECK					1
#define IMG_INDEX_FOLD_OPEN_NOCHECK					2
#define IMG_INDEX_FOLD_SHUT_CHECK					3
#define IMG_INDEX_FOLD_OPEN_CHECK					4
#define IMG_INDEX_VAL									5
#define IMG_INDEX_ACTIVE_OFFSET						0
#define IMG_INDEX_DEFAULT_OFFSET						5
#define IMG_INDEX_UNUSED_OFFSET						10
#define IMG_INDEX_ACTIVE_DEFAULT_OFFSET			15
#define IMG_INDEX_ACTIVE_UNUSED_OFFSET				20
#define IMG_INDEX_DEFAULT_UNUSED_OFFSET			25
#define IMG_INDEX_ACTIVE_DEFAULT_UNUSED_OFFSET	30

// Registry defines:
#define KEY_VAL_MODE	"KeyValMode"


//-----------------------------------------------------------------------------
// Global objects and/or functions:

KeyValPanelController* g_pTheKeyValPanelController = NULL;

void (CALLBACK *KeyValPanelController::m_pRenameStartCallback)(void) = NULL;
void (CALLBACK *KeyValPanelController::m_pRenameEndCallback)(void) = NULL;


void KeyValPanelController::SetHWnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

static LRESULT APIENTRY PushButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
    if (uMsg == WM_KILLFOCUS || uMsg == WM_SETFOCUS)
		return TRUE;
	
	WNDPROC oldProc = (WNDPROC) GetWindowLong(hwnd, GWLP_USERDATA);
	
    return CallWindowProc(oldProc, hwnd, uMsg, wParam, lParam); 
} 

inline static int GetImageOffset(const KeyInfo &keyInfo)
{
	if (keyInfo.m_activeCount>0)
	{
		if (keyInfo.m_defaultCount>0)
		{
			if (keyInfo.m_unusedCount>0)
			{
				return IMG_INDEX_ACTIVE_DEFAULT_UNUSED_OFFSET;
			}
			else
			{
				return IMG_INDEX_ACTIVE_DEFAULT_OFFSET;
			}
		}
		else
		{
			if (keyInfo.m_unusedCount>0)
			{
				return IMG_INDEX_ACTIVE_UNUSED_OFFSET;
			}
			else
			{
				return IMG_INDEX_ACTIVE_OFFSET;
			}
		}
	}
	else
	{
		if (keyInfo.m_defaultCount>0)
		{
			if (keyInfo.m_unusedCount>0)
			{
				return IMG_INDEX_DEFAULT_UNUSED_OFFSET;
			}
			else
			{
				return IMG_INDEX_DEFAULT_OFFSET;
			}
		}
		else
		{
			if (keyInfo.m_unusedCount>0)
			{
				return IMG_INDEX_UNUSED_OFFSET;
			}
			else
			{
				return IMG_INDEX_ACTIVE_OFFSET; // this one should not be
			}
		}
	}
}

inline static int GetImageOffset(const ValInfo &valInfo)
{
	if (valInfo.m_activeCount>0)
	{
		if (valInfo.m_defaultCount>0)
		{
			if (valInfo.m_unusedCount>0)
			{
				return IMG_INDEX_ACTIVE_DEFAULT_UNUSED_OFFSET;
			}
			else
			{
				return IMG_INDEX_ACTIVE_DEFAULT_OFFSET;
			}
		}
		else
		{
			if (valInfo.m_unusedCount>0)
			{
				return IMG_INDEX_ACTIVE_UNUSED_OFFSET;
			}
			else
			{
				return IMG_INDEX_ACTIVE_OFFSET;
			}
		}
	}
	else
	{
		if (valInfo.m_defaultCount>0)
		{
			if (valInfo.m_unusedCount>0)
			{
				return IMG_INDEX_DEFAULT_UNUSED_OFFSET;
			}
			else
			{
				return IMG_INDEX_DEFAULT_OFFSET;
			}
		}
		else
		{
			if (valInfo.m_unusedCount>0)
			{
				return IMG_INDEX_UNUSED_OFFSET;
			}
			else
			{
				return IMG_INDEX_ACTIVE_OFFSET; // this one should not be
			}
		}
	}
}


void KeyValPanelController::OnInitDialog(void)
{

	// Set up the key/val panel toolbar:
	m_hSelectButton = GetDlgItem(m_hWnd, IDC_SELECT_BUTTON);
	m_hApplyButton = GetDlgItem(m_hWnd, IDC_APPLY_BUTTON);
	m_hPurgeButton = GetDlgItem(m_hWnd, IDC_PURGE_BUTTON);
	m_hRemapButton = GetDlgItem(m_hWnd, IDC_REMAP_BUTTON);
	m_hClearButton = GetDlgItem(m_hWnd, IDC_CLEAR_BUTTON);
	m_hShowActiveButton = GetDlgItem(m_hWnd, IDC_SHOW_ACTIVE_BUTTON);
	m_hShowDefaultButton = GetDlgItem(m_hWnd, IDC_SHOW_DEFAULT_BUTTON);
	m_hShowUnusedButton = GetDlgItem(m_hWnd, IDC_SHOW_UNUSED_BUTTON);
	m_hFilterButton = GetDlgItem(m_hWnd, IDC_FILTER_BUTTON);

	// subclass the buttons to handle the killfocus problem
	SetWindowLongPtr(m_hSelectButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(m_hSelectButton,GWL_WNDPROC,(LONG)PushButtonProc));
	SetWindowLongPtr(m_hApplyButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(m_hApplyButton,GWL_WNDPROC,(LONG)PushButtonProc));
	SetWindowLongPtr(m_hPurgeButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(m_hPurgeButton,GWL_WNDPROC,(LONG)PushButtonProc));
	SetWindowLongPtr(m_hRemapButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(m_hRemapButton,GWL_WNDPROC,(LONG)PushButtonProc));
	SetWindowLongPtr(m_hClearButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(m_hClearButton,GWL_WNDPROC,(LONG)PushButtonProc));
	SetWindowLongPtr(m_hShowActiveButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(m_hShowActiveButton,GWL_WNDPROC,(LONG)PushButtonProc));
	SetWindowLongPtr(m_hShowDefaultButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(m_hShowDefaultButton,GWL_WNDPROC,(LONG)PushButtonProc));
	SetWindowLongPtr(m_hShowUnusedButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(m_hShowUnusedButton,GWL_WNDPROC,(LONG)PushButtonProc));
	SetWindowLongPtr(m_hFilterButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(m_hFilterButton,GWL_WNDPROC,(LONG)PushButtonProc));

	m_hIconSelect = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_SELECT), IMAGE_ICON, 0, 0, 0);
	m_hIconApply = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_APPLY), IMAGE_ICON, 0, 0, 0);
	m_hIconPurge = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_PURGE), IMAGE_ICON, 0, 0, 0);
	m_hIconRemap = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_REMAP), IMAGE_ICON, 0, 0, 0);
	m_hIconClear = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_CLEAR), IMAGE_ICON, 0, 0, 0);
	m_hIconShowActive = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_SHOW_ACTIVE), IMAGE_ICON, 0, 0, 0);
	m_hIconShowDefault = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_SHOW_DEFAULTS), IMAGE_ICON, 0, 0, 0);
	m_hIconShowUnused = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_SHOW_UNUSED), IMAGE_ICON, 0, 0, 0);
	m_hIconFilter = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_FILTER), IMAGE_ICON, 0, 0, 0);

	::SendMessage(m_hSelectButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconSelect);
	::SendMessage(m_hApplyButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconApply);
	::SendMessage(m_hPurgeButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconPurge);
	::SendMessage(m_hRemapButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconRemap);
	::SendMessage(m_hClearButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconClear);
	::SendMessage(m_hShowActiveButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconShowActive);
	::SendMessage(m_hShowDefaultButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconShowDefault);
	::SendMessage(m_hShowUnusedButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconShowUnused);
	::SendMessage(m_hFilterButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconFilter);

	m_hApplySel = GetDlgItem(m_hWnd, IDC_BUTTON_APPLYTOSEL);
	m_hApplyScene = GetDlgItem(m_hWnd, IDC_BUTTON_APPLYTOSCENE);

	::SendMessage(m_hShowActiveButton, BM_SETSTATE, (BOOL)m_bShowActive, 0);
	::SendMessage(m_hShowDefaultButton, BM_SETSTATE, (BOOL)m_bShowDefault, 0);
	::SendMessage(m_hShowUnusedButton, BM_SETSTATE, (BOOL)m_bShowUnused, 0);

	// set up tooltips
	m_hToolTip = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		CustomControls::GetWindowClassName(CustomControls::TOOLTIP_WINDOW_CLASS), NULL,
		WS_POPUP, 
		0,0,0,0,
		m_hWnd, NULL, m_hInstance,
		NULL);

	ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(m_hToolTip);

	int toolTipBehavior = ICustomToolTip::TOOLTIP_MULTIPLE_SHOW | ICustomToolTip::TOOLTIP_TRACKING_MOUSE | ICustomToolTip::TOOLTIP_CLOSE_LEAVEWND | ICustomToolTip::TOOLTIP_NOCLOSE_OVER;
	int toolTipStyle = ICustomToolTip::TOOLTIP_SHADOW;
	pToolTip->SetBehaviour(toolTipBehavior);
	pToolTip->SetStyles(toolTipStyle);
	pToolTip->SetDelayTime(TTDT_AUTOPOP,0);
	pToolTip->SetDelayTime(TTDT_INITIAL,500);
	pToolTip->SetDelayTime(TTDT_RESHOW,100);

	pToolTip->AddTool(m_hSelectButton,0);
	pToolTip->AddTool(m_hApplyButton,0);
	pToolTip->AddTool(m_hPurgeButton,0);
	pToolTip->AddTool(m_hRemapButton,0);
	pToolTip->AddTool(m_hClearButton,0);
	pToolTip->AddTool(m_hShowActiveButton,0);
	pToolTip->AddTool(m_hShowDefaultButton,0);
	pToolTip->AddTool(m_hShowUnusedButton,0);
	pToolTip->AddTool(m_hFilterButton,0);

	pToolTip->SetNotify(TRUE);
	pToolTip->SetNotify(m_hWnd);
	pToolTip->Activate(TRUE);

	pToolTip->SetParentWnd(m_hWnd); // new parent since one gathered from constructor is wrong(since dialog is a child)

	ICustomToolTip::ReleaseIToolTip(pToolTip);
}


BOOL KeyValPanelController::OnNotify(LPARAM lParam)
{
	LPNMHDR hdr = (LPNMHDR) lParam;
	// This is where the tooltip text is provided for the toolbar buttons and the two
	// action buttons:
	switch (hdr->code)
	{
		case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
		{
			ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;

			CHAR charBuff[200];
			if (pNotify->ti->m_hWnd == m_hSelectButton)
			{
				LoadString(m_hInstance, IDS_TT_SELECT, charBuff, sizeof(charBuff));
				pNotify->ti->m_sTooltip = charBuff;
			}
			else if (pNotify->ti->m_hWnd == m_hApplyButton)
			{
				LoadString(m_hInstance, IDS_TT_APPLYVALUE, charBuff, sizeof(charBuff));
				pNotify->ti->m_sTooltip = charBuff;
			}
			else if (pNotify->ti->m_hWnd == m_hPurgeButton)
			{
				LoadString(m_hInstance, IDS_TT_PURGE, charBuff, sizeof(charBuff));
				pNotify->ti->m_sTooltip = charBuff;
			}
			else if (pNotify->ti->m_hWnd == m_hRemapButton)
			{
				LoadString(m_hInstance, IDS_TT_REMAP, charBuff, sizeof(charBuff));
				pNotify->ti->m_sTooltip = charBuff;
			}
			else if (pNotify->ti->m_hWnd == m_hClearButton)
			{
				LoadString(m_hInstance, IDS_TT_CLEAR, charBuff, sizeof(charBuff));
				pNotify->ti->m_sTooltip = charBuff;
			}
			else if (pNotify->ti->m_hWnd == m_hShowActiveButton)
			{
				LoadString(m_hInstance, IDS_TT_SHOW_ACTIVE, charBuff, sizeof(charBuff));
				pNotify->ti->m_sTooltip = charBuff;
			}
			else if (pNotify->ti->m_hWnd == m_hShowDefaultButton)
			{
				LoadString(m_hInstance, IDS_TT_SHOW_DEFAULT, charBuff, sizeof(charBuff));
				pNotify->ti->m_sTooltip = charBuff;
			}
			else if (pNotify->ti->m_hWnd == m_hShowUnusedButton)
			{
				LoadString(m_hInstance, IDS_TT_SHOW_UNUSED, charBuff, sizeof(charBuff));
				pNotify->ti->m_sTooltip = charBuff;
			}
			else if (pNotify->ti->m_hWnd == m_hFilterButton)
			{
				LoadString(m_hInstance, IDS_TT_FILTER, charBuff, sizeof(charBuff));
				pNotify->ti->m_sTooltip = charBuff;
			}
		}
		return TRUE;

		case NM_CLICK:
		{
			if (hdr->idFrom == IDC_TREE_KEYVAL)
			{
				TVHITTESTINFO ht = {0};
				DWORD dwpos = GetMessagePos();
				ht.pt.x = GET_X_LPARAM(dwpos);
				ht.pt.y = GET_Y_LPARAM(dwpos);
				MapWindowPoints(HWND_DESKTOP, hdr->hwndFrom, &ht.pt, 1);
				TreeView_HitTest(hdr->hwndFrom, &ht);
			
				if (TVHT_ONITEMSTATEICON & ht.flags)
				{
					// The user clicked on the checkbox:
					HTREEITEM hItemClicked = ht.hItem;
					HandleCheckBoxClick(hItemClicked);
				}
			}
			return TRUE;
		}
		case TVN_ITEMEXPANDED:
		{
			HandleItemExpanded(lParam);
			return TRUE;
		}
		case TVN_BEGINLABELEDIT:
		{
			if (m_pRenameStartCallback)
			{
				m_pRenameStartCallback();
			}
			HandleBeginLabelEdit((LPNMTVDISPINFO) lParam);
			return FALSE;
		}
		case TVN_ENDLABELEDIT:
		{
			if (m_pRenameStartCallback)
			{
				m_pRenameStartCallback();
			}
			HandleEndLabelEdit((LPNMTVDISPINFO) lParam);
			return FALSE;
		}
	}
	return TRUE;
}

void KeyValPanelController::OnSize(HWND hWnd,int cx,int cy)
{
	HWND hwndTreeCtrl = GetDlgItem(hWnd, IDC_TREE_KEYVAL);
	if (hwndTreeCtrl!=NULL)
	{
		RECT rect;
		GetClientRect(hWnd,&rect);
		RECT myRect;
		GetWindowRect(hwndTreeCtrl,&myRect);
		ScreenToClient(hWnd, (LPPOINT)&myRect);
		ScreenToClient(hWnd, ((LPPOINT)&myRect)+1);
		MoveWindow(hwndTreeCtrl,5,30,(rect.right-rect.left)-5,myRect.bottom-myRect.top,FALSE);
	}
}

//-----------------------------------------------------------------------------
// KeyValPanelProc
// This function acts as the callback proc for the keyval rollup panel dialog.
// Messages sent to the panel dialog are processed here.
//
// Input:
//  HWND hWnd -- Handle to the dialog.
//  UINT msg -- Specifies the message sent.
//  WPARAM wParam -- Specifies additional message-specific information.
//  LPARAM lParam -- Specifies additional message-specific information.
//
// Output: TRUE if this callback processed the message, FALSE otherwise.
//-----------------------------------------------------------------------------
static BOOL CALLBACK KeyValPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			g_pTheKeyValPanelController->SetHWnd(hWnd);
			g_pTheKeyValPanelController->OnInitDialog();
			return TRUE;
		}
		case WM_NOTIFY:
		{
			return g_pTheKeyValPanelController->OnNotify(lParam);
		}
		case WM_SIZE:
		{
			g_pTheKeyValPanelController->OnSize(hWnd,LOWORD(lParam),HIWORD(lParam));
			return TRUE;
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam)==BN_CLICKED)
			{
				switch(LOWORD(wParam))
				{
					case IDC_SELECT_BUTTON:
					{
						g_pTheKeyValPanelController->SetMode(KeyValPanelController::SELECT);
						g_pTheKeyValPanelController->UpdateTreeViewAndKeeper();
						return TRUE;
					}
					case IDC_APPLY_BUTTON:
					{
						g_pTheKeyValPanelController->SetMode(KeyValPanelController::APPLY);
						g_pTheKeyValPanelController->UpdateTreeViewAndKeeper();
						return TRUE;
					}
					case IDC_PURGE_BUTTON:
					{
						g_pTheKeyValPanelController->SetMode(KeyValPanelController::PURGE);
						g_pTheKeyValPanelController->UpdateTreeViewAndKeeper();
						return TRUE;
					}
					case IDC_REMAP_BUTTON:
					{
						g_pTheKeyValPanelController->SetMode(KeyValPanelController::REMAP);
						g_pTheKeyValPanelController->UncheckAllKeysAndVals();
						g_pTheKeyValPanelController->UpdateTreeViewAndKeeper();
						return TRUE;
					}
					case IDC_CLEAR_BUTTON:
					{
						g_pTheKeyValPanelController->UncheckAllKeysAndVals();
						g_pTheKeyValPanelController->UpdateTreeViewAndKeeper();
						return TRUE;
					}
					case IDC_SHOW_ACTIVE_BUTTON:
					{
						g_pTheKeyValPanelController->HandleShowActiveButton();
						return TRUE;
					}
					case IDC_SHOW_DEFAULT_BUTTON:
					{
						g_pTheKeyValPanelController->HandleShowDefaultButton();
						return TRUE;
					}
					case IDC_SHOW_UNUSED_BUTTON:
					{
						g_pTheKeyValPanelController->HandleShowUnusedButton();
						return TRUE;
					}
					case IDC_FILTER_BUTTON:
					{
						g_pTheKeyValPanelController->HandleFilterButton();
						return TRUE;
					}
					case IDC_BUTTON_APPLYTOSEL:
					{
						g_pTheKeyValPanelController->HandleApplyButtons();
						return TRUE;
					}
					case IDC_BUTTON_APPLYTOSCENE:
					{
						g_pTheKeyValPanelController->HandleApplyButtons(true);
						return TRUE;
					}
				}
			}
		}
		case WM_SETCURSOR:
		{
			return g_pTheKeyValPanelController->OnSetCursor((UINT)LOWORD(lParam),(UINT)HIWORD(lParam));
		}
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// KeyValPanelController::KeyValPanelController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
KeyValPanelController::KeyValPanelController(KeyValCommunicatorBase* pKeyValCommBase)
{
	m_pKeyValCommunicatorBase = pKeyValCommBase;
	m_hSelectButton = NULL;
	m_hApplyButton = NULL;
	m_hPurgeButton = NULL;
	m_hRemapButton = NULL;
	m_hClearButton = NULL;
	m_hShowActiveButton = NULL;
	m_hShowDefaultButton = NULL;
	m_hShowUnusedButton = NULL;
	m_hFilterButton = NULL;
	m_hIconSelect = NULL;
	m_hIconApply = NULL;
	m_hIconPurge = NULL;
	m_hIconRemap = NULL;
	m_hIconClear = NULL;
	m_hIconShowActive = NULL;
	m_hIconShowDefault = NULL;
	m_hIconShowUnused = NULL;
	m_hIconFilter = NULL;
	m_mode = NONE;
	m_hwndPanel = NULL;
	m_hwndTreeCtrl = NULL;
	m_meansToEditLabel = EL_NONE;
	m_pFindDialog = NULL;
	m_previousVisibleKeyIndex = -1;
	m_previousVisibleValueIndex = -1;
	m_bShowActive = true;
	m_bShowDefault = false;
	m_bShowUnused = false;
}


//-----------------------------------------------------------------------------
// KeyValPanelController::~KeyValPanelController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
KeyValPanelController::~KeyValPanelController()
{
	if (m_hIconSelect)
		::DestroyIcon(m_hIconSelect);
	if (m_hIconApply)
		::DestroyIcon(m_hIconApply);
	if (m_hIconPurge)
		::DestroyIcon(m_hIconPurge);
	if (m_hIconRemap)
		::DestroyIcon(m_hIconRemap);
	if (m_hIconClear)
		::DestroyIcon(m_hIconClear);

	if (m_hIconShowActive)
		::DestroyIcon(m_hIconShowActive);
	if (m_hIconShowDefault)
		::DestroyIcon(m_hIconShowDefault);
	if (m_hIconShowUnused)
		::DestroyIcon(m_hIconShowUnused);

	if (m_hIconFilter)
		::DestroyIcon(m_hIconFilter);

	if (m_pFindDialog)
		delete m_pFindDialog;
}

//-----------------------------------------------------------------------------
// KeyValPanelController::InitKeyValPanel
// This method will initialize and append the keyval panel to the rollup
// window passed in.
//
// Input:
//	IRollupWindow* pRollupWindow -- The rollup window to append the panel to.
//  HINSTANCE hInstance -- The HINSTANCE of the plugin to use.
//  LPARAM lParam -- The LPARAM to use.  This should be the parent HWND cast as
//					 an LPARAM.
//	int nCategory -- The category to use for the keyval panel order.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	m_hInstance = hInstance;
	// Get the mode from the registry:
	Mode regMode = NONE;
	DWORD dwMode;
	KeyValRegistry kvReg(m_pKeyValCommunicatorBase->GetRegKeyName());
	if (kvReg.Get(KEY_VAL_MODE, &dwMode) == TRUE)
	{
		regMode = (Mode) dwMode;
	}

	// Set the mode:
	m_mode = regMode;

	// Set up the title:
	static TCHAR buf[80];
	int bufLen = 80;
	LoadString(hInstance, IDS_KEYS_AND_VALUES, buf, bufLen);

	// Append the rollup:
	int nPanelIndex = pCustomRollupWindow->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_KEYVALPANEL), 
												  KeyValPanelProc, buf, lParam, 0);

	// Get the HWNDs:
	m_hwndPanel = pCustomRollupWindow->GetPanelDlg(nPanelIndex);
	m_hwndTreeCtrl = GetDlgItem(m_hwndPanel, IDC_TREE_KEYVAL);

	// Set up the styles, etc. on the treeview:
	TreeView_SetBkColor(m_hwndTreeCtrl, RGB(255, 255, 255));

	// Set up the image lists (for this to work correctly, I needed to add the
	// normal image list first):
	HIMAGELIST normalImageList;
	normalImageList = ImageList_LoadImage(hInstance, MAKEINTRESOURCE(IDB_TREEVIEW_NORMAL),
										 16, 1, RGB(190, 190, 190), IMAGE_BITMAP, LR_CREATEDIBSECTION);
	TreeView_SetImageList(m_hwndTreeCtrl, normalImageList, TVSIL_NORMAL);

	HIMAGELIST stateImageList;
	stateImageList = ImageList_LoadImage(hInstance, MAKEINTRESOURCE(IDB_TREEVIEW_STATE),
										 16, 1, RGB(190, 190, 190), IMAGE_BITMAP, 0);
	TreeView_SetImageList(m_hwndTreeCtrl, stateImageList, TVSIL_STATE);

	// Call SetMode so that the buttons and states get set correctly:
	SetMode(regMode, false);
}


//-----------------------------------------------------------------------------
// KeyValPanelController::IdCancelMessage
// This method is called to clean up anything dealing with the keyval panel.
// It should be called whenever the parent dialog or window is going away.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::IdCancelMessage()
{
	// Save out the mode to the registry:
	KeyValRegistry kvReg(m_pKeyValCommunicatorBase->GetRegKeyName());
	kvReg.Set(KEY_VAL_MODE, (DWORD) GetMode());

	// Now clean up:
	m_hwndPanel = NULL;
	m_hwndTreeCtrl = NULL;

	if (m_pFindDialog)
		delete m_pFindDialog;
	m_pFindDialog = NULL;
}


//-----------------------------------------------------------------------------
// KeyValPanelController::SetMode
// This method will enable and disable the correct buttons in the toolbar.  It
// will also set the internal mode correctly.
//
// Input:
//  Mode newMode -- The new mode to set to.
//	bool bRespectLastState -- If true is passed in, then if the current mode is
//							  the same as the mode passed in, then the mode is
//							  set to NONE.  If false is passed in, then the
//							  current mode setting is ignored.  This is used
//							  to allow the buttons to be all in the out
//							  position and the mode set to NONE.  Default value
//							  is true.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::SetMode(Mode newMode, bool bRespectLastState)
{
	// Set the button state and mode accordingly:
	if ((bRespectLastState == true) && (m_mode == newMode))
	{
		::SendMessage(m_hSelectButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hApplyButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hPurgeButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hRemapButton, BM_SETSTATE, FALSE, 0);

		m_mode = NONE;
		SetApplyBttnStates(false, false);
	}
	else if (newMode == NONE)
	{
		::SendMessage(m_hSelectButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hApplyButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hPurgeButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hRemapButton, BM_SETSTATE, FALSE, 0);
		m_mode = NONE;
		SetApplyBttnStates(false, false);
	}
	else if (newMode == SELECT)
	{
		::SendMessage(m_hSelectButton, BM_SETSTATE, TRUE, 0);
		::SendMessage(m_hApplyButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hPurgeButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hRemapButton, BM_SETSTATE, FALSE, 0);
		m_mode = SELECT;
		SetApplyBttnStates(true, true);
	}
	else if (newMode == APPLY)
	{
		::SendMessage(m_hSelectButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hApplyButton, BM_SETSTATE, TRUE, 0);
		::SendMessage(m_hPurgeButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hRemapButton, BM_SETSTATE, FALSE, 0);
		m_mode = APPLY;
		SetApplyBttnStates(true, true);
	}
	else if (newMode == PURGE)
	{
		::SendMessage(m_hSelectButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hApplyButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hPurgeButton, BM_SETSTATE, TRUE, 0);
		::SendMessage(m_hRemapButton, BM_SETSTATE, FALSE, 0);
		m_mode = PURGE;
		SetApplyBttnStates(true, true);
	}
	else if (newMode == REMAP)
	{
		::SendMessage(m_hSelectButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hApplyButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hPurgeButton, BM_SETSTATE, FALSE, 0);
		::SendMessage(m_hRemapButton, BM_SETSTATE, TRUE, 0);
		m_mode = REMAP;
		SetApplyBttnStates(true, true);
	}

	// Now update the treeview styles:
	UpdateTreeViewStyles();
}


//-----------------------------------------------------------------------------
// KeyValPanelController::UpdateTreeViewStyles
// This method will change the styles for the treeview in the keyval panel
// according to the current mode.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::UpdateTreeViewStyles()
{
	if (GetMode() == REMAP)
	{
		// Add the TVS_EDITLABELS style:
		LONG dwStyle = GetWindowLong(m_hwndTreeCtrl, GWL_STYLE);
		SetWindowLong(m_hwndTreeCtrl, GWL_STYLE, dwStyle | TVS_EDITLABELS);
	}
	else
	{
		// Remove the TVS_EDITLABELS style:
		LONG dwStyle = GetWindowLong(m_hwndTreeCtrl, GWL_STYLE);
		SetWindowLong(m_hwndTreeCtrl, GWL_STYLE, dwStyle & ~TVS_EDITLABELS);
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::GetMode
// This method will return the current mode (purge, remap, etc.) for the
// treeview in the keyval panel.
//
// Input: None.
//
// Output: Mode of the treeview. 
//-----------------------------------------------------------------------------
KeyValPanelController::Mode KeyValPanelController::GetMode()
{
	return m_mode;
}


//-----------------------------------------------------------------------------
// KeyValPanelController::UncheckAllKeysAndVals
// This method will go through all the items (keys and vals) in the treeview
// and change all to their unchecked state.  It will also changed all the items
// in the keeper to be unchecked.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::UncheckAllKeysAndVals()
{
	// Cycle through each key in the keeper:
	int nNumKeys = m_keyKeeper.GetSize();
	for (int nIndex = 0; nIndex < nNumKeys; nIndex++)
	{
		// Change this key to be unchecked in the keeper:
		m_keyKeeper[nIndex].m_bChecked = false;

		if (!m_keyKeeper[nIndex].m_bVisible)
			continue;

		// Change this key's item in the treeview to be unchecked, its normal
		// image to show folder without a check, and its keyname back to the
		// original (with no bolding):
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_IMAGE | TVIF_TEXT;
		tvKey.hItem = m_keyKeeper[nIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(1);
		tvKey.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
		if (m_keyKeeper[nIndex].m_bExpanded == true)
		{
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
		}
		else
		{
			tvKey.iImage = IMG_INDEX_FOLD_SHUT_NOCHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_SHUT_NOCHECK;
		}
		int imageOffset = GetImageOffset(m_keyKeeper[nIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;

		tvKey.pszText = (LPTSTR) ((const char *)m_keyKeeper[nIndex].m_strKeyname);
		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);

		// Cycle through each val for this key:
		int nNumVals = m_keyKeeper[nIndex].m_valKeeper.GetSize();
		for (int nLoop = 0; nLoop < nNumVals; nLoop++)
		{
			// Change this val to be unchecked in the keeper:
			m_keyKeeper[nIndex].m_valKeeper[nLoop].m_bChecked = false;

			if (!m_keyKeeper[nIndex].m_valKeeper[nLoop].m_bVisible)
				continue;

			// Change this val's item in the treeview to be unchecked, and its
			// valname back to the original (with no bolding):
			TV_ITEM tvVal;
			tvVal.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
			tvVal.hItem = m_keyKeeper[nIndex].m_valKeeper[nLoop].m_hTreeItem;
			tvVal.state = INDEXTOSTATEIMAGEMASK(1);
			tvVal.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
			tvVal.pszText = (LPTSTR) ((const char *)m_keyKeeper[nIndex].m_valKeeper[nLoop].m_strVal);
			TreeView_SetItem(m_hwndTreeCtrl, &tvVal);
		}
	}
}

//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::GetRegKeyName
// This method will return the m_pRegKeyName member data.
//
// Input: None
//
// Output: Pointer to a char representing the registry keyname to use. 
//-----------------------------------------------------------------------------
char* KeyValPanelController::GetRegKeyName()
{
	return m_pKeyValCommunicatorBase->GetRegKeyName();
}

//-----------------------------------------------------------------------------
// KeyValPanelController::SetApplyBttnStates
// This method can be called to set the state (enabled or disabled) for the
// two apply buttons.
//
// Input:
//  bool bApplySelEnabled -- Pass in true to enable the Apply To Sel button.
//  bool bApplySceneEnabled -- Pass in true to enable the Apply To Scene button.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled)
{
	if (m_hwndPanel != NULL)
	{
		if (GetMode() == NONE)
		{
			EnableWindow(m_hApplySel, false);
			EnableWindow(m_hApplyScene, false);
		}
		else
		{
			EnableWindow(m_hApplySel, bApplySelEnabled);
			EnableWindow(m_hApplyScene, bApplySceneEnabled);
		}
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::BuildTreeView
// This method will build the treeview in the keyval panel based on the data in
// the keykeeper.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::BuildTreeView()
{
	::SendMessage(m_hwndTreeCtrl,WM_SETREDRAW,FALSE,0);
	// Make sure any data in the treeview is removed:
	BOOL bSuccess = TreeView_DeleteAllItems(m_hwndTreeCtrl);
//	UpdateWindow(m_hwndTreeCtrl);

	// Cycle through all the keys, adding each to the treeview:
	Mode currMode = GetMode();
	int nNumKeys = m_keyKeeper.GetSize();
	for (int nIndex = 0; nIndex < nNumKeys; nIndex++)
	{
		KeyInfo &sourceKeyInfo = m_keyKeeper[nIndex];

		if (!sourceKeyInfo.m_bVisible)
		{
			continue;
		}

		// Set up the structure for the key and val indices to store:
		KeyValIndices kvIndices;
		kvIndices.m_nKeyIndex = nIndex;
		kvIndices.m_nValIndex = -1;

		// Now create the insert structure for the key:
		TV_INSERTSTRUCT insertStructKey;
		insertStructKey.hParent = TVI_ROOT;
		insertStructKey.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE;
		insertStructKey.item.iImage = IMG_INDEX_FOLD_SHUT_NOCHECK;
		insertStructKey.item.iSelectedImage = IMG_INDEX_FOLD_SHUT_NOCHECK;

		int imageOffset = GetImageOffset(sourceKeyInfo);
		insertStructKey.item.iImage += imageOffset;
		insertStructKey.item.iSelectedImage += imageOffset;

		if ((currMode == NONE) || (currMode == APPLY))
		{
			insertStructKey.item.state = INDEXTOSTATEIMAGEMASK(0);
		}
		else
		{
			if (sourceKeyInfo.m_bChecked == true)
			{
				insertStructKey.item.state = INDEXTOSTATEIMAGEMASK(2);
			}
			else
			{
				insertStructKey.item.state = INDEXTOSTATEIMAGEMASK(1);
			}
		}
		insertStructKey.item.stateMask = TVIS_STATEIMAGEMASK;
		insertStructKey.item.pszText = (LPTSTR) ((const char *)sourceKeyInfo.m_strKeyname);
		insertStructKey.item.lParam = *((LPARAM*)&kvIndices);
		insertStructKey.hInsertAfter = TVI_SORT;
		HTREEITEM newItemKey = TreeView_InsertItem(m_hwndTreeCtrl, &insertStructKey);
		sourceKeyInfo.m_hTreeItem = newItemKey;

		// Cycle through all the vals for this key, adding each to the treeview:
		int nNumVals = sourceKeyInfo.m_valKeeper.GetSize();
		for (int nLoop = 0; nLoop < nNumVals; nLoop++)
		{
			ValInfo &sourceValInfo = sourceKeyInfo.m_valKeeper[nLoop];

			if (!sourceValInfo.m_bVisible)
				continue;

			// Set the index for the value to store:
			kvIndices.m_nValIndex = nLoop;

			// Create the insert structure for this value:
			TV_INSERTSTRUCT insertStructVal;
			insertStructVal.hParent = newItemKey;
			insertStructVal.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE;

			int imageIndex = IMG_INDEX_VAL + GetImageOffset(sourceValInfo);
			insertStructVal.item.iImage = imageIndex;
			insertStructVal.item.iSelectedImage = imageIndex;
			if (currMode == NONE)
			{
				insertStructVal.item.state = INDEXTOSTATEIMAGEMASK(0);
			}
			else
			{
				if (sourceValInfo.m_bChecked == true)
					insertStructVal.item.state = INDEXTOSTATEIMAGEMASK(2);
				else
					insertStructVal.item.state = INDEXTOSTATEIMAGEMASK(1);
			}
			insertStructVal.item.stateMask = TVIS_STATEIMAGEMASK;
			insertStructVal.item.pszText = (LPTSTR) ((const char *)sourceValInfo.m_strVal);
			insertStructVal.item.lParam = *((LPARAM*)&kvIndices);
			insertStructVal.hInsertAfter = TVI_SORT;
			HTREEITEM newItemVal = TreeView_InsertItem(m_hwndTreeCtrl, &insertStructVal);
			sourceValInfo.m_hTreeItem = newItemVal;				
		}

		// Check to see if the key should be expanded or not:
		if (sourceKeyInfo.m_bExpanded == true)
		{
			TreeView_Expand(m_hwndTreeCtrl, newItemKey, TVE_EXPAND);
		}
	}

	// If the mode is REMAP, use any new names that exist:
	if (currMode == REMAP)
		ReplaceWithNewNamesInTV();

	::SendMessage(m_hwndTreeCtrl,WM_SETREDRAW,TRUE,0);

}

void KeyValPanelController::AdjustTreeViewScroll()
{
	if (m_previousVisibleKeyIndex != -1)
	{
		int newVisibleKeyIndex = -1;
		int newVisibleValueIndex = -1;
		const KeyInfo &previousKeyInfo = m_previousKeyKeeper[m_previousVisibleKeyIndex];

		for (int keyIndex = 0; keyIndex < m_keyKeeper.GetSize(); keyIndex++)
		{
			const KeyInfo &keyInfo = m_keyKeeper[keyIndex];

			if (!keyInfo.m_bVisible)
				continue;

			if (keyInfo.m_strKeyname == previousKeyInfo.m_strKeyname)
			{
				newVisibleKeyIndex = keyIndex;
				if (m_previousVisibleValueIndex != -1)
				{
					const ValInfo &previousValueInfo = previousKeyInfo.m_valKeeper[m_previousVisibleValueIndex];
					for (int valueIndex = 0; valueIndex < keyInfo.m_valKeeper.GetSize(); valueIndex++)
					{
						const ValInfo &valueInfo = keyInfo.m_valKeeper[valueIndex];

						if (!valueInfo.m_bVisible)
							continue;

						if (valueInfo.m_strVal == previousValueInfo.m_strVal)
						{
							newVisibleValueIndex = valueIndex;
						}
					}
					// if not found then try to go to the same index 
					if (newVisibleValueIndex == -1)
					{
						newVisibleValueIndex = m_previousVisibleValueIndex;
						if (newVisibleValueIndex > keyInfo.m_valKeeper.GetSize()-1)
						{
							newVisibleValueIndex = keyInfo.m_valKeeper.GetSize()-1;
						}
					}
				}
			}
		}
		// if not found try to go to the same index
		if (newVisibleKeyIndex == -1)
		{
			newVisibleKeyIndex = m_previousVisibleKeyIndex;
			if (newVisibleKeyIndex > m_keyKeeper.GetSize()-1)
			{
				newVisibleKeyIndex = m_keyKeeper.GetSize()-1;
			}
		}
		if (newVisibleKeyIndex != -1)
		{
			if (newVisibleValueIndex != -1)
			{
				TreeView_Select(m_hwndTreeCtrl,m_keyKeeper[newVisibleKeyIndex].m_valKeeper[newVisibleValueIndex].m_hTreeItem,TVGN_FIRSTVISIBLE);
			}
			else
			{
				TreeView_Select(m_hwndTreeCtrl,m_keyKeeper[newVisibleKeyIndex].m_hTreeItem,TVGN_FIRSTVISIBLE);
			}
		}
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::ReplaceWithNewNamesInTV
// This method will go through all the entries in the treeview and if the entry
// is checked, it will replace its original name with its newname (if it
// exists).  This method should really only be called if the current mode is
// REMAP.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::ReplaceWithNewNamesInTV()
{
	// Cycle through all the keys:
	int nNumKeys = m_keyKeeper.GetSize();
	for (int nKeyIndex = 0; nKeyIndex < nNumKeys; nKeyIndex++)
	{
		if ((m_keyKeeper[nKeyIndex].m_bVisible == true) &&
			 (m_keyKeeper[nKeyIndex].m_bChecked == true) &&
			 (m_keyKeeper[nKeyIndex].m_strNewKeyname != ""))
		{
			// Build the new item:
			TV_ITEM tvKey;
			tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
			tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
			tvKey.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
			tvKey.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
			tvKey.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_strNewKeyname);
			TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
		}

		// Cycle through all the vals for this key:
		int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
		for (int nValIndex = 0; nValIndex < nNumVals; nValIndex++)
		{
			if ((m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bVisible == true) &&
				 (m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked == true) &&
				 (m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal != ""))
			{
				// Build the new item:
				TV_ITEM tvVal;
				tvVal.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
				tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
				tvVal.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
				tvVal.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
				tvVal.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal);
				TreeView_SetItem(m_hwndTreeCtrl, &tvVal);
			}
		}
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::UpdateTreeViewAndKeeper
// This method will update the data in the keeper based on the current attrib
// data and on the current mode (purge, remap, etc.).  It will then update the
// treeview accordingly.  The current state of the treeview (expanded items,
// items checked, etc.) are maintained when at all possible.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::UpdateTreeViewAndKeeper()
{
	// update previous key keeper if it has something
	if (m_keyKeeper.GetSize())
	{
		m_previousKeyKeeper = m_keyKeeper;
		m_previousVisibleKeyIndex = -1;
		m_previousVisibleValueIndex = -1;

		HTREEITEM hFirstVisibleItem = TreeView_GetFirstVisible(m_hwndTreeCtrl);
		if (hFirstVisibleItem != NULL)
		{
			TV_ITEM itemVisible;
			memset(&itemVisible, 0, sizeof(itemVisible));
			itemVisible.hItem = hFirstVisibleItem;
			itemVisible.mask = TVIF_PARAM;
			TreeView_GetItem(m_hwndTreeCtrl, &itemVisible);

			m_previousVisibleKeyIndex = (*(KeyValIndices*)&(itemVisible.lParam)).m_nKeyIndex;
			m_previousVisibleValueIndex = (*(KeyValIndices*)&(itemVisible.lParam)).m_nValIndex;
		}
	}

	// Now clear out the main keeper and load it with the current attrib data.
	// Note: LoadKeyValPanelData() will clear the keeper for us:
	m_pKeyValCommunicatorBase->LoadKeyValPanelData(m_keyKeeper);

	// Deal with the show buttons and the filter
	MarkVisibleEntries();

	// Next, merge any data from the previous keeper into the main keeper:
	MergeDataIntoMainKeeper();

	// Now update the data in the main keeper based on the current mode:
	UpdateKeeperBasedOnMode();

	// Now build the treeview based on the newly built keeper:
	BuildTreeView();

	// Now update the scroll of the tree to be what it was
	AdjustTreeViewScroll();
}


//-----------------------------------------------------------------------------
// KeyValPanelController::MarkVisibleEntries
// This method will make sure the keeper only has valid entries from the filter
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::MarkVisibleEntries()
{
	// if we have an active filter
	if (!m_bShowActive || !m_bShowDefault || m_bShowUnused)
	{
		for (int sourceKeeperIndex = 0; sourceKeeperIndex < m_keyKeeper.GetSize(); sourceKeeperIndex++)
		{
			KeyInfo &sourceKeyInfo = m_keyKeeper[sourceKeeperIndex];
			if ((sourceKeyInfo.m_activeCount > 0 && m_bShowActive) ||
				 (sourceKeyInfo.m_defaultCount > 0 && m_bShowDefault) ||
				 (sourceKeyInfo.m_unusedCount > 0 && m_bShowUnused))
			{
				sourceKeyInfo.m_bVisible = true;
				int numValues = sourceKeyInfo.m_valKeeper.GetSize();
				for (int sourceValueIndex = 0; sourceValueIndex < numValues; sourceValueIndex++)
				{
					ValInfo &sourceValInfo = sourceKeyInfo.m_valKeeper[sourceValueIndex];
					if ((sourceValInfo.m_activeCount > 0 && m_bShowActive) ||
						(sourceValInfo.m_defaultCount > 0 && m_bShowDefault) ||
						(sourceValInfo.m_unusedCount > 0 && m_bShowUnused))
					{
						sourceValInfo.m_bVisible = true;
					}
				}
			}
		}
	}
	else
	{
		for (int sourceKeeperIndex = 0; sourceKeeperIndex < m_keyKeeper.GetSize(); sourceKeeperIndex++)
		{
			KeyInfo &sourceKeyInfo = m_keyKeeper[sourceKeeperIndex];
			sourceKeyInfo.m_bVisible = true;
			int numValues = sourceKeyInfo.m_valKeeper.GetSize();
			for (int sourceValueIndex = 0; sourceValueIndex < numValues; sourceValueIndex++)
			{
				ValInfo &sourceValInfo = sourceKeyInfo.m_valKeeper[sourceValueIndex];
				sourceValInfo.m_bVisible = true;
			}
		}
	}

	// if we have an active filter
	if (m_filterKeyKeeper.GetSize())
	{
		for (int sourceKeeperIndex = 0; sourceKeeperIndex < m_keyKeeper.GetSize(); sourceKeeperIndex++)
		{
			KeyInfo &sourceKeyInfo = m_keyKeeper[sourceKeeperIndex];
			// if a visible key is not in the list mark as invisible
			if (sourceKeyInfo.m_bVisible && m_filterKeyKeeper.GetIndex(sourceKeyInfo) == -1)
			{
				sourceKeyInfo.m_bVisible = false;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// KeyValPanelController::MergeDataIntoMainKeeper
// This method will merge the data from the incoming souce keykeeper into the
// main keykeeper.  The source keykeeper data has precedence and will override
// any data in the main keykeeper.  The exception is that no handles are copied
// into the main keykeeper since they will be reset when the treeview is
// created.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::MergeDataIntoMainKeeper()
{
	// Walk through all the keys in the source keykeeper:
	int nNumKeys = m_previousKeyKeeper.GetSize();
	for (int nIndex = 0; nIndex < nNumKeys; nIndex++)
	{
		// See if there is a key in the main keeper that matches up with this key:
		int nKeyMainIndex = m_keyKeeper.GetIndex(m_previousKeyKeeper[nIndex]);
		if (nKeyMainIndex != -1)
		{
			// Copy the necessary data for this key into the main key:
			m_keyKeeper[nKeyMainIndex].m_strNewKeyname = m_previousKeyKeeper[nIndex].m_strNewKeyname;
			m_keyKeeper[nKeyMainIndex].m_bChecked = m_previousKeyKeeper[nIndex].m_bChecked;
			m_keyKeeper[nKeyMainIndex].m_bExpanded = m_previousKeyKeeper[nIndex].m_bExpanded;

			// Now walk through all the vals for this key in the source keykeeper:
			int nNumVals = m_previousKeyKeeper[nIndex].m_valKeeper.GetSize();
			for (int nLoop = 0; nLoop < nNumVals; nLoop++)
			{
				// See if there is a val for this key in the main keeper that
				// matches up with this val:
				int nValMainIndex = m_keyKeeper[nKeyMainIndex].m_valKeeper.GetIndex(m_previousKeyKeeper[nIndex].m_valKeeper[nLoop]);
				if (nValMainIndex != -1)
				{
					// Copy the necessary data for this val into the main val:
					m_keyKeeper[nKeyMainIndex].m_valKeeper[nValMainIndex].m_strNewVal = m_previousKeyKeeper[nIndex].m_valKeeper[nLoop].m_strNewVal;
					m_keyKeeper[nKeyMainIndex].m_valKeeper[nValMainIndex].m_bChecked = m_previousKeyKeeper[nIndex].m_valKeeper[nLoop].m_bChecked;
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::UpdateKeeperBasedOnMode
// This method will update the data in the main keykeeper based on the current
// mode.  For example, if the current mode is PURGE, and a key is checked, it
// will make sure that all the values for that key are checked.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::UpdateKeeperBasedOnMode()
{
	switch (m_mode)
	{
		case SELECT:
		{
			// If any val is checked, make sure its key is checked:
			int nNumKeys = m_keyKeeper.GetSize();
			for (int nIndex = 0; nIndex < nNumKeys; nIndex++)
			{
				int nNumVals = m_keyKeeper[nIndex].m_valKeeper.GetSize();
				for (int nLoop = 0; nLoop < nNumVals; nLoop++)
				{
					if (m_keyKeeper[nIndex].m_valKeeper[nLoop].m_bChecked == true)
					{
						m_keyKeeper[nIndex].m_bChecked = true;
						break;
					}
				}
			}
			break;
		}
		case PURGE:
		{
			// If a key is checked, then make sure all its vals are checked.
			// Conversely, if all the vals for a key are checked, make sure the
			// key itself is checked:
			int nNumKeys = m_keyKeeper.GetSize();
			for (int nIndex = 0; nIndex < nNumKeys; nIndex++)
			{
				if (m_keyKeeper[nIndex].m_bChecked == true)
				{
					// Make sure all the vals are checked:
					int nNumVals = m_keyKeeper[nIndex].m_valKeeper.GetSize();
					for (int nLoop = 0; nLoop < nNumVals; nLoop++)
					{
						m_keyKeeper[nIndex].m_valKeeper[nLoop].m_bChecked = true;
					}
				}
				else
				{
					// If all the vals are checked, make sure the key is checked:
					bool bAllValsChecked = true;
					int nNumVals = m_keyKeeper[nIndex].m_valKeeper.GetSize();
					for (int nLoop = 0; nLoop < nNumVals; nLoop++)
					{
						if (m_keyKeeper[nIndex].m_valKeeper[nLoop].m_bChecked == false)
						{
							bAllValsChecked = false;
							break;
						}
					}
					if (bAllValsChecked == true)
					{
						m_keyKeeper[nIndex].m_bChecked = true;
					}
				}
			}			 
			break;
		}
		case APPLY:
		{
			// If more than one value is checked for a key, make sure that only
			// the first one is checked and the others are changed to nonchecked:
			int nNumKeys = m_keyKeeper.GetSize();
			for (int nIndex = 0; nIndex < nNumKeys; nIndex++)
			{
				// only when not a checklist value
				if (!m_pKeyValCommunicatorBase->IsCheckListKey(m_keyKeeper[nIndex].m_strKeyname))
				{
					bool bFoundFirstCheck = false;
					int nNumVals = m_keyKeeper[nIndex].m_valKeeper.GetSize();
					for (int nLoop = 0; nLoop < nNumVals; nLoop++)
					{
						if (bFoundFirstCheck == true)
						{
							m_keyKeeper[nIndex].m_valKeeper[nLoop].m_bChecked = false;
						}
						else
						{
							if (m_keyKeeper[nIndex].m_valKeeper[nLoop].m_bChecked == true)
								bFoundFirstCheck = true;
						}
					}
				}
			}
			break;
		}
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleBeginLabelEdit
// This method will be called when the user initiates an editing of an item's
// label by actually clicking on the label (rather than in its checkbox).
//
// Input:
//  LPNMTVDISPINFO pTVDispInfo -- A pointer to a NMTVDISPINFO object containing
//								  data about the item being edited.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleBeginLabelEdit(LPNMTVDISPINFO pTVDispInfo)
{
	if (m_meansToEditLabel == EL_NONE)
	{
		// See if we are dealing with a key or a val and call the appropriate method:
		if (pTVDispInfo != NULL)
		{
			TV_ITEM tvItem = pTVDispInfo->item;
			short nValIndex = (*(KeyValIndices*)&(tvItem.lParam)).m_nValIndex;
			if (nValIndex == -1)
				HandleBeginLabelEditKey(pTVDispInfo);
			else
				HandleBeginLabelEditVal(pTVDispInfo);
		}
	}
	else
	{
		// Some other means was used, in which case other methods will handle
		// setting things up for the beginning of the editing of a label.
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleBeginLabelEditKey
// This method will be called when the user initiates an editing of a key's
// label by actually clicking on the label (rather than in its checkbox).
//
// Input:
//  LPNMTVDISPINFO pTVDispInfo -- A pointer to a NMTVDISPINFO object containing
//								  data about the item being edited.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleBeginLabelEditKey(LPNMTVDISPINFO pTVDispInfo)
{
	TV_ITEM tvKeyItem = pTVDispInfo->item;
	short nKeyIndex = (*(KeyValIndices*)&(tvKeyItem.lParam)).m_nKeyIndex;

	// Set the key's entry in the keykeeper to the correct checked state:
	m_keyKeeper[nKeyIndex].m_bChecked = true;

	// Change the state image and text for the key entry in the treeview accordingly:
	if (m_keyKeeper[nKeyIndex].m_strNewKeyname == "")
	{
		// No new keyname to use:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(2);
		tvKey.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
	}
	else
	{
		// There is a new keyname to use:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
		tvKey.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
		tvKey.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_strNewKeyname);
		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);

		// Since the edit control is already up by the point this method is called,
		// we have to get the edit control and put the correct text in it:
		HWND hwndCurrEdit = TreeView_GetEditControl(m_hwndTreeCtrl);
		SetWindowText(hwndCurrEdit, m_keyKeeper[nKeyIndex].m_strNewKeyname);
	}

	// Set the flag to show the user is editing a key label:
	m_meansToEditLabel = EL_KEYLABEL;
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleBeginLabelEditVal
// This method will be called when the user initiates an editing of a val's
// label by actually clicking on the label (rather than in its checkbox).
//
// Input:
//  LPNMTVDISPINFO pTVDispInfo -- A pointer to a NMTVDISPINFO object containing
//								  data about the item being edited.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleBeginLabelEditVal(LPNMTVDISPINFO pTVDispInfo)
{
	TV_ITEM tvValItem = pTVDispInfo->item;
	short nKeyIndex = (*(KeyValIndices*)&(tvValItem.lParam)).m_nKeyIndex;
	short nValIndex = (*(KeyValIndices*)&(tvValItem.lParam)).m_nValIndex;

	// Set the val's entry in the keykeeper to the correct checked state:
	m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = true;

	// Change the state image and text for the val entry in the treeview accordingly:
	if (m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal == "")
	{
		// No new valname to use:
		TV_ITEM tvVal;
		tvVal.mask = TVIF_HANDLE | TVIF_STATE;
		tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
		tvVal.state = INDEXTOSTATEIMAGEMASK(2);
		tvVal.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvVal);
	}
	else
	{
		// There is a new valname to use:
		TV_ITEM tvVal;
		tvVal.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
		tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
		tvVal.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
		tvVal.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
		tvVal.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal);
		TreeView_SetItem(m_hwndTreeCtrl, &tvVal);

		// Since the edit control is already up by the point this method is called,
		// we have to get the edit control and put the correct text in it:
		HWND hwndCurrEdit = TreeView_GetEditControl(m_hwndTreeCtrl);
		SetWindowText(hwndCurrEdit,m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal);
	}

	// Change the normal image for the parent key accordingly:
	TV_ITEM tvKey;
	tvKey.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
	tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
	tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;

	int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
	tvKey.iImage += imageOffset;
	tvKey.iSelectedImage += imageOffset;

	TreeView_SetItem(m_hwndTreeCtrl, &tvKey);

	// Set the flag to show the user is editing a key label:
	m_meansToEditLabel = EL_VALLABEL;
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleEndLabelEdit
// This method will be called to handle the ending of label editing by the user.
//
// Input:
//  LPNMTVDISPINFO pTVDispInfo -- A pointer to a NMTVDISPINFO object containing
//								  data about the item being edited.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleEndLabelEdit(LPNMTVDISPINFO pTVDispInfo)
{
	// Detect what the user was doing to get into edit mode in the first place,
	// and then call the appropriate method to handle the label edit:
	switch (m_meansToEditLabel)
	{
		case EL_NONE:
		{
			break;
		}
		case EL_KEYCHECK:
		{
			HandleEndLabelEditKeyCheck(pTVDispInfo);
			break;
		}
		case EL_VALCHECK:
		{
			HandleEndLabelEditValCheck(pTVDispInfo);
			break;
		}
		case EL_KEYLABEL:
		{
			HandleEndLabelEditKeyCheck(pTVDispInfo);
			break;
		}
		case EL_VALLABEL:
		{
			HandleEndLabelEditValCheck(pTVDispInfo);
			break;
		}
	}

	// Set the m_meansToEditLabel back to EL_NONE to prepare for another editing:
	m_meansToEditLabel = EL_NONE;
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleEndLabelEditKeyCheck
// This method will be called to handle the ending of label editing when the
// key's checkbox was clicked on to instigate the label editing.
//
// Input:
//  LPNMTVDISPINFO pTVDispInfo -- A pointer to a NMTVDISPINFO object containing
//								  data about the item being edited.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleEndLabelEditKeyCheck(LPNMTVDISPINFO pTVDispInfo)
{
	if (pTVDispInfo != NULL)
	{
		TV_ITEM tvItem = pTVDispInfo->item;
		short nKeyIndex = (*(KeyValIndices*)&(tvItem.lParam)).m_nKeyIndex;
		if (tvItem.pszText != NULL)
		{
			// The editing of the label was accepted by the user:
			// Update the text and state image for the item in the treeview:
			TV_ITEM tvNewItem;
			tvNewItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
			tvNewItem.hItem = tvItem.hItem;
			tvNewItem.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
			tvNewItem.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
			tvNewItem.pszText = tvItem.pszText;
			TreeView_SetItem(m_hwndTreeCtrl, &tvNewItem);

			// Update the new label name and checked value for this item in the keeper:
			m_keyKeeper[nKeyIndex].m_strNewKeyname = (LPTSTR) (tvItem.pszText);
			m_keyKeeper[nKeyIndex].m_bChecked = true;
		}
		else
		{
			// The editing of the label was canceled by the user:
			if (m_keyKeeper[nKeyIndex].m_strNewKeyname == "")
			{
				// There is no new name, so go back to original name, make it
				// nonbold and update the state image to nonchecked:
				TV_ITEM tvNewItem;
				tvNewItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
				tvNewItem.hItem = tvItem.hItem;
				tvNewItem.state = INDEXTOSTATEIMAGEMASK(1);
				tvNewItem.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
				tvNewItem.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_strKeyname);
				TreeView_SetItem(m_hwndTreeCtrl, &tvNewItem);

				// Update the checked value for this item in the keeper:
				m_keyKeeper[nKeyIndex].m_bChecked = false;
			}
			else
			{
				// There is a new name to go back to, so use it for the label,
				// make it bold and update state image to checked:
				TV_ITEM tvNewItem;
				tvNewItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
				tvNewItem.hItem = tvItem.hItem;
				tvNewItem.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
				tvNewItem.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
				tvNewItem.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_strNewKeyname);
				TreeView_SetItem(m_hwndTreeCtrl, &tvNewItem);

				// Update the checked value for this item in the keeper:
				m_keyKeeper[nKeyIndex].m_bChecked = true;
			}
		}
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleEndLabelEditValCheck
// This method will be called to handle the ending of label editing when the
// val's checkbox was clicked on to instigate the label editing.
//
// Input:
//  LPNMTVDISPINFO pTVDispInfo -- A pointer to a NMTVDISPINFO object containing
//								  data about the item being edited.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleEndLabelEditValCheck(LPNMTVDISPINFO pTVDispInfo)
{
	if (pTVDispInfo != NULL)
	{
		TV_ITEM tvItem = pTVDispInfo->item;
		short nKeyIndex = (*(KeyValIndices*)&(tvItem.lParam)).m_nKeyIndex;
		short nValIndex = (*(KeyValIndices*)&(tvItem.lParam)).m_nValIndex;
		if (tvItem.pszText != NULL)
		{
			// The editing of the label was accepted by the user:
			// Update the text and state image for the item in the treeview:
			TV_ITEM tvNewItem;
			tvNewItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
			tvNewItem.hItem = tvItem.hItem;
			tvNewItem.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
			tvNewItem.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
			tvNewItem.pszText = tvItem.pszText;
			TreeView_SetItem(m_hwndTreeCtrl, &tvNewItem);

			// Update the new label name and checked value for this item in the keeper:
			m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal = (LPTSTR) (tvItem.pszText);
			m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = true;
		}
		else
		{
			// The editing of the label was canceled by the user:
			if (m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal == "")
			{
				// There is no new name, so go back to original name, make it
				// nonbold and update the state image to nonchecked:
				TV_ITEM tvNewItem;
				tvNewItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
				tvNewItem.hItem = tvItem.hItem;
				tvNewItem.state = INDEXTOSTATEIMAGEMASK(1);
				tvNewItem.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
				tvNewItem.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal);
				TreeView_SetItem(m_hwndTreeCtrl, &tvNewItem);

				// Update the checked value for this item in the keeper:
				m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = false;

				// Now change the normal image for the parent key accordingly:
				bool bChildValChecked = false;
				int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
				for (int nIndex = 0; nIndex < nNumVals; nIndex++)
				{
					if (m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_bChecked == true)
					{
						bChildValChecked = true;
						break;
					}
				}
				TV_ITEM tvKey;
				tvKey.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE;
				tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
				if (bChildValChecked == true)
				{
					tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
					tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;
				}
				else
				{
					tvKey.iImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
					tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
				}
				int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
				tvKey.iImage += imageOffset;
				tvKey.iSelectedImage += imageOffset;

				TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
			}
			else
			{
				// There is a new name to go back to, so use it for the label,
				// make it bold and update state image to checked:
				TV_ITEM tvNewItem;
				tvNewItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
				tvNewItem.hItem = tvItem.hItem;
				tvNewItem.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
				tvNewItem.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
				tvNewItem.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal);
				TreeView_SetItem(m_hwndTreeCtrl, &tvNewItem);

				// Update the checked value for this item in the keeper:
				m_keyKeeper[nKeyIndex].m_bChecked = true;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// KeyValPanelController::HandleCheckBoxClick
// This method is called whenever the user clicks on an item's checkbox in the
// keyval panel's treeview.  It will handle detecting which item was clicked
// on and setting the checked state for that item.  It will also change the
// key's image icon if necessary.
//
// Input:
//  HTREEITEM hItemClicked -- The handle to the item clicked on.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleCheckBoxClick(HTREEITEM hItemClicked)
{
	// Set up a TV_ITEM to temporarily store info in:
	TV_ITEM itemClicked;
	memset(&itemClicked, 0, sizeof(itemClicked));
	itemClicked.hItem = hItemClicked;
	itemClicked.mask = TVIF_PARAM;
	TreeView_GetItem(m_hwndTreeCtrl, &itemClicked);

	// Pull out the state and indent level of the item.  Note:  The state
	// returned here will actually be the opposite of what the user sees, since
	// the state actually hasn't changed by the time this code is reached, so
	// I just use the opposite of what is returned:
	UINT nCheckState = TreeView_GetCheckState(m_hwndTreeCtrl, hItemClicked);
	short nKeyIndex = (*(KeyValIndices*)&(itemClicked.lParam)).m_nKeyIndex;
	short nValIndex = (*(KeyValIndices*)&(itemClicked.lParam)).m_nValIndex;

	// Based on the key and val indices, and the current mode, call the
	// appropriate method to handle the check:
	if (nValIndex == -1)
	{
		// A key was clicked:
		Mode currMode = GetMode();
		if (currMode == SELECT)
			HandleKeyCheckSelect(nKeyIndex, nCheckState);
		else if (currMode == PURGE)
			HandleKeyCheckPurge(nKeyIndex, nCheckState);
		else if (currMode == REMAP)
			HandleKeyCheckRemap(nKeyIndex, nCheckState);
	}
	else
	{
		// A val was clicked:
		Mode currMode = GetMode();
		if (currMode == SELECT)
			HandleValCheckSelect(nKeyIndex, nValIndex, nCheckState);
		else if (currMode == PURGE)
			HandleValCheckPurge(nKeyIndex, nValIndex, nCheckState);
		else if (currMode == REMAP)
			HandleValCheckRemap(nKeyIndex, nValIndex, nCheckState);
		else if (currMode == APPLY)
			HandleValCheckApply(nKeyIndex, nValIndex, nCheckState);
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleKeyCheckSelect
// This method is called when the user has clicked in the checkbox for a key
// item in the treeview.  It will handle changing any necessary data in the
// keykeeper as well as changing any necessary state or normal images in the
// treeview.  It is only called if the current mode is SELECT.
//
// Input:
//  short nKeyIndex -- The index of the key item in the keykeeper.
//  UINT nCheckState -- The check state of the item in the tree.  NOTE: this
//						will actually be the check state of the item before the
//						user clicked on the checkbox, not after.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleKeyCheckSelect(short nKeyIndex, UINT nCheckState)
{
	if (nCheckState == 0)
	{
		// Item is being checked:
		// Set the key's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_bChecked = true;

		// Change the state images for the key entry in the treeview accordingly:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(2);
		tvKey.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
	}
	else
	{
		// Item is being unchecked:
		// Set the key's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_bChecked = false;

		// Change the state and normal images for the key entry in the treeview
		// accordingly:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(1);
		tvKey.stateMask = TVIS_STATEIMAGEMASK;
		if (m_keyKeeper[nKeyIndex].m_bExpanded == true)
		{
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
		}
		else
		{
			tvKey.iImage = IMG_INDEX_FOLD_SHUT_NOCHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_SHUT_NOCHECK;
		}

		int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;

		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);

		// Make sure all the vals for this key are unchecked:
		int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
		for (int nValIndex = 0; nValIndex < nNumVals; nValIndex++)
		{
			// Set the checked state for this val in the keykeeper:
			m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = false;

			// Now change the state image for this val in the treeview to unchecked:
			TV_ITEM tvVal;
			tvVal.mask = TVIF_HANDLE | TVIF_STATE;
			tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
			tvVal.state = INDEXTOSTATEIMAGEMASK(1);
			tvVal.stateMask = TVIS_STATEIMAGEMASK;
			TreeView_SetItem(m_hwndTreeCtrl, &tvVal);
		}
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleKeyCheckPurge
// This method is called when the user has clicked in the checkbox for a key
// item in the treeview.  It will handle changing any necessary data in the
// keykeeper as well as changing any necessary state or normal images in the
// treeview.  It is only called if the current mode is PURGE.
//
// Input:
//  short nKeyIndex -- The index of the key item in the keykeeper.
//  UINT nCheckState -- The check state of the item in the tree.  NOTE: this
//						will actually be the check state of the item before the
//						user clicked on the checkbox, not after.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleKeyCheckPurge(short nKeyIndex, UINT nCheckState)
{
	if (nCheckState == 0)
	{
		// Item is being checked:
		// Set the key's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_bChecked = true;

		// Make sure all the vals for this key are checked:
		int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
		for (int nIndex = 0; nIndex < nNumVals; nIndex++)
		{
			// Set the checked state for this val in the keykeeper:
			m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_bChecked = true;

			// Now change the state image for this val in the treeview to checked:
			TV_ITEM tvVal;
			tvVal.mask = TVIF_HANDLE | TVIF_STATE;
			tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_hTreeItem;
			tvVal.state = INDEXTOSTATEIMAGEMASK(2);
			tvVal.stateMask = TVIS_STATEIMAGEMASK;
			TreeView_SetItem(m_hwndTreeCtrl, &tvVal);
		}

		// Change the state and normal images for the key entry in the treeview
		// accordingly:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(2);
		tvKey.stateMask = TVIS_STATEIMAGEMASK;
		if (m_keyKeeper[nKeyIndex].m_bExpanded == true)
		{
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;
		}
		else
		{
			tvKey.iImage = IMG_INDEX_FOLD_SHUT_CHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_SHUT_CHECK;
		}

		int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;
		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
	}
	else
	{
		// Item is being unchecked:
		// Set the key's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_bChecked = false;

		// Change the state images for the key entry in the treeview accordingly:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(1);
		tvKey.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleKeyCheckRemap
// This method is called when the user has clicked in the checkbox for a key
// item in the treeview.  It will handle changing any necessary data in the
// keykeeper as well as changing any necessary state or normal images in the
// treeview.  It is only called if the current mode is REMAP.
//
// Input:
//  short nKeyIndex -- The index of the key item in the keykeeper.
//  UINT nCheckState -- The check state of the item in the tree.  NOTE: this
//						will actually be the check state of the item before the
//						user clicked on the checkbox, not after.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleKeyCheckRemap(short nKeyIndex, UINT nCheckState)
{
	if (nCheckState == 0)
	{
		// Item is being checked:
		// Set the key's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_bChecked = true;

		// Change the state image and text for the key entry in the treeview accordingly:
		if (m_keyKeeper[nKeyIndex].m_strNewKeyname == "")
		{
			// No new keyname to use:
			TV_ITEM tvKey;
			tvKey.mask = TVIF_HANDLE | TVIF_STATE;
			tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
			tvKey.state = INDEXTOSTATEIMAGEMASK(2);
			tvKey.stateMask = TVIS_STATEIMAGEMASK;
			TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
		}
		else
		{
			// There is a new keyname to use:
			TV_ITEM tvKey;
			tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
			tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
			tvKey.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
			tvKey.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
			tvKey.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_strNewKeyname);
			TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
		}

		// Send a message to the treeview to start label editing:
		m_meansToEditLabel = EL_KEYCHECK;
		PostMessage(m_hwndTreeCtrl, TVM_EDITLABEL, 0, (LPARAM) m_keyKeeper[nKeyIndex].m_hTreeItem);
	}
	else
	{
		// Item is being unchecked:
		// Set the key's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_bChecked = false;

		// Change the state image for the key entry in the treeview accordingly,
		// and return the text to its original with no bolding:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(1);
		tvKey.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
		tvKey.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_strKeyname);
		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleValCheckSelect
// This method is called when the user has clicked in the checkbox for a val
// item in the treeview.  It will handle changing any necessary data in the
// keykeeper as well as changing any necessary state or normal images in the
// treeview.  It is only called if the current mode is SELECT.
//
// Input:
//  short nKeyIndex -- The index of the key item in the keykeeper.
//	short nValIndex -- The index of the val item in the keykeeper.
//  UINT nCheckState -- The check state of the item in the tree.  NOTE: this
//						will actually be the check state of the item before the
//						user clicked on the checkbox, not after.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleValCheckSelect(short nKeyIndex, short nValIndex, UINT nCheckState)
{
	if (nCheckState == 0)
	{
		// Item is being checked:
		// Set the val's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = true;

		// Change the state image for the val entry in the treeview accordingly:
		TV_ITEM tvVal;
		tvVal.mask = TVIF_HANDLE | TVIF_STATE;
		tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
		tvVal.state = INDEXTOSTATEIMAGEMASK(2);
		tvVal.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvVal);

		// Set the parent key's state image to be checked, and the normal image
		// to the checked folder:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(2);
		tvKey.stateMask = TVIS_STATEIMAGEMASK;
		tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
		tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;

		int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;

		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);

		// And set the parent key's entry in the keykeeper to be checked:
		m_keyKeeper[nKeyIndex].m_bChecked = true;
	}
	else
	{
		// Item is being unchecked:
		// Set the val's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = false;

		// Change the state image for the val entry in the treeview accordingly:
		TV_ITEM tvVal;
		tvVal.mask = TVIF_HANDLE | TVIF_STATE;
		tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
		tvVal.state = INDEXTOSTATEIMAGEMASK(1);
		tvVal.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvVal);

		// Now change the normal image for the parent key accordingly:
		bool bChildValChecked = false;
		int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
		for (int nIndex = 0; nIndex < nNumVals; nIndex++)
		{
			if (m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_bChecked == true)
			{
				bChildValChecked = true;
				break;
			}
		}
		TV_ITEM tvKey;
		tvKey.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		if (bChildValChecked == true)
		{
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;
		}
		else
		{
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
		}

		int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;

		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleValCheckPurge
// This method is called when the user has clicked in the checkbox for a val
// item in the treeview.  It will handle changing any necessary data in the
// keykeeper as well as changing any necessary state or normal images in the
// treeview.  It is only called if the current mode is PURGE.
//
// Input:
//  short nKeyIndex -- The index of the key item in the keykeeper.
//	short nValIndex -- The index of the val item in the keykeeper.
//  UINT nCheckState -- The check state of the item in the tree.  NOTE: this
//						will actually be the check state of the item before the
//						user clicked on the checkbox, not after.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleValCheckPurge(short nKeyIndex, short nValIndex, UINT nCheckState)
{
	if (nCheckState == 0)
	{
		// Item is being checked:
		// Set the val's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = true;

		// Change the state image for the val entry in the treeview accordingly:
		TV_ITEM tvVal;
		tvVal.mask = TVIF_HANDLE | TVIF_STATE;
		tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
		tvVal.state = INDEXTOSTATEIMAGEMASK(2);
		tvVal.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvVal);

		// If this val and all of its siblings are checked then check
		// the parent key:
		bool bAllValsChecked = true;
		int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
		for (int nIndex = 0; nIndex < nNumVals; nIndex++)
		{
			if (m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_bChecked != true)
			{
				bAllValsChecked = false;
				break;
			}
		}
		if (bAllValsChecked == true)
		{
			// Set the parent key's state image and normal image to be checked:
			TV_ITEM tvKey;
			tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
			tvKey.state = INDEXTOSTATEIMAGEMASK(2);
			tvKey.stateMask = TVIS_STATEIMAGEMASK;
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;

			int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
			tvKey.iImage += imageOffset;
			tvKey.iSelectedImage += imageOffset;

			TreeView_SetItem(m_hwndTreeCtrl, &tvKey);

			// And set the parent key's entry in the keykeeper to be checked:
			m_keyKeeper[nKeyIndex].m_bChecked = true;
		}
		else
		{
			// Just set the parent key's normal image to be checked:
			TV_ITEM tvKey;
			tvKey.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;

			int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
			tvKey.iImage += imageOffset;
			tvKey.iSelectedImage += imageOffset;

			TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
		}
	}
	else
	{
		// Item is being unchecked:
		// Set the val's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = false;

		// Change the state image for the val entry in the treeview accordingly:
		TV_ITEM tvVal;
		tvVal.mask = TVIF_HANDLE | TVIF_STATE;
		tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
		tvVal.state = INDEXTOSTATEIMAGEMASK(1);
		tvVal.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvVal);

		// Now change the normal image for the parent key accordingly:
		bool bChildValChecked = false;
		int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
		for (int nIndex = 0; nIndex < nNumVals; nIndex++)
		{
			if (m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_bChecked == true)
			{
				bChildValChecked = true;
				break;
			}
		}
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.state = INDEXTOSTATEIMAGEMASK(1);
		tvKey.stateMask = TVIS_STATEIMAGEMASK;
		if (bChildValChecked == true)
		{
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;
		}
		else
		{
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
		}

		int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;

		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);

		// And set the parent key's entry in the keykeeper to be unchecked:
		m_keyKeeper[nKeyIndex].m_bChecked = false;
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleValCheckApply
// This method is called when the user has clicked in the checkbox for a val
// item in the treeview.  It will handle changing any necessary data in the
// keykeeper as well as changing any necessary state or normal images in the
// treeview.  It is only called if the current mode is APPLY.
//
// Input:
//  short nKeyIndex -- The index of the key item in the keykeeper.
//	short nValIndex -- The index of the val item in the keykeeper.
//  UINT nCheckState -- The check state of the item in the tree.  NOTE: this
//						will actually be the check state of the item before the
//						user clicked on the checkbox, not after.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleValCheckApply(short nKeyIndex, short nValIndex, UINT nCheckState)
{
	if (nCheckState == 0)
	{
		// Item is being checked:
		// Set the val's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = true;

		// Change the state image for the val entry in the treeview accordingly:
		TV_ITEM tvVal;
		tvVal.mask = TVIF_HANDLE | TVIF_STATE;
		tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
		tvVal.state = INDEXTOSTATEIMAGEMASK(2);
		tvVal.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvVal);

		if (!m_pKeyValCommunicatorBase->IsCheckListKey(m_keyKeeper[nKeyIndex].m_strKeyname))
		{
			// Make sure no other vals are checked for the key:
			int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
			for (int nIndex = 0; nIndex < nNumVals; nIndex++)
			{
				if (nIndex != nValIndex)
				{
					if (m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_bChecked == true)
					{
						// Set the val's state image to be uchecked:
						TV_ITEM tvCurrVal;
						tvCurrVal.mask = TVIF_HANDLE | TVIF_STATE;
						tvCurrVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_hTreeItem;
						tvCurrVal.state = INDEXTOSTATEIMAGEMASK(1);
						tvCurrVal.stateMask = TVIS_STATEIMAGEMASK;
						TreeView_SetItem(m_hwndTreeCtrl, &tvCurrVal);

						// And set the val's entry in the keykeeper to be unchecked:
						m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_bChecked = false;
					}
				}
			}
		}

		// Now change the normal image for the parent key accordingly:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
		tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;

		int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;

		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
	}
	else
	{
		// Item is being unchecked:
		// Set the val's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = false;

		// Change the state image for the val entry in the treeview accordingly:
		TV_ITEM tvVal;
		tvVal.mask = TVIF_HANDLE | TVIF_STATE;
		tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
		tvVal.state = INDEXTOSTATEIMAGEMASK(1);
		tvVal.stateMask = TVIS_STATEIMAGEMASK;
		TreeView_SetItem(m_hwndTreeCtrl, &tvVal);

		// Now change the normal image for the parent key accordingly:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.iImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
		tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_NOCHECK;

		int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;

		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleValCheckRemap
// This method is called when the user has clicked in the checkbox for a val
// item in the treeview.  It will handle changing any necessary data in the
// keykeeper as well as changing any necessary state or normal images in the
// treeview.  It is only called if the current mode is REMAP.
//
// Input:
//  short nKeyIndex -- The index of the key item in the keykeeper.
//	short nValIndex -- The index of the val item in the keykeeper.
//  UINT nCheckState -- The check state of the item in the tree.  NOTE: this
//						will actually be the check state of the item before the
//						user clicked on the checkbox, not after.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleValCheckRemap(short nKeyIndex, short nValIndex, UINT nCheckState)
{
	if (nCheckState == 0)
	{
		// Item is being checked:
		// Set the val's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = true;

		// Change the state image and text for the val entry in the treeview accordingly:
		if (m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal == "")
		{
			// No new valname to use:
			TV_ITEM tvVal;
			tvVal.mask = TVIF_HANDLE | TVIF_STATE;
			tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
			tvVal.state = INDEXTOSTATEIMAGEMASK(2);
			tvVal.stateMask = TVIS_STATEIMAGEMASK;
			TreeView_SetItem(m_hwndTreeCtrl, &tvVal);
		}
		else
		{
			// There is a new valname to use:
			TV_ITEM tvVal;
			tvVal.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
			tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
			tvVal.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_BOLD;
			tvVal.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
			tvVal.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strNewVal);
			TreeView_SetItem(m_hwndTreeCtrl, &tvVal);
		}

		// Change the normal image for the parent key accordingly:
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
		tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;

		int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;

		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);

		// Send a message to the treeview to start label editing:
		m_meansToEditLabel = EL_VALCHECK;
		PostMessage(m_hwndTreeCtrl, TVM_EDITLABEL, 0, (LPARAM) m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem);
	}
	else
	{
		// Item is being unchecked:
		// Set the val's entry in the keykeeper to the correct checked state:
		m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked = false;

		// Change the state image for the val entry in the treeview accordingly,
		// and return the text to its original with no bolding:
		TV_ITEM tvVal;
		tvVal.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
		tvVal.hItem = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_hTreeItem;
		tvVal.state = INDEXTOSTATEIMAGEMASK(1);
		tvVal.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD;
		tvVal.pszText = (LPTSTR) ((const char *)m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal);
		TreeView_SetItem(m_hwndTreeCtrl, &tvVal);

		// Now change the normal image for the parent key accordingly:
		bool bChildValChecked = false;
		int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
		for (int nIndex = 0; nIndex < nNumVals; nIndex++)
		{
			if (m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_bChecked == true)
			{
				bChildValChecked = true;
				break;
			}
		}
		TV_ITEM tvKey;
		tvKey.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvKey.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
		if (bChildValChecked == true)
		{
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;
		}
		else
		{
			tvKey.iImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
			tvKey.iSelectedImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
		}

		int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
		tvKey.iImage += imageOffset;
		tvKey.iSelectedImage += imageOffset;

		TreeView_SetItem(m_hwndTreeCtrl, &tvKey);
	}
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleItemExpanded
// This method will be called whenever an item is expanded or collapsed in the
// keyval panel's treeview.  It will handle the changing of the item's image
// accordingly.
//
// Input:
//  LPARAM lParam -- The lparam containing the item that expanded or collapsed.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleItemExpanded(LPARAM lParam)
{
	// Check to see if the item is expanding or collapsing:
	bool bItemExpanding = false;
	LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lParam;
	if (pnmtv->action == TVE_EXPAND)
		bItemExpanding = true;

	// Since only keys can expand or collapse, we know we are dealing only with
	// a key, so set the correct image for the item that corresponds to this key:
	TV_ITEM itemClicked;
	memset(&itemClicked, 0, sizeof(itemClicked));
	itemClicked.hItem = pnmtv->itemNew.hItem;
	itemClicked.mask = TVIF_PARAM;
	TreeView_GetItem(m_hwndTreeCtrl, &itemClicked);
	short nKeyIndex = (*(KeyValIndices*)&(itemClicked.lParam)).m_nKeyIndex;

	bool bChildValChecked = false;
	int nNumVals = m_keyKeeper[nKeyIndex].m_valKeeper.GetSize();
	for (int nIndex = 0; nIndex < nNumVals; nIndex++)
	{
		if (m_keyKeeper[nKeyIndex].m_valKeeper[nIndex].m_bChecked == true)
		{
			bChildValChecked = true;
			break;
		}
	}
	TV_ITEM tv;
	tv.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE;
	tv.hItem = m_keyKeeper[nKeyIndex].m_hTreeItem;
	if (bChildValChecked == true)
	{
		if (bItemExpanding == true)
		{
			tv.iImage = IMG_INDEX_FOLD_OPEN_CHECK;
			tv.iSelectedImage = IMG_INDEX_FOLD_OPEN_CHECK;
		}
		else
		{
			tv.iImage = IMG_INDEX_FOLD_SHUT_CHECK;
			tv.iSelectedImage = IMG_INDEX_FOLD_SHUT_CHECK;
		}
	}
	else
	{
		if (bItemExpanding == true)
		{
			tv.iImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
			tv.iSelectedImage = IMG_INDEX_FOLD_OPEN_NOCHECK;
		}
		else
		{
			tv.iImage = IMG_INDEX_FOLD_SHUT_NOCHECK;
			tv.iSelectedImage = IMG_INDEX_FOLD_SHUT_NOCHECK;
		}
	}

	int imageOffset = GetImageOffset(m_keyKeeper[nKeyIndex]);
	tv.iImage += imageOffset;
	tv.iSelectedImage += imageOffset;

	TreeView_SetItem(m_hwndTreeCtrl, &tv);

	// Set the entry's bExpanded flag in the keeper to the correct value:
	m_keyKeeper[nKeyIndex].m_bExpanded = bItemExpanding;
}

//-----------------------------------------------------------------------------
// KeyValPanelController::HandleFindButton
// This method will be called when the user clicks on the find button.
// The find dialog is put up and the user is able to enter data to find
// and select object based on keys and/or values
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleFindButton()
{
	TupString defaultKey = "";
	TupString defaultValue = "";

	HTREEITEM hSelectedItem = TreeView_GetSelection(m_hwndTreeCtrl);
	if (hSelectedItem != NULL)
	{
		TV_ITEM itemSelected;
		memset(&itemSelected, 0, sizeof(itemSelected));
		itemSelected.hItem = hSelectedItem;
		itemSelected.mask = TVIF_PARAM;
		TreeView_GetItem(m_hwndTreeCtrl, &itemSelected);

		short nKeyIndex = (*(KeyValIndices*)&(itemSelected.lParam)).m_nKeyIndex;
		short nValIndex = (*(KeyValIndices*)&(itemSelected.lParam)).m_nValIndex;

		defaultKey = m_keyKeeper[nKeyIndex].m_strKeyname;

		if (nValIndex != -1)
		{
			defaultValue = m_keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal;
		}
	}
	ShowFindDialog(defaultKey, defaultValue);
}

//-----------------------------------------------------------------------------
// KeyValPanelController::HandleFindButton
// This method will be called when the user clicks on the find button.
// The find dialog is put up and the user is able to enter data to find
// and select object based on keys and/or values
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::ShowFindDialog(const char *pKeyName, const char *pValueName)
{
	if (m_pFindDialog == NULL)
	{
		m_pFindDialog = new KeyValFindDialog(m_hInstance,m_hWnd,this);
	}
	m_pFindDialog->SetFindKey(pKeyName);
	m_pFindDialog->SetFindValue(pValueName);
	m_pFindDialog->Show();
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleShowActiveButton
// This method will be called when the user clicks on the show active button.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleShowActiveButton(void)
{
	m_bShowActive = !m_bShowActive;
	::SendMessage(m_hShowActiveButton, BM_SETSTATE, (BOOL)m_bShowActive, 0);
	UpdateTreeViewAndKeeper();
}

//-----------------------------------------------------------------------------
// KeyValPanelController::HandleShowDefaultButton
// This method will be called when the user clicks on the show default button.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleShowDefaultButton(void)
{
	m_bShowDefault = !m_bShowDefault;
	::SendMessage(m_hShowDefaultButton, BM_SETSTATE, (BOOL)m_bShowDefault, 0);
	UpdateTreeViewAndKeeper();
}

//-----------------------------------------------------------------------------
// KeyValPanelController::HandleShowUnusedButton
// This method will be called when the user clicks on the show unused button.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleShowUnusedButton(void)
{
	m_bShowUnused = !m_bShowUnused;
	::SendMessage(m_hShowUnusedButton, BM_SETSTATE, (BOOL)m_bShowUnused, 0);
	UpdateTreeViewAndKeeper();
}

//-----------------------------------------------------------------------------
// KeyValPanelController::HandleFilterButton
// This method will be called when the user clicks on the filter button.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleFilterButton()
{
	if (m_filterKeyKeeper.GetSize())
	{
		::SendMessage(m_hFilterButton, BM_SETSTATE, FALSE, 0);
		m_filterKeyKeeper.RemoveAll();
	}
	else
	{
		m_filterKeyKeeper.RemoveAll();
		for (int sourceKeeperIndex = 0; sourceKeeperIndex < m_keyKeeper.GetSize(); sourceKeeperIndex++)
		{
			const KeyInfo &sourceKeyInfo = m_keyKeeper[sourceKeeperIndex];
			if (!sourceKeyInfo.m_bVisible)
				continue;

			if (sourceKeyInfo.m_bChecked)
			{
				int filterIndex = m_filterKeyKeeper.Add(sourceKeyInfo);
				m_filterKeyKeeper[filterIndex].m_valKeeper.RemoveAll(); // don't need values
			}
			else
			{
				KeyInfo tempKeyInfo = sourceKeyInfo;
				tempKeyInfo.m_valKeeper.RemoveAll(); // don't need values
				bool bFoundCheck = false;
				for (int sourceValueIndex = 0; sourceValueIndex < sourceKeyInfo.m_valKeeper.GetSize(); sourceValueIndex++)
				{
					if (!sourceKeyInfo.m_valKeeper[sourceValueIndex].m_bVisible)
						continue;

					// see if any values are checked
					if (sourceKeyInfo.m_valKeeper[sourceValueIndex].m_bChecked)
					{
						bFoundCheck = true;
						break;
					}
				}
				if (bFoundCheck) // if any values are check then add the key
				{
					m_filterKeyKeeper.Add(tempKeyInfo);
				}
			}
		}
		if (m_filterKeyKeeper.GetSize())
		{
			::SendMessage(m_hFilterButton, BM_SETSTATE, TRUE, 0);
		}
		else
		{
			::SendMessage(m_hFilterButton, BM_SETSTATE, FALSE, 0);
		}
	}
	UpdateTreeViewAndKeeper();
}


//-----------------------------------------------------------------------------
// KeyValPanelController::HandleApplyButtons
// This method will be called when the user clicks on either of the Apply
// buttons in the key/val panel.  It will handle detecting what mode the user
// is in and calling the appropriate methods.
//
// Input:
//  bool bApplyToAll -- Pass in false if you want only the selected items to
//						be acted on.  True if you want all the items to be
//						acted on (for example, all the objects in the scene
//						versus only the selected objects).
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValPanelController::HandleApplyButtons(bool bApplyToAll)
{
	switch (m_mode)
	{
		case NONE:
		{
			m_pKeyValCommunicatorBase->UpdateStatsText(TupString(""));
			break;
		}
		case SELECT:
		{
			int totalAffected = 0;
			if (bApplyToAll == true)
			{
				// Pass the keykeeper back to the keyvalcommunicator and tell
				// it to handle the selecting of items within all the items:
				totalAffected = m_pKeyValCommunicatorBase->HandleApplySelect(m_keyKeeper, true);
			}
			else
			{
				// Pass the keykeeper back to the keyvalcommunicator and tell
				// it to handle the selecting of items within the selection set:
				totalAffected = m_pKeyValCommunicatorBase->HandleApplySelect(m_keyKeeper);
			}
			TupString stats;
			stats.Format("%d Selected",totalAffected);
			m_pKeyValCommunicatorBase->UpdateStatsText(stats);
			break;
		}
		case APPLY:
		{
			int totalAffected = 0;
			if (bApplyToAll == true)
			{
				// Pass the keykeeper back to the keyvalcommunicator and tell
				// it to handle the applying of vals within all the items:
				totalAffected = m_pKeyValCommunicatorBase->HandleApplyApply(m_keyKeeper, true);
			}
			else
			{
				// Pass the keykeeper back to the keyvalcommunicator and tell
				// it to handle the applying of vals within all the items:
				totalAffected = m_pKeyValCommunicatorBase->HandleApplyApply(m_keyKeeper);
			}
			TupString stats;
			stats.Format("%d Changed",totalAffected);
			m_pKeyValCommunicatorBase->UpdateStatsText(stats);
			break;
		}
		case PURGE:
		{
			int totalAffected = 0;
			if (bApplyToAll == true)
			{
				// Pass the keykeeper back to the keyvalcommunicator and tell
				// it to handle the purging of keys and vals within all the items:
				totalAffected = m_pKeyValCommunicatorBase->HandleApplyPurge(m_keyKeeper, true);
			}
			else
			{
				// Pass the keykeeper back to the keyvalcommunicator and tell
				// it to handle the purging of keys and vals within the selection set:
				totalAffected = m_pKeyValCommunicatorBase->HandleApplyPurge(m_keyKeeper);
			}
			TupString stats;
			stats.Format("%d Changed",totalAffected);
			m_pKeyValCommunicatorBase->UpdateStatsText(stats);
			break;
		}
		case REMAP:
		{
			int totalAffected = 0;
			if (bApplyToAll == true)
			{
				// Pass the keykeeper back to the keyvalcommunicator and tell
				// it to handle the remapping of keys and vals within all the items:
				totalAffected = m_pKeyValCommunicatorBase->HandleApplyRemap(m_keyKeeper, true);
			}
			else
			{
				// Pass the keykeeper back to the keyvalcommunicator and tell
				// it to handle the remapping of keys and vals within all the items:
				totalAffected = m_pKeyValCommunicatorBase->HandleApplyRemap(m_keyKeeper);
			}
			g_pTheKeyValPanelController->UncheckAllKeysAndVals();
			TupString stats;
			stats.Format("%d Changed",totalAffected);
			m_pKeyValCommunicatorBase->UpdateStatsText(stats);
			break;
		}
	}
}

int KeyValPanelController::HandleFind(const KeyValueFind &keyValueFind, bool bCurrentSelection, bool bAppendSelection)
{
	return m_pKeyValCommunicatorBase->HandleFind(keyValueFind,bCurrentSelection,bAppendSelection);
}

void KeyValPanelController::SetRenameStartCallback(void (CALLBACK *renameStartCallback)(void))
{
	m_pRenameStartCallback = renameStartCallback;
}

void KeyValPanelController::SetRenameEndCallback(void (CALLBACK *renameEndCallback)(void))
{
	m_pRenameEndCallback = renameEndCallback;
}


BOOL KeyValPanelController::OnSetCursor(UINT nHitTest,UINT message)
{
	if (NULL != m_hToolTip)
	{
		MSG msg;
		memset(&msg,0,sizeof(MSG));
		POINT pt;
		::GetCursorPos(&pt);   
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = 0;
		msg.lParam = MAKELONG(pt.x, pt.y);//logical unit
		msg.time = ::GetTickCount();
		msg.pt = pt;         
		switch(message)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
		case WM_NCMBUTTONDBLCLK:
		case WM_MOUSEMOVE:
			{
				ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(m_hToolTip);
				if (pToolTip)
				{
					pToolTip->RelayEvent(&msg);
					ICustomToolTip::ReleaseIToolTip(pToolTip);
				}
			}
			break;
		}
	}
	return FALSE;
}
