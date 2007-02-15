//

#include "stdafx.h"
#include "HCBaseDoc.h"
#include "SelectManipulator.h"
#include "../resource.h"
#include "HCGLBaseView.h"
#include "HHitArray.h"
#include "HCommandPluginManager.h"
#include "HCommandPlugin.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HPropertyArray.h"
#include "HBoolProperty.h"
#include "HTypeProperty.h"
#include "CommandUIAccel.h"
#include "HVertex.h"
#include "HEdge.h"
#include "HPoly.h"

// Plugin //////////////////////////////////////////////////
HBoolProperty::HInfo *SelectManipulatorPlugin::m_pMouseOverInfo;
HBoolProperty *SelectManipulatorPlugin::m_pMouseOver;

HBoolProperty::HInfo *SelectManipulatorPlugin::m_pRealTimeInfo;
HBoolProperty *SelectManipulatorPlugin::m_pRealTime;

HBoolProperty::HInfo *SelectManipulatorPlugin::m_pSelectObscuredInfo;
HBoolProperty *SelectManipulatorPlugin::m_pSelectObscured;

HTypeProperty::HInfo::HNode *SelectManipulatorPlugin::m_pMethodNodes;
HTypeProperty::HInfo *SelectManipulatorPlugin::m_pMethodInfo;
HTypeProperty *SelectManipulatorPlugin::m_pMethod;

SelectManipulatorPlugin::SelectManipulatorPlugin()
{
   m_pSelectCommandPlugin = NULL;


   // Common Properties
   m_pPropertyArray = HPropertyArray::New();

   // These properties are static for a couple reasons
   // 1. We want Select, Translate, Scale, and Rotate to all use these same properties
   // 2. We want to have CommandUIPlugins change their values
   // There is a problem however in doing this, the first parent set in a property will always remain the parent
   // no matter how many PropertyArrays the property get added to, thus the SelectManipulatorPlugin
   // will receive all the OnObjectNotify calls, not the SelectManipulator, TranslateManipulator, RotateManipulator...
   // This is OK, because it gives us the behavior we desire... I think...
   if (m_pMouseOverInfo==NULL) 
   {
      m_pMouseOverInfo = HBoolProperty::HInfo::New("Mouse Over", "MouseOver", FALSE);
      m_pMouseOver = HBoolProperty::New( m_pMouseOverInfo, NULL );

      m_pRealTimeInfo = HBoolProperty::HInfo::New("Realtime Select", "RealTimeSelect", TRUE);
      m_pRealTime = HBoolProperty::New( m_pRealTimeInfo, NULL );

      m_pSelectObscuredInfo = HBoolProperty::HInfo::New("Select Obscured", "SelectObsured", TRUE);
      m_pSelectObscured = HBoolProperty::New( m_pSelectObscuredInfo, NULL ); 

      m_pMethodNodes = HTypeProperty::HInfo::AllocNodes(SM_NUMMETHODS);
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_SINGLE)->SetName("Single");
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_SINGLE)->SetMatchName("Single");
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_BOUND)->SetName("Bound");
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_BOUND)->SetMatchName("Bound");
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_LASSO)->SetName("Lasso");
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_LASSO)->SetMatchName("Lasso");
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_POLYLASSO)->SetName("Poly Lasso");
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_POLYLASSO)->SetMatchName("PolyLasso");
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_PAINT)->SetName("Paint");
      HTypeProperty::HInfo::GetNodeAt(m_pMethodNodes, SM_PAINT)->SetMatchName("Paint");

      m_pMethodInfo = HTypeProperty::HInfo::New("Select Method", "SelectMethod", m_pMethodNodes, SM_NUMMETHODS, SM_SINGLE );
      m_pMethodInfo->SetDisplayAsRadios(TRUE);
      m_pMethod = HTypeProperty::New( m_pMethodInfo, NULL );
   }

   m_pPropertyArray->Add( m_pMouseOver );
   m_pPropertyArray->Add( m_pRealTime );
   m_pPropertyArray->Add( m_pSelectObscured );
   m_pPropertyArray->Add( m_pMethod );
}

#define SAFE_DELETE( ptr ) {if (ptr){ delete ptr; ptr = NULL;}}

