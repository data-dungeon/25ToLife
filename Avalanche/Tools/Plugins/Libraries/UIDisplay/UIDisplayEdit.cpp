//-----------------------------------------------------------------------------
//	File:		UIDisplayEdit.cpp
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayEdit class contains all the information needed to
//			display the proper ui control for an editbox in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplayEdit.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#define EDIT_CONTROL_MIN_WIDTH	52
#define EDIT_CONTROL_MAX_WIDTH	200
#define BETWEEN_EDIT_AND_FRAME	4


using namespace AttrControls;

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplayEdit::UIDisplayEdit
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayEdit::UIDisplayEdit()
{
	m_strCurrText = "";
	m_type = ConfigEditBox::STRING;
	m_hLabel = NULL;
	m_hCustEditCtrl = NULL;
}


//-----------------------------------------------------------------------------
// UIDisplayEdit::~UIDisplayEdit
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayEdit::~UIDisplayEdit()
{
	if (m_hLabel != NULL)
		DestroyWindow(m_hLabel);
	if (m_hCustEditCtrl != NULL)
		DestroyWindow(m_hCustEditCtrl);
}


//-----------------------------------------------------------------------------
// UIDisplayEdit::SetCurrText
// This method will set the current text for this editbox.
//
// Input:
//  TupString strCurrText -- Text to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayEdit::SetCurrText(TupString strCurrText)
{
	m_strCurrText = strCurrText;
}


//-----------------------------------------------------------------------------
// UIDisplayEdit::SetType
// This method will set the current type for this editbox.
//
// Input:
//  ConfigEditBox::EditBoxType newType -- Type to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayEdit::SetType(ConfigEditBox::EditBoxType newType)
{
	m_type = newType;
}


//-----------------------------------------------------------------------------
// UIDisplayEdit::SetLabelID
// This method will set the label id for this editbox.
//
// Input:
//  int nNewLabelID -- The new label id to use.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayEdit::SetLabelID(int nNewLabelID)
{
	m_nLabelID = nNewLabelID;
}

//-----------------------------------------------------------------------------
// UIDisplayEdit::SetControlID
// This method will set the control id for this editbox.
//
// Input:
//  int nNewControlID -- The new control id to use.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayEdit::SetControlID(int nNewControlID)
{
	m_nControlID = nNewControlID;
}


//-----------------------------------------------------------------------------
// UIDisplayEdit::GetCurrText
// This method will get the current text for this editbox.
//
// Input: None.
//
// Output: TupString representing the current text.
//-----------------------------------------------------------------------------
TupString UIDisplayEdit::GetCurrText()
{
	return m_strCurrText;
}


//-----------------------------------------------------------------------------
// UIDisplayEdit::GetType
// This method will get the current type of this editbox.
//
// Input: None.
//
// Output: ConfigEditBox::EditBoxType representing the current type.
//-----------------------------------------------------------------------------
ConfigEditBox::EditBoxType UIDisplayEdit::GetType()
{
	return m_type;
}


//-----------------------------------------------------------------------------
// UIDisplayEdit::GetControlID
// This method will return the control id for this editbox.
//
// Input: None.
//
// Output: int representing the control id.
//-----------------------------------------------------------------------------
int UIDisplayEdit::GetControlID()
{
	return m_nControlID;
}

//-----------------------------------------------------------------------------
// UIDisplayEdit::GetLabelID
// This method will return the label id for this editbox.
//
// Input: None.
//
// Output: int representing the label id.
//-----------------------------------------------------------------------------
int UIDisplayEdit::GetLabelID()
{
	return m_nLabelID;
}


