//-----------------------------------------------------------------------------
//	File:		UIDisplayRadioGroup.cpp
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayRadioGroup class contains all the information needed
//			to display the proper ui control for a radio group in max.  It does
//			not handle the actual drawing of the control or the message
//			handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplayRadioGroup.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#define RADIO_BUTTON_WIDTH	19
#define RADIO_BUTTON_HEIGHT 17
#define RADIO_BUTTON_Y_DISTANCE 17

using namespace AttrControls;


//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::UIDisplayRadioGroup
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayRadioGroup::UIDisplayRadioGroup()
{
	m_hLabel = NULL;
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::~UIDisplayRadioGroup
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayRadioGroup::~UIDisplayRadioGroup()
{
	if (m_hLabel != NULL)
		DestroyWindow(m_hLabel);
	int nNumCtrlHWNDs = m_vectCtrlHWNDs.GetSize();
	for (int nIndex = 0; nIndex < nNumCtrlHWNDs; nIndex++)
	{
		if (m_vectCtrlHWNDs[nIndex] != NULL)
			DestroyWindow(m_vectCtrlHWNDs[nIndex]);
	}
	m_vectCtrlHWNDs.RemoveAll();
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::SetSelectionIndex
// This method will set the index for the current selection for this radiogroup.
//
// Input:
//  int nNewSelIndex -- New index for current selection.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayRadioGroup::SetSelectionIndex(int nNewSelIndex)
{
	m_nCurrSelIndex = nNewSelIndex;
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::AddRadioLabel
// This method will add a radio label for this radiogroup to the vector of
// labels.
//
// Input:
//  TupString strLabel -- String to add as a label.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayRadioGroup::AddRadioLabel(const TupString &strLabel)
{
	m_vectRadioLabels.Add(strLabel);
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::AddRadioKey
// This method will add a radio key for this radiogroup to the vector of keys.
//
// Input:
//  TupString strToAdd -- String to add as a key.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayRadioGroup::AddRadioKey(const TupString &strKey)
{
	m_vectRadioKeys.Add(strKey);
}

//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::AddRadioToolTip
// This method will add a radio tooltip for this radiogroup to the vector of keys.
//
// Input:
//  TupString strToAdd -- String to add as a tooltip.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayRadioGroup::AddRadioToolTip(const TupString &strLabel,const TupString &strKey,const TupString &strToolTip)
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
	m_vectRadioToolTips.Add(newString);
}

//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::AddControlID
// This method will add a control id for one of the buttons in this ragiogroup.
//
// Input:
//  int nNewID -- The new id to add.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayRadioGroup::AddControlID(int nNewID)
{
	m_vectControlIDs.Add(nNewID);
}

//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::SetLabelID
// This method is a mutator for the m_nLabelID data member.
//
// Input:
//  int m_nLabelID -- new control id to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayRadioGroup::SetLabelID(int nNewLabelID)
{
	m_nLabelID = nNewLabelID;
}

//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::AddPurgeKey
// This method will add a key string to the vector of keys to purge.
//
// Input:
//  TupString strKeyToPurge -- String to add as a key to purge.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayRadioGroup::AddPurgeKey(TupString strKeyToPurge)
{
	m_vectKeysToPurge.Add(strKeyToPurge);
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::GetSelectionIndex
// This method will get the current selection index for this radiogroup.
//
// Input: None.
//
// Output: int representing the current selection index.
//-----------------------------------------------------------------------------
int UIDisplayRadioGroup::GetSelectionIndex()
{
	return m_nCurrSelIndex;
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::GetRadioLabel
// This method will return the radio label at the index specified.
//
// Input:
//  int nIndex -- The index of the radio label to get.
//
// Output: TupString of the radio label.
//-----------------------------------------------------------------------------
const TupString &UIDisplayRadioGroup::GetRadioLabel(int nIndex)
{
	int nNumLabels = m_vectRadioLabels.GetSize();
	if (nIndex < nNumLabels)
	{
		return m_vectRadioLabels[nIndex];
	}
	else
	{
		static TupString strReturn = "";
		return strReturn;
	}
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::GetRadioKey
// This method will return the radio key at the index specified.
//
// Input:
//  int nIndex -- The index of the radio key to get.
//
// Output: TupString of the radio key.
//-----------------------------------------------------------------------------
const TupString &UIDisplayRadioGroup::GetRadioKey(int nIndex)
{
	int nNumKeys = m_vectRadioKeys.GetSize();
	if (nIndex < nNumKeys)
	{
		return m_vectRadioKeys[nIndex];
	}
	else
	{
		static TupString strReturn = "";
		return strReturn;
	}
}

//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::GetRadioToolTip
// This method will return the radio key at the index specified.
//
// Input:
//  int nIndex -- The index of the radio key to get.
//
// Output: TupString of the radio key.
//-----------------------------------------------------------------------------
const TupString &UIDisplayRadioGroup::GetRadioToolTip(int nIndex)
{
	int nNumKeys = m_vectRadioKeys.GetSize();
	if (nIndex < nNumKeys)
	{
		return m_vectRadioToolTips[nIndex];
	}
	else
	{
		static TupString strReturn = "";
		return strReturn;
	}
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::GetControlID
// This method will return the control id at the index specified.
//
// Input:
//  int nIndex -- The index of the control id to get.
//
// Output: int representing the control id.
//-----------------------------------------------------------------------------
int UIDisplayRadioGroup::GetControlID(int nIndex)
{
	int nNumIDs = m_vectControlIDs.GetSize();
	if (nIndex < nNumIDs)
		return m_vectControlIDs[nIndex];
	else
		return -1;
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::IdInGroupAt
// This method will look at the m_vectControlIDs to see if the id passed in is
// contained in the vector.  If it is, the index of the id is passed back.
//
// Input:
//  int nIdToCheck -- The id to look for.
//
// Output: int of the index for the id.  Otherwise -1 is returned.
//-----------------------------------------------------------------------------
int UIDisplayRadioGroup::IdInGroupAt(int nIdToCheck)
{
	int nIdIndex = -1;

	int nNumIds = m_vectControlIDs.GetSize();
	for (int nIndex = 0; nIndex < nNumIds; nIndex++)
	{
		if (m_vectControlIDs[nIndex] == nIdToCheck)
		{
			nIdIndex = nIndex;
			break;
		}
	}

	return nIdIndex;
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::BuildActualUI
// This method will set up and create the actual UI necessary to display a
// radiogroup.
//
// Input:
//  HWND hParentDlg -- The HWND of the parent dialog.
//  HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The handle to the tooltip for the parent dialog.
//  HFONT hUIFont -- The font to use.
//  int nStartx -- The base x coord to use for radiogroup.  If the indent level
//				   for this control is > 0, then the radiogroup will be drawn
//				   to the right of this x coord (based on the level of
//				   indentation).
//	int nStarty -- The y coord to start the radiogroup at.
//  int nLabelWidth -- The width of the labels for the radiogroup.
//  int nLabelHeight -- The height of the labels for the radiogroup.
//	int& nControlSizeY -- Upon return, this value should represent the size that
//						  the control should take up in the Y direction.
//
// Output: True if succesfull, false otherwise.
//-----------------------------------------------------------------------------
bool UIDisplayRadioGroup::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
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
	SIZE size;
	GetTextExtentPoint32(hdc, m_strLabel, m_strLabel.GetLength(), &size);
	SelectObject(hdc, hfntOld);
	ReleaseDC(hParentDlg, hdc);
	DWORD nDisabledFlag = GetDisabled() ? WS_DISABLED : 0;
	m_hLabel = CreateWindow(_T("static"), m_strLabel,
							WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOTIFY | nDisabledFlag,
							nStartx + nXOffset, nStarty, size.cx+1, nLabelHeight,
							hParentDlg, (HMENU) m_nLabelID, hInstance, NULL);
	// Set its font:
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM) hLabelFont, MAKELPARAM(TRUE,0));

	// set up the unique values button
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		m_hUniqueValuesButton = CreateWindow(_T("button"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_ICON | nDisabledFlag,
			nStartx + nXOffset + size.cx + 3, nStarty +1, 12, 12, hParentDlg, (HMENU) m_uniqueValuesID,
			hInstance, NULL);
		m_hUniqueValuesIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_UNIQUE_VALUES), IMAGE_ICON, 0, 0, 0);
		::SendMessage(m_hUniqueValuesButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hUniqueValuesIcon);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hUniqueValuesButton);
	}

	// Set the control size in Y:
	nControlSizeY = nLabelHeight;

	// tell the tooltip about the label
	if (m_strToolTip.GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hLabel);
	}

	// Create the radio buttons:
	// Create the first one (this one has the WS_GROUP set):
	// Figure out the necessary size of the radio button:
	hdc = GetDC(hParentDlg);
	hfntOld = (HFONT) SelectObject(hdc, hFixedFont);
	GetTextExtentPoint32(hdc, m_vectRadioLabels[0], m_vectRadioLabels[0].GetLength(), &size);
	SelectObject(hdc, hfntOld);
	ReleaseDC(hParentDlg, hdc);
	HWND hRadioButton = CreateWindow(_T("button"), m_vectRadioLabels[0],
									 WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | BS_AUTORADIOBUTTON | nDisabledFlag,
									 nStartx + nXOffset, nStarty + nLabelHeight, RADIO_BUTTON_WIDTH + size.cx, RADIO_BUTTON_HEIGHT,
									 hParentDlg, (HMENU) m_vectControlIDs[0],
									 hInstance, NULL);

	SendMessage(hRadioButton, WM_SETFONT, (WPARAM) hFixedFont, MAKELPARAM(TRUE,0));
	
	m_vectCtrlHWNDs.Add(hRadioButton);
	// Add to the control size:
	nControlSizeY += RADIO_BUTTON_Y_DISTANCE;

	// tell the tooltip about the first radio button
	if (m_vectRadioToolTips[0].GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,hRadioButton);
	}

	int nButtonY = nStarty + nLabelHeight;
	// Now create the others (without the WS_GROUP style):
	int nNumSelections = m_vectRadioLabels.GetSize();
	for (int nIndex = 1; nIndex < nNumSelections; nIndex++)
	{
		nButtonY += RADIO_BUTTON_Y_DISTANCE;

		// Figure out the necessary size of the radio button:
		hdc = GetDC(hParentDlg);
		hfntOld = (HFONT) SelectObject(hdc, hFixedFont);
		GetTextExtentPoint32(hdc, m_vectRadioLabels[nIndex], m_vectRadioLabels[nIndex].GetLength(), &size);
		SelectObject(hdc, hfntOld);
		ReleaseDC(hParentDlg, hdc);

		HWND hRadioBttn = CreateWindow(_T("button"), m_vectRadioLabels[nIndex],
									   WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON | nDisabledFlag,
									   nStartx + nXOffset, nButtonY, RADIO_BUTTON_WIDTH + size.cx, RADIO_BUTTON_HEIGHT,
									   hParentDlg, (HMENU) m_vectControlIDs[nIndex],
									   hInstance, NULL);

		SendMessage(hRadioBttn, WM_SETFONT, (WPARAM) hFixedFont, MAKELPARAM(TRUE,0));
		
		m_vectCtrlHWNDs.Add(hRadioBttn);
		// Add to the control size:
		nControlSizeY += RADIO_BUTTON_Y_DISTANCE;

		// tell the tooltip about the radio button
		if (m_vectRadioToolTips[nIndex].GetLength())
		{
			AddToolTip(hParentDlg,hInstance,hToolTip,hRadioBttn);
		}
	}
	// Set the radio group's state:
	if (m_nCurrSelIndex >= 0)
		CheckRadioButton(hParentDlg, m_vectControlIDs[0], m_vectControlIDs[m_vectControlIDs.GetSize() - 1], m_vectControlIDs[m_nCurrSelIndex]);

	// Adjust the control size in Y to make things look better:
//	nControlSizeY -= 4;

	m_nCenterY = nStarty + (nLabelHeight - 1) + ((nNumSelections*RADIO_BUTTON_Y_DISTANCE)>>1);
	m_nTopY = nStarty;
	m_nBottomY = nStarty+nControlSizeY;

	return bResult;
}


//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::HandledWinMessage
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
bool UIDisplayRadioGroup::HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
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
				// Check for a radio button pressed:
				case BN_CLICKED:
				{
					// See if the control id matches any of this group's ids:
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
					else
					{
						int nIdIndexAt = this->IdInGroupAt(nID);
						if (nIdIndexAt > -1)
						{
							// Get its state:
							int nRadioState = SendMessage(m_vectCtrlHWNDs[nIdIndexAt], BM_GETSTATE, 0, 0);
							if ((nRadioState & 0x0003) == BST_CHECKED)
							{
								this->SetSelectionIndex(nIdIndexAt);
								// Load up any keys to purge:
								int nNumPurgeKeys = m_vectKeysToPurge.GetSize();
								for (int nIndex = 0; nIndex < nNumPurgeKeys; nIndex++)
								{
									vectKeysToPurge.Add(m_vectKeysToPurge[nIndex]);
									nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_PURGE_KEYS;
								}
								// Set the key and val:
								strKeyname = m_strKeyname;
								strValue = m_vectRadioKeys[nIdIndexAt];
								nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;
								// Set the exclusive flag:
								if (m_bExclusive)
									nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;
								bResult = true;
							}
							else
							{
								// Shouldn't get to here!
							}
						}
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
						 if (pNotify->ti->m_hWnd == m_hLabel)
						 {
							 pNotify->ti->m_sTooltip = m_strToolTip;
							 bResult = true;
						 }
						 else if ((m_uniqueValues.m_uniqueValues.GetSize() && pNotify->ti->m_hWnd == m_hUniqueValuesButton))
						 {
							 pNotify->ti->m_sTooltip = "Pick from selected values";
							 bResult = true;
						 }
						 else
						 {
							 int nNumSelections = m_vectRadioLabels.GetSize();
							 for (int nIndex = 0; nIndex < nNumSelections; nIndex++)
							 {
								 if (pNotify->ti->m_hWnd == m_vectCtrlHWNDs[nIndex])
								 {
									 pNotify->ti->m_sTooltip.Format("%s\n%s",m_strToolTip,m_vectRadioToolTips[nIndex]);
									 bResult = true;
								 }
							 }
						 }
					 }
					 break;
				 }
			 }

	}

	return bResult;
}

//-----------------------------------------------------------------------------
// UIDisplayRadioGroup::RemoveToolTips
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
void UIDisplayRadioGroup::RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip)
{
	if (m_strToolTip.GetLength())
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hLabel);

	int nNumSelections = m_vectRadioLabels.GetSize();
	for (int nIndex = 0; nIndex < nNumSelections; nIndex++)
	{
		if (m_vectRadioToolTips[nIndex].GetLength())
		{
			RemoveToolTip(hParentWnd,hInstance,hToolTip,m_vectCtrlHWNDs[nIndex]);
		}
	}
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hUniqueValuesButton);
	}
}
