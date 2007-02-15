#pragma once
#include "HCommandPluginHelper.h"
#include "HVertex.h"

class WeldPair
{
private:
   HVertex::HWeldInfo *m_pWeldInfo;
   HVertex *m_pFrom;
   HVertex *m_pTo;

public:
   WeldPair( HVertex *pFrom, HVertex *pTo )
   {
      m_pWeldInfo = NULL;
      m_pFrom = pFrom;
      m_pTo = pTo;
   }

   ~WeldPair();

   virtual BOOL Do( String &strError ); 
   virtual void Undo(); 
   virtual void Redo(); 
};

class WeldCommand : public CommandInstance
{
public:
   CList<WeldPair *> m_WeldPairs;

   WeldCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
   }

   virtual ~WeldCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class WeldCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( WeldCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
