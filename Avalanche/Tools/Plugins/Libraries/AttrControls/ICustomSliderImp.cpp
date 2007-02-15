//
//	ICustomSliderImp
//	

/*
 * $History: ICustomSliderImp.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 */

#include "stdafx.h"
#include "ICustomSliderImp.h"
#include "CustomSlider.h"

namespace AttrControls 
{

// ICustomSlider Methods
ICustomSlider *ICustomSlider::GetISlider( HWND hCtrl )
{
	CustomSlider *pControl = (CustomSlider *)GetWindowLongPtr( hCtrl, GWLP_USERDATA );
	if ( !pControl )
		return NULL;
	return new ICustomSliderImp(pControl);
}

void ICustomSlider::ReleaseISlider( ICustomSlider *pControl )
{
	delete ((ICustomSliderImp *)pControl);
}

ICustomSlider *ICustomSlider::SetupIntSlider(HWND hwnd, int idSlider, int idEdit,  int min, int max, int val, int numSegs)
{
	ICustomSlider *pControl = GetISlider(GetDlgItem(hwnd, idSlider));
	if (pControl)
	{
		pControl->LinkToEdit( GetDlgItem(hwnd, idEdit), ICustomControl::EDITTYPE_INT );
		pControl->SetLimits( min, max, FALSE );
		pControl->SetValue(val, FALSE);
		pControl->SetNumSegs(numSegs);
	}
	return pControl;
}

ICustomSlider *ICustomSlider::SetupFloatSlider(HWND hwnd, int idSlider, int idEdit,  float min, float max, float val, int numSegs, const char *pFormat)
{
	ICustomSlider *pControl = GetISlider(GetDlgItem(hwnd, idSlider));
	if (pControl)
	{
		pControl->SetSliderFormat(pFormat);
		pControl->LinkToEdit( GetDlgItem(hwnd, idEdit), ICustomControl::EDITTYPE_FLOAT );
		pControl->SetLimits( min, max, FALSE );
		pControl->SetValue(val, FALSE);
		pControl->SetNumSegs(numSegs);
	}
	return pControl;
}

// ICustomSliderImp Methods
ICustomSliderImp::ICustomSliderImp(CustomSlider *pControl) : m_pControl(pControl)
{
}

HWND ICustomSliderImp::GetHWnd(void)
{
	return m_pControl->GetHWnd();
}

void ICustomSliderImp::Enable(BOOL onOff)
{
	m_pControl->Enable(onOff);
}

void ICustomSliderImp::Disable(void)
{
	m_pControl->Disable();
}

BOOL ICustomSliderImp::IsEnabled(void)
{
	return m_pControl->IsEnabled();
}

float ICustomSliderImp::GetFVal(void)
{
	return m_pControl->GetFVal();
}

int ICustomSliderImp::GetIVal(void)
{
	return m_pControl->GetIVal();
}

void ICustomSliderImp::SetNumSegs( int num )
{
	m_pControl->SetNumSegs(num);
}

void ICustomSliderImp::SetValue( float v, BOOL bNotify )
{
	m_pControl->SetValue(v,bNotify);
}

void ICustomSliderImp::SetValue( int v, BOOL bNotify )
{
	m_pControl->SetValue(v,bNotify);
}

void ICustomSliderImp::SetLimits( int min, int max, BOOL bLimitCurValue)
{
	m_pControl->SetLimits(min,max,bLimitCurValue);
}

void ICustomSliderImp::SetLimits( float min, float max, BOOL bLimitCurValue)
{
	m_pControl->SetLimits(min,max,bLimitCurValue);
}

void ICustomSliderImp::LinkToEdit( HWND hEdit, ICustomControl::EditSpinnerType type )
{
	m_pControl->LinkToEdit(hEdit,type);
}

void ICustomSliderImp::SetIndeterminate(BOOL i)
{
	m_pControl->SetIndeterminate(i);
}

BOOL ICustomSliderImp::IsIndeterminate(void)
{
	return m_pControl->IsIndeterminate();
}

void ICustomSliderImp::SetResetValue(float v)
{
	m_pControl->SetResetValue(v);
}

void ICustomSliderImp::SetResetValue(int v)
{
	m_pControl->SetResetValue(v);
}

void ICustomSliderImp::SetKeyBrackets(BOOL onOff)
{
	m_pControl->SetKeyBrackets(onOff);
}

void ICustomSliderImp::SetDragNotify(BOOL onOff)
{
	m_pControl->SetDragNotify(onOff);
}

BOOL ICustomSliderImp::GetDragNotify(void)
{
	return m_pControl->GetDragNotify();
}

void ICustomSliderImp::SetSliderFormat(const char *pFormat)
{
	m_pControl->SetSliderFormat(pFormat);
}

const char *ICustomSliderImp::GetSliderFormat(void)
{
	return m_pControl->GetSliderFormat();
}

} // AttrControls