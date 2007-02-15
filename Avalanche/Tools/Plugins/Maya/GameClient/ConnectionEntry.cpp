#include "GameClientPCH.h"
#include "ConnectionManager.h"

///////////////////////////////////////////////////////////////////////////////
// Connection Entry
///////////////////////////////////////////////////////////////////////////////

ConnectionEntry::ConnectionEntry()
{
   m_pData = NULL;
   m_pConnectionManager = NULL;
}

ConnectionEntry::~ConnectionEntry()
{
   switch (m_connectionType)
   {
   case ConnectionEntry::MODEL :
      {
      }
      break;
   case ConnectionEntry::SCENE :
      {
      }
      break;
   case ConnectionEntry::NODE :
      {
         MNodeMessage::removeCallback(m_callbackID);
      }
      break;
   }
   if (m_pData)
      delete m_pData;
}
