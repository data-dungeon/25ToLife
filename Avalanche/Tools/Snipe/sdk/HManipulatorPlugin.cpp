//
#include "stdafx.h"
#include "ManipulatorPlugin.h"
#include "HManipulatorPlugin.h"

// Plugin ////////////////////////////////////////////////////////////////////////////////////////
void HManipulatorPlugin::SetSupportSOTCallback(SDK_PFN_SUPPORTSOT_MANIPULATORPLUGIN pfnSupportSOT)
{
   ((ManipulatorPlugin *)this)->SetSupportSOTCallback( (PFN_SUPPORTSOT_MANIPULATORPLUGIN)pfnSupportSOT );
}

void HManipulatorPlugin::SetCreateInstanceCallback(SDK_PFN_CREATEINSTANCE_MANIPULATORPLUGIN pfnCreateInstance)
{
   ((ManipulatorPlugin *)this)->SetCreateInstanceCallback( (PFN_CREATEINSTANCE_MANIPULATORPLUGIN)pfnCreateInstance );
}

// Instance //////////////////////////////////////////////////////////////////////////////////////
void HManipulatorPlugin::SetDestroyInstanceCallback(SDK_PFN_DESTROY_MANIPULATORINSTANCE pfnDestroyInstance)
{
   ((ManipulatorPlugin *)this)->SetDestroyInstanceCallback( (PFN_DESTROY_MANIPULATORINSTANCE) pfnDestroyInstance );
}

void HManipulatorPlugin::SetGetPropertyArrayInstanceCallback(SDK_PFN_GETPROPERTYARRAY_MANIPULATORINSTANCE pfnGetPropertyArrayInstance)
{
   ((ManipulatorPlugin *)this)->SetGetPropertyArrayInstanceCallback( (PFN_GETPROPERTYARRAY_MANIPULATORINSTANCE) pfnGetPropertyArrayInstance );
}

void HManipulatorPlugin::SetOnObjectNotifyInstanceCallback(SDK_PFN_ONOBJECTNOTIFY_MANIPULATORINSTANCE pfnOnObjectNotifyInstance)
{
   ((ManipulatorPlugin *)this)->SetOnObjectNotifyInstanceCallback( (PFN_ONOBJECTNOTIFY_MANIPULATORINSTANCE) pfnOnObjectNotifyInstance );
}

void HManipulatorPlugin::SetGetStatusbarInstructionsCallback(SDK_PFN_GETSTATUSBARINSTRUCTIONS_MANIPULATORINSTANCE pfnGetStatusbarInstructions)
{
   ((ManipulatorPlugin *)this)->SetGetStatusbarInstructionsCallback( (PFN_GETSTATUSBARINSTRUCTIONS_MANIPULATORINSTANCE) pfnGetStatusbarInstructions );
}

void HManipulatorPlugin::SetGetKeyModifiersCallback(SDK_PFN_GETKEYMODIFIERS_MANIPULATORINSTANCE pfnGetKeyModifiers )
{
   ((ManipulatorPlugin *)this)->SetGetKeyModifiersCallback( (PFN_GETKEYMODIFIERS_MANIPULATORINSTANCE) pfnGetKeyModifiers );
}

void HManipulatorPlugin::SetDrawPickCallback(SDK_PFN_DRAWPICK_MANIPULATORINSTANCE pfnDrawPick)
{
   ((ManipulatorPlugin *)this)->SetDrawPickCallback( (PFN_DRAWPICK_MANIPULATORINSTANCE) pfnDrawPick );
}

void HManipulatorPlugin::SetPreDrawCallback(SDK_PFN_PREDRAW_MANIPULATORINSTANCE pfnPreDraw)
{
   ((ManipulatorPlugin *)this)->SetPreDrawCallback( (PFN_PREDRAW_MANIPULATORINSTANCE) pfnPreDraw );
}

void HManipulatorPlugin::SetPostDrawCallback(SDK_PFN_POSTDRAW_MANIPULATORINSTANCE pfnPostDraw)
{
   ((ManipulatorPlugin *)this)->SetPostDrawCallback( (PFN_POSTDRAW_MANIPULATORINSTANCE) pfnPostDraw );
}

void HManipulatorPlugin::SetOnLButtonDownCallback(SDK_PFN_ONLBUTTONDOWN_MANIPULATORINSTANCE pfnOnLButtonDown)
{
   ((ManipulatorPlugin *)this)->SetOnLButtonDownCallback( (PFN_ONLBUTTONDOWN_MANIPULATORINSTANCE) pfnOnLButtonDown );
}

void HManipulatorPlugin::SetOnLButtonDblClkCallback(SDK_PFN_ONLBUTTONDBLCLK_MANIPULATORINSTANCE pfnOnLButtonDblClk)
{
   ((ManipulatorPlugin *)this)->SetOnLButtonDblClkCallback( (PFN_ONLBUTTONDBLCLK_MANIPULATORINSTANCE) pfnOnLButtonDblClk );
}

void HManipulatorPlugin::SetOnLButtonUpCallback(SDK_PFN_ONLBUTTONUP_MANIPULATORINSTANCE pfnOnLButtonUp)
{
   ((ManipulatorPlugin *)this)->SetOnLButtonUpCallback( (PFN_ONLBUTTONUP_MANIPULATORINSTANCE) pfnOnLButtonUp );
}

