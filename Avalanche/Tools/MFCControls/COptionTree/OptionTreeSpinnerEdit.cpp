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
#include "OptionTreeSpinnerEdit.h"

// Added Headers
#include "OptionTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionTreeSpinnerEdit

COptionTreeSpinnerEdit::COptionTreeSpinnerEdit()
{
	// Initialize variables
	m_otSpinnerButton = NULL;
}

COptionTreeSpinnerEdit::~COptionTreeSpinnerEdit()
{
}


BEGIN_MESSAGE_MAP(COptionTreeSpinnerEdit, CEdit)
	//{{AFX_MSG_MAP(COptionTreeSpinnerEdit)
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_CONTROL_REFLECT(EN_CHANGE, OnTextChange)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionTreeSpinnerEdit message handlers

void COptionTreeSpinnerEdit::SetOwnerSpinner(COptionTreeSpinnerButton *otSpinnerButton)
{
	// Save pointer
	m_otSpinnerButton = otSpinnerButton;
}

void COptionTreeSpinnerEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Find what key was presed
	if (nChar == VK_UP)
	{
		m_otSpinnerButton->SendMessage(OT_NOTIFY_UP, 0, 0);
	}
	else if (nChar == VK_DOWN)
	{
		m_otSpinnerButton->SendMessage(OT_NOTIFY_DOWN, 0, 0);
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL COptionTreeSpinnerEdit::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// User edit
	if(m_otSpinnerButton != NULL)
	{
		// -- No user edit
		if (m_otSpinnerButton->GetOption(OT_EDIT_USEREDIT) == FALSE)
		{
			// -- -- No paste or cut
			if (LOWORD(wParam) == WM_PASTE || LOWORD(wParam) == WM_CUT)
			{
				return FALSE;
			}
		}
		// -- User edit
		else
		{
			switch (LOWORD(wParam))
			{
				case EM_UNDO:
				case WM_CUT:
				case WM_COPY:
				case WM_CLEAR:
				case WM_PASTE:
					return SendMessage(LOWORD(wParam));

				case OT_MES_NSELECTALL:
					return SendMessage(EM_SETSEL, 0, -1);

				default:
					return CEdit::OnCommand(wParam, lParam);
			}
		}
	}

	return FALSE;
}

int COptionTreeSpinnerEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// Create edit
	if (CEdit::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
	return 0;
}

void COptionTreeSpinnerEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    // Declare variables
	CMenu mMenu;
	BOOL bReadOnly;
	DWORD dwFlags;
	int nSelStart, nSelEnd;
	int nLen;

	// Make sure we have options
	if (m_otSpinnerButton == NULL)
	{
		return;
	}

    // Crete menu
	mMenu.CreatePopupMenu();

    // Get read only
	bReadOnly = GetStyle() & ES_READONLY;
	
	// Get selection
	GetSel(nSelStart, nSelEnd);

	// Get window length
	nLen = GetWindowTextLength();

	// Insert Undo
	if (m_otSpinnerButton->GetOption(OT_EDIT_USEREDIT) == TRUE)
	{
		dwFlags = CanUndo() && !bReadOnly ? 0 : MF_GRAYED | MF_DISABLED;
	}
	else
	{
		dwFlags = MF_GRAYED | MF_DISABLED;
	}
    mMenu.InsertMenu(0, MF_BYPOSITION | dwFlags, EM_UNDO, OT_MES_UNDO);
	
	// Insert seperator
    mMenu.InsertMenu(1, MF_BYPOSITION | MF_SEPARATOR);

    // Insert copy
	if (nSelStart == nSelEnd)
	{
		dwFlags = MF_GRAYED | MF_DISABLED;
	}
	else
	{
		dwFlags = 0;
	}
    mMenu.InsertMenu(2, MF_BYPOSITION | dwFlags, WM_COPY, OT_MES_COPY);

	// Insert cut and clear
	if (m_otSpinnerButton->GetOption(OT_EDIT_USEREDIT) == TRUE)
	{
		if (nSelStart == nSelEnd || bReadOnly == TRUE)
		{
			dwFlags = MF_GRAYED | MF_DISABLED;
		}
		else
		{
			dwFlags = 0;
		}
	}
	else
	{
		dwFlags = MF_GRAYED | MF_DISABLED;
	}
    mMenu.InsertMenu(2, MF_BYPOSITION | dwFlags, WM_CUT, OT_MES_CUT);
    mMenu.InsertMenu(4, MF_BYPOSITION | dwFlags, WM_CLEAR, OT_MES_DELETE);

    // Insert paste
	if (m_otSpinnerButton->GetOption(OT_EDIT_USEREDIT) == TRUE)
	{
		if (IsClipboardFormatAvailable(CF_TEXT) == FALSE || bReadOnly == TRUE)
		{
			dwFlags = MF_GRAYED | MF_DISABLED;
		}
		else
		{
			dwFlags = 0;
		}
	}
	else
	{
		dwFlags = MF_GRAYED | MF_DISABLED;
	}
    mMenu.InsertMenu(4, MF_BYPOSITION | dwFlags, WM_PASTE, OT_MES_PASTE);

	// Insert seperator
    mMenu.InsertMenu(6, MF_BYPOSITION | MF_SEPARATOR);

    // Insert select all
    mMenu.InsertMenu(7, MF_BYPOSITION | dwFlags, OT_MES_NSELECTALL, OT_MES_SELECTALL);

    // Show menu
	mMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);	
}

void COptionTreeSpinnerEdit::OnKillFocus(CWnd* pNewWnd) 
{
	// Make sure we have options
	if (m_otSpinnerButton != NULL)
	{
		// -- If new focus is not parent then sent message to item to commit
		if (pNewWnd->GetSafeHwnd() != m_otSpinnerButton->GetSafeHwnd())
		{
			::PostMessage(m_otSpinnerButton->GetSafeHwnd(), OT_NOTIFY_COMMITCHANGES, NULL, NULL);
		}			
	}
	
	CEdit::OnKillFocus(pNewWnd);	
}

void COptionTreeSpinnerEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);	
}

void COptionTreeSpinnerEdit::OnTextChange() 
{
	// Force spinner button redraw
	::PostMessage(m_otSpinnerButton->GetSafeHwnd(), OT_NOTIFY_FORCEREDRAW, NULL, NULL);	
}

BOOL COptionTreeSpinnerEdit::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN) 
	{
		if(pMsg->wParam == VK_RETURN) 
		{
			::PostMessage(m_otSpinnerButton->GetSafeHwnd(), OT_NOTIFY_COMMITCHANGES, NULL, NULL);
			return TRUE;
		}
	}
	int significantDigits = m_otSpinnerButton->GetSignificantDigits();
	double dRangeTop;
	double dRangeBottom;
	m_otSpinnerButton->GetRange(dRangeBottom,dRangeTop);

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
			if (dRangeBottom<0.0)
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
				ASSERT(significantDigits >= 0 && significantDigits <= 9);

				// accept integer values only?
				if (!significantDigits)
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
				//					  by significantDigits?
				else if ((dwSelStart == dwSelEnd) &&
					(strBuffer.Mid(dwSelStart+1).GetLength() >= significantDigits) )
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
				//					  by significantDigits?
				if ( (iPos >= 0) && 
					((int)dwSelStart > iPos) && 
					(dwSelStart == dwSelEnd) &&
					(strBuffer.Mid(iPos+1).GetLength() >= significantDigits) )
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

void COptionTreeSpinnerEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Disallow user edit
	if(m_otSpinnerButton != NULL)
	{
		// -- No user edit
		if (m_otSpinnerButton->GetOption(OT_EDIT_USEREDIT) == FALSE)
		{
			return;
		}
	}
			
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
