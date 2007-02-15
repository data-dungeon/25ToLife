#include "stdafx.h"
#include "PropertyComponent.h"
#include "PropertyPoly.h"
#include "..\Utility\ArrayTemplate.h"

IMPLEMENT_COMPONENT_CREATE( PropertyComponent, Component )

PropertyComponent::PropertyComponent()
{
   m_nNumSurroundingPropertyFaces = 0;
   m_nNumMaxSurroundingPropertyFaces = 0;
   m_nNumSurroundingPropertyEdges = 0;
   m_nNumMaxSurroundingPropertyEdges = 0;
   m_pSurroundingPropertyPolys = NULL;
   m_pSurroundingPropertyEdges = NULL;
}

PropertyComponent::PropertyComponent( UINT nID )
: Component( nID )
{
}

PropertyComponent::~PropertyComponent()
{
   if (m_pSurroundingPropertyPolys)
   {
      DeconstructArray<PropertyPoly*>(m_pSurroundingPropertyPolys,m_nNumSurroundingPropertyFaces);
      delete[] (BYTE*)m_pSurroundingPropertyPolys;
   }
   if (m_pSurroundingPropertyEdges)
   {
      DeconstructArray<PropertyEdge*>(m_pSurroundingPropertyEdges,m_nNumSurroundingPropertyEdges);
      delete[] (BYTE*)m_pSurroundingPropertyEdges;
   }
}

PropertySet *PropertyComponent::GetPropertySet()
{
   SnipeObject *pOwner = GetOwner();
   return (PropertySet *)pOwner;
}

void PropertyComponent::SetDirty()
{
   for (UINT i = 0; i < GetNumSurroundingPropertyEdges(); i++) // Shouldn't this be in a PatchPropertyComponent class? like the PatchVertex?  This is for tangent dirting which is not needed in PolyModels
   {
      PropertyEdge *pPropertyEdge = GetSurroundingPropertyEdgeAt(i);

      if ( pPropertyEdge->GetFirstPropertyComponent() )
         pPropertyEdge->GetFirstPropertyComponent()->DirtyDependentEdges();

      if ( pPropertyEdge->GetSecondPropertyComponent() )
         pPropertyEdge->GetSecondPropertyComponent()->DirtyDependentEdges();
   }
}

void PropertyComponent::DirtyDependentEdges()
{
   for (UINT i = 0; i < GetNumSurroundingPropertyEdges(); i++)
      GetSurroundingPropertyEdgeAt(i)->SetDirty();
}

BOOL PropertyComponent::IsUsed()
{
   return GetNumSurroundingPropertyEdges();
}

void PropertyComponent::Disconnect()
{
   Component::Disconnect();
}

void PropertyComponent::Reconnect()
{
   Component::Reconnect();
}

void PropertyComponent::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   Component::Save( bs );
}

BOOL PropertyComponent::Load( BaseStream &bs )
{
   if (!Component::Load( bs ))
      return FALSE;

   return TRUE;
}

void PropertyComponent::AddSurroundingPropertyEdge( PropertyEdge *pPropertyEdge )
{
   UINT nIndex = m_nNumSurroundingPropertyEdges;
   UINT nSize = m_nNumSurroundingPropertyEdges;
   UINT nMaxSize = m_nNumMaxSurroundingPropertyEdges;
   SetArraySize<PropertyEdge*>(&m_pSurroundingPropertyEdges,nSize+1,nSize,nMaxSize,4);
   ASSERT(nMaxSize<256);
   m_nNumSurroundingPropertyEdges = nSize;
   m_nNumMaxSurroundingPropertyEdges = nMaxSize;
   m_pSurroundingPropertyEdges[nIndex] = pPropertyEdge;
   if (!m_bInList)
      ReInsertIntoList();
   SetDirty();
}

PropertyEdge *PropertyComponent::GetSurroundingPropertyEdgeAt( UINT nIndex ) const
{
   return m_pSurroundingPropertyEdges[nIndex];
}

