// COptionTree
//
// License
// -------
// This code is provided "as is" with no expressed or implied warranty.
// 
// You may use this code in a commercial product with or without acknowledgement.
// However you may not sell this code or any modification of this code, this includes 
// commercial libraries and anything else for profit.
//
// I would appreciate a notification of any bugs or bug fixes to help the control grow.
//
// History:
// --------
//	See License.txt for full history information.
//
//
// Copyright (c) 1999-2002 
// ComputerSmarts.net 
// mattrmiller@computersmarts.net
#include "stdafx.h"
#include "OptionTreeItemEdit.h"

// Added Headers
#include "OptionTree.h"
#include ".\optiontreeitemedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionTreeItemEdit

COptionTreeItemEdit::COptionTreeItemEdit()
{
	// Initialize variables
	m_bFocus = FALSE;
	m_dwOptions = 0;	
	m_bAllowNegativeValues = true;
	m_significantDigits = 6;
	// Set item type
	SetItemType(OT_ITEM_EDIT);
}

COptionTreeItemEdit::~COptionTreeItemEdit()
{
}


BEGIN_MESSAGE_MAP(COptionTreeItemEdit, CEdit)
	//{{AFX_MSG_MAP(COptionTreeItemEdit)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionTreeItemEdit message handlers

void COptionTreeItemEdit::OnActivate()
{
	// Make sure window is valid
	if (IsWindow(GetSafeHwnd()))
	{
		// -- Show window
		ShowWindow(SW_SHOW);

		// -- Set selection
		SetSel(0, 0);

		// -- Set window position
		MoveWindow(m_rcAttribute.left, m_rcAttribute.top, m_rcAttribute.Width(), m_rcAttribute.Height());

		// -- Set focus
		SetFocus();
	}
}

UINT COptionTreeItemEdit::OnGetDlgCode() 
{
	return CEdit::OnGetDlgCode();// | DLGC_WANTALLKEYS;
}

void COptionTreeItemEdit::DrawAttribute(CDC *pDC, const RECT &rcRect)
{
	// If we don't have focus, text is drawn.
	if (m_bFocus == TRUE)
	{
		return;
	}

	// Make sure options aren't NULL
	if (m_otOption == NULL)
	{
		return;
	}

	// Make sure there is a window
	if (!IsWindow(GetSafeHwnd()))
	{
		return;
	}

	// Set window position
	if (IsWindow(GetSafeHwnd()))
	{
		MoveWindow(m_rcAttribute.left, m_rcAttribute.top, m_rcAttribute.Width(), m_rcAttribute.Height());
	}

	// Declare variables
	HGDIOBJ hOld;
	COLORREF crOld;
	int nOldBack;
	CRect rcText;
	CString strText, strWindowText;
	COLORREF crOldBack;

	// Get window text
	GetWindowText(strWindowText);

	// Make text
	// -- Password
	if (GetOption(OT_EDIT_PASSWORD) == TRUE)
	{
		strText = "";
		for (int i = 0; i < strWindowText.GetLength(); i++)
		{
			strText += _T("*");
		}
	}
	// -- No password
	else
	{
		strText = strWindowText;
	}

	// Select font
	hOld = pDC->SelectObject(m_otOption->GetNormalFont());
	
	// Set text color
	if (IsReadOnly() == TRUE || m_otOption->IsWindowEnabled() == FALSE)
	{
		crOld = pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
	}
	else
	{
		crOld = pDC->SetTextColor(GetTextColor());
	}

	// Set background mode
	nOldBack = pDC->SetBkMode(TRANSPARENT);

	// Set background color
	crOldBack = pDC->SetBkColor(GetBackgroundColor());	

	// Get rectangle
	rcText = rcRect;

	// Draw text
	// -- Single line
	if (GetOption(OT_EDIT_MULTILINE) == FALSE)
	{
		pDC->DrawText(strText, rcText, DT_SINGLELINE | DT_VCENTER);
	}
	// -- Mutliline
	else
	{
		pDC->DrawText(strText, rcText, DT_TOP);
	}

	// Restore GDI ojects
	pDC->SelectObject(hOld);
	pDC->SetTextColor(crOld);
	pDC->SetBkMode(nOldBack);
	pDC->SetBkColor(crOldBack);
}

void COptionTreeItemEdit::OnCommit()
{
	// Hide edit control
	if (IsWindow(GetSafeHwnd()))
	{
		// -- Show window
		ShowWindow(SW_HIDE);
	}

}

