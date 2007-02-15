//
#include "stdafx.h"
#include "PluginCmdProfile.h"
#include "Resource.h"

CExtCmdItem *CExtPluginCmdProfile::OnCreateCmdItem( const CExtCmdItem & _cmd )
{
   return new CExtPluginCmdItem( _cmd );
}

bool CExtPluginCmdProfile::SerializeState( CArchive & ar )
{
   if (!CExtCmdProfile::SerializeState( ar ) )
   {
      ASSERT( FALSE );
      return false;
   }

   if( ar.IsStoring() )
   {
      DWORD dwCount = (DWORD)m_cmds.GetCount();
      ar << dwCount;
      
      for (POSITION pos = m_cmds.GetStartPosition(); pos; ) {
         UINT nCmdID = 0;
         CExtPluginCmdItem *pCmdItem = NULL;
         
         m_cmds.GetNextAssoc( pos, nCmdID, (CExtCmdItem*&)pCmdItem );
         
         ASSERT( CExtCmdManager::IsCommand(nCmdID) );
         ASSERT( pCmdItem != NULL && pCmdItem->m_nCmdID == nCmdID );

         ar << pCmdItem->m_nCmdID;
         ar << (bool)pCmdItem->m_bPluginCmd;
         if (pCmdItem->m_bPluginCmd) {
            ar << pCmdItem->m_strPluginName;
            ar << pCmdItem->m_strClassName;
         }
      }
   }
   else { // Loading
      DWORD dwCount;
      ar >> dwCount;

      //Lets build a map of "PLUGINNAME:CLASSNAME" to CExtPluginCmdItems
      CMap <String, const String &, CExtPluginCmdItem *, CExtPluginCmdItem *> nametocommandmap;
      int nCmdCount = m_cmds.GetCount();
      nCmdCount = fast_ftol_signed((float)nCmdCount * 1.2); //20% bigger for speed
      nametocommandmap.InitHashTable(nCmdCount);

      UINT CmdID;
      CExtPluginCmdItem *pCmdItem;
     
      for (POSITION pos = m_cmds.GetStartPosition(); pos; ) {
         m_cmds.GetNextAssoc(pos, CmdID, (CExtCmdItem *&)pCmdItem);
         if (!pCmdItem->m_bPluginCmd)
            continue;
         String strLookup((String)(LPCTSTR)pCmdItem->m_strPluginName + ":" + (LPCTSTR)pCmdItem->m_strClassName);
         nametocommandmap.SetAt(strLookup, pCmdItem);
      }

      int numids = ID_PLUGIN_COMMAND_LAST-ID_PLUGIN_COMMAND_FIRST+1;
      BOOL usedids[ID_PLUGIN_COMMAND_LAST-ID_PLUGIN_COMMAND_FIRST+1];
      memset(usedids, 0, numids);

      for( DWORD dwItem = 0; dwItem < dwCount; dwItem++ ) {
         CExtPluginCmdItem _cmdItem;
         ar >> _cmdItem.m_nCmdID;
         
         bool bPluginCmd;
         ar >> bPluginCmd;
         _cmdItem.m_bPluginCmd = bPluginCmd;
         
         if (!bPluginCmd)
            continue;
         
         ar >> _cmdItem.m_strPluginName;
         ar >> _cmdItem.m_strClassName;

         ASSERT(!_cmdItem.m_strPluginName.IsEmpty());
         ASSERT(!_cmdItem.m_strClassName.IsEmpty());

         //Lets give our registered cmditems there ID they had before
         String strLookup(_cmdItem.m_strPluginName + ":" + _cmdItem.m_strClassName);
         if (nametocommandmap.Lookup(strLookup, pCmdItem)) {
            pCmdItem->m_nCmdID = _cmdItem.m_nCmdID;
            pCmdItem->m_pEntry->SetCmdID(pCmdItem->m_nCmdID);
            pCmdItem->m_bValidCmdID = TRUE;
            pCmdItem->m_bNewCommand = FALSE;
         }
         // We want to mark the ID as used regardless if it doesn't
         // exist as a command anymore because we don't want a new command 
         // to get it's ID because we don't want the new command to get its state loaded
         // based on the reuse of an old cmdid.
         usedids[pCmdItem->m_nCmdID - ID_PLUGIN_COMMAND_FIRST] = TRUE;
      }
      // Lets give all new CmdID's a valid CmdID
      for (POSITION pos = m_cmds.GetStartPosition(); pos; ) {
         m_cmds.GetNextAssoc(pos, CmdID, (CExtCmdItem *&)pCmdItem);
         if (!pCmdItem->m_bPluginCmd)
            continue;
         if (pCmdItem->m_bNewCommand) {
            UINT nNewCmdID = (UINT)-1;
            for (int i=0; i < numids; i++) {
               if (usedids[i]==FALSE) {
                  nNewCmdID = ID_PLUGIN_COMMAND_FIRST + i;
                  usedids[i] = TRUE;
                  break;
               }
            }
            ASSERT( nNewCmdID != (UINT)-1 ); // What we used all the commandid's?
            pCmdItem->m_nCmdID = nNewCmdID; 
            pCmdItem->m_pEntry->SetCmdID(nNewCmdID);
            pCmdItem->m_bValidCmdID = TRUE;
         }
      }
   }
   return true;
}
