//

#include "stdafx.h"
#include "Resource.h"
#include "RegistryIDMap.h"

void RegistryIDMap::Load()
{
   // Load CmdIDs
   m_mapCmdID.RemoveAll();
   m_usedcmdids.RemoveAll();
   m_usedcmdids.SetSize(ID_PLUGIN_COMMAND_LAST - ID_PLUGIN_COMMAND_FIRST +1);
   
   LPBYTE pData = NULL;
   UINT n = 0;

   BOOL ret = AfxGetApp()->GetProfileBinary("Plugins", "CmdIDs", &pData, &n);
   if (ret && n!=0) {
      int nEntries = n/sizeof(CmdID);

      MemoryStream ms;
      ms.Open((char*)pData, n, BaseStream::BS_INPUT);

      CmdID cmdentry;
      for (int i=0; i < nEntries; i++) {
         cmdentry.Serialize(ms);
         m_mapCmdID.SetAt(cmdentry.m_lookup, cmdentry.m_nID);
         m_usedcmdids[cmdentry.m_nID-ID_PLUGIN_COMMAND_FIRST] = TRUE;
      }
   }

   if (pData)
      delete [] pData;
}

void RegistryIDMap::Save()
{
   MemoryStream ms;
   ms.Open(BaseStream::BS_OUTPUT);

   POSITION pos;

   CmdID cmdentry;
   for (pos = m_mapCmdID.GetStartPosition(); pos; ) {
      m_mapCmdID.GetNextAssoc(pos, cmdentry.m_lookup, cmdentry.m_nID);
      cmdentry.Serialize(ms);
   }
   if (ms.GetLength()==0)
      AfxGetApp()->WriteProfileString("Plugins", "CmdIDs", NULL); // Should delete the data 
   else
      AfxGetApp()->WriteProfileBinary("Plugins", "CmdIDs", (LPBYTE) ms.GetBuffer(), (UINT)ms.GetLength());
}

void RegistryIDMap::Free()
{
   m_mapCmdID.RemoveAll();
   m_usedcmdids.RemoveAll();
}

UINT RegistryIDMap::GetCmdID(const String &strPluginName, const String &strClassName)
{
   CmdLookup lookup(strPluginName, strClassName);
   
   UINT nID;
   if (m_mapCmdID.Lookup(lookup, nID))
      return nID;

   for (nID = ID_PLUGIN_COMMAND_FIRST; nID < ID_PLUGIN_COMMAND_LAST; nID++) {
      if (m_usedcmdids[nID-ID_PLUGIN_COMMAND_FIRST]==FALSE) {
         m_usedcmdids[nID-ID_PLUGIN_COMMAND_FIRST] = TRUE;
         m_mapCmdID.SetAt(lookup, nID);
         return nID;
      }
   }
   return (UINT)-1;
}