void HManipulatorPlugin::SetOnMButtonDownCallback(SDK_PFN_ONMBUTTONDOWN_MANIPULATORINSTANCE pfnOnMButtonDown)
{
   ((ManipulatorPlugin *)this)->SetOnMButtonDownCallback( (PFN_ONMBUTTONDOWN_MANIPULATORINSTANCE) pfnOnMButtonDown );
}

void HManipulatorPlugin::SetOnMButtonDblClkCallback(SDK_PFN_ONMBUTTONDBLCLK_MANIPULATORINSTANCE pfnOnMButtonDblClk)
{
   ((ManipulatorPlugin *)this)->SetOnMButtonDblClkCallback( (PFN_ONMBUTTONDBLCLK_MANIPULATORINSTANCE) pfnOnMButtonDblClk );
}

void HManipulatorPlugin::SetOnMButtonUpCallback(SDK_PFN_ONMBUTTONUP_MANIPULATORINSTANCE pfnOnMButtonUp)
{
   ((ManipulatorPlugin *)this)->SetOnMButtonUpCallback( (PFN_ONMBUTTONUP_MANIPULATORINSTANCE) pfnOnMButtonUp );
}

void HManipulatorPlugin::SetOnRButtonDownCallback(SDK_PFN_ONRBUTTONDOWN_MANIPULATORINSTANCE pfnOnRButtonDown)
{
   ((ManipulatorPlugin *)this)->SetOnRButtonDownCallback( (PFN_ONRBUTTONDOWN_MANIPULATORINSTANCE) pfnOnRButtonDown );
}

void HManipulatorPlugin::SetOnRButtonDblClkCallback(SDK_PFN_ONRBUTTONDBLCLK_MANIPULATORINSTANCE pfnOnRButtonDblClk)
{
   ((ManipulatorPlugin *)this)->SetOnRButtonDblClkCallback( (PFN_ONRBUTTONDBLCLK_MANIPULATORINSTANCE) pfnOnRButtonDblClk );
}

void HManipulatorPlugin::SetOnRButtonUpCallback(SDK_PFN_ONRBUTTONUP_MANIPULATORINSTANCE pfnOnRButtonUp)
{
   ((ManipulatorPlugin *)this)->SetOnRButtonUpCallback( (PFN_ONRBUTTONUP_MANIPULATORINSTANCE) pfnOnRButtonUp );
}

void HManipulatorPlugin::SetOnMouseMoveCallback(SDK_PFN_ONMOUSEMOVE_MANIPULATORINSTANCE pfnOnMouseMove)
{
   ((ManipulatorPlugin *)this)->SetOnMouseMoveCallback( (PFN_ONMOUSEMOVE_MANIPULATORINSTANCE) pfnOnMouseMove );
}

void HManipulatorPlugin::SetOnMouseWheelCallback(SDK_PFN_ONMOUSEWHEEL_MANIPULATORINSTANCE pfnOnMouseWheel)
{
   ((ManipulatorPlugin *)this)->SetOnMouseWheelCallback( (PFN_ONMOUSEWHEEL_MANIPULATORINSTANCE) pfnOnMouseWheel );
}

void HManipulatorPlugin::SetOnSetCursorCallback(SDK_PFN_ONSETCURSOR_MANIPULATORINSTANCE pfnOnSetCursor)
{
   ((ManipulatorPlugin *)this)->SetOnSetCursorCallback( (PFN_ONSETCURSOR_MANIPULATORINSTANCE) pfnOnSetCursor );
}

void HManipulatorPlugin::SetGetToolTipTextCallback(SDK_PFN_GETTOOLTIPTEXT_MANIPULATORINSTANCE pfnGetToolTipText)
{
   ((ManipulatorPlugin *)this)->SetGetToolTipTextCallback( (PFN_GETTOOLTIPTEXT_MANIPULATORINSTANCE) pfnGetToolTipText );
}

void HManipulatorPlugin::SetOnApplyCallback(SDK_PFN_ONAPPLY_MANIPULATORINSTANCE pfnOnApply)
{
   ((ManipulatorPlugin *)this)->SetOnApplyCallback( (PFN_ONAPPLY_MANIPULATORINSTANCE) pfnOnApply );
}

void HManipulatorPlugin::SetOnCancelCallback(SDK_PFN_ONCANCEL_MANIPULATORINSTANCE pfnOnCancel)
{
   ((ManipulatorPlugin *)this)->SetOnCancelCallback( (PFN_ONCANCEL_MANIPULATORINSTANCE) pfnOnCancel );
}

void HManipulatorPlugin::SetOnArrowKeyDownCallback(SDK_PFN_OnArrowKeyDown_MANIPULATORINSTANCE pfnOnArrowKeyDown)
{
   ((ManipulatorPlugin *)this)->SetOnArrowKeyDownCallback( (PFN_OnArrowKeyDown_MANIPULATORINSTANCE) pfnOnArrowKeyDown );
}

void HManipulatorPlugin::SetOnArrowKeyUpCallback(SDK_PFN_OnArrowKeyUp_MANIPULATORINSTANCE pfnOnArrowKeyUp)
{
   ((ManipulatorPlugin *)this)->SetOnArrowKeyUpCallback( (PFN_OnArrowKeyUp_MANIPULATORINSTANCE) pfnOnArrowKeyUp );
}