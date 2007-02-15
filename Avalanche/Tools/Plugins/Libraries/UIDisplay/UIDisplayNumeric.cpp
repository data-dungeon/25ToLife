//-----------------------------------------------------------------------------
//	File:		UIDisplayNumeric.cpp
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayNumeric class contains all the information needed to
//			display the proper ui control for a numeric in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplayNumeric.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#define SLIDER_CONTROL_MIN_WIDTH	65
#define SLIDER_CONTROL_MAX_WIDTH	200
#define BETWEEN_SLIDER_AND_FRAME	4

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplayNumeric::UIDisplayNumeric
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayNumeric::UIDisplayNumeric()
{
	m_type = ConfigNumeric::INT;
	m_nCurrValue = 0;
	m_fCurrValue = 0.0;
	m_nMin = 0;
	m_fMin = 0.0;
	m_nMax = 0;
	m_fMax = 0.0;
	m_nDefault = 0;
	m_fDefault = 0.0;
	m_nTickStep = 10;
	m_strFormat = ".3";
	m_bHasEdit = true;
	m_bHasSpinner = true;
	m_bHasSlider = false;
	m_hLabel = NULL;
	m_hEditCtrl = NULL;
	m_hSpinnerCtrl = NULL;
	m_hSliderCtrl = NULL;
	m_hChildEditCtrl = NULL;
	m_bShowIndeterminate = false;
}


//-----------------------------------------------------------------------------
// UIDisplayNumeric::~UIDisplayNumeric
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayNumeric::~UIDisplayNumeric()
{
	if (m_hLabel != NULL)
		DestroyWindow(m_hLabel);
	if (m_hEditCtrl != NULL)
		DestroyWindow(m_hEditCtrl);
	if (m_hSpinnerCtrl != NULL)
		DestroyWindow(m_hSpinnerCtrl);
	if (m_hSliderCtrl != NULL)
		DestroyWindow(m_hSliderCtrl);
}


//-----------------------------------------------------------------------------
// The following methods act as mutators for the data in this class:
//-----------------------------------------------------------------------------
void UIDisplayNumeric::SetType(ConfigNumeric::NumericType newType)
{
	m_type = newType;
}

void UIDisplayNumeric::SetCurrValue(int nNewValue)
{
	m_nCurrValue = nNewValue;
}

void UIDisplayNumeric::SetCurrValue(float fNewValue)
{
	m_fCurrValue = fNewValue;
}

void UIDisplayNumeric::SetMin(int nMin)
{
	m_nMin = nMin;
}

void UIDisplayNumeric::SetMin(float fMin)
{
	m_fMin = fMin;
}

void UIDisplayNumeric::SetMax(int nMax)
{
	m_nMax = nMax;
}

void UIDisplayNumeric::SetMax(float fMax)
{
	m_fMax = fMax;
}

void UIDisplayNumeric::SetDefault(int nDefault)
{
	m_nDefault = nDefault;
}

void UIDisplayNumeric::SetDefault(float fDefault)
{
	m_fDefault = fDefault;
}

void UIDisplayNumeric::SetTickStep(int nTickStep)
{
	m_nTickStep = nTickStep;
}

void UIDisplayNumeric::SetSpinnerStep(float fSpinnerStep)
{
	m_fSpinnerStep = fSpinnerStep;
}

void UIDisplayNumeric::SetFormat(TupString strNewFormat)
{
	m_strFormat = strNewFormat;
}

void UIDisplayNumeric::SetHasEdit(bool bHasEdit)
{
	m_bHasEdit = bHasEdit;
}

void UIDisplayNumeric::SetHasSpinner(bool bHasSpinner)
{
	m_bHasSpinner = bHasSpinner;
}

void UIDisplayNumeric::SetHasSlider(bool bHasSlider)
{
	m_bHasSlider = bHasSlider;
}

void UIDisplayNumeric::SetLabelID(int nNewID)
{
	m_nLabelID = nNewID;
}

void UIDisplayNumeric::SetEditID(int nNewID)
{
	m_nEditID = nNewID;
}

void UIDisplayNumeric::SetSpinnerID(int nNewID)
{
	m_nSpinnerID = nNewID;
}

void UIDisplayNumeric::SetSliderID(int nNewID)
{
	m_nSliderID = nNewID;
}

