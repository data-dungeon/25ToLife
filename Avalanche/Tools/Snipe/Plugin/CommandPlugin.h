#pragma once
#include "Plugin.h"
#include "GUI/GLImageList.h"
#include "GUI/BaseDoc.h"

class SnipePlugin;

class CommandParameters;
class CommandPluginManager;

typedef void (* PFN_INIT_COMMANDPLUGIN)( void *pPluginData, CommandPluginManager *pManager );
typedef void (* PFN_CREATEINSTANCE_COMMANDPLUGIN)( void *pPluginData, const String &pParameters, void *&pCommand );

// Instance
typedef void (* PFN_DESTROY_COMMANDINSTANCE)( void *pCommand );
typedef BOOL (* PFN_VALIDATE_COMMANDINSTANCE)( void *pCommand, String &strOutError );
typedef UndoType (* PFN_SUPPORTUNDO_COMMANDINSTANCE)( void *pCommand );
typedef BOOL (* PFN_DO_COMMANDINSTANCE)( void *pCommand, StringList &strResults );
typedef void (* PFN_REDO_COMMANDINSTANCE)( void *pCommand );
typedef void (* PFN_UNDO_COMMANDINSTANCE)( void *pCommand );

// Create parameters class
class CommandPlugin : public Plugin
{
private:
   GLImageList m_glImageListIcon;
   String m_strHelp;
   String m_strCLIName;

   catch_msg PFN_INIT_COMMANDPLUGIN m_pfnInit_CommandPlugin;
   catch_msg PFN_CREATEINSTANCE_COMMANDPLUGIN  m_pfnCreateInstance_CommandPlugin;

   //Command Instance
   catch_msg PFN_DESTROY_COMMANDINSTANCE     m_pfnDestroy_CommandInstance;
   catch_msg PFN_VALIDATE_COMMANDINSTANCE    m_pfnValidate_CommandInstance;
   catch_msg PFN_SUPPORTUNDO_COMMANDINSTANCE m_pfnSupportUndo_CommandInstance;
   catch_msg PFN_DO_COMMANDINSTANCE          m_pfnDo_CommandInstance;
   catch_msg PFN_REDO_COMMANDINSTANCE        m_pfnRedo_CommandInstance;
   catch_msg PFN_UNDO_COMMANDINSTANCE        m_pfnUndo_CommandInstance;

   union {
      ULONG    m_nCommandPluginFlags;
      struct {
         ULONG m_bValid      : 1, // All dependences have been found
               m_bValidating : 1; // To handle circular dependencies.
      };
   };

public:
   CommandPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData ) : 
    Plugin(pSnipePlugin, pPluginData, pfnDestroy_PluginData ),
    m_pfnInit_CommandPlugin(NULL),
    m_pfnCreateInstance_CommandPlugin(NULL),
    m_pfnDestroy_CommandInstance(NULL),
    m_pfnValidate_CommandInstance(NULL),
    m_pfnSupportUndo_CommandInstance(NULL),
    m_pfnDo_CommandInstance(NULL),
    m_pfnRedo_CommandInstance(NULL),
    m_pfnUndo_CommandInstance(NULL),
    m_nCommandPluginFlags(0)
   {
   }

   virtual ~CommandPlugin();

   void SetCLIName( const String &strCLIName ) { m_strCLIName = strCLIName; }
   const String &GetCLIName() const { return m_strCLIName; }

   void SetHelp(const String &strHelp ) { m_strHelp = strHelp; }
   String GetHelp() const { return m_strHelp; }

   void SetInitCommandPluginCallback(PFN_INIT_COMMANDPLUGIN pfnInit) { m_pfnInit_CommandPlugin = pfnInit; }
   void SetCreateCommandInstanceCallback(PFN_CREATEINSTANCE_COMMANDPLUGIN pfnCreate) { m_pfnCreateInstance_CommandPlugin = pfnCreate; }
   void SetDestroyCommandInstanceCallback(PFN_DESTROY_COMMANDINSTANCE pfnDestroy) { m_pfnDestroy_CommandInstance = pfnDestroy; }
   void SetValidateCommandInstanceCallback(PFN_VALIDATE_COMMANDINSTANCE pfnValidate) { m_pfnValidate_CommandInstance = pfnValidate; }
   void SetSupportUndoCommandInstanceCallback(PFN_SUPPORTUNDO_COMMANDINSTANCE pfnSupportUndo) { m_pfnSupportUndo_CommandInstance = pfnSupportUndo; }
   void SetDoCommandInstanceCallback(PFN_DO_COMMANDINSTANCE pfnDo) { m_pfnDo_CommandInstance = pfnDo; }
   void SetRedoCommandInstanceCallback(PFN_REDO_COMMANDINSTANCE pfnRedo) { m_pfnRedo_CommandInstance = pfnRedo; }
   void SetUndoCommandInstanceCallback(PFN_UNDO_COMMANDINSTANCE pfnUndo) { m_pfnUndo_CommandInstance = pfnUndo; }

   PFN_INIT_COMMANDPLUGIN GetInitCommandPluginCallback() const { return m_pfnInit_CommandPlugin; }
   PFN_CREATEINSTANCE_COMMANDPLUGIN GetCreateCommandInstanceCallback() const { return m_pfnCreateInstance_CommandPlugin; }
   PFN_DESTROY_COMMANDINSTANCE GetDestroyCommandInstanceCallback() const { return m_pfnDestroy_CommandInstance; }
   PFN_VALIDATE_COMMANDINSTANCE GetValidateCommandInstanceCallback() const { return m_pfnValidate_CommandInstance; }
   PFN_SUPPORTUNDO_COMMANDINSTANCE GetSupportUndoCommandInstanceCallback() const { return m_pfnSupportUndo_CommandInstance; }
   PFN_DO_COMMANDINSTANCE GetDoCommandInstanceCallback() const { return m_pfnDo_CommandInstance; }
   PFN_REDO_COMMANDINSTANCE GetRedoCommandInstanceCallback() const { return m_pfnRedo_CommandInstance; }
   PFN_UNDO_COMMANDINSTANCE GetUndoCommandInstanceCallback() const { return m_pfnUndo_CommandInstance; }

   BOOL IsValid() const { return m_bValid; } // This is validating dependencies only, don't mix up with the Validate call back
   void SetValid( BOOL bValid ) { m_bValid = bValid; }

   BOOL IsValidating() const { return m_bValidating; }
   void SetValidating( BOOL bValidating ) { m_bValidating = bValidating; }

   GLImageList *GetIconImageList() { return &m_glImageListIcon; }
   catch_msg void Validate();

   String GetCommandInfo( const String &strParameters, BOOL bLocalizedName= TRUE, BOOL bAbbreviated=TRUE ) const;

   // Plugin Overrides
   virtual String GetClassType() const;
};