SelectManipulatorPlugin::~SelectManipulatorPlugin()
{
   SAFE_DELETE( m_pMouseOver );
   SAFE_DELETE( m_pMouseOverInfo );

   SAFE_DELETE( m_pRealTime );
   SAFE_DELETE( m_pRealTimeInfo );

   SAFE_DELETE( m_pSelectObscured );
   SAFE_DELETE( m_pSelectObscuredInfo );


   SAFE_DELETE( m_pMethod );
   SAFE_DELETE( m_pMethodInfo );

   if (m_pMethodNodes) {
      delete [] m_pMethodNodes;
      m_pMethodNodes = NULL;
   }

   delete m_pPropertyArray;
}

HPropertyArray *SelectManipulatorPlugin::GetPropertyArray() const
{
   return m_pPropertyArray;
}

ManipulatorInstance *SelectManipulatorPlugin::CreateInstance()
{
   return SNEW SelectManipulator( this );
}

const char *SelectManipulatorPlugin::GetTitleName() const { return "Select"; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'Q')
};

void SelectManipulatorPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

UINT SelectManipulatorPlugin::GetBitmapID() const
{
   return IDB_SELECT;
}

void SelectManipulatorPlugin::Init(HCommandPluginManager *pCommandManager)
{
// Collect dependencies here
   m_pSelectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Select");
   if (m_pSelectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select ).");
}

BOOL SelectManipulatorPlugin::SupportSOT( SnipeObjectType nType)
{
   return nType != SOT_Unknown;
}

BOOL SelectManipulatorPlugin::OnObjectNotify( ObjectNotificationMsg nMsg, WPARAM wParam, LPARAM lParam ) 
{ 
   switch ( nMsg ) 
   {
   case NOTIFYOBJ_VALUECHANGED: 
      {
         HProperty *pProperty = (HProperty *)lParam;
         if (pProperty == m_pMouseOver) 
         {
            if (!m_pMouseOver->GetValue())
               HSelectionList::SetMouseOverObject(NULL);
            return TRUE;
         }
         break;
      }
   }

   return HManipulatorPluginHelper::OnObjectNotify( nMsg, wParam, lParam );
}

// Instance ////////////////////////////////////////////////
SelectManipulator::SelectManipulator( SelectManipulatorPlugin *pPluginHelper) :
 ManipulatorInstance( pPluginHelper )
{
   m_pPropertyArray = HPropertyArray::New();

   m_pOptionsInfo = HProperty::HInfo::New("Options", "Options");
   m_pOptions = HProperty::New( m_pOptionsInfo, NULL );
   m_pPropertyArray->Add( m_pOptions );

   m_pOptions->AddProperty(GetSelectManipulatorPlugin()->m_pMouseOver);
   m_pOptions->AddProperty(GetSelectManipulatorPlugin()->m_pRealTime);
   m_pOptions->AddProperty(GetSelectManipulatorPlugin()->m_pSelectObscured);
   m_pOptions->AddProperty(GetSelectManipulatorPlugin()->m_pMethod);

   Reset();
}

SelectManipulator::~SelectManipulator()
{
   delete m_pOptions;
   delete m_pOptionsInfo;

   delete m_pPropertyArray;
}

HPropertyArray *SelectManipulator::GetPropertyArray() const
{
   return m_pPropertyArray;
}

KeyModifiers SelectManipulator::GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const 
{
   switch ( mousestate.GetValue() )
   {
   case MOUSESTATE_NONE:
   case MOUSESTATE_LEFT:
      return KeyModifiers( KEYMOD_NONE | KEYMOD_SHIFT | KEYMOD_CTRL | KEYMOD_SHIFTCTRL );
   default:
      return KeyModifiers( KEYMOD_NOTSUPPORTED );
   }
}

const char *SelectManipulator::GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const 
{ 
   if (mousestate == MOUSESTATE_LEFT)
   {
      switch (keymodifier.GetValue())
      {
      case KEYMOD_SHIFTCTRL:
         return "Subtract from existing selection.";
      case KEYMOD_SHIFT:
         return "Add to the existing selection.";
      case KEYMOD_CTRL:
         return "Invert with the existing selection.";
      default:
         switch (GetSelectManipulatorPlugin()->m_pMethod->GetValue())
         {
         case SelectManipulatorPlugin::SM_SINGLE:
            return "Click to select an object, or draw a rectangular selection.";
         case SelectManipulatorPlugin::SM_BOUND:
            return "Draw a rectangular selection.";
         case SelectManipulatorPlugin::SM_LASSO:
            return "Draw freeform selection.";
         case SelectManipulatorPlugin::SM_POLYLASSO:
            return "Click to set a selection segment point.";
         case SelectManipulatorPlugin::SM_PAINT:
            return "Paint a selection.";
         }
      }
   }

   return NULL;
}

