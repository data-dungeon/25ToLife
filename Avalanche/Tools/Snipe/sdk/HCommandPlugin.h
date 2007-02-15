#pragma once
#include "HPlugin.h"

class HCommandPluginManager;
class HCBaseDoc;
class HSelectionList;

typedef void (* SDK_PFN_INIT_COMMANDPLUGIN)( void *pPluginData, HCommandPluginManager *pManager );
typedef void (* SDK_PFN_CREATEINSTANCE_COMMANDPLUGIN)( void *pPluginData, const String &strParameters, void *&pOutCommand );

// Instance
typedef void (* SDK_PFN_DESTROY_COMMANDINSTANCE)( void *pCommand );
typedef BOOL (* SDK_PFN_DO_COMMANDINSTANCE)( void *pCommand, StringList &strResults );
typedef BOOL (* SDK_PFN_VALIDATE_COMMANDINSTANCE)( void *pCommand, String &strOutError );
typedef UndoType (* SDK_PFN_SUPPORTUNDO_COMMANDINSTANCE)( void *pCommand );
typedef void (* SDK_PFN_REDO_COMMANDINSTANCE)( void *pCommand );
typedef void (* SDK_PFN_UNDO_COMMANDINSTANCE)( void *pCommand );

class SNIPEEXPORT HCommandPlugin : public HPlugin
{
public:
   void SetCLIName( const char *szCLIName );
   void SetBitmapID(UINT nID);
   void SetInitCommandPluginCallback(SDK_PFN_INIT_COMMANDPLUGIN pfnInit);
   void SetCreateCommandInstanceCallback(SDK_PFN_CREATEINSTANCE_COMMANDPLUGIN pfnCreate);
   void SetDestroyCommandInstanceCallback(SDK_PFN_DESTROY_COMMANDINSTANCE pfnDestroy);
   void SetValidateCommandInstanceCallback(SDK_PFN_VALIDATE_COMMANDINSTANCE pfnDo);
   void SetSupportUndoCommandInstanceCallback(SDK_PFN_SUPPORTUNDO_COMMANDINSTANCE pfnDo);
   void SetDoCommandInstanceCallback(SDK_PFN_DO_COMMANDINSTANCE pfnDo);
   void SetRedoCommandInstanceCallback(SDK_PFN_REDO_COMMANDINSTANCE pfnRedo);
   void SetUndoCommandInstanceCallback(SDK_PFN_UNDO_COMMANDINSTANCE pfnUndo);
   void SetHelp(const String &strHelp );

   SDK_PFN_INIT_COMMANDPLUGIN GetInitCommandPluginCallback() const;
   SDK_PFN_CREATEINSTANCE_COMMANDPLUGIN GetCreateCommandInstanceCallback() const;
   SDK_PFN_DESTROY_COMMANDINSTANCE GetDestroyCommandInstanceCallback() const;
   SDK_PFN_VALIDATE_COMMANDINSTANCE GetValidateCommandInstanceCallback() const;
   SDK_PFN_SUPPORTUNDO_COMMANDINSTANCE GetSupportUndoCommandInstanceCallback() const;
   SDK_PFN_DO_COMMANDINSTANCE GetDoCommandInstanceCallback() const;
   SDK_PFN_REDO_COMMANDINSTANCE GetRedoCommandInstanceCallback() const;
   SDK_PFN_UNDO_COMMANDINSTANCE GetUndoCommandInstanceCallback() const;
};