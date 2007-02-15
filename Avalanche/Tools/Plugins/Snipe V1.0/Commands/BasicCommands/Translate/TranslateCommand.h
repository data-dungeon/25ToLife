#pragma once
#include "HCommandPluginHelper.h"
#include "Matrix44.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class TranslateCommand : public CommandInstance
{
public:
   Vector   m_vDelta;
   Matrix44 m_matrix;
   CList<HSnipeObject *> m_objectlist;
   HSelectionList *m_pSelectionList;

            TranslateCommand(HCommandPluginHelper *pPlugin);
   virtual ~TranslateCommand();

   BOOL IsListEmpty() const;
   POSITION GetListHeadPosition();
   HSnipeObject *GetListNext( POSITION &pos );
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
};

class TranslateCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( TranslateCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
