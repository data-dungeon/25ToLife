//

#include "stdafx.h"
#include "AddVertexManipulator.h"
#include "../resource.h"
#include "HCGLModelView.h"
#include "HHitArray.h"
#include "HCommandPluginManager.h"
#include "Matrix44.h"
#include "Vector2.h"
#include "MouseOn.h"
#include "HPatchEdge.h"
#include "CommandUIAccel.h"
#include "MemoryStream.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "AddVertexCommand.h"
#include "HPoly.h"
#include "HPatchVertex.h"
#include "HVectorProperty.h"
#include "HSnipeObjectList.h"

#define MANIPULATORSIZE 8.0f

////////////////////////////////////////////////////
AlongEdgeManipulator::AlongEdgeManipulator( HEdge *pEdge, float fPercentage ) :
VertexManipulator()
{ 
   m_pEdge = pEdge;
   SetPercentage( fPercentage, TRUE );
}

void AlongEdgeManipulator::SetPercentage( float fPercentage, BOOL bSnap )
{
   if (bSnap)
   {
      if (fPercentage >= 1.0f - ADDVERTEX_SNAPTOLERANCE)
         fPercentage = 1.0f;
      else if (fPercentage <= ADDVERTEX_SNAPTOLERANCE)
         fPercentage = 0.0f;
      else if ( fPercentage <= .5f + ADDVERTEX_SNAPTOLERANCE &&
         fPercentage >= .5f-ADDVERTEX_SNAPTOLERANCE )
         fPercentage = .5f;
   }

   m_fPercentage = fPercentage; 

   ASSERT( m_pEdge );
   m_vDrawPos = m_pEdge->GetPosAlongEdge( fPercentage ); 
}

void AlongEdgeManipulator::ComputePosition( HCGLBaseView *pView, CPoint &point, const KeyModifiers &keymodifiers )
{
   ASSERT( m_pEdge );

   Vector vMouse;
   m_pEdge->GetWorldPos( vMouse );
   pView->WorldToScreen( vMouse );
   vMouse.x = (float)point.x;
   vMouse.y = (float)point.y;
   pView->ScreenToWorld( vMouse );

   float fPercentage;
   m_pEdge->FindClosestIntersection( pView, point, fPercentage );
   SetPercentage( fPercentage, keymodifiers != KEYMOD_SHIFT );// we want to call this
}

