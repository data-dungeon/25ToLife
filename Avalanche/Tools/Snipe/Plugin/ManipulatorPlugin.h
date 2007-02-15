#pragma once
#include "CommandUIPlugin.h"
#include "KeyModifiers.h"
#include "MouseStates.h"

class CommandPlugin;

// Plugin, called per Doc
typedef void *(* PFN_CREATEINSTANCE_MANIPULATORPLUGIN)( void *PluginData ); // Return parameter is the PluginInstanceData
typedef BOOL  (* PFN_SUPPORTSOT_MANIPULATORPLUGIN)( void *pPluginData, SnipeObjectType nType );

// Plugin Instance
typedef void (* PFN_DESTROY_MANIPULATORINSTANCE)( void *pPluginInstanceData );
typedef PropertyArray *(* PFN_GETPROPERTYARRAY_MANIPULATORINSTANCE)( void *pPluginInstanceData );
typedef BOOL  (* PFN_ONOBJECTNOTIFY_MANIPULATORINSTANCE)( void *pPluginInstanceData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
typedef const char *(* PFN_GETSTATUSBARINSTRUCTIONS_MANIPULATORINSTANCE)( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CGLBaseView *pView );
typedef KeyModifiers (* PFN_GETKEYMODIFIERS_MANIPULATORINSTANCE)( void *pPluginInstanceData, CGLBaseView *pView, MouseStates mousestate );
typedef void  (* PFN_DRAWPICK_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, const CGLBaseView *pView );
typedef void  (* PFN_PREDRAW_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, CGLBaseView *pView );
typedef void  (* PFN_POSTDRAW_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, CGLBaseView *pView );
//typedef void  (* PFN_ONSELECTIONCHANGED_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, SelectionList *pSelectionList, CGLBaseView *pView );
typedef BOOL  (* PFN_ONLBUTTONDOWN_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONLBUTTONDBLCLK_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONLBUTTONUP_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONRBUTTONDOWN_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONRBUTTONDBLCLK_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONRBUTTONUP_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONMBUTTONDOWN_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONMBUTTONDBLCLK_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONMBUTTONUP_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONMOUSEMOVE_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONMOUSEWHEEL_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, short zDelta, CPoint point, CGLBaseView *pView );
typedef BOOL  (* PFN_ONSETCURSOR_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
typedef UINT  (* PFN_GETTOOLTIPTEXT_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, CPoint point, String &strText, const CGLBaseView *pView );
typedef BOOL  (* PFN_ONAPPLY_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, CGLBaseView *pView );
typedef BOOL  (* PFN_ONCANCEL_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, CGLBaseView *pView );
typedef BOOL  (* PFN_OnArrowKeyDown_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, UINT nChar, UINT nRepCnt, CGLBaseView *pView );
typedef BOOL  (* PFN_OnArrowKeyUp_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, UINT nChar, UINT nRepCnt, CGLBaseView *pView );

class ManipulatorPlugin : public CommandUIPlugin
{
private:
// Plugin
   catch_msg PFN_CREATEINSTANCE_MANIPULATORPLUGIN m_pfnCreateInstance_ManipulatorPlugin;
   catch_msg PFN_SUPPORTSOT_MANIPULATORPLUGIN m_pfnSupportSOT_ManipulatorPlugin;

// Instance
   catch_msg PFN_DESTROY_MANIPULATORINSTANCE m_pfnDestroy_ManipulatorInstance;
   catch_msg PFN_GETPROPERTYARRAY_MANIPULATORINSTANCE m_pfnGetPropertyArray_ManipulatorInstance;
   catch_msg PFN_ONOBJECTNOTIFY_MANIPULATORINSTANCE m_pfnOnObjectNotify_ManipulatorInstance;
   catch_msg PFN_GETSTATUSBARINSTRUCTIONS_MANIPULATORINSTANCE m_pfnGetStatusbarInstructions_ManipulatorInstance;
   catch_msg PFN_GETKEYMODIFIERS_MANIPULATORINSTANCE m_pfnGetKeyModifiers_ManipulatorInstance;
   catch_msg PFN_DRAWPICK_MANIPULATORINSTANCE m_pfnDrawPick_ManipulatorInstance;
   catch_msg PFN_PREDRAW_MANIPULATORINSTANCE m_pfnPreDraw_ManipulatorInstance;
   catch_msg PFN_POSTDRAW_MANIPULATORINSTANCE m_pfnPostDraw_ManipulatorInstance;
   catch_msg PFN_ONLBUTTONDOWN_MANIPULATORINSTANCE m_pfnOnLButtonDown_ManipulatorInstance;
   catch_msg PFN_ONLBUTTONDBLCLK_MANIPULATORINSTANCE m_pfnOnLButtonDblClk_ManipulatorInstance;
   catch_msg PFN_ONLBUTTONUP_MANIPULATORINSTANCE m_pfnOnLButtonUp_ManipulatorInstance;
   catch_msg PFN_ONRBUTTONDOWN_MANIPULATORINSTANCE m_pfnOnRButtonDown_ManipulatorInstance;
   catch_msg PFN_ONRBUTTONDBLCLK_MANIPULATORINSTANCE m_pfnOnRButtonDblClk_ManipulatorInstance;
   catch_msg PFN_ONRBUTTONUP_MANIPULATORINSTANCE m_pfnOnRButtonUp_ManipulatorInstance;
   catch_msg PFN_ONMBUTTONDOWN_MANIPULATORINSTANCE m_pfnOnMButtonDown_ManipulatorInstance;
   catch_msg PFN_ONMBUTTONDBLCLK_MANIPULATORINSTANCE m_pfnOnMButtonDblClk_ManipulatorInstance;
   catch_msg PFN_ONMBUTTONUP_MANIPULATORINSTANCE m_pfnOnMButtonUp_ManipulatorInstance;
   catch_msg PFN_ONMOUSEMOVE_MANIPULATORINSTANCE m_pfnOnMouseMove_ManipulatorInstance;
   catch_msg PFN_ONMOUSEWHEEL_MANIPULATORINSTANCE m_pfnOnMouseWheel_ManipulatorInstance;
   catch_msg PFN_ONSETCURSOR_MANIPULATORINSTANCE m_pfnOnSetCursor_ManipulatorInstance;
   catch_msg PFN_GETTOOLTIPTEXT_MANIPULATORINSTANCE m_pfnGetToolTipText_ManipulatorInstance;
   catch_msg PFN_ONAPPLY_MANIPULATORINSTANCE m_pfnOnApply_ManipulatorInstance;
   catch_msg PFN_ONCANCEL_MANIPULATORINSTANCE m_pfnOnCancel_ManipulatorInstance;
   catch_msg PFN_OnArrowKeyDown_MANIPULATORINSTANCE m_pfnOnArrowKeyDown_ManipulatorInstance;
   catch_msg PFN_OnArrowKeyUp_MANIPULATORINSTANCE m_pfnOnArrowKeyUp_ManipulatorInstance;

public:

   ManipulatorPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData );

   ~ManipulatorPlugin()
   {
   }

   void SetCreateInstanceCallback(PFN_CREATEINSTANCE_MANIPULATORPLUGIN pfnCreateInstance_ManipulatorPlugin) { m_pfnCreateInstance_ManipulatorPlugin = pfnCreateInstance_ManipulatorPlugin; }
   void SetSupportSOTCallback(PFN_SUPPORTSOT_MANIPULATORPLUGIN pfnSupportSOT_ManipulatorPlugin) { m_pfnSupportSOT_ManipulatorPlugin = pfnSupportSOT_ManipulatorPlugin; }
   
   void SetDestroyInstanceCallback(PFN_DESTROY_MANIPULATORINSTANCE pfnDestroy_ManipulatorInstance) { m_pfnDestroy_ManipulatorInstance = pfnDestroy_ManipulatorInstance; }
   void SetGetPropertyArrayInstanceCallback(PFN_GETPROPERTYARRAY_MANIPULATORINSTANCE pfnGetPropertyArray_ManipulatorInstance) { m_pfnGetPropertyArray_ManipulatorInstance = pfnGetPropertyArray_ManipulatorInstance; }
   void SetOnObjectNotifyInstanceCallback(PFN_ONOBJECTNOTIFY_MANIPULATORINSTANCE pfnOnObjectNotify_ManipulatorInstance) { m_pfnOnObjectNotify_ManipulatorInstance = pfnOnObjectNotify_ManipulatorInstance; }
   void SetGetStatusbarInstructionsCallback(PFN_GETSTATUSBARINSTRUCTIONS_MANIPULATORINSTANCE pfnGetStatusbarInstructions_ManipulatorInstance) { m_pfnGetStatusbarInstructions_ManipulatorInstance = pfnGetStatusbarInstructions_ManipulatorInstance; }
   void SetGetKeyModifiersCallback(PFN_GETKEYMODIFIERS_MANIPULATORINSTANCE pfnGetKeyModifiers_ManipulatorInstance) { m_pfnGetKeyModifiers_ManipulatorInstance = pfnGetKeyModifiers_ManipulatorInstance; }
   void SetDrawPickCallback(PFN_DRAWPICK_MANIPULATORINSTANCE pfnDrawPick_ManipulatorInstance) { m_pfnDrawPick_ManipulatorInstance = pfnDrawPick_ManipulatorInstance; }
   void SetPreDrawCallback(PFN_PREDRAW_MANIPULATORINSTANCE pfnPreDraw_ManipulatorInstance) { m_pfnPreDraw_ManipulatorInstance = pfnPreDraw_ManipulatorInstance; }
   void SetPostDrawCallback(PFN_POSTDRAW_MANIPULATORINSTANCE pfnPostDraw_ManipulatorInstance) { m_pfnPostDraw_ManipulatorInstance = pfnPostDraw_ManipulatorInstance; }
   void SetOnLButtonDownCallback(PFN_ONLBUTTONDOWN_MANIPULATORINSTANCE pfnOnLButtonDown_ManipulatorInstance) { m_pfnOnLButtonDown_ManipulatorInstance = pfnOnLButtonDown_ManipulatorInstance; }
   void SetOnLButtonDblClkCallback(PFN_ONLBUTTONDBLCLK_MANIPULATORINSTANCE pfnOnLButtonDblClk_ManipulatorInstance) { m_pfnOnLButtonDblClk_ManipulatorInstance = pfnOnLButtonDblClk_ManipulatorInstance; }
   void SetOnLButtonUpCallback(PFN_ONLBUTTONUP_MANIPULATORINSTANCE pfnOnLButtonUp_ManipulatorInstance) { m_pfnOnLButtonUp_ManipulatorInstance = pfnOnLButtonUp_ManipulatorInstance; }
   void SetOnRButtonDownCallback(PFN_ONRBUTTONDOWN_MANIPULATORINSTANCE pfnOnRButtonDown_ManipulatorInstance) { m_pfnOnRButtonDown_ManipulatorInstance = pfnOnRButtonDown_ManipulatorInstance; }
   void SetOnRButtonDblClkCallback(PFN_ONRBUTTONDBLCLK_MANIPULATORINSTANCE pfnOnRButtonDblClk_ManipulatorInstance) { m_pfnOnRButtonDblClk_ManipulatorInstance = pfnOnRButtonDblClk_ManipulatorInstance; }
   void SetOnRButtonUpCallback(PFN_ONRBUTTONUP_MANIPULATORINSTANCE pfnOnRButtonUp_ManipulatorInstance) { m_pfnOnRButtonUp_ManipulatorInstance = pfnOnRButtonUp_ManipulatorInstance; }
   void SetOnMButtonDownCallback(PFN_ONMBUTTONDOWN_MANIPULATORINSTANCE pfnOnMButtonDown_ManipulatorInstance) { m_pfnOnMButtonDown_ManipulatorInstance = pfnOnMButtonDown_ManipulatorInstance; }
   void SetOnMButtonDblClkCallback(PFN_ONMBUTTONDBLCLK_MANIPULATORINSTANCE pfnOnMButtonDblClk_ManipulatorInstance) { m_pfnOnMButtonDblClk_ManipulatorInstance = pfnOnMButtonDblClk_ManipulatorInstance; }
   void SetOnMButtonUpCallback(PFN_ONMBUTTONUP_MANIPULATORINSTANCE pfnOnMButtonUp_ManipulatorInstance) { m_pfnOnMButtonUp_ManipulatorInstance = pfnOnMButtonUp_ManipulatorInstance; }
   void SetOnMouseMoveCallback(PFN_ONMOUSEMOVE_MANIPULATORINSTANCE pfnOnMouseMove_ManipulatorInstance) { m_pfnOnMouseMove_ManipulatorInstance = pfnOnMouseMove_ManipulatorInstance; }
   void SetOnMouseWheelCallback(PFN_ONMOUSEWHEEL_MANIPULATORINSTANCE pfnOnMouseWheel_ManipulatorInstance) { m_pfnOnMouseWheel_ManipulatorInstance = pfnOnMouseWheel_ManipulatorInstance; }
   void SetOnSetCursorCallback(PFN_ONSETCURSOR_MANIPULATORINSTANCE pfnOnSetCursor_ManipulatorInstance) { m_pfnOnSetCursor_ManipulatorInstance = pfnOnSetCursor_ManipulatorInstance; }
   void SetGetToolTipTextCallback(PFN_GETTOOLTIPTEXT_MANIPULATORINSTANCE pfnGetToolTipText_ManipulatorInstance) { m_pfnGetToolTipText_ManipulatorInstance = pfnGetToolTipText_ManipulatorInstance; }
   void SetOnApplyCallback(PFN_ONAPPLY_MANIPULATORINSTANCE pfnOnApply_ManipulatorInstance) { m_pfnOnApply_ManipulatorInstance = pfnOnApply_ManipulatorInstance; }
   void SetOnCancelCallback(PFN_ONCANCEL_MANIPULATORINSTANCE pfnOnCancel_ManipulatorInstance) { m_pfnOnCancel_ManipulatorInstance = pfnOnCancel_ManipulatorInstance; }
   void SetOnArrowKeyDownCallback(PFN_OnArrowKeyDown_MANIPULATORINSTANCE pfnOnArrowKeyDown_ManipulatorInstance) { m_pfnOnArrowKeyDown_ManipulatorInstance = pfnOnArrowKeyDown_ManipulatorInstance; }
   void SetOnArrowKeyUpCallback(PFN_OnArrowKeyUp_MANIPULATORINSTANCE pfnOnArrowKeyUp_ManipulatorInstance) { m_pfnOnArrowKeyUp_ManipulatorInstance = pfnOnArrowKeyUp_ManipulatorInstance; }

   PFN_CREATEINSTANCE_MANIPULATORPLUGIN GetCreateInstanceCallback() const { return m_pfnCreateInstance_ManipulatorPlugin; }
   PFN_SUPPORTSOT_MANIPULATORPLUGIN GetSupportSOTCallback() const { return m_pfnSupportSOT_ManipulatorPlugin; }

   PFN_DESTROY_MANIPULATORINSTANCE GetDestroyInstanceCallback() const { return m_pfnDestroy_ManipulatorInstance; }
   PFN_GETPROPERTYARRAY_MANIPULATORINSTANCE GetGetPropertyArrayInstanceCallback() const { return m_pfnGetPropertyArray_ManipulatorInstance; }
   PFN_ONOBJECTNOTIFY_MANIPULATORINSTANCE GetOnObjectNotifyInstanceCallback() const { return m_pfnOnObjectNotify_ManipulatorInstance; }
   PFN_GETSTATUSBARINSTRUCTIONS_MANIPULATORINSTANCE GetGetStatusbarInstructionsCallback() const { return m_pfnGetStatusbarInstructions_ManipulatorInstance; }
   PFN_GETKEYMODIFIERS_MANIPULATORINSTANCE GetGetKeyModifiersCallback() const { return m_pfnGetKeyModifiers_ManipulatorInstance; }
   PFN_DRAWPICK_MANIPULATORINSTANCE GetDrawPickCallback() const { return m_pfnDrawPick_ManipulatorInstance; }
   PFN_PREDRAW_MANIPULATORINSTANCE GetPreDrawCallback() const { return m_pfnPreDraw_ManipulatorInstance; }
   PFN_POSTDRAW_MANIPULATORINSTANCE GetPostDrawCallback() const { return m_pfnPostDraw_ManipulatorInstance; }
   PFN_ONLBUTTONDOWN_MANIPULATORINSTANCE GetOnLButtonDownCallback() const { return m_pfnOnLButtonDown_ManipulatorInstance; }
   PFN_ONLBUTTONDBLCLK_MANIPULATORINSTANCE GetOnLButtonDblClkCallback() const { return m_pfnOnLButtonDblClk_ManipulatorInstance; }
   PFN_ONLBUTTONUP_MANIPULATORINSTANCE GetOnLButtonUpCallback() const { return m_pfnOnLButtonUp_ManipulatorInstance; }
   PFN_ONRBUTTONDOWN_MANIPULATORINSTANCE GetOnRButtonDownCallback() const { return m_pfnOnRButtonDown_ManipulatorInstance; }
   PFN_ONRBUTTONDBLCLK_MANIPULATORINSTANCE GetOnRButtonDblClkCallback() const { return m_pfnOnRButtonDblClk_ManipulatorInstance; }
   PFN_ONRBUTTONUP_MANIPULATORINSTANCE GetOnRButtonUpCallback() const { return m_pfnOnRButtonUp_ManipulatorInstance; }
   PFN_ONMBUTTONDOWN_MANIPULATORINSTANCE GetOnMButtonDownCallback() const { return m_pfnOnMButtonDown_ManipulatorInstance; }
   PFN_ONMBUTTONDBLCLK_MANIPULATORINSTANCE GetOnMButtonDblClkCallback() const { return m_pfnOnMButtonDblClk_ManipulatorInstance; }
   PFN_ONMBUTTONUP_MANIPULATORINSTANCE GetOnMButtonUpCallback() const { return m_pfnOnMButtonUp_ManipulatorInstance; }
   PFN_ONMOUSEMOVE_MANIPULATORINSTANCE GetOnMouseMoveCallback() const { return m_pfnOnMouseMove_ManipulatorInstance; }
   PFN_ONMOUSEWHEEL_MANIPULATORINSTANCE GetOnMouseWheelCallback() const { return m_pfnOnMouseWheel_ManipulatorInstance; }
   PFN_ONSETCURSOR_MANIPULATORINSTANCE GetOnSetCursorCallback() const { return m_pfnOnSetCursor_ManipulatorInstance; }
   PFN_GETTOOLTIPTEXT_MANIPULATORINSTANCE GetGetToolTipTextCallback() const { return m_pfnGetToolTipText_ManipulatorInstance; }
   PFN_ONAPPLY_MANIPULATORINSTANCE GetOnApplyCallback() const { return m_pfnOnApply_ManipulatorInstance; }
   PFN_ONCANCEL_MANIPULATORINSTANCE GetOnCancelCallback() const { return m_pfnOnCancel_ManipulatorInstance; }
   PFN_OnArrowKeyDown_MANIPULATORINSTANCE GetOnArrowKeyDownCallback() const { return m_pfnOnArrowKeyDown_ManipulatorInstance; }
   PFN_OnArrowKeyUp_MANIPULATORINSTANCE GetOnArrowKeyUpCallback() const { return m_pfnOnArrowKeyUp_ManipulatorInstance; }

//CommandUIPlugin Overrides
   catch_msg virtual void Validate();

//Plugin Overrides
   virtual String GetClassType() const;
};
