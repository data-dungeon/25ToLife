#include "stdafx.h"
#include "PropertyPatchEdge.h"
#include "../Utility/MathTools.h"
#include "PatchEdge.h"
#include "PropertyPatch.h"
#include "PropertySet.h"

IMPLEMENT_COMPONENT_CREATE( PropertyPatchEdge, PropertyEdge )

PropertyPatchEdge::~PropertyPatchEdge()
{
   if (m_pFirstPropertyVertexHandle)
      delete[]m_pFirstPropertyVertexHandle;
   if (m_pSecondPropertyVertexHandle)
      delete[]m_pSecondPropertyVertexHandle;
}

void PropertyPatchEdge::ClearTessellation()
{
   m_tessPropertyVertices.Clear();
}

void PropertyPatchEdge::AllocateTessellation()
{
   int nSub = GetEdge()->CalcEdgeSubdivisions();
   int nNumSteps = 1 << nSub;
   nNumSteps += 1;

   if (GetNumTessPropertyVertices() == nNumSteps)
      return;

   SetNumTessPropertyVertices( nNumSteps );

   for (int i=0; i<nNumSteps; i++)
   {
      PropertyVertex *pPropertyVertex;
      if (i==0) // steal first uv
         pPropertyVertex = GetFirstPropertyVertex();
      else if (i==nNumSteps-1)// steal last uv
         pPropertyVertex = GetSecondPropertyVertex();
      else // create
         pPropertyVertex = GetPropertySet()->AddTessellatedPropertyVertex();
 
      ASSERT(pPropertyVertex);
      SetTessPropertyVertexAt( i, pPropertyVertex );
   }
}

BOOL PropertyPatchEdge::AddPropertyPoly( PropertyPoly *pPropertyPoly)
{
   BOOL bResult = PropertyEdge::AddPropertyPoly( pPropertyPoly );
   SetTangentsDirty( TRUE );
   return bResult;
}

void PropertyPatchEdge::Disconnect()
{
   PropertyEdge::Disconnect();
   SetTessellationDirty( TRUE );
}

void PropertyPatchEdge::Reverse()
{
   ClearTessellation();
   PropertyEdge::Reverse();
   SetTessellationDirty( TRUE );
}

void PropertyPatchEdge::SetDirty()
{
   PropertyEdge::SetDirty();
   SetTangentsDirty( TRUE );
}

void PropertyPatchEdge::SetTangentsDirty( BOOL bDirty )
{
   m_bTangentsDirty = bDirty;

   SetTessellationDirty( bDirty );

   PropertyPatch *pPropertyVertexPatch = GetPropertyPatch();
   if (!pPropertyVertexPatch)
      return;
   pPropertyVertexPatch->SetTangentsDirty( bDirty );

   pPropertyVertexPatch = GetConnectedPropertyPatch( pPropertyVertexPatch );
   if (pPropertyVertexPatch)
      pPropertyVertexPatch->SetTangentsDirty( bDirty );
}

/*
void PropertyPatchEdge::Draw( BOOL bDrawTess )
{

   if (!bDrawTess)
   {
      PropertyEdge::Draw( bDrawTess );
      return;
   }

   glPushAttrib(GL_CURRENT_BIT);
   glVertex3f(0.0f,1.0f,0.0f);
   Vector tangent;
   glBegin( GL_POINTS );
      tangent = this->GetFirstUVTangentPos();
      glVertex2fv( &tangent.x );
      tangent = this->GetSecondUVTangentPos();
      glVertex2fv( &tangent.x );
   glEnd();
   glPopAttrib();

   UINT nNumVerts = GetNumTessPropertyVertices();
   if (nNumVerts==0)
      return;

   glBegin(GL_LINES);

   for (UINT i = 0; i < nNumVerts-1; i++)
   {
      PropertyVertex *pPropertyVertex = GetTessPropertyVertexAt(i);
      ASSERT(pPropertyVertex);
      glVertex3fv( &pPropertyVertex->GetPos().x );
      pPropertyVertex = GetTessPropertyVertexAt(i+1);
      ASSERT(pPropertyVertex);
      glVertex3fv( &pPropertyVertex->GetPos().x );
   }

   glEnd();
}
*/