BOOL SelectManipulator::OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   switch (GetSelectManipulatorPlugin()->m_pMethod->GetValue())
   {
   case SelectManipulatorPlugin::SM_SINGLE:
      switch (keymodifier.GetValue())
      {
      case KEYMOD_NONE:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_SINGLEPICK));
         return TRUE;
      case KEYMOD_SHIFTCTRL:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_SUBTRACT));
         return TRUE;
      case KEYMOD_SHIFT:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_ADD));
         return TRUE;
      case KEYMOD_CTRL:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_TOGGLE));
         return TRUE;
      }
      break;

   case SelectManipulatorPlugin::SM_BOUND:
      switch (keymodifier.GetValue())
      {
      case KEYMOD_NONE:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_BOUND));
         return TRUE;
      case KEYMOD_SHIFTCTRL:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_BOUND_SUBTRACT));
         return TRUE;
      case KEYMOD_SHIFT:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_BOUND_ADD));
         return TRUE;
      case KEYMOD_CTRL:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_BOUND_TOGGLE));
         return TRUE;
      }
      break;

   case SelectManipulatorPlugin::SM_LASSO:
      switch (keymodifier.GetValue())
      {
      case KEYMOD_NONE:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_LASSO));
         return TRUE;
      case KEYMOD_SHIFTCTRL:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_LASSO_SUBTRACT));
         return TRUE;
      case KEYMOD_SHIFT:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_LASSO_ADD));
         return TRUE;
      case KEYMOD_CTRL:
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_LASSO_TOGGLE));
         return TRUE;
      }
      break;

    case SelectManipulatorPlugin::SM_POLYLASSO:
       switch (keymodifier.GetValue())
       {
       case KEYMOD_NONE:
          ::SetCursor(AfxGetApp()->LoadCursor(IDC_POLYLASSO));
          return TRUE;
       case KEYMOD_SHIFTCTRL:
          ::SetCursor(AfxGetApp()->LoadCursor(IDC_POLYLASSO_SUBTRACT));
          return TRUE;
       case KEYMOD_SHIFT:
          ::SetCursor(AfxGetApp()->LoadCursor(IDC_POLYLASSO_ADD));
          return TRUE;
       case KEYMOD_CTRL:
          ::SetCursor(AfxGetApp()->LoadCursor(IDC_POLYLASSO_TOGGLE));
          return TRUE;
       }
       break;

    case SelectManipulatorPlugin::SM_PAINT:
//       switch (keymodifier.GetValue())
//       {
//       case KEYMOD_NONE:
//         ::SetCursor(AfxGetApp()->LoadCursor(IDC_PAINT));
//         return TRUE;
//       case KEYMOD_SHIFTCTRL:
//          ::SetCursor(AfxGetApp()->LoadCursor(IDC_PAINT_SUBTRACT));
//          return TRUE;
//       case KEYMOD_SHIFT:
//          ::SetCursor(AfxGetApp()->LoadCursor(IDC_PAINT_ADD));
//          return TRUE;
//       case KEYMOD_CTRL:
//          ::SetCursor(AfxGetApp()->LoadCursor(IDC_PAINT_TOGGLE));
//          return TRUE;
//       }
       break;
   }

   return ManipulatorInstance::OnSetCursor( keymodifier, mousestate, point, pView );
}


void SelectManipulator::PostDraw(HCGLBaseView *pView)
{
   if (m_pMarqueeView != pView)
      return;

   switch (GetSelectManipulatorPlugin()->m_pMethod->GetValue()) 
   {
      case SelectManipulatorPlugin::SM_SINGLE:
         Single_PostDraw(pView);
         break;
      case SelectManipulatorPlugin::SM_BOUND:
         Bound_PostDraw(pView);
         break;
      case SelectManipulatorPlugin::SM_LASSO:
         Lasso_PostDraw(pView);
         break;
      case SelectManipulatorPlugin::SM_POLYLASSO:
         PolyLasso_PostDraw(pView);
         break;
      case SelectManipulatorPlugin::SM_PAINT:
         Paint_PostDraw(pView);
         break;
   }
}

