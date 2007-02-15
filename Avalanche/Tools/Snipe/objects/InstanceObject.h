#pragma once
#include "PropertyObject.h"

class InstanceObject : public PropertyObject
{
protected:
   class DeleteInfo : public PropertyObject::DeleteInfo
   {
   public:
      InstanceObject *m_pCache;
      UINT m_nInstanceIndex;

      DeleteInfo( SnipeObject *pSnipeObject )
       : PropertyObject::DeleteInfo( pSnipeObject )
      {
      }
   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

public:
   DECLARE_SNIPEOBJECT_PURE( InstanceObject, PropertyObject )

   InstanceObject          *m_pCache;
   CList<InstanceObject *>  m_instancelist;
   int                      m_nInstanceNumber;

            InstanceObject();
   virtual ~InstanceObject();

// Operations
   int GetNumInstances() const { return m_instancelist.GetCount(); }
   int RemoveInstance( InstanceObject *pInstance ); // returns instance index
   UINT FindInstance( InstanceObject *pInstance ) const;
   InstanceObject *FindInstance( const String &name ) const;
   InstanceObject *GetRootCache() const { return m_pCache ? m_pCache->GetRootCache() : (InstanceObject *)this; } // Note: returns this if it is the cache
   InstanceObject *GetCache() const { return m_pCache ? m_pCache : (InstanceObject *)this; } // Note: returns this if it is the cache

// Overridables
   virt_base virtual InstanceObject *CreateInstance();

// SnipeObject overrides
   virtual BOOL IsCache() const { return m_pCache == NULL; }
   virtual void FreeDeleteInfo( SnipeObject::DeleteInfo *pDeleteInfo ) { delete (DeleteInfo *)pDeleteInfo; }// DeleteInfo does not have a v-table, this must be here to get to the right destructor
   virtual BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
   virtual BOOL UnDelete( SnipeObject::DeleteInfo *pDeleteInfo );
   virtual void MarkSaveDependents();
};
