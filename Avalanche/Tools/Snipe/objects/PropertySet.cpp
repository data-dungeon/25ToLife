#include "stdafx.h"
#include "PropertyVertex.h"
#include "PropertySet.h"
#include "../gui/Snipe.h"
#include "../gui/GLRenderWnd.h"
#include "PropertyPoly.h"
#include "Poly.h"
#include "../GUI/GLTexCoordView.h"
#include "PatchModel.h"
#include "../interface/Implement.h"
#include "resource.h"

#include "PropertyVertex.h"
#include "PropertyEdge.h"
#include "PropertyPoly.h"
//#include "PropertyPatchComponent.h"
#include "PropertyPatchEdge.h"
#include "PropertyPatch.h"

/////////////////////////////////////////////////////
// PropertySetContainer

IMPLEMENT_SNIPEOBJECT_CREATE( PropertySetContainer )
PropertySetContainer::PropertySetContainer()
{
   m_strName = "Property Component Sets";
}

PropertySetContainer::~PropertySetContainer()
{
}

//CMap<PropertyVertex*,PropertyVertex*,BOOL,BOOL> PropertySet::m_memoryleakmap;

/////////////////////////////////////////////////////
// PropertySet

IMPLEMENT_SNIPEOBJECT_CREATE( PropertySet );
IMPLEMENT_COMMANDOBJECT( PropertySet );

PropertySet::PropertySet()
{
   m_propertypolys.SetOwner(this);
   m_propertyvertexlist.SetOwner(this);
   m_propertyedgelist.SetOwner(this);
}

PropertySet::~PropertySet()
{
//   int nCount = 0;
//   for (POSITION pos = m_memoryleakmap.GetStartPosition(); pos; )
//   {
//      BOOL bBool;
//      PropertyVertex *pPropertyVertex;
//      m_memoryleakmap.GetNextAssoc(pos,pPropertyVertex,bBool);
//      g_outputWindow << "Property Vertex Leak: " << pPropertyVertex->GetID() << ", refcount = " << pPropertyVertex->GetRefCount() << "\n";
//      nCount++;
//   }
//   g_outputWindow << "Count = " << nCount << "\n";
}

void PropertySet::PreDestruct()
{
   HierObject::PreDestruct();
}

PolyModel *PropertySet::GetModel() const
{
   for (SnipeObject *pObject = GetParent(); pObject; pObject = pObject->GetParent())
   {
      if (pObject->IsKindOf(SOT_PolyModel))
         return (PolyModel *)pObject;
   }

   return NULL;
}

PropertyVertex *PropertySet::AddPropertyVertex(UINT nID/*=-1*/)
{
   PropertyVertex *pPropertyVertex;
   if (GetModel()->IsKindOf(SOT_PatchModel))
      pPropertyVertex = CreatePropertyPatchComponent(nID);
   else
      pPropertyVertex = CreatePropertyVertex(nID);
   m_propertyvertexlist.Add( pPropertyVertex );
   return pPropertyVertex;
}

PropertyVertex *PropertySet::AddTessellatedPropertyVertex(UINT nID/*=-1*/)
{
   PropertyVertex *pPropertyVertex = AddPropertyVertex(nID);
   pPropertyVertex->SetVisible(FALSE);
   return pPropertyVertex;
}

PropertyVertex *PropertySet::AddTessellatedPropertyVertices( UINT nSize )
{
   return PropertyVertex::CreateArray(nSize);
}

void PropertySet::RemovePropertyVertex( PropertyVertex *pPropertyVertex )
{
   m_propertyvertexlist.Remove( pPropertyVertex );
}

PropertyPoly *PropertySet::AddPoly( Poly *pPoly )
{
   PropertyPoly *pPropertyPoly = pPoly->FindPropertyPoly( this );
  
   if (pPropertyPoly)
      return pPropertyPoly;

   // this might be better if we create an AddPatch()?
   if (pPoly->GetOwner()->GetSOT() == SOT_PatchModel)
      pPropertyPoly = CreatePropertyPatch();
   else
      pPropertyPoly = CreatePropertyPoly();

   m_propertypolys.Add( pPropertyPoly );
   pPoly->AddPropertyPoly( pPropertyPoly );
   return pPropertyPoly;
}

void PropertySet::RemovePoly( PropertyPoly *pPropertyPoly )
{
   pPropertyPoly->GetPoly()->RemovePropertyPoly( pPropertyPoly );
   m_propertypolys.Remove( pPropertyPoly );
}

// HierObject overrides
void PropertySet::CountPickObjectType( CGLBaseView *pBaseView, UINT &nCount )
{
   switch (pBaseView->m_nPickObjectType)
   {
   case SOT_PropertyVertex :
      nCount += m_propertyvertexlist.GetSize();
   }
}

