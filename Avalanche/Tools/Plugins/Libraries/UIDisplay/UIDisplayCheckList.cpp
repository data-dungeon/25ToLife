//-----------------------------------------------------------------------------
//	File:		UIDisplayCheckList.cpp
//	Created:	July 30, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayCheckList class contains all the information needed to
//			display the proper ui control for a checklist in Max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	7/30/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplayCheckList.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#define CL_CONTROL_MIN_WIDTH	200
#define CL_CONTROL_WIDTH_OVERHEAD 60

#define BETWEEN_CL_AND_FRAME	4
#define	LV_SCROLL_WIDTH			20

#define LV_UNCHECKED			0x2000
#define LV_CHECKED				0x3000
#define LV_INDETERMINATE		0x4000

using namespace AttrControls;


//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplayCheckList::UIDisplayCheckList
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayCheckList::UIDisplayCheckList()
{
	m_nPixelHeight = 100;
	m_nDefaultState = BST_UNCHECKED;
	m_hLabel = NULL;
	m_hCheckList = NULL;
	m_vectEntityNames.RemoveAll();
	m_vectCheckedStates.RemoveAll();
}


//-----------------------------------------------------------------------------
// UIDisplayCheckList::~UIDisplayCheckList
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayCheckList::~UIDisplayCheckList()
{
	if (m_hLabel != NULL)
		DestroyWindow(m_hLabel);
	if (m_hCheckList != NULL)
		DestroyWindow(m_hCheckList);
}


//-----------------------------------------------------------------------------
// UIDisplayCheckList::SetControlID
// This method is a mutator for the m_nControlID data member.
//
// Input:
//  int nNewControlID -- new control id to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheckList::SetControlID(int nNewControlID)
{
	m_nControlID = nNewControlID;
}

//-----------------------------------------------------------------------------
// UIDisplayCheckList::SetLabelID
// This method is a mutator for the m_nLabelID data member.
//
// Input:
//  int m_nLabelID -- new control id to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheckList::SetLabelID(int nNewLabelID)
{
	m_nLabelID = nNewLabelID;
}

//-----------------------------------------------------------------------------
// UIDisplayCheckList::SetPixelHeight
// This method is a mutator for the m_nPixelHeight data member.
//
// Input:
//  int nPixelHeight -- The new pixelHeight
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheckList::SetPixelHeight(int nNewPixelHeight)
{
	m_nPixelHeight = nNewPixelHeight;
}

//-----------------------------------------------------------------------------
// UIDisplayCheckList::SetDefaultState
// This method is a mutator for the m_nDefaultState data member.
//
// Input:
//  int nNewDefaultState -- The new default state to set to.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheckList::SetDefaultState(int nNewDefaultState)
{
	m_nDefaultState = nNewDefaultState;
}


//-----------------------------------------------------------------------------
// UIDisplayCheckList::SetEntityNames
// This method is a mutator for the m_vectEntityNames.  The vector will be set
// to be a mirror image of the vector passed in.
//
// Input:
//  TupArray<TupString>& vectEntityNames -- The vector to mirror. 
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheckList::SetEntityNames(const Keeper<TupString>& vectEntityNames)
{
	m_vectEntityNames.RemoveAll();

	int nNumEntityNames = vectEntityNames.GetSize();
	for (int nIndex = 0; nIndex < nNumEntityNames; nIndex++)
	{
		m_vectEntityNames.Add(vectEntityNames[nIndex]);
	}
}


//-----------------------------------------------------------------------------
// UIDisplayCheckList::SetCheckedStates
// This method is a mutator for the m_vectCheckedStates.  The vector will be
// set to be a mirror image of the vector passed in.
//
// Input:
//  TupArray<int>& vectCheckedStates -- The vector to mirror.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheckList::SetCheckedStates(const TupArray<int>& vectCheckedStates)
{
	m_vectCheckedStates.RemoveAll();

	int nNumCheckedStates = vectCheckedStates.GetSize();
	for (int nIndex = 0; nIndex < nNumCheckedStates; nIndex++)
	{
		m_vectCheckedStates.Add(vectCheckedStates[nIndex]);
	}
}


