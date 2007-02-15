//

#include "stdafx.h"
#include "RotateFocusCommand.h"
#include "HSnipeObjectList.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HVertex.h"
#include "HEdge.h"
#include "HPoly.h"
#include "../resource.h"

CommandInstance *RotateFocusCommandPlugin::CreateCommandInstance() { return SNEW RotateFocusCommand(this); }

const char *RotateFocusCommandPlugin::GetCLIName() const { return "RotateFocus"; }; // Name used in scripts and commandline
const char *RotateFocusCommandPlugin::GetTitleName() const { return "Rotate Focus"; }

UINT RotateFocusCommandPlugin::GetBitmapID() const { return IDB_ROTATEFOCUSNEXT; }

String RotateFocusCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Rotate the near object around the focus object.\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void RotateFocusCommand::OnParse( const String &strParameters )
{
   m_pPrevFocusObject = HSelectionList::GetFocusObject();
   m_pPrevNearestFocusObject = HSelectionList::GetNearestFocusObject();

   if ( m_pPrevFocusObject && m_pPrevNearestFocusObject )
      CommandInstance::OnParse( strParameters );
   else
      THROW_FATAL_ERROR("Unable to execute RotateFocus command, no active focus object or nearest object.");
}

BOOL RotateFocusCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-p" ) == 0)
   {
      m_bRotateTowardsPrevious = TRUE;
      return TRUE;
   }
   return FALSE;
}

UndoType RotateFocusCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL RotateFocusCommand::OnDo(StringList &strResults)
{
   if (m_pPrevFocusObject->IsKindOf(SOT_Vertex))
   {
      HVertex *pFocusVertex = (HVertex *)m_pPrevFocusObject;
      if (m_pPrevNearestFocusObject->IsKindOf(SOT_Edge))
      {
         HEdge *pNewEdge;
         if (m_bRotateTowardsPrevious)
            pNewEdge = pFocusVertex->GetPrevEdgeClockwise( (HEdge *) m_pPrevNearestFocusObject );
         else
            pNewEdge = pFocusVertex->GetNextEdgeClockwise( (HEdge *) m_pPrevNearestFocusObject );

         ASSERT(pNewEdge);

         // It is possible on a spline model that we need to change focus objects, because this edge has a new Knot
         if (pNewEdge->GetFirstVertex() != pFocusVertex && (pNewEdge->GetFirstVertex() == pFocusVertex))
            pFocusVertex = pNewEdge->GetFirstVertex();
         else if (pNewEdge->GetSecondVertex() != pFocusVertex && (pNewEdge->GetSecondVertex() == pFocusVertex))
            pFocusVertex = pNewEdge->GetSecondVertex();

         HSelectionList::SetFocusObject( pFocusVertex, pNewEdge );
      }
      else // Must be face
      {
         HPoly *pNewFace;
         if (m_bRotateTowardsPrevious)
            pNewFace = pFocusVertex->GetPrevFaceClockwise( (HPoly *) m_pPrevNearestFocusObject );
         else
            pNewFace = pFocusVertex->GetNextFaceClockwise( (HPoly *) m_pPrevNearestFocusObject );

         ASSERT(pNewFace);

         // It is possible on a spline model that we need to change focus objects, because this faces corner is a new Knot
         for (UINT i=0; i<pNewFace->GetNumSides(); i++)
         {
            HVertex *pVertex = pNewFace->GetVertexAt(i);
            if (pVertex == pFocusVertex)
               break;
            if (pVertex == pFocusVertex)
            {
               pFocusVertex = pVertex;
               break;
            }
         }
         HSelectionList::SetFocusObject( pFocusVertex, pNewFace );
      }
   }
   else if (m_pPrevFocusObject->IsKindOf(SOT_Edge))
   {
   }
   else if (m_pPrevFocusObject->IsKindOf(SOT_Poly))
   {
   }
   else
      ASSERT(FALSE);

   return TRUE;
}

void RotateFocusCommand::OnUndo()
{
   HSelectionList::SetFocusObject(m_pPrevFocusObject, m_pPrevNearestFocusObject);
}