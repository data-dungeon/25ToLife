//

#include "stdafx.h"
#include "TranslateManipulator.h"
#include "CommandUIAccel.h"
#include "HCommandPluginManager.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "RGBColor.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HHitArray.h"
#include "HEdge.h"
#include "../Resource.h"

// Plugin //////////////////////////////////////////
ManipulatorInstance *TranslateManipulatorPlugin::CreateInstance()
{
   return SNEW TranslateManipulator( this );
}

const char *TranslateManipulatorPlugin::GetTitleName() const { return "Translate"; }

UINT TranslateManipulatorPlugin::GetBitmapID() const { return IDB_TRANSLATE; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'W')
};

void TranslateManipulatorPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void TranslateManipulatorPlugin::Init(HCommandPluginManager *pCommandManager)
{
   TransformManipulatorPlugin::Init( pCommandManager );

   // Collect dependencies here
   m_pAttachCommandPlugin = pCommandManager->FindCommandByCLIName( "SNIPEBASICCOMMANDS", "Attach" );
   if (m_pAttachCommandPlugin == NULL)
      THROW_FATAL_ERROR( "Missing dependent command ( SnipeBasicCommand : Attach )." );
  
   m_pWeldCommandPlugin = pCommandManager->FindCommandByCLIName( "SNIPEBASICCOMMANDS", "Weld" );
   if (m_pWeldCommandPlugin == NULL)
      THROW_FATAL_ERROR( "Missing dependent command ( SnipeBasicCommand : Weld )." );
}

// Instance //////////////////////////////////////////////////////////
TranslateManipulator::TranslateManipulator( TranslateManipulatorPlugin *pPluginHelper) :
 TransformManipulator( pPluginHelper )
{
   SetMode( MODE_TRANSLATE_ONLY );
   SetShowManipulator( FALSE );

   m_pRotate->SetVisible( FALSE );
   m_pScale->SetVisible( FALSE );
   m_nDownRepeatCount = 0;
}

TranslateManipulator::~TranslateManipulator()
{
}

KeyModifiers TranslateManipulator::GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const 
{
   KeyModifiers keymodifiers = TransformManipulator::GetKeyModifiers( pView, mousestate );

   switch ( mousestate.GetValue() )
   {
      case MOUSESTATE_LEFTMIDDLE:
      case MOUSESTATE_LEFTRIGHT:
         return KeyModifiers( KEYMOD_NONE ) | keymodifiers;
   }

   return keymodifiers;
}

