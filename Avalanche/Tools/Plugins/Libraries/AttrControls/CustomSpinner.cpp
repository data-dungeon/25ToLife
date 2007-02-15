//
//	CustomSpinner
//	

/*
 * $History: CustomSpinner.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 1/14/03    Time: 4:00p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * release build problems with using member variables as parameters in
 * constructor
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 9:58a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new controls to numeric type.
 * Fixed color issues with Max
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 1:18p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * all compiling
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 */

#include "stdafx.h"
#include "CustomSpinner.h"
#include "ICustomSpinnerImp.h"
#include "CustomEditBox.h"
#include "CustomControls.h"
#include "WinUtil.h"

namespace AttrControls
{

CustomSpinner::CustomSpinner( HWND hWnd, float value, float scale ) : CustomControl(hWnd) 
{
	m_value = value;
	m_scale = scale;
	m_bDown  = 0;
	m_hEdit = NULL;
	m_type = ICustomSpinner::EDITTYPE_INT;
	m_min = m_max = value;		 
	m_buttonStatus = BUT_OUT;
	m_bIndeterminate = FALSE;
	m_bAutoScale = FALSE;
	m_reset = 0.0f;
	m_bKeyBrackets = FALSE;
	m_bNeedNotify = FALSE;
	m_bSnapSpinner = FALSE;
	m_spinSnapValue = 1.0f;
	m_spinnerFormat = ".3";
	m_bSpinWrap = FALSE;
	m_bDragNotify = TRUE;

	RECT rect;
	GetClientRect(m_hWnd,&rect);
	if ((rect.right-rect.left) != 12 || (rect.bottom-rect.top) != 16) 
	{
		SetWindowPos(m_hWnd,NULL,0,0,12,16,SWP_NOZORDER|SWP_NOMOVE);
	}
}

CustomSpinner::~CustomSpinner() 
{
}

void CustomSpinner::SetIndeterminate(BOOL i)
{
	m_bIndeterminate = i;
	UpdateEdit();
}

BOOL CustomSpinner::IsIndeterminate()
{
	return m_bIndeterminate;
}

void CustomSpinner::LinkToEdit( HWND hEdit, ICustomControl::EditSpinnerType type )
{
	CustomEditBox *pEditControl = (CustomEditBox *)GetWindowLongPtr( hEdit, GWLP_USERDATA );		
	if ( !pEditControl ) return;
	pEditControl->SetSpinner( this );
	m_hEdit = hEdit;

	m_type  = type;	
	
	UpdateEdit();	
}

BOOL CustomSpinner::IsValidCharacter(int ch) 
{
	// We'll check the special characters here...
	if(ch == VK_BACK)
		return TRUE;

	if (!(_istdigit(ch) || ch=='.' || ch==',' 
			|| ch=='-' || ch==':' || ch =='\'' || ch=='\"' 
			|| ch==3 || ch==22 || ch==24)) 
	{
		return FALSE;
	}
	return TRUE;
}

void CustomSpinner::UpdateEdit()
{
	if ( m_hEdit ) 
	{
		CustomEditBox *pEditControl = (CustomEditBox *)GetWindowLongPtr( m_hEdit, GWLP_USERDATA );		
		assert(pEditControl);
		if (!pEditControl) return;

		if (m_bIndeterminate) 
		{
			pEditControl->SetText(_T(""));
			return;
		}
		switch(m_type) 
		{
			case ICustomControl::EDITTYPE_INT:
				pEditControl->SetText( GetIVal() );
				break;
			case ICustomControl::EDITTYPE_FLOAT:
				pEditControl->SetText( GetFVal(), m_spinnerFormat);
				break;
		}				
		pEditControl->UpdateEditWindow();
	}
}

BOOL CustomSpinner::UpdateFromEdit(void)
{
	BOOL bChanged = FALSE;
	if (m_hEdit) 
	{
		TCHAR buf[256];
		CustomEditBox *pEditControl = (CustomEditBox *)GetWindowLongPtr( m_hEdit, GWLP_USERDATA );		
		assert(pEditControl);	
		pEditControl->GetText(buf,256);
		BOOL bValid = TRUE;
		
		float newVal = m_value;

		// we're leaving the edit control, and user has left it blank.
		// set the value back to the original value and update the edit box
		if (buf[0]=='\0') 
		{
			if (!m_bIndeterminate) 
			{
				// Don't use indeterminate value! -J.Zhao, 5/23/01
				SetValue(m_value,TRUE);
				UpdateEdit();
			}
			return FALSE;
		}

		switch(m_type)
		{
			case ICustomControl::EDITTYPE_INT:
				newVal = (float)pEditControl->GetInt(&bValid);
				break;
			case ICustomControl::EDITTYPE_FLOAT:
				newVal = pEditControl->GetFloat(&bValid);
				break;
		}

		if (bValid && (newVal!=m_value || m_bIndeterminate)) 
		{
			bChanged = TRUE;
			SetValue(newVal,TRUE);			
		}
		
		m_bIndeterminate = FALSE;		
		UpdateEdit();
	}
	return bChanged;
}

void CustomSpinner::SetLimits( int min, int max, BOOL limitCurValue )
{
	SetLimits( float(min), float(max), limitCurValue );
}

void CustomSpinner::SetLimits( float min, float max, BOOL limitCurValue )
{
	m_min = min;
	m_max = max;

	if ( limitCurValue ) 
	{
		ICustomSpinnerImp ispin(this);

		if ( m_value < m_min ) 
		{
			m_value = m_min;
			UpdateEdit();
			SendMessage( GetParent(m_hWnd), CC_ATSPINNER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE),(LPARAM)&ispin);
		}
		if ( m_value > m_max ) 
		{
			m_value = m_max;
			UpdateEdit();
			SendMessage( GetParent(m_hWnd), CC_ATSPINNER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE), (LPARAM)&ispin);
		}
	}
}