BOOL AlongEdgeManipulator::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) 
{
   HMainInterface *pMI = GetHMainInterface();
   if (pMI->UndoTempCommand()) // this will undo any temp translates leaving only a possible AddVertex command on there from the dangle we need to get rid of
      pMI->KillLastCommand(); // Get Rid of previous Add Vertex command

   ComputePosition( pView, point, keymodifier );

   HEdge *pV0ContEdge = NULL;
   HEdge *pV1ContEdge = NULL;

   String strParameters = "-s LAST ";

   String strEdge;
   pMI->ObjectToDescription( m_pEdge, strEdge );

   if (pAddVertexManipulator->GetPrevContinuousEdge())
   {
      String strPrevEdge;
      pMI->ObjectToDescription( pAddVertexManipulator->GetPrevContinuousEdge(), strPrevEdge );
      strParameters += (String)"-fce " + strPrevEdge + " ";
   }

   String strPrevious;
   if (pAddVertexManipulator->GetPrevVertex())
   {
      if (!keymodifier.m_bCtrlKeyModifier && (m_fPercentage == 0 || m_fPercentage == 1)) // Must be on a vertex
      {
         HVertex *pVertex = m_fPercentage==0 ? m_pEdge->GetFirstVertex() : m_pEdge->GetSecondVertex();

         if (!m_pEdge->GetContinuousEdge(pVertex))
            strParameters += (String)"-lce " + strEdge + " ";
         else 
         {
            for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
            {
               HEdge *pEdge = pVertex->GetSurroundingEdgeAt(i);
               if (pEdge==m_pEdge)
                  continue;

               if (!pEdge->GetContinuousEdge(pVertex))
               {
                  String strCurrEdge;
                  pMI->ObjectToDescription( pEdge, strCurrEdge );
                  strParameters += (String)"-lce " + strCurrEdge + " ";
                  break;
               }
            }
         }
      }
      strParameters += pAddVertexManipulator->GetPrevAddString() + " ";
   }

   strParameters += (String)"-e " + strEdge + " " + m_fPercentage;

   StringList strResults;
   BOOL bResult = pMI->DoCommand( pAddVertexManipulator->GetAddVertexManipulatorPlugin()->m_pAddVertexCommandPlugin, strParameters, strResults, TRUE );

   ASSERT(bResult);
   ASSERT( strResults.GetCount() );

   if ( strResults.GetCount()==2 )
   {
      String strPrevVertex = strResults.RemoveHead();
      HVertex *pPrevVertex = (HVertex *)pMI->DescriptionToObject(strPrevVertex, NULL );
      ASSERT( pPrevVertex );
      pAddVertexManipulator->SetPrevVertex( pPrevVertex, pAddVertexManipulator->GetPrevAddString() );
   }

   ASSERT(strResults.GetCount());

   String strCurrentVertex = strResults.RemoveHead();
   HVertex *pCurrentVertex = (HVertex *)pMI->DescriptionToObject( strCurrentVertex );
   ASSERT( pCurrentVertex );

   if (pAddVertexManipulator->GetPrevVertex())
      pAddVertexManipulator->SetCurrentEdge( HEdge::FindEdge( pAddVertexManipulator->GetPrevVertex(), pCurrentVertex ) );
   else
   {
      if (!keymodifier.m_bCtrlKeyModifier && (m_fPercentage == 0 || m_fPercentage == 1) && m_pEdge->IsKindOf(SOT_PatchEdge))
      {
         HVertex *pVertex = m_fPercentage==0 ? m_pEdge->GetFirstVertex() : m_pEdge->GetSecondVertex();
         HPatchEdge *pPatchEdge = (HPatchEdge *)m_pEdge;

         if (!pPatchEdge->GetContinuousEdge(pVertex))
            pAddVertexManipulator->SetCurrentEdge( m_pEdge );
      }
   }

   pAddVertexManipulator->SetCurrentVertex( pCurrentVertex, (String)"-v " + strCurrentVertex );

   return TRUE;
}

BOOL AlongEdgeManipulator::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) 
{
   GetHMainInterface()->UndoTempCommand(); // Puts back original edge needed for the ComputePosition call

   pAddVertexManipulator->SetCurrentVertex(NULL, "");
   pAddVertexManipulator->SetCurrentEdge(NULL);

   return OnLButtonDown( keymodifier, mousestate, point, pView, pAddVertexManipulator );
}

BOOL AlongEdgeManipulator::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) 
{
   GetHMainInterface()->CommitTempCommand();// this will finalize our splitEdge

   HEdge *pCurrentEdge = pAddVertexManipulator->GetCurrentEdge();

   if (!keymodifier.m_bCtrlKeyModifier && pCurrentEdge && pCurrentEdge->IsKindOf( SOT_PatchEdge ))
   {
      if (((HPatchEdge *)pCurrentEdge)->GetContinuousEdge(pAddVertexManipulator->GetCurrentVertex()))
      {
         pAddVertexManipulator->OnApply( pView );
         return TRUE;
      }
   }

   AtPosManipulator *pNewManipulator = SNEW AtPosManipulator( m_vDrawPos );

   pAddVertexManipulator->SetPrevContinuousEdge(keymodifier.m_bCtrlKeyModifier ? NULL : pCurrentEdge );
   pAddVertexManipulator->SetCurrentVertexManipulator( pNewManipulator ); // this will delete this
   pNewManipulator->OnLButtonDown( keymodifier, mousestate, point, pView, pAddVertexManipulator );
   pNewManipulator->OnLButtonUp( keymodifier, mousestate, point, pView, pAddVertexManipulator );

   return TRUE;
}

////////////////////////////////////////////////////
BOOL AlongEdgeRingManipulator::OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) 
{
   return OnLButtonDown( keymodifier, mousestate, point, pView, pAddVertexManipulator );
}

// We do this here because mouse move calls OnLButtonDown
BOOL AlongEdgeRingManipulator::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) 
{
   if (AlongEdgeManipulator::OnLButtonDown( keymodifier, mousestate, point, pView, pAddVertexManipulator ))
   {
      GetHMainInterface()->UndoTempCommand();
      pAddVertexManipulator->SplitRing( GetEdge(), GetPercentage() );
      return TRUE;
   }

   return FALSE;
}

