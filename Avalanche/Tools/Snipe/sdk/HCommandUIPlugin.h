#pragma once
#include "HPlugin.h"
#include "enum.h"

class HSnipeObject;
class HCommandPlugin;
class HCommandPluginManager;
class HCMainFrame;
class HCGLBaseView;
class HCBaseDoc;
class CommandUIAccel;
class HPropertyArray;

typedef void (* SDK_PFN_INIT_COMMANDUIPLUGIN)( void *pPluginData, HCommandPluginManager *pManager ); // Called once usually to collect CommandPlugin pointers
typedef UINT (* SDK_PFN_GETBITMAPID_COMMANDUIPLUGIN)( void *pPluginData );

typedef void (* SDK_PFN_PLACE_COMMANDUIPLUGIN)( void *pPluginData, HCMainFrame *pMainFrame ); // Called once to place command in UI only if command is new
typedef BOOL (* SDK_PFN_ONOBJECTNOTIFY_COMMANDUIPLUGIN) (void *pPluginData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam);

typedef void (* SDK_PFN_ON_COMMAND_OBJECT) (void *pPluginData, UINT nCmdID, const HSnipeObject *, HCommandPlugin *&pOutEntry, String &strOutParameters);
typedef void (* SDK_PFN_ON_UPDATE_COMMAND_UI_OBJECT) (void *pPluginData, const HSnipeObject *, CCmdUI *);

typedef void (* SDK_PFN_ON_COMMAND_VIEW) (void *pPluginData, UINT nCmdID, HCGLBaseView *, HCommandPlugin *&pOutEntry, String &strOutParameters);
typedef void (* SDK_PFN_ON_UPDATE_COMMAND_UI_VIEW) (void *pPluginData, const HCGLBaseView *, CCmdUI *);

typedef void (* SDK_PFN_ON_COMMAND_DOC) (void *pPluginData, UINT nCmdID, HCBaseDoc *, HCommandPlugin *&pOutEntry, String &strOutParameters);
typedef void (* SDK_PFN_ON_UPDATE_COMMAND_UI_DOC) (void *pPluginData, const HCBaseDoc *, CCmdUI *);

typedef void (* SDK_PFN_ON_COMMAND_MAINFRAME) (void *pPluginData, UINT nCmdID, HCMainFrame *, HCommandPlugin *&pOutEntry, String &strOutParameters);
typedef void (* SDK_PFN_ON_UPDATE_COMMAND_UI_MAINFRAME) (void *pPluginData, const HCMainFrame *, CCmdUI *);

class SNIPEEXPORT HCommandUIPlugin : public HPlugin
{
public:
   void SetDescription(const char *szStatusBarText);
   void SetCategory(const char *szCategory);
   void SetAccelerators( UINT nCount, CommandUIAccel *pAccel );
   void SetPropertyArray( HPropertyArray *pArray );

   void SetInitCommandUIPluginCallback(SDK_PFN_INIT_COMMANDUIPLUGIN pfnInit);
   void SetGetBitmapIDCallback(SDK_PFN_GETBITMAPID_COMMANDUIPLUGIN pfnGetBitmapID);
   void SetPlaceCallback(SDK_PFN_PLACE_COMMANDUIPLUGIN pfnPlace);
   void SetOnObjectNotifyCallback(SDK_PFN_ONOBJECTNOTIFY_COMMANDUIPLUGIN pfnOnObjectNotify);

   void SetMessageMapForObject(SnipeObjectType nObjectType, 
    SDK_PFN_ON_COMMAND_OBJECT lpfnOnCommandObject, 
    SDK_PFN_ON_UPDATE_COMMAND_UI_OBJECT lpfnOnUpdateCommandObject);

   void SetMessageMapForView(SnipeViewType nViewType, 
    SDK_PFN_ON_COMMAND_VIEW lpfnOnCommandView, 
    SDK_PFN_ON_UPDATE_COMMAND_UI_VIEW lpfnOnUpdateCommandView);

   void SetMessageMapForDoc(SnipeDocType nDocType, 
    SDK_PFN_ON_COMMAND_DOC lpfnOnCommandDoc, 
    SDK_PFN_ON_UPDATE_COMMAND_UI_DOC lpfnOnUpdateCommandDoc);

   void SetMessageMapForMainFrame( SDK_PFN_ON_COMMAND_MAINFRAME lpfnOnCommandMainFrame, 
    SDK_PFN_ON_UPDATE_COMMAND_UI_MAINFRAME lpfnOnUpdateCommandMainFrame);

   UINT GetCmdID() const;
   HPropertyArray *GetPropertyArray();
};