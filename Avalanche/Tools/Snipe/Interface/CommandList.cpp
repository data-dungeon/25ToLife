//
#include "stdafx.h"
#include "CommandList.h"
#include "CommandPlugin.h"
#include "SnipeCommand.h"

#include "..\Gui\Snipe.h"
#include "..\Gui\MainFrm.h"
#include "..\Gui\OutputView.h"

CommandList::CommandList()
{
   m_pos = 0;
   m_nMaxSize = 30;
   m_nAt = 0;
   m_bRebuildListControl = FALSE;
   m_bRefreshListControl = FALSE;
   m_pTempCommand = NULL;
   m_pCommandObject = NULL;
}

CommandList::~CommandList()
{
   Empty();
}

void CommandList::Empty()
{
   RemoveAll();
   if (m_pTempCommand)
      delete m_pTempCommand;
}

void CommandList::RemoveAll()
{
   for (POSITION pos = GetHeadPosition(); pos; )
      delete GetNext(pos);

   m_pos = 0;
   m_nAt = 0;
   CommandListBase::RemoveAll();
   m_bRebuildListControl = TRUE;
}

void CommandList::RemoveRedo()
{
   POSITION curpos;
   
   if (m_pos){
      curpos = m_pos;
      GetNext(curpos);
   }
   else
      curpos = GetHeadPosition();
   
   while (curpos) {
      POSITION oldpos = curpos;
      delete GetNext(curpos);
      RemoveAt(oldpos);
   }
   m_bRebuildListControl = TRUE;
}

void CommandList::SetMaxSize( UINT nSize )
{
   if (nSize == m_nMaxSize)
      return;

   m_bRebuildListControl = TRUE;
   if (nSize > (UINT)GetCount()) { // we are growing this is fine
      m_nMaxSize = nSize;
      return;
   }

   // we are getting smaller, lets remove our redo stack first
   // it is less important than undo

   RemoveRedo();

   if (nSize >= (UINT)GetCount()) { // we are fine
      m_nMaxSize = nSize;
      return;
   }

   // Ahh we are shrinking, lets remove the oldest history
   int nCount = GetCount() - nSize;
   int i = 0;
   for (POSITION pos = GetHeadPosition(); i < nCount; i++)
      delete GetNext(pos);

   m_pos = GetTailPosition();
   m_nAt = GetSize();
}

void CommandList::AddCommand(SnipeCommand *pCommand)
{
   RemoveRedo();

   AddTail(pCommand);
   m_pos = GetTailPosition();

	if ((UINT)GetSize() > m_nMaxSize)
      delete RemoveHead();

   m_nAt = GetSize();
   m_bRebuildListControl = TRUE;
}

BOOL CommandList::Do(CommandPlugin *pCommandPlugin, const String &strParameters, StringList &strResults, BOOL bTempCommand/*=FALSE*/ )
{
   UndoTempCommand();

   if (!bTempCommand && GetApp().IsDisplayCommandInOutput())
   {
      String strDesc;
      GetMainInterface()->ObjectToDescription( m_pCommandObject, strDesc, FALSE );
      g_outputWindow << strDesc << "<:>" << pCommandPlugin->GetCommandInfo(strParameters, FALSE) << "\n";
   }

   SnipeCommand *pCommand = NULL;
   UndoType nUndoType;

   void *pCommandInstance = NULL;
   try 
   {
	   pCommandPlugin->GetCreateCommandInstanceCallback()(pCommandPlugin->GetPluginData(), strParameters, pCommandInstance);
      pCommand = SNEW SnipeCommand(pCommandPlugin, strParameters, pCommandInstance);
   }
   catch (const String &strReason) 
   {
      g_outputWindow << pCommandPlugin->FormatError("Create Command Instance", strReason) << "\n";
      return NULL;
   }
   catch (...) 
   {
      g_outputWindow << pCommandPlugin->FormatError("Create Command Instance", "Unknown") << "\n";
      return NULL;
   }

   if (pCommandPlugin->GetValidateCommandInstanceCallback())
   {
      try 
      {
         String strError;
         if (!pCommandPlugin->GetValidateCommandInstanceCallback()(pCommandInstance, strError ))
         {
            g_outputWindow << pCommandPlugin->FormatError("Validate Command Instance", strError) << "\n";
            delete pCommand;
            return FALSE;
         }
      }
      catch (const String &strReason) 
      {
         g_outputWindow << pCommandPlugin->FormatError("Validate Command Instance", strReason) << "\n";
         delete pCommand;
         return FALSE;
      }
      catch (...) 
      {
         g_outputWindow << pCommandPlugin->FormatError("Validate Command Instance", "Unknown") << "\n";
         delete pCommand;
         return FALSE;
      }
   }

   try 
   {
      nUndoType = pCommandPlugin->GetSupportUndoCommandInstanceCallback()(pCommandInstance);
   }
   catch (const String &strReason) 
   {
      g_outputWindow << pCommandPlugin->FormatError("Undo Support Command Instance", strReason) << "\n";
      delete pCommand;
      return FALSE;
   }
   catch (...) 
   {
      g_outputWindow << pCommandPlugin->FormatError("Undo Support Command Instance", "Unknown") << "\n";
      delete pCommand;
      return FALSE;
   }

   if ( !bTempCommand && nUndoType == UNDO_SUPPORTED)
      AddCommand(pCommand);

   pCommand->Do(m_pCommandObject, strResults, bTempCommand);

   if (bTempCommand)
   {
      ASSERT(nUndoType==UNDO_SUPPORTED);
      m_pTempCommand = pCommand;
   }

   if (nUndoType==UNDO_UNSUPPORTED)
   {
      Empty();
      delete pCommand;
      return FALSE;
   }

   if (nUndoType==UNDO_UNNECESSARY)
   {
      delete pCommand;
      return FALSE;
   }

   return TRUE;
};