void CustomSpinner::SetResetValue(float v)
{
	m_reset = v;
}

void CustomSpinner::SetResetValue(int v)
{
	m_reset = float(v);
}
			
void CustomSpinner::SetValue( float v, BOOL bNotify ) 
{ 
	float ov = m_value;

	m_value = v;
	LimitValue();

	if (m_value != ov || m_bIndeterminate) 
	{
		UpdateEdit(); 
		if (bNotify) 
		{
			ICustomSpinnerImp ispin(this);
			SendMessage( GetParent(m_hWnd), CC_ATSPINNER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE),(LPARAM)&ispin );
		}
	}
}

float CustomSpinner::GetFVal(void) 
{ 
	return m_value; 
}

int CustomSpinner::GetIVal(void) 
{ 
	return int(floor(m_value+0.5f));
}

void CustomSpinner::LimitValue(void)
{
	if ( m_value < m_min ) m_value = m_min;
	if( m_value > m_max ) m_value = m_max;
}

void CustomSpinner::SetValue( int v, BOOL bNotify ) 
{ 
	SetValue( float(v), bNotify );
}

void CustomSpinner::Increment(void)
{	
	float s = m_scale;
	if (m_bAutoScale) 
	{
		s = float(fabs(m_value*AUTOSCALE_FACTOR));
		if (s<0.01f) s = 0.01f;
	}
	if (m_bSnapSpinner)
		s = m_spinSnapValue;
	if (m_type == ICustomControl::EDITTYPE_INT && s < float(1) ) 
	{
		s = float(1);
	}

	// RB: Set the value ourselves so the spinner message can indicate 
	// that the mouse button is down.
	//SetValue( value + s, TRUE );

	m_value += s;
	LimitValue();
	ICustomSpinnerImp ispin(this);
	UpdateEdit();			
	SendMessage( GetParent(m_hWnd), CC_ATSPINNER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE),(LPARAM)&ispin);
}

void CustomSpinner::Decrement(void)
{
	float s = m_scale;
	if (m_bAutoScale) 
	{
		s = float(fabs(m_value*AUTOSCALE_FACTOR));
		if (s<0.01f) s = 0.01f;
	}
	if (m_bSnapSpinner)
		s = m_spinSnapValue;
	if (m_type == ICustomControl::EDITTYPE_INT && s < float(1) ) 
	{
		s = float(1);
	}

	// RB: Set the value ourselves so the spinner message can indicate 
	// that the mouse button is down.
	//SetValue( newValue, TRUE );

	m_value -= s;
	LimitValue();
	ICustomSpinnerImp ispin(this);
	UpdateEdit();			
	SendMessage( GetParent(m_hWnd), CC_ATSPINNER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE),(LPARAM)&ispin);
}

void CustomSpinner::SetKeyBrackets(BOOL onOff)
{
	if (m_bKeyBrackets==onOff) 
		return;
	m_bKeyBrackets = onOff;
	InvalidateRect(m_hWnd, NULL, TRUE);
}