BOOL SelectManipulator::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   BOOL bInvalidate = FALSE;

   switch (GetSelectManipulatorPlugin()->m_pMethod->GetValue()) 
   {
      case SelectManipulatorPlugin::SM_SINGLE:
         bInvalidate = Single_OnLButtonDown(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_BOUND:
         bInvalidate = Bound_OnLButtonDown(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_LASSO:
         bInvalidate = Lasso_OnLButtonDown(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_POLYLASSO:
         bInvalidate = PolyLasso_OnLButtonDown(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_PAINT:
         bInvalidate = Paint_OnLButtonDown(keymodifier, mousestate, point, pView);
         break;
   }

   if (bInvalidate)
      pView->Invalidate();

   return TRUE;
}

void GetContinuousEdges( HEdge *pStartEdge, HSnipeObjectList *pList );

BOOL SelectManipulator::OnLButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   BOOL bInvalidate = FALSE;

   if (GetHMainInterface()->GetCurrentSelectionSOT()==SOT_Edge)
   {
      HPickHitArray *pPickHitArray = HPickHitArray::New();
      DoPick( pView, CRect(point, point), pPickHitArray );
      if (pPickHitArray->GetNumHits())
      {
         ASSERT(pPickHitArray->GetNumHits()==1);
         HEdge *pEdge = (HEdge *)pPickHitArray->GetHit(0)->GetName(0);
         HSnipeObjectList *pList = HSnipeObjectList::New();

         GetContinuousEdges( pEdge, pList );
         bInvalidate = OnSelect(pView, pList, FALSE, NULL, pEdge);

         delete pList;
      }
      delete pPickHitArray;
   }

   if (bInvalidate)
   {
      pView->Invalidate();
      return TRUE;
   }

   return FALSE;
}

BOOL SelectManipulator::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   if (!m_pMarqueeView)
      return FALSE;

   BOOL bInvalidate = FALSE;

   switch (GetSelectManipulatorPlugin()->m_pMethod->GetValue()) 
   {
      case SelectManipulatorPlugin::SM_SINGLE:
         bInvalidate = Single_OnLButtonUp(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_BOUND:
         bInvalidate = Bound_OnLButtonUp(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_LASSO:
         bInvalidate = Lasso_OnLButtonUp(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_PAINT:
         bInvalidate = Paint_OnLButtonUp(keymodifier, mousestate, point, pView);
         break;
   }

   if (bInvalidate)
      pView->Invalidate();

   return TRUE;
}

BOOL SelectManipulator::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   if (!m_pMarqueeView)
   {
      if (GetSelectManipulatorPlugin()->m_pMouseOver->GetValue())
      {
         HSnipeObject *pNewMouseOverObject = NULL;

         HPickHitArray *pPickHitArray = HPickHitArray::New();
         DoPick( pView, CRect(point, point), pPickHitArray );

         if (pPickHitArray->GetNumHits() == 1)
         {
            pNewMouseOverObject = (HSnipeObject *)pPickHitArray->GetHit(0)->GetName(0);
         }

         delete pPickHitArray;

         if ( pNewMouseOverObject != HSelectionList::GetMouseOverObject() )
         {
            HSelectionList::SetMouseOverObject( pNewMouseOverObject );
            pView->Invalidate();
            return TRUE;
         }
      }
      return FALSE;
   }

   BOOL bInvalidate = FALSE;

   switch (GetSelectManipulatorPlugin()->m_pMethod->GetValue()) 
   {
      case SelectManipulatorPlugin::SM_SINGLE:
         bInvalidate = Single_OnMouseMove(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_BOUND:
         bInvalidate = Bound_OnMouseMove(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_LASSO:
         bInvalidate = Lasso_OnMouseMove(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_POLYLASSO:
         bInvalidate = PolyLasso_OnMouseMove(keymodifier, mousestate, point, pView);
         break;
      case SelectManipulatorPlugin::SM_PAINT:
         bInvalidate = Paint_OnMouseMove(keymodifier, mousestate, point, pView);
         break;
   }

   if (bInvalidate)
      pView->Invalidate();

   return TRUE;
}

BOOL SelectManipulator::OnApply( HCGLBaseView *pView ) 
{ 
   BOOL bInvalidate = FALSE;

   switch (GetSelectManipulatorPlugin()->m_pMethod->GetValue()) 
   {
      case SelectManipulatorPlugin::SM_SINGLE:
         bInvalidate = Bound_OnApply(pView);
         break;
      case SelectManipulatorPlugin::SM_BOUND:
         bInvalidate = Bound_OnApply(pView);
         break;
      case SelectManipulatorPlugin::SM_LASSO:
         bInvalidate = Lasso_OnApply(pView);
         break;
      case SelectManipulatorPlugin::SM_POLYLASSO:
         bInvalidate = PolyLasso_OnApply(pView);
         break;
      case SelectManipulatorPlugin::SM_PAINT:
         bInvalidate = Paint_OnApply(pView);
         break;
   }

   if (bInvalidate)
      pView->Invalidate();

   return TRUE;
}

BOOL SelectManipulator::OnCancel( HCGLBaseView *pView ) 
{ 
   Reset();
   return TRUE;
}

void SelectManipulator::Reset()
{
   m_pMarqueeView = NULL;
   m_pointlist.RemoveAll();
   HSelectionList::SetMouseOverObject( NULL );
}

void SelectManipulator::DoPick( HCGLBaseView *pView, const CRect &rcRect, HPickHitArray *pPickHitArray, BOOL *bOutSinglePick/*=NULL*/ )
{
   pView->DoPick( rcRect, pPickHitArray, bOutSinglePick );
}

///////////////////////////////////////////////////////////////////////////////////////
// Single

void SelectManipulator::Single_PostDraw(HCGLBaseView *pView)
{
   if (!m_pMarqueeView)
      return;

   pView->Push2DAttributes();
   glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

   BOOL bStipple = glIsEnabled( GL_LINE_STIPPLE );
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1,0x0F0F);

   glBegin(GL_LINES);
      glVertex2f((float)m_rcMarquee.left, (float)m_rcMarquee.top);
      glVertex2f((float)m_rcMarquee.right, (float)m_rcMarquee.top);

      glVertex2f((float)m_rcMarquee.left, (float)m_rcMarquee.top);
      glVertex2f((float)m_rcMarquee.left, (float)m_rcMarquee.bottom);

      glVertex2f((float)m_rcMarquee.right, (float)m_rcMarquee.top);
      glVertex2f((float)m_rcMarquee.right, (float)m_rcMarquee.bottom);

      glVertex2f((float)m_rcMarquee.left, (float)m_rcMarquee.bottom);
      glVertex2f((float)m_rcMarquee.right, (float)m_rcMarquee.bottom);
   glEnd();

   glColor4f(1.0f, 1.0f, 0.0f, 0.05f);
   glBegin(GL_QUADS);
      glVertex2f((float)m_rcMarquee.left + 0.5f, (float)m_rcMarquee.top + 0.5f);
      glVertex2f((float)m_rcMarquee.right + 0.5f, (float)m_rcMarquee.top + 0.5f);
      glVertex2f((float)m_rcMarquee.right + 0.5f, (float)m_rcMarquee.bottom + 0.5f);
      glVertex2f((float)m_rcMarquee.left + 0.5f, (float)m_rcMarquee.bottom + 0.5f);
   glEnd();

   if (!bStipple)
   	glDisable(GL_LINE_STIPPLE);

   pView->PopAttributes();
}

BOOL SelectManipulator::Single_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   m_keymodifier = keymodifier;
   m_origpoint = point;
	m_rcMarquee.SetRect( point, point);

   BOOL bSingleSelect, bResult;
   bResult = Single_DoSelect( pView, TRUE, &bSingleSelect );
   if (bSingleSelect) {
      Reset();
      GetHMainInterface()->CommitTempCommand(); // Could do nothing
      OnSingleSelect( point, pView );
      return bResult;
   }

   m_pMarqueeView = pView;
   return bResult;
}

BOOL SelectManipulator::Single_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   if (m_pMarqueeView==NULL)
      return FALSE;

   m_keymodifier = keymodifier;
	m_rcMarquee.right = point.x;
	m_rcMarquee.bottom = point.y;

   BOOL bResult;
   if (GetSelectManipulatorPlugin()->m_pRealTime->GetValue()) {
      m_pMarqueeView = NULL;
      bResult = Single_DoSelect( pView, TRUE );
      m_pMarqueeView = pView;
   }
   else
      bResult = TRUE;

   return bResult;
}

BOOL SelectManipulator::Single_OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView )   
{
   if (m_pMarqueeView==NULL)
      return FALSE;

   m_keymodifier = keymodifier;
   return Single_OnApply( pView );
}

BOOL SelectManipulator::Single_DoSelect( HCGLBaseView *pView, BOOL bTemp, BOOL *bSingleSelect )
{
   HPickHitArray *pPickHitArray = HPickHitArray::New();
   DoPick( pView, m_rcMarquee, pPickHitArray, bSingleSelect );
   HSnipeObjectList *pSelectList = HSnipeObjectList::New();

   UINT nNumHits = pPickHitArray->GetNumHits();
   for (UINT nHitIndex=0; nHitIndex<nNumHits; nHitIndex++)
   {
      HSnipeObject *pObject = (HSnipeObject *)pPickHitArray->GetHit(nHitIndex)->GetName(0);
      ASSERT(pObject);
      pSelectList->AddTail( pObject );
   }
   delete pPickHitArray;

   BOOL bResult = OnSelect(pView, pSelectList, bTemp, bSingleSelect);
   delete pSelectList;

   return bResult;
}

BOOL SelectManipulator::OnSelect( HCGLBaseView *pView, HSnipeObjectList *pSelectList, BOOL bTemp, BOOL *bSingleSelect/*=NULL*/, HSnipeObject *pFocus/*=NULL*/ )
{
   HMainInterface *pMI = GetHMainInterface();

   HSnipeObject *pHitObject = NULL;
   if (pSelectList->GetSize() == 1)
      pHitObject = pSelectList->GetHead();

   String strOptions;
   String strObjects;

   if (pFocus)
   {
      String strFocus;
      if (pMI->ObjectToDescription(pFocus, strFocus))
         strOptions+="-f "+strFocus;
   }

   switch (m_keymodifier.GetValue())
   {
      case KEYMOD_SHIFTCTRL:
         strOptions += " -o SUBTRACT";
         if (bSingleSelect)
            *bSingleSelect = FALSE;
         break;
      case KEYMOD_SHIFT:
         strOptions += " -o ADD";
         break;
      case KEYMOD_CTRL:
         strOptions += " -o TOGGLE";
         if (bSingleSelect)
            *bSingleSelect = FALSE;
         break;
      default:
         if (bSingleSelect && *bSingleSelect && pHitObject && pHitObject->IsSelected())
            strOptions += " -o ADD";
   }

   if (!pMI->ObjectsToDescription( pSelectList, strObjects ))
   {
      DisplayWarning( strObjects );
      return FALSE;
   }
   //strObjects = "Vertex(1:13000)";

   if (bSingleSelect && pHitObject==NULL)
      *bSingleSelect = FALSE;

   String strParameters = strOptions;

   // Lets see if we should set the static focus edge and focus patch
   if ( pHitObject && (strOptions.IsEmpty() || strOptions == " -o ADD")) // Must being adding to selection
   {
      if (pHitObject->IsKindOf(SOT_Vertex)) // selected a vertex
      {
         ASSERT(pView->IsKindOf(SVT_CGLModelView));

         HEdge *pEdge = ((HVertex *)pHitObject)->FindNearestEdge((HCGLModelView *)pView, m_rcMarquee.CenterPoint());
         if (pEdge)
         {
            String strEdgeDescription;
            if (pMI->ObjectToDescription(pEdge, strEdgeDescription))
               strParameters += (String)" -n " + strEdgeDescription; 
         }
         else
         {
            HPoly *pFace = ((HVertex *)pHitObject)->FindNearestFace((HCGLModelView *)pView, m_rcMarquee.CenterPoint());
            if (pFace)
            {
               String strFaceDescription;
               if (pMI->ObjectToDescription(pFace, strFaceDescription))
                  strParameters += (String)" -n " + strFaceDescription; 
            }
         }
      }
   }

   // Now lets add the objects
   if (!strObjects.IsEmpty())
   {
      if (!strParameters.IsEmpty())
         strParameters += (String)" ";
      strParameters += strObjects;
   }

   pMI->UndoTempCommand();

   int nCount = strParameters.Length();
   if (nCount == 0 && pView->GetDocument()->GetSelectionList()->IsEmpty())
      return TRUE;

   StringList strResults;
   pMI->DoCommand( GetSelectManipulatorPlugin()->m_pSelectCommandPlugin, strParameters, strResults, bTemp );

   return TRUE;
}

BOOL SelectManipulator::Single_OnApply( HCGLBaseView *pView )
{
   Reset();
   return Single_DoSelect( pView, FALSE );
}

///////////////////////////////////////////////////////////////////////////////////////
// Bound

void SelectManipulator::Bound_PostDraw(HCGLBaseView *pView)
{
   if (!m_pMarqueeView)
      return;

   pView->Push2DAttributes();
   glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

   BOOL bStipple = glIsEnabled( GL_LINE_STIPPLE );
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1,0x0F0F);

   glBegin(GL_LINES);
      glVertex2f((float)m_rcMarquee.left, (float)m_rcMarquee.top);
      glVertex2f((float)m_rcMarquee.right, (float)m_rcMarquee.top);

      glVertex2f((float)m_rcMarquee.left, (float)m_rcMarquee.top);
      glVertex2f((float)m_rcMarquee.left, (float)m_rcMarquee.bottom);

      glVertex2f((float)m_rcMarquee.right, (float)m_rcMarquee.top);
      glVertex2f((float)m_rcMarquee.right, (float)m_rcMarquee.bottom);

      glVertex2f((float)m_rcMarquee.left, (float)m_rcMarquee.bottom);
      glVertex2f((float)m_rcMarquee.right, (float)m_rcMarquee.bottom);
   glEnd();

   glColor4f(1.0f, 1.0f, 0.0f, 0.05f);
   glBegin(GL_QUADS);
      glVertex2f((float)m_rcMarquee.left + 0.5f, (float)m_rcMarquee.top + 0.5f);
      glVertex2f((float)m_rcMarquee.right + 0.5f, (float)m_rcMarquee.top + 0.5f);
      glVertex2f((float)m_rcMarquee.right + 0.5f, (float)m_rcMarquee.bottom + 0.5f);
      glVertex2f((float)m_rcMarquee.left + 0.5f, (float)m_rcMarquee.bottom + 0.5f);
   glEnd();

   if (!bStipple)
   	glDisable(GL_LINE_STIPPLE);

   pView->PopAttributes();
}

BOOL SelectManipulator::Bound_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   m_keymodifier = keymodifier;
   m_origpoint = point;
	m_rcMarquee.SetRect( point, point);
   m_pMarqueeView = pView;
   return TRUE;
}