void UIDisplayNumeric::SetShowIndeterminate(bool bShow)
{
	m_bShowIndeterminate = bShow;
}


//-----------------------------------------------------------------------------
// The following methods act as accessors for the data in this class:
//-----------------------------------------------------------------------------
ConfigNumeric::NumericType UIDisplayNumeric::GetType()
{
	return m_type;
}

int UIDisplayNumeric::GetCurrValueInt()
{
	return m_nCurrValue;
}

float UIDisplayNumeric::GetCurrValueFloat()
{
	return m_fCurrValue;
}

int UIDisplayNumeric::GetMinInt()
{
	return m_nMin;
}

float UIDisplayNumeric::GetMinFloat()
{
	return m_fMin;
}

int UIDisplayNumeric::GetMaxInt()
{
	return m_nMax;
}

float UIDisplayNumeric::GetMaxFloat()
{
	return m_fMax;
}

int UIDisplayNumeric::GetTickStep()
{
	return m_nTickStep;
}

int UIDisplayNumeric::GetDefaultInt()
{
	return m_nDefault;
}

float UIDisplayNumeric::GetDefaultFloat()
{
	return m_fDefault;
}

TupString UIDisplayNumeric::GetFormat()
{
	return m_strFormat;
}

bool UIDisplayNumeric::HasEdit()
{
	return m_bHasEdit;
}

bool UIDisplayNumeric::HasSpinner()
{
	return m_bHasSpinner;
}

bool UIDisplayNumeric::HasSlider()
{
	return m_bHasSlider;
}

int UIDisplayNumeric::GetLabelID()
{
	return m_nLabelID;
}

int UIDisplayNumeric::GetEditID()
{
	return m_nEditID;
}

int UIDisplayNumeric::GetSpinnerID()
{
	return m_nSpinnerID;
}

int UIDisplayNumeric::GetSliderID()
{
	return m_nSliderID;
}