void CustomSpinner::SetAutoScale(BOOL on)
{
	m_bAutoScale = on;
}

void CustomSpinner::SetScale( float s )
{
	m_scale = s;
}

void CustomSpinner::SetSnapSpinner(BOOL b)
{
	m_bSnapSpinner = b;
}

BOOL CustomSpinner::GetSnapSpinner(void)
{
	return m_bSnapSpinner;
}

void CustomSpinner::SetSnapSpinValue(float f)
{
	m_spinSnapValue = f;
}

float CustomSpinner::GetSnapSpinValue(void)
{
	return m_spinSnapValue;
}

void CustomSpinner::SetSpinnerFormat(const char *pFormat)
{
	m_spinnerFormat = pFormat;
}

const char *CustomSpinner::GetSpinnerFormat(void)
{
	return m_spinnerFormat;
}

void CustomSpinner::SetSpinnerWrap(BOOL b)
{
	m_bSpinWrap = b;
}

BOOL CustomSpinner::GetSpinnerWrap(void)
{
	return m_bSpinWrap;
}

void CustomSpinner::SetDragNotify(BOOL b)
{
	m_bDragNotify = b;
}

BOOL CustomSpinner::GetDragNotify(void)
{
	return m_bDragNotify;
}


LRESULT CustomSpinner::OnPaint() 
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint( m_hWnd, &ps );

	RECT rect;
	HPEN hTextPen = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNTEXT ) );
	HPEN hDarkPen = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNSHADOW ) );

	rect.left = 0;
	rect.top = 0;
	rect.right = 12;
	rect.bottom = 9;
	WinUtil::DrawButton( hdc, rect, m_buttonStatus == BUT_IN || m_buttonStatus == TOPBUT_IN);
	rect.top = 8;
	rect.bottom = 16;
	WinUtil::DrawButton( hdc, rect, m_buttonStatus == BUT_IN || m_buttonStatus == BOTBUT_IN);

	WinUtil::DrawTriangle( hdc, 3, 3, hTextPen, hDarkPen, 0);
	WinUtil::DrawTriangle( hdc, 3, 10, hTextPen, hDarkPen, 1);

	SelectObject(hdc, GetStockObject(BLACK_PEN));
	DeleteObject(hTextPen);
	DeleteObject(hDarkPen);


	if (m_bKeyBrackets) 
	{
#define BRACKETSIZE	3
		RECT rect;
		GetClientRect(m_hWnd,&rect);
		//SelectObject(hdc, GetStockObject(BLACK_PEN));
		//SelectObject(hdc, GetStockObject(NULL_BRUSH));
		//Rectangle(hdc, 0, 0, rect.right-1, rect.bottom-1);
		HPEN hRedPen = CreatePen(PS_SOLID,0,RGB(255,0,0));
		SelectObject(hdc, hRedPen/*GetStockObject(WHITE_PEN)*/);
		MoveToEx(hdc, 0, BRACKETSIZE, NULL);
		LineTo  (hdc, 0, 0);
		LineTo  (hdc, BRACKETSIZE, 0);
		
		MoveToEx(hdc, rect.right-1-BRACKETSIZE, 0, NULL);
		LineTo(hdc, rect.right-1, 0);
		LineTo(hdc, rect.right-1, BRACKETSIZE);

		MoveToEx(hdc, rect.right-1, rect.bottom-1-BRACKETSIZE, NULL);
		LineTo(hdc, rect.right-1, rect.bottom-1);
		LineTo(hdc, rect.right-1-BRACKETSIZE, rect.bottom-1);

		MoveToEx(hdc, BRACKETSIZE, rect.bottom-1, NULL);
		LineTo(hdc, 0, rect.bottom-1);
		LineTo(hdc, 0, rect.bottom-1-BRACKETSIZE);
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		DeleteObject(hRedPen);
	}
	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT	CustomSpinner::OnCommand( WORD notify, WORD id, HWND hCtrl )
{	
	return 0;
}

LRESULT CustomSpinner::OnTimer( UINT nIDEvent )
{
	// First timer is the delay. Set the repeat timer.
	if ( nIDEvent == 1 ) 
	{
		KillTimer(m_hWnd,nIDEvent);
		SetTimer(m_hWnd,2,SPINNER_REPEATTIME,NULL);
		return 0;
	}

	switch ( m_buttonStatus ) 
	{
		case TOPBUT_IN:
			Increment();
			break;
		case BOTBUT_IN:
			Decrement();
			break;
		default:
			KillTimer(m_hWnd,nIDEvent);
			break;
	}
	return 0;
}
		