BOOL SelectManipulator::Bound_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   //if (mousestate!= MOUSESTATE_LEFT)
   //   return FALSE;

   m_keymodifier = keymodifier;
	m_rcMarquee.right = point.x;
	m_rcMarquee.bottom = point.y;

   BOOL bResult;
   if (GetSelectManipulatorPlugin()->m_pRealTime->GetValue()) {
      m_pMarqueeView = NULL;
      bResult = Bound_DoSelect( pView, TRUE );
      m_pMarqueeView = pView;
   }
   else
      bResult = TRUE;

   return bResult;
}

BOOL SelectManipulator::Bound_OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView )   
{
   m_keymodifier = keymodifier;
   return Bound_OnApply( pView );
}

BOOL SelectManipulator::Bound_DoSelect( HCGLBaseView *pView, BOOL bTemp )
{
   HPickHitArray *pPickHitArray = HPickHitArray::New();
   DoPick( pView, m_rcMarquee, pPickHitArray );
   HSnipeObjectList *pSelectList = HSnipeObjectList::New();

   UINT nNumHits = pPickHitArray->GetNumHits();

   for (UINT nHitIndex=0; nHitIndex<nNumHits; nHitIndex++)
   {
      HSnipeObject *pObject = (HSnipeObject *)pPickHitArray->GetHit(nHitIndex)->GetName(0);
      ASSERT(pObject);
      pSelectList->AddTail( pObject );
   }
   delete pPickHitArray;

   BOOL bResult = OnSelect(pView, pSelectList, bTemp);
   delete pSelectList;

   return bResult;
}

