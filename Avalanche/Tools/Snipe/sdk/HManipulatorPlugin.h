#pragma once
#include "HCommandUIPlugin.h"
#include "enum.h"
#include "KeyModifiers.h"
#include "MouseStates.h"

class HCommandPluginManager;
class HCGLBaseView;
class HCBaseDoc;
class HSelectionNode;
class HPropertyArray;
class HCMainFrame;
class CommandUIAccel;
class HCommandPlugin;
class HSelectionList;

// Plugin
typedef void *(* SDK_PFN_CREATEINSTANCE_MANIPULATORPLUGIN)( void *pPluginData );
typedef BOOL (* SDK_PFN_SUPPORTSOT_MANIPULATORPLUGIN)( void *pPluginData, SnipeObjectType nType );

// Instance
typedef void (* SDK_PFN_DESTROY_MANIPULATORINSTANCE)( void *pPluginInstanceData );
typedef HPropertyArray *(* SDK_PFN_GETPROPERTYARRAY_MANIPULATORINSTANCE)( void *pPluginInstanceData );
typedef BOOL (* SDK_PFN_ONOBJECTNOTIFY_MANIPULATORINSTANCE)( void *pPluginInstanceData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
typedef const char *(* SDK_PFN_GETSTATUSBARINSTRUCTIONS_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView );
typedef KeyModifiers (* SDK_PFN_GETKEYMODIFIERS_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, HCGLBaseView *pView, MouseStates mousestate );
typedef void (* SDK_PFN_DRAWPICK_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, HCGLBaseView *pView );
typedef void (* SDK_PFN_PREDRAW_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, HCGLBaseView *pView );
typedef void (* SDK_PFN_POSTDRAW_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, HCGLBaseView *pView );
//typedef void (* SDK_PFN_ONSELECTIONCHANGED_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, HSelectionList *pSelectionList, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONLBUTTONDOWN_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONLBUTTONDBLCLK_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONLBUTTONUP_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONRBUTTONDOWN_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONRBUTTONDBLCLK_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONRBUTTONUP_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONMBUTTONDOWN_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONMBUTTONDBLCLK_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONMBUTTONUP_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONMOUSEMOVE_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONMOUSEWHEEL_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, short zDelta, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONSETCURSOR_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef UINT (* SDK_PFN_GETTOOLTIPTEXT_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, CPoint point, String &strText, const HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONAPPLY_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONCANCEL_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_ONMBUTTONUP_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_OnArrowKeyDown_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, UINT nChar, UINT nRepCnt, HCGLBaseView *pView );
typedef BOOL (* SDK_PFN_OnArrowKeyUp_MANIPULATORINSTANCE ) ( void *pPluginInstanceData, UINT nChar, UINT nRepCnt, HCGLBaseView *pView );

class SNIPEEXPORT HManipulatorPlugin : public HCommandUIPlugin
{
public:
   void SetCreateInstanceCallback(SDK_PFN_CREATEINSTANCE_MANIPULATORPLUGIN pfnCreateInstance);
   void SetSupportSOTCallback(SDK_PFN_SUPPORTSOT_MANIPULATORPLUGIN pfnSupportSOT);
   
   void SetDestroyInstanceCallback(SDK_PFN_DESTROY_MANIPULATORINSTANCE pfnDestroy);
   void SetGetPropertyArrayInstanceCallback(SDK_PFN_GETPROPERTYARRAY_MANIPULATORINSTANCE pfnGetPropertyArray);
   void SetOnObjectNotifyInstanceCallback(SDK_PFN_ONOBJECTNOTIFY_MANIPULATORINSTANCE pfnOnObjectNotify);
   void SetGetStatusbarInstructionsCallback(SDK_PFN_GETSTATUSBARINSTRUCTIONS_MANIPULATORINSTANCE pfnGetStatusbarInstructions);
   void SetGetKeyModifiersCallback(SDK_PFN_GETKEYMODIFIERS_MANIPULATORINSTANCE pfnGetKeyModifiers);
   void SetDrawPickCallback(SDK_PFN_PREDRAW_MANIPULATORINSTANCE pfnDrawPick);
   void SetPreDrawCallback(SDK_PFN_PREDRAW_MANIPULATORINSTANCE pfnPreDraw);
   void SetPostDrawCallback(SDK_PFN_POSTDRAW_MANIPULATORINSTANCE pfnPostDraw);
   void SetOnLButtonDownCallback(SDK_PFN_ONLBUTTONDOWN_MANIPULATORINSTANCE pfnOnLButtonDown);
   void SetOnLButtonDblClkCallback(SDK_PFN_ONLBUTTONDBLCLK_MANIPULATORINSTANCE pfnOnLButtonDblClk);
   void SetOnLButtonUpCallback(SDK_PFN_ONLBUTTONUP_MANIPULATORINSTANCE pfnOnLButtonUp);
   void SetOnMButtonDownCallback(SDK_PFN_ONMBUTTONDOWN_MANIPULATORINSTANCE pfnOnMButtonDown);
   void SetOnMButtonDblClkCallback(SDK_PFN_ONMBUTTONDBLCLK_MANIPULATORINSTANCE pfnOnMButtonDblClk);
   void SetOnMButtonUpCallback(SDK_PFN_ONMBUTTONUP_MANIPULATORINSTANCE pfnOnMButtonUp); 
   void SetOnRButtonDownCallback(SDK_PFN_ONRBUTTONDOWN_MANIPULATORINSTANCE pfnOnRButtonDown);
   void SetOnRButtonDblClkCallback(SDK_PFN_ONRBUTTONDBLCLK_MANIPULATORINSTANCE pfnOnRButtonDblClk);
   void SetOnRButtonUpCallback(SDK_PFN_ONRBUTTONUP_MANIPULATORINSTANCE pfnOnRButtonUp);
   void SetOnMouseMoveCallback(SDK_PFN_ONMOUSEMOVE_MANIPULATORINSTANCE pfnOnMouseMove);
   void SetOnMouseWheelCallback(SDK_PFN_ONMOUSEWHEEL_MANIPULATORINSTANCE pfnOnMouseWheel);
   void SetOnSetCursorCallback(SDK_PFN_ONSETCURSOR_MANIPULATORINSTANCE pfnOnSetCursor);
   void SetGetToolTipTextCallback(SDK_PFN_GETTOOLTIPTEXT_MANIPULATORINSTANCE pfnGetToolTipText);
   void SetOnApplyCallback(SDK_PFN_ONAPPLY_MANIPULATORINSTANCE pfnOnApply);
   void SetOnCancelCallback(SDK_PFN_ONCANCEL_MANIPULATORINSTANCE pfnOnCancel);
   void SetOnArrowKeyDownCallback(SDK_PFN_OnArrowKeyDown_MANIPULATORINSTANCE pfnOnArrowKeyDown);
   void SetOnArrowKeyUpCallback(SDK_PFN_OnArrowKeyUp_MANIPULATORINSTANCE pfnOnArrowKeyUp);
};