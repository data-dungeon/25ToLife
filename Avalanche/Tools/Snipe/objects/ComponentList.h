#pragma once

#include "../Utility/MathTools.h"
#include "Component.h"

class SnipeObject;
class SimpleComponentList;

template<class ARG_KEY>
AFX_INLINE UINT AFXAPI HashKey( const UINT &key )
{
   return key;
}

template< class TYPE >
class CComponentList : public CMap<UINT, UINT, TYPE *, TYPE *>
{
private:
   SnipeObject *m_pOwner;
   UINT         m_nNextID;
   BOOL         m_bDirty;

   UINT GetNewID()
   {
      return m_nNextID++;
   }

public:
   CComponentList()
   {
      m_pOwner = NULL;
      m_nNextID = 1;
      m_bDirty = TRUE;
   }
   ~CComponentList()
   {
      for (POSITION pos=GetHeadPosition(); pos; )
      {
         TYPE *pComponent = GetNext( pos );
         pComponent->m_bInList = FALSE;
         pComponent->Disconnect();
         pComponent->DeReference();
      }
   }
   UINT GetNextID()
   {
      return m_nNextID;
   }
   SnipeObject *GetOwner() const
   {
      return m_pOwner;
   }
   void SetOwner( SnipeObject *pOwner )
   {
      m_pOwner = pOwner;
   }
   TYPE *GetHead()
   {
      POSITION pos = GetHeadPosition();
      if (pos)
         return GetNext(pos);
      else
         return NULL;
   }
   POSITION GetHeadPosition() const
   {
      return GetStartPosition();
   }
   TYPE *GetNext( POSITION &rPosition ) const
   {
      UINT key;
      TYPE *value;
      GetNextAssoc( rPosition, key, value );
      return value;
   }
   void Save( BaseStream &bs, const String &strHeader )
   {
      DataHeader header( bs, strHeader );

      bs << mar << GetCount() << nl;

      UINT nMaxID = GetNextID();
      for (UINT nID=0; nID<nMaxID; nID++)
      {
         Component *pComponent = Find( nID );
         if (pComponent)
            pComponent->Save( bs, TRUE );
      }
   }
   BOOL Load( BaseStream &bs )
   {
      bs.ReadMargin();

      UINT nCount;
      bs >> nCount;

      MemoryStream ms( 1024 );
      for (UINT nIndex=0; nIndex<nCount; nIndex++)
      {
         TYPE *pComponent = (TYPE *)GetOwner()->CreateComponent( TYPE::GetThisSOT() );
         ASSERT( pComponent );
         pComponent->m_pList = (SimpleComponentList *)this;

         bs.ReadMargin();
         bs.ReadLine( ms );
         if (pComponent->Load( ms ))
            Add( pComponent );
         else
         {
            g_outputWindow << "<W>Warning: Failed to load " << pComponent->GetClassName() << "(" << pComponent->GetID() << ")\n";
            //ASSERT(FALSE);
            pComponent->Disconnect();
            delete pComponent;
         }
      }

      return TRUE;
   }
   void RebuildMap()
   {
      if (!m_bDirty)
         return;

      CArray<TYPE *> m_copyarray;

      UINT nSize = GetCount();

      m_copyarray.SetSize( nSize );
      UINT i=0;
      UINT ntemp;

      for (POSITION pos = GetStartPosition(); pos; i++)
         GetNextAssoc(pos, ntemp, m_copyarray[i]);

      RemoveAll();
      UINT nNextPrime = GetPrimeLargerThan( nSize * 1.2 );
      InitHashTable( nNextPrime );

      for (i=0; i < nSize; i++)
      {
         TYPE *pComponent = m_copyarray[i];
         SetAt( pComponent->GetID(), pComponent );
      }

      m_bDirty = FALSE;
   }
   TYPE *Find( UINT nID )
   {
      if (m_bDirty)
         RebuildMap();

      TYPE *value;
      if (Lookup( nID, value ))
         return value;
      return NULL;
   }
   void Add( TYPE *pComponent )
   {
      if (pComponent->GetID() == -1)
         pComponent->SetID( GetNewID() );
      else if (pComponent->GetID() >= m_nNextID)
         m_nNextID = pComponent->GetID() + 1;

      pComponent->m_pList = (SimpleComponentList *)this;
      pComponent->m_bInList = TRUE;
      pComponent->Reference();

      SetAt( pComponent->GetID(), pComponent );
      m_bDirty = TRUE;

      GetOwner()->OnObjectNotify( NOTIFYOBJ_ADDEDCHILD, 0, (LPARAM)pComponent );
   }
   void ReInsert( TYPE *pComponent )
   {
      ASSERT( pComponent->GetID()!=-1 && /*pComponent->GetID()<m_nNextID &&*/ !pComponent->m_bInList );

      if (pComponent->GetID() >= m_nNextID)
         m_nNextID = pComponent->GetID() + 1;

      SetAt( pComponent->GetID(), pComponent );

      pComponent->m_bInList = TRUE;
      pComponent->Reference();
      pComponent->Reconnect();
      m_bDirty = TRUE;

      GetOwner()->OnObjectNotify( NOTIFYOBJ_ADDEDCHILD, 0, (LPARAM)pComponent );
   }
   BOOL Remove( Component *pComponent )
   {
      if (!pComponent->m_bInList)
         return FALSE;

      pComponent->m_bInList = FALSE;
      pComponent->GetMirrorObject(); // Make sure you cache your mirror object
      pComponent->Disconnect();
      BOOL rvalue = RemoveKey( pComponent->GetID() );

      if (pComponent->GetID() == m_nNextID-1)
      {
          while (!Find(--m_nNextID))
          {
             if (m_nNextID==0)
                break;
          }
          m_nNextID++;
      }

      m_bDirty = TRUE;

      GetOwner()->OnObjectNotify( NOTIFYOBJ_REMOVEDCHILD, 0, (LPARAM)pComponent );

      pComponent->DeReference();

      return rvalue;
   }

   void ChangeComponentID( UINT nOldID, UINT nNewID )
   {
      TYPE *value;
      if (Lookup( nOldID, value ))
      {
         RemoveKey( nOldID );
         SetAt( nNewID, value );
         m_bDirty = TRUE;
      }

      if (nNewID > m_nNextID)
         m_nNextID = nNewID+1;
   }
};

class SimpleComponentList : public CComponentList<Component>
{
};