#if 0
void TranslateManipulator::Draw( HCGLBaseView *pView, float fAlpha/*=1.0f*/, BOOL bOpaqueItemsOnly/*=FALSE*/ )
{
   glPushAttrib( GL_LIGHTING_BIT | GL_ENABLE_BIT );
   GLfloat param[4];
   param[3] = fAlpha;

   Vector center;
   m_curmatrix.GetTranslation( center );
   Vector startcenter;
   m_startmatrix.GetTranslation( startcenter );

   // line back to original location while manipulating
   if (m_bManipulating && !bOpaqueItemsOnly)
   {
      glDisable( GL_LIGHTING );
      glColor4f( 1.0f, 1.0f, 0.0f, fAlpha*0.25f );
      Vector vDelta = startcenter - center;
      glBegin( GL_LINES );
         glVertex3f( 0.0f, 0.0f, 0.0f );
         glVertex3fv( &vDelta.x );
      glEnd();

      glBegin( GL_POINTS );
         glVertex3fv( &vDelta.x );
      glEnd();
   }

// Scale it to n screen pixels big
   glPushMatrix();
   Vector viewcenter = center;
   pView->WorldToView( viewcenter );
   float fPixelSize = pView->GetScreenPixelSize( fabs(viewcenter.z) );
   float fManipulatorSize = fPixelSize * m_pScreenSize->GetValue();
   glScalef( fManipulatorSize, fManipulatorSize, fManipulatorSize );
/////////

   if (IsShowManipulator())
   {
      if (!m_bManipulating || m_nSelectedElement == AXIS_TRANSLATE_SCREEN)
      {
         glDisable( GL_LIGHTING );
         glLoadName( AXIS_TRANSLATE_SCREEN );
         if ((m_nSelectedElement==AXIS_TRANSLATE_SCREEN || m_nMouseOverElement==AXIS_TRANSLATE_SCREEN)/* && !m_bManipulating*/)
            glColor4f( 1.0f, 1.0f, 0.0f, fAlpha );
         else
            glColor4f( 0.0f, 1.0f, 1.0f, fAlpha );
         Vector corner;
         glLineWidth( 1.4f );

         // TODO: Draw a circle if any snapping is on
         if (m_bDrawPick)
            glBegin( GL_QUADS );
         else
            glBegin( GL_LINE_LOOP );

         corner.Set( -0.1f, 0.1f, 0.0f );
         TransformVectorNoTranslate( *pView->GetViewToWorldMatrix(), corner );
         glVertex3fv( &corner.x );

         corner.Set( -0.1f, -0.1f, 0.0f );
         TransformVectorNoTranslate( *pView->GetViewToWorldMatrix(), corner );
         glVertex3fv( &corner.x );

         corner.Set( 0.1f, -0.1f, 0.0f );
         TransformVectorNoTranslate( *pView->GetViewToWorldMatrix(), corner );
         glVertex3fv( &corner.x );

         corner.Set( 0.1f, 0.1f, 0.0f );
         TransformVectorNoTranslate( *pView->GetViewToWorldMatrix(), corner );
         glVertex3fv( &corner.x );

         glEnd();
      }

      BOOL b3D = pView->IsKindOf( SVT_CGL3DView ) != NULL;
      glDisable( GL_LIGHTING );

      if (b3D)
      {
         DrawPlane( AXIS_TRANSLATE_XY, RGBAFloat(1,0,0,fAlpha*0.4f), RGBAFloat(0,1,0,fAlpha*0.4f) );
         if (DrawElement( AXIS_TRANSLATE_Z, RGBAFloat(0,0,1,fAlpha), GetTranslateManipulatorPlugin()->m_nCallListTranslsateZArrow ))
         {
            glBegin( GL_LINES );
               glVertex3f( 0.0f, 0.0f, 0.0f );
               glVertex3f( 0.0f, 0.0f, 0.85f );
            glEnd();
         }
      }

      glRotatef( -90, 0, 1, 0 );
      glRotatef( -90, 1, 0, 0 );
         if (b3D)
            DrawPlane( AXIS_TRANSLATE_XZ, RGBAFloat(0,0,1,fAlpha*0.4f), RGBAFloat(1,0,0,fAlpha*0.4f) );
         if (DrawElement( AXIS_TRANSLATE_Y, RGBAFloat(0,1,0,fAlpha), GetTranslateManipulatorPlugin()->m_nCallListTranslsateZArrow ))
         {
            glBegin( GL_LINES );
            glVertex3f( 0.0f, 0.0f, 0.0f );
            glVertex3f( 0.0f, 0.0f, 0.85f );
            glEnd();
         }
      glRotatef( 90, 1, 0, 0 );
      glRotatef( 90, 0, 1, 0 );

      glRotatef( 90, 1, 0, 0 );
      glRotatef( 90, 0, 1, 0 );
         if (b3D)
            DrawPlane( AXIS_TRANSLATE_YZ, RGBAFloat(0,1,0,fAlpha*0.4f), RGBAFloat(0,0,1,fAlpha*0.4f) );
         if (DrawElement( AXIS_TRANSLATE_X, RGBAFloat(1,0,0,fAlpha), GetTranslateManipulatorPlugin()->m_nCallListTranslsateZArrow ))
         {
            glBegin( GL_LINES );
            glVertex3f( 0.0f, 0.0f, 0.0f );
            glVertex3f( 0.0f, 0.0f, 0.85f );
            glEnd();
         }
      glRotatef( -90, 0, 1, 0 );
      glRotatef( -90, 1, 0, 0 );
   }

   glPopMatrix();

   if (m_bManipulating && bOpaqueItemsOnly && fAlpha==1.0f)
   {
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

      Vector vOffset = center - startcenter;
      float fDistance = vOffset.Length();
      if (fDistance)
      {
         String strValue;
         strValue = fDistance;

         Vector screenstartcenter = startcenter;
         pView->WorldToScreen( screenstartcenter );
         Vector screencenter = center;
         pView->WorldToScreen( screencenter );
         Vector2 viewoffset( screencenter.x - screenstartcenter.x, screencenter.y - screenstartcenter.y );
         viewoffset.Normalize();
         screenstartcenter.x -= pView->GetTextExtent( strValue ).cx / 2;
         if (viewoffset.y > 0)
            screenstartcenter.y -= 14.0f;
         else
            screenstartcenter.y += 14.0f;

         glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
         pView->DrawText( screenstartcenter.x + 1, screenstartcenter.y + 5, strValue );
         glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
         pView->DrawText( screenstartcenter.x, screenstartcenter.y + 4, strValue );
      }
   }

   glPopAttrib();
}
#endif

