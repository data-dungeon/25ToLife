#include "stdafx.h"
#include "SnipeNavigator.h"

//SnipeNavigator
SnipeNavigator::SnipeNavigator(NavigatorPlugin *pNavigatorPlugin) :
 SnipeManipulator(pNavigatorPlugin)
{
}

SnipeNavigator::~SnipeNavigator()
{
}

SnipeNavigatorInstance *SnipeNavigator::CreateInstance( CGLBaseView *pView )
{
   try {
      if (((NavigatorPlugin *)m_pPlugin)->GetCreateNavigatorInstanceCallback()) { 
         void *pPluginInstanceData = ((NavigatorPlugin *)m_pPlugin)->GetCreateNavigatorInstanceCallback()( m_pPlugin->GetPluginData(), pView );
         if (!pPluginInstanceData)
            return NULL;
         return SNEW SnipeNavigatorInstance( this, pPluginInstanceData );
      }
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("Navigator CreateInstance", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("Navigator CreateInstance", "Unknown") << "\n";
   }
   return NULL;
}

// SnipeNavigatorInstance
BOOL SnipeNavigatorInstance::SetViewAngle( CGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye/*=TRUE*/ )
{
   try {
      if (((NavigatorPlugin *)m_pPlugin)->GetSetViewAngleCallback())
         return ((NavigatorPlugin *)m_pPlugin)->GetSetViewAngleCallback()( m_pPluginInstanceData, pView, nIndex, bSnapToLastBirdsEye );
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("SetViewAngle", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("SetViewAngle", "Unknown") << "\n";
   }
   return NULL;
}

BOOL SnipeNavigatorInstance::ZoomFit( CGLBaseView *pView, SnipeObjectList *pList )
{
   try {
      if (((NavigatorPlugin *)m_pPlugin)->GetZoomFitCallback())
         return ((NavigatorPlugin *)m_pPlugin)->GetZoomFitCallback()( m_pPluginInstanceData, pView, pList );
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("ZoomFit", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("ZoomFit", "Unknown") << "\n";
   }
   return FALSE;
}