BOOL AlongEdgeRingManipulator::OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) 
{
   pAddVertexManipulator->OnApply(pView);
   return TRUE;
}

////////////////////////////////////////////////////
void AtPosManipulator::ComputePosition( HCGLBaseView *pView, CPoint &point, const KeyModifiers &keymodifiers )
{
   pView->WorldToScreen( m_vPos );
   m_vPos.x = (float)point.x;
   m_vPos.y = (float)point.y;
   pView->ScreenToWorld( m_vPos );
}

void AtPosManipulator::ReExecuteAddVertexCommand(AddVertexManipulator *pAddVertexManipulator)
{
   HMainInterface *pMI = GetHMainInterface();
   if (pMI->UndoTempCommand()) // Get Rid of Translate temp command
   {
      pMI->KillLastCommand(); // Get Rid of previous Add Vertex command because we know there was a temp translate which means we didn't get here from a AlongEdge split

      String strParameters = "-s LAST ";
      
      if (pAddVertexManipulator->GetPrevContinuousEdge())
      {
         String strEdge;
         pMI->ObjectToDescription( pAddVertexManipulator->GetPrevContinuousEdge(), strEdge );
         strParameters += (String)"-fce " + strEdge + " ";
      }
      
      strParameters += pAddVertexManipulator->GetPrevAddString() + " " + pAddVertexManipulator->GetCurrentAddString();

      StringList strResults;
      BOOL bResult = pMI->DoCommand( pAddVertexManipulator->GetAddVertexManipulatorPlugin()->m_pAddVertexCommandPlugin, strParameters, strResults );

      ASSERT(bResult);
      ASSERT( strResults.GetCount()==2 );

      String strPrevVertex = strResults.RemoveHead();
      String strCurrentVertex = strResults.RemoveHead();

      HVertex *pPrevVertex = (HVertex *)pMI->DescriptionToObject(strPrevVertex );
      HVertex *pCurrentVertex = (HVertex *)pMI->DescriptionToObject(strCurrentVertex );
      ASSERT( pPrevVertex );
      ASSERT( pCurrentVertex );

      pAddVertexManipulator->SetPrevVertex(pPrevVertex, pAddVertexManipulator->GetPrevAddString() );
      pAddVertexManipulator->SetCurrentVertex( pCurrentVertex,  String("-v ") + strCurrentVertex );
      pAddVertexManipulator->SetCurrentEdge( HEdge::FindEdge( pPrevVertex, pCurrentVertex ));
   }
}

void AtPosManipulator::ExecuteAddVertexCommand(AddVertexManipulator *pAddVertexManipulator)
{
   String strParameters = "-s LAST ";
   String strPrevAddString;
   String strCurrentAddString;

   HMainInterface *pMI = GetHMainInterface();

   if (pAddVertexManipulator->GetCurrentEdge())
   {
      String strEdge;
      pMI->ObjectToDescription( pAddVertexManipulator->GetCurrentEdge(), strEdge );
      strParameters += (String)"-fce " + strEdge + " ";
   }

   MemoryStream ms(255);
   ms << m_vPos << '\0';

   if (pAddVertexManipulator->GetCurrentVertex())
   {
      String strVertex;
      pMI->ObjectToDescription( pAddVertexManipulator->GetCurrentVertex(), strVertex );
      strPrevAddString = "-v " + strVertex;
   }
   else
      strPrevAddString = (String)"-p " + ms.GetBuffer();

   strCurrentAddString = (String)"-p " + ms.GetBuffer();
   strParameters += strPrevAddString + " " + strCurrentAddString;

   StringList strResults;
   BOOL bResult = pMI->DoCommand( pAddVertexManipulator->GetAddVertexManipulatorPlugin()->m_pAddVertexCommandPlugin, strParameters, strResults );
   ASSERT(bResult);
   ASSERT( strResults.GetCount()==2 );

   String strPrevVertex = strResults.RemoveHead();
   String strCurrentVertex = strResults.RemoveHead();

   HVertex *pPrevVertex = (HVertex *)pMI->DescriptionToObject(strPrevVertex );
   HVertex *pCurrentVertex = (HVertex *)pMI->DescriptionToObject(strCurrentVertex );
   ASSERT( pPrevVertex );
   ASSERT( pCurrentVertex );

   m_vPositionProperty = pCurrentVertex->GetPositionPoperty();
   ASSERT(m_vPositionProperty);

   pAddVertexManipulator->SetPrevVertex(pPrevVertex, strPrevAddString);
   pAddVertexManipulator->SetCurrentVertex( pCurrentVertex, strCurrentAddString );
   pAddVertexManipulator->SetPrevContinuousEdge(pAddVertexManipulator->GetCurrentEdge());
   pAddVertexManipulator->SetCurrentEdge( HEdge::FindEdge( pPrevVertex, pCurrentVertex ));
}

