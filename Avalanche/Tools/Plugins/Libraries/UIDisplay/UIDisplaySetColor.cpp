//-----------------------------------------------------------------------------
//	File:		UIDisplaySetColor.cpp
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplaySetColor class contains all the information needed to
//			display the proper ui control for a setcolorbox in max.  It does
//			not handle the actual drawing of the control or the message
//			handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplaySetColor.h"
#include "FuncPtrController.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#define SWATCH_WIDTH				20
#define SWATCH_HEIGHT				20
#define BETWEEN_INDCTR_AND_CHOICE	10
#define BETWEEN_CHOICES				2
#define MIN_NUM_SWATCHES_PER_ROW	3
#define MAX_NUM_SWATCHES_PER_ROW	8

using namespace AttrControls;


//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplaySetColor::UIDisplaySetColor
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplaySetColor::UIDisplaySetColor()
{
	m_nCurrSelIndex = 0;
	m_hLabel = NULL;
	m_hCurrColorCtrl = NULL;
	m_bShowIndeterminate = false;
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::~UIDisplaySetColor
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplaySetColor::~UIDisplaySetColor()
{
	if (m_hLabel != NULL)
		DestroyWindow(m_hLabel);
	if (m_hCurrColorCtrl != NULL)
		DestroyWindow(m_hCurrColorCtrl);
	int nNumCtrlHWNDs = m_vectColorChoiceHWNDs.GetSize();
	for (int nIndex = 0; nIndex < nNumCtrlHWNDs; nIndex++)
	{
		if (m_vectColorChoiceHWNDs[nIndex] != NULL)
			DestroyWindow(m_vectColorChoiceHWNDs[nIndex]);
	}
	m_vectColorChoiceHWNDs.RemoveAll();
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::SetSelectionIndex
// This method will set the index for the current selection for this
// setcolorbox.
//
// Input:
//  int nNewSelIndex -- New index for current selection.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplaySetColor::SetSelectionIndex(int nNewSelIndex)
{
	m_nCurrSelIndex = nNewSelIndex;
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::AddColor
// This method will add a color to the color choices for this setcolor control.
//
// Input:
//  AttribColor newColor -- New color to add.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplaySetColor::AddColor(AttribColor newColor)
{
	m_vectColors.Add(newColor);
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::SetCurrColorID
// This method will set the current color control id for this setcolorbox.
//
// Input:
//  int nNewID -- The new id to use.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplaySetColor::SetCurrColorID(int nNewID)
{
	m_nCurrColorID = nNewID;
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::AddChoiceCtrlID
// This method will add a control id for one of the color controls that
// represent the color choices for this setcolorbox.
//
// Input:
//  int nNewID -- The new control id to add.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplaySetColor::AddChoiceCtrlID(int nNewID)
{
	m_vectControlIDs.Add(nNewID);
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::SetShowIndeterminate
// This method will set the m_bShowIndeterminate data member.
//
// Input:
//  bool bShow -- Pass in true if the indeterminate state should be shown in
//				  the control, false otherwise.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplaySetColor::SetShowIndeterminate(bool bShow)
{
	m_bShowIndeterminate = bShow;
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::GetSelectionIndex
// This method will get the current selection index for this setcolorbox.
//
// Input: None.
//
// Output: int representing the current selection index.
//-----------------------------------------------------------------------------
int UIDisplaySetColor::GetSelectionIndex()
{
	return m_nCurrSelIndex;
}

//-----------------------------------------------------------------------------
// UIDisplaySetColor::GetColor
// This method will return the color at the index specified.
//
// Input:
//  int nIndex -- The index of the color to get.
//
// Output: AttribColor representing the color to get.
//-----------------------------------------------------------------------------
AttribColor UIDisplaySetColor::GetColor(int nIndex)
{
	int nNumColors = m_vectColors.GetSize();
	if (nIndex >=0 && nIndex < nNumColors)
	{
		return m_vectColors[nIndex];
	}
	else
	{
		AttribColor colorReturn;
		colorReturn.m_strName = "";
		colorReturn.m_nRed = 0;
		colorReturn.m_nGreen = 0;
		colorReturn.m_nBlue = 0;
		return colorReturn;
	}
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::GetCurrColorID
// This method will return the control id for the control representing the
// current color choice.
//
// Input: None.
//
// Output: int representing the control id.
//-----------------------------------------------------------------------------
int UIDisplaySetColor::GetCurrColorID()
{
	return m_nCurrColorID;
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::GetChoiceCtrlID
// This method will return the control id for the control representing the
// color choice control at the index specified.
//
// Input:
//  int nIndex -- The index of the control id to get.
//
// Output: int representing the control id.
//-----------------------------------------------------------------------------
int UIDisplaySetColor::GetChoiceCtrlID(int nIndex)
{
	int nNumControlIDs = m_vectControlIDs.GetSize();
	if (nIndex >= 0 && nIndex < nNumControlIDs)
		return m_vectControlIDs[nIndex];
	else
		return -1;
}

//-----------------------------------------------------------------------------
// UIDisplaySetColor::SetLabelID
// This method will set the label id for this editbox.
//
// Input:
//  int nNewLabelID -- The new label id to use.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplaySetColor::SetLabelID(int nNewLabelID)
{
	m_nLabelID = nNewLabelID;
}

//-----------------------------------------------------------------------------
// UIDisplaySetColor::BuildActualUI
// This method will set up and create the actual UI necessary to display a
// setcolor.
//
// Input:
//  HWND hParentDlg -- The HWND of the parent dialog.
//  HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The handle to the tooltip for the parent dialog.
//  HFONT hUIFont -- The font to use.
//  int nStartx -- The base x coord to use for the setcolor.  If the indent
//				   level for this control is > 0, then the setcolor will be
//				   drawn to the right of this x coord (based on the level of
//				   indentation).
//	int nStarty -- The y coord to start the setcolor at.
//  int nLabelWidth -- The width of the label for the setcolor.
//  int nLabelHeight -- The height of the label for the setcolor.
//	int& nControlSizeY -- Upon return, this value should represent the size that
//						  the control should take up in the Y direction.
//
// Output: True if succesfull, false otherwise.
//-----------------------------------------------------------------------------
bool UIDisplaySetColor::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance, HWND hToolTip,
									  int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
									  int& nControlSizeY)
{
	bool bResult = true;

	// Figure out how far to indent:
	int nXOffsetClrIndicator = (m_nIndentLevel * INDENT_SCALER) + m_nAddedIndent;
	int nXOffsetClrChoice = nXOffsetClrIndicator + SWATCH_WIDTH + BETWEEN_INDCTR_AND_CHOICE;

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
							nStartx + nXOffsetClrIndicator, nStarty, size.cx, nLabelHeight,
							hParentDlg, (HMENU) m_nLabelID, hInstance, NULL);
	// Set its font:
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM) hFixedFont, MAKELPARAM(TRUE,0));

	// set up the unique values button
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		m_hUniqueValuesButton = CreateWindow(_T("button"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_ICON | nDisabledFlag,
			nStartx + nXOffsetClrIndicator + size.cx + 3, nStarty + 2, 12, 12, hParentDlg, (HMENU) m_uniqueValuesID,
			hInstance, NULL);
		m_hUniqueValuesIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_UNIQUE_VALUES), IMAGE_ICON, 0, 0, 0);
		::SendMessage(m_hUniqueValuesButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hUniqueValuesIcon);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hUniqueValuesButton);
	}

	// Set the control size in Y:
	nControlSizeY = 15;

	// tell the tooltip about the label
	if (m_strToolTip.GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hLabel);
	}

	// Create and set up the color swatch that indicates the currently selected color:
	m_hCurrColorCtrl = CreateWindow(_T("ColorSwatch"), NULL,
									WS_CHILD | WS_VISIBLE | nDisabledFlag,
									nStartx + nXOffsetClrIndicator, nStarty + nControlSizeY, SWATCH_WIDTH, SWATCH_HEIGHT,
									hParentDlg, (HMENU) m_nCurrColorID, hInstance, NULL);
	int nRed, nGreen, nBlue;
	if (m_bShowIndeterminate == false)
	{
		nRed = m_vectColors[m_nCurrSelIndex].m_nRed;
		nGreen = m_vectColors[m_nCurrSelIndex].m_nGreen;
		nBlue = m_vectColors[m_nCurrSelIndex].m_nBlue;
	}
	else
	{
		nRed = 0;
		nGreen = 0;
		nBlue = 0;		
	}
#if 0 // TODO add color swatch
	IColorSwatch* pSetColorSwatch = GetIColorSwatch(m_hCurrColorCtrl, RGB(nRed, nGreen, nBlue), _T(""));
	ReleaseIColorSwatch(pSetColorSwatch);
#endif
	// Now subclass the swatch:
	WNDPROC origProc;
	origProc = (WNDPROC) SetWindowLongPtr(m_hCurrColorCtrl, GWLP_WNDPROC, (LONG_PTR) theFuncPtrController.GetNewSwatchProc());
	theFuncPtrController.SetOrigSwatchProc(origProc);

	// tell the tooltip about the current color box
	if (m_strToolTip.GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hCurrColorCtrl);
	}

	// Create and set up the color swatches for the color choices:
	// Find the width of the parent dialog:
	RECT parentRect;
	GetClientRect(hParentDlg, &parentRect);
	int nParentWidth = parentRect.right - parentRect.left;
	// Find out how many swatches can fit per row (with min of 3, max of 8):
	int nNumSwatchesPerRow = MIN_NUM_SWATCHES_PER_ROW;
	int nXToTest = nStartx + nXOffsetClrChoice;
	// Adjust x for the 3 swatches:
	nXToTest += ((MIN_NUM_SWATCHES_PER_ROW * SWATCH_WIDTH) + (MIN_NUM_SWATCHES_PER_ROW * BETWEEN_CHOICES));
	// Adjust for one more swatch
	nXToTest += (SWATCH_WIDTH + BETWEEN_CHOICES);
	while (nXToTest < nParentWidth)
	{
		nNumSwatchesPerRow++;
		if (nNumSwatchesPerRow == MAX_NUM_SWATCHES_PER_ROW)
			break;
		nXToTest += (SWATCH_WIDTH + BETWEEN_CHOICES);				// Adjust for one more swatch
	}
	// Now draw the color choices:
	int nXSwatchAt = nStartx + nXOffsetClrChoice;
	int nYSwatchAt = nStarty + nControlSizeY;
	// Adjust the control size in y to account for the first row:
	nControlSizeY += (SWATCH_HEIGHT + BETWEEN_CHOICES);
	int nNumChoices = m_vectColors.GetSize();
	for (int nIndex = 0; nIndex < nNumChoices; nIndex++)
	{
		HWND hColorChoice = CreateWindow(_T("ColorSwatch"), NULL,
										 WS_CHILD | WS_VISIBLE | nDisabledFlag,
										 nXSwatchAt, nYSwatchAt, SWATCH_WIDTH, SWATCH_HEIGHT,
										 hParentDlg, (HMENU) m_vectControlIDs[nIndex], hInstance, NULL);
		// Set the rgb for this swatch:
		int nRed = m_vectColors[nIndex].m_nRed;
		int nGreen = m_vectColors[nIndex].m_nGreen;
		int nBlue = m_vectColors[nIndex].m_nBlue;
#if 0 // TODO add color swatch
		pSetColorSwatch = GetIColorSwatch(hColorChoice, RGB(nRed, nGreen, nBlue), _T(""));
		ReleaseIColorSwatch(pSetColorSwatch);
#endif
		// Add the HWND to the HWND vector:
		m_vectColorChoiceHWNDs.Add(hColorChoice);
		// Subclass the swatch:
		origProc = (WNDPROC) SetWindowLongPtr(hColorChoice, GWLP_WNDPROC, (LONG_PTR) theFuncPtrController.GetNewSwatchProc());
		theFuncPtrController.SetOrigSwatchProc(origProc);
		// Find the correct position for the next color swatch:
		int nRemainder = (nIndex + 1) % nNumSwatchesPerRow;
		if (nRemainder == 0)
		{
			// Move back to the beginning in x, and down in y:
			nXSwatchAt = nStartx + nXOffsetClrChoice;
			if ((nIndex + 1) < nNumChoices)
			{
				nYSwatchAt = nStarty + nControlSizeY;
				nControlSizeY += (SWATCH_HEIGHT + BETWEEN_CHOICES);
			}
		}
		else
		{
			// Move to the right in x:
			nXSwatchAt += (SWATCH_WIDTH + BETWEEN_CHOICES);
		}

		// tell the tooltip about the swatch
		if (m_strToolTip.GetLength())
		{
			AddToolTip(hParentDlg,hInstance,hToolTip,hColorChoice);
		}
	}

	// Adjust the control size in Y to look better:
	nControlSizeY -= 2;

	m_nCenterY = nStarty + (nControlSizeY>>1);
	m_nTopY = nStarty;
	m_nBottomY = nStarty+nControlSizeY;

	return bResult;
}


