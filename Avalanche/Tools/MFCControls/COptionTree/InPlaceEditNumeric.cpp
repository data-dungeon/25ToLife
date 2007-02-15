// InPlaceEdit.cpp : implementation file
//
// Adapted by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
//
// The code contained in this file is based on the original
// CInPlaceEditNumeric from http://www.codeguru.com/listview/edit_subitems.shtml
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.10+
//
// History:
//         10 May 1998  Uses GVN_ notifications instead of LVN_,
//                      Sends notification messages to the parent, 
//                      instead of the parent's parent.
//         15 May 1998  There was a problem when editing with the in-place editor, 
//                      there arises a general protection fault in user.exe, with a 
//                      few qualifications:
//                         (1) This only happens with owner-drawn buttons;
//                         (2) This only happens in Win95
//                         (3) This only happens if the handler for the button does not 
//                             create a new window (even an AfxMessageBox will avoid the 
//                             crash)
//                         (4) This will not happen if Spy++ is running.
//                      PreTranslateMessage was added to route messages correctly.
//                      (Matt Weagle found and fixed this problem)
//         26 Jul 1998  Removed the ES_MULTILINE style - that fixed a few probs!
//          6 Aug 1998  Added nID to the constructor param list
//          6 Sep 1998  Space no longer clears selection when starting edit (Franco Bez)
//         10 Apr 1999  Enter, Tab and Esc key prob fixed (Koay Kah Hoe)
//                      Workaround for bizzare "shrinking window" problem in CE
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCHAR.h"
#include "InPlaceEditNumeric.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditNumeric

CInPlaceEditNumeric::CInPlaceEditNumeric(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
									int nRow, int nColumn, CString sInitText, 
									UINT nFirstChar,int significantDigits,bool bAllowNegativeValues)
{
	m_sInitText     = sInitText;
	m_nRow          = nRow;
	m_nColumn       = nColumn;
	m_nLastChar     = 0; 
	m_bExitOnArrows = (nFirstChar != VK_LBUTTON);    // If mouse click brought us here,
	// then no exit on arrows

	m_significantDigits = significantDigits;
	m_bAllowNegativeValues = bAllowNegativeValues;

	m_Rect = rect;  // For bizarre CE bug.

	DWORD dwEditStyle = WS_BORDER|WS_CHILD|WS_VISIBLE| ES_AUTOHSCROLL //|ES_MULTILINE
		| dwStyle;
	if (!Create(dwEditStyle, rect, pParent, nID)) return;

	SetFont(pParent->GetFont());

	SetWindowText(sInitText);
	SetFocus();

	switch (nFirstChar){
		  case VK_LBUTTON: 
		  case VK_RETURN:   SetSel((int)_tcslen(m_sInitText), -1); return;
		  case VK_BACK:     SetSel((int)_tcslen(m_sInitText), -1); break;
		  case VK_TAB:
		  case VK_DOWN: 
		  case VK_UP:   
		  case VK_RIGHT:
		  case VK_LEFT:  
		  case VK_NEXT:  
		  case VK_PRIOR: 
		  case VK_HOME:
		  case VK_SPACE:
		  case VK_END:      SetSel(0,-1); return;
		  default:          SetSel(0,-1);
	}

	// Added by KiteFly. When entering DBCS chars into cells the first char was being lost
	// SenMessage changed to PostMessage (John Lagerquist)
	if( nFirstChar < 0x80)
		PostMessage(WM_CHAR, nFirstChar);   
	else
		PostMessage(WM_IME_CHAR, nFirstChar);
}

CInPlaceEditNumeric::~CInPlaceEditNumeric()
{
}

BEGIN_MESSAGE_MAP(CInPlaceEditNumeric, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEditNumeric)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// CInPlaceEditNumeric message handlers

// If an arrow key (or associated) is pressed, then exit if
//  a) The Ctrl key was down, or
//  b) m_bExitOnArrows == TRUE
void CInPlaceEditNumeric::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
		nChar == VK_DOWN  || nChar == VK_UP   ||
		nChar == VK_RIGHT || nChar == VK_LEFT) &&
		(m_bExitOnArrows || GetKeyState(VK_CONTROL) < 0))
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

// As soon as this edit loses focus, kill it.
void CInPlaceEditNumeric::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	EndEdit();
}

void CInPlaceEditNumeric::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_TAB || nChar == VK_RETURN)
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();    // This will destroy this window
		return;
	}
	if (nChar == VK_ESCAPE) 
	{
		SetWindowText(m_sInitText);    // restore previous text
		m_nLastChar = nChar;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString str;
	GetWindowText( str );

	// add some extra buffer
	str += _T("  ");

	CWindowDC dc(this);
	CFont *pFontDC = dc.SelectObject(GetFont());
	CSize size = dc.GetTextExtent( str );
	dc.SelectObject( pFontDC );

	// Get client rect
	CRect ParentRect;
	GetParent()->GetClientRect( &ParentRect );

	// Check whether control needs to be resized
	// and whether there is space to grow
	if (size.cx > m_Rect.Width())
	{
		if( size.cx + m_Rect.left < ParentRect.right )
			m_Rect.right = m_Rect.left + size.cx;
		else
			m_Rect.right = ParentRect.right;
		MoveWindow( &m_Rect );
	}
}

UINT CInPlaceEditNumeric::OnGetDlgCode() 
{
	return DLGC_WANTALLKEYS;
}

////////////////////////////////////////////////////////////////////////////
// CInPlaceEditNumeric overrides

// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL CInPlaceEditNumeric::PreTranslateMessage(MSG* pMsg) 
{
	// Catch the Alt key so we don't choke if focus is going to an owner drawn button
	if (pMsg->message == WM_SYSCHAR)
		return TRUE;

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

// Auto delete
void CInPlaceEditNumeric::PostNcDestroy() 
{
	CEdit::PostNcDestroy();

	delete this;	
}

////////////////////////////////////////////////////////////////////////////
// CInPlaceEditNumeric implementation

void CInPlaceEditNumeric::EndEdit()
{
	CString str;

	// EFW - BUG FIX - Clicking on a grid scroll bar in a derived class
	// that validates input can cause this to get called multiple times
	// causing assertions because the edit control goes away the first time.
	static BOOL bAlreadyEnding = FALSE;

	if(bAlreadyEnding)
		return;

	bAlreadyEnding = TRUE;
	GetWindowText(str);

	// Send Notification to parent
	GV_DISPINFO dispinfo;

	dispinfo.hdr.hwndFrom = GetSafeHwnd();
	dispinfo.hdr.idFrom   = GetDlgCtrlID();
	dispinfo.hdr.code     = GVN_ENDLABELEDIT;

	dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM;
	dispinfo.item.row     = m_nRow;
	dispinfo.item.col     = m_nColumn;
	dispinfo.item.strText  = str;
	dispinfo.item.lParam  = (LPARAM) m_nLastChar;

	CWnd* pOwner = GetOwner();
	if (pOwner)
		pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );

	// Close this window (PostNcDestroy will delete this)
	if (IsWindow(GetSafeHwnd()))
		SendMessage(WM_CLOSE, 0, 0);
	bAlreadyEnding = FALSE;
}