BOOL AtPosManipulator::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) 
{
   if (pAddVertexManipulator->GetPrevVertex())
      ReExecuteAddVertexCommand(pAddVertexManipulator);

   ComputePosition( pView, point, keymodifier );
   ExecuteAddVertexCommand(pAddVertexManipulator);

   return TRUE;
}

BOOL AtPosManipulator::OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) 
{
   pAddVertexManipulator->OnCancel( pView );

   return TRUE;
}

BOOL AtPosManipulator::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) 
{
   ComputePosition( pView, point, keymodifier );

   String strParameters = "-s LAST ";;

   HMainInterface *pMI = GetHMainInterface();
   pMI->ObjectToDescription( m_vPositionProperty, strParameters );

   MemoryStream ms(255);
   ms << m_vPos << '\0';

   strParameters += (String)"= " + ms.GetBuffer();

   StringList strResults;
   BOOL bResult = pMI->DoCommand( pAddVertexManipulator->GetAddVertexManipulatorPlugin()->m_pStoreValueCommandPlugin, strParameters, strResults, TRUE );
   ASSERT(bResult);

   pAddVertexManipulator->SetCurrentVertex(pAddVertexManipulator->GetCurrentVertex(), String("-p ") + ms.GetBuffer());
   return TRUE;
}

// Plugin //////////////////////////////////////////
AddVertexManipulatorPlugin::AddVertexManipulatorPlugin()
{
   m_pAddVertexCommandPlugin = NULL;
}

ManipulatorInstance *AddVertexManipulatorPlugin::CreateInstance()
{
   return SNEW AddVertexManipulator( this );
}

const char *AddVertexManipulatorPlugin::GetTitleName() const { return "Add Vertex"; }

UINT AddVertexManipulatorPlugin::GetBitmapID() const { return IDB_ADDVERTEX; }

static CommandUIAccel f_pAccel [] = 
{ 
   CommandUIAccel(FVIRTKEY, 'A') 
};

void AddVertexManipulatorPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}


void AddVertexManipulatorPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pAddVertexCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "AddVertex");
   if (m_pAddVertexCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : AddVertex ).");

   m_pStoreValueCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "StoreValue");
   if (m_pStoreValueCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : StoreValue ).");
}

BOOL AddVertexManipulatorPlugin::SupportSOT( SnipeObjectType nType )
{
   return nType == SOT_Vertex || 
    nType == SOT_Edge ||
    nType == SOT_Poly;
}

// Instance ////////////////////////////////////////
AddVertexManipulator::AddVertexManipulator( AddVertexManipulatorPlugin *pPluginHelper) :
 ManipulatorInstance( pPluginHelper )
{
   m_pCurrentVertexManipulator = NULL;// Must do this before you call reset
   Reset();
}

AddVertexManipulator::~AddVertexManipulator()
{
   ASSERT(m_pCurrentVertexManipulator==NULL);
}

KeyModifiers AddVertexManipulator::GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const
{
   switch (mousestate.GetValue())
   {
      case MOUSESTATE_NONE:
         return KeyModifiers( KEYMOD_NONE | KEYMOD_SHIFT | KEYMOD_CTRL ); // For cursors
      case MOUSESTATE_LEFT:
         return KeyModifiers( KEYMOD_NONE | KEYMOD_SHIFT | KEYMOD_CTRL );
      case MOUSESTATE_RIGHT:
         if (m_pPrevVertex)
            return KeyModifiers( KEYMOD_NONE );
         else
            return KeyModifiers( KEYMOD_NONE | KEYMOD_SHIFT );
      default:
          return KeyModifiers( KEYMOD_NOTSUPPORTED );
   }
}

