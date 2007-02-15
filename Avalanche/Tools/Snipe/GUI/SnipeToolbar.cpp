// Dan  4/9/2004  \DanLP110\HashView\Toolbar.cpp

#include "StdAfx.h"
#include "Chevron.h"
#include "SnipeToolbar.h"
#include "Resource.h"
#include "MainFrm.h"
#include ".\snipetoolbar.h"

///////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSnipeToolBar, CExtToolControlBar)
	ON_COMMAND(ID_ALLOWDOCKING, OnToggleAllowDocking)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSnipeToolBar, CExtToolControlBar);

CExtBarContentExpandButton *CSnipeToolBar::OnCreateBarRightBtn()
{
   if ( m_bPaletteMode )
      return NULL;

   return SNEW Chevron( this );
}

void CSnipeToolBar::UpdateButton(UINT cmdid)
{
   _UpdateButton(cmdid);
}

CSize CSnipeToolBar::_CalcLayout(DWORD dwMode, int nLength )
{
   CExtBarContentExpandButton *pRightButton = GetRightButton();
   if ( pRightButton == NULL ) {
      VERIFY( InitContentExpandButton() );
      pRightButton = GetRightButton();
      ASSERT( pRightButton != NULL );
   } // if( pRightButton == NULL )
   _RecalcPositionsImpl();
   CSize _sizeLayout = CExtToolControlBar::_CalcLayout(  dwMode, nLength );
   _RecalcPositionsImpl();
   ((Chevron *)pRightButton)->_AdjustVisibility();
   _sizeLayout = CExtToolControlBar::_CalcLayout(dwMode, nLength );

   return _sizeLayout;
}

CSize CSnipeToolBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
   if ((nLength == -1) && !(dwMode & (LM_MRUWIDTH|LM_COMMIT))
    &&  (dwMode & (LM_HORZDOCK|LM_VERTDOCK)))
      return CalcFixedLayout( dwMode & LM_STRETCH, dwMode & LM_HORZDOCK );
      
   ASSERT((dwMode&(LM_HORZ|LM_HORZDOCK)) || (!(dwMode&LM_HORZDOCK)));
   return _CalcLayout( dwMode, nLength );
}
   
CSize CSnipeToolBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
   DWORD dwMode = bStretch ? LM_STRETCH : 0;
   dwMode |= bHorz ? LM_HORZ : 0;
   ASSERT((dwMode&(LM_HORZ|LM_HORZDOCK)) || (!(dwMode&LM_HORZDOCK)));
   return _CalcLayout( dwMode );
}

void CSnipeToolBar::OnToggleAllowDocking()
{
   BOOL canbedocked = m_dwDockStyle & CBRS_ALIGN_ANY;
	if (canbedocked) {
		m_dwMRUDockingState = m_dwDockStyle & CBRS_ALIGN_ANY;
		if (!IsFloating())
			ToggleDocking();
		EnableDocking(0);
	}
	else
		EnableDocking (m_dwMRUDockingState);
}

void CSnipeToolBar::SetAllCommandsBasic()
{
   int i;
   UINT id, style;
   CWinApp *pApp = ::AfxGetApp();
   CExtCmdProfile *pProfile = g_CmdManager->ProfileGetPtr( pApp->m_pszProfileName );
   
   for (i=0; i < GetButtonsCount(); i++) {
      GetButtonInfo(i, id, style);
      CExtCmdItem *pCmdItem =	pProfile->CmdGetPtr(id);
      if (pCmdItem) 
         pCmdItem->StateSetBasic(TRUE);
   }
}

//******************************************************************************
//******************************************************************************
//******************************************************************************

IMPLEMENT_DYNAMIC(CSnipeControlBar, CExtControlBar);

BEGIN_MESSAGE_MAP(CSnipeControlBar, CExtControlBar)
	ON_COMMAND(ID_ALLOWDOCKING, OnToggleAllowDocking)
END_MESSAGE_MAP()

void CSnipeControlBar::OnToggleAllowDocking()
{
   BOOL canbedocked = m_dwDockStyle & CBRS_ALIGN_ANY;
	if (canbedocked) {
		m_dwMRUDockingState = m_dwDockStyle & CBRS_ALIGN_ANY;
		if (!IsFloating())
			ToggleDocking();
		EnableDocking(0);
	}
	else
		EnableDocking (m_dwMRUDockingState);
}