SimpleComponentList *PropertySet::FindComponentList( const String &strComponentType )
{
   if (strComponentType == "property")
      return (SimpleComponentList *)&m_propertyvertexlist;
   return SnipeObject::FindComponentList(strComponentType);
}

Component *PropertySet::CreateComponent( SnipeObjectType nSOT )
{
   switch (nSOT)
   {
   case SOT_PropertyVertex:
      return PropertyVertex::Create();
   case SOT_PropertyPoly:
      return PropertyPoly::Create();
   }

   return HierObject::CreateComponent( nSOT );
}

void PropertySet::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   HierObject::Save( bs );
}

BOOL PropertySet::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   return HierObject::ParseData( bs, dataheader );
}

PropertyEdge *PropertySet::AddPropertyEdge( PropertyPoly *pPropertyPoly, PropertyVertex *pPropertyVertex0, PropertyVertex *pPropertyVertex1, Edge *pEdge, UINT nID/*=-1*/ )
{
   PropertyEdge *pPropertyEdge = pEdge->FindPropertyEdge( pPropertyVertex0, pPropertyVertex1 );

   if (pPropertyEdge)
      return pPropertyEdge;

   pPropertyEdge = AddPropertyEdge( pEdge, nID );
   pPropertyEdge->SetFirstPropertyVertex( pPropertyVertex0 );
   pPropertyEdge->SetSecondPropertyVertex( pPropertyVertex1 );

   return pPropertyEdge;
}

PropertyEdge *PropertySet::AddPropertyEdge( Edge *pEdge, UINT nID/*=-1*/ )
{
   PolyModel *pModel = GetModel();
   ASSERT(pModel);

   PropertyEdge *pPropertyEdge = NULL;

   // this might be better if we create an AddPatch()?
   if (pEdge->GetOwner()->GetSOT() == SOT_PatchModel)
      pPropertyEdge = CreatePropertyPatchEdge(nID);
   else
      pPropertyEdge = CreatePropertyEdge(nID);

   pEdge->AddPropertyEdge( pPropertyEdge );

   m_propertyedgelist.Add(pPropertyEdge);
   return pPropertyEdge;
}

BOOL PropertySet::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_REMOVEDFROMPROJECT:
      {
         for (POSITION pos=GetHeadPropertyPolyPosition(); pos; )
         {
            PropertyPoly *pPropertyPoly = GetNextPropertyPoly( pos );
            pPropertyPoly->m_pPoly->RemovePropertyPoly( pPropertyPoly );

            for (UINT i=0; i< pPropertyPoly->GetNumSides(); i++)
            {
               PropertyEdge *pPropertyEdge = pPropertyPoly->GetPropertyEdgeAt(i);
               pPropertyEdge->GetEdge()->RemovePropertyEdge(pPropertyEdge);
            }
         }
         break;
      }
      
      case NOTIFYOBJ_ADDEDTOPROJECT:
      {
         for (POSITION pos=GetHeadPropertyPolyPosition(); pos; )
         {
            PropertyPoly *pPropertyPoly = GetNextPropertyPoly( pos );
            for (UINT i=0; i< pPropertyPoly->GetNumSides(); i++)
            {
               PropertyEdge *pPropertyEdge = pPropertyPoly->GetPropertyEdgeAt(i);
               pPropertyEdge->GetEdge()->AddPropertyEdge(pPropertyEdge);
            }

            pPropertyPoly->m_pPoly->AddPropertyPoly( pPropertyPoly );
         }
         break;
      }
   }

   return HierObject::OnObjectNotify( msg, wParam, lParam );
}

PropertyVertex *PropertySet::CreatePropertyVertex(UINT nID/*=-1*/)
{
   return PropertyVertex::CreateWithID(nID);
}

PropertyEdge *PropertySet::CreatePropertyEdge(UINT nID/*=-1*/)
{
   return PropertyEdge::CreateWithID(nID);
}

PropertyPoly *PropertySet::CreatePropertyPoly(UINT nID/*=-1*/)
{
   return PropertyPoly::CreateWithID(nID);
}

PropertyVertex *PropertySet::CreatePropertyPatchComponent(UINT nID/*=-1*/)
{
   return PropertyVertex::CreateWithID(nID);
}

PropertyPatchEdge *PropertySet::CreatePropertyPatchEdge(UINT nID/*=-1*/)
{
   return PropertyPatchEdge::CreateWithID(nID);
}

PropertyPatch *PropertySet::CreatePropertyPatch(UINT nID/*=-1*/)
{
   return PropertyPatch::CreateWithID(nID);
}