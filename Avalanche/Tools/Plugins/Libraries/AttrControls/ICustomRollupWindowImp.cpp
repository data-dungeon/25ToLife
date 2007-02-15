//
//	ICustomRollupWindowImp
//	

/*
 * $History: ICustomRollupWindowImp.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 */

#include "stdafx.h"
#include "ICustomRollupWindowImp.h"
#include "CustomRollupWindow.h"

namespace AttrControls 
{

// ICustomRollupWindow static methods
ICustomRollupWindow *ICustomRollupWindow::GetICustomRollupWindow( HWND hCtrl )
{
	CustomRollupWindow *pControl = 	(CustomRollupWindow *)GetWindowLongPtr( hCtrl, GWLP_USERDATA );
	if ( !pControl ) return NULL;
	return new ICustomRollupWindowImp(pControl);
}

void ICustomRollupWindow::ReleaseICustomRollupWindow( ICustomRollupWindow *irw )
{
	delete ((ICustomRollupWindowImp *)irw);
}

ICustomRollupWindowImp::ICustomRollupWindowImp(CustomRollupWindow *pControl) : m_pControl(pControl)
{
}

HWND ICustomRollupWindowImp::GetHWnd(void)
{
	return m_pControl->GetHWnd();
}

void ICustomRollupWindowImp::Enable(BOOL onOff)
{
	m_pControl->Enable(onOff);
}

void ICustomRollupWindowImp::Disable(void)
{
	m_pControl->Disable();
}

BOOL ICustomRollupWindowImp::IsEnabled(void)
{
	return m_pControl->IsEnabled();
}


void ICustomRollupWindowImp::Show(void)
{
	m_pControl->Show();
}

void ICustomRollupWindowImp::Hide(void)
{
	m_pControl->Hide();
}

void ICustomRollupWindowImp::Show(int index)
{
	m_pControl->Show(index);
}

void ICustomRollupWindowImp::Hide(int index)
{
	m_pControl->Hide(index);
}

HWND ICustomRollupWindowImp::GetPanelDlg(int index)
{
	return m_pControl->GetPanelDlg(index);
}

int ICustomRollupWindowImp::GetPanelIndex(HWND hWnd)
{
	return m_pControl->GetPanelIndex(hWnd);
}

void ICustomRollupWindowImp::SetPanelTitle(int index,TCHAR *title)
{
	m_pControl->SetPanelTitle(index,title);
}

int ICustomRollupWindowImp::AppendRollup( HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags)
{
	return m_pControl->AppendRollup(hInst,dlgTemplate,dlgProc,title,param,flags);
}

int ICustomRollupWindowImp::AppendRollup( HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags)
{
	return m_pControl->AppendRollup(hInst,dlgTemplate,dlgProc,title,param,flags);
}

int ICustomRollupWindowImp::ReplaceRollup( int index, HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags)
{
	return m_pControl->ReplaceRollup(index,hInst,dlgTemplate,dlgProc,title,param,flags);
}

int ICustomRollupWindowImp::ReplaceRollup( int index, HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags)
{
	return m_pControl->ReplaceRollup(index,hInst,dlgTemplate,dlgProc,title,param,flags);
}

void ICustomRollupWindowImp::DeleteRollup( int index, int count )
{
	m_pControl->DeleteRollup(index,count );
}

void ICustomRollupWindowImp::SetPageDlgHeight(int index,int height)
{
	m_pControl->SetPageDlgHeight(index,height);
}

void ICustomRollupWindowImp::DlgMouseMessage( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	m_pControl->DlgMouseMessage(hDlg,message,wParam,lParam );
}

int ICustomRollupWindowImp::GetNumPanels(void)
{
	return m_pControl->GetNumPanels();
}

BOOL ICustomRollupWindowImp::IsPanelOpen(int index)
{
	return m_pControl->IsPanelOpen(index);
}

void ICustomRollupWindowImp::SetPanelOpen(int index, BOOL isOpen, BOOL ignoreFlags) 
{
	m_pControl->SetPanelOpen(index,isOpen,ignoreFlags) ;
}

int ICustomRollupWindowImp::GetScrollPos(void)
{
	return m_pControl->GetScrollPos();
}

void ICustomRollupWindowImp::SetScrollPos(int spos)
{
	m_pControl->SetScrollPos(spos);
}

void ICustomRollupWindowImp::MoveRollupPanelFrom(ICustomRollupWindow *from, HWND hPanel, BOOL top)
{
	ICustomRollupWindowImp *pImpFrom = (ICustomRollupWindowImp *)from;
	m_pControl->MoveRollupPanelFrom(pImpFrom->m_pControl,hPanel,top);
}

int ICustomRollupWindowImp::GetPanelHeight(int index)
{
	return m_pControl->GetPanelHeight(index);
}

int ICustomRollupWindowImp::GetScrollHeight(void)
{
	return m_pControl->GetScrollHeight();
}

void ICustomRollupWindowImp::UpdateLayout(void)
{
	m_pControl->UpdateLayout();
}

ICustomRollupPanel *ICustomRollupWindowImp::GetPanel(HWND hWnd)
{
	return m_pControl->GetPanel(hWnd);
}

} // AttrControls