//-----------------------------------------------------------------------------
// UIDisplaySetColor::HandledWinMessage
// This method will look at the incoming message and decide if this object
// needs to handle the message, and if so, how it does it.
//
// Input:
//  HWND hColorSwatch -- The HWND of the color swatch.
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
// int& nFlags -- Any flags that need to be returned to the calling method.
//
// Output: True if this object handled the message, false otherwise.  Also, see
//		   above for the two string parameters that are also used as return values.
//-----------------------------------------------------------------------------
bool UIDisplaySetColor::HandledWinMessage(HWND hColorSwatch, UINT message, WPARAM wParam, LPARAM lParam,
										  TupString& strKeyname, TupString& strValue,
										  TupArray<TupString>& vectKeysToPurge, int& nFlags)
{
	bool bResult = false;
#if 0 // TODO add color swatch

	switch (message)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		{
			// See if the HWND is the same as one of the HWNDs for this object:
			bool bIdMatches = false;
			int nNumHWNDs = m_vectColorChoiceHWNDs.size();
			for (int nIndex = 0; nIndex < nNumHWNDs; nIndex++)
			{
				if (m_vectColorChoiceHWNDs.at(nIndex) == hColorSwatch)
				{
					bIdMatches = true;
					break;
				}
			}

			if (bIdMatches == true)
			{
				// The HWND is one of ours, so do our thing:
				// Get the color swatch that was clicked on:
				IColorSwatch* pCSClicked = GetIColorSwatch(hColorSwatch);
				IColorSwatch* pCSToSet = GetIColorSwatch(m_hCurrColorCtrl);
				// Now set the color:
				pCSToSet->SetColor(pCSClicked->GetColor(), FALSE);
				// Release the color swatches:
				ReleaseIColorSwatch(pCSClicked);
				ReleaseIColorSwatch(pCSToSet);
				// Set the current selection index:
				m_nCurrSelIndex = nIndex;
				// Set the keyname and value strings:
				strKeyname = m_strKeyname;
				strValue.Format("%d,%d,%d",m_vectColors.at(m_nCurrSelIndex).m_nRed,m_vectColors.at(m_nCurrSelIndex).m_nGreen,m_vectColors.at(m_nCurrSelIndex).m_nBlue);
				nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;
				// Set the exclusive flag:
				if (m_bExclusive)
					nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;
				bResult = true;
			}
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
						if (pNotify->ti->m_hWnd == (UINT)m_hLabel ||
							pNotify->ti->m_hWnd == (UINT)m_hCurrColorCtrl)
						{
							pNotify->ti->m_sTooltip = m_strToolTip;
							bResult = true;
						}
						else if ((m_uniqueValues.m_uniqueValues.GetSize() && pNotify->ti->m_hWnd == (UINT)m_hUniqueValuesButton))
						{
							pNotify->ti->m_sTooltip = "Pick from selected values";
							bResult = true;
						}
						else
						{
							int nNumChoices = m_vectColors.GetSize();
							for (int nIndex = 0; nIndex < nNumChoices; nIndex++)
							{
								if (pNotify->ti->m_hWnd == (UINT)m_vectColorChoiceHWNDs[nIndex])
								{
									pNotify->ti->m_sTooltip = m_strToolTip;
									bResult = true;
								}
							}
						}
					}
					break;
				}
		}
		break;
	}
#endif
	return bResult;
}

//-----------------------------------------------------------------------------
// UIDisplaySetColor::RemoveToolTips
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
void UIDisplaySetColor::RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip)
{
	if (m_strToolTip.GetLength())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hLabel);
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hCurrColorCtrl);
		int nNumChoices = m_vectColors.GetSize();
		for (int nIndex = 0; nIndex < nNumChoices; nIndex++)
		{
			RemoveToolTip(hParentWnd,hInstance,hToolTip,m_vectColorChoiceHWNDs[nIndex]);
		}
	}
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hUniqueValuesButton);
	}
}
