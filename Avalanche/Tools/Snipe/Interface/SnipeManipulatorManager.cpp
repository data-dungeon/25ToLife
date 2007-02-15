//

#include "stdafx.h"
#include "SnipeManipulatorManager.h"
#include "SnipeManipulator.h"
#include "ManipulatorPlugin.h"

SnipeManipulatorManager::SnipeManipulatorManager()
{
}

SnipeManipulatorManager::~SnipeManipulatorManager()
{
   RemoveAll();
}

SnipeManipulator *SnipeManipulatorManager::Find( UINT nID ) const
{
    return m_alllist.Find( nID );
}

void SnipeManipulatorManager::AddSnipeManipulator( ManipulatorPlugin *pPlugin )
{
   m_alllist.AddTail(SNEW SnipeManipulator(pPlugin));
}

void SnipeManipulatorManager::RemoveAll()
{
   for (POSITION pos = m_alllist.GetHeadPosition(); pos; )
      delete m_alllist.GetNext(pos);

   m_alllist.RemoveAll();
}
