//-----------------------------------------------------------------------------
//	File:		UIDisplayFreeColor.cpp
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayFreeColor class contains all the information needed to
//			display the proper ui control for a freecolorbox in max.  It does
//			not handle the actual drawing of the control or the message
//			handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplayFreeColor.h"

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplayFreeColor::UIDisplayFreeColor
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayFreeColor::UIDisplayFreeColor()
{
	m_colorCurrent.m_strName = "";
	m_colorCurrent.m_nRed = 0;
	m_colorCurrent.m_nGreen = 0;
	m_colorCurrent.m_nBlue = 0;
	m_hLabel = NULL;
	m_hFreeColorCtrl = NULL;
}


//-----------------------------------------------------------------------------
// UIDisplayFreeColor::~UIDisplayFreeColor
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayFreeColor::~UIDisplayFreeColor()
{
	if (m_hLabel != NULL)
		DestroyWindow(m_hLabel);
	if (m_hFreeColorCtrl != NULL)
		DestroyWindow(m_hFreeColorCtrl);
}


//-----------------------------------------------------------------------------
// UIDisplayFreeColor::SetCurrentColor
// This method will set the current color using the color passed in.
//
// Input:
//  AttribColor newColor -- The color to use when setting the current color.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayFreeColor::SetCurrentColor(AttribColor newColor)
{
	m_colorCurrent.m_strName = newColor.m_strName;
	m_colorCurrent.m_nRed = newColor.m_nRed;
	m_colorCurrent.m_nGreen = newColor.m_nGreen;
	m_colorCurrent.m_nBlue = newColor.m_nBlue;
}


//-----------------------------------------------------------------------------
// UIDisplayFreeColor::SetCurrentColor
// This method will set the current color using the color passed in.
//
// Input:
//  COLORREF newColor -- The color to use when setting the current color.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayFreeColor::SetCurrentColor(COLORREF newColor)
{
	m_colorCurrent.m_strName = "";
	m_colorCurrent.m_nRed = GetRValue(newColor);
	m_colorCurrent.m_nGreen = GetGValue(newColor);
	m_colorCurrent.m_nBlue = GetBValue(newColor);	
}


//-----------------------------------------------------------------------------
// UIDisplayFreeColor::SetControlID
// This method will set the control id for this freecolorbox.
//
// Input:
//  int nNewControlID -- The new control id to use.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayFreeColor::SetControlID(int nNewControlID)
{
	m_nControlID = nNewControlID;
}

//-----------------------------------------------------------------------------
// UIDisplayFreeColor::SetLabelID
// This method is a mutator for the m_nLabelID data member.
//
// Input:
//  int m_nLabelID -- new control id to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayFreeColor::SetLabelID(int nNewLabelID)
{
	m_nLabelID = nNewLabelID;
}

//-----------------------------------------------------------------------------
// UIDisplayFreeColor::GetCurrentColor
// This method will return the current color.
//
// Input: None.
//
// Output: AttribColor representing the current color.
//-----------------------------------------------------------------------------
AttribColor UIDisplayFreeColor::GetCurrentColor()
{
	return m_colorCurrent;
}


//-----------------------------------------------------------------------------
// UIDisplayFreeColor::GetControlID
// This method will return the control id for this freecolorbox.
//
// Input: None.
//
// Output: int representing the control id.
//-----------------------------------------------------------------------------
int UIDisplayFreeColor::GetControlID()
{
	return m_nControlID;
}


