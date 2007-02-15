#ifndef __SimpleList_H
#define __SimpleList_H

#include "ADataType.h"

#define SIMPLELISTREFCOUNT 0xFFFFFFFF

template< class SIMPLELISTENTRY_TYPE > class SimpleList;

class SimpleListBase;

class SimpleListEntry
{
protected:
   SimpleListEntry *m_pPrevious, *m_pNext;
   BYTE m_nRefCount;
   BYTE m_nDetachCount;
   UWORD m_nBlockSize;
   UINT m_nID;
   SimpleListEntry *m_pBlockHead;
   SimpleListEntry *m_pBlockTail;
   SimpleListBase *m_pList;
public:
                      SimpleListEntry();
   virt_base virtual ~SimpleListEntry(); // Don't believe there is a reason for this to be virtual.  We do not need a v-table.

   SimpleListEntry *Next();
   SimpleListEntry *Previous();
   void SetNext( SimpleListEntry *pNext ) { m_pNext = pNext; }
   void SetPrevious( SimpleListEntry *pPrevious ) { m_pPrevious = pPrevious; }

   SimpleListEntry *BlockHead() { return m_pBlockHead; }
   SimpleListEntry *BlockTail() { return m_pBlockTail; }
   UWORD GetBlockSize() { return m_nBlockSize; }

   virt_base virtual void Reference();
   virt_base virtual void DeReference();
   virt_base virtual void DeReferenceBlock();
   virt_base virtual void Detach();
   virt_base virtual void OnDetach() {}
   virt_base virtual void ReInsert();
   virt_base virtual void OnReInsert() {}
   virt_base virtual void Purge(BOOL bForceDestroy = FALSE);
   virt_base virtual void OnPurge() {}
   virt_base virtual void PurgeBlock(BOOL bForceDestroy = FALSE);

   BYTE RefCount();
   BYTE DetachCount();

   UINT GetID() const { return m_nID; }
   void SetID( UINT nID );

   SimpleListBase *GetList() { return m_pList; }
   void  SetList( SimpleListBase *pList ) { m_pList = pList; }

   friend class SimpleList;
   friend class SimpleListBase;
};

template< class SIMPLELISTENTRY_TYPE >
class SimpleEntryShortcut : public SimpleListEntry
{
protected:
   SimpleListEntry *m_pEntry;
public:
   SIMPLELISTENTRY_TYPE *GetEntry() { return m_pEntry; }
   void SetEntry(SIMPLELISTENTRY_TYPE *pEntry) { m_pEntry = pEntry; }
};

class SimpleListBase
{
   friend SimpleListEntry;
protected:
   UINT m_nSize;
   SimpleListEntry *m_pBegin,*m_pEnd;
   UINT m_nNextAvailableID;
///   CArray<UINT> m_unusedIDs;
   void *m_pOwner;
public:
                      SimpleListBase();
   virt_base virtual ~SimpleListBase();

   SimpleListEntry *Begin() const;
   SimpleListEntry *End() const;

   void Clear(); // RemoveAll
   UINT GetSize() { return m_nSize; }

   void Erase(SimpleListEntry *pEntry);
   void Detach(SimpleListEntry *pEntry);
   void Purge(SimpleListEntry *pEntry);
   void ReInsert(SimpleListEntry *pEntry);

   void GetNewID(UINT &nID);

   void SetOwner( void *pOwner ) { m_pOwner = pOwner; }
   void *GetOwner() { return m_pOwner; }

   //Overrideables
   virt_base virtual void OnChanged() {}
};

template< class SIMPLELISTENTRY_TYPE >
class SimpleList : public SimpleListBase
{
   friend SimpleListEntry;
public:
   SimpleList();
   virt_base virtual ~SimpleList();

   SIMPLELISTENTRY_TYPE *Begin() const;
   SIMPLELISTENTRY_TYPE *End() const;

   static SIMPLELISTENTRY_TYPE *CreateBlock(UINT nSize);

   void PushBack(SIMPLELISTENTRY_TYPE *pNewEntry);
//   void PushBack(SIMPLELISTENTRY_TYPE *pNewEntry,UINT nSize);
   void PushFront(SIMPLELISTENTRY_TYPE *pNewEntry);
   void PushBackBlock(SIMPLELISTENTRY_TYPE *pNewEntry);

   void InsertBefore(SIMPLELISTENTRY_TYPE *pNewEntry,SIMPLELISTENTRY_TYPE *pAtEntry);
   void InsertAfter(SIMPLELISTENTRY_TYPE *pNewEntry,SIMPLELISTENTRY_TYPE *pAtEntry);
};

template< class SIMPLELISTENTRY_TYPE >
class SimpleMap : public SimpleList<SIMPLELISTENTRY_TYPE>
{
private:
   CMap<UINT, UINT, SIMPLELISTENTRY_TYPE *, SIMPLELISTENTRY_TYPE *> m_idmap;
   BOOL m_bDirty;

   void RebuildMap();

public:
   SimpleMap()
   {
      m_bDirty = TRUE;
   }

   SIMPLELISTENTRY_TYPE *Find(UINT nID);

   //SimpleList overrides
   void OnChanged() { m_bDirty = TRUE; }
};


#include "simplelist.hpp"

#endif