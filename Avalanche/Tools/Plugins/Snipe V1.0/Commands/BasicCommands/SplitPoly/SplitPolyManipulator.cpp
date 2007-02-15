//

#include "stdafx.h"
#include "SplitPolyManipulator.h"
#include "../resource.h"
#include "HCGLModelView.h"
#include "HHitArray.h"
#include "HCommandPluginManager.h"
#include "Matrix44.h"
#include "Vector2.h"
#include "MouseOn.h"
#include "HEdge.h"
#include "CommandUIAccel.h"
#include "MemoryStream.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HPoly.h"

#define MANIPULATORSIZE 8.0f

////////////////////////////////////////////////////
Manipulator *Manipulator::m_pMouseOver;

void Manipulator::SetPos( float fPos, BOOL bSnap )
{
   if (bSnap)
   {
      if (fPos >= 1.0f - SPLITPOLY_SNAPTOLERANCE)
         fPos = 1.0f;
      else if (fPos <= SPLITPOLY_SNAPTOLERANCE)
         fPos = 0.0f;
      else if ( fPos <= .5f + SPLITPOLY_SNAPTOLERANCE &&
      fPos >= .5f-SPLITPOLY_SNAPTOLERANCE )
         fPos = .5f;
   }

   m_fPos = fPos; 

   ASSERT( m_pEdge );
   m_vDrawPos = m_pEdge->GetPosAlongEdge( fPos ); 
}

void Manipulator::ComputePosition( HCGLBaseView *pView, CPoint &point, const KeyModifiers &keymodifiers )
{
   ASSERT( m_pEdge );

   Vector vMouse;
   m_pEdge->GetWorldPos( vMouse );
   pView->WorldToScreen( vMouse );
   vMouse.x = (float)point.x;
   vMouse.y = (float)point.y;
   pView->ScreenToWorld( vMouse );

   float fPos;
   m_pEdge->FindClosestIntersection( pView, point, fPos );
   SetPos( fPos, keymodifiers != KEYMOD_SHIFT );// we want to call this
}

void Manipulator::Draw( RGBAFloat &color )
{
   glPointSize(MANIPULATORSIZE);
   glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

   Vector vPos = GetPos();
   glBegin(GL_POINTS);
   glVertex3fv(&vPos.x);
   glEnd();

   if ( m_fPos == 0.0f || m_fPos == 1.0f )
   {
      glPointSize(MANIPULATORSIZE-4);
      glColor4f( 0.0f, 0.0f, 0.0f, color.GetAlpha() );

      Vector vPos = GetPos();
      glBegin(GL_POINTS);
         glVertex3fv(&vPos.x);
      glEnd();
   }
}

// Plugin //////////////////////////////////////////
SplitPolyManipulatorPlugin::SplitPolyManipulatorPlugin()
{
   m_pAddVertexCommandPlugin = NULL;
}

ManipulatorInstance *SplitPolyManipulatorPlugin::CreateInstance()
{
   return SNEW SplitPolyManipulator( this );
}

const char *SplitPolyManipulatorPlugin::GetTitleName() const { return "SplitPoly"; }

UINT SplitPolyManipulatorPlugin::GetBitmapID() const
{
   return IDB_SPLITPOLY;
}

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FSHIFT, 'A')
};

void SplitPolyManipulatorPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void SplitPolyManipulatorPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pAddVertexCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "AddVertex");
   if (m_pAddVertexCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : AddVertex ).");
}

BOOL SplitPolyManipulatorPlugin::SupportSOT( SnipeObjectType nType)
{
   return nType == SOT_Vertex || 
    nType == SOT_Edge ||
    nType == SOT_Poly;
}

// Instance ////////////////////////////////////////
SplitPolyManipulator::SplitPolyManipulator( SplitPolyManipulatorPlugin *pPluginHelper) :
 ManipulatorInstance( pPluginHelper )
{
   m_pCurrentManipulator = NULL;
   m_nIfClicked = OPERATION_NONE;
}

SplitPolyManipulator::~SplitPolyManipulator()
{
}

KeyModifiers SplitPolyManipulator::GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const
{
   switch (mousestate.GetValue())
   {
      case MOUSESTATE_NONE:
         return KeyModifiers( KEYMOD_NONE | KEYMOD_SHIFT );
      case MOUSESTATE_LEFT:
         return KeyModifiers( KEYMOD_NONE | KEYMOD_SHIFT );
      case MOUSESTATE_RIGHT:
         if (m_vertexmanipulatorlist.GetSize())
            return KeyModifiers( KEYMOD_NONE );
         else
            return KeyModifiers( KEYMOD_NOTSUPPORTED );
      default:
         return KeyModifiers( KEYMOD_NOTSUPPORTED );
   }
}

