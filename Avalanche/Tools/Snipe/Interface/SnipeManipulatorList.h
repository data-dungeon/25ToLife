#pragma once

#include "SnipeManipulator.h"

class SnipeManipulatorList : public CList< SnipeManipulator *>
{
private:
   SnipeManipulatorInstance *m_pCurrentManipulatorInstance;

public:
   BOOL m_bDirty;

   SnipeManipulatorList()
   {
      m_pCurrentManipulatorInstance = NULL;
      m_bDirty = FALSE;
   }

   ~SnipeManipulatorList()
   {
      RemoveAll();
   }

   void RemoveAll();
   SnipeManipulator *Find( UINT nID ) const;
   SnipeManipulatorInstance *SetCurrentManipulatorInstance( SnipeManipulator *pCurrentManipulator );
   SnipeManipulatorInstance *GetCurrentManipulatorInstance() const { return m_pCurrentManipulatorInstance; }
};

inline SnipeManipulatorInstance *SnipeManipulatorList::SetCurrentManipulatorInstance( SnipeManipulator *pCurrentManipulator )
{
   if (m_pCurrentManipulatorInstance) {
      m_pCurrentManipulatorInstance->Destroy();
      m_pCurrentManipulatorInstance = NULL;
   }

   if (pCurrentManipulator)
      m_pCurrentManipulatorInstance = pCurrentManipulator->CreateInstance();

   return m_pCurrentManipulatorInstance;
}

inline void SnipeManipulatorList::RemoveAll()
{
   SetCurrentManipulatorInstance(NULL);
   CList< SnipeManipulator *>::RemoveAll();
}

