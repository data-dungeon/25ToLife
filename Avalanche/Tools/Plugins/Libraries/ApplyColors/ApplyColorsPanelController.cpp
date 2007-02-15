//-----------------------------------------------------------------------------
//	File:		ApplyColorsPanelController.cpp
//	Created:	November 17th, 2004
//				Copyright (c) 2002-2004, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The ApplyColorsPanelController class controlls all functionality
//			regarding the apply colors panel
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes:

#include "ApplyColorsPCH.h"
#include "ApplyColorsPanelController.h"
#include "ApplyColorsCommunicatorBase.h"
#include "ColorSetManager.h"
//-----------------------------------------------------------------------------
// Defines, etc.:

//-----------------------------------------------------------------------------
// Global objects and/or functions:

ApplyColorsPanelController* g_pTheApplyColorsPanelController = NULL;

void ApplyColorsPanelController::SetHWnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

void ApplyColorsPanelController::OnInitDialog(void)
{
	m_hApplyButton = GetDlgItem(m_hWnd, IDC_BUTTON_APPLYCOLORS);
	m_hRemoveColorButton = GetDlgItem(m_hWnd, IDC_BUTTON_REMOVECOLORS);
	m_hComboBox = GetDlgItem(m_hWnd, IDC_COMBO_COLORSETS);
}


void ApplyColorsPanelController::OnSize(HWND hWnd,int cx,int cy)
{
	HWND hwndTreeCtrl = GetDlgItem(hWnd, IDC_TREE_APPLYCOLORS);
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
	HWND hwndComboBox = GetDlgItem(hWnd, IDC_COMBO_COLORSETS);
	if (hwndComboBox!=NULL)
	{
		RECT rect;
		GetClientRect(hWnd,&rect);
		RECT myRect;
		GetWindowRect(hwndComboBox,&myRect);
		ScreenToClient(hWnd, (LPPOINT)&myRect);
		ScreenToClient(hWnd, ((LPPOINT)&myRect)+1);
		MoveWindow(hwndComboBox,69,5,(rect.right-rect.left)-69,myRect.bottom-myRect.top,FALSE);
	}
}

//-----------------------------------------------------------------------------
// ApplyColorsPanelProc
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
static BOOL CALLBACK ApplyColorsPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			g_pTheApplyColorsPanelController->SetHWnd(hWnd);
			g_pTheApplyColorsPanelController->OnInitDialog();
			return TRUE;
		}
		case WM_SIZE:
		{
			g_pTheApplyColorsPanelController->OnSize(hWnd,LOWORD(lParam),HIWORD(lParam));
			return TRUE;
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam)==BN_CLICKED)
			{
				switch(LOWORD(wParam))
				{
					case IDC_BUTTON_APPLYCOLORS:
					{
						g_pTheApplyColorsPanelController->HandleApplyButton();
						return TRUE;
					}
					case IDC_BUTTON_REMOVECOLORS:
					{
						g_pTheApplyColorsPanelController->HandleRemoveColorButton();
						return TRUE;
					}
				}
			}
			else if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				if (LOWORD(wParam)==IDC_COMBO_COLORSETS)
				{
					g_pTheApplyColorsPanelController->HandleColorSetChange();
				}
			}
		}
	}
	return FALSE;
}



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ApplyColorsPanelController::ApplyColorsPanelController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ApplyColorsPanelController::ApplyColorsPanelController(ApplyColorsCommunicatorBase* pApplyColorsCommBase)
{
	m_pApplyColorsCommunicatorBase = pApplyColorsCommBase;
	m_hPanel = NULL;
	m_hTreeCtrl = NULL;
	m_hComboBox = NULL;
	m_hApplyButton = NULL;
	m_hRemoveColorButton = NULL;
}


//-----------------------------------------------------------------------------
// ApplyColorsPanelController::~ApplyColorsPanelController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ApplyColorsPanelController::~ApplyColorsPanelController()
{
}

//-----------------------------------------------------------------------------
// ApplyColorsPanelController::InitApplyColorsPanel
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
void ApplyColorsPanelController::InitApplyColorsPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	m_hInstance = hInstance;
	// Get the mode from the registry:

	// Set up the title:
	static TCHAR buf[80];
	int bufLen = 80;
	LoadString(hInstance, IDS_APPLY_COLORS, buf, bufLen);

	// Append the rollup:
	int nPanelIndex = pCustomRollupWindow->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_APPLYCOLORSPANEL), 
												  ApplyColorsPanelProc, buf, lParam, 0);

	// Get the HWNDs:
	m_hPanel = pCustomRollupWindow->GetPanelDlg(nPanelIndex);
	m_hTreeCtrl = GetDlgItem(m_hPanel, IDC_TREE_APPLYCOLORS);

	// Set up the styles, etc. on the treeview:
	TreeView_SetBkColor(m_hTreeCtrl, RGB(255, 255, 255));

	HIMAGELIST colorImageList;
	colorImageList = ImageList_LoadImage(hInstance, MAKEINTRESOURCE(IDB_TREEVIEW_COLORS),
										 16, 1, CLR_NONE, IMAGE_BITMAP, LR_CREATEDIBSECTION);
	TreeView_SetImageList(m_hTreeCtrl, colorImageList, TVSIL_NORMAL);
}


//-----------------------------------------------------------------------------
// ApplyColorsPanelController::IdCancelMessage
// This method is called to clean up anything dealing with the keyval panel.
// It should be called whenever the parent dialog or window is going away.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsPanelController::IdCancelMessage()
{
	// Now clean up:
	m_hPanel = NULL;
	m_hTreeCtrl = NULL;
}

