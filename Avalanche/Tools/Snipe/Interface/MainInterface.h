#pragma once

#include "Plugin/PluginManagers.h"
#include "SnipeObject.h"
#include "CommandUIManager.h"
#include "SnipeManipulatorManager.h"
#include "SnipeNavigatorManager.h"
#include "SelectionList.h"
#include "CommandList.h"

class SnipeCommand;
class Project;
class HierObject;
class ModelTranslatorPluginManager;
class CommandUIPluginManager;
class CommandPluginManager;
class ManipulatorPluginManager;
class SnipeManipulator;
class SnipeNavigator;
class NavigatorPluginManager;
class SceneTranslatorPluginManager;
class MotionTranslatorPluginManager;
class MotionSetTranslatorPluginManager;

enum PluginType {
 COMMAND_PLUGIN, // Must be before Manipulator for Init
 MANIPULATOR_PLUGIN, // Must be before CommandUI for Init
 NAVIGATOR_PLUGIN, // Must be before CommandUI for Init
 COMMANDUI_PLUGIN,
 MODELTRANSLATOR_PLUGIN,
 SCENETRANSLATOR_PLUGIN,
 MOTIONTRANSLATOR_PLUGIN,
 MOTIONSETTRANSLATOR_PLUGIN,
 NUM_PLUGINTYPES
};

#define DECLARE_COMMANDOBJECT() \
   private: \
      SelectionList m_selectionlist; \
      CommandList   m_commandlist; \
      SelectionContainer *m_pSelectionContainer; \
   public: \
      virtual SelectionList *GetSelectionList() { return &m_selectionlist; } \
      virtual CommandList *GetCommandList() { return &m_commandlist; } \
      virtual BOOL IsCommandObject()  { return TRUE; } \
      virtual SelectionContainer *CreateSelectionContainer(); \
      virtual SelectionContainer *GetSelectionContainer() { return m_pSelectionContainer; } \
      virtual void SetSelectionContainer( SelectionContainer *pSelectionContainer ) { m_pSelectionContainer = pSelectionContainer; } \
      virtual void PreDestruct();// Any Object that uses this Macro must put a m_commandlist.Empty() in there PreDestruct 

class MainInterface : public SnipeObject
{
private:
   PluginManagers m_pluginmanagers;
   CommandUIManager m_commanduimanager;
   SnipeManipulatorManager m_manipulatormanager;
   SnipeNavigatorManager m_navigatormanager;

   Project *m_pProject;

   // State Variables
   SnipeObject *m_pCurrentWorkingObject;
   SnipeObject *m_pCurrentCommandObject;
   SelectionList *m_pCurrentSelectionList;
   CommandList *m_pCurrentCommandList;
   SnipeObjectType m_nCurrentSelectionSOT;
   SnipeNavigator *m_pCurrentNavigator;

public:
   static CGLBaseView *m_pCurrentView;

    MainInterface();
   ~MainInterface();

   void Init();

   // Don't mix this up with the plugin manager
   CommandUIManager *GetCommandUIManager() { return &m_commanduimanager; }
   SnipeManipulatorManager *GetSnipeManipulatorManager() { return &m_manipulatormanager; }
   SnipeNavigatorManager *GetSnipeNavigatorManager() { return &m_navigatormanager; }

   // Plugin Managers
   PluginManagers *GetPluginManagers() { return &m_pluginmanagers; }
 
   ModelTranslatorPluginManager *GetModelTranslatorPluginManager() { return (ModelTranslatorPluginManager *)m_pluginmanagers.GetAt(MODELTRANSLATOR_PLUGIN); }
   SceneTranslatorPluginManager *GetSceneTranslatorPluginManager() { return (SceneTranslatorPluginManager *)m_pluginmanagers.GetAt(SCENETRANSLATOR_PLUGIN); }
   MotionTranslatorPluginManager *GetMotionTranslatorPluginManager() { return (MotionTranslatorPluginManager *)m_pluginmanagers.GetAt(MOTIONTRANSLATOR_PLUGIN); }
   MotionSetTranslatorPluginManager *GetMotionSetTranslatorPluginManager() { return (MotionSetTranslatorPluginManager *)m_pluginmanagers.GetAt(MOTIONSETTRANSLATOR_PLUGIN); }

