#pragma once
#include "Plugin.h"
#include "../Interface/CommandUI.h"
#include "SDK/Enum.h"

class SnipePlugin;
class SnipeObject;
class CommandPlugin;
class CommandPluginManager;
class CMainFrame;
class CBaseDoc;
class CGLBaseView;
class PropertyArray;

typedef void (* PFN_INIT_COMMANDUIPLUGIN)( void *pPluginData, CommandPluginManager *pManager );
typedef UINT (* PFN_GETBITMAPID_COMMANDUIPLUGIN)( void *pPluginData );
typedef void (* PFN_PLACE_COMMANDUIPLUGIN) (void *pPluginData, CMainFrame *pMainFrame);

typedef BOOL (* PFN_ONOBJECTNOTIFY_COMMANDUIPLUGIN) (void *pPluginData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam);

typedef void (* PFN_ON_COMMAND_OBJECT) (void *pPluginData, UINT nCmdID, const SnipeObject *, CommandPlugin *&pPlugin, const String &strParameters);
typedef void (* PFN_ON_UPDATE_COMMAND_UI_OBJECT) (void *pPluginData, const SnipeObject *, CCmdUI *);

typedef void (* PFN_ON_COMMAND_VIEW) (void *pPluginData, UINT nCmdID, CGLBaseView *, CommandPlugin *&pPlugin, const String &strParameters);
typedef void (* PFN_ON_UPDATE_COMMAND_UI_VIEW) (void *pPluginData, const CGLBaseView *, CCmdUI *);

typedef void (* PFN_ON_COMMAND_DOC) (void *pPluginData, UINT nCmdID, CBaseDoc *, CommandPlugin *&pPlugin, const String &strParameters);
typedef void (* PFN_ON_UPDATE_COMMAND_UI_DOC) (void *pPluginData, const CBaseDoc *, CCmdUI *);

typedef void (* PFN_ON_COMMAND_MAINFRAME) (void *pPluginData, UINT nCmdID, CMainFrame *, CommandPlugin *&pPlugin, const String &strParameters);
typedef void (* PFN_ON_UPDATE_COMMAND_UI_MAINFRAME) (void *pPluginData, const CMainFrame *, CCmdUI *);

class CommandUIPlugin : public Plugin, public CommandUI // Plugin must come first for Plugins to get correct address
{
protected:
   catch_msg PFN_INIT_COMMANDUIPLUGIN m_pfnInit; // Called once usually to collect CommandPlugin pointers
   catch_msg PFN_GETBITMAPID_COMMANDUIPLUGIN m_pfnGetBitmapID; 
   catch_msg PFN_PLACE_COMMANDUIPLUGIN m_pfnPlace; // Called once to place command in UI only if command is new
   catch_msg PFN_ONOBJECTNOTIFY_COMMANDUIPLUGIN m_pfnOnObjectNotify;

   catch_msg PFN_ON_COMMAND_OBJECT m_pfnOnCommandObject;
   catch_msg PFN_ON_UPDATE_COMMAND_UI_OBJECT m_pfnOnUpdateCommandObject;
             SnipeObjectType m_nObjectType;

   catch_msg PFN_ON_COMMAND_VIEW m_pfnOnCommandView;
   catch_msg PFN_ON_UPDATE_COMMAND_UI_VIEW m_pfnOnUpdateCommandView;
             SnipeViewType m_nViewType;

   catch_msg PFN_ON_COMMAND_DOC m_pfnOnCommandDoc;
   catch_msg PFN_ON_UPDATE_COMMAND_UI_DOC m_pfnOnUpdateCommandDoc;
             SnipeDocType m_nDocType;

   catch_msg PFN_ON_COMMAND_MAINFRAME m_pfnOnCommandMainFrame;
   catch_msg PFN_ON_UPDATE_COMMAND_UI_MAINFRAME m_pfnOnUpdateCommandMainFrame;

public:
   UINT m_nCurrentBitmapID;

   CommandUIPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData ) : 
    Plugin(pSnipePlugin, pPluginData, pfnDestroy_PluginData)
   {
      m_nCurrentBitmapID = -1;
      m_pfnInit = NULL;
      m_pfnGetBitmapID = NULL;
      m_pfnPlace = NULL;
      m_pfnOnObjectNotify = NULL;
      m_pfnOnCommandObject = NULL;
      m_pfnOnUpdateCommandObject = NULL;
      m_nObjectType = SOT_Unknown; //-1;
      m_pfnOnCommandView = NULL;
      m_pfnOnUpdateCommandView = NULL;
      m_nViewType = SVT_Unknown;//(SnipeViewType)-1;
      m_pfnOnCommandDoc = NULL;
      m_pfnOnUpdateCommandDoc = NULL;
      m_nDocType = SDT_Unknown;//(SnipeDocType)-1;
      m_pfnOnCommandMainFrame = NULL;
      m_pfnOnUpdateCommandMainFrame = NULL;
   }

   ~CommandUIPlugin()
   {
   }

   //CommandUI members 
   virtual void SetTitleName( const String &strTitleName ) { Plugin::SetTitleName(strTitleName);  CommandUI::SetName(strTitleName); }
   void SetAccelerators( UINT nCount, CommandUIAccel *pAccel);
   
   void SetInitCallback(PFN_INIT_COMMANDUIPLUGIN pfnInit) { m_pfnInit = pfnInit; }
   void SetGetBitmapIDCallback(PFN_GETBITMAPID_COMMANDUIPLUGIN pfnGetBitmapID) { m_pfnGetBitmapID = pfnGetBitmapID; }
   void SetPlaceCallback(PFN_PLACE_COMMANDUIPLUGIN pfnPlace) { m_pfnPlace = pfnPlace; }
   void SetOnObjectNotifyCallback(PFN_ONOBJECTNOTIFY_COMMANDUIPLUGIN pfnOnObjectNotify) { m_pfnOnObjectNotify = pfnOnObjectNotify; }

   void SetMessageMapForObject(SnipeObjectType nObjectType, 
    PFN_ON_COMMAND_OBJECT pfnOnCommandObject, 
    PFN_ON_UPDATE_COMMAND_UI_OBJECT pfnOnUpdateCommandObject) 
   {
      m_nObjectType = nObjectType;
      m_pfnOnCommandObject = pfnOnCommandObject;
      m_pfnOnUpdateCommandObject = pfnOnUpdateCommandObject;
   }

   void SetMessageMapForView(SnipeViewType nViewType, 
    PFN_ON_COMMAND_VIEW pfnOnCommandView, 
    PFN_ON_UPDATE_COMMAND_UI_VIEW pfnOnUpdateCommandView) 
   {
      m_nViewType = nViewType;
      m_pfnOnCommandView = pfnOnCommandView;
      m_pfnOnUpdateCommandView = pfnOnUpdateCommandView;
   }

   void SetMessageMapForDoc(SnipeDocType nDocType, 
    PFN_ON_COMMAND_DOC pfnOnCommandDoc, 
    PFN_ON_UPDATE_COMMAND_UI_DOC pfnOnUpdateCommandDoc) 
   {
      m_nDocType = nDocType;
      m_pfnOnCommandDoc = pfnOnCommandDoc;
      m_pfnOnUpdateCommandDoc = pfnOnUpdateCommandDoc;
   }

   void SetMessageMapForMainFrame( PFN_ON_COMMAND_MAINFRAME pfnOnCommandMainFrame, 
    PFN_ON_UPDATE_COMMAND_UI_MAINFRAME pfnOnUpdateCommandMainFrame) 
   {
      m_pfnOnCommandMainFrame = pfnOnCommandMainFrame;
      m_pfnOnUpdateCommandMainFrame = pfnOnUpdateCommandMainFrame;
   }

   //CommandUI members 
   PFN_INIT_COMMANDUIPLUGIN GetInitCallback() const { return m_pfnInit; }
   PFN_GETBITMAPID_COMMANDUIPLUGIN GetGetBitmapIDCallback() const { return m_pfnGetBitmapID; }
   PFN_PLACE_COMMANDUIPLUGIN GetPlaceCallback() const { return m_pfnPlace; }
   PFN_ONOBJECTNOTIFY_COMMANDUIPLUGIN GetOnObjectNotifyCallback() const { return m_pfnOnObjectNotify; }

   SnipeObjectType GetObjectType() { return m_nObjectType; }
   PFN_ON_COMMAND_OBJECT GetOnCommandObjectCallback() { return m_pfnOnCommandObject; }
   PFN_ON_UPDATE_COMMAND_UI_OBJECT GetOnUpdateCommandObjectCallback() { return m_pfnOnUpdateCommandObject; }

   SnipeViewType GetViewType() { return m_nViewType; }
   PFN_ON_COMMAND_VIEW GetOnCommandViewCallback() { return m_pfnOnCommandView; }
   PFN_ON_UPDATE_COMMAND_UI_VIEW GetOnUpdateCommandViewCallback() { return m_pfnOnUpdateCommandView; }

   SnipeDocType GetDocType() { return m_nDocType; }
   PFN_ON_COMMAND_DOC GetOnCommandDocCallback() { return m_pfnOnCommandDoc; }
   PFN_ON_UPDATE_COMMAND_UI_DOC GetOnUpdateCommandDocCallback() { return m_pfnOnUpdateCommandDoc; }

   PFN_ON_COMMAND_MAINFRAME GetOnCommandMainFrameCallback() { return m_pfnOnCommandMainFrame; }
   PFN_ON_UPDATE_COMMAND_UI_MAINFRAME GetOnUpdateCommandMainFrameCallback() { return m_pfnOnUpdateCommandMainFrame; }

   void SetPropertyArray( PropertyArray *pPropertyArray );

// Overridables
   catch_msg virt_base virtual void Validate();

// Plugin overrides
   virtual String GetClassType() const;

// CommandUI overrides
   virtual BOOL CreateIcon();
   virtual BOOL IsAutoDelete() { return FALSE; }

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
