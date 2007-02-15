#pragma once
#include "SDKMacros.h"
#include "HMainInterface.h"

/////////////////////////////////////////////////////////////////////////////
// C++ Helper classe for HPluginHelper

BOOL OnObjectNotifyCallback( void *pData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );

class HCallbackHelper
{
public:
   BOOL AddCallback( UINT msgs, HSnipeObject *pObject=NULL )
   {
      if (!pObject)
         pObject = GetHMainInterface();
      return pObject->AddCallback( msgs, OnObjectNotifyCallback, this );
   }
   BOOL RemoveCallback( HSnipeObject *pObject=NULL, UINT msgs=NOTIFYOBJ_ALL )
   {
      if (!pObject)
         pObject = GetHMainInterface();
      return pObject->RemoveCallback( OnObjectNotifyCallback, this, msgs );
   }

// Overridables
   virt_base virtual void OnSelectionChanged( HSnipeObject *pObject, BOOL bSelected ) {}
   virt_base virtual void OnSelectionListChanged( HSelectionList *pSelectionList, BOOL bTemporary ) {}
   virt_base virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      switch (msg)
      {
         case NOTIFYOBJ_SELECTIONCHANGED:
         {
            BOOL bSelected = (BOOL)wParam;
            HSnipeObject *pObject = (HSnipeObject *)lParam;
            OnSelectionChanged( pObject, bSelected );
         }
         case NOTIFYOBJ_SELECTIONLISTCHANGED:
         {
            BOOL bTemporary = (BOOL)wParam;
            HSelectionList *pSelectionList = (HSelectionList *)lParam;
            OnSelectionListChanged( pSelectionList, bTemporary );
         }
      }

      return FALSE;
   }
};

inline BOOL OnObjectNotifyCallback( void *pData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   HCallbackHelper *pPlugin = (HCallbackHelper *)pData;
   return pPlugin->OnObjectNotify( msg, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
// C++ Helper classe for HPluginHelper

class HPlugin;
class HPluginHelper
{
private:
   HPlugin *m_pPlugin;

public:
   HPluginHelper() :
    m_pPlugin(NULL)
   {
   }

   virtual ~HPluginHelper() {};

   void SetPlugin( HPlugin *pPlugin ) { m_pPlugin = pPlugin; }
   HPlugin *GetPlugin() { return m_pPlugin; }

   //pure
   virtual const char *GetTitleName() const = 0;
   virtual const char *GetClassName() const = 0;
   virtual const HPluginHelper *IsKindOf( const String &strClassName ) const { return NULL; }
};

inline void Destroy_PluginData(void *pPluginData)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   if (pPluginData)
      delete (HPluginHelper *) pPluginData;
}

#define DECLARE_PLUGINHELPER(class_name, baseclass_name) \
   static const char *GetThisClassName() { return #class_name; } \
   virtual const char *GetClassName() const { return #class_name; } \
   virtual const HPluginHelper *IsKindOf( const String &strClassName ) const \
   { \
      if (strClassName == GetThisClassName()) \
         return this; \
      return baseclass_name::IsKindOf( strClassName ); \
   } 