//
//   String strOptions;
//   String strObjects;
//
//   switch (m_keymodifier.GetValue())
//   {
//      case KEYMOD_SHIFTCTRL:
//         strOptions = "-o SUBTRACT";
//         break;
//      case KEYMOD_SHIFT:
//         strOptions = "-o ADD";
//         break;
//      case KEYMOD_CTRL:
//         strOptions = "-o TOGGLE";
//         break;
//   }
//
//    if (!pMI->ObjectsToDescription( pSelectList, strObjects ))
//    {
//        DisplayWarning( strObjects );
//       return FALSE;
//    }
//
//   String strParameters;
//   
//   if (!strOptions.IsEmpty())
//      strParameters += (String )" " + strOptions;
//   
//   if (!strObjects.IsEmpty())
//      strParameters += (String)" " + strObjects;
//
//   int nCount = strParameters.Length();
//
//   UndoTempCommand();
//
//   if (nCount == 0 && pView->GetDocument()->GetSelectionList()->IsEmpty())
//      return TRUE;
//
//   DoCommand( GetSelectManipulatorPlugin()->m_pSelectCommandPlugin, strParameters, bTemp );
//
//   return TRUE;
//}

BOOL SelectManipulator::Bound_OnApply( HCGLBaseView *pView )
{
   Reset();
   return Bound_DoSelect( pView, FALSE );
}