//-----------------------------------------------------------------------------
// UIDisplayCheckList::BuildActualUI
// This method will set up and create the actual UI necessary to display a
// checklist.
//
// Input:
//  HWND hParentDlg -- The HWND of the parent dialog.
//  HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The handle to the tooltip for the parent dialog.
//  HFONT hUIFont -- The font to use.
//  int nStartx -- The base x coord to use for checklist.  If the indent level
//				   for this control is > 0, then the checklist will be drawn to
//				   the right of this x coord (based on the level of
//				   indentation).
//	int nStarty -- The y coord to start the label at.
//  int nLabelWidth -- The width of the label for the checklist.
//  int nLabelHeight -- The height of the label for the checklist.
//	int& nControlSizeY -- Upon return, this value should represent the size that
//						  the control should take up in the Y direction.
//
// Output: True if succesfull, false otherwise.
//-----------------------------------------------------------------------------
bool UIDisplayCheckList::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance, HWND hToolTip,
								  int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
								  int& nControlSizeY)
{
	bool bResult = true;

	// Figure out how far to indent:
	int nXOffset = (m_nIndentLevel * INDENT_SCALER) + m_nAddedIndent;

	// Create the static text label:
	// Figure out the necessary size of the label:
	HDC hdc = GetDC(hParentDlg);
	HFONT hFixedFont = CustomControls::GetFixedFont();
	HFONT hfntOld = (HFONT) SelectObject(hdc, hFixedFont);
	SIZE labelPixelWidth;
	GetTextExtentPoint32(hdc, m_strLabel, m_strLabel.GetLength(), &labelPixelWidth);

	// Create the list view:
	// Figure out the width:
	int nNumNames = m_vectEntityNames.GetSize();
	int maxNameLength;
	int maxNameIndex = -1;
	for (int nIndex = 0; nIndex < nNumNames; nIndex++)
	{
		if (maxNameIndex == -1 || maxNameLength < m_vectEntityNames[nIndex].GetLength())
		{
			maxNameLength = m_vectEntityNames[nIndex].GetLength();
			maxNameIndex = nIndex;
		}
	}
	int nControlWidth = CL_CONTROL_MIN_WIDTH;
	if (maxNameIndex >=0)
	{
		SIZE maxListBoxWidth;
		GetTextExtentPoint32(hdc, m_vectEntityNames[maxNameIndex], m_vectEntityNames[maxNameIndex].GetLength(), &maxListBoxWidth);
		if (maxListBoxWidth.cx + CL_CONTROL_WIDTH_OVERHEAD > nControlWidth)
			nControlWidth = maxListBoxWidth.cx + CL_CONTROL_WIDTH_OVERHEAD;
	}

	SelectObject(hdc, hfntOld);
	ReleaseDC(hParentDlg, hdc);
	DWORD nDisabledFlag = GetDisabled() ? WS_DISABLED : 0;
	m_hLabel = CreateWindow(_T("static"), m_strLabel,
							WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOTIFY | nDisabledFlag,
							nStartx + nXOffset, nStarty, labelPixelWidth.cx, nLabelHeight,
							hParentDlg, (HMENU) m_nLabelID, hInstance, NULL);

	// Set its font:
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM) hFixedFont, MAKELPARAM(TRUE,0));

	// set up the unique values button
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		m_hUniqueValuesButton = CreateWindow(_T("button"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_ICON | nDisabledFlag,
			nStartx + nXOffset + labelPixelWidth.cx + 3, nStarty + 2, 12, 12, hParentDlg, (HMENU) m_uniqueValuesID,
			hInstance, NULL);
		m_hUniqueValuesIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_UNIQUE_VALUES), IMAGE_ICON, 0, 0, 0);
		::SendMessage(m_hUniqueValuesButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hUniqueValuesIcon);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hUniqueValuesButton);
	}

	// Add to the control size in Y:
	nControlSizeY = 15;

	int nControlX = nStartx + nXOffset;

	m_hCheckList = CreateWindow(WC_LISTVIEW, NULL,
								WS_CHILD | WS_VISIBLE | WS_DLGFRAME | WS_TABSTOP |
								LVS_REPORT | LVS_ALIGNTOP | LVS_SORTASCENDING | LVS_NOCOLUMNHEADER | nDisabledFlag,
								nControlX, nStarty + 15, nControlWidth, m_nPixelHeight,
								hParentDlg, (HMENU) m_nControlID, hInstance, NULL);

	SendMessage(m_hCheckList, WM_SETFONT, (WPARAM) hFixedFont, MAKELPARAM(TRUE,0));

	// Set up the state image list:
	HIMAGELIST stateImageList;
	stateImageList = ImageList_LoadImage(hInstance, MAKEINTRESOURCE(IDB_CHECKLIST_STATE),
										 16, 1, RGB(190, 190, 190), IMAGE_BITMAP, 0);
	ListView_SetImageList(m_hCheckList, stateImageList, LVSIL_STATE);

	// Create a column for the list view:
	RECT RectListView;
	GetClientRect(m_hCheckList, &RectListView);
	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH;
	lvColumn.cx = RectListView.right - RectListView.left - LV_SCROLL_WIDTH;
	ListView_InsertColumn(m_hCheckList, 0, &lvColumn);

	// Add to the control size in Y:
	nControlSizeY += m_nPixelHeight;

	// Now add the items to the listview:
	this->LoadListView();

	// tell the tooltip about the controls
	if (m_strToolTip.GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hLabel);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hCheckList);
	}

	m_nCenterY = nStarty + 15 + (m_nPixelHeight>>1);
	m_nTopY = nStarty;
	m_nBottomY = nStarty+nControlSizeY;

	return bResult;
}


