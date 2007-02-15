#include "stdafx.h"
#include "PropertyPoly.h"
#include "Poly.h"
#include "PolyModel.h"
#include "ComponentList.h"
#include "PolyModel.h"
#include "PropertySet.h"

IMPLEMENT_COMPONENT_CREATE( PropertyPoly, "Face" )

PropertyPoly::~PropertyPoly()
{
   if (m_pPropertyVertices)
   {
      delete [] m_pPropertyVertices;

      for (UINT i=0; i<GetNumSides(); i++)
      {
         if (m_pPropertyEdges[i]) // don't use GetPropertyPolyAt because it will build them for no reason
            m_pPropertyEdges[i]->DeReference( TRUE );
      }

      ASSERT( m_pPropertyEdges );
      delete [] m_pPropertyEdges;
   }
}

UINT PropertyPoly::GetNumSides() const
{
   ASSERT(m_pPoly);
   return m_pPoly->GetNumSides();
}

PropertySet *PropertyPoly::GetPropertySet()
{
   if (GetList())
      return (PropertySet*)GetList()->GetOwner();
   return NULL;
}

BOOL PropertyPoly::IsUsed()
{
   return m_pPoly != NULL;
}

void PropertyPoly::Disconnect()
{
   if (m_pPoly)
   {
      for (int i=GetNumSides()-1; i>=0; i--)
      {
         if (GetPropertyVertexAt(i))
            GetPropertyVertexAt(i)->RemoveSurroundingPropertyPoly( this );
      }

      for (int i=GetNumSides()-1; i>=0; i--)
      {
         if (m_pPropertyEdges[i]) // don't use GetPropertyPolyAt because it will build them for no reason
            m_pPropertyEdges[i]->RemovePropertyPoly( this );
      }
   }

   Component::Disconnect();
}

void PropertyPoly::Reconnect()
{
   Component::Reconnect();

   for (int i=GetNumSides()-1; i>=0; i--)
      GetPropertyVertexAt(i)->AddSurroundingPropertyPoly( this );

   for (UINT i=0; i<GetNumSides(); i++)
      GetPropertyEdgeAt(i)->AddPropertyPoly( this );
}

void PropertyPoly::SetPropertyVertexAt( const UINT nIndex, PropertyVertex *pPropertyVertex )
{
   ASSERT( nIndex < GetNumSides() );

   if (m_pPropertyVertices[nIndex])
   {
      if (m_pPropertyVertices[nIndex] == pPropertyVertex)
         return;

      m_pPropertyVertices[nIndex]->RemoveSurroundingPropertyPoly( this );
   }

   m_pPropertyVertices[nIndex] = pPropertyVertex;

   // PropertyPolys don't reference PropertyVertices, only PropertyEdges do since they are the ones in charge of destroying them
   if (pPropertyVertex)
      pPropertyVertex->AddSurroundingPropertyPoly( this );
}

inline void PropertyPoly::SetPropertyEdgeAt( UINT nIndex, PropertyEdge *pPropertyEdge )
{
   ASSERT( nIndex < GetNumSides() );

   if (m_pPropertyEdges[nIndex])
   {
      if (m_pPropertyEdges[nIndex] == pPropertyEdge)
         return;

      m_pPropertyEdges[nIndex]->RemovePropertyPoly( this );
      m_pPropertyEdges[nIndex]->DeReference();
   }

   if (pPropertyEdge)
   {
      BOOL bResult = pPropertyEdge->AddPropertyPoly( this );
//      ASSERT(bResult);

      if (bResult)
         pPropertyEdge->Reference();
      else
      {
         ASSERT(FALSE);
      }
   }

   m_pPropertyEdges[nIndex] = pPropertyEdge;
}

void PropertyPoly::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   Component::Save( bs );

   bs << sp << GetPoly()->GetID();

   for (UINT nIndex=0; nIndex<GetPoly()->GetNumSides(); nIndex++)
   {
      PropertyVertex *pPropertyVertex = GetPropertyVertexAt( nIndex );
      bs << sp << pPropertyVertex->GetID();
   }

   bs << nl;
}