void CommandList::Redo()
{
   UndoTempCommand();

   SnipeCommand *pCommand;

   if (m_pos) {
      GetNext(m_pos);
      pCommand = GetAt(m_pos);
   }
   else {
      m_pos = GetHeadPosition();
      pCommand = GetAt(m_pos);
   }
   m_nAt++;
   m_bRefreshListControl = TRUE;

   if (GetApp().IsDisplayCommandInOutput())
   {
      String strDesc;
      GetMainInterface()->ObjectToDescription( m_pCommandObject, strDesc, FALSE );
      g_outputWindow << strDesc << "<:>" << "Redo\n";
   }

 	pCommand->Redo(m_pCommandObject, FALSE);
};

BOOL CommandList::UndoTempCommand()
{
   if (m_pTempCommand) {
      m_pCommandObject->GetSelectionList()->SetSelectionChangedOnTempCommand(FALSE);
      m_pTempCommand->Undo(m_pCommandObject, TRUE);
      delete m_pTempCommand;
      m_pTempCommand = NULL;
      return TRUE;
   }
   return FALSE;
}

BOOL CommandList::CommitTempCommand()
{
   if (m_pTempCommand==NULL)
      return FALSE;

   SnipeCommand *pCommand = m_pTempCommand;
   m_pTempCommand = NULL;

   AddCommand(pCommand);

  if (m_pCommandObject->GetSelectionList()->WasSelectionChangedOnTempCommand())
     GetMainInterface()->NotifyNodes( NOTIFYOBJ_SELECTIONLISTCHANGED, (WPARAM)FALSE, (LPARAM)m_pCommandObject->GetSelectionList() );

   if (GetApp().IsDisplayCommandInOutput())
   {
      String strDesc;
      GetMainInterface()->ObjectToDescription( m_pCommandObject, strDesc, FALSE );
      g_outputWindow << strDesc << "<:>" << pCommand->m_pPlugin->GetCommandInfo(pCommand->m_strParameters, FALSE) << "\n";
   }

   return TRUE;
}

void CommandList::Undo()
{
   UndoTempCommand();

	SnipeCommand *pCommand = GetPrev(m_pos);
   m_nAt--;
   m_bRefreshListControl = TRUE;

   if (GetApp().IsDisplayCommandInOutput())
   {
      String strDesc;
      GetMainInterface()->ObjectToDescription( m_pCommandObject, strDesc, FALSE );
      g_outputWindow << strDesc << "<:>" << "Undo\n";
   }

 	pCommand->Undo(m_pCommandObject, FALSE);
};

BOOL CommandList::HasUndo()
{
   return m_nAt!=0;
}

BOOL CommandList::HasRedo()
{
   return (m_nAt < (UINT)GetSize());
}

void CommandList::SetCurrentPosition( UINT nIndex )
{
   ASSERT( nIndex <= (UINT)GetSize() );

   if (nIndex > m_nAt ) {
      while (m_nAt != nIndex)
         Redo();
   }
   else {
      while (m_nAt != nIndex)
         Undo();
   }
}