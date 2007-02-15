//
#include "stdafx.h"
#include "CommandUI.h"
#include "CommandUIManager.h"
#include "resource.h"
#include "../Utility/MemoryStream.h"

CommandUIManager::~CommandUIManager()
{
   RemoveAll();

   if (m_hAccel)
      ::DestroyAcceleratorTable(m_hAccel);
}

void CommandUIManager::RemoveAll()
{
   for (POSITION pos = m_commanduimap.GetStartPosition(); pos; ) {
      UINT nID;
      CommandUI *pCommandUI;
      m_commanduimap.GetNextAssoc( pos, nID, pCommandUI );
      if (pCommandUI &&  pCommandUI->IsAutoDelete())
         delete pCommandUI;
   }
   m_commanduimap.RemoveAll();
}

void CommandUIManager::AddCommandUI( CommandUI *pCommandUI )
{
   ASSERT( pCommandUI );
   ASSERT( GetCommandUI( pCommandUI->m_nCmdID ) == NULL );

   m_commanduimap.SetAt( pCommandUI->m_nCmdID, pCommandUI );
}

CommandUI *CommandUIManager::CreateCommandUI( UINT nCmdID )
{
   if (nCmdID == 0)
      return NULL;

   CommandUI *pCommandUI = GetCommandUI( nCmdID );
   if (pCommandUI)
      return pCommandUI;

   pCommandUI = SNEW CommandUI;
   pCommandUI->m_nCmdID = nCmdID;

   CString strFullText;
   strFullText.LoadString( nCmdID );
   CString strSubText;
   AfxExtractSubString( strSubText, strFullText, 0, '\n' );
   pCommandUI->m_strDescription = strSubText;
   AfxExtractSubString( strSubText, strFullText, 1, '\n' );
   if (strSubText.IsEmpty())
      pCommandUI->m_strName = pCommandUI->m_strDescription;
   else
      pCommandUI->m_strName = strSubText;

   pCommandUI->CreateIcon();

   m_commanduimap.SetAt( nCmdID, pCommandUI );

   return pCommandUI;
}

CommandUI *CommandUIManager::GetCommandUI( UINT nCmdID ) const
{
   CommandUI *pCommandUI = NULL;

   m_commanduimap.Lookup( nCmdID, pCommandUI);

   return pCommandUI;
}

GLImageList *CommandUIManager::CreateImageList( CBitmap *pBitmap )
{
   return NULL;
}

void CommandUIManager::Init()
{
   LoadAccelFromResource(); // Loads in Accel for non plugin commands
   LoadCustomAccelerators(); // Loads in all accelerator customizations and build actuallists
   RebuildAccelTableFromCommandUIs();
}

void CommandUIManager::LoadAccelFromResource()
{

   HACCEL hAccel = LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME));

   if (hAccel == NULL)
      return;

   int nCount;

   nCount = CopyAcceleratorTable(hAccel, NULL, 0);

   if ( nCount <=0 )
      return;

   LPACCEL pAccelArray = new ACCEL[ nCount ];
	::memset( pAccelArray, 0, sizeof(ACCEL)*nCount );

   int nCountCopied = ::CopyAcceleratorTable( hAccel, pAccelArray, nCount );
	if( nCountCopied != nCount )
	{
		ASSERT( FALSE );
		delete [] pAccelArray;
		return;
	}

   for (int i=0; i<nCount; i++) {
      CommandUI *pCommandUI = CreateCommandUI( pAccelArray[i].cmd );
      if (pCommandUI->m_strCategory.IsEmpty())
         pCommandUI->m_strCategory = "Default";
      pCommandUI->m_defaultaccellist.AddTail( CommandUIAccel(pAccelArray[i].fVirt, pAccelArray[i].key) );
   }

   delete [] pAccelArray;
}

#define ACCELVERSION 1

