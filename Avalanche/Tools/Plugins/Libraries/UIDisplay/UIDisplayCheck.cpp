//-----------------------------------------------------------------------------
//	File:		UIDisplayCheck.cpp
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayCheck class contains all the information needed to
//			display the proper ui control for a checkbox in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplayCheck.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#define CHECK_BOX_WIDTH	19

using namespace AttrControls;


//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplayCheck::UIDisplayCheck
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayCheck::UIDisplayCheck()
{
	m_nState = BST_UNCHECKED;
	m_hCheckBox = NULL;
}


//-----------------------------------------------------------------------------
// UIDisplayCheck::~UIDisplayCheck
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayCheck::~UIDisplayCheck()
{
	if (m_hCheckBox != NULL)
		DestroyWindow(m_hCheckBox);
}


//-----------------------------------------------------------------------------
// UIDisplayCheck::SetState
// This method will set the state for this checkbox (BST_CHECKED,
// BST_UNCHECKED, or BST_INDETERMINATE).
//
// Input:
//  int nNewState -- One of the three possible states.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheck::SetState(int nNewState)
{
	m_nState = nNewState;
}


//-----------------------------------------------------------------------------
// UIDisplayCheck::SetControlID
// This method will set the control id for this checkbox.
//
// Input:
//  int nNewControlID -- New control id to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheck::SetControlID(int nNewControlID)
{
	m_nControlID = nNewControlID;
}

//-----------------------------------------------------------------------------
// UIDisplayCheck::AddPurgeKey
// This method will add a key string to the vector of keys to purge.
//
// Input:
//  TupString strKeyToPurge -- String to add as a key to purge.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCheck::AddPurgeKey(TupString strKeyToPurge)
{
	m_vectKeysToPurge.Add(strKeyToPurge);
}


//-----------------------------------------------------------------------------
// UIDisplayCheck::GetState
// This method will return the state for this checkbox (BST_CHECKED,
// BST_UNCHECKED, or BST_INDETERMINATE).
//
// Input: None.
//
// Output: int representing the state.
//-----------------------------------------------------------------------------
int UIDisplayCheck::GetState()
{
	return m_nState;
}


//-----------------------------------------------------------------------------
// UIDisplayCheck::GetControlID
// This method will return the control if for this checkbox.
//
// Input: None.
//
// Output: int representing the control id.
//-----------------------------------------------------------------------------
int UIDisplayCheck::GetControlID()
{
	return m_nControlID;
}


//-----------------------------------------------------------------------------
// UIDisplayCheck::BuildActualUI
// This method will set up and create the actual UI necessary to display a
// checkbox.
//
// Input:
//  HWND hParentDlg -- The HWND of the parent dialog.
//  HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The handle to the tooltip for the parent dialog.
//  HFONT hUIFont -- The font to use.
//  int nStartx -- The base x coord to use for checkbox.  If the indent level
//				   for this control is > 0, then the checkbox will be drawn to
//				   the right of this x coord (based on the level of
//				   indentation).
//	int nStarty -- The y coord to draw the checkbox at.
//  int nLabelWidth -- The width of the label for the checkbox.
//  int nLabelHeight -- The height of the label for the checkbox.
//	int& nControlSizeY -- Upon return, this value should represent the size that
//						  the control should take up in the Y direction.
//
// Output: True if succesfull, false otherwise.
//-----------------------------------------------------------------------------
bool UIDisplayCheck::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance, HWND hToolTip,
								   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
								   int& nControlSizeY)
{
	bool bResult = true;

	// Figure out how far to indent:
	int nXOffset = (m_nIndentLevel * INDENT_SCALER) + m_nAddedIndent;

	// Figure out the necessary size of the checkbox:
	HDC hdc = GetDC(hParentDlg);
//	HFONT hLabelFont = m_bExclusive ? CustomControls::GetFixedFont() :  CustomControls::GetFixedFontBold();
	HFONT hLabelFont = CustomControls::GetFixedFont();
	HFONT hfntOld = (HFONT) SelectObject(hdc, hLabelFont);
	SIZE size;
	GetTextExtentPoint32(hdc, m_strLabel, m_strLabel.GetLength(), &size);
	SelectObject(hdc, hfntOld);
	ReleaseDC(hParentDlg, hdc);
	DWORD nDisabledFlag = GetDisabled() ? WS_DISABLED : 0;
	int nNonExclusiveFlag = m_bExclusive ? 0 : BS_PUSHLIKE;
	int nWindowHeight = m_bExclusive ? 14 : 14 + 6;
	int nWindowWidth = CHECK_BOX_WIDTH + size.cx;

	// Create the checkbox:
	m_hCheckBox = CreateWindow(_T("button"), m_strLabel,
							   WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTO3STATE | nNonExclusiveFlag | nDisabledFlag,
							   nStartx + nXOffset, nStarty, nWindowWidth, nWindowHeight,
							   hParentDlg, (HMENU) m_nControlID, hInstance, NULL);

	SendMessage(m_hCheckBox, WM_SETFONT, (WPARAM) hLabelFont, MAKELPARAM(TRUE,0));
	
	// Set the checkbox's state:
	if (this->GetState() == BST_CHECKED)
		SendMessage(m_hCheckBox, BM_SETCHECK, BST_CHECKED, 0);
	else if (this->GetState() == BST_UNCHECKED)
		SendMessage(m_hCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);
	else if (this->GetState() == BST_INDETERMINATE)
		SendMessage(m_hCheckBox, BM_SETCHECK, BST_INDETERMINATE, 0);
	else
		SendMessage(m_hCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);

	// set up the unique values button
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		m_hUniqueValuesButton = CreateWindow(_T("button"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_ICON | nDisabledFlag,
			nStartx + nXOffset + CHECK_BOX_WIDTH + size.cx + 3, nStarty + 1, 12, 12, hParentDlg, (HMENU) m_uniqueValuesID,
			hInstance, NULL);
		m_hUniqueValuesIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_UNIQUE_VALUES), IMAGE_ICON, 0, 0, 0);
		::SendMessage(m_hUniqueValuesButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hUniqueValuesIcon);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hUniqueValuesButton);
	}

	// Set the size of the control in Y:
	nControlSizeY = nWindowHeight;

	// tell the tooltip about the checkbox
	if (m_strToolTip.GetLength())
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hCheckBox);

	m_nCenterY = nStarty + (nControlSizeY>>1) - 1;
	m_nTopY = nStarty;
	m_nBottomY = nStarty+nControlSizeY;

	return bResult;
}


