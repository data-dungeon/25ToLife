//

#include "stdafx.h"
#include "LockCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "HPolyModel.h"
#include "HVertex.h"
#include "HEdge.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HCommandPluginManager.h"
#include "../Resource.h"

CommandInstance *LockCommandPlugin::CreateCommandInstance() { return SNEW LockCommand(this); }

const char *LockCommandPlugin::GetCLIName() const { return "Lock"; }; // Name used in scripts and command line
const char *LockCommandPlugin::GetTitleName() const { return "Lock"; }
UINT LockCommandPlugin::GetBitmapID() const { return IDB_LOCK; }

void LockCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
}

String LockCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Locks objects and components so that they cannot be manipulated.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
LockCommand::LockCommand(HCommandPluginHelper *pPlugin) : CommandInstance(pPlugin)
{
   m_bLock = TRUE;
}

LockCommand::~LockCommand()
{
}

UndoType LockCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL LockCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-a" ) == 0) // all objects of current type
   {
      HSnipeObjectList *pList = GetHMainInterface()->GetCurrentCommandObject()->CreateListFromSOT( GetHMainInterface()->GetCurrentSelectionSOT() );
      if (pList)
      {
         for (POSITION pos=pList->GetHeadPosition(); pos; )
         {
            HSnipeObject *pObject = pList->GetNext( pos );
            if (pObject->IsLockable())
               m_objectlist.AddTail( pObject );
         }
         delete pList;
      }
      if (m_objectlist.IsEmpty())
         THROW_FATAL_ERROR( "Unable to execute Lock command, no lockable objects." );
      return TRUE;
   }
   else if (stricmp( szToken, "-i" ) == 0) // inverse of selected objects
   {
      HSnipeObjectList *pList = GetHMainInterface()->GetCurrentCommandObject()->CreateListFromSOT( GetHMainInterface()->GetCurrentSelectionSOT() );
      if (pList)
      {
         for (POSITION pos=pList->GetHeadPosition(); pos; )
         {
            HSnipeObject *pObject = pList->GetNext( pos );
            if (!pObject->IsSelected() && pObject->IsLockable())
               m_objectlist.AddTail( pObject );
         }
         delete pList;
      }
      if (m_objectlist.IsEmpty())
         THROW_FATAL_ERROR( "Unable to execute Lock command, no lockable unselected objects." );
      return TRUE;
   }
   else if (stricmp( szToken, "-o" ) == 0) // specified objects
   {
      StringList strErrorList;
      HSnipeObjectList *pObjectList = GetHMainInterface()->DescriptionToObjects( tokenlist.GetNext(), strErrorList );
      for (POSITION pos=pObjectList->GetHeadPosition(); pos; )
      {
         HSnipeObject *pObject = pObjectList->GetNext( pos );
         if (pObject->IsLockable())
            m_objectlist.AddTail( pObject );
      }
      delete pObjectList;
      if (m_objectlist.IsEmpty())
         THROW_FATAL_ERROR( "Unable to execute Lock command, no lockable objects provided." );
      return TRUE;
   }
   else if (stricmp( szToken, "-u" ) == 0) // unlock
   {
      m_bLock = FALSE;
      return TRUE;
   }

   return FALSE;
}

BOOL LockCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate(strOutError))
      return FALSE;

   if (m_objectlist.IsEmpty())
   {
      // Lets use the current selectionlist
      HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
      for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
      {
         HSnipeObject *pObject = pSelectionList->GetNext( pos );
         if (pObject->IsLocked()!=m_bLock && pObject->IsLockable())
            m_objectlist.AddTail( pObject );
      }
   }
   else
   {
      for (POSITION pos=m_objectlist.GetHeadPosition(); pos; )
      {
         POSITION prevpos = pos;
         HSnipeObject *pObject = m_objectlist.GetNext( pos );
         if (pObject->IsLocked() == m_bLock)
            m_objectlist.RemoveAt( prevpos );
      }
   }

   if (m_objectlist.IsEmpty())
   {
      strOutError = "Unable to execute Lock command, no lockable objects provided.";
      return FALSE;
   }

   return TRUE;
}

BOOL LockCommand::OnDo( StringList &strResults )
{
   //HSnipeObjectList *pSnipeObjectList = pSelectionList->CreateSnipeObjectList();
   //pSnipeObjectList->ConvertCompatibleObjectsToSOT( SOT_Vertex );

   for (POSITION pos=m_objectlist.GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = m_objectlist.GetNext( pos );
      pSnipeObject->SetLocked( m_bLock );
   }

   //delete pSnipeObjectList;

   return TRUE;
}

void LockCommand::OnUndo()
{
   for (POSITION pos = m_objectlist.GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = m_objectlist.GetNext(pos);
      pSnipeObject->SetLocked( !m_bLock );
   }
}