void TranslateManipulator::OnSingleSelect( CPoint point, HCGLBaseView *pView )
{
   if (pView->IsKindOf( SVT_CGLSceneView ))
      m_nSelectedElement = AXIS_TRANSLATE_XZ;
   else
      m_nSelectedElement = AXIS_TRANSLATE_SCREEN;
   SetShowManipulator(FALSE);
   OnStartManipulating( point, pView );
}

BOOL TranslateManipulator::OnMButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   if (mousestate.m_bLeftMiddleButton)
   {
      if (AttachContinuous( pView ))
      {
         TransformManipulator::OnLButtonUp( keymodifier, mousestate, point, pView ); // let it
         return TRUE;
      }
   }

   return TransformManipulator::OnMButtonDown( keymodifier, mousestate, point, pView );
}

BOOL TranslateManipulator::OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   if (mousestate.m_bLeftRighttButton)
   {
      if (Attach( pView ))
      {
         TransformManipulator::OnLButtonUp( keymodifier, mousestate, point, pView ); // let it
         return TRUE;
      }
   }

   return TransformManipulator::OnRButtonDown( keymodifier, mousestate, point, pView );
}

BOOL TranslateManipulator::OnArrowKeyDown(UINT nChar, UINT nRepCnt, HCGLBaseView *pView) 
{
   HMainInterface *pMI = GetHMainInterface();

   pMI->UndoTempCommand();

   m_nDownRepeatCount += nRepCnt;

   Vector vViewPos = m_pPivotPos->GetValue();
   pView->WorldToView( vViewPos );
   float fDepth = -vViewPos.z;
   float fPixelSize = pView->GetScreenPixelSize(fDepth);

   Vector vDelta(0.0f);

   switch(nChar)
   {
   case VK_UP:
      switch (m_nSelectedElement)
      {
         case 0:
         case AXIS_TRANSLATE_SCREEN:
         case AXIS_TRANSLATE_XY:
         case AXIS_TRANSLATE_Y:
            vDelta.y = (float)m_nDownRepeatCount;
            break;
         case AXIS_TRANSLATE_XZ:
         case AXIS_TRANSLATE_Z:
            vDelta.z = (float)m_nDownRepeatCount;
            break;
      }   
      break;
   case VK_DOWN:
      switch (m_nSelectedElement)
      {
         case 0:
         case AXIS_TRANSLATE_SCREEN:
         case AXIS_TRANSLATE_XY:
         case AXIS_TRANSLATE_Y:
            vDelta.y = -(float)m_nDownRepeatCount;
            break;
         case AXIS_TRANSLATE_XZ:
         case AXIS_TRANSLATE_Z:
            vDelta.z = -(float)m_nDownRepeatCount;
            break;
      }   
      break;
   case VK_LEFT:
      switch (m_nSelectedElement)
      {
         case 0:
         case AXIS_TRANSLATE_SCREEN:
         case AXIS_TRANSLATE_XY:
         case AXIS_TRANSLATE_XZ:
         case AXIS_TRANSLATE_X:
            vDelta.x = -(float)m_nDownRepeatCount;
            break;
      }   
      break;
   case VK_RIGHT:
      switch (m_nSelectedElement)
      {
         case 0:
         case AXIS_TRANSLATE_SCREEN:
         case AXIS_TRANSLATE_XY:
         case AXIS_TRANSLATE_XZ:
         case AXIS_TRANSLATE_X:
            vDelta.x = (float)m_nDownRepeatCount;
            break;
      }   
      break;
   }

   vDelta *= fPixelSize;

   if (!m_nSelectedElement || m_nSelectedElement==AXIS_TRANSLATE_SCREEN)
   {
      Matrix44 matrix = pView->GetViewToWorldMatrix();
      TransformVectorNoTranslate( matrix, vDelta );
   }

   m_pTranslate->StoreValue( vDelta, FALSE );

   return TRUE;
}