BOOL PropertyPoly::Load( BaseStream &bs )
{
   if (!Component::Load( bs ))
      return FALSE;

   PropertySet *pPropertySet = GetPropertySet();
   PolyModel *pPolyModel = (PolyModel *)pPropertySet->GetParent()->GetParent();

   UINT nPolyID;
   bs >> nPolyID;
   Poly *pPoly = pPolyModel->FindPoly( nPolyID );
   if (!pPoly)
      return FALSE;

   pPoly->AddPropertyPoly( this );

   UINT nNumPropertyVertices = pPoly->GetNumSides();
   for (UINT nIndex=0; nIndex<nNumPropertyVertices; nIndex++)
   {
      UINT nPropertyVertexID;
      bs >> nPropertyVertexID;
      PropertyVertex *pPropertyVertex = pPropertySet->FindPropertyVertex( nPropertyVertexID );
      SetPropertyVertexAt( nIndex, pPropertyVertex );
   }

   return TRUE;
}

PropertyEdge *PropertyPoly::GetPropertyEdgeAt( UINT nIndex ) const
{
   ASSERT(nIndex < GetNumSides());
//   ASSERT(m_pPropertyEdges[nIndex]);
   return m_pPropertyEdges[nIndex];
}

UINT PropertyPoly::GetNextIndex( UINT nIndex ) const
{
   nIndex++;
   if (nIndex >= GetNumSides())
      nIndex = 0;
   return nIndex;
}

UINT PropertyPoly::GetPrevIndex( UINT nIndex ) const
{
   if (nIndex == 0)
      nIndex = GetNumSides();
   nIndex--;
   return nIndex;
}

// uses face neighbors, assumes pPropertyVertex0 and pPropertyVertex1 are aligned with this
PropertyPoly *PropertyPoly::FindConnectedPropertyPoly(PropertyVertex *pPropertyVertex0, PropertyVertex *pPropertyVertex1)
{
   UINT nIndex;
   PropertyPoly *pSurroundingPropertyPoly;
   for (UINT i = 0; i < pPropertyVertex0->GetNumSurroundingPropertyPolys(); i++)
   {
      pSurroundingPropertyPoly = pPropertyVertex0->GetSurroundingPropertyPolyAt(i);
      if (pSurroundingPropertyPoly==this)
         continue;
      nIndex = pSurroundingPropertyPoly->FindPropertyVertexFaceRelativeIndex(pPropertyVertex1);
      PropertyVertex *pNextPropertyVertex = pSurroundingPropertyPoly->GetPropertyVertexAt(pSurroundingPropertyPoly->GetNextIndex(nIndex)); // TOny I added pSurroundingPropertyPoly to the GetNextIndex(nIndex) I think that is what you wanted
      if (pPropertyVertex0 == pNextPropertyVertex)
         return pSurroundingPropertyPoly;
   }
   return NULL;
}

void PropertyPoly::BuildPropertyEdges()
{
   PropertySet *pPropertySet = (PropertySet*)GetOwner();

   Poly *pPoly = GetPoly();
   UINT nNumSides = pPoly->GetNumSides();

   for (UINT i = 0; i < nNumSides; i++)
   {
      ASSERT(GetPropertyEdgeAt(i) == NULL);

      Edge *pEdge = pPoly->GetEdgeAt(i);

      PropertyVertex *pPropertyVertex0, *pPropertyVertex1;
      if (pEdge->IsAligned(pPoly))
      {
         pPropertyVertex0 = m_pPropertyVertices[i];
         pPropertyVertex1 = m_pPropertyVertices[GetNextIndex(i)];
      }
      else
      {
         pPropertyVertex0 = m_pPropertyVertices[GetNextIndex(i)];
         pPropertyVertex1 = m_pPropertyVertices[i];
      }
      ASSERT(pPropertyVertex0 && pPropertyVertex1);

      PropertyEdge *pPropertyEdge = pPropertySet->AddPropertyEdge( this, pPropertyVertex0, pPropertyVertex1, pEdge );
      SetPropertyEdgeAt( i, pPropertyEdge );
   }
}

void PropertyPoly::ReverseWinding()
{
   UINT halfnumsides = GetNumSides() / 2;
   for (UINT i=1; i<halfnumsides; i++)
   {
      UINT nNewIndex = GetNumSides() - i;
      SwapPointers( (void *&)m_pPropertyVertices[i], (void *&)m_pPropertyVertices[nNewIndex] );
   }
   
   for (UINT i=0; i<halfnumsides; i++)
   {
      UINT nNewIndex = GetNumSides() - i - 1;
      SwapPointers( (void *&)m_pPropertyEdges[i], (void *&)m_pPropertyEdges[nNewIndex] );
   }
}