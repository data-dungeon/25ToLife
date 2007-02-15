//-----------------------------------------------------------------------------
//	File:		UIDisplayLabel.cpp
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayLabel class contains all the information needed to
//			display the proper ui control for an editbox in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplayLabel.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplayLabel::UIDisplayLabel
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayLabel::UIDisplayLabel()
{
}


//-----------------------------------------------------------------------------
// UIDisplayLabel::~UIDisplayLabel
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayLabel::~UIDisplayLabel()
{
	for (int labelIndex=0;labelIndex<m_hLabels.GetSize();labelIndex++)
	{
		if (m_hLabels[labelIndex])
			DestroyWindow(m_hLabels[labelIndex]);
	}
}


//-----------------------------------------------------------------------------
// UIDisplayLabel::AddLabelString
// This method will set the label string to the control
//
// Input:
//  TupString newLabel -- The new label string
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayLabel::AddLabelString(const TupString &newLabel)
{
	m_labelStrings.Add(newLabel);
}

//-----------------------------------------------------------------------------
// UIDisplayLabel::AddLabelID
// This method will set the label id for this editbox.
//
// Input:
//  int nNewLabelID -- The new label id to use.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayLabel::AddLabelID(int nNewLabelID)
{
	m_nLabelIDs.Add(nNewLabelID);
}

//-----------------------------------------------------------------------------
// UIDisplayLabel::BuildActualUI
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
bool UIDisplayLabel::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
											 int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
											 int& nControlSizeY)
{
	bool bResult = true;

	// Figure out how far to indent:
	int nXOffset = (m_nIndentLevel * INDENT_SCALER) + m_nAddedIndent;

	// Create the static text label:
	// Figure out the necessary size of the label:
	m_hLabels.RemoveAll();
	nControlSizeY=0;
	for (int labelIndex=0;labelIndex<m_labelStrings.GetSize();labelIndex++)
	{
		SIZE size;
		HDC hdc = GetDC(hParentDlg);
		HFONT hFixedFont = CustomControls::GetFixedFont();
		HFONT hfntOld = (HFONT) SelectObject(hdc, hFixedFont);
		TEXTMETRIC tm;
		GetTextMetrics(hdc,&tm);
		int nHeight = tm.tmHeight; //The height of the font
		GetTextExtentPoint32(hdc, m_labelStrings[labelIndex], m_labelStrings[labelIndex].GetLength(), &size);
		SelectObject(hdc, hfntOld);
		ReleaseDC(hParentDlg, hdc);

		DWORD nDisabledFlag = GetDisabled() ? WS_DISABLED : 0;
		HWND hLabel = CreateWindow(_T("static"), m_labelStrings[labelIndex],
			WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOTIFY | nDisabledFlag,
			nStartx + nXOffset, nStarty + (nHeight * labelIndex), size.cx, nHeight,
			hParentDlg, (HMENU) m_nLabelIDs[labelIndex], hInstance, NULL);

		// Set its font:
		SendMessage(hLabel, WM_SETFONT, (WPARAM) hFixedFont, MAKELPARAM(TRUE,0));

		m_hLabels.Add(hLabel);

		// Add to the control size in Y:
		nControlSizeY += nHeight;

		// tell the tooltip about the controls
		if (m_strToolTip.GetLength())
		{
			AddToolTip(hParentDlg,hInstance,hToolTip,m_hLabels[labelIndex]);
		}
	}

	m_nCenterY = nStarty + (nControlSizeY>>1);
	m_nTopY = nStarty;
	m_nBottomY = nStarty+nControlSizeY;

	return bResult;
}


//-----------------------------------------------------------------------------
// UIDisplayLabel::HandledWinMessage
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
bool UIDisplayLabel::HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
												  TupString& strKeyname, TupString& strValue,
												  TupArray<TupString>& vectKeysToPurge, int& nFlags)
{
	bool bResult = false;

	switch (message)
	{
	case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR) lParam;
			switch (hdr->code)
			{
				// if this is a tooltip message
			case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
				{
					ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;
					for (int labelIndex=0;labelIndex<m_hLabels.GetSize();labelIndex++)
					{
						if (pNotify->ti->m_hWnd == m_hLabels[labelIndex])
						{
							pNotify->ti->m_sTooltip = m_strToolTip;
							bResult = true;
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
// UIDisplayLabel::RemoveToolTips
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
void UIDisplayLabel::RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip)
{
	if (m_strToolTip.GetLength())
	{
		for (int labelIndex=0;labelIndex<m_hLabels.GetSize();labelIndex++)
		{
			RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hLabels[labelIndex]);
		}
	}
}

//-----------------------------------------------------------------------------
// UIDisplayLabel::SetToolTip
// This method will set the tooltip for this UIDisplay object.
//
// Input:
//  TupString strToolTip -- ToolTip to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayLabel::SetLabelToolTip(const TupString &strToolTip)
{
	m_strToolTip = strToolTip;
}