//-----------------------------------------------------------------------------
// UIDisplayNumeric::BuildActualUI
// This method will set up and create the actual UI necessary to display a
// numeric.
//
// Input:
//  HWND hParentDlg -- The HWND of the parent dialog.
//	 HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The handle to the tooltip for the parent dialog.
//  HFONT hUIFont -- The font to use.
//  int nStartx -- The base x coord to use for the numeric.  If the indent level
//				   for this control is > 0, then the numeric will be drawn to
//				   the right of this x coord (based on the level of
//				   indentation).
//	int nStarty -- The y coord to draw the label at.
//  int nLabelWidth -- The width of the label for the numeric.
//  int nLabelHeight -- The height of the label for the numeric.
//	int& nControlSizeY -- Upon return, this value should represent the size that
//						  the control should take up in the Y direction.
//
// Output: True if succesfull, false otherwise.
//-----------------------------------------------------------------------------
bool UIDisplayNumeric::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
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
	int nEditWidth = 50;
	m_hEditCtrl = CreateWindow(CustomControls::GetWindowClassName(CustomControls::EDIT_WINDOW_CLASS), NULL,
								  WS_CHILD | WS_VISIBLE | WS_TABSTOP | nDisabledFlag,
								  nStartx + nXOffset, nStarty + 15, nEditWidth, 16,
								  hParentDlg, (HMENU) m_nEditID, hInstance, NULL);

	// Initialize a pointer to the control to call methods on it:
	ICustomEditBox* pCustEdit = ICustomEditBox::GetICustomEditBox(m_hEditCtrl);
	m_hChildEditCtrl = pCustEdit->GetHEdit();
	// Release the custom edit:
	ICustomEditBox::ReleaseICustomEditBox(pCustEdit);

	// Create the custom spinner:
	m_hSpinnerCtrl = CreateWindow(CustomControls::GetWindowClassName(CustomControls::SPINNER_WINDOW_CLASS), NULL,
								  WS_CHILD | WS_VISIBLE | nDisabledFlag,
								  nStartx + nEditWidth + nXOffset, nStarty + 15, 7, 10,
								  hParentDlg, (HMENU) m_nSpinnerID, hInstance, NULL);
	// Set up the spinner:
	if (m_type == ConfigNumeric::INT)
	{
		// make sure step is at least one
		int spinnerStep = (int)m_fSpinnerStep;
		if (spinnerStep<=0)
			spinnerStep = 1;
		ICustomSpinner *pCustSpinner = ICustomSpinner::SetupIntSpinner(hParentDlg,m_nSpinnerID,m_nEditID,m_nMin,m_nMax,m_nCurrValue,spinnerStep);
		pCustSpinner->SetSpinnerFormat(m_strFormat);
		pCustSpinner->SetValue(m_nCurrValue, FALSE);
		if (m_bShowIndeterminate == true)
			pCustSpinner->SetIndeterminate(TRUE);
		ICustomSpinner::ReleaseISpinner(pCustSpinner);
	}
	else
	{
		ICustomSpinner *pCustSpinner = ICustomSpinner::SetupFloatSpinner(hParentDlg,m_nSpinnerID,m_nEditID,m_fMin,m_fMax,m_fCurrValue,m_fSpinnerStep,m_strFormat);
		pCustSpinner->SetValue(m_fCurrValue, FALSE);
		if (m_bShowIndeterminate == true)
			pCustSpinner->SetIndeterminate(TRUE);
		ICustomSpinner::ReleaseISpinner(pCustSpinner);
	}

	// set up the unique values button
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		m_hUniqueValuesButton = CreateWindow(_T("button"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_ICON | nDisabledFlag,
			nStartx + nEditWidth + nXOffset + 15, nStarty + 17, 12, 12, hParentDlg, (HMENU) m_uniqueValuesID,
			hInstance, NULL);
		m_hUniqueValuesIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_UNIQUE_VALUES), IMAGE_ICON, 0, 0, 0);
		::SendMessage(m_hUniqueValuesButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hUniqueValuesIcon);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hUniqueValuesButton);
	}

	// tell the tooltip about the controls
	if (m_strToolTip.GetLength())
	{
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hLabel);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hEditCtrl);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hSpinnerCtrl);
		AddToolTip(hParentDlg,hInstance,hToolTip,m_hChildEditCtrl);
	}


	// Create the custom slider (if needed):
	if (m_bHasSlider == true)
	{
		// Figure out the width:
		int nControlWidth = SLIDER_CONTROL_MIN_WIDTH;
		RECT parentRect;
		GetClientRect(hParentDlg, &parentRect);
		int nParentWidth = parentRect.right - parentRect.left;
		int nControlX = nStartx + nXOffset;
		if ((nControlX + SLIDER_CONTROL_MIN_WIDTH) > nParentWidth ||
			(nControlX + SLIDER_CONTROL_MAX_WIDTH) < nParentWidth)
		{
			// Parent dialog is either too small to allow for the min width of the
			// slider, or it is large enough to handle the max width.  In either
			// case, just use the max width of the slider:
			nControlWidth = SLIDER_CONTROL_MAX_WIDTH;
		}
		else
		{
			// The slider needs to be sized correctly:
			nControlWidth = (nParentWidth - nControlX) - BETWEEN_SLIDER_AND_FRAME;
		}
		m_hSliderCtrl = CreateWindow(CustomControls::GetWindowClassName(CustomControls::SLIDER_WINDOW_CLASS), NULL,
									 WS_CHILD | WS_VISIBLE | nDisabledFlag,
									 nControlX, nStarty + 15 + 16 + 3, nControlWidth, 23,
									 hParentDlg, (HMENU) m_nSliderID,
									 hInstance, NULL);
		// Set up the slider:
		if (m_type == ConfigNumeric::INT)
		{
			ICustomSlider *pCustSlider = ICustomSlider::SetupIntSlider(hParentDlg, m_nSliderID, m_nEditID,m_nMin, m_nMax, m_nCurrValue, m_nTickStep);
			pCustSlider->SetSliderFormat(m_strFormat);
			pCustSlider->SetValue(m_nCurrValue, FALSE);
			pCustSlider->SetResetValue(m_nDefault);
			if (m_bShowIndeterminate == true)
				pCustSlider->SetIndeterminate(TRUE);
			// Release the custom slider:
			ICustomSlider::ReleaseISlider(pCustSlider);
		}
		else
		{
			ICustomSlider* pCustSlider = ICustomSlider::SetupFloatSlider(hParentDlg, m_nSliderID, m_nEditID,m_fMin, m_fMax, m_fCurrValue, m_nTickStep,m_strFormat);
			pCustSlider->SetValue(m_fCurrValue, FALSE);
			pCustSlider->SetResetValue(m_fDefault);
			if (m_bShowIndeterminate == true)
				pCustSlider->SetIndeterminate(TRUE);
			// Release the custom slider:
			ICustomSlider::ReleaseISlider(pCustSlider);
		}

		// tell the tooltip about the control
		if (m_strToolTip.GetLength())
		{
			AddToolTip(hParentDlg,hInstance,hToolTip,m_hSliderCtrl);
		}
	}

	// Set the control size in Y:
	if (m_bHasSlider == true)
	{
		nControlSizeY += 32+3;
	}
	else
	{
		nControlSizeY += 14;
	}

	m_nCenterY = nStarty + 22;
	m_nTopY = nStarty;
	m_nBottomY = nStarty+nControlSizeY;

	// Need to invalidate the slider region so the ticks marks get drawn correctly:
	InvalidateRgn(m_hSliderCtrl, NULL, FALSE);
	UpdateWindow(m_hSliderCtrl);

	return bResult;
}