const char *SplitPolyManipulator::GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const 
{ 
   switch (mousestate.GetValue())
   {
      case MOUSESTATE_LEFT:
         switch (keymodifier.GetValue())
         {
            case KEYMOD_SHIFT:
               return "Add point no snapping";
            case KEYMOD_NONE:
               return "Add point with snapping";
         }
         break;
      case MOUSESTATE_RIGHT:
         return "Commit";
   }
   return NULL;
}

void SplitPolyManipulator::DrawPick( HCGLBaseView *pView )
{
   glPointSize(8.0f); // This does nothing

   for (POSITION pos = m_vertexmanipulatorlist.GetHeadPosition(); pos; )
   {
      Manipulator *pVertexManipulator = m_vertexmanipulatorlist.GetNext( pos );
      Vector vPos =pVertexManipulator->GetPos();

      glLoadName( (int)pVertexManipulator );
      glBegin(GL_POINTS);
         glVertex3fv(&vPos.x);
      glEnd();
   }
}

void SplitPolyManipulator::PostDraw(HCGLBaseView *pView)
{
   glDisable(GL_LIGHTING);
//   glDisable(GL_DEPTH_TEST);

   for (POSITION pos = m_vertexmanipulatorlist.GetHeadPosition(); pos; )
   {
      Manipulator *pVertexManipulator = m_vertexmanipulatorlist.GetNext( pos );
      RGBAFloat color;
      if (pVertexManipulator == Manipulator::m_pMouseOver || pVertexManipulator == m_pCurrentManipulator)
         color.Set(1.0f, 1.0f, 0.0f, 1.0f);
      else
         color.Set(0.0f, 0.5f, 1.0f, 1.0f);

      pVertexManipulator->Draw( color );
   }
}

BOOL SplitPolyManipulator::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   HPickHit *pPickHit = HPickHit::New();
   pView->HitTestManipulator( point, pPickHit );
   if (pPickHit->GetNumNames())
   {
      m_pCurrentManipulator = (Manipulator *)pPickHit->GetName(0);
   }
   else
   {
       m_pCurrentManipulator = AddVertexManipulator( keymodifier, mousestate, point, pView );
       if (m_pCurrentManipulator)
       {
         ExecuteTempCommand();
         pView->Invalidate();
       }
   }

   delete pPickHit;

   return TRUE;
}

BOOL SplitPolyManipulator::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   m_pCurrentManipulator = NULL;
   pView->Invalidate();
   return TRUE;
}

BOOL SplitPolyManipulator::OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   OnApply( pView );
   return TRUE;
}

BOOL SplitPolyManipulator::OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   return TRUE;
}

BOOL SplitPolyManipulator::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   HPickHit *pPickHit = HPickHit::New();
   pView->HitTestManipulator( point, pPickHit );
   Manipulator *pOver = NULL;
   if (pPickHit->GetNumNames())
   {
      pOver = (Manipulator *)pPickHit->GetName(0);
   }

   delete pPickHit;

   HMainInterface *pMI = GetHMainInterface();

   if (pOver != Manipulator::m_pMouseOver)
   {
      Manipulator::m_pMouseOver = pOver;
      pView->Invalidate();
   }

   if (pOver || m_pCurrentManipulator)
   {
      m_nIfClicked = OPERATION_REPOSITION_ALONGEDGE;

      if (!m_pCurrentManipulator)
         return TRUE;
   }

   pMI->UndoTempCommand();

   if (!m_pCurrentManipulator)
   {
      ASSERT(pView->IsKindOf(SVT_CGLModelView));
      HCGLModelView *pModelView = (HCGLModelView *)pView;

      HEdge *pEdge = NULL;

      if (pModelView->IsDisplayShaded())
      {
         HPickHitArray *pPickHitArray = HPickHitArray::New();
         pView->DoPick( CRect(point, point), pPickHitArray, NULL, SOT_Poly );
         if (pPickHitArray->GetNumHits() == 1)
         {
            HPoly *pPoly = (HPoly *)pPickHitArray->GetHit(0)->GetName(0);
            pEdge = pPoly->FindNearestEdge( pModelView, point );
         }

         delete pPickHitArray;
      }
      else
      {
         HPickHitArray *pPickHitArray = HPickHitArray::New();
         pView->DoPick( CRect(point, point), pPickHitArray, NULL, SOT_Edge );
         if (pPickHitArray->GetNumHits() == 1)
         {
            pEdge = (HEdge *)pPickHitArray->GetHit(0)->GetName(0);
         }
        
         delete pPickHitArray;
      }

      if (pEdge)
      {
         HEdge *pTemp;
         if (m_usededges.Lookup( pEdge, pTemp ))
            m_nIfClicked = OPERATION_NOADD_ALONGEDGE;
         else
            m_nIfClicked = OPERATION_ADD_ALONGEDGE;
         HSelectionList::SetMouseOverObject( pEdge );
      }
      else
      {
         m_nIfClicked = OPERATION_NONE;
         HSelectionList::SetMouseOverObject( NULL );
      }

      ExecuteTempCommand();
      pView->Invalidate();

      return TRUE;
   }

   m_pCurrentManipulator->ComputePosition( pView, point, keymodifier );
   ExecuteTempCommand();
   pView->Invalidate();

   return TRUE;
}

