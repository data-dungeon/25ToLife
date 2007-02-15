#pragma once

#include "PropertyObject.h"

class Component;
template< class TYPE >
class CComponentList;

class Component : public PropertyObject
{
private:
   UINT                 m_nID;
   SimpleComponentList *m_pList;

   void SetID( UINT nID ) { m_nID = nID; }

public:
   DECLARE_COMPONENT_PURE(Component, PropertyObject)

            Component();
            Component( UINT nID );
   virtual ~Component();

   friend class CComponentList;

   UINT GetID()
   {
      return m_nID;
   }
   BOOL SafeSetID( UINT nID );

   SimpleComponentList *GetList() { return m_pList; }
   void SetList( SimpleComponentList *pList ) { m_pList = pList; }
   BOOL RemoveFromList();
   void ReInsertIntoList();

// Overrideables
   virt_base virtual const char *GetCommandLineName() const = 0;
   virt_base virtual void Reconnect();
   virt_base virtual void Disconnect();
   virt_base virtual BOOL IsUsed() { return FALSE; }

// SnipeObject overrides
   virtual String GetMatchName() const { return (ULONG)m_nID; }
   virtual BOOL IsHidable() { return TRUE; }
   virtual BOOL IsLockable() { return TRUE; }
   virtual SnipeObject *GetOwner();
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
   virtual SnipeObject *GetParent() const;
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual void DrawSelected( CGLBaseView *pView );
   virtual void DrawMirrored( CGLBaseView *pView );
   virtual void DrawFocused( CGLBaseView *pView );
};

class ComponentLeakList : public CList<Component *>
{
public:
   UINT m_nLocalID;
   CMap<Component *, Component *, UINT, UINT> m_localIDmap;

   ComponentLeakList()
   {
      m_nLocalID = 0;
   }

   ~ComponentLeakList()
   {
      for (POSITION pos = GetHeadPosition(); pos; )
      {
         Component *pComponent = GetNext(pos);
         ASSERT(pComponent);
         String strMsg("Class %1 leak: Address=%2, ID=%3, Index added at=%4\n");
         strMsg.Replace("%1", pComponent->GetClassName());
         char szText[255];
         sprintf( szText, "0x%08X", pComponent );
         strMsg.Replace("%2", szText);
         strMsg.Replace("%3", (int)pComponent->GetID());
         UINT nIndexAddedAt=(UINT)-1;
         m_localIDmap.Lookup( pComponent, nIndexAddedAt);
         strMsg.Replace("%4", (int)nIndexAddedAt);
         OutputDebugString(strMsg.Get());
      }
   }

   BOOL Remove( Component *pComponent)
   {
      POSITION pos = Find( pComponent );
      if (pos)
      {
         RemoveAt( pos );
         return TRUE;
      }
      return FALSE;
   }

   POSITION AddTail(const Component *)
   {
      ASSERT(FALSE);// Call ADD
      return 0;
   }

   POSITION AddHead(const Component *)
   {
      ASSERT(FALSE);// Call ADD
      return 0;
   }

   UINT Add( Component *pComponent )
   {
      CList<Component *>::AddTail( pComponent );
      m_localIDmap.SetAt(pComponent, m_nLocalID);
      m_nLocalID++;
      return m_nLocalID-1;
   }
};