///////////////////////////////////////////////////////////////////////////////////////
// Lasso
void SelectManipulator::Lasso_PostDraw(HCGLBaseView *pView)
{
   if (!m_pMarqueeView)
      return;

   pView->Push2DAttributes();

   glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

   glPointSize(3.0f);
   glBegin(GL_POINTS);
      Vector2 &p = m_pointlist.GetHead();
      glVertex2fv((GLfloat *) &p);
   glEnd();

   glLineWidth(1.0f);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1,0x0F0F);
   glBegin(GL_LINE_STRIP);
      for (POSITION pos = m_pointlist.GetHeadPosition(); pos; ) {
         Vector2 &p = m_pointlist.GetNext(pos);
         glVertex2fv((GLfloat *) &p);
      }
   glEnd();

   ::glDisable(GL_LINE_STIPPLE);

   //glColor4f(0.0f, 0.0f, 0.8f, 0.15f);
   //glBegin(GL_POLYGON);
   //   for (POSITION pos = m_pointlist.GetHeadPosition(); pos; ) {
   //      Vector2 &p = m_pointlist.GetNext(pos);
   //      glVertex2fv((GLfloat *) &p);
   //   }
   //glEnd();
   pView->PopAttributes();
}

BOOL SelectManipulator::Lasso_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   m_pointlist.RemoveAll();
   
   m_origpoint = point;
   m_prevpoint = point;
   m_pointlist.AddTail(Vector2((float)point.x, (float)point.y));

   m_pMarqueeView = pView;

   return TRUE;
}

