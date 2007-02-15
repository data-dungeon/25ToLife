//
//	CustomSpinner
//	

/*
 * $History: CustomSlider.cpp $
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
#include "CustomSlider.h"
#include "ICustomSliderImp.h"
#include "CustomEditBox.h"
#include "CustomControls.h"

namespace AttrControls
{

#define SLIDER_SNAP_THRESH	0.5

CustomSlider::CustomSlider( HWND hWnd, float value, int numSegs ) : CustomControl(hWnd)
{
	m_value = value;
	m_numSegs = numSegs;
	m_bDown  = false;
	m_hEdit = NULL;
	m_type = ICustomControl::EDITTYPE_INT;
	m_min = m_max = value;
	m_range = 0.0f;
	m_bIndeterminate = FALSE;
	m_oldVal = m_resetVal = value;
	m_bKeyBrackets = FALSE;
	m_bNeedNotify = FALSE;
	GetClientRect(m_hWnd,&m_rect);
	m_radius = 5;
	m_rect.left += m_radius;
	m_rect.right -= (m_radius+1);
	m_bDragNotify = TRUE;
	m_sliderFormat = ".3";
}

CustomSlider::~CustomSlider()
{
}

void CustomSlider::SetIndeterminate(BOOL i)
{
	m_bIndeterminate = i;
	UpdateEdit();
}

BOOL CustomSlider::IsIndeterminate()
{
	return m_bIndeterminate;
}

void CustomSlider::LinkToEdit( HWND m_hEdit, ICustomControl::EditSpinnerType type )
{
	CustomEditBox *pControl = (CustomEditBox *)GetWindowLongPtr( m_hEdit, GWLP_USERDATA );		
	if (!pControl)
		return;
	pControl->SetSlider( this );
	this->m_hEdit = m_hEdit;
	m_type  = type;	
	UpdateEdit();	
}

BOOL CustomSlider::IsValidCharacter(int ch)
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

void CustomSlider::UpdateEdit()
{
	if ( m_hEdit )
	{
		CustomEditBox *pEditControl = (CustomEditBox *)GetWindowLongPtr( m_hEdit, GWLP_USERDATA );		
		assert(pEditControl);
		if (!pEditControl)
			return;

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
				pEditControl->SetText( GetFVal(), m_sliderFormat ); 
				break;
		}				
		pEditControl->UpdateEditWindow();
	}
}

BOOL CustomSlider::UpdateFromEdit(void)
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

		// 4/25/01 11:45am --MQM-- 
		// we're leaving the edit control, and user has left it blank.
		// set the value back to the original value and update the edit box
		if (buf[0]=='\0') 
		{
			if (!m_bIndeterminate) 
			{
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
			SetValue(newVal, TRUE);
		}
		
		m_bIndeterminate = FALSE;		
		UpdateEdit();
	}
	return bChanged;
}

void CustomSlider::SetLimits( int min, int max, BOOL bLimitCurValue )
{
	SetLimits( float(min), float(max), bLimitCurValue );
}

void CustomSlider::SetLimits( float min, float max, BOOL bLimitCurValue )
{
	m_min = min;
	m_max = max;
	m_range = max - min;

	if ( bLimitCurValue )
	{
		ICustomSliderImp islider(this);

		if ( m_value < m_min )
		{
			m_value = m_min;
			UpdateEdit();
			SendMessage( GetParent(m_hWnd), CC_ATSLIDER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE), (LPARAM)&islider );
		}
		
		if ( m_value > m_max )
		{
			m_value = m_max;
			UpdateEdit();
			SendMessage( GetParent(m_hWnd), CC_ATSLIDER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE), (LPARAM)&islider );
		}
	}
}

void CustomSlider::SetResetValue(float v)
{
	m_resetVal = v;
}
			
void CustomSlider::CalcKnobRect(RECT &knobRect, float val)
{
	knobRect.top = m_rect.top;
	knobRect.bottom = m_rect.bottom;
	knobRect.left = XFromVal(val) - 2*m_radius;
	knobRect.right = knobRect.left + 4*m_radius;
}

int CustomSlider::XFromVal(float value)
{
	if (value<m_min)
		value = m_min;
	if (value>m_max)
		value = m_max;

	int controlWidth = m_rect.right-m_rect.left;
	if (m_min<m_max)
	{
		float percent = (value-m_min)/(m_max-m_min);
		int pos = (int)(m_rect.left+(controlWidth*percent));
		return pos;
	}
	return m_rect.left;
}

int CustomSlider::XFromPercent(float percent)
{
	int controlWidth = m_rect.right-m_rect.left;
	if (m_min<m_max)
	{
		int pos = (int)(m_rect.left+(controlWidth*percent));
		return pos;
	}
	return m_rect.left;
}

float CustomSlider::ValueFromX(int x)
{
	int controlWidth = m_rect.right-m_rect.left;
	if (m_min<m_max)
	{
		float percent = (float(x-m_rect.left))/(controlWidth);
		return m_min+((m_max-m_min)*percent);
	}
	return m_min;
}

float CustomSlider::PercentFromX(int x)
{
	int controlWidth = m_rect.right-m_rect.left;
	if (m_min<m_max)
	{
		float percent = (float(x-m_rect.left))/(controlWidth);
		return percent;
	}
	return 0.0f;
}

float CustomSlider::ValueFromPercent(float percent)
{
	if (m_min<m_max)
	{
		return m_min+((m_max-m_min)*percent);
	}
	return m_min;
}

void CustomSlider::SetValue( float v, BOOL bNotify ) 
{ 
	static float ov;
	ov = m_value;
	static RECT knobRect;

	m_value = v;
	LimitValue();

	if (m_value != ov || m_bIndeterminate)
	{		
		CalcKnobRect(knobRect, ov);
		InvalidateRect(m_hWnd, &knobRect, TRUE);
		CalcKnobRect(knobRect, m_value);
		InvalidateRect(m_hWnd, &knobRect, TRUE);
		UpdateWindow(m_hWnd);
		UpdateEdit();
		if ( bNotify )
		{
			ICustomSliderImp islider(this);
			SendMessage( GetParent(m_hWnd), CC_ATSLIDER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE), (LPARAM)&islider );
		}
	}
}

float CustomSlider::GetFVal(void)
{ 
	return m_value; 
}

int CustomSlider::GetIVal(void) 
{ 
	return int( floor(m_value+0.5f) ); // round
}

void CustomSlider::LimitValue(void)
{
	if ( m_value < m_min )
		m_value = m_min;
	if( m_value > m_max )
		m_value = m_max;
}

void CustomSlider::SetValue( int v, BOOL bNotify ) 
{ 
	SetValue( float(v), bNotify );
}

void CustomSlider::SetKeyBrackets(BOOL onOff)
{
	if (m_bKeyBrackets==onOff)
		return;
	m_bKeyBrackets = onOff;
	InvalidateRect(m_hWnd, NULL, TRUE);
}

LRESULT CustomSlider::OnPaint(void) 
{
	static float percent;
	static int x, y;
	static POINT pts[5];
	static RECT rc;
	static PAINTSTRUCT ps;
	static HDC hdc;
	static HBRUSH hBrush;
	static HPEN hLightPen, hDarkPen, hRedPen;
	hdc = BeginPaint( m_hWnd, &ps );

	hBrush  = CustomControls::GetColorBrush(COLOR_BTNFACE);
	hLightPen = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNHILIGHT ) );
	hDarkPen  = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNSHADOW ) );

	x = XFromVal(m_value);
	y = m_rect.bottom-m_radius-5; // 'radius' - knob used to be circular

	SetROP2( hdc, R2_COPYPEN );
	SetBkMode( hdc, OPAQUE );

	// ticks
	if (m_numSegs > 0)
	{
		static tx, ty, oldX, resetX;
		SelectObject( hdc, GetStockObject(BLACK_PEN) );
		ty = y-6;
		percent = 1.0f/m_numSegs;
		for (int i=0; i<=m_numSegs; i++)
		{
			tx = XFromPercent(i*percent);
			rc.left = tx-1; rc.right = tx+1;
			rc.bottom = ty-1; rc.top = ty+1;
			FillRect( hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH) );
		}

		if (m_bDown)
		{
			SelectObject( hdc, hBrush );
			tx = XFromVal(m_oldVal);
			MoveToEx(hdc, tx-2, ty-3, NULL);
			LineTo(hdc, tx-2, ty+3);
			LineTo(hdc, tx+2, ty+3);
			LineTo(hdc, tx+2, ty-3);
			LineTo(hdc, tx-2, ty-3);
		}

		tx = XFromVal(m_resetVal);
		rc.left = tx-2; rc.right = tx+2;
		rc.bottom = ty-3; rc.top = ty+3;
		FillRect( hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH) );
	}

	// track
	SelectObject( hdc, hDarkPen );
	MoveToEx(hdc, m_rect.left, y+1, NULL);
	LineTo(hdc, m_rect.left, y-1);
	LineTo(hdc, m_rect.right, y-1);
	SelectObject( hdc, hLightPen );
	LineTo(hdc, m_rect.right, y+1);
	LineTo(hdc, m_rect.left, y+1);

	SelectObject(hdc, hBrush);

	// pointer
	pts[0].x = x-3;	pts[0].y = y-9;
	pts[1].x = x-3;	pts[1].y = y+3;
	pts[2].x = x+3;	pts[2].y = y+3;
	pts[3].x = x+3;	pts[3].y = y-9;
	pts[4].x = x;	pts[4].y = y-12;
	Polygon( hdc, (POINT *)&pts, 5);
	SelectObject( hdc, hDarkPen );
	MoveToEx(hdc, x-3, y+3, NULL);
	LineTo(hdc, x+3, y+3);
	LineTo(hdc, x+3, y-9);
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	MoveToEx(hdc, x-3, y+4, NULL);
	LineTo(hdc, x+4, y+4);
	LineTo(hdc, x+4, y-9);

	if (m_bKeyBrackets)
	{
		SelectObject( hdc, hRedPen = CreatePen( PS_SOLID, 0, RGB(255,0,0) ) );
		MoveToEx(hdc, x-2, y-13, NULL);
		LineTo(hdc, x-5, y-13);
		LineTo(hdc, x-5, y-10);
		MoveToEx(hdc, x-5, y+2, NULL);
		LineTo(hdc, x-5, y+5);
		LineTo(hdc, x-2, y+5);
		MoveToEx(hdc, x+2, y+5, NULL);
		LineTo(hdc, x+5, y+5);
		LineTo(hdc, x+5, y+2);
		MoveToEx(hdc, x+5, y-10, NULL);
		LineTo(hdc, x+5, y-13);
		LineTo(hdc, x+2, y-13);
		DeleteObject(hRedPen);
	}

	DeleteObject(hLightPen);
	DeleteObject(hDarkPen);

	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT	CustomSlider::OnCommand( WORD notify, WORD id, HWND hCtrl )
{	
	return 0;
}

LRESULT CustomSlider::OnTimer( UINT nIDEvent )
{
	KillTimer(m_hWnd,nIDEvent);
	return 0;
}
		
LRESULT	CustomSlider::OnRButDown( int x, int y, WORD keys )
{
	if (m_bDown)
	{
		if ( m_value != m_oldVal )
			SetValue(m_oldVal, TRUE);

		m_bDown = false;
		ReleaseCapture();
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		
		ICustomSliderImp islider(this);			
		SendMessage( GetParent(m_hWnd), CC_ATSLIDER_BUTTONUP, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE), (LPARAM)&islider );
	}
	else
	{
		if (keys&MK_SHIFT)
		{
			ICustomSliderImp islider(this);
			SendMessage( GetParent(m_hWnd), CC_ATSLIDER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE), (LPARAM)&islider );
			SendMessage( GetParent(m_hWnd), CC_ATSLIDER_BUTTONUP, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE), (LPARAM)&islider );
		}
		else
		{
			ICustomSliderImp islider(this);
			SendMessage( GetParent(m_hWnd), CC_ATSLIDER_BUTTONDOWN, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE), (LPARAM)&islider );			
			SetValue(m_resetVal, TRUE);
			SendMessage( GetParent(m_hWnd), CC_ATSLIDER_BUTTONUP, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE), (LPARAM)&islider );
		}
	}
	return 0;
}

LRESULT	CustomSlider::OnLButDown( int x, int y, WORD keys )
{
	if ( !m_bDown )
	{
		m_bDown = true;
		SetCapture(m_hWnd);		
		SetCursor(CustomControls::GetSliderCur());
		m_bIndeterminate = FALSE;

		m_oldVal = m_value;

		// Send a BUTTON_DOWN message so the theHold.Accept() gets called.
		ICustomSliderImp islider(this);
		SendMessage( GetParent(m_hWnd), CC_ATSLIDER_BUTTONDOWN, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),FALSE), (LPARAM)&islider );			

		x = (x < m_rect.left) ? m_rect.left : (x > m_rect.right) ? m_rect.right : x;

		float val = ValueFromX(x);
		if (val != m_value)
		{
			SetValue(val, FALSE);
			if (m_bDragNotify)
			{
				ICustomSliderImp islider(this);
				SendMessage( GetParent(m_hWnd),  CC_ATSLIDER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE), (LPARAM)&islider );
			}
			else
			{
				m_bNeedNotify = TRUE;
			}
		}
	}
	return 0;
}

LRESULT	CustomSlider::OnMouseMove( int x, int y, WORD keys )
{
	static float val, percent;
	if (m_bDown)
	{
		x = (x < m_rect.left) ? m_rect.left : (x > m_rect.right) ? m_rect.right : x;

		if (keys&MK_CONTROL)
		// snap
		{
			percent = PercentFromX(x);
			double lower = floor(percent*m_numSegs)/m_numSegs;
			double relPos = (percent - lower)*m_numSegs;
			if ( relPos <= SLIDER_SNAP_THRESH )
				// snap to lower
				val = ValueFromPercent( float(lower) );
			else if (relPos > (1.0 - SLIDER_SNAP_THRESH) )
				// snap to higher
				val = ValueFromPercent( float(lower+1.0/m_numSegs) );
			else
				val = ValueFromPercent(percent);
		}
		else
		{
			val = ValueFromX(x);
		}

		if (val != m_value)
		{
			SetValue(val, FALSE);
			if (m_bDragNotify)
			{
				ICustomSliderImp islider(this);
				SendMessage( GetParent(m_hWnd),CC_ATSLIDER_CHANGE,MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE),(LPARAM)&islider );
			}
			else
			{
				m_bNeedNotify = TRUE;
			}
		}
	}
	return 0;
}

LRESULT	CustomSlider::OnLButUp( int x, int y, WORD keys)
{
	if ( m_bDown )
	{
		m_bDown = false;
		ReleaseCapture();
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		InvalidateRect( m_hWnd, NULL, TRUE );

		ICustomSliderImp islider(this);
		if (!m_bDragNotify && m_bNeedNotify)
		{
			m_bNeedNotify = FALSE;
			SendMessage( GetParent(m_hWnd), CC_ATSLIDER_CHANGE, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE), (LPARAM)&islider );
		}
		SendMessage( GetParent(m_hWnd), CC_ATSLIDER_BUTTONUP, MAKEWPARAM(GetWindowLongPtr(m_hWnd,GWL_ID),TRUE), (LPARAM)&islider );
	}
	return 0;
}

LRESULT CustomSlider::OnWMEnable( BOOL bEnabled )
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

void CustomSlider::SetNumSegs( int num )
{
	m_numSegs = num;
	InvalidateRect(m_hWnd, NULL, TRUE);
}

void CustomSlider::SetResetValue(int v)
{
	SetResetValue((float) v);
}

void CustomSlider::SetDragNotify(BOOL onOff)
{
	m_bDragNotify = onOff;
}

BOOL CustomSlider::GetDragNotify(void)
{
	return m_bDragNotify;
}

void CustomSlider::SetSliderFormat(const char *pFormat)
{
	m_sliderFormat = pFormat;
}

const char *CustomSlider::GetSliderFormat(void)
{
	return m_sliderFormat;
}

} //AttrControls