//-----------------------------------------------------------------------------
// UIDisplayEdit::BuildActualUI
// This method will set up and create the actual UI necessary to display an
// editbox.
//
// Input:
//  HWND hParentDlg -- The HWND of the parent dialog.
//  HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The handle to the tooltip for the parent dialog.
//  HFONT hUIFont -- The font to use.
//  int nStartx -- The base x coord to use for editbox.  If the indent level
//				   for this control is > 0, then the editbox will be drawn to
//				   the right of this x coord (based on the level of
//				   indentation).
//	int nStarty -- The y coord to start the label at.
//  int nLabelWidth -- The width of the label for the editbox.
//  int nLabelHeight -- The height of the label for the editbox.
//	int& nControlSizeY -- Upon return, this value should represent the size that
//						  the control should take up in the Y direction.
//
// Output: True if succesfull, false otherwise.
//-----------------------------------------------------------------------------
bool UIDisplayEdit::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
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
	SIZE size;
	GetTextExtentPoint32(hdc, m_strLabel, m_strLabel.GetLength(), &size);
	SelectObject(hdc, hfntOld);
	ReleaseDC(hParentDlg, hdc);
	DWORD nDisabledFlag = GetDisabled() ? WS_DISABLED : 0;
	m_hLabel = CreateWindow(_T("static"), m_strLabel,
							WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOTIFY | nDisabledFlag,
							nStartx + nXOffset, nStarty, size.cx, nLabelHeight,
							hParentDlg, (HMENU) m_nLabelID, hInstance, NULL);
	// Set its font:
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM) hFixedFont, MAKELPARAM(TRUE,0));

	// Add to the control size in Y:
	nControlSizeY = 15;

	// Create the custom editbox:
	// Figure out the width:
	int nControlWidth = EDIT_CONTROL_MIN_WIDTH;
	RECT parentRect;
	GetClientRect(hParentDlg, &parentRect);
	int nParentWidth = parentRect.right - parentRect.left;
	int nControlX = nStartx + nXOffset;
	if ((nControlX + EDIT_CONTROL_MIN_WIDTH) > nParentWidth ||
		(nControlX + EDIT_CONTROL_MAX_WIDTH) < nParentWidth)
	{
		// Parent dialog is either too small to allow for the min width of the
		// edit box, or it is large enough to handle the max width.  In either
		// case, just use the max width of the edit box:
		nControlWidth = EDIT_CONTROL_MAX_WIDTH;
	}
	else
	{
		// The edit box needs to be sized correctly:
		nControlWidth = (nParentWidth - nControlX) - BETWEEN_EDIT_AND_FRAME;
	}

	m_hCustEditCtrl = CreateWindow(CustomControls::GetWindowClassName(CustomControls::EDIT_WINDOW_CLASS), NULL,
								  WS_CHILD | WS_VISIBLE | WS_TABSTOP | nDisabledFlag,
								  nControlX, nStarty + 15, nControlWidth, 20,
								  hParentDlg, (HMENU) m_nControlID, hInstance, NULL);

	// Initialize a pointer to the control to call methods on it:
	ICustomEditBox* pCustEdit = ICustomEditBox::GetICustomEditBox(m_hCustEditCtrl);
	// For some reason we need to bump the text down 2 pixels:
	pCustEdit->SetLeading(2);
	// Set the text:
	pCustEdit->SetText(m_strCurrText);
	m_hChildEditCtrl = pCustEdit->GetHEdit();
	// Release the custom edit:
	ICustomEditBox::ReleaseICustomEditBox(pCustEdit);

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

	// Add to the control size in Y:
	nControlSizeY += 20;

	// tell the tooltip about the controls
	if (m_strToolTip.GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hLabel);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hCustEditCtrl);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hChildEditCtrl);
	}

	m_nCenterY = nStarty + 24;
	m_nTopY = nStarty;
	m_nBottomY = nStarty+nControlSizeY;

	return bResult;
}


//-----------------------------------------------------------------------------
// UIDisplayEdit::HandledWinMessage
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
//	int& nFlags -- Any flags that need to be returned to the calling method.
//
// Output: True if this object handled the message, false otherwise.  Also, see
//		   above for the two string parameters that are also used as return values.
//-----------------------------------------------------------------------------
bool UIDisplayEdit::HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
									  TupString& strKeyname, TupString& strValue,
									  TupArray<TupString>& vectKeysToPurge, int& nFlags)
{
	bool bResult = false;

	switch (message)
	{
		// Check for the focus leaving an edit box:
		case WM_ATCUSTOMEDIT_ENTER:
		{
			// See if the control id matches this object's id:
			int nID = LOWORD(wParam);
			if (nID == m_nControlID)
			{
				// Get the custom edit pointer:
				ICustomEditBox* pCustEdit = ICustomEditBox::GetICustomEditBox(m_hCustEditCtrl);
				char chBuff[255];
				pCustEdit->GetText(chBuff, 255);
				TupString strEditText = chBuff;
				if (strEditText != m_strCurrText)
				{
					// Set the current text:
					this->SetCurrText(strEditText);
					// Set the keyname and value:
					strKeyname = m_strKeyname;
					strValue = strEditText;
					ICustomEditBox::ReleaseICustomEditBox(pCustEdit);
					nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;
					// Set the exclusive flag:
					if (m_bExclusive)
						nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;
					bResult = true;
				}
			}
			break;
		}
		break;
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
		case WM_NOTIFY:
			{
				LPNMHDR hdr = (LPNMHDR) lParam;
				switch (hdr->code)
				{
					// if this is a tooltip message
					case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
					{
						ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;
						if (pNotify->ti->m_hWnd == m_hLabel ||
							 pNotify->ti->m_hWnd == m_hCustEditCtrl ||
							 pNotify->ti->m_hWnd == m_hChildEditCtrl)
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

	}

	return bResult;
}

//-----------------------------------------------------------------------------
// UIDisplayEdit::RemoveToolTips
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
void UIDisplayEdit::RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip)
{
	if (m_strToolTip.GetLength())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hLabel);
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hCustEditCtrl);
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hChildEditCtrl);
	}
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hUniqueValuesButton);
	}
}
