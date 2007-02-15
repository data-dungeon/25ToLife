#pragma once
#include "InstanceObject.h"
#include "../Interface/MainInterface.h"
#include "Plugin/CommandUIPluginManager.h"

class ParentableObject : public InstanceObject
{
public:
   DECLARE_SNIPEOBJECT_PURE(ParentableObject, InstanceObject);

   class DeleteInfo : public InstanceObject::DeleteInfo
   {
   public:
      SnipeObject *m_pParent;

      DeleteInfo( SnipeObject *pSnipeObject )
       : InstanceObject::DeleteInfo( pSnipeObject )
      {
      }
   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

   SnipeObject *m_pParent;

            ParentableObject();
   virtual ~ParentableObject();

   void SetParent( SnipeObject *pParent ) { m_pParent = pParent; }
   BOOL IsInProject()
   {
      for (SnipeObject *pParent=this; pParent; pParent = pParent->GetParent())
         if (pParent->GetSOT() == SOT_Project)
            return TRUE;

      return FALSE;
   }

// SnipeObject overrides
   virtual SnipeObject *GetParent() const { return m_pParent; }
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual void FreeDeleteInfo( SnipeObject::DeleteInfo *pDeleteInfo ) { delete (DeleteInfo *)pDeleteInfo; }// DeleteInfo does not have a v-table, this must be here to get to the right destructor
   virtual BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
   virtual BOOL UnDelete( SnipeObject::DeleteInfo *pDeleteInfo );
   virtual void MarkSaveDependents();
};
