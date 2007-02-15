//
//	CustomControl
//	

/*
 * $History: CustomEditBox.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/14/03    Time: 2:15p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * use of new window class prefixes
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/19/02   Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/27/02   Time: 10:21a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * callbacks setup and focus bugs
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 9:58a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new controls to numeric type.
 * Fixed color issues with Max
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 1:18p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * all compiling
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/20/02   Time: 4:20p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * working on custom control port
 */

#include "stdafx.h"
#include "CustomEditBox.h"
#include "CustomSpinner.h"
#include "CustomSlider.h"
#include "CustomControls.h"
#include "ICustomEditBox.h"
#include "WinUtil.h"

#define CE_HBORDER	2
#define CE_VBORDER	2

#define DEF_XOFFSET -2
#define DEF_YOFFSET 1

namespace AttrControls 
{

CustomEditBox::CustomEditBox( HWND hWnd ) : CustomControl(hWnd) 
{
	RECT rect;

	::GetClientRect( hWnd, &rect );

	m_xOff = DEF_XOFFSET;
	m_yOff = DEF_YOFFSET;
	m_pSpinner = NULL;
	m_pSlider = NULL;
	m_bWantReturn = FALSE;
	m_bGotReturn = FALSE;
	m_bWantDlgNextCtrl = FALSE;
	m_bNotifyOnKillFocus = TRUE;

	m_hStatic = CreateWindow(
		CustomControls::GetWindowClassName(CustomControls::EDIT_DUMMY_WINDOW_CLASS),
		NULL,
		WS_CHILD | WS_VISIBLE,
		CE_HBORDER, CE_VBORDER, 
		(rect.right-rect.left)-CE_HBORDER*2, 
		(rect.bottom-rect.top)-CE_VBORDER*2,
		hWnd,
		NULL,
		(HINSTANCE)GetWindowLongPtr( hWnd, GWLP_HINSTANCE ),
		NULL );	
	
	::GetClientRect( m_hStatic, &rect );

	m_hEdit = CreateWindow(
		_T("EDIT"), NULL,
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT | ES_WANTRETURN | ES_MULTILINE,
		-m_xOff, -m_yOff,
		(rect.right-rect.left)+m_xOff*2, 
		(rect.bottom-rect.top)+m_yOff*2,
		m_hStatic,
		NULL,
		(HINSTANCE)GetWindowLongPtr( hWnd, GWLP_HINSTANCE ),
		NULL );			

	::SendMessage( m_hEdit, WM_SETFONT, (WPARAM)CustomControls::GetFixedFont(), MAKELONG(0, 0));

	if ( !CustomControls::GetEditWndProc() ) 
	{
		CustomControls::SetEditWndProc((WNDPROC)GetClassLongPtr( m_hEdit, GCLP_WNDPROC ));
	}
	SetWindowLongPtr( m_hEdit, GWLP_WNDPROC, (LONG_PTR)CustomControls::EditSubWndProc);
}


void CustomEditBox::SetBold(BOOL onOff) 
{
	::SendMessage( m_hEdit, WM_SETFONT, onOff?(WPARAM)CustomControls::GetFixedFontBold():(WPARAM)CustomControls::GetFixedFont(), MAKELONG(0, 0));
}

void CustomEditBox::SetNotifyOnKillFocus(BOOL onOff)
{
	m_bNotifyOnKillFocus = onOff;
}

void CustomEditBox::WantDlgNextCtl(BOOL yesNo)
{
	m_bWantDlgNextCtrl = yesNo;
}

BOOL CustomEditBox::HasFocus()
{
	return ::GetFocus()==m_hEdit;
}

void CustomEditBox::GiveFocus()
{
	SetFocus(m_hEdit);
}

void CustomEditBox::SetLeading(int lead)
{
	m_yOff = DEF_YOFFSET - lead;
	::SetWindowPos(m_hEdit,NULL,-m_xOff,-m_yOff,0,0,SWP_NOZORDER|SWP_NOSIZE);
}

void CustomEditBox::GetText( char *text, int ct )
{
	::GetWindowText(m_hEdit, text, ct );
}

void CustomEditBox::SetText( const char *text )
{	
	::SetWindowText(m_hEdit, text);
	if (::GetFocus()==m_hEdit) 
	{
		// Select all the text.
		SendMessage(m_hEdit, EM_SETSEL, 0, (WPARAM)((INT)-1));
	}
}

void CustomEditBox::SetText( int i )
{
	WinUtil::SetWindowTextInt(m_hEdit, i );
	if (GetFocus()==m_hEdit) 
	{
		// Select all the text.
		::SendMessage(m_hEdit, EM_SETSEL, 0, (WPARAM)((INT)-1));
	}
}

void CustomEditBox::SetText( float f, const char *pFormat )
{
	WinUtil::SetWindowTextFloat(m_hEdit, f, pFormat );
	if (GetFocus()==m_hEdit) 
	{
		// Select all the text.
		::SendMessage(m_hEdit, EM_SETSEL, 0, (WPARAM)((INT)-1));
	}
}

int CustomEditBox::GetInt(BOOL *valid)
{
	return WinUtil::GetWindowInt(m_hEdit, valid);
}

float CustomEditBox::GetFloat(BOOL *valid)
{
	return WinUtil::GetWindowFloat(m_hEdit, valid);
}

void CustomEditBox::UpdateEditWindow(void)
{
	::UpdateWindow(m_hEdit);
}

BOOL CustomEditBox::IsValidCharacter(int ch) 
{
	// If it's linked to a spinner, see if the spinner says it's an OK character
	if(m_pSpinner)
		return m_pSpinner->IsValidCharacter(ch);
	else if (m_pSlider)
		return m_pSlider->IsValidCharacter(ch);
	else
		return TRUE;
}

LRESULT CustomEditBox::OnPaint(void)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

