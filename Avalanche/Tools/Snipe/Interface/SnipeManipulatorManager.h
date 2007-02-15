#pragma once

#include "sdk/enum.h"
#include "SnipeManipulatorList.h"

class ManipulatorPlugin;
class SnipeManipulator;

class SnipeManipulatorManager
{
public:
   SnipeManipulatorList m_alllist;

   SnipeManipulatorManager();
   virtual ~SnipeManipulatorManager();

   void AddSnipeManipulator( ManipulatorPlugin *pPlugin );
   void RemoveAll();

   SnipeManipulator *Find( UINT nID ) const;
};
