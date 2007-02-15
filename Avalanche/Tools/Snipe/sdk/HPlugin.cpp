//
#include "stdafx.h"
#include "HPlugin.h"
#include "Plugin.h"

void HPlugin::SetClassName(const char *szClassName)
{
   ((Plugin *)this)->SetClassName(szClassName);
}

const char *HPlugin::GetClassName() const
{
   return ((Plugin *)this)->GetClassName().Get();
}

void *HPlugin::GetPluginData()
{
   return ((Plugin *)this)->GetPluginData();
}

void HPlugin::SetTitleName(const char *szClassName)
{
   ((Plugin *)this)->SetTitleName(szClassName);
}

const char *HPlugin::GetTitleName() const
{
   return ((Plugin *)this)->GetTitleName().Get();
}
