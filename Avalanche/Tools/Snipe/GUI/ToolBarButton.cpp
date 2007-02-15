
#include "StdAfx.h"
#include "MainFrm.h"
#include "ToolBarButton.h"

CWnd *CSnipeExtBarTextFieldButton::OnInplaceControlCreate()
{
   CEdit *pEdit = (CEdit *)CExtBarTextFieldButton::OnInplaceControlCreate();
   if (pEdit)
      pEdit->SetSel(0, -1); // This function is overridden so I can select the editbox on initial select
	return pEdit;
}

void CSnipeExtBarTextFieldButton::OnInplaceControlSessionEnd()
{
   // Send an OnNotify Message, or a Kill Focus
   CExtBarTextFieldButton::OnInplaceControlSessionEnd();
   GetMainFrame()->SendMessage( WM_COMMAND, MAKEWPARAM(GetCmdID(), EN_KILLFOCUS), (LPARAM)this );
}

//************************************************************************8

ULONG CSnipeExtBarSliderButton::ScrollPosSetNoSetTime( ULONG nScrollPos )
{
	ULONG ulRet = CExtBarSliderButton::ScrollPosSet( nScrollPos );
	RedrawButton();
   return ulRet;
}

ULONG CSnipeExtBarSliderButton::ScrollPosSet( ULONG nScrollPos )
{
	ULONG ulRet = ScrollPosSetNoSetTime( nScrollPos );
   
   // Go through normal command routing which will call SetTime
   GetMainFrame()->SendMessage( WM_COMMAND, GetCmdID(), (LPARAM)this );
   
   return ulRet;
}

CExtSafeString CSnipeExtBarSliderButton::GetText() const
{
  // to set buttons text empty just return this
  return CExtSafeString( _T("") );
}