void PropertyComponent::RemoveSurroundingPropertyEdgeAt( UINT nIndex )
{
   UINT nSize = m_nNumSurroundingPropertyEdges;
   RemoveArrayElements<PropertyEdge*>(m_pSurroundingPropertyEdges,nIndex,nSize);
   m_nNumSurroundingPropertyEdges = nSize;
   if (!IsUsed())
      RemoveFromList();
}

void PropertyComponent::RemoveSurroundingPropertyEdge(PropertyEdge *pPropertyEdge)
{
   for (UINT i = 0; i < GetNumSurroundingPropertyEdges(); i++)
   {
      if (GetSurroundingPropertyEdgeAt(i)==pPropertyEdge)
      {
         RemoveSurroundingPropertyEdgeAt(i);
         return;
      }
   }
}

void PropertyComponent::AddSurroundingPropertyPoly( PropertyPoly *pPropertyPoly )
{
   UINT nIndex = m_nNumSurroundingPropertyFaces;
   UINT nSize = m_nNumSurroundingPropertyFaces;
   UINT nMaxSize = m_nNumMaxSurroundingPropertyFaces;
   SetArraySize<PropertyPoly*>(&m_pSurroundingPropertyPolys,nSize+1,nSize,nMaxSize,4);
   ASSERT(nMaxSize<256);
   m_nNumSurroundingPropertyFaces = nSize;
   m_nNumMaxSurroundingPropertyFaces = nMaxSize;
   m_pSurroundingPropertyPolys[nIndex] = pPropertyPoly;
}

PropertyPoly *PropertyComponent::GetSurroundingPropertyPolyAt( UINT nIndex ) const
{
   return m_pSurroundingPropertyPolys[nIndex];
}

void PropertyComponent::RemoveSurroundingPropertyPolyAt( UINT nIndex )
{
   UINT nSize = m_nNumSurroundingPropertyFaces;
   RemoveArrayElements<PropertyPoly*>(m_pSurroundingPropertyPolys,nIndex,nSize);
   m_nNumSurroundingPropertyFaces = nSize;
}

void PropertyComponent::RemoveSurroundingPropertyPoly( PropertyPoly *pPropertyPoly )
{
   for (UINT i = 0; i < GetNumSurroundingPropertyPolys(); i++)
   {
      if (GetSurroundingPropertyPolyAt(i)==pPropertyPoly)
      {
         RemoveSurroundingPropertyPolyAt(i);
         return;
      }
   }
}

void PropertyComponent::Compact()
{
   UINT nMaxSize = m_nNumMaxSurroundingPropertyFaces;
   FreeArrayExtra<PropertyPoly*>(&m_pSurroundingPropertyPolys,m_nNumSurroundingPropertyFaces,nMaxSize);
   m_nNumMaxSurroundingPropertyFaces = nMaxSize;

   nMaxSize = m_nNumMaxSurroundingPropertyEdges;
   FreeArrayExtra<PropertyEdge*>(&m_pSurroundingPropertyEdges,m_nNumSurroundingPropertyEdges,nMaxSize);
   m_nNumMaxSurroundingPropertyEdges = nMaxSize;
}

void PropertyComponent::GetEdgePair( PropertyPoly *pPoly, PropertyEdge **pEdge0, PropertyEdge **pEdge1, UINT *nIndex0/*=NULL*/, UINT *nIndex1/*=NULL*/)
{
   int nFaceRelativeIndex = pPoly->FindPropertyComponentFaceRelativeIndex(this);
   ASSERT(nFaceRelativeIndex>=0);

   UINT nIndexPrev, nIndexNext;
   if (nFaceRelativeIndex-1 <0)
      nIndexPrev = pPoly->GetNumSides()-1;
   else
      nIndexPrev = nFaceRelativeIndex-1;

   if (nFaceRelativeIndex+1 >= (int)pPoly->GetNumSides())
      nIndexNext = nFaceRelativeIndex;
   else
      nIndexNext = nFaceRelativeIndex;

   if (nIndex0)
      *nIndex0 = nIndexPrev;
   if (nIndex1)
      *nIndex1 = nIndexNext;

   *pEdge0 = pPoly->GetPropertyEdgeAt(nIndexPrev);
   *pEdge1 = pPoly->GetPropertyEdgeAt(nIndexNext);
}
