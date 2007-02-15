#pragma once

class SnipeCommand;
class CommandPlugin;
class SnipeObject;

typedef CList<SnipeCommand *> CommandListBase;

class CommandList : public CommandListBase
{
protected:
   UINT     m_nMaxSize;
   POSITION m_pos; // these should always be the same command
   UINT     m_nAt;
   ULONG    m_bRebuildListControl : 1,
            m_bRefreshListControl : 1;
   SnipeCommand *m_pTempCommand;
   SnipeObject  *m_pCommandObject;

   void AddCommand(SnipeCommand *pCommand);

public:

    CommandList();
	~CommandList();

   void RemoveRedo();
   void Empty();
	void RemoveAll();
	void SetMaxSize( UINT nSize );
   UINT GetMaxSize() { return m_nMaxSize; }

   void SetCurrentPosition( UINT nIndex );// one based
   UINT GetCurrentPosition() { return m_nAt; } // one based

   void SetCommandObject ( SnipeObject *pCommandObject ) { m_pCommandObject = pCommandObject; }
   SnipeCommand *GetTempCommand() { return m_pTempCommand; }

   BOOL HasUndo();
   BOOL HasRedo();

   BOOL IsRebuildListControl() const { return m_bRebuildListControl; }
   void SetRebuildListControl( BOOL bRebuildListControl ) { m_bRebuildListControl = bRebuildListControl; }

   BOOL IsRefreshListControl() const { return m_bRefreshListControl; }
   void SetRefreshListControl( BOOL bRefreshListControl ) { m_bRefreshListControl = bRefreshListControl; }

	BOOL Do( CommandPlugin *pCommand, const String &strParameters, StringList &strResults, BOOL bTempCommand=FALSE );
	void Redo();
	void Undo();
   BOOL UndoTempCommand();
   BOOL CommitTempCommand();
};