LRESULT	CustomSpinner::OnRButDown( int x, int y, WORD keys )
{
	if (m_bDown) 
	{
		if ( m_value != m_originalValue ) 
		{
			SetValue( m_originalValue, TRUE );
		}

		m_bDown = FALSE;
		ReleaseCapture();
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		m_buttonStatus = BUT_OUT;
		InvalidateRect( m_hWnd, NULL, TRUE );
		
		ICustomSpinnerImp ispin(this);			
		SendMessage( GetParent(m_hWnd), CC_ATSPINNER_BUTTONUP, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE),(LPARAM)&ispin );
	}
	else
	{
		if (keys&MK_SHIFT) 
		{
			ICustomSpinnerImp ispin(this);
			SendMessage( GetParent(m_hWnd), CC_ATSPINNER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE),(LPARAM)&ispin );
			SendMessage( GetParent(m_hWnd), CC_ATSPINNER_BUTTONUP, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE),(LPARAM)&ispin );
		} 
		else 
		{
			// Send a BUTTON_UP message so the theHold.Accept() gets called.
			ICustomSpinnerImp ispin(this);
			SendMessage( GetParent(m_hWnd), CC_ATSPINNER_BUTTONDOWN, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE),(LPARAM)&ispin );		
			SetValue(m_reset,TRUE);
			SendMessage( GetParent(m_hWnd), CC_ATSPINNER_BUTTONUP, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE),(LPARAM)&ispin );
		}
	}
	return 0;
}

LRESULT	CustomSpinner::OnLButDown( int x, int y, WORD keys )
	{
	BOOL alt =  GetKeyState(VK_MENU)&0x8000 ? TRUE : FALSE;

	if ( !m_bDown ) 
	{
		m_originalX = x;
		m_originalY = y;
		m_originalValue  = m_value;
		m_bDown = TRUE;
		SetCapture( m_hWnd );		
//so the spinner gets allthe keyboord messages
//in some cases like if a command mode button is pressed before a spinner was the command mode was getting
//messages which was screwing things up
		::SetFocus(m_hWnd);
		m_bIndeterminate = FALSE;
		ICustomSpinnerImp ispin(this);			
		SendMessage( GetParent(m_hWnd), CC_ATSPINNER_BUTTONDOWN, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE),(LPARAM)&ispin );

		if (keys&MK_CONTROL || alt) 
		{
			m_buttonStatus = BUT_IN;
			InvalidateRect( m_hWnd, NULL, TRUE );
			UpdateWindow(m_hWnd);
			SetCursor(CustomControls::GetSpinCur());
		} 
		else 
		{
			if ( y < 8 ) 
			{
			 	m_buttonStatus = TOPBUT_IN;
				InvalidateRect( m_hWnd, NULL, TRUE );
				UpdateWindow(m_hWnd);
				Increment();
			} 
			else 
			{
				m_buttonStatus = BOTBUT_IN;
				InvalidateRect( m_hWnd, NULL, TRUE );
				UpdateWindow(m_hWnd);
				Decrement();
			}
		
			SetTimer(m_hWnd,1,SPINNER_DELAYTIME,NULL);		
		}
	}
	return 0;
}