BOOL TranslateManipulator::OnArrowKeyUp(UINT nChar, UINT nRepCnt, HCGLBaseView *pView)
{
   GetHMainInterface()->CommitTempCommand();
   m_pTranslate->StoreValue( Vector(0), FALSE );
   m_nDownRepeatCount = 0;
   return TRUE;
}

static BOOL GetBestContinuousEdgeChoice( HVertex *pFromVertex, HEdge *&pOutFromEdge, HVertex *pToVertex, HEdge *&pOutToEdge )
{
   float fBestDot = MIN_FLOAT;
   pOutFromEdge = NULL;
   pOutToEdge = NULL;

   for (UINT i=0; i < pFromVertex->GetNumSurroundingEdges(); i++ )
   {
      HEdge *pCurFromEdge = pFromVertex->GetSurroundingEdgeAt(i);
      if (pCurFromEdge->GetContinuousEdge(pFromVertex))
         continue;

      HVertex *pOtherFromVertex = pCurFromEdge->GetConnectedVertex( pFromVertex );
      Vector vFrom = pOtherFromVertex->GetPos() - pFromVertex->GetPos();
      vFrom.Normalize();

      for (UINT i=0; i < pToVertex->GetNumSurroundingEdges(); i++ )
      {
         HEdge *pCurToEdge = pToVertex->GetSurroundingEdgeAt(i);
         if (pCurToEdge->GetContinuousEdge(pToVertex))
            continue;

         HVertex *pOtherToVertex = pCurToEdge->GetConnectedVertex( pToVertex );
         Vector vTo = pToVertex->GetPos() - pOtherToVertex->GetPos();
         vTo.Normalize();

         float fDot = vFrom.Dot( vTo );
         if (fDot > fBestDot )
         {
            pOutToEdge = pCurToEdge;
            pOutFromEdge = pCurFromEdge;
            fBestDot = fDot;
         }
      }
   }

   return pOutFromEdge!=NULL;
}

static BOOL GetBestParallelEdgeChoice( HVertex *pFromVertex, HEdge *&pOutFromEdge, HVertex *pToVertex, HEdge *&pOutToEdge )
{
   float fBestDot = MIN_FLOAT;
   pOutFromEdge = NULL;
   pOutToEdge = NULL;

   for (UINT i=0; i < pFromVertex->GetNumSurroundingEdges(); i++ )
   {
      HEdge *pCurFromEdge = pFromVertex->GetSurroundingEdgeAt(i);
      if (pCurFromEdge->GetSecondPoly() != NULL)
         continue;

      HVertex *pOtherFromVertex = pCurFromEdge->GetConnectedVertex( pFromVertex );
      Vector vFrom = pOtherFromVertex->GetPos() - pFromVertex->GetPos();
      vFrom.Normalize();

      for (UINT i=0; i < pToVertex->GetNumSurroundingEdges(); i++ )
      {
         HEdge *pCurToEdge = pToVertex->GetSurroundingEdgeAt(i);
         if (pCurToEdge->GetSecondPoly() != NULL)
            continue;

         HVertex *pOtherToVertex = pCurToEdge->GetConnectedVertex( pToVertex );
         Vector vTo = pOtherToVertex->GetPos() - pToVertex->GetPos();
         vTo.Normalize();

         float fDot = vFrom.Dot( vTo );
         if (fDot > fBestDot )
         {
            pOutToEdge = pCurToEdge;
            pOutFromEdge = pCurFromEdge;
            fBestDot = fDot;
         }
      }
   }

   return pOutFromEdge!=NULL;
}

