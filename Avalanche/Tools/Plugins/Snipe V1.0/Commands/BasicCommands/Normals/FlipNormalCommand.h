#pragma once
#include "HCommandPluginHelper.h"

class HPoly;

class FlipNormalCommand : public CommandInstance
{
public:
   CList<HPoly *> m_polylist;
 //  CList<HDeleteFaceInfo *> m_deletefaceinfos;

   FlipNormalCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
   }

   ~FlipNormalCommand();

   // PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class FlipNormalCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( FlipNormalCommandPlugin, HCommandPluginHelper )

   FlipNormalCommandPlugin() 
   {
   }
   //HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};