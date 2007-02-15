//
//	ICustomSpinnerImp
//	

/*
 * $History: ICustomSpinnerImp.cpp $
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
#include "ICustomSpinnerImp.h"
#include "CustomSpinner.h"

namespace AttrControls 
{

// ICustomSpinner Methods
ICustomSpinner *ICustomSpinner::GetISpinner( HWND hCtrl )
{
	CustomSpinner *pControl = (CustomSpinner *)GetWindowLongPtr( hCtrl, GWLP_USERDATA );
	if ( !pControl ) return NULL;
	return new ICustomSpinnerImp(pControl);
}

void ICustomSpinner::ReleaseISpinner( ICustomSpinner *pControl )
{
	delete ((ICustomSpinnerImp *)pControl);
}

ICustomSpinner *ICustomSpinner::SetupIntSpinner(HWND hwnd, int idSpin, int idEdit,  int min, int max, int val, int scale) 
{
	ICustomSpinner *pControl= GetISpinner(GetDlgItem(hwnd, idSpin));
	if (pControl) 
	{
		pControl->LinkToEdit( GetDlgItem(hwnd,idEdit), ICustomSpinner::EDITTYPE_INT );
		pControl->SetLimits( min,max, FALSE );
		pControl->SetValue(val,FALSE);
		pControl->SetScale((float)scale);
	}
	return pControl;
}

ICustomSpinner *ICustomSpinner::SetupFloatSpinner(HWND hwnd, int idSpin, int idEdit,  float min, float max, float val, float scale, const char *pFormat) 
{
	ICustomSpinner *pControl= GetISpinner(GetDlgItem(hwnd, idSpin));
	if (pControl) 
	{
		pControl->SetSpinnerFormat(pFormat);
		pControl->LinkToEdit( GetDlgItem(hwnd,idEdit), ICustomSpinner::EDITTYPE_FLOAT );
		pControl->SetLimits( min,max, FALSE );
		pControl->SetValue(val,FALSE);
		pControl->SetScale(scale);
	}
	return pControl;
}

// ICustomSpinnerImp Methods
ICustomSpinnerImp::ICustomSpinnerImp(CustomSpinner *pControl) : m_pControl(pControl)
{
}

HWND ICustomSpinnerImp::GetHWnd(void)
{
	return m_pControl->GetHWnd();
}

void ICustomSpinnerImp::Enable(BOOL onOff)
{
	m_pControl->Enable(onOff);
}

void ICustomSpinnerImp::Disable(void)
{
	m_pControl->Disable();
}

BOOL ICustomSpinnerImp::IsEnabled(void)
{
	return m_pControl->IsEnabled();
}

float ICustomSpinnerImp::GetFVal(void)
{
	return m_pControl->GetFVal();
}

int ICustomSpinnerImp::GetIVal(void)
{
	return m_pControl->GetIVal();
}

void ICustomSpinnerImp::SetAutoScale(BOOL on)
{
	m_pControl->SetAutoScale(on);
}

void ICustomSpinnerImp::SetScale( float s )
{
	m_pControl->SetScale(s);
}

void ICustomSpinnerImp::SetValue( float v, BOOL bNotify )
{
	m_pControl->SetValue(v,bNotify);
}

void ICustomSpinnerImp::SetValue( int v, BOOL bNotify )
{
	m_pControl->SetValue(v,bNotify);
}

void ICustomSpinnerImp::SetLimits( int min, int max, BOOL bLimitCurValue)
{
	m_pControl->SetLimits(min,max,bLimitCurValue);
}

void ICustomSpinnerImp::SetLimits( float min, float max, BOOL bLimitCurValue)
{
	m_pControl->SetLimits(min,max,bLimitCurValue);
}

void ICustomSpinnerImp::LinkToEdit( HWND hEdit, EditSpinnerType type )
{
	m_pControl->LinkToEdit(hEdit,type);
}

void ICustomSpinnerImp::SetIndeterminate(BOOL i)
{
	m_pControl->SetIndeterminate(i);
}

BOOL ICustomSpinnerImp::IsIndeterminate(void)
{
	return m_pControl->IsIndeterminate();
}

void ICustomSpinnerImp::SetResetValue(float v)
{
	m_pControl->SetResetValue(v);
}

void ICustomSpinnerImp::SetResetValue(int v)
{
	m_pControl->SetResetValue(v);
}

void ICustomSpinnerImp::SetKeyBrackets(BOOL onOff)
{
	m_pControl->SetKeyBrackets(onOff);
}

void ICustomSpinnerImp::SetSnapSpinner(BOOL b)
{
	m_pControl->SetSnapSpinner(b);
}

BOOL ICustomSpinnerImp::GetSnapSpinner(void)
{
	return m_pControl->GetSnapSpinner();
}

void ICustomSpinnerImp::SetSnapSpinValue(float f)
{
	m_pControl->SetSnapSpinValue(f);
}

float ICustomSpinnerImp::GetSnapSpinValue(void)
{
	return m_pControl->GetSnapSpinValue();
}

void ICustomSpinnerImp::SetSpinnerFormat(const char *pFormat)
{
	m_pControl->SetSpinnerFormat(pFormat);
}

const char *ICustomSpinnerImp::GetSpinnerFormat(void)
{
	return m_pControl->GetSpinnerFormat();
}

void ICustomSpinnerImp::SetSpinnerWrap(BOOL b)
{
	m_pControl->SetSpinnerWrap(b);
}

BOOL ICustomSpinnerImp::GetSpinnerWrap(void)
{
	return m_pControl->GetSpinnerWrap();
}

void ICustomSpinnerImp::SetDragNotify(BOOL b)
{
	m_pControl->SetDragNotify(b);
}

BOOL ICustomSpinnerImp::GetDragNotify(void)
{
	return m_pControl->GetDragNotify();
}

} // AttrControls
