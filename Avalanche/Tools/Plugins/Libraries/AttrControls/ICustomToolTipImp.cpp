//
//	ICustomToolTipImp
//	

#include "stdafx.h"
#include "ICustomToolTipImp.h"
#include "CustomToolTip.h"

namespace AttrControls 
{

// ICustomToolTip Methods
ICustomToolTip *ICustomToolTip::GetIToolTip( HWND hCtrl )
{
	CustomToolTip *pControl = (CustomToolTip *)GetWindowLongPtr( hCtrl, GWLP_USERDATA );
	if ( !pControl ) return NULL;
	return new ICustomToolTipImp(pControl);
}

void ICustomToolTip::ReleaseIToolTip( ICustomToolTip *pControl )
{
	delete ((ICustomToolTipImp *)pControl);
}

// ICustomToolTipImp Methods
ICustomToolTipImp::ICustomToolTipImp(CustomToolTip *pControl) : m_pControl(pControl)
{
}

HWND ICustomToolTipImp::GetHWnd(void)
{
	return m_pControl->GetHWnd();
}

void ICustomToolTipImp::Enable(BOOL onOff)
{
	m_pControl->Enable(onOff);
}

void ICustomToolTipImp::Disable(void)
{
	m_pControl->Disable();
}

BOOL ICustomToolTipImp::IsEnabled(void)
{
	return m_pControl->IsEnabled();
}

void ICustomToolTipImp::SetStyles(DWORD nStyles, int nIndexTool)
{
	m_pControl->SetStyles(nStyles,nIndexTool);
}

void ICustomToolTipImp::ModifyStyles(DWORD nAddStyles, DWORD nRemoveStyles, int nIndexTool)
{
	m_pControl->ModifyStyles(nAddStyles,nRemoveStyles,nIndexTool);
}

DWORD ICustomToolTipImp::GetStyles(int nIndexTool)
{
	return m_pControl->GetStyles(nIndexTool);

}

void ICustomToolTipImp::SetDefaultStyles(int nIndexTool)
{
	m_pControl->SetDefaultStyles(nIndexTool);
}

void ICustomToolTipImp::Activate(BOOL bActive)
{
	m_pControl->Activate(bActive);
}

void ICustomToolTipImp::SetColor(int nIndex, COLORREF crColor)
{
	m_pControl->SetColor(nIndex,crColor);
}

COLORREF ICustomToolTipImp::GetColor(int nIndex)
{
	return m_pControl->GetColor(nIndex);
}

void ICustomToolTipImp::SetDefaultColors()
{
	m_pControl->SetDefaultColors();
}

void ICustomToolTipImp::SetGradientColors(COLORREF crBegin, COLORREF crMid, COLORREF crEnd, int nIndexTool)
{
	m_pControl->SetGradientColors(crBegin,crMid,crEnd,nIndexTool);
}

void ICustomToolTipImp::GetGradientColors(COLORREF & crBegin, COLORREF & crMid, COLORREF & crEnd, int nIndexTool)
{
	m_pControl->GetGradientColors(crBegin,crMid,crEnd,nIndexTool);
}

void ICustomToolTipImp::AddTool(HWND hWnd, UINT nIdText, HICON hIcon, LPCRECT lpRectTool, UINT nIDTool)
{
	m_pControl->AddTool(hWnd,nIdText,hIcon,lpRectTool,nIDTool);
}

void ICustomToolTipImp::AddTool(HWND hWnd, UINT nIdText, UINT nIdIcon, LPCRECT lpRectTool, UINT nIDTool)
{
	m_pControl->AddTool(hWnd,nIdText,nIdIcon,lpRectTool,nIDTool);
}

void ICustomToolTipImp::AddTool(HWND hWnd, TupString sTooltipText, HICON hIcon, LPCRECT lpRectTool, UINT nIDTool)
{
	m_pControl->AddTool(hWnd,sTooltipText,hIcon,lpRectTool,nIDTool);
}

void ICustomToolTipImp::AddTool(HWND hWnd, TupString sTooltipText, UINT nIdIcon, LPCRECT lpRectTool, UINT nIDTool)
{
	m_pControl->AddTool(hWnd,sTooltipText,nIdIcon,lpRectTool,nIDTool);
}

void ICustomToolTipImp::AddTool(ToolTipInfo ti)
{
	m_pControl->AddTool(ti);
}

int  ICustomToolTipImp::FindTool(POINT & pt)
{
	return m_pControl->FindTool(pt);
}

int  ICustomToolTipImp::FindTool(HWND hWnd, LPCRECT lpRect)
{
	return m_pControl->FindTool(hWnd,lpRect);
}

int  ICustomToolTipImp::FindTool(UINT nIDTool)
{
	return m_pControl->FindTool(nIDTool);
}

BOOL ICustomToolTipImp::GetTool(int nIndexTool, ICustomToolTip::ToolTipInfo & ti)
{
	return m_pControl->GetTool(nIndexTool,ti);
}

BOOL ICustomToolTipImp::RemoveTool(int nIndexTool)
{
	return m_pControl->RemoveTool(nIndexTool);
}

void ICustomToolTipImp::RemoveAllTools()
{
	m_pControl->RemoveAllTools();
}

BOOL ICustomToolTipImp::IsExistTool(int nIndexTool)
{
	return m_pControl->IsExistTool(nIndexTool);
}

void ICustomToolTipImp::SetAtTool(int nIndexTool, ICustomToolTip::ToolTipInfo & ti)
{
	m_pControl->SetAtTool(nIndexTool,ti);
}

void ICustomToolTipImp::AddNameOfResource(TupString sName, UINT nID, BYTE nTypeRes, COLORREF crMask)
{
	m_pControl->AddNameOfResource(sName,nID,nTypeRes,crMask);
}

int  ICustomToolTipImp::FindIdOfResource(TupString sName)
{
	return m_pControl->FindIdOfResource(sName);
}

int  ICustomToolTipImp::FindNameOfResource(UINT nID)
{
	return m_pControl->FindNameOfResource(nID);
}

void ICustomToolTipImp::RemoveNameOfResource(int nIndex)
{
	m_pControl->RemoveNameOfResource(nIndex);
}

void ICustomToolTipImp::RemoveAllNamesOfResource()
{
	m_pControl->RemoveAllNamesOfResource();
}

void ICustomToolTipImp::SetImageList(UINT nIdBitmap, int cx, int cy, int nCount, COLORREF crMask)
{
	m_pControl->SetImageList(nIdBitmap,cx,cy,nCount,crMask);
}

void ICustomToolTipImp::SetImageList(HBITMAP hBitmap, int cx, int cy, int nCount, COLORREF crMask)
{
	m_pControl->SetImageList(hBitmap,cx,cy,nCount,crMask);
}

HIMAGELIST ICustomToolTipImp::GetImageList(SIZE & sz)
{
	return m_pControl->GetImageList(sz);
}

void  ICustomToolTipImp::SetMaskTool(int nIndexTool, UINT nMask)
{
	m_pControl->SetMaskTool(nIndexTool,nMask);
}

void  ICustomToolTipImp::ModifyMaskTool(int nIndexTool, UINT nAddMask, UINT nRemoveMask)
{
	m_pControl->ModifyMaskTool(nIndexTool,nAddMask,nRemoveMask);
}

UINT  ICustomToolTipImp::GetMaskTool(int nIndexTool)
{
	return m_pControl->GetMaskTool(nIndexTool);
}

void ICustomToolTipImp::SetEffectBk(UINT nEffect, BYTE nGranularity, int nIndexTool)
{
	m_pControl->SetEffectBk(nEffect,nGranularity,nIndexTool);
}

UINT ICustomToolTipImp::GetEffectBk(int nIndexTool)
{
	return m_pControl->GetEffectBk(nIndexTool);
}

UINT ICustomToolTipImp::GetEffectBk(BYTE & nGranularity, int nIndexTool)
{
	return m_pControl->GetEffectBk(nGranularity,nIndexTool);
}

void  ICustomToolTipImp::SetNotify(HWND hWnd)
{
	m_pControl->SetNotify(hWnd);
}

void  ICustomToolTipImp::SetNotify(BOOL bParentNotify)
{
	m_pControl->SetNotify(bParentNotify);
}

BOOL  ICustomToolTipImp::GetNotify()
{
	return m_pControl->GetNotify();
}

void ICustomToolTipImp::SetDelayTime(DWORD dwDuration, UINT nTime)
{
	m_pControl->SetDelayTime(dwDuration,nTime);
}

UINT ICustomToolTipImp::GetDelayTime(DWORD dwDuration) const
{
	return m_pControl->GetDelayTime(dwDuration);
}

void ICustomToolTipImp::SetSize(int nSizeIndex, UINT nValue)
{
	m_pControl->SetSize(nSizeIndex,nValue);
}

UINT ICustomToolTipImp::GetSize(int nSizeIndex)
{
	return m_pControl->GetSize(nSizeIndex);
}

void ICustomToolTipImp::SetDefaultSizes(BOOL bBalloonSize)
{
	m_pControl->SetDefaultSizes(bBalloonSize);
}

void ICustomToolTipImp::SetDirection(UINT nDirection, int nIndex)
{
	m_pControl->SetDirection(nDirection,nIndex);
}

UINT ICustomToolTipImp::GetDirection(int nIndexTool)
{
	return m_pControl->GetDirection(nIndexTool);
}

void ICustomToolTipImp::SetBehaviour(UINT nBehaviour, int nIndexTool)
{
	m_pControl->SetBehaviour(nBehaviour,nIndexTool);
}

UINT ICustomToolTipImp::GetBehaviour(int nIndexTool)
{
	return m_pControl->GetBehaviour(nIndexTool);
}

BOOL  ICustomToolTipImp::SetFont(LPLOGFONT lf)
{
	return m_pControl->SetFont(lf);
}

BOOL  ICustomToolTipImp::SetFont(LPCTSTR lpszFaceName, int nSizePoints,	BOOL bUnderline, BOOL bBold,	BOOL bStrikeOut, BOOL bItalic )
{
	return m_pControl->SetFont(lpszFaceName,nSizePoints,bUnderline,bBold,bStrikeOut,bItalic);
}

void  ICustomToolTipImp::SetDefaultFont()
{
	m_pControl->SetDefaultFont();
}

void  ICustomToolTipImp::GetFont(LPLOGFONT lf)
{
	m_pControl->GetFont(lf);
}

void ICustomToolTipImp::RelayEvent(MSG* pMsg)
{
	m_pControl->RelayEvent(pMsg);
}

void ICustomToolTipImp::ShowHelpTooltip(POINT & pt, UINT nIdText, HICON hIcon)
{
	m_pControl->ShowHelpTooltip(pt,nIdText,hIcon);
}

void ICustomToolTipImp::ShowHelpTooltip(POINT & pt, UINT nIdText, UINT nIdIcon)
{
	m_pControl->ShowHelpTooltip(pt,nIdText,nIdIcon);
}

void ICustomToolTipImp::ShowHelpTooltip(POINT & pt, TupString sTooltipText, HICON hIcon)
{
	m_pControl->ShowHelpTooltip(pt,sTooltipText,hIcon);
}

void ICustomToolTipImp::ShowHelpTooltip(POINT & pt, TupString sTooltipText, UINT nIdIcon)
{
	m_pControl->ShowHelpTooltip(pt,sTooltipText,nIdIcon);
}

void ICustomToolTipImp::ShowHelpTooltip(POINT & pt, ICustomToolTip::ToolTipInfo & ti)
{
	m_pControl->ShowHelpTooltip(pt,ti);
}

void ICustomToolTipImp::SetParentWnd(HWND hParentWnd)
{
	m_pControl->SetParentWnd(hParentWnd);
}


} // AttrControls
