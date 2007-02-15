// Dan  7/7/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\KeyPose\KeyPose.cpp
#include "stdafx.h"
#include "ModelExt.h"
#include "sdk/HModel.h"
#include "MessageR.h"
#include "Hxt.h"

#define HEADERHEIGHT 18

HProperty *GetFirstTreeProperty( HTreeObject *obj )
{
   HProperty *property;

   for (int i=0; property=obj->GetPropertyAt(i); i++)
//      if (VISIBLE == property->IsVisibleInTree(tree))
         return property;

   return NULL;
}


IMPLEMENT_DYNAMIC(CKeyPoseWnd, CWnd);

BEGIN_MESSAGE_MAP(CKeyPoseWnd, CWnd)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_NOTIFY( TCN_SELCHANGE, 0xFFFF, OnTabChange )
END_MESSAGE_MAP()

CKeyPoseWnd::CKeyPoseWnd()
{
   m_model = NULL;
   m_modelext = NULL;
   m_isinsetmodel = FALSE;
}

int CKeyPoseWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_tab.Create(CRect(0,0,0,0), this, 0xFFFF);
   m_tree.Create(CRect(0,0,0,0), this, 0xFFFF);

   SetModel( NULL );

   return 0;                                       
}

void CKeyPoseWnd::OnSize( UINT nType, int cx, int cy )
{
   CWnd::OnSize(nType, cx, cy);

   CRect rect;
   GetClientRect( &rect );

   m_tab.MoveWindow( rect.left, rect.top, rect.Width(), HEADERHEIGHT);
   m_tree.MoveWindow( rect.left, rect.top+HEADERHEIGHT, rect.Width(), rect.Height()-HEADERHEIGHT);
}

void CKeyPoseWnd::SetModel( HModel *model)
{
   if (GetSafeHwnd()==NULL)
      return;

   if (m_isinsetmodel == TRUE)
      return;

   m_isinsetmodel = TRUE;

   if (m_model) {
      m_modelext->m_posebaractivetab = m_tab.GetActiveTab();
      m_modelext->m_posebarvertscrollpos = m_tree.GetScrollPos( SB_VERT );
      SetModelExt(m_model, m_modelext); // Stores ext into dataproperty on model
   }
 
   m_tree.m_startobj = NULL;

   m_model = model;
   delete m_modelext;

   if (m_model)
      m_modelext = GetModelExt(m_model);
   else
      m_modelext = NULL;
   
   OnTabChange( NULL, NULL );

   m_tab.m_tabs.RemoveAll();
   m_tab.SetActiveTab(-1);

   if (!m_model) {
      m_isinsetmodel = FALSE;
      return;
   }

   HUserCategoryProperty *userprop = m_model->GetUserCategoryProperty();

   if (!userprop) {
      m_isinsetmodel= FALSE;
      return;
   }

   BOOL alltab = FALSE;
   HProperty *property;

   for (int i=0; property=userprop->GetPropertyAt(i); i++) {
      if (property->GetObjectType() == HOT_USERCATEGORYPROPERTY)
         m_tab.AddTab( property->GetName(), GetFirstTreeProperty(property));
      else if (!alltab /* && HTreeObject::VISIBLE == property->IsVisibleInTree() */) {
         alltab = TRUE;
         m_tab.AddTab( "All", property );
      }
   }

   m_tab.SetActiveTab( m_modelext->m_posebaractivetab );
   if (m_modelext->m_posebarvertscrollpos != 0 )
      m_tree.ScrollVert( m_modelext->m_posebarvertscrollpos); 

   m_isinsetmodel = FALSE;
}

void CKeyPoseWnd::OnTabChange( NMHDR* pNMHDR, LRESULT* pResult )
{
 	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   HASHTAB_NMHDR *tabhdr = (HASHTAB_NMHDR *)pNMHDR;

   m_tree.RemoveAllPropertyWnd();

   if (tabhdr && tabhdr->tab)
      m_tree.m_startobj=(HProperty *)tabhdr->tab->m_data;
//      m_tree.ReassignPtr((HashPtrObject**)&m_tree.m_startobj, (Property *)tabhdr->tab->m_data);
   else   
      m_tree.m_startobj=NULL;
//      m_tree.ReassignPtr((HashPtrObject**)&m_tree.m_startobj, NULL);

   m_tree.RecalcScrollBars();
   m_tree.AdjustAllPropertyWnd();
   m_tree.Invalidate( FALSE );   
}