const char *AddVertexManipulator::GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const 
{ 
   switch (mousestate.GetValue())
   {
      case MOUSESTATE_LEFT:
         switch (keymodifier.GetValue())
         {
            case KEYMOD_CTRL:
               return "Add point no auto connect if on vertex";
            case KEYMOD_SHIFT:
               return "Add point no snapping if on edge";
            case KEYMOD_NONE:
               return "Add point with snapping on edge";
         }
         break;
      case MOUSESTATE_RIGHT:
         if (m_pPrevVertex)
            return "Cancel";

         switch (keymodifier.GetValue())
         {
         case KEYMOD_SHIFT:
            return "Split ring no snapping.";
         case KEYMOD_NONE:
            return "Split ring with snapping";
         }
         break;
   }
   return NULL;
}

BOOL AddVertexManipulator::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   HSnipeObjectList *pList = NULL;
   
   if (GetCurrentEdge())
   {
      pList = HSnipeObjectList::New();
      pList->AddTail(GetCurrentEdge());
   }

   HPickHitArray *pPickHitArray = HPickHitArray::New();
   pView->DoPick( CRect(point, point), pPickHitArray, NULL, SOT_Edge, pList );

   if (pList)
      delete pList;

   UINT nNumHits = pPickHitArray->GetNumHits();

   VertexManipulator *pManipulator = NULL;

   if ( pPickHitArray->GetNumHits()==1 ) // SplitEdge
   {
      HEdge *pEdge = (HEdge *)pPickHitArray->GetHit(0)->GetName(0);
      delete pPickHitArray;

      pManipulator = SNEW AlongEdgeManipulator( pEdge, 0 );
   }
   else // AtPosition
   {
      if (mousestate.m_bLeftButton)
      {
         ASSERT( pPickHitArray->GetNumHits()==0);

         delete pPickHitArray;

         Vector vLastPos;
         if (m_pPrevVertex)
            vLastPos = m_pPrevVertex->GetPos();
         else
            vLastPos.Set(0.0f);// needed for z only

         pManipulator = SNEW AtPosManipulator( vLastPos );
      }
   }

   SetCurrentVertexManipulator( pManipulator );
   if (!pManipulator)
      return FALSE;

   return pManipulator->OnLButtonDown( keymodifier, mousestate, point, pView, this ) ;
}

BOOL AddVertexManipulator::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   if (m_pCurrentVertexManipulator)
   {
      if (m_pCurrentVertexManipulator->OnMouseMove( keymodifier, mousestate, point, pView, this ))
      {
         pView->Invalidate();
         return TRUE;
      }
   }

   return FALSE;
}

BOOL AddVertexManipulator::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   if (m_pCurrentVertexManipulator)
   {
      if (m_pCurrentVertexManipulator->OnLButtonUp( keymodifier, mousestate, point, pView, this ))
      {
         pView->Invalidate();
         return TRUE;
      }
   }

   return FALSE;
}

BOOL AddVertexManipulator::OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   if (m_pCurrentVertexManipulator)
      return m_pCurrentVertexManipulator->OnRButtonDown( keymodifier, mousestate, point, pView, this );
   else
   {
      HPickHitArray *pPickHitArray = HPickHitArray::New();
      pView->DoPick( CRect(point, point), pPickHitArray, NULL, SOT_Edge );

      UINT nNumHits = pPickHitArray->GetNumHits();
      VertexManipulator *pManipulator = NULL;

      if ( pPickHitArray->GetNumHits()==1 ) // SplitEdge
      {
         HEdge *pEdge = (HEdge *)pPickHitArray->GetHit(0)->GetName(0);
         delete pPickHitArray;

         pManipulator = SNEW AlongEdgeRingManipulator( pEdge, 0 );
         SetCurrentVertexManipulator( pManipulator );
         return pManipulator->OnRButtonDown( keymodifier, mousestate, point, pView, this ) ;
      }
   }

   return FALSE;
}

BOOL AddVertexManipulator::OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   if (m_pCurrentVertexManipulator)
      return m_pCurrentVertexManipulator->OnRButtonUp( keymodifier, mousestate, point, pView, this );
   return FALSE;
}

