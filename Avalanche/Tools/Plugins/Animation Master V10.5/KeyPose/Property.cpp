//

#include "stdafx.h"
#include "Hxt.h"
#include "SDK/HPropert.h"
#include "SDK/HModel.h"

extern CHxtApp theApp;

static HProperty *FindProperty(HProperty *parentprop, const String &match, int *pIndex=NULL)
{
   HProperty *prop;

   if (pIndex)
      *pIndex = -1;

   for (int i=0; prop = parentprop->GetPropertyAt(i); i++)
   {
      if (prop->GetMatchName() == match ){
         if (pIndex)
            *pIndex = i;
         return prop;
      }
   }
   return NULL;
}

BOOL CHxtApp::AddModelProperties(HModel *modelcache, HCategoryProperty *pluginproperties)
{

   HCategoryProperty *avalancheproperty = (HCategoryProperty *)FindProperty(pluginproperties, "Avalanche", &m_avalanchepropertyindex );
   if (avalancheproperty==NULL) {
      avalancheproperty = HCategoryProperty::New(m_avalanchepropertyinfo);
      m_avalanchepropertyindex = pluginproperties->AddProperty(avalancheproperty);
   }

      HDataProperty *dataproperty = HDataProperty::New(m_datapropertyinfo);
      m_datapropertyindex = avalancheproperty->AddProperty(dataproperty);

   return TRUE;
}

HDataProperty *CHxtApp::GetDataProperty(HModel *model)
{
   HCategoryProperty *pluginproperties = model->GetPluginProperties();
   HProperty *avalancheproperty = pluginproperties->GetPropertyAt(theApp.m_avalanchepropertyindex);
   return (HDataProperty *)avalancheproperty->GetPropertyAt(theApp.m_datapropertyindex);
}

