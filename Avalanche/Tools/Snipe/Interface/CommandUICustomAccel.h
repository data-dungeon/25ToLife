#pragma once

#include "SDK/CommandUIAccel.h"
#include "../Utility/MemoryStream.h"

class CommandUICustomAccel : public CommandUIAccel
{
public:
   BOOL m_bAdd;

   CommandUICustomAccel() {}
   CommandUICustomAccel(BOOL bAdd, BYTE virt, WORD key):
    CommandUIAccel(virt, key),
    m_bAdd(bAdd)
   {
   }

   void Serialize( BaseStream &bs )
   {
      if (bs.IsInputStream()) {
         bs >> m_virt;
         bs >> m_key;
         bs >> m_bAdd;
      }
      else {
         bs << m_virt;
         bs << m_key;
         bs << m_bAdd;
      }
   }
};

