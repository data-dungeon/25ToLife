//
//	CustomControl
//	

/*
 * $History: ICustomEditBoxImp.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/19/02   Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/20/02   Time: 4:20p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * working on custom control port
 */

#include "stdafx.h"
#include "ICustomEditBoxImp.h"
#include "CustomEditBox.h"

namespace AttrControls 
{

// ICustomEditBox Methods
ICustomEditBox *ICustomEditBox::GetICustomEditBox(HWND hCtrl)
{
	CustomEditBox *pControl = (CustomEditBox *)GetWindowLongPtr( hCtrl, GWLP_USERDATA );
	if (!pControl) return NULL;
	return new ICustomEditBoxImp(pControl);
}

void ICustomEditBox::ReleaseICustomEditBox(ICustomEditBox *pControl)
{
	delete ((ICustomEditBoxImp *)pControl);
}


// ICustomEditBoxImp Methods
ICustomEditBoxImp::ICustomEditBoxImp(CustomEditBox *pControl) : m_pControl(pControl)
{
}

HWND ICustomEditBoxImp::GetHWnd(void)
{
	return m_pControl->GetHWnd();
}

void ICustomEditBoxImp::Enable(BOOL onOff)
{
	m_pControl->Enable(onOff);
}

void ICustomEditBoxImp::Disable(void)
{
	m_pControl->Disable();
}

BOOL ICustomEditBoxImp::IsEnabled(void)
{
	return m_pControl->IsEnabled();
}

void ICustomEditBoxImp::GetText( char *text, int ct )
{
	m_pControl->GetText(text,ct);
}

void ICustomEditBoxImp::SetText( const char *text )
{
	m_pControl->SetText(text);
}

void ICustomEditBoxImp::SetText( int i )
{
	m_pControl->SetText(i);
}

void ICustomEditBoxImp::SetText( float f, const char *pFormat )
{
	m_pControl->SetText(f,pFormat);
}

int ICustomEditBoxImp::GetInt(BOOL *valid)
{
	return m_pControl->GetInt(valid);
}

float ICustomEditBoxImp::GetFloat(BOOL *valid)
{
	return m_pControl->GetFloat(valid);
}

void ICustomEditBoxImp::SetLeading(int lead)
{
	m_pControl->SetLeading(lead);
}

void ICustomEditBoxImp::WantReturn(BOOL yesNo)
{
	m_pControl->WantReturn(yesNo);
}

BOOL ICustomEditBoxImp::GotReturn(void)
{
	return m_pControl->GotReturn();
}

void ICustomEditBoxImp::GiveFocus(void)
{
	m_pControl->GiveFocus();
}

BOOL ICustomEditBoxImp::HasFocus(void)
{
	return m_pControl->HasFocus();
}

void ICustomEditBoxImp::WantDlgNextCtl(BOOL yesNo)
{
	m_pControl->WantDlgNextCtl(yesNo);
}

void ICustomEditBoxImp::SetNotifyOnKillFocus(BOOL onOff)
{
	m_pControl->SetNotifyOnKillFocus(onOff);
}

void ICustomEditBoxImp::SetBold(BOOL onOff)
{
	m_pControl->SetBold(onOff);
}

HWND ICustomEditBoxImp::GetHEdit(void)
{
	return m_pControl->GetHEdit();
}


} // AttrControls