//-----------------------------------------------------------------------------
// UIDisplayCheck::HandledWinMessage
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
//	TupArray<TupString>& vectKeysToPurge -- Upon return, this vector will
//												 be loaded with any keys to purge.
//	int& nFlags -- Any flags that need to be returned to the calling method.
//
// Output: True if this object handled the message, false otherwise.  Also, see
//		   above for the two string parameters that are also used as return values.
//-----------------------------------------------------------------------------
bool UIDisplayCheck::HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
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
					// Check for a checkbox button pressed:
					case BN_CLICKED:
					{
						// See if the control id matches this object's id:
						int nID = LOWORD(wParam);
						if (nID == m_nControlID)
						{
							// Get its state:
							int nCheckFlags = SendMessage(m_hCheckBox, BM_GETSTATE, 0, 0);
							if ((nCheckFlags & 0x0003) == BST_CHECKED)
							{
								this->SetState(BST_CHECKED);
								strKeyname = m_strKeyname;
								strValue = "True";
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;
								bResult = true;
							}
							else if ((nCheckFlags & 0x0003) == BST_UNCHECKED)
							{
								this->SetState(BST_UNCHECKED);
								strKeyname = m_strKeyname;
								strValue = "False";
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;
								bResult = true;
							}
							else if ((nCheckFlags & 0x0003) == BST_INDETERMINATE)
							{
								// When the user clicks on a tri-state checkbox
								// that is checked, it goes to an indeterminate
								// state.  I want to ignore this and switch it
								// automatically to an unchecked state:
								SendMessage(m_hCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);
								this->SetState(BST_UNCHECKED);
								strKeyname = m_strKeyname;
								strValue = "False";
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;
								bResult = true;
							}
							else
							{
								// Shouldn't get to here!
							}

							// Load up any keys to purge:
							int nNumPurgeKeys = m_vectKeysToPurge.GetSize();
							for (int nIndex = 0; nIndex < nNumPurgeKeys; nIndex++)
							{
								vectKeysToPurge.Add(m_vectKeysToPurge[nIndex]);
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_PURGE_KEYS;
							}

							// Set the exclusive flag:
							if (m_bExclusive)
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;
						}
						else if (nID == m_uniqueValuesID)
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
								// Load up any keys to purge:
								int nNumPurgeKeys = m_vectKeysToPurge.GetSize();
								for (int nIndex = 0; nIndex < nNumPurgeKeys; nIndex++)
								{
									vectKeysToPurge.Add(m_vectKeysToPurge[nIndex]);
									nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_PURGE_KEYS;
								}

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
		case WM_NOTIFY:
			{
				LPNMHDR hdr = (LPNMHDR) lParam;
				switch (hdr->code)
				{
					// if this is a tooltip message
					case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
					{
						ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;
						if (pNotify->ti->m_hWnd == m_hCheckBox)
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
			}
			break;
	}
	return bResult;
}


//-----------------------------------------------------------------------------
// UIDisplayCheck::RemoveToolTips
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
void UIDisplayCheck::RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip)
{
	if (m_strToolTip.GetLength())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hCheckBox);
	}
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hUniqueValuesButton);
	}
}