BOOL SplitPolyManipulator::OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   switch ( m_nIfClicked )
   {
   case OPERATION_ADD_ALONGEDGE:
      ::SetCursor(AfxGetApp()->LoadCursor(IDC_ADD_ALONGEDGE));
      return TRUE;
   case OPERATION_NOADD_ALONGEDGE:
      ::SetCursor(AfxGetApp()->LoadCursor(IDC_NOADD_ALONGEDGE));
      return TRUE;
   case OPERATION_REPOSITION_ALONGEDGE:
   case OPERATION_NONE:
   default:
      break;
   }

   return ManipulatorInstance::OnSetCursor( keymodifier, mousestate, point, pView );
}

BOOL SplitPolyManipulator::OnApply( HCGLBaseView *pView ) 
{
   GetHMainInterface()->CommitTempCommand();
   Reset();
   pView->Invalidate();
   return TRUE;
}

BOOL SplitPolyManipulator::OnCancel( HCGLBaseView *pView ) 
{
   GetHMainInterface()->UndoTempCommand();
   Reset();
   pView->Invalidate();
   return TRUE;
}

void SplitPolyManipulator::Reset()
{
   for (POSITION pos = m_vertexmanipulatorlist.GetHeadPosition(); pos; )
      delete m_vertexmanipulatorlist.GetNext( pos );
   m_vertexmanipulatorlist.RemoveAll();

   m_usededges.RemoveAll();
}

Manipulator *SplitPolyManipulator::AddVertexManipulator( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   HPickHitArray *pPickHitArray = HPickHitArray::New();
   pView->DoPick( CRect(point, point), pPickHitArray, NULL, SOT_Edge );

   UINT nNumHits = pPickHitArray->GetNumHits();

   if ( pPickHitArray->GetNumHits() !=1 )
   {
      delete pPickHitArray;
      return NULL;
   }

   HEdge *pEdge = (HEdge *)pPickHitArray->GetHit(0)->GetName(0);
   HEdge *pTemp;

   delete pPickHitArray;

   if (m_usededges.Lookup( pEdge, pTemp ))
      return NULL;

   Manipulator *pManipulator = SNEW Manipulator( pEdge, 0);
   pManipulator->ComputePosition( pView, point, keymodifier );

   m_vertexmanipulatorlist.AddTail( pManipulator );
   m_usededges.SetAt( pEdge, pEdge );

   return pManipulator;
}

void SplitPolyManipulator::ExecuteTempCommand()
{
   if (m_vertexmanipulatorlist.IsEmpty())
      return;

   String strParameters;
   HMainInterface *pMI = GetHMainInterface();

   for (POSITION pos = m_vertexmanipulatorlist.GetHeadPosition(); pos; )
   {
      Manipulator *pManipulator = m_vertexmanipulatorlist.GetNext( pos );
      if (!strParameters.IsEmpty())
         strParameters += " ";

      String strEdge;
      pMI->ObjectToDescription( pManipulator->GetEdge(), strEdge );

      String strValue = "-e " + strEdge + " " + pManipulator->GetPercentage();

      strParameters += strValue;
   }

   StringList strResults;
   pMI->DoCommand( GetSplitPolyManipulatorPlugin()->m_pAddVertexCommandPlugin, strParameters, strResults, TRUE );
}