static BOOL AddToParameters( HMainInterface *pMI, HVertex *pFromVertex, HVertex *pToVertex, String &strParameters )
{
   String strFromVertex;
   if (!pMI->ObjectToDescription( pFromVertex, strFromVertex ))
   {
      ASSERT(FALSE);
      return FALSE;
   }

   String strToVertex;
   if (!pMI->ObjectToDescription( pToVertex, strToVertex ))
   {
      ASSERT(FALSE);
      return FALSE;
   }

   if (!strParameters.IsEmpty())
      strParameters += " ";

   strParameters += (String)"-vp -fv " + strFromVertex + " -tv " + strToVertex;

   if (pToVertex->IsKindOf(SOT_PatchVertex)) // Continuous edge stuff
   {
      HEdge *pFromContEdge;
      HEdge *pToContEdge;
      GetBestContinuousEdgeChoice(pFromVertex, pFromContEdge, pToVertex, pToContEdge);

      if (pFromContEdge && pToContEdge)
      {
         String strFromEdge;
         pMI->ObjectToDescription( pFromContEdge, strFromEdge );

         String strToEdge;
         pMI->ObjectToDescription( pToContEdge, strToEdge );

         strParameters += (String)" -fe " + strFromEdge + " -te " + strToEdge;
      }
   }

   return TRUE;
}

BOOL TranslateManipulator::Attach(HCGLBaseView *pView)
{
   HSnipeObject *pFocusObject = HSelectionList::GetFocusObject();
   if (!pFocusObject)
      return FALSE;

   if (!pFocusObject->IsKindOf(SOT_Vertex))
      return FALSE;
   
   HVertex *pFromVertex = (HVertex *)pFocusObject;
   Vector vPos;
   pFocusObject->GetWorldPos(vPos);

   pView->WorldToScreen( vPos );
   CRect rcPos( (int)vPos.x, (int)vPos.y, (int)vPos.x, (int)vPos.y );

   HMainInterface *pMI = GetHMainInterface();

   HSnipeObjectList *pExcludeList = pMI->GetCurrentSelectionList()->CreateSnipeObjectList();
   HPickHitArray *pPickHitArray = HPickHitArray::New();
   pView->DoPick( rcPos, pPickHitArray, NULL, SOT_Vertex, pExcludeList );
   delete pExcludeList;

   HVertex *pToVertex = NULL;
   if (pPickHitArray->GetNumHits() == 1)
      pToVertex = (HVertex *)pPickHitArray->GetHit(0)->GetName(0);
   delete pPickHitArray;

   if (!pToVertex)
      return FALSE;

   String strFromVertex;
   if (!pMI->ObjectToDescription( pFromVertex, strFromVertex ))
   {
      ASSERT(FALSE);
      return FALSE;
   }
   
   String strToVertex;
   if (!pMI->ObjectToDescription( pToVertex, strToVertex ))
   {
      ASSERT(FALSE);
      return FALSE;
   }

   BOOL bWeld = FALSE;
   UINT nNumSurroundingEdges = pFromVertex->GetNumSurroundingEdges();
   for (UINT nIndex=0; nIndex<nNumSurroundingEdges; nIndex++)
   {
      HEdge *pEdge = pFromVertex->GetSurroundingEdgeAt(nIndex);
      if (pEdge->IsInEdge(pToVertex))
      {
         bWeld=TRUE;
         break;
      }
   }
   BOOL bResult;
   StringList strResults;

   if (bWeld)
   {
      String strParameters = (String)strFromVertex + " " + strToVertex;
      bResult = pMI->DoCommand( GetTranslateManipulatorPlugin()->m_pWeldCommandPlugin, strParameters, strResults );
   }
   else
   {
      String strParameters;
      bResult = AddToParameters( pMI, pFromVertex, pToVertex, strParameters );

      if (bResult)
         bResult = pMI->DoCommand( GetTranslateManipulatorPlugin()->m_pAttachCommandPlugin, strParameters, strResults );
   }

   ASSERT(bResult);
   return TRUE;
}

