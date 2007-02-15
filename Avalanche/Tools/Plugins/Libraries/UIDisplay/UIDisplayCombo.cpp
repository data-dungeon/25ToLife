//-----------------------------------------------------------------------------
//	File:		UIDisplayCombo.cpp
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayCombo class contains all the information needed to
//			display the proper ui control for a combo box in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplayCombo.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#define COMBO_CONTROL_MIN_WIDTH	200
#define COMBO_CONTROL_WIDTH_OVERHEAD 40
#define BETWEEN_COMBO_AND_FRAME	4

using namespace AttrControls;


//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplayCombo::UIDisplayCombo
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayCombo::UIDisplayCombo()
{
	m_hLabel = NULL;
	m_hComboBox = NULL;
	m_bShowIndeterminate = false;
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::~UIDisplayCombo
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayCombo::~UIDisplayCombo()
{
	if (m_hLabel != NULL)
		DestroyWindow(m_hLabel);
	if (m_hComboBox != NULL)
		DestroyWindow(m_hComboBox);
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::SetControlID
// This method will set the control id for this combobox.
//
// Input:
//  int nNewControlID -- New control id to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCombo::SetControlID(int nNewControlID)
{
	m_nControlID = nNewControlID;
}

//-----------------------------------------------------------------------------
// UIDisplayCombo::SetLabelID
// This method is a mutator for the m_nLabelID data member.
//
// Input:
//  int m_nLabelID -- new control id to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCombo::SetLabelID(int nNewLabelID)
{
	m_nLabelID = nNewLabelID;
}

//-----------------------------------------------------------------------------
// UIDisplayCombo::SetSelectionIndex
// This method will set the index for the current selection for this combobox.
//
// Input:
//  int nNewSelIndex -- New index for current selection.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCombo::SetSelectionIndex(int nNewSelIndex)
{
	m_nCurrSelIndex = nNewSelIndex;
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::AddComboLabel
// This method will add a combo label for this combobox to the vector of
// labels.
//
// Input:
//  TupString strToAdd -- String to add as a label.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCombo::AddComboLabel(const TupString &strLabel)
{
	m_vectComboLabels.Add(strLabel);
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::AddComboKey
// This method will add a combo key for this combobox to the vector of keys.
//
// Input:
//  TupString strToAdd -- String to add as a key.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCombo::AddComboKey(const TupString &strToAdd)
{
	m_vectComboKeys.Add(strToAdd);
}

//-----------------------------------------------------------------------------
// UIDisplayCombo::AddComboToolTip
// This method will add a combo tooltip for this combobox to the vector of keys.
//
// Input:
//  TupString strToAdd -- String to add as a key.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCombo::AddComboToolTip(const TupString &strLabel,const TupString &strKey,const TupString &strToolTip)
{
	TupString newString;
	if (strKey.GetLength())
	{
		TupString colorString;
		if (m_bExclusive)
		{
			colorString = "<ct=0xff0000>"; // blue
		}
		else
		{
			colorString = "<ct=0x00c000>"; // green
		}

		if (strToolTip.GetLength())
		{
			newString.Format("\n<al_c><hr=90%></al_c>\n\n%s<al_c><b>Value:</b> %s (%s)</al_c></ct>\n\n%s",(const char *)colorString,(const char *)strLabel,(const char *)strKey,(const char *)strToolTip);
		}
		else
		{
			newString.Format("\n<al_c><hr=90%></al_c>\n\n%s<al_c><b>Value:</b> %s (%s)</al_c></ct>",(const char *)colorString,(const char *)strLabel,(const char *)strKey);
		}
	}
	else
	{
		newString = strToolTip;
	}
	m_vectComboToolTips.Add(newString);
}

//-----------------------------------------------------------------------------
// UIDisplayCombo::AddPurgeKey
// This method will add a key string to the vector of keys to purge.
//
// Input:
//  TupString strKeyToPurge -- String to add as a key to purge.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCombo::AddPurgeKey(const TupString &strKeyToPurge)
{
	m_vectKeysToPurge.Add(strKeyToPurge);
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::SetShowIndeterminate
// This method will set the m_bShowIndeterminate data member.
//
// Input:
//  bool bShow -- Pass in true if the indeterminate state should be shown in
//				  the control, false otherwise.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayCombo::SetShowIndeterminate(bool bShow)
{
	m_bShowIndeterminate = bShow;
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::GetControlID
// This method will return the control id for this combobox.
//
// Input: None.
//
// Output: int representing the control id.
//-----------------------------------------------------------------------------
int UIDisplayCombo::GetControlID()
{
	return m_nControlID;
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::GetSelectionIndex
// This method will get the current selection index for this combobox.
//
// Input: None.
//
// Output: int representing the current selection index.
//-----------------------------------------------------------------------------
int UIDisplayCombo::GetSelectionIndex()
{
	return m_nCurrSelIndex;
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::GetComboLabel
// This method will return the combo label at the index specified.
//
// Input:
//  int nIndex -- The index of the combo label to get.
//
// Output: TupString of the combo label.
//-----------------------------------------------------------------------------
TupString UIDisplayCombo::GetComboLabel(int nIndex)
{
	int nNumLabels = m_vectComboLabels.GetSize();
	if (nIndex < nNumLabels)
	{
		return m_vectComboLabels[nIndex];
	}
	else
	{
		TupString strReturn = "";
		return strReturn;
	}
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::GetComboKey
// This method will return the combo key at the index specified.
//
// Input:
//  int nIndex -- The index of the combo key to get.
//
// Output: TupString of the combo key.
//-----------------------------------------------------------------------------
TupString UIDisplayCombo::GetComboKey(int nIndex)
{
	int nNumKeys = m_vectComboKeys.GetSize();
	if (nIndex < nNumKeys)
	{
		return m_vectComboKeys[nIndex];
	}
	else
	{
		TupString strReturn = "";
		return strReturn;
	}
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::BuildActualUI
// This method will set up and create the actual UI necessary to display a
// combobox.
//
// Input:
//  HWND hParentDlg -- The HWND of the parent dialog.
//  HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The handle to the tooltip for the parent dialog.
//  HFONT hUIFont -- The font to use.
//  int nStartx -- The base x coord to use for combobox.  If the indent level
//				   for this control is > 0, then the combobox will be drawn to
//				   the right of this x coord (based on the level of
//				   indentation).
//  int nStarty -- The y coord to start the label at.
//  int nLabelWidth -- The width of the label for the combobox.
//  int nLabelHeight -- The height of the label for the combobox.
//  int& nControlSizeY -- Upon return, this value should represent the size that
//						  the control should take up in the Y direction.
//
// Output: True if succesfull, false otherwise.
//-----------------------------------------------------------------------------
bool UIDisplayCombo::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance, HWND hToolTip,
								   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
								   int& nControlSizeY)
{
	bool bResult = true;

	// Figure out how far to indent:
	int nXOffset = (m_nIndentLevel * INDENT_SCALER) + m_nAddedIndent;

	// Create the static text label:
	// Figure out the necessary size of the label:
	HDC hdc = GetDC(hParentDlg);
	HFONT hLabelFont = m_bExclusive ? CustomControls::GetFixedFont() : CustomControls::GetFixedFontBold();
	HFONT hFixedFont = CustomControls::GetFixedFont();
	HFONT hfntOld = (HFONT) SelectObject(hdc, hLabelFont);
	SIZE labelPixelWidth;
	GetTextExtentPoint32(hdc, m_strLabel, m_strLabel.GetLength(), &labelPixelWidth);

	// Figure out the width:
	int nNumNames = m_vectComboLabels.GetSize();
	int maxNameLength;
	int maxNameIndex = -1;
	for (int nIndex = 0; nIndex < nNumNames; nIndex++)
	{
		if (maxNameIndex == -1 || maxNameLength < m_vectComboLabels[nIndex].GetLength())
		{
			maxNameLength = m_vectComboLabels[nIndex].GetLength();
			maxNameIndex = nIndex;
		}
	}
	int nControlWidth = COMBO_CONTROL_MIN_WIDTH;
	if (maxNameIndex >=0)
	{
		SIZE maxListBoxWidth;
		GetTextExtentPoint32(hdc, m_vectComboLabels[maxNameIndex], m_vectComboLabels[maxNameIndex].GetLength(), &maxListBoxWidth);
		if (maxListBoxWidth.cx + COMBO_CONTROL_WIDTH_OVERHEAD > nControlWidth)
			nControlWidth = maxListBoxWidth.cx + COMBO_CONTROL_WIDTH_OVERHEAD;
	}

	SelectObject(hdc, hfntOld);
	ReleaseDC(hParentDlg, hdc);
	DWORD nDisabledFlag = GetDisabled() ? WS_DISABLED : 0;
	m_hLabel = CreateWindow(_T("static"), m_strLabel,
							WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOTIFY | nDisabledFlag,
							nStartx + nXOffset, nStarty, labelPixelWidth.cx+2, nLabelHeight,
							hParentDlg, (HMENU) m_nLabelID, hInstance, NULL);

	// Set its font:
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM) hLabelFont, MAKELPARAM(TRUE,0));

	// Add to the control size in Y:
	nControlSizeY = 15;
	int nControlX = nStartx + nXOffset;

	m_hComboBox = CreateWindow(_T("combobox"), NULL,
							   WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | nDisabledFlag,
							   nControlX, nStarty + 15, nControlWidth, 200, hParentDlg, (HMENU) m_nControlID,
							   hInstance, NULL);

	// Set its font:
	SendMessage(m_hComboBox, WM_SETFONT, (WPARAM) hFixedFont, MAKELPARAM(TRUE,0));

	::SendMessage(m_hComboBox,WM_SETREDRAW,FALSE,0);
	// Add items to the combobox:
	int nNumItems = m_vectComboLabels.GetSize();
	for (int nIndex = 0; nIndex < nNumItems; nIndex++)
	{
		SendMessage(m_hComboBox, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) m_vectComboLabels[nIndex]);
	}
	::SendMessage(m_hComboBox,WM_SETREDRAW,TRUE,0);

	// Set the currently selected item:
	SendMessage(m_hComboBox, CB_SETCURSEL, (WPARAM) m_nCurrSelIndex, 0);

	// set up the unique values button
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		m_hUniqueValuesButton = CreateWindow(_T("button"), NULL, 
									WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_ICON | nDisabledFlag,
									nControlX + nControlWidth + 3, nStarty + 19, 12, 12, hParentDlg, (HMENU) m_uniqueValuesID,
									hInstance, NULL);
		m_hUniqueValuesIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_UNIQUE_VALUES), IMAGE_ICON, 0, 0, 0);
		::SendMessage(m_hUniqueValuesButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hUniqueValuesIcon);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hUniqueValuesButton);
	}

	// Set the control size in Y:
	nControlSizeY += 20;

	// tell the tooltip about the controls
	if (m_strToolTip.GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hLabel);
	}
	if (!m_bShowIndeterminate && m_vectComboToolTips[m_nCurrSelIndex].GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hComboBox);
	}

	m_nCenterY = nStarty + 25;
	m_nTopY = nStarty;
	m_nBottomY = nStarty+nControlSizeY;

	return bResult;
}


