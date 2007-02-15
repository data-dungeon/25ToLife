//

#include "stdafx.h"
#include "GLToolBar.h"
#include "Resource.h"
#include "Snipe.h"
#include "MainFrm.h"
#include "../Utility/HotKey.h"

GLImageList CGLToolBarControl::m_glImageListButton;

CGLToolBarControl::CGLToolBarControl()
{
}

CGLToolBarControl::CGLToolBarControl( const String &strName )
{
   SetName( strName );
}

CGLToolBarControl::~CGLToolBarControl()
{
}

int CGLToolBarControl::GetMinWidth() const
{
   int nBtnWidth = m_glImageListButton.GetWidthEach();

   int pos = 0;
   for (int i=0; i<m_buttonarray.GetCount(); i++)
   {
      if (m_buttonarray[i].m_pCommandUI)
         pos += nBtnWidth;
      else
         pos += SEPARATORWIDTH;
   }

   return pos;
}

int CGLToolBarControl::AddButton( CommandUI *pCommandUI/*=NULL*/, int nPos/*=-1*/ )
{
   Button button( pCommandUI );
   if (nPos == -1)
      return m_buttonarray.Add( button );
   else {
      m_buttonarray.InsertAt( nPos, button );
      return nPos;
   }
}

void CGLToolBarControl::CheckButton( int nIndex, BOOL bChecked )
{
   Button *pButton = GetButton( nIndex );

   if (pButton->m_bChecked == bChecked)
      return;

   pButton->m_bChecked = bChecked;
   Invalidate();
}

BOOL CGLToolBarControl::PressButton( int nIndex, BOOL bPressed )
{
   Button *pButton = GetButton( nIndex );

   if (pButton->m_bPressed == bPressed)
      return FALSE;

   pButton->m_bPressed = bPressed;
   Invalidate();
   return TRUE;
}

void CGLToolBarControl::EnableButton( int nIndex, BOOL bEnabled )
{
   Button *pButton = GetButton( nIndex );

   if (pButton->m_bEnabled == bEnabled)
      return;

   pButton->m_bEnabled = bEnabled ? TRUE : FALSE;
   Invalidate();
}

struct CToolBarData
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	//WORD aItems[wItemCount]

	WORD* items()
		{ return (WORD*)(this+1); }
};

BOOL CGLToolBarControl::CreateFromResource( UINT nIDResource, UINT nIDTitle )
{
   m_buttonarray.RemoveAll();

   LPCTSTR lpszResourceName = MAKEINTRESOURCE(nIDResource);

	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);

   m_strName = String::GetResourceString(nIDTitle);

	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;
	ASSERT(pData->wVersion == 1);

   CommandUIManager *pCommandUIManager = GetMainInterface()->GetCommandUIManager();

   for (int i = 0; i < pData->wItemCount; i++) {
      CommandUI *pCommandUI = pCommandUIManager->CreateCommandUI( pData->items()[i] );
      if (pCommandUI)
         pCommandUI->m_strCategory = m_strName;
      AddButton( pCommandUI );
   }

   return TRUE;
}

void CGLToolBarControl::PostInit()
{
   CGLControl::PostInit();

   if (m_glImageListButton.IsEmpty())
      m_glImageListButton.Create( IDB_TOOLBARBUTTON, 28 );
}

void CGLToolBarControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   int pos = 0;
   int nBtnWidth = m_glImageListButton.GetWidthEach();
   int nBtnHeight = m_glImageListButton.GetHeight();
   int nBtnIndex;
   for (int i=0; i<m_buttonarray.GetCount(); i++)
   {
      CommandUI *pCommandUI = m_buttonarray[i].m_pCommandUI;
      if (pCommandUI)
      {
         if ((i==0 || !m_buttonarray[i-1].m_pCommandUI) && (i+1==m_buttonarray.GetCount() || !m_buttonarray[i+1].m_pCommandUI)) // standalone
            nBtnIndex = 0;
         else if (i==0 || !m_buttonarray[i-1].m_pCommandUI) // start of group
            nBtnIndex = 1;
         else if (i+1==m_buttonarray.GetCount() || !m_buttonarray[i+1].m_pCommandUI) // end of group
            nBtnIndex = 3;
         else
            nBtnIndex = 2;

         if (m_buttonarray[i].m_bChecked || m_buttonarray[i].m_bPressed)
            nBtnIndex += 4;

         m_glImageListButton.Draw( pos, 0, nBtnIndex );

         GLImageList *pImageList = pCommandUI->GetImageList();
         if (!pImageList->IsEmpty())
            pImageList->Draw( pos + (nBtnWidth - pImageList->GetWidthEach()) / 2, (nBtnHeight - pImageList->GetHeight()) / 2 + 1 );

         if (!m_buttonarray[i].m_bEnabled)
            m_glImageListButton.Draw( pos, 0, nBtnIndex % 4 + 8 );

         pos += nBtnWidth;
      }
      else
         pos += SEPARATORWIDTH;
   }

   CGLControl::Draw( nDrawFlags );
}

int CGLToolBarControl::HitTestButtons( CPoint point ) const
{
   int nBtnWidth = m_glImageListButton.GetWidthEach();
   int nBtnHeight = m_glImageListButton.GetHeight();
   int pos = 0;

   for (int i=0; i<m_buttonarray.GetCount(); i++)
   {
      if (m_buttonarray[i].m_pCommandUI)
      {
         CRect rcButton( pos, 0, pos + nBtnWidth, nBtnHeight );
         if (rcButton.PtInRect( point ))
            return i;

         pos += nBtnWidth;
      }
      else
         pos += SEPARATORWIDTH;
   }

   return -1;
}

