#include "stdafx.h"
#include "SetObject.h"

SetNode::SetNode()
{
   m_pComponent = NULL;
}

SetNode::SetNode(const SetNode &other)
{
   m_pComponent = other.m_pComponent;
}

SetNode::~SetNode()
{
}

SetNode &SetNode::operator = ( const SetNode &other )
{
   m_pComponent = other.m_pComponent;
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SNIPEOBJECT_PURE( SetObject )

SetObject::SetObject()
{
   m_setnodes.SetOwner(this);
}

SetObject::~SetObject()
{
}

SetNode *SetObject::Add()
{
   SetNode *pSetNode = SNEW SetNode;
   m_setnodes.Add(pSetNode);
   return pSetNode;
}

SetNode *SetObject::AddComponent(Component *pComponent)
{
   SetNode *pSetNode = Add();
   pSetNode->m_pComponent = pComponent;
   return pSetNode;
}

void SetObject::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   m_setnodes.Save( bs, "Objects" );

   HierObject::Save( bs );
}

BOOL SetObject::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "Objects")
      return m_setnodes.Load( bs );
   else
      return HierObject::ParseData( bs, dataheader );
}
