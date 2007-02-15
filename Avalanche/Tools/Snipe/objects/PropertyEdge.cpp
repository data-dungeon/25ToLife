#include "stdafx.h"
#include "PropertyEdge.h"
#include "PropertyPoly.h"
#include "ComponentList.h"

IMPLEMENT_COMPONENT_CREATE( PropertyEdge, Edge )

void PropertyEdge::Init()
{
   m_nPropertyEdgeFlags = 0;
   m_propertypolys[0] = NULL;
   m_propertypolys[1] = NULL;
   m_PropertyVertices[0] = NULL;
   m_PropertyVertices[1] = NULL;
   m_pNextPropertyEdge = NULL;
}

PropertyEdge::PropertyEdge()
{
   Init();
}

PropertyEdge::PropertyEdge( UINT nID )
: Component( nID )
{
   Init();
}

PropertyEdge::~PropertyEdge()
{
   SetFirstPropertyVertex(NULL);
   SetSecondPropertyVertex(NULL);
}

PropertySet *PropertyEdge::GetPropertySet()
{
   if (GetList())
      return (PropertySet*)GetList()->GetOwner();
   return NULL;
}

BOOL PropertyEdge::IsUsed()
{
   return GetFirstPropertyPoly() || GetSecondPropertyPoly();
}

void PropertyEdge::Disconnect()
{
   if (GetSecondPropertyVertex())
      GetSecondPropertyVertex()->RemoveSurroundingPropertyEdge( this );

   if (GetFirstPropertyVertex())
      GetFirstPropertyVertex()->RemoveSurroundingPropertyEdge( this );

   if (GetEdge())
      GetEdge()->RemovePropertyEdge( this );

   Component::Disconnect();
}

void PropertyEdge::Reconnect()
{
   Component::Reconnect();

   if (GetEdge())
      GetEdge()->AddPropertyEdge( this );

   if (GetFirstPropertyVertex())
      GetFirstPropertyVertex()->AddSurroundingPropertyEdge( this );

   if (GetSecondPropertyVertex())
      GetSecondPropertyVertex()->AddSurroundingPropertyEdge(this);

}

PropertyEdge *PropertyEdge::FindEdge( const PropertyVertex *pPropertyVertex0, const PropertyVertex *pPropertyVertex1)
{
   // uses surrounding edges
   for (UINT i = 0; i < pPropertyVertex0->GetNumSurroundingPropertyEdges(); i++)
   {
      PropertyEdge *pPropertyEdge = pPropertyVertex0->GetSurroundingPropertyEdgeAt(i);
      if ( pPropertyEdge->GetFirstPropertyVertex() == pPropertyVertex1 || pPropertyEdge->GetSecondPropertyVertex() == pPropertyVertex1)
         return pPropertyEdge;
   }
   return NULL;
}

BOOL PropertyEdge::AddPropertyPoly( PropertyPoly *pPropertyPoly)
{
   if (m_propertypolys[0] && m_propertypolys[1])
   {
//      ASSERT(FALSE);
      return FALSE;//already full
   }

   if (!m_bInList)
      ReInsertIntoList();

   if ((!m_propertypolys[0] && m_propertypolys[1] && m_propertypolys[1]!=pPropertyPoly)
    || (!m_propertypolys[0] && !m_propertypolys[1]))
      SetFirstPropertyPoly(pPropertyPoly);

   if (!m_propertypolys[1] && m_propertypolys[0] && m_propertypolys[0]!=pPropertyPoly)
      SetSecondPropertyPoly(pPropertyPoly);

   return TRUE;
}

void PropertyEdge::RemovePropertyPoly(PropertyPoly *pPropertyPoly)
{
   if (GetFirstPropertyPoly() == pPropertyPoly)
      SetFirstPropertyPoly( NULL );
   else if (GetSecondPropertyPoly() == pPropertyPoly)
      SetSecondPropertyPoly( NULL );
   else
      return;

   if (!IsUsed())
      RemoveFromList();
}

void PropertyEdge::Reverse()
{
   PropertyVertex *pTemp = m_PropertyVertices[0];
   m_PropertyVertices[0] = m_PropertyVertices[1];
   m_PropertyVertices[1] = pTemp;
}

void PropertyEdge::SetDirty()
{
}

void PropertyEdge::SetFirstPropertyVertex( PropertyVertex *pPropertyVertex )
{
   if (GetFirstPropertyVertex())
   {
      GetFirstPropertyVertex()->RemoveSurroundingPropertyEdge( this );
      GetFirstPropertyVertex()->DeReference();
   }

   m_PropertyVertices[0] = pPropertyVertex;

   if (pPropertyVertex)
   {
      pPropertyVertex->Reference();
      pPropertyVertex->AddSurroundingPropertyEdge(this);
   }
}

void PropertyEdge::SetSecondPropertyVertex( PropertyVertex *pPropertyVertex )
{
   if (GetSecondPropertyVertex())
   {
      GetSecondPropertyVertex()->RemoveSurroundingPropertyEdge( this );
      GetSecondPropertyVertex()->DeReference();
   }

   m_PropertyVertices[1] = pPropertyVertex;

   if (pPropertyVertex)
   {
      pPropertyVertex->Reference();
      pPropertyVertex->AddSurroundingPropertyEdge(this);
   }
}

BOOL PropertyEdge::IsAligned( PropertyPoly *pPropertyPoly ) const
{
   int nIndex = pPropertyPoly->FindPropertyVertexFaceRelativeIndex( this );
   ASSERT(nIndex != -1);

   PropertyVertex *pPropertyVertex0 = pPropertyPoly->GetPropertyVertexAt( nIndex );
   PropertyVertex *pPropertyVertex1 = pPropertyPoly->GetPropertyVertexAt( pPropertyPoly->GetNextIndex( nIndex) );

   return GetFirstPropertyVertex() == pPropertyVertex0 && GetSecondPropertyVertex() == pPropertyVertex1;
}

void PropertyEdge::Draw( BOOL bDrawTess )
{
}