BOOL AddVertexManipulator::OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   if (!m_pCurrentVertexManipulator && mousestate == MOUSESTATE_NONE)
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
            pEdge = (HEdge *)pPickHitArray->GetHit(0)->GetName(0);

         delete pPickHitArray;
      }

      if (pEdge)
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_ADD_ALONGEDGE));
      else
         ::SetCursor(AfxGetApp()->LoadCursor(IDC_ADD));
      return TRUE;
   }

   return ManipulatorInstance::OnSetCursor( keymodifier, mousestate, point, pView );
}

void AddVertexManipulator::Reset()
{
   m_pPrevVertex = NULL;
   m_pCurrentVertex = NULL;
   m_pCurrentEdge = NULL;
   m_pPrevContinuousEdge = NULL;
   SetCurrentVertexManipulator(NULL);
}

BOOL AddVertexManipulator::OnApply( HCGLBaseView *pView ) 
{
   GetHMainInterface()->CommitTempCommand();
   Reset();
   pView->Invalidate();

   return TRUE;
}

BOOL AddVertexManipulator::OnCancel( HCGLBaseView *pView ) 
{
   HMainInterface *pMI = GetHMainInterface();

   pMI->UndoTempCommand(); // don't want anything more on undo stack
   pMI->KillLastCommand(); // Get Rid of previous Add Vertex command
   Reset();

   pView->Invalidate();
   return TRUE;
}

class AlignedEdge
{
public:
   HEdge *m_pEdge;
   BOOL m_bAligned;

   AlignedEdge(HEdge *pEdge, BOOL bAligned)
   {
      m_pEdge = pEdge;
      m_bAligned = bAligned;
   }
};

void AddVertexManipulator::SplitRing( HEdge *pEdge, float fPercent )
{
   if (fPercent == 0.0f || fPercent == 1.0f)
      return;

   CList<AlignedEdge *> edgelist;

   edgelist.AddTail(SNEW AlignedEdge(pEdge, TRUE));
   BOOL bSameDirection;
   HEdge *pCurrEdge = NULL;

   HPoly *pPoly = pEdge->GetFirstPoly();
   if (pPoly)
   {
      BOOL bAligned = TRUE;
      for (pCurrEdge = pEdge->GetOppositeEdge(pPoly, &bSameDirection); pCurrEdge && pCurrEdge != pEdge; pCurrEdge = pCurrEdge->GetOppositeEdge(pPoly, &bSameDirection))
      {
         edgelist.AddTail(SNEW AlignedEdge(pCurrEdge, bSameDirection ? bAligned : !bAligned));
         if (!bSameDirection)
            bAligned = !bAligned;
      }
   }

   BOOL bLoop;
   if (!pCurrEdge) // Didn't loop
   {
      bLoop = FALSE;
      pPoly = pEdge->GetSecondPoly();
      if (pPoly)
      {
         BOOL bAligned = TRUE;
         for (pCurrEdge = pEdge->GetOppositeEdge(pPoly, &bSameDirection); pCurrEdge && pCurrEdge != pEdge; pCurrEdge = pCurrEdge->GetOppositeEdge(pPoly, &bSameDirection))
         {
            edgelist.AddHead(SNEW AlignedEdge(pCurrEdge, bSameDirection ? bAligned : !bAligned));
            if (!bSameDirection)
               bAligned = !bAligned;
         }
      }
   }
   else
      bLoop = TRUE;

   // OK we have all the edges we want to split

   float fReversedPercent = 1.0f - fPercent;
   String strParameters;
   if (bLoop)
      strParameters = "-al ";

   HMainInterface *pMI = GetHMainInterface();

   for ( POSITION pos = edgelist.GetHeadPosition(); pos; )
   {
      AlignedEdge *pAlignedEdge = edgelist.GetNext(pos);
      String strEdge;
      if (pMI->ObjectToDescription( pAlignedEdge->m_pEdge, strEdge ))
      {
         if (!strParameters.IsEmpty())
            strParameters += " ";
         strParameters+=(String)"-e " + strEdge;
         if (pAlignedEdge->m_bAligned)
            strParameters+=(String)" " + fPercent;
         else
            strParameters+=(String)" " + fReversedPercent;
      }
      else
         ASSERT(FALSE);
      delete pAlignedEdge;
   }

   if (!strParameters.IsEmpty())
   {
      StringList strResults;
      pMI->DoCommand( GetAddVertexManipulatorPlugin()->m_pAddVertexCommandPlugin, strParameters, strResults, TRUE );
   }
}

