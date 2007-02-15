#pragma once

#include "Component.h"

class Poly;

class VertexNormal : public Component
{
private:
   Vector       m_direction;

   union {
      ULONG     m_nVertexNormalFlags;
      struct {
         ULONG 
                m_bIsVertexNormalDirty : 1,
                m_nPolyCount        : 16;
      };
   };

public:
   DECLARE_COMPONENT_CREATE( VertexNormal, Component )

   VertexNormal()
   {
      m_nVertexNormalFlags = 0L;
      SetVertexNormalDirtyFlag(TRUE);
   }
   VertexNormal( UINT nID )
      : Component( nID )
   {
      m_nVertexNormalFlags = 0L;
      SetVertexNormalDirtyFlag(TRUE);
   }

   void AddPoly( Poly *pPoly )
   {
      if (!m_bInList)
         ReInsertIntoList();
      m_nPolyCount++;
   }
   void RemovePoly( Poly *pPoly )
   {
      ASSERT( m_nPolyCount );
      m_nPolyCount--;
      if (!IsUsed())
         RemoveFromList();
   }

   void SetDir(const Vector &direction)
   {
      m_direction = direction;
      SetVertexNormalDirtyFlag( FALSE );
   }
   const Vector &GetDir() const { return m_direction; }

   BOOL IsVertexNormalDirty() { return m_bIsVertexNormalDirty; }
   void SetVertexNormalDirtyFlag(BOOL bFlag)
   {
      m_bIsVertexNormalDirty = bFlag;
   }

// Component overrides
   virtual BOOL IsUsed();
   virtual void Reconnect();
   virtual void Disconnect();
};