BOOL SelectManipulator::Lasso_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   //if (mousestate!= MOUSESTATE_LEFT)
   //   return FALSE;

   if (abs(point.x-m_prevpoint.x) < 5 && abs(point.y-m_prevpoint.y) < 5)
      return TRUE;

   m_pointlist.AddTail(Vector2((float)point.x, (float)point.y));
   return TRUE;
}

BOOL SelectManipulator::Lasso_OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   return Lasso_OnApply( pView);
}

BOOL SelectManipulator::Lasso_OnApply( HCGLBaseView *pView )
{
   // Do Selection Here
   Reset();
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
// PolyLasso

void SelectManipulator::PolyLasso_PostDraw(HCGLBaseView *pView)
{
   Lasso_PostDraw( pView );
}

BOOL SelectManipulator::PolyLasso_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   if (!m_pMarqueeView) { // First time in
      m_pointlist.RemoveAll();
   
      m_origpoint = point;
      m_prevpoint = point;

      m_pointlist.AddTail(Vector2((float)point.x, (float)point.y));
      m_pointlist.AddTail(Vector2((float)point.x, (float)point.y)); // point dragging
      m_pMarqueeView = pView;
      return TRUE;
   }

   m_pointlist.AddTail(Vector2((float)point.x, (float)point.y));
   
   // Check to see if we are finished    
   if (abs( m_origpoint.x - point.x ) < TOLERANCE && abs( m_origpoint.y - point.y ) < TOLERANCE)
      return PolyLasso_OnApply( pView );

   return TRUE;
}

BOOL SelectManipulator::PolyLasso_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   Vector2 &lastpoint = m_pointlist.GetTail();
   lastpoint = Vector2((float)point.x, (float)point.y);
   return TRUE;
}

BOOL SelectManipulator::PolyLasso_OnApply( HCGLBaseView *pView )
{
   // Do Selection Here
   Reset();
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
// Paint
void SelectManipulator::Paint_PostDraw(HCGLBaseView *pView)
{
}

BOOL SelectManipulator::Paint_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   return TRUE;
}

BOOL SelectManipulator::Paint_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   return TRUE;
}

BOOL SelectManipulator::Paint_OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   return TRUE;
}

BOOL SelectManipulator::Paint_OnApply( HCGLBaseView *pView )
{
   return TRUE;
}