//-----------------------------------------------------------------------------
// UIDisplayCheckList::LoadListView
// This method will actually perform the loading up of items in the listview.
// Before calling this method, the m_vectEntityNames and the
// m_vectCheckedStates data members should be valid.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheckList::LoadListView()
{
	// Cycle through the names loading them into the listview with their
	// correct checked states:
	::SendMessage(m_hCheckList,WM_SETREDRAW,FALSE,0);
	int nNumNames = m_vectEntityNames.GetSize();
	for (int nIndex = 0; nIndex < nNumNames; nIndex++)
	{
		LVITEM lvItemInsert;
		ZeroMemory(&lvItemInsert, sizeof(LVITEM));
		lvItemInsert.mask = LVIF_TEXT | LVIF_STATE;
		lvItemInsert.iItem = nIndex;
		lvItemInsert.pszText = (LPTSTR) ((const char *)m_vectEntityNames[nIndex]);
		lvItemInsert.state = INDEXTOSTATEIMAGEMASK(m_vectCheckedStates[nIndex] + 2);
		lvItemInsert.stateMask = LVIS_STATEIMAGEMASK;
		int nResult = ListView_InsertItem(m_hCheckList, &lvItemInsert);		
	}
	::SendMessage(m_hCheckList,WM_SETREDRAW,TRUE,0);
}