void PropertyPatchEdge::SetFirstPropertyVertex(PropertyVertex *pPropertyVertex)
{
   ClearTessellation();
   SetTessellationDirty( TRUE );
   PropertyEdge::SetFirstPropertyVertex( pPropertyVertex );
}

void PropertyPatchEdge::SetSecondPropertyVertex(PropertyVertex *pPropertyVertex)
{
   ClearTessellation();
   SetTessellationDirty( TRUE );
   PropertyEdge::SetSecondPropertyVertex( pPropertyVertex );
}

PropertyPatchEdge *PropertyPatchEdge::GetPrevContPropertyEdge(BOOL bLoop)
{
   PatchEdge *pPatchEdge = GetEdge()->GetPrevContEdge(bLoop);
   if (!pPatchEdge)
      return NULL;
   PropertyVertex *pPrevContPropertyVertex = GetPrevContPropertyVertex();
   PropertyEdge *pPropertyEdge = pPatchEdge->GetHeadPropertyEdge();
   while (pPropertyEdge)
   {
      if (pPropertyEdge->GetPropertySet() == GetPropertySet())
      {
         for (UINT i = 0; i < pPrevContPropertyVertex->GetNumSurroundingPropertyEdges(); i++)
         {
            if (pPropertyEdge == pPrevContPropertyVertex->GetSurroundingPropertyEdgeAt(i))
               return (PropertyPatchEdge*)pPropertyEdge;
         }
      }
      pPropertyEdge = pPropertyEdge->GetNextPropertyEdge();
   }
   return NULL;
}

PropertyPatchEdge *PropertyPatchEdge::GetNextContPropertyEdge(BOOL bLoop)
{
   PatchEdge *pPatchEdge = GetEdge()->GetNextContEdge(bLoop);
   if (!pPatchEdge)
      return NULL;
   PropertyVertex *pNextContPropertyVertex = GetNextContPropertyVertex();
   PropertyEdge *pPropertyEdge = pPatchEdge->GetHeadPropertyEdge();
   while (pPropertyEdge)
   {
      if (pPropertyEdge->GetPropertySet() == GetPropertySet())
      {
         for (UINT i = 0; i < pNextContPropertyVertex->GetNumSurroundingPropertyEdges(); i++)
         {
            if (pPropertyEdge == pNextContPropertyVertex->GetSurroundingPropertyEdgeAt(i))
             return (PropertyPatchEdge*)pPropertyEdge;
         }
      }
      pPropertyEdge = pPropertyEdge->GetNextPropertyEdge();
   }
   return NULL;
}

UINT PropertyPatchEdge::GetNumTessPropertyVertices()
{
   GetEdge()->Tessellate();
   return m_tessPropertyVertices.GetNumPropertyVertices();
}

PropertyVertex *PropertyPatchEdge::GetTessPropertyVertexAt( UINT nIndex, BOOL bReversed/*=FALSE*/ )
{
   GetEdge()->Tessellate();
   return m_tessPropertyVertices.GetPropertyVertexAt( nIndex, bReversed );
}

TessPropertyEdge *PropertyPatchEdge::GetTessPropertyEdge() 
{ 
   GetEdge()->Tessellate(); 
   return &m_tessPropertyVertices; 
}

PropertyVertex *PropertyPatchEdge::GetPrevContPropertyVertex()
{
   PatchVertex *pPrevPropertyVertex = GetEdge()->GetPrevContVertex();
   if (pPrevPropertyVertex == GetEdge()->GetFirstVertex())
   {
      return GetFirstPropertyVertex();
   }
   return GetSecondPropertyVertex();
}

PropertyVertex *PropertyPatchEdge::GetNextContPropertyVertex()
{
   PatchVertex *pNextPropertyVertex = GetEdge()->GetNextContVertex();
   if (pNextPropertyVertex == GetEdge()->GetFirstVertex())
   {
      return GetFirstPropertyVertex();
   }
   return GetSecondPropertyVertex();
}