void CommandUIManager::LoadCustomAccelerators()
{
   LPBYTE pData = NULL;
   UINT n = 0;

   BOOL ret = AfxGetApp()->GetProfileBinary("CommandUI", "Accel", &pData, &n);

   if (ret && n!=0) {
      MemoryStream ms;
      ms.Open((char*)pData, n, BaseStream::BS_INPUT);

      int nEntries;
      int nVersion;
      ms >> nVersion;
      ms >> nEntries;

      CommandUICustomAccel accel;
      UINT nID;
      for (int i=0; i < nEntries; i++) {
         ms >> nID;
         accel.Serialize(ms);

         CommandUI *pCommandUI;
         if (m_commanduimap.Lookup(nID, pCommandUI)) {
            ASSERT(pCommandUI);
            pCommandUI->m_customaccellist.AddTail(accel);
         }
      }
   }

   // Build Actual Accel List
   for (POSITION pos = m_commanduimap.GetStartPosition(); pos; ) {
      UINT nID;
      CommandUI *pCommandUI;
      m_commanduimap.GetNextAssoc(pos, nID, pCommandUI);

      ASSERT(pCommandUI);
      pCommandUI->BuildActualAccelList();
   }
}

void CommandUIManager::SaveCustomAccelerators()
{
   MemoryStream ms;
   ms.Open(BaseStream::BS_OUTPUT);

   ms << ACCELVERSION;
   ms << (UINT) m_commanduimap.GetCount();

   POSITION pos;

   CommandUICustomAccel accel;

   for (pos = m_commanduimap.GetStartPosition(); pos; ) {
      CommandUI *pCommandUI;
      UINT nID;
     
      m_commanduimap.GetNextAssoc(pos, nID, pCommandUI);
      ASSERT(pCommandUI);
      
      for (POSITION accelpos = pCommandUI->m_customaccellist.GetHeadPosition(); accelpos; ) {
         CommandUICustomAccel &accel = pCommandUI->m_customaccellist.GetNext(accelpos);

         ms << nID;
         accel.Serialize(ms);
      }
   }
   if (ms.GetLength()==0)
      AfxGetApp()->WriteProfileString("CommandUI", "Accel", NULL); // Should delete the data 
   else
      AfxGetApp()->WriteProfileBinary("CommandUI", "Accel", (LPBYTE) ms.GetBuffer(), (UINT)ms.GetLength());
}

void CommandUIManager::RebuildAccelTableFromCommandUIs()
{
   if (m_hAccel) {
      DestroyAcceleratorTable(m_hAccel);
      m_hAccel = NULL;
   }

   int nCount = 0;

   for (POSITION pos = m_commanduimap.GetStartPosition(); pos; ) {
      UINT nID;
      CommandUI *pCommandUI;
      m_commanduimap.GetNextAssoc(pos, nID, pCommandUI);
      ASSERT(pCommandUI);
      nCount += pCommandUI->m_actualaccellist.GetCount();
   }

   if (nCount==0)
      return;

   LPACCEL pAccelArray = SNEW ACCEL [ nCount ];
	::memset( pAccelArray, 0, sizeof(ACCEL)*nCount );

   int nCurrIndex = 0;
   for (POSITION pos = m_commanduimap.GetStartPosition(); pos; ) {
      UINT nID;
      CommandUI *pCommandUI;
      m_commanduimap.GetNextAssoc(pos, nID, pCommandUI);
      for (POSITION accelpos = pCommandUI->m_actualaccellist.GetHeadPosition(); accelpos; ) {
         CommandUIAccel &commanduiaccel = pCommandUI->m_actualaccellist.GetNext(accelpos);
         LPACCEL lpaccel = &pAccelArray[nCurrIndex];
         lpaccel->cmd = nID;
         lpaccel->fVirt = commanduiaccel.m_virt;
         lpaccel->key = commanduiaccel.m_key;
         nCurrIndex++; 
      }
   }

	m_hAccel = ::CreateAcceleratorTable( pAccelArray, nCount );

   delete pAccelArray;
}