LRESULT	CustomSpinner::OnMouseMove( int x, int y, WORD keys )
{
	BOOL alt =  GetKeyState(VK_MENU)&0x8000 ? TRUE : FALSE;

	if (m_bDown) 
	{
		if (abs(y-m_originalY) < SPINMOVE_THRESH && m_buttonStatus != BUT_IN) 
		{
			return 0;
		}
		if (m_buttonStatus != BUT_IN) 
		{
			m_buttonStatus = BUT_IN;
			InvalidateRect(m_hWnd, NULL, TRUE);
			UpdateWindow(m_hWnd);
			SetCursor( CustomControls::GetSpinCur() );
		}

		float ov = m_value, s = m_scale;
		if (m_bAutoScale) 
		{
			s = float(fabs(m_originalValue * AUTOSCALE_FACTOR));
			if (s<0.01f) s = 0.01f;
		}

		if (keys&MK_CONTROL)   s *= 10.0f;
		if (alt) s *= .1f;
				
		m_value = m_originalValue - float(y-m_originalY) * s;

		LimitValue();				
		if (m_originalValue != m_value) 
		{
			ICustomSpinnerImp ispin(this);
			UpdateEdit();			
			if (m_bDragNotify) 
			{
				SendMessage( GetParent(m_hWnd), CC_ATSPINNER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE), (LPARAM)&ispin );
			} 
			else 
			{
				m_bNeedNotify = TRUE;
			}
		}

		// Let the mouse wrap around when we hit the bottom 
		// or top of the screen.

#define	WRAP_MARGIN				10

		int sy = GetSystemMetrics(SM_CYSCREEN);
		POINT pt;
		pt.x = x;
		pt.y = y;
		ClientToScreen(m_hWnd,&pt);

		if (pt.y<WRAP_MARGIN) 
		{
			m_originalY  += sy-WRAP_MARGIN-pt.y;
			pt.y = sy-WRAP_MARGIN;
			SetCursorPos(pt.x, pt.y);
		} 
		else if (pt.y>sy-WRAP_MARGIN) 
		{			
			m_originalY  += WRAP_MARGIN-pt.y;
			pt.y = WRAP_MARGIN;			
			SetCursorPos(pt.x, pt.y);
		}

		// check if the "local spinner wrap" is on.  If so, keep the
		// cursor near the spinner. 

#define LOCAL_WRAP_DISTANCE		40

		if(m_bSpinWrap) 
		{
			RECT rect;
			GetWindowRect(m_hWnd, &rect);
			if (pt.y < rect.top - LOCAL_WRAP_DISTANCE) 
			{
				// cursor is too far above spinner
				m_originalY += 2*LOCAL_WRAP_DISTANCE;
				pt.y += 2*LOCAL_WRAP_DISTANCE;
				SetCursorPos(pt.x, pt.y);
			} 
			else if (pt.y > rect.bottom + LOCAL_WRAP_DISTANCE) 
			{
				// cursor is too far below spinner
				m_originalY  -= 2*LOCAL_WRAP_DISTANCE;
				pt.y -= 2*LOCAL_WRAP_DISTANCE;
				SetCursorPos(pt.x, pt.y);
			}
			return 0;
		}

	}
	return 0;
}

LRESULT	CustomSpinner::OnLButUp( int x, int y, WORD keys )
{
	if ( m_bDown ) 
	{
		m_bDown = FALSE;
		ReleaseCapture();
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		m_buttonStatus = BUT_OUT;
		InvalidateRect( m_hWnd, NULL, TRUE );
		
		ICustomSpinnerImp ispin(this);			
		if (!m_bDragNotify && m_bNeedNotify) 
		{
			m_bNeedNotify = FALSE;
			SendMessage( GetParent(m_hWnd), CC_ATSPINNER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE),(LPARAM)&ispin );
		}
		SendMessage( GetParent(m_hWnd), CC_ATSPINNER_BUTTONUP, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE), (LPARAM)&ispin );
	}
	return 0;
}

LRESULT	CustomSpinner::OnCaptureReleased( HWND hwnd )
{
	if ( m_bDown ) 
	{
		m_bDown = FALSE;
		ReleaseCapture();
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		m_buttonStatus = BUT_OUT;
		InvalidateRect( m_hWnd, NULL, TRUE );
		
		ICustomSpinnerImp ispin(this);			
		if (!m_bDragNotify && m_bNeedNotify) 
		{
			m_bNeedNotify = FALSE;
			SendMessage( GetParent(m_hWnd), CC_ATSPINNER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE),(LPARAM)&ispin );
		}
		SendMessage( GetParent(m_hWnd), CC_ATSPINNER_BUTTONUP, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE), (LPARAM)&ispin );
	}
	return 0;
}


LRESULT CustomSpinner::OnWMEnable( BOOL bEnabled )
{
	if (bEnabled && m_hEdit) 
	{
		CustomEditBox *pEditControl = (CustomEditBox *)GetWindowLongPtr( m_hEdit, GWLP_USERDATA );
		pEditControl->Enable();
 	} 
	else if (m_hEdit) 
	{
		CustomEditBox *pEditControl = (CustomEditBox *)GetWindowLongPtr( m_hEdit, GWLP_USERDATA );
		pEditControl->Disable();
	}
	InvalidateRect(m_hWnd,NULL,TRUE);
	return 0;
}

} // AttrControls
