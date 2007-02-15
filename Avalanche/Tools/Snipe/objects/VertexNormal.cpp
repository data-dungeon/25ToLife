#include "stdafx.h"
#include "VertexNormal.h"

IMPLEMENT_COMPONENT_CREATE( VertexNormal, Normal )

BOOL VertexNormal::IsUsed()
{
   return m_nPolyCount > 0;
}

void VertexNormal::Disconnect()
{
   ASSERT( m_nPolyCount==0 );

   Component::Disconnect();
}

void VertexNormal::Reconnect()
{
   ASSERT( m_nPolyCount==0 );

   Component::Reconnect();
}