	hdc = ::BeginPaint( m_hWnd, &ps );
	GetClientRect( m_hWnd, &rect );	
	
	if (IsWindowEnabled(m_hWnd) ) 
	{
		SelectObject(hdc, CustomControls::GetColorBrush( COLOR_WINDOW ) );
	}
	else 
	{
		SelectObject(hdc, CustomControls::GetColorBrush( COLOR_3DSHADOW ) );
	}
	
	WinUtil::WhiteRect3D( hdc, rect, TRUE );

	EndPaint(m_hWnd, &ps);
	return 0;
}

LRESULT	CustomEditBox::OnCommand( WORD id, WORD notify, HWND hCtrl )	
{

	switch (id) 
	{
		case ID_ATUPDATE_SPINNER:
			if (m_pSpinner) 
			{
				TCHAR buf[256];
				GetText(buf, 256);
				if(m_pSpinner->UpdateFromEdit())
				{
					SendMessage(GetParent(m_hWnd),WM_ATCUSTOMEDIT_ENTER,
						GetWindowLongPtr(m_hWnd,GWL_ID),(LONG_PTR)m_hWnd );
				}
			}
			else if (m_bNotifyOnKillFocus) 
			{
				SendMessage(GetParent(m_hWnd),WM_ATCUSTOMEDIT_ENTER,
					GetWindowLongPtr(m_hWnd,GWL_ID),(LONG_PTR)m_hWnd );
			}
			break;		
		case ID_ATUPDATE_SLIDER:
			if (m_pSlider)
			{				
				TCHAR buf[256];
				GetText(buf, 256);
				if( m_pSlider->UpdateFromEdit() )
				{
					SendMessage(GetParent(m_hWnd), WM_ATCUSTOMEDIT_ENTER, 
						GetWindowLongPtr(m_hWnd,GWL_ID),(LONG_PTR)m_hWnd );
				}
			}
			else if (m_bNotifyOnKillFocus) 
			{
				SendMessage(GetParent(m_hWnd),WM_ATCUSTOMEDIT_ENTER,
					GetWindowLongPtr(m_hWnd,GWL_ID),(LONG_PTR)m_hWnd );
			}
			break;		
	}
	return 0;
}

LRESULT CustomEditBox::OnRButDown( int x, int y, WORD keys )
{
	return 1;
}

LRESULT CustomEditBox::OnSetFocus( HWND hLost )
{
	::SetFocus( m_hEdit );
	SendMessage(m_hEdit, EM_SETSEL, 0, (WPARAM)((INT)-1));
	return 0;
}

LRESULT CustomEditBox::OnWMEnable( BOOL bEnabled ) 
{ 
	EnableWindow(m_hEdit,bEnabled);
	InvalidateRect(m_hWnd,NULL,TRUE);
	return 0; 
}

LRESULT CustomEditBox::OnSize( int w, int h ) //RK:11/29/00
{
	SetWindowPos(m_hStatic,NULL,0,0, w-CE_HBORDER*2, h-CE_VBORDER*2, SWP_NOZORDER|SWP_NOMOVE);	
	SetWindowPos(m_hEdit,NULL,0,0, w+m_xOff*2, h+m_yOff*2, SWP_NOZORDER|SWP_NOMOVE);
	return 0;
}

void CustomEditBox::SetSpinner(CustomSpinner *pSpinner)
{
	m_pSpinner = pSpinner;
}

void CustomEditBox::SetSlider(CustomSlider *pSlider)
{
	m_pSlider = pSlider;
}

void CustomEditBox::WantReturn(BOOL yesNo)
{
	m_bWantReturn = yesNo;
}

BOOL CustomEditBox::GotReturn(void)
{
	return m_bGotReturn;
}

HWND CustomEditBox::GetHEdit(void)
{
	return m_hEdit;
}

} //AttrControls 