//-----------------------------------------------------------------------------
// ApplyColorsPanelController::SetApplyButtonState
// This method can be called to set the state (enabled or disabled) for the apply button
//
// Input:
//  bool bEnabled -- Pass in true to enable the Apply button.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsPanelController::SetApplyButtonState(bool bEnabled)
{
	if (m_hPanel != NULL)
	{
		EnableWindow(m_hApplyButton, bEnabled);
	}
}

//-----------------------------------------------------------------------------
// ApplyColorsPanelController::SetRemoveColorButtonState
// This method can be called to set the state (enabled or disabled) for the remove color button
//
// Input:
//  bool bEnabled -- Pass in true to enable the remove color button.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsPanelController::SetRemoveColorButtonState(bool bEnabled)
{
	if (m_hPanel != NULL)
	{
		EnableWindow(m_hRemoveColorButton, bEnabled);
	}
}


//-----------------------------------------------------------------------------
// ApplyColorsPanelController::BuildTreeView
// This method will build the treeview in the apply colors panel based on the current color set
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsPanelController::BuildTreeView()
{
	// Make sure any data in the treeview is removed:
	BOOL bSuccess = TreeView_DeleteAllItems(m_hTreeCtrl);

	int currentColorSetIndex = m_pApplyColorsCommunicatorBase->GetCurrentColorSetIndex();
	ColorSetManager *pColorSetManager = m_pApplyColorsCommunicatorBase->GetColorSetManager();

	// valid color set to display?
	if (currentColorSetIndex>=0 && pColorSetManager != NULL && currentColorSetIndex<pColorSetManager->GetNumColorSets())
	{
		const ColorSet &colorSet = pColorSetManager->GetColorSet(currentColorSetIndex);

		int numColors = colorSet.GetNumColorsUsed();
		for (int colorIndex = 0; colorIndex<numColors; colorIndex++)
		{
			// Now create the insert structure for the key:
			TV_INSERTSTRUCT insertStructColor;
			insertStructColor.hParent = TVI_ROOT;
			insertStructColor.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM ;
			insertStructColor.item.iImage = colorSet.GetColorValueIndex(colorIndex);
			insertStructColor.item.iSelectedImage = colorSet.GetColorValueIndex(colorIndex);
			insertStructColor.item.stateMask = 0;
			insertStructColor.item.pszText = (LPTSTR) colorSet.GetColorName(colorIndex);
			insertStructColor.hInsertAfter = TVI_LAST;
			HTREEITEM newItemKey = TreeView_InsertItem(m_hTreeCtrl, &insertStructColor);
		}
	}
}

//-----------------------------------------------------------------------------
// ApplyColorsPanelController::BuildComboBox
// This method will build the treeview in the apply colors panel based on the current color set
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsPanelController::BuildComboBox()
{
	::SendMessage(m_hComboBox,CB_RESETCONTENT,0,0L);

	int currentColorSetIndex = m_pApplyColorsCommunicatorBase->GetCurrentColorSetIndex();
	ColorSetManager *pColorSetManager = m_pApplyColorsCommunicatorBase->GetColorSetManager();

	// valid color sets to display
	if (pColorSetManager != NULL)
	{
		for (int colorSetIndex=0;colorSetIndex<pColorSetManager->GetNumColorSets();colorSetIndex++)
		{
			const ColorSet &colorSet = pColorSetManager->GetColorSet(colorSetIndex);

			int comboIndex = ::SendMessage(m_hComboBox,CB_ADDSTRING,0,(LPARAM)((const char *)colorSet.GetName()));
			::SendMessage(m_hComboBox,CB_SETITEMDATA,comboIndex,colorSetIndex);
			if (currentColorSetIndex==colorSetIndex)
			{
				::SendMessage(m_hComboBox,CB_SETCURSEL,comboIndex,0L);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// ApplyColorsPanelController::UpdateTreeViewAndKeeper
// This method will update the data in the keeper based on the current attrib
// data and on the current mode (purge, remap, etc.).  It will then update the
// treeview accordingly.  The current state of the treeview (expanded items,
// items checked, etc.) are maintained when at all possible.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsPanelController::UpdateTreeViewAndCombo()
{
	BuildTreeView();
	BuildComboBox();
}

//-----------------------------------------------------------------------------
// ApplyColorsPanelController::HandleApplyButton
// This method will be called when the user clicks on the apply button
// int the apply colors panel.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsPanelController::HandleApplyButton(void)
{
	m_pApplyColorsCommunicatorBase->HandleApplyColor();
}

//-----------------------------------------------------------------------------
// ApplyColorsPanelController::HandleRemoveColorButton
// This method will be called when the user clicks on the remove color button
// int the apply colors panel.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsPanelController::HandleRemoveColorButton(void)
{
	m_pApplyColorsCommunicatorBase->HandleRemoveColor();
}

//-----------------------------------------------------------------------------
// ApplyColorsPanelController::HandleRemoveColorButton
// This method will be called when the user clicks on the remove color button
// int the apply colors panel.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsPanelController::HandleColorSetChange(void)
{
	int comboBoxSelection = ::SendMessage(m_hComboBox,CB_GETCURSEL,0,0L);
	int colorSetIndex = ::SendMessage(m_hComboBox,CB_GETITEMDATA,comboBoxSelection,0L);
	m_pApplyColorsCommunicatorBase->SetCurrentColorSetIndex(colorSetIndex);
	BuildTreeView();
}