void COptionTreeItemEdit::OnRefresh()
{
	// Set the window text
	if (IsWindow(GetSafeHwnd()))
	{
		MoveWindow(m_rcAttribute.left, m_rcAttribute.top, m_rcAttribute.Width(), m_rcAttribute.Height());
	}
}

void COptionTreeItemEdit::OnMove()
{
	// Set window position
	if (IsWindow(GetSafeHwnd()))
	{
		MoveWindow(m_rcAttribute.left, m_rcAttribute.top, m_rcAttribute.Width(), m_rcAttribute.Height());
	}

	// Hide window
	if (m_bFocus == FALSE && IsWindow(GetSafeHwnd()))
	{
		// -- Show window
		ShowWindow(SW_HIDE);
	}
}

void COptionTreeItemEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// If return is pressed and not multiline, commit
//	if (IsWindow(GetSafeHwnd()))
//	{
//		if (nChar == VK_RETURN && (GetStyle() & ES_WANTRETURN) == FALSE)
//		{
//			// -- Commit changes
//			CommitChanges();
//
//			return;
//		}
//	}


	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COptionTreeItemEdit::OnKillFocus(CWnd* pNewWnd) 
{
	// Validate
	if (m_otOption == NULL)
	{
		CEdit::OnKillFocus(pNewWnd);
		return;
	}

	// See if new window is tree of list
//	if (m_otOption->IsChild(pNewWnd) == TRUE)//AKJ
//	{
		// -- Mark focus
		m_bFocus = FALSE;

		// -- Commit changes
		CommitChanges();
//	}
	
	CEdit::OnKillFocus(pNewWnd);	
}