class EdgePair
{
public:
   HEdge *m_pFromEdge;
   HEdge *m_pToEdge;
   BOOL m_bReversed;

   EdgePair()
   {
      m_pFromEdge = NULL;
      m_pToEdge = NULL;
   }

   EdgePair( HEdge *pFromEdge, HEdge *pToEdge, HVertex *pFromVertex, HVertex *pToVertex )
   {
      m_pFromEdge = pFromEdge;
      m_pToEdge = pToEdge;

      ASSERT( pFromEdge->IsInEdge(pFromVertex));
      ASSERT( pToEdge->IsInEdge(pToVertex));

      if ((pFromEdge->GetFirstVertex() == pFromVertex && pToEdge->GetFirstVertex() == pToVertex)
       || (pFromEdge->GetSecondVertex() == pFromVertex && pToEdge->GetSecondVertex() == pToVertex))
          m_bReversed = FALSE;
      else
         m_bReversed = TRUE;
   }

   EdgePair &operator = (const EdgePair &pOther )
   {
      m_pFromEdge = pOther.m_pFromEdge;
      m_pToEdge = pOther.m_pToEdge;
      m_bReversed = pOther.m_bReversed;

      return *this;
   }
};

BOOL TranslateManipulator::AttachContinuous(HCGLBaseView *pView)
{
   HSnipeObject *pFocusObject = HSelectionList::GetFocusObject();
   if (!pFocusObject)
      return FALSE;

   if (!pFocusObject->IsKindOf(SOT_Vertex))
      return FALSE;

   HVertex *pFromVertex = (HVertex *)pFocusObject;
   Vector vPos;
   pFocusObject->GetWorldPos(vPos);

   pView->WorldToScreen( vPos );
   CRect rcPos( (int)vPos.x, (int)vPos.y, (int)vPos.x, (int)vPos.y );

   HMainInterface *pMI = GetHMainInterface();

   HSnipeObjectList *pExcludeList = pMI->GetCurrentSelectionList()->CreateSnipeObjectList();
   HPickHitArray *pPickHitArray = HPickHitArray::New();
   pView->DoPick( rcPos, pPickHitArray, NULL, SOT_Vertex, pExcludeList );
   delete pExcludeList;

   HVertex *pToVertex = NULL;
   if (pPickHitArray->GetNumHits() == 1)
      pToVertex = (HVertex *)pPickHitArray->GetHit(0)->GetName(0);
   delete pPickHitArray;

   if (!pToVertex)
      return FALSE;

   HEdge *pFromEdge;
   HEdge *pToEdge;
   GetBestParallelEdgeChoice(pFromVertex, pFromEdge, pToVertex, pToEdge);

   CList<EdgePair> edgepairlist;

   HEdge *pCurrFromEdge = pFromEdge;
   HVertex *pCurrFromVertex = pFromVertex;

   HEdge *pCurrToEdge = pToEdge;
   HVertex *pCurrToVertex = pToVertex;
   BOOL bWalkPrevious = TRUE;

   while (pCurrFromEdge && pCurrToEdge)
   {
      if (pCurrFromEdge->IsVisible() && pCurrToEdge->IsVisible())
         edgepairlist.AddTail( EdgePair( pCurrFromEdge, pCurrToEdge, pCurrFromVertex, pCurrToVertex) );

      pCurrFromEdge = pCurrFromEdge->GetContinuousEdge( pCurrFromVertex );
      pCurrToEdge = pCurrToEdge->GetContinuousEdge( pCurrToVertex );

      if (pCurrFromEdge==pFromEdge || pCurrToEdge==pToEdge)
      {
         bWalkPrevious = FALSE;
         break;
      }

      if (pCurrFromEdge)
         pCurrFromVertex = pCurrFromEdge->GetConnectedVertex(pCurrFromVertex);

      if (pCurrToEdge)
         pCurrToVertex = pCurrToEdge->GetConnectedVertex(pCurrToVertex);
   }

   // Lets walk Previous Now
   if (bWalkPrevious)
   {
      pCurrFromEdge = pFromEdge;
      pCurrFromVertex = pCurrFromEdge->GetConnectedVertex(pFromVertex);

      pCurrToEdge = pToEdge;
      pCurrToVertex = pCurrToEdge->GetConnectedVertex(pToVertex);

      while (pCurrFromEdge && pCurrToEdge)
      {
         if ( pCurrFromEdge != pFromEdge && pCurrToEdge != pToEdge && pCurrFromEdge->IsVisible() && pCurrToEdge->IsVisible() ) // Already done above
            edgepairlist.AddTail( EdgePair( pCurrFromEdge, pCurrToEdge, pCurrFromVertex, pCurrToVertex) );

         pCurrFromEdge = pCurrFromEdge->GetContinuousEdge( pCurrFromVertex );
         pCurrToEdge = pCurrToEdge->GetContinuousEdge( pCurrToVertex );

         if (pCurrFromEdge==pFromEdge || pCurrToEdge==pToEdge)
            break;

         if (pCurrFromEdge)
            pCurrFromVertex = pCurrFromEdge->GetConnectedVertex(pCurrFromVertex);

         if (pCurrToEdge)
            pCurrToVertex = pCurrToEdge->GetConnectedVertex(pCurrToVertex);
      }
   }

   String strParameters;
   CMap<UINT, UINT, UINT, UINT> usedvertexidlist;
   usedvertexidlist.InitHashTable( 101 );

   for (POSITION pos = edgepairlist.GetHeadPosition(); pos; )
   {
      EdgePair &pEdgePair = edgepairlist.GetNext( pos );

      for (UINT i=0; i<2; i++)
      {
         HVertex *pFrom;
         HVertex *pTo;
         if ( i==0 ) // Use First Vertex
         {
            pFrom = pEdgePair.m_pFromEdge->GetFirstVertex();
            pTo = pEdgePair.m_bReversed ? pEdgePair.m_pToEdge->GetSecondVertex() : pEdgePair.m_pToEdge->GetFirstVertex();
         }
         else
         {
            pFrom = pEdgePair.m_pFromEdge->GetSecondVertex();
            pTo = pEdgePair.m_bReversed ? pEdgePair.m_pToEdge->GetFirstVertex() : pEdgePair.m_pToEdge->GetSecondVertex();
         }

         UINT nUnused;
         if ( usedvertexidlist.Lookup( pFrom->GetID(), nUnused ))
            continue;

         if ( usedvertexidlist.Lookup( pTo->GetID(), nUnused ))
            continue;

         usedvertexidlist.SetAt( pFrom->GetID(), pFrom->GetID() );
         usedvertexidlist.SetAt( pTo->GetID(), pTo->GetID() );

         AddToParameters( pMI, pFrom, pTo, strParameters );
      }
   }

   if ( !strParameters.IsEmpty() )
   {
      strParameters = (String)"-nf " + strParameters;
      StringList strResults;
      pMI->DoCommand( GetTranslateManipulatorPlugin()->m_pAttachCommandPlugin, strParameters, strResults );
   }

   return TRUE;
}