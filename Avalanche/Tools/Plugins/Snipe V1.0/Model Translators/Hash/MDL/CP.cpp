//

#include "stdafx.h"
#include "CP.h"
#include "HPoly.h"

CP::CP( UINT nID )
{
   Init( nID );
}

CP::CP( HVertex *pVertex )
{
   Init( pVertex->GetID() );

   m_pVertex = pVertex;
}

void CP::Init( UINT nID )
{
   m_nID = nID;

   m_pVertex = NULL;
   m_pNextEdge = NULL;
   m_pPrevEdge = NULL;

   m_pParentCP = NULL;
   m_pChildCP = NULL;
   m_pNextInSpline = NULL;
   m_pPrevInSpline = NULL;

   m_fInAlpha = m_fOutAlpha = 0.0f;
   m_fInGamma = m_fOutGamma = 0.0f;
   m_fInMagnitude = m_fOutMagnitude = 1.0f;

   m_bSmooth = TRUE;
   m_bLoop = FALSE;
   m_bConstrained = FALSE;
}

BOOL CP::IsConstrained() const
{
   return m_bConstrained;
}

CP *CP::DrillForTop()
{
   if (!this)
      return NULL;

   for (CP *pCP=this; pCP->GetParentCP(); pCP=pCP->GetParentCP())
      ;

   return pCP;
}

CP *CP::DrillForBottom()
{
   if (!this)
      return NULL;

   for (CP *pCP=this; pCP->GetChildCP(); pCP=pCP->GetChildCP())
      ;

   return pCP;
}

BOOL CP::DrillForCP( CP *pCP ) const
{
   for (const CP *pCP2=this; pCP2; pCP2=pCP2->GetChildCP())
   {
      if (pCP2 == pCP)
         return TRUE;
   }

   for (pCP2=GetParentCP(); pCP2; pCP2=pCP2->GetParentCP())
   {
      if (pCP2 == pCP)
         return TRUE;
   }

   return FALSE;
}

CP *CP::DrillForConstrained()
{
   for (CP *pCP=this; pCP; pCP=pCP->GetChildCP())
   {
      if (pCP->IsConstrained())
         return pCP;
   }

   for (pCP=GetParentCP(); pCP; pCP=pCP->GetParentCP())
   {
      if (pCP->IsConstrained())
         return pCP;
   }

   return NULL;
}

void CP::InsertCPAfter( CP *pInsert )
{
   if (pInsert->IsLoop())
   {
      ASSERT(!pInsert->m_pNextInSpline);
      for (CP *pCP=this; pCP && pCP->PreviousInSpline(FALSE); pCP=pCP->PreviousInSpline(FALSE))
         ;
      pInsert->m_pNextInSpline = pCP;
      pCP->SetPreviousInSpline( pInsert );
   }

   if (IsLoop())
   {
      pInsert->SetLoop( TRUE );
      SetLoop( FALSE );
   }

   if (!pInsert->m_pNextInSpline && m_pNextInSpline)
   {
      pInsert->m_pNextInSpline = m_pNextInSpline;
      m_pNextInSpline->SetPreviousInSpline( pInsert );
   }
   m_pNextInSpline = pInsert;

   pInsert->SetPreviousInSpline( this );

   //SetDirty();
   //pInsert->SetDirty();
}

void CP::MoveToStackWithChildren( CP *pAttachTo )
{
   ASSERT( pAttachTo );

   if (GetParentCP())
   {
      // remove ourselves from our parent
      GetParentCP()->SetChildCP( NULL );
   }

   // set every CP in our stack's vertex data to be the same as the one we are attaching to
   for (CP *pCP=this; pCP; pCP=pCP->GetChildCP())
      pCP->SetVertex( pAttachTo->GetVertex() );

   // Hook us in as a child of the bottom most CP
   CP *pBottomCP = pAttachTo->DrillForBottom();
   pBottomCP->SetChildCP( this );
   SetParentCP( pBottomCP );
}

void CP::MoveToStack( CP *pAttachTo )
{
   ASSERT( pAttachTo );

   if (GetParentCP())
      GetParentCP()->SetChildCP( GetChildCP() );

   if (GetChildCP())
      GetChildCP()->SetParentCP( GetParentCP() );

   SetVertex( pAttachTo->GetVertex() );

   // Hook us in as a child of the bottom most CP
   CP *pBottomCP = pAttachTo->DrillForBottom();
   pBottomCP->SetChildCP( this );
   SetParentCP( pBottomCP );
}

//static void MoveToStack( CP *pFrom, CP *pTo, CP *pToChild )
//{
//   CP *pOldStack = NULL;
//
//   // Unstitch us
//   if (pFrom->GetParentCP())
//   {
//      pOldStack = pFrom->GetParentCP();
//      pFrom->GetParentCP()->SetChildCP( pFrom->GetChildCP() );
//   }
//
//   if (pFrom->GetChildCP())
//   {
//      pOldStack = pFrom->GetChildCP();
//      pFrom->GetChildCP()->SetParentCP( pFrom->GetParentCP() );
//   }
//
//   // Stitch us into pAttachTo
//   if (pToChild)
//      pFrom->SetChildCP(pToChild);
//   else
//      pFrom->SetChildCP(NULL);
//
//   if (pFrom->GetChildCP())
//      pFrom->GetChildCP()->SetParentCP( pFrom );
//
//   pFrom->SetParentCP(pTo);
//
//   HVertex *pAttachToVertex;
//   if (pTo)
//   {
//      pTo->SetChildCP( pFrom );
//      pAttachToVertex = pTo->GetVertex();
//   }
//   else
//   {
//      if (pToChild)
//         pAttachToVertex = pToChild->GetVertex();
//      else
//         pAttachToVertex = pFrom->AllocVertexData();
//   }
//
//   pFrom->SetVertex( pAttachToVertex );
//}

CP *CP::DrillForTConstrained()
{
   CP *pConstrained = DrillForConstrained();
   if (pConstrained && pConstrained->IsTConstrained())
      return pConstrained;
   return NULL;
}

BOOL CP::IsTConstrainedCPInEdge( HPoly *pPoly, UINT nIndex )
{
   ASSERT( IsTConstrained() );

   CP *pPrevInSpline = PreviousInSpline(TRUE);
   CP *pNextInSpline = NextInSpline(TRUE);

   UINT nPrevPatchIndex = pPoly->GetPrevIndex( nIndex );
   HVertex *pPrevInPatch = pPoly->GetVertexAt(nPrevPatchIndex);

   UINT nNextPatchIndex = pPoly->GetNextIndex( nIndex );
   HVertex *pNextInPatch = pPoly->GetVertexAt(nNextPatchIndex);

   return (pNextInSpline->IsAttached(pNextInPatch) || pPrevInSpline->IsAttached(pNextInPatch))
      && (pNextInSpline->IsAttached(pPrevInPatch) || pPrevInSpline->IsAttached(pPrevInPatch));
}