//-----------------------------------------------------------------------------
// UIDisplayCheckList::HandledWinMessage
// This method will look at the incoming message and decide if this object
// needs to handle the message, and if so, how it does it.
//
// Input:
//  HWND hParent -- The HWND of the parent of the control the message pertains to.
//  UINT message -- Specifies the message.
//  WPARAM wParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message.
//  LPARAM lParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message.
//  TupString& strKeyname -- If any object handled the message, then upon
//							   return, this parameter will hold the keyname in
//							   question, otherwise the contents will not be changed.
//  TupString& strValue -- If any object handled the message, then upon
//							 return, this parameter will hold the value in
//							 question, otherwise the contents will not be changed.
//	TupArray<TupString>& vectKeysToPurge -- This parameter is ignored.
//
// Output: True if this object handled the message, false otherwise.  Also, see
//		   above for the two string parameters that are also used as return values.
//-----------------------------------------------------------------------------
bool UIDisplayCheckList::HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
										   TupString& strKeyname, TupString& strValue,
										   TupArray<TupString>& vectKeysToPurge, int& nFlags)
{
	bool bResult = false;

	switch (message)
	{
	case WM_COMMAND:
		{
			switch(HIWORD(wParam))
			{
				// Check for the unique values button press
				case BN_CLICKED:
				{
					// See if the control id matches this object's id:
					int nID = LOWORD(wParam);
					if (nID == m_uniqueValuesID)
					{
						HMENU hFindPopupMenu = ::CreatePopupMenu();
						HMENU hSetPopupMenu = ::CreatePopupMenu();
						HMENU hPopupMenu = ::CreatePopupMenu();

						int numUniqueValues = m_uniqueValues.m_uniqueValues.GetSize();
						TupArray<TupString> menuStrings;
						menuStrings.SetSize(numUniqueValues);

						const int startingFindID = 1000;
						const int startingSetID = 2000;

						for (int entryIndex=0;entryIndex<numUniqueValues;entryIndex++)
						{
							UniqueValue &uniqueValue = m_uniqueValues.m_uniqueValues[entryIndex];
							TupString &menuString = menuStrings[entryIndex];

							if (entryIndex==m_uniqueValues.m_defaultIndex)
								menuString.Format("%s (%d of %d Default)",(const char *) uniqueValue.m_strValue,uniqueValue.m_count,m_uniqueValues.m_totalCount);
							else
								menuString.Format("%s (%d of %d)",(const char *) uniqueValue.m_strValue,uniqueValue.m_count,m_uniqueValues.m_totalCount);
							::AppendMenu(hFindPopupMenu, MF_STRING|MF_ENABLED, startingFindID + entryIndex, menuString);
							::AppendMenu(hSetPopupMenu, MF_STRING|MF_ENABLED, startingSetID + entryIndex, menuString);
						}

						::AppendMenu(hPopupMenu, MF_STRING|MF_ENABLED|MF_POPUP,(unsigned int) hFindPopupMenu, "&Find");
						::AppendMenu(hPopupMenu, MF_STRING|MF_ENABLED|MF_POPUP,(unsigned int) hSetPopupMenu, "&Set");

						RECT buttonRect;
						::GetWindowRect(m_hUniqueValuesButton, &buttonRect);

						int result = ::TrackPopupMenu(hPopupMenu,
							TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
							buttonRect.right,buttonRect.bottom,
							0,m_hUniqueValuesButton,NULL);

						DestroyMenu(hFindPopupMenu);
						DestroyMenu(hSetPopupMenu);
						DestroyMenu(hPopupMenu);

						if (result>=startingSetID && result <startingSetID+numUniqueValues)
						{
							// Pass back the keyname and value:
							strKeyname = m_strKeyname;
							strValue = m_uniqueValues.m_uniqueValues[result-startingSetID].m_strValue;

							nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;

							// Set the exclusive flag:
							if (m_bExclusive)
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;
						}
						else if (result>=startingFindID && result <startingFindID+numUniqueValues)
						{
							strKeyname = m_strKeyname;
							strValue = m_uniqueValues.m_uniqueValues[result-startingFindID].m_strValue;
							nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_FIND_DIALOG;
						}
						bResult = true;
					}
					break;
				}
			}
		}
		break;
		// Check for an item being clicked on in the listview:
		case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR) lParam;
			switch (hdr->code)
			{
				// Check to see if a listview was clicked on:
				case NM_CLICK:
				{
					if (hdr->idFrom == (unsigned)m_nControlID)
					{
						LVHITTESTINFO ht = {0};
						DWORD dwpos = GetMessagePos();
						ht.pt.x = GET_X_LPARAM(dwpos);
						ht.pt.y = GET_Y_LPARAM(dwpos);
						MapWindowPoints(HWND_DESKTOP, hdr->hwndFrom, &ht.pt, 1);
						ListView_HitTest(hdr->hwndFrom, &ht);

						if (LVHT_ONITEMSTATEICON & ht.flags)
						{
							// The user clicked on this listview.
							bResult = true;

							// Change the checked state of the item:
							int nItemIndex = ht.iItem;
							UINT uiState = ListView_GetItemState(m_hCheckList, nItemIndex, LVIS_STATEIMAGEMASK);
							uiState &= LVIS_STATEIMAGEMASK;
							if (uiState == LV_UNCHECKED)
							{
								uiState = LV_CHECKED;
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_FORM_CHECK_LIST;
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_CHECK_LIST_CHECKED;
								ListView_SetItemState(m_hCheckList, nItemIndex,
													uiState, LVIS_STATEIMAGEMASK);
							}
							else if (uiState == LV_CHECKED)
							{
								uiState = LV_UNCHECKED;
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_FORM_CHECK_LIST;
								nFlags &= ~UIDisplayBase::WIN_MESSAGE_FLAG_CHECK_LIST_CHECKED;
								ListView_SetItemState(m_hCheckList, nItemIndex,
													uiState, LVIS_STATEIMAGEMASK);
							}
							else if (uiState == LV_INDETERMINATE)
							{
								uiState = LV_UNCHECKED;
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_FORM_CHECK_LIST;
								nFlags &= ~UIDisplayBase::WIN_MESSAGE_FLAG_CHECK_LIST_CHECKED;
								ListView_SetItemState(m_hCheckList, nItemIndex,
													uiState, LVIS_STATEIMAGEMASK);
							}

							// Set the keyname and value:
							strKeyname = m_strKeyname;
							TCHAR buff[500];
							ListView_GetItemText(m_hCheckList, nItemIndex, 0,
												buff, sizeof(buff));
							strValue = buff;

							// Set the exclusive flag:
							if (m_bExclusive)
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;
						}
					}
				}
				break;
				// if this is a tooltip message
				case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
				{
					ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;
					if (pNotify->ti->m_hWnd == m_hCheckList)
					{
						pNotify->ti->m_sTooltip = m_strToolTip;
						bResult = true;
					}
					else if ((m_uniqueValues.m_uniqueValues.GetSize() && pNotify->ti->m_hWnd == m_hUniqueValuesButton))
					{
						pNotify->ti->m_sTooltip = "Pick from selected values";
						bResult = true;
					}
				}
				break;
			}
			break;
		}
	}

	return bResult;
}

//-----------------------------------------------------------------------------
// UIDisplayCheckList::RemoveToolTips
// This method call RemoveToolTip on all controls that have requested one
//
// Input:
//  HWND hParent -- The HWND of the parent of the control the message pertains to.
//  HINSTANCE hInstance -- Application Instance
//  HWND hToolTip -- The HWND of the tooltip control
//
// Output: none
//
//-----------------------------------------------------------------------------
void UIDisplayCheckList::RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip)
{
	if (m_strToolTip.GetLength())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hLabel);
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hCheckList);
	}
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hUniqueValuesButton);
	}
}