BOOL COptionTreeItemEdit::IsStringNumeric(CString strString)
{
	// See if string is numeric or not
	if (strString.FindOneOf("1234567890") == -1 || strString.FindOneOf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`~[]{}_=+\\|'/?>,<") != -1)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL COptionTreeItemEdit::GetEditFloat(float &fReturn)
{
	// Declare variables
	CString strWindowText;

	// Set blank
	fReturn = 0;

	// Get window text
	if (IsWindow(GetSafeHwnd()))
	{
		GetWindowText(strWindowText);
	}
	else
	{
		return FALSE;
	}

	// First see if we are in numerical mode
	if (GetOption(OT_EDIT_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(strWindowText) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	fReturn = (float) atof(strWindowText);

	return TRUE;
}

BOOL COptionTreeItemEdit::GetEditDouble(double &dReturn)
{
	// Declare variables
	CString strWindowText;

	// Set blank
	dReturn = 0;

	// Get window text
	if (IsWindow(GetSafeHwnd()))
	{
		GetWindowText(strWindowText);
	}
	else
	{
		return FALSE;
	}

// AKJ
	// First see if we are in numerical mode
	if (GetOption(OT_EDIT_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(strWindowText) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	dReturn = atof(strWindowText);

	return TRUE;
}

BOOL COptionTreeItemEdit::GetEditLong(long &lReturn)
{
	// Declare variables
	CString strWindowText;

	// Set blank
	lReturn = 0;

	// Get window text
	if (IsWindow(GetSafeHwnd()))
	{
		GetWindowText(strWindowText);
	}
	else
	{
		return FALSE;
	}

	// First see if we are in numerical mode
	if (GetOption(OT_EDIT_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(strWindowText) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	lReturn = atol(strWindowText);

	return TRUE;
}

BOOL COptionTreeItemEdit::GetEditInt(int &nReturn)
{
	// Declare variables
	CString strWindowText;

	// Set blank
	nReturn = 0;

	// Get window text
	if (IsWindow(GetSafeHwnd()))
	{
		GetWindowText(strWindowText);
	}
	else
	{
		return FALSE;
	}

	// First see if we are in numerical mode
	if (GetOption(OT_EDIT_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(strWindowText) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	nReturn = atoi(strWindowText);

	return TRUE;
}

BOOL COptionTreeItemEdit::GetEditDword(DWORD &dwReturn)
{
	// Declare variables
	CString strWindowText;

	// Set blank
	dwReturn = 0;

	// Get window text
	if (IsWindow(GetSafeHwnd()))
	{
		GetWindowText(strWindowText);
	}
	else
	{
		return FALSE;
	}

	// First see if we are in numerical mode
	if (GetOption(OT_EDIT_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(strWindowText) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	dwReturn = (DWORD) atoi(strWindowText);

	return TRUE;
}

BOOL COptionTreeItemEdit::GetEditString(CString &strWindowText)
{
	// Get window text
	if (IsWindow(GetSafeHwnd()))
	{
		GetWindowText(strWindowText);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void COptionTreeItemEdit::CleanDestroyWindow()
{
	// Destroy window
	if (IsWindow(GetSafeHwnd()))
	{
		// -- Destroy window
		DestroyWindow();
	}
}

void COptionTreeItemEdit::OnSetFocus(CWnd* pOldWnd) 
{
	// Mark focus
	m_bFocus = TRUE;
	
	CEdit::OnSetFocus(pOldWnd);	
}

BOOL COptionTreeItemEdit::CreateEditItem(DWORD dwOptions, DWORD dwAddStyle)
{
	// Declare variables
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;
	BOOL bRet = FALSE;


	// Make sure options is not NULL
	if (m_otOption == NULL)
	{
		return FALSE;
	}

	// Create edit control
	if (!IsWindow(GetSafeHwnd()))
	{
		// -- Create edit style
		// -- -- Multiline
		if (dwOptions & OT_EDIT_MULTILINE)
		{
			dwStyle |= ES_MULTILINE;
		}
		// -- -- Password
		if (dwOptions & OT_EDIT_PASSWORD)
		{
			dwStyle |= ES_PASSWORD;
		}
		// -- -- Additional style
		if (dwAddStyle != 0)
		{
			dwStyle |= dwAddStyle;
		}

		// -- Create the edit view
		bRet = Create(dwStyle, m_rcAttribute, m_otOption->GetCtrlParent(), GetCtrlID());

		// -- Setup edit
		if (bRet == TRUE)
		{
			// -- -- Set font
			SetFont(m_otOption->GetNormalFont(), TRUE);

			// -- -- Modify style
			ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);

			// -- -- Save options
			m_dwOptions = dwOptions;

			// -- -- Set selection
			SetSel(0, 0);

			// -- -- Set multiline
			if (GetOption(OT_EDIT_MULTILINE) == TRUE)
			{
				SetItemHeight(OT_EDIT_MLHEIGHT);
			}

			// -- -- Set window position
			MoveWindow(m_rcAttribute.left, m_rcAttribute.top, m_rcAttribute.Width(), m_rcAttribute.Height());

			// -- -- Hide window
			ShowWindow(SW_HIDE);
		}
	}

	return bRet;
}

void COptionTreeItemEdit::SetEditDouble(double dValue)
{
	// Declare variables
	CString strText;

	// Convert string
	if (m_significantDigits==0)
		strText.Format("%.0f", dValue);
	else
	{
		char ascii[12];
		itoa(m_significantDigits,ascii,10);
		CString temp = "%.";
		temp += ascii;
		temp += "f";
		strText.Format(temp, dValue);//AKJ
	}

	// Modify style
	if (::IsWindow(GetSafeHwnd()))
	{
		SetWindowText(strText);
	}
}

void COptionTreeItemEdit::SetEditInt(int nValue)
{
	// Declare variables
	CString strText;

	// Convert string
	strText.Format("%d", nValue);

	// Modify style
	if (::IsWindow(GetSafeHwnd()))
	{
		SetWindowText(strText);
	}
}

void COptionTreeItemEdit::SetEditFloat(float fValue)
{
	// Declare variables
	CString strText;

	// Convert string
	strText.Format("%.0f", fValue);

	// Modify style
	if (::IsWindow(GetSafeHwnd()))
	{
		SetWindowText(strText);
	}
}

void COptionTreeItemEdit::SetEditDword(DWORD dwValue)
{
	// Declare variables
	CString strText;

	// Convert string
	strText.Format("%d", dwValue);

	// Modify style
	if (::IsWindow(GetSafeHwnd()))
	{
		SetWindowText(strText);
	}
}

void COptionTreeItemEdit::SetEditLong(long lValue)
{
	// Declare variables
	CString strText;

	// Convert string
	strText.Format("%.0f", lValue);

	// Modify style
	if (::IsWindow(GetSafeHwnd()) == TRUE)
	{
		SetWindowText(strText);
	}
}

void COptionTreeItemEdit::SetEditString(const CString &strWindowText)
{
	// Modify style
	if (::IsWindow(GetSafeHwnd()) == TRUE)
	{
		SetWindowText(strWindowText);
	}
}

void COptionTreeItemEdit::OnDeSelect()
{
	// Hide window
	if (IsWindow(GetSafeHwnd()))
	{
		// -- Show window
		ShowWindow(SW_HIDE);
	}
}

void COptionTreeItemEdit::OnSelect()
{
	// Do nothing here
}

BOOL COptionTreeItemEdit::GetOption(DWORD dwOption)
{
	// Return option
	return (m_dwOptions & dwOption) ? TRUE : FALSE;
}

void COptionTreeItemEdit::SetOption(DWORD dwOption, BOOL bSet)
{
	// Set option
	if (bSet == TRUE)
	{
		m_dwOptions |= dwOption;
	}
	else
	{
		m_dwOptions &= ~dwOption;
	}

	// Options changed
	OptionsChanges();
}

void COptionTreeItemEdit::OptionsChanges()
{
	// Multiline
	SetDrawMultiline(GetOption(OT_EDIT_MULTILINE));
	if (GetOption(OT_EDIT_MULTILINE) == TRUE)
	{
		SetItemHeight(OT_EDIT_MLHEIGHT);

		if (IsWindow(GetSafeHwnd()))
		{
			ModifyStyle(0, ES_MULTILINE, 0);
		}
	}
	else
	{
		SetItemHeight(OT_DEFHEIGHT);

		if (IsWindow(GetSafeHwnd()))
		{
			ModifyStyle(ES_MULTILINE, 0, 0);
		}
	}

	// Password
	if (GetOption(OT_EDIT_PASSWORD) == TRUE)
	{
		if (IsWindow(GetSafeHwnd()))
		{
			ModifyStyle(0, ES_PASSWORD, 0);
		}
	}
	else
	{
		if (IsWindow(GetSafeHwnd()))
		{
			ModifyStyle(ES_PASSWORD, 0, 0);
		}
	}

	// Numerical
	SetDrawMultiline(GetOption(OT_EDIT_NUMERICAL));
	if (GetOption(OT_EDIT_NUMERICAL) == TRUE)
	{
		SetItemHeight(OT_EDIT_MLHEIGHT);

		if (IsWindow(GetSafeHwnd()))
		{
			ModifyStyle(0, ES_MULTILINE, 0);
		}
	}
	else
	{
		SetItemHeight(OT_DEFHEIGHT);

		if (IsWindow(GetSafeHwnd()))
		{
			ModifyStyle(ES_MULTILINE, 0, 0);
		}
	}
}

BOOL COptionTreeItemEdit::PreTranslateMessage(MSG* pMsg)
{
	//if key is pressed
	if (pMsg->message == WM_KEYDOWN) 
	{
		if(pMsg->wParam == VK_RETURN) 
		{
			// if a single line or multiple with ctrl key down
			if (GetKeyState(VK_CONTROL) < 0 || (GetStyle() & ES_WANTRETURN) == FALSE)
			{
				CommitChanges();
				return TRUE;
			}
		}
	}
	if (GetOption(OT_EDIT_NUMERICAL) == TRUE)
	{
		return PreTranslateMessageNumeric(pMsg);
	}
	return CEdit::PreTranslateMessage(pMsg);
}

BOOL COptionTreeItemEdit::PreTranslateMessageNumeric(MSG* pMsg)
{
	CString	strBuffer;			// edit control's text buffer
	DWORD	dwSelStart = 0,		// selection starting position
		dwSelEnd = 0;		// selection ending position
	int		iPos = 0;			// to hold the return value of CString::Find
	UINT	nKeyCode = pMsg->wParam;	// virtual key code of the key pressed

	// not a WM_KEYDOWN message?
	if (pMsg->message != WM_KEYDOWN)
		// forward for default processing!
		goto LForwardMsg;

	// CTRL+C, CTRL+X, or, CTRL+V?
	if ( (nKeyCode == _T('C') || nKeyCode == _T('X') || nKeyCode == _T('V')) && 
		(::GetKeyState(VK_CONTROL) & 0x8000) )
		goto LForwardMsg;

	//TRACE1("(Info) CNumericEdit::PreTranslateMessage:"
	//		" Virtual key code of the key pressed = 0x%04x\n", nKeyCode);

	switch (nKeyCode)
	{

		// subtract key?
	case VK_SUBTRACT:
	case VK_OEM_MINUS:	// Removed By Gurmeet S. Kochar (Mar 04, 2004)
		{
			// get into this only if negative values are allowed!
			if (m_bAllowNegativeValues)
			{
				GetWindowText(strBuffer);
				if (strBuffer.IsEmpty())
					break;

				// retrieve selection range
				SendMessage(EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);

				// is the selection beginning from the very first character?
				if (!dwSelStart)
				{
					// NOTE: if text buffer starts from a decimal symbol, 
					//		 then we cannot accept a negative symbol until the 
					//		 decimal points symbol falls within the current 
					//		 selection range

					/**
					*	Removed By Gurmeet S. Kochar (Mar 04, 2004)
					*/
					if (strBuffer[0] == _T('.'))
					{
						// decimal symbol falls within the current selection range?
						if (dwSelEnd != dwSelStart)
							// accept the negative symbol as it is going to 
							// replace the currently selected text
							break;

						::MessageBeep(-1);
						return (TRUE);
					}

					// check to make sure the first character is not 
					// already a negative sign
					if (strBuffer[0] != _T('-'))
						break;

					// the first character is a negative sign but 
					// is it within the current selection range?
					if (dwSelEnd != dwSelStart)
						break;
				}
			}

			::MessageBeep(-1);
			return (TRUE);
		}

		// digits on the main section of keyboard?
	case 0x30:	case 0x31:	case 0x32:	case 0x33:	case 0x34:
	case 0x35:	case 0x36:	case 0x37:	case 0x38:	case 0x39:
		// digits on the numeric keypad?
	case 0x60:	case 0x61:	case 0x62:	case 0x63:	case 0x64:
	case 0x65:	case 0x66:	case 0x67:	case 0x68:	case 0x69:
		// decimal symbol?
	case VK_DECIMAL:
	case VK_OEM_PERIOD:	// Removed By Gurmeet S. Kochar (Mar 04, 2004)
		{
			GetWindowText(strBuffer);
			if (strBuffer.IsEmpty())
				break;

			/**
			*	Removed By Gurmeet S. Kochar (Mar 04, 2004)
			*/
			iPos = strBuffer.Find(_T('.'));

			// get the current selection range
			SendMessage(EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);

			//TRACE2("(Info) CNumericEdit::PreTranslateMessage:"
			//		" SelStart = %ld, SelEnd = %ld\n", dwSelStart, dwSelEnd);

			// decimal symbol?
			if (nKeyCode == VK_DECIMAL || nKeyCode == VK_OEM_PERIOD)
			{
				ASSERT(m_significantDigits >= 0 && m_significantDigits <= 9);

				// accept integer values only?
				if (!m_significantDigits)
				{
					::MessageBeep(-1);
					return (TRUE);
				}

				// a decimal symbol is already there?
				if (iPos >= 0)
				{
					if ( (dwSelEnd == dwSelStart) && 
						(iPos < (int)dwSelStart || iPos > (int)dwSelEnd) )
					{
						// there can be only one decimal symbol in a numeric value
						::MessageBeep(-1);
						return (TRUE);
					}
				}
				// iPos >= 0 : value contains a decimal symbol?
				// dwSelStart > iPos : user is typing text after the decimal symbol?
				// dwSelStart == dwSelEnd : the selection is empty?
				// strBuffer.Mid... : number of digits after decimal symbol is 
				//					  going to exceed than the value specified  
				//					  by m_significantDigits?
				else if ((dwSelStart == dwSelEnd) &&
					(strBuffer.Mid(dwSelStart+1).GetLength() >= m_significantDigits) )
				{
					::MessageBeep(-1);
					return (TRUE);
				}
			}

			// one of the digit keys has been pressed?
			else
			{
				// iPos >= 0 : value contains a decimal symbol?
				// dwSelStart > iPos : user is typing text after the decimal symbol?
				// dwSelStart == dwSelEnd : the selection is empty?
				// strBuffer.Mid... : number of digits after decimal symbol is 
				//					  going to exceed than the value specified  
				//					  by m_significantDigits?
				if ( (iPos >= 0) && 
					((int)dwSelStart > iPos) && 
					(dwSelStart == dwSelEnd) &&
					(strBuffer.Mid(iPos+1).GetLength() >= m_significantDigits) )
				{
					::MessageBeep(-1);
					return (TRUE);
				}
			}

			// neither a digit nor a decimal symbol is allowed 
			// at the starting of the text when the value in the 
			// text buffer is a negative value

			/**
			*	Removed By Gurmeet S. Kochar (Mar 04, 2004)
			*/
			if ( (!dwSelStart) && (strBuffer[0] == _T('-')) )
			{
				// negative sign is within the current selection range?
				if (dwSelEnd == dwSelStart)
				{
					::MessageBeep(-1);
					return (TRUE);
				}
			}

			break;
		}

	default:
		{
			if ((nKeyCode >= 0x01 && nKeyCode <= 0x40) || 
				(nKeyCode >= 0x5B && nKeyCode <= 0x69) || 
				(nKeyCode >= 0x70 && nKeyCode <= 0xB9) || 
				(nKeyCode >= 0xE5 && nKeyCode <= 0xFE))
			{
				// spaces are not allowed in numeric values!
				if (nKeyCode == VK_SPACE)
				{
					::MessageBeep(-1);
					return (TRUE);
				}

				break;
			}

			return (TRUE);	// eat message; ignore the key!!!
		}
	}

LForwardMsg:
	return (CEdit::PreTranslateMessage(pMsg));

}