//-----------------------------------------------------------------------------
// UIDisplayCombo::HandledWinMessage
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
bool UIDisplayCombo::HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
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
				// Check for a selection change in a combobox:
				case CBN_CLOSEUP:
				{
					// See if the control id matches this object's id:
					int nID = LOWORD(wParam);
					if (nID == m_nControlID)
					{
						// Change the selection index:
						int nSelIndex = SendMessage(m_hComboBox, CB_GETCURSEL, 0, 0);
						if (nSelIndex > -1)
						{
							if (nSelIndex != this->GetSelectionIndex())
							{
								this->SetSelectionIndex(nSelIndex);

								// Load up any keys to purge:
								int nNumPurgeKeys = m_vectKeysToPurge.GetSize();
								for (int nIndex = 0; nIndex < nNumPurgeKeys; nIndex++)
								{
									vectKeysToPurge.Add(m_vectKeysToPurge[nIndex]);
									nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_PURGE_KEYS;
								}

								// Pass back the keyname and value:
								strKeyname = m_strKeyname;
								strValue = m_vectComboKeys[nSelIndex];

								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;

								// Set the exclusive flag:
								if (m_bExclusive)
									nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;

								bResult = true;
							}
						}
					}
				}
				break;
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
					 case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
					 {
						 ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;
						 if (pNotify->ti->m_hWnd == m_hLabel)
						 {
							 pNotify->ti->m_sTooltip = m_strToolTip;
							 bResult = true;
						 }
						 else if (pNotify->ti->m_hWnd == m_hComboBox)
						 {
							 pNotify->ti->m_sTooltip.Format("%s\n%s",m_strToolTip,m_vectComboToolTips[m_nCurrSelIndex]);
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

	}

	return bResult;
}

//-----------------------------------------------------------------------------
// UIDisplayCombo::RemoveToolTips
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
void UIDisplayCombo::RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip)
{
	if (m_strToolTip.GetLength())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hLabel);
	}
	if (!m_bShowIndeterminate && m_vectComboToolTips[m_nCurrSelIndex].GetLength())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hComboBox);
	}
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hUniqueValuesButton);
	}
}