UINT CGLToolBarControl::GetButtonToolTipText( int nIndex, String &strText ) const
{
   CommandUI *pCommandUI = m_buttonarray[nIndex].m_pCommandUI;
   if (!pCommandUI)
      return FALSE;

   strText = pCommandUI->GetName();
   if (!pCommandUI->m_actualaccellist.IsEmpty())
   {
      CommandUIAccel &commanduiaccel = pCommandUI->m_actualaccellist.GetHead();
      String strItemDesc = ::GetHotKeyDescription( commanduiaccel.m_key, commanduiaccel.m_virt );
      strText += (String)" (" + strItemDesc + ")";
   }
   GetMainFrame()->m_glMainRenderWnd.m_glStatusPaneControl.SetText( pCommandUI->GetDescription().Get() );
   return pCommandUI->m_nCmdID;
}

UINT CGLToolBarControl::GetToolTipText( CPoint point, String &strText ) const
{
   int nHit = HitTestButtons( point );
   if (nHit == -1)
   {
      GetMainFrame()->m_glMainRenderWnd.m_glStatusPaneControl.SetText( NULL );
      return FALSE;
   }

   return GetButtonToolTipText( nHit, strText );
}

void CGLToolBarControl::OnMouseEnter()
{
}

void CGLToolBarControl::OnMouseLeave()
{
   GetMainFrame()->m_glMainRenderWnd.m_glStatusPaneControl.SetText( NULL );
}

BOOL CGLToolBarControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   int nHit = HitTestButtons( point );
   if (nHit == -1 || !m_buttonarray[nHit].m_bEnabled)
      return FALSE;

   PressButton( nHit, TRUE );
   GetApp().DrawInvalidControls();

   MSG msgLoop;
   msgLoop.message = 0;
   while (msgLoop.message != WM_LBUTTONUP)	{
		if (PeekMessage(&msgLoop, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msgLoop);
			DispatchMessage(&msgLoop);
			continue;
		}

      GetCursorPos( &point );
      ((CWnd *)m_pGLRenderWnd)->ScreenToClient( &point );
      ClientToControl( point );
      if (PressButton( nHit, HitTestButtons( point )==nHit ))
         GetApp().DrawInvalidControls();
   }

   if (IsButtonPressed( nHit )) {
      AfxGetMainWnd()->PostMessage( WM_COMMAND, m_buttonarray[nHit].m_pCommandUI->m_nCmdID );

      PressButton( nHit, FALSE );
      GetApp().DrawInvalidControls();
   }
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGLToolBarControl idle update through CToolCmdUI class

class CToolCmdUI : public CCmdUI        // class private to this file !
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
};

void CToolCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
	CGLToolBarControl* pToolBar = (CGLToolBarControl*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CGLToolBarControl, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

   pToolBar->EnableButton( m_nIndex, bOn );
}

void CToolCmdUI::SetCheck(int nCheck)
{
	ASSERT(nCheck >= 0 && nCheck <= 2); // 0=>off, 1=>on, 2=>indeterminate
	CGLToolBarControl* pToolBar = (CGLToolBarControl*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CGLToolBarControl, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

   pToolBar->CheckButton( m_nIndex, nCheck );
}

void CToolCmdUI::SetText(LPCTSTR)
{
	// ignore it
}

void CGLToolBarControl::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CToolCmdUI state;
	state.m_pOther = (CWnd *)this;

	state.m_nIndexMax = GetButtonCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
	{
		// get buttons state
		Button *pButton = &m_buttonarray[state.m_nIndex];

		// ignore separators
		if (pButton->m_pCommandUI)
		{
   		state.m_nID = pButton->m_pCommandUI->m_nCmdID;
         if (pButton->m_pCommandUI->CreateIcon())
            Invalidate();

         // allow reflections
			//if (OnCmdMsg(0, 
			//	MAKELONG(CN_UPDATE_COMMAND_UI&0xff, WM_COMMAND+WM_REFLECT_BASE), 
			//	&state, NULL))
			//	continue;

			// allow the toolbar itself to have update handlers
			//if (OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
			//	continue;

			// allow the owner to process the update
			state.DoUpdate(pTarget, bDisableIfNoHndler);
		}
	}
}

/////////////////////////////////////////////////////////////////////
// CGLToolBarShelf

CGLToolBarShelf::CGLToolBarShelf() :
 CGLTabControl( FALSE, TRUE )
{
}

CGLToolBarShelf::~CGLToolBarShelf()
{
   for (int i=0; i<GetToolBarCount(); i++)
   {
      CGLToolBarControl *pToolBar = GetToolBarAt( i );
      SetItemData( i, NULL );
      delete pToolBar;
   }
}

CGLToolBarControl *CGLToolBarShelf::FindToolBar( const String &strName ) const
{
   for (int i=0; i<GetToolBarCount(); i++)
   {
      CGLToolBarControl *pToolBar = GetToolBarAt( i );
      if (pToolBar->GetName() == strName)
         return pToolBar;
   }

   return NULL;
}

CGLToolBarControl *CGLToolBarShelf::AddToolBar( const String &strName )
{
   CGLToolBarControl *pToolBar = FindToolBar( strName );
   if (!pToolBar)
   {
      pToolBar = SNEW CGLToolBarControl( strName );
      AddTab( strName, (LPARAM)pToolBar );
   }
   return pToolBar;
}

void CGLToolBarShelf::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
   if (m_nSelectedTab >= 0)
      ((CGLToolBarControl *)m_tabarray[m_nSelectedTab].GetControlHead())->OnUpdateCmdUI( pTarget, bDisableIfNoHndler );
}