   CommandUIPluginManager *GetCommandUIPluginManager() { return (CommandUIPluginManager *) m_pluginmanagers.GetAt(COMMANDUI_PLUGIN); }
   CommandPluginManager *GetCommandPluginManager() { return (CommandPluginManager *) m_pluginmanagers.GetAt(COMMAND_PLUGIN); }
   ManipulatorPluginManager *GetManipulatorPluginManager() { return (ManipulatorPluginManager *) m_pluginmanagers.GetAt(MANIPULATOR_PLUGIN); }
   NavigatorPluginManager *GetNavigatorPluginManager() { return (NavigatorPluginManager *) m_pluginmanagers.GetAt(NAVIGATOR_PLUGIN); }

   Project *GetProject() { return m_pProject; }
   void SetProject( Project *pProject );
   Project *CreateProject();

   // State Managers
   static void SetCurrentView( CGLBaseView *pView );
   static CGLBaseView *GetCurrentView() { return m_pCurrentView; }

   void SetCurrentNavigator( SnipeNavigator *pNavigator );
   SnipeNavigator *GetCurrentNavigator() const { return m_pCurrentNavigator; }

   SnipeObject *SetCurrentWorkingObject( SnipeObject *pObject ) { SnipeObject *pPrevWorkingObject = m_pCurrentWorkingObject; m_pCurrentWorkingObject = pObject; return pPrevWorkingObject; }
   SnipeObject *GetCurrentWorkingObject() const { return m_pCurrentWorkingObject; }

   void SetCurrentCommandObject( SnipeObject *pObject ); 
   SnipeObject *GetCurrentCommandObject() const { return m_pCurrentCommandObject; }

   SelectionList *GetCurrentSelectionList()const { return m_pCurrentSelectionList; }
   CommandList *GetCurrentCommandList()const { return m_pCurrentCommandList; }

   void SetCurrentSelectionSOT( SnipeObjectType nSOT) { m_nCurrentSelectionSOT = nSOT; }
   SnipeObjectType GetCurrentSelectionSOT()const { return m_nCurrentSelectionSOT; }

   // Command Execution
   BOOL DoCommand(const String &strCommandName, const String &strParameters, StringList &strResults, BOOL bTempCommand=FALSE);
   BOOL DoCommand(CommandPlugin *pCommand, const String &strParameters, StringList &strResults, BOOL bTempCommand=FALSE);
   BOOL UndoCommand();
   BOOL KillLastCommand();// Just like UndoCommand except there can not be a redo
   BOOL RedoCommand();
   BOOL UndoTempCommand();
   BOOL CommitTempCommand();

   //Search
   SnipeObject     *DescriptionToObject(const String &strObject, String *pError=NULL); // Use | for hier, . for properties and components, ( ) for component ID's and : for ranges
   SnipeObjectList *DescriptionToObjects(const String &strObjects, StringList &strOutErrorList); // Use | for hier, . for properties and components, ( ) for component ID's and : for ranges
   BOOL ObjectToDescription( const SnipeObject *pMatchObject, String &strOutDescription, BOOL bRelativeToCurrentWorkingObject=TRUE );
   BOOL ObjectsToDescription( const SnipeObjectList *pObjectList, String &strOutDescription, BOOL bRelativeToCurrentWorkingObject=TRUE );
   BOOL ObjectToDescription( const SnipeObject *pMatchObject, MemoryStream &ms, BOOL bRelativeToCurrentWorkingObject=TRUE );
   BOOL ObjectsToDescription( const SnipeObjectList *pObjectList, MemoryStream &ms, BOOL bRelativeToCurrentWorkingObject=TRUE );

   SnipeObject *CreateObject( SnipeObjectType nSOT, SnipeObject::DeleteInfo **pDeleteInfo=NULL);

// SnipeObject overrides
   virtual const char *GetClassName() const { return "MainInterface"; }
   virtual String GetMatchName() const  { return "MainInterface"; }
};

MainInterface *GetMainInterface();
void DoSelectCommand( SnipeObject *pCommandObject, SnipeObjectList *pSelectionList );
float GetFPS();

extern RGBAFloat g_selectedcolor;
extern RGBAFloat g_selectedcolorlocked;
extern RGBAFloat g_focuscolor;
extern RGBAFloat g_focuscolorlocked;
extern RGBAFloat g_mirrorcolor;
extern RGBAFloat g_mirrorcolorlocked;