//-----------------------------------------------------------------------------
// UIDisplayFreeColor::BuildActualUI
// This method will set up and create the actual UI necessary to display a
// freecolor.
//
// Input:
//  HWND hParentDlg -- The HWND of the parent dialog.
//  HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The handle to the tooltip for the parent dialog.
//  HFONT hUIFont -- The font to use.
//  int nStartx -- The base x coord to use for the freecolor.  If the indent
//				   level for this control is > 0, then the freecolor will be
//				   drawn to the right of this x coord (based on the level of
//				   indentation).
//	int nStarty -- The y coord to draw the label at.
//  int nLabelWidth -- The width of the label for the freecolor.
//  int nLabelHeight -- The height of the label for the freecolor.
//	int& nControlSizeY -- Upon return, this value should represent the size that
//						  the control should take up in the Y direction.
//
// Output: True if succesfull, false otherwise.
//-----------------------------------------------------------------------------
bool UIDisplayFreeColor::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,  HWND hToolTip,
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

	// set up the unique values button
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		m_hUniqueValuesButton = CreateWindow(_T("button"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_ICON | nDisabledFlag,
			nStartx + nXOffset + size.cx + 3, nStarty + 2, 12, 12, hParentDlg, (HMENU) m_uniqueValuesID,
			hInstance, NULL);
		m_hUniqueValuesIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_UNIQUE_VALUES), IMAGE_ICON, 0, 0, 0);
		::SendMessage(m_hUniqueValuesButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hUniqueValuesIcon);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hUniqueValuesButton);
	}

	// Add to the control size in Y:
	nControlSizeY = 15;

	// Create the color swatch:
	m_hFreeColorCtrl = CreateWindow(_T("ColorSwatch"), NULL,
									WS_CHILD | WS_VISIBLE | nDisabledFlag,
									nStartx + nXOffset, nStarty + 15, 20, 20,
									hParentDlg, (HMENU) m_nControlID, hInstance, NULL);
#if 0  // TODO add the new color swatch to the attr controls library
	// Set up the color swatch:
	IColorSwatch* pColorSwatch = GetIColorSwatch(m_hFreeColorCtrl, RGB(m_colorCurrent.m_nRed, m_colorCurrent.m_nGreen, m_colorCurrent.m_nBlue),
												 const_cast<char*>(m_strLabel.data()));
	// Perform any calls to the color swatch here:
	pColorSwatch->SetModal();
	// Release the color swatch:
	ReleaseIColorSwatch(pColorSwatch);
#endif
	// Set the control size in Y:
	nControlSizeY += 20;

	// tell the tooltip about the controls
	if (m_strToolTip.GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hLabel);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hFreeColorCtrl);
	}

	m_nCenterY = nStarty + (nControlSizeY>>1);
	m_nTopY = nStarty;
	m_nBottomY = nStarty+nControlSizeY;

	return bResult;
}


//-----------------------------------------------------------------------------
// UIDisplayFreeColor::HandledWinMessage
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
bool UIDisplayFreeColor::HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
										   TupString& strKeyname, TupString& strValue,
										   TupArray<TupString>& vectKeysToPurge, int& nFlags)
{
	bool bResult = false;

#if 0  // TODO add the new color swatch to the attr controls library

	switch (message)
	{
	    case CC_COLOR_CHANGE:
			// Check to see if the id matches this object's control id:
			int nID = LOWORD(wParam);
			if (nID == m_nControlID)
			{
				// Get the colorswatch, and then get the value from it:
				IColorSwatch* pColorSwatch = (IColorSwatch*) lParam;
				this->SetCurrentColor(pColorSwatch->GetColor());
				// Set the keyname and value strings:
				strKeyname = m_strKeyname;
				strValue.Format("%d,%d,%d",m_colorCurrent.m_nRed,m_colorCurrent.m_nGreen,m_colorCurrent.m_nBlue);
				nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;

				// Set the exclusive flag:
				if (m_bExclusive)
					nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;

				bResult = true;
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
					 // if this is a toolbar message
					case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
					 {
						 ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;
						 if (pNotify->ti->m_hWnd == (UINT)m_hLabel ||
							 pNotify->ti->m_hWnd == (UINT)m_hLabel)
						 {
							 pNotify->ti->m_sTooltip = m_strToolTip;
							 bResult = true;
						 }
						 else if ((m_uniqueValues.m_uniqueValues.GetSize() && pNotify->ti->m_hWnd == (UINT)m_hUniqueValuesButton))
						 {
							 pNotify->ti->m_sTooltip = "Pick from selected values";
							 bResult = true;
						 }

					 }
					 break;
				 }
			 }
	}
#endif

	return bResult;
}

//-----------------------------------------------------------------------------
// UIDisplayFreeColor::RemoveToolTips
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
void UIDisplayFreeColor::RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip)
{
	if (m_strToolTip.GetLength())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hLabel);
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hFreeColorCtrl);
	}
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hUniqueValuesButton);
	}
}