// strip off all but one trailing zero 
static void StripTrailingZeros(char* buf) 
{
	int n = strlen(buf);
	for (int i=0; i<n; i++) {
		if (buf[i] == _T('.') || buf[i] == _T(',')) 
		{
			for (int j = n-1; j>i+1; j--) 
			{
				if (buf[j]==_T('0')) 
					buf[j] = 0;
				else 
					break;
			}
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// UIDisplayNumeric::HandledWinMessage
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
//	int& nFlags -- Any methods that need to be returned to the calling method.
//
// Output: True if this object handled the message, false otherwise.  Also, see
//		   above for the two string parameters that are also used as return values.
//-----------------------------------------------------------------------------
bool UIDisplayNumeric::HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
										 TupString& strKeyname, TupString& strValue,
										 TupArray<TupString>& vectKeysToPurge, int& nFlags)
{
	bool bResult = false;

	switch (message)
	{
		case CC_ATSPINNER_BUTTONUP:
		{
			// See if the spinner is the same as our spinner id:
			int nID = LOWORD(wParam);
			if (nID == m_nSpinnerID)
			{
				// Get the spinner and slider controls and set the values:
				ICustomSpinner* pCustSpinner = ICustomSpinner::GetISpinner(m_hSpinnerCtrl);
				if (pCustSpinner->IsIndeterminate() == TRUE)
					pCustSpinner->SetIndeterminate(FALSE);
				if (m_type == ConfigNumeric::INT)
				{
					int nValue = pCustSpinner->GetIVal();
					// See if there is an associated slider control:
					if (this->HasSlider() == true)
					{
						ICustomSlider* pCustSlider = ICustomSlider::GetISlider(m_hSliderCtrl);
						if (pCustSlider->IsIndeterminate() == TRUE)
							pCustSlider->SetIndeterminate(FALSE);
						pCustSlider->SetValue(nValue, FALSE);
						ICustomSlider::ReleaseISlider(pCustSlider);
					}
					m_nCurrValue = nValue;
					strKeyname = m_strKeyname;
					char pChValue[50];
					strValue = itoa(m_nCurrValue, pChValue, 10);
				}
				else
				{
					float fValue = pCustSpinner->GetFVal();
					// See if there is an associated slider control:
					if (this->HasSlider() == true)
					{
						ICustomSlider* pCustSlider = ICustomSlider::GetISlider(m_hSliderCtrl);
						if (pCustSlider->IsIndeterminate() == TRUE)
							pCustSlider->SetIndeterminate(FALSE);
						pCustSlider->SetValue(fValue, FALSE);
						ICustomSlider::ReleaseISlider(pCustSlider);
					}
					m_fCurrValue = fValue;
					strKeyname = m_strKeyname;
					char pChValue[64];
					char format[64];
					_sntprintf( format, 64, _T("%%%sf"), m_strFormat );
					_sntprintf( pChValue, 64, format, m_fCurrValue );	
					StripTrailingZeros(pChValue);
					strValue = pChValue;
				}
				// Release the spinner:
				ICustomSpinner::ReleaseISpinner(pCustSpinner);
				// Set the exclusive flag:
				if (m_bExclusive)
					nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;
				nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;
				bResult = true;
			}

			break;
		}
		case CC_ATSLIDER_BUTTONUP:
		{
			// See if the slider is the same as our slider id:
			int nID = LOWORD(wParam);
			if (nID == m_nSliderID)
			{
				// Get the slider and spinner controls and set the values:
				// Note: For right now, all numerics will have a spinner, so
				// there is no need to check for the existance of a spinner.
				ICustomSlider* pCustSlider = ICustomSlider::GetISlider(m_hSliderCtrl);
				ICustomSpinner* pCustSpinner = ICustomSpinner::GetISpinner(m_hSpinnerCtrl);
				if (pCustSlider->IsIndeterminate() == TRUE)
					pCustSlider->SetIndeterminate(FALSE);
				if (pCustSpinner->IsIndeterminate() == TRUE)
					pCustSpinner->SetIndeterminate(FALSE);
				if (m_type == ConfigNumeric::INT)
				{
					int nValue = pCustSlider->GetIVal();
					pCustSpinner->SetValue(nValue, FALSE);
					m_nCurrValue = nValue;
					strKeyname = m_strKeyname;
					char pChValue[50];
					strValue = itoa(m_nCurrValue, pChValue, 10);
				}
				else
				{
					float fValue = pCustSlider->GetFVal();
					pCustSpinner->SetValue(fValue, FALSE);
					m_fCurrValue = fValue;
					strKeyname = m_strKeyname;
					char pChValue[64];
					char format[64];
					_sntprintf( format, 64, _T("%%%sf"), m_strFormat );
					_sntprintf( pChValue, 64, format, m_fCurrValue );	
					StripTrailingZeros(pChValue);
					strValue = pChValue;
				}
				// Release the slider and spinner:
				ICustomSlider::ReleaseISlider(pCustSlider);
				ICustomSpinner::ReleaseISpinner(pCustSpinner);
				// Set the exclusive flag:
				if (m_bExclusive)
					nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;
				nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;
				bResult = true;
			}
			break;
		}
		case WM_ATCUSTOMEDIT_ENTER:
		{
			// See if the edit is the same as our edit id:
			int nID = LOWORD(wParam);
			if (nID == m_nEditID)
			{
				// It appears that a change in the numeric's editbox automatically
				// updates the spinner and slider correctly, so just get the
				// value from the spinner:
				ICustomSpinner* pCustSpinner = ICustomSpinner::GetISpinner(m_hSpinnerCtrl);
				if (m_type == ConfigNumeric::INT)
				{
					int nValue = pCustSpinner->GetIVal();
					m_nCurrValue = nValue;
					strKeyname = m_strKeyname;
					char pChValue[50];
					strValue = itoa(m_nCurrValue, pChValue, 10);
				}
				else
				{
					float fValue = pCustSpinner->GetFVal();
					m_fCurrValue = fValue;
					strKeyname = m_strKeyname;
					char pChValue[64];
					char format[64];
					_sntprintf( format, 64, _T("%%%sf"), m_strFormat );
					_sntprintf( pChValue, 64, format, m_fCurrValue );	
					StripTrailingZeros(pChValue);
					strValue = pChValue;
				}

				// Release the spinner:
				ICustomSpinner::ReleaseISpinner(pCustSpinner);
				// Set the exclusive flag:
				if (m_bExclusive)
					nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE;
				nFlags |= UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE;
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
					case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
					{
						ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;
						if (pNotify->ti->m_hWnd == m_hLabel ||
							pNotify->ti->m_hWnd == m_hEditCtrl ||
							pNotify->ti->m_hWnd == m_hSpinnerCtrl ||
							pNotify->ti->m_hWnd == m_hChildEditCtrl ||
							(m_bHasSlider && pNotify->ti->m_hWnd == m_hSliderCtrl))
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
// UIDisplayNumeric::RemoveToolTips
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
void UIDisplayNumeric::RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip)
{
	if (m_strToolTip.GetLength())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hLabel);
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hEditCtrl);
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hSpinnerCtrl);
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hChildEditCtrl);
		if (m_bHasSlider == true)
		{
			RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hSliderCtrl);
		}
	}
	if (m_uniqueValues.m_uniqueValues.GetSize())
	{
		RemoveToolTip(hParentWnd,hInstance,hToolTip,m_hUniqueValuesButton);
	}
}
