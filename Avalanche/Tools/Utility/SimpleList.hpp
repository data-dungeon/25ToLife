#define DEBUGDESTROYED 0xFEEEFEEE

inline SimpleListEntry::SimpleListEntry()
{
   m_pPrevious = NULL;
   m_pNext = NULL;
   m_nRefCount = 0;
   m_pBlockHead = NULL;
   m_pBlockTail = NULL;
   m_pList = NULL;
   m_nBlockSize = 0;
   m_nDetachCount = 0;
}

inline SimpleListEntry::~SimpleListEntry()
{
}

inline SimpleListEntry *SimpleListEntry::Next()
{
   return m_pNext;
}

inline SimpleListEntry *SimpleListEntry::Previous()
{
   return m_pPrevious;
}

inline void SimpleListEntry::Reference()
{
   ASSERT(m_nRefCount < 255);
   ASSERT(m_nDetachCount < 255);
   m_nRefCount++;
   m_nDetachCount++;
}

inline void SimpleListEntry::DeReference()
{
   Detach(); 
   Purge();
}

inline void SimpleListEntry::DeReferenceBlock()
{
   if (m_pList)
      m_pList->Detach(this); 
   PurgeBlock();
}

inline void SimpleListEntry::Detach()
{
   if (m_pList)
      m_pList->Detach(this);
}

inline void SimpleListEntry::ReInsert()
{
   if (m_pList)
      m_pList->ReInsert(this);
}

inline void SimpleListEntry::Purge(BOOL bForceDestroy/*=FALSE*/)
{
   if (bForceDestroy)
      m_nRefCount = 1;

   m_nRefCount--;

   if (m_nRefCount)
      return;

   OnPurge();

   if (m_pBlockHead)
      delete [] this;
   else
      delete this;
}

inline void SimpleListEntry::PurgeBlock(BOOL bForceDestroy/*=FALSE*/)
{
   if (bForceDestroy)
      m_nRefCount = 1;

   m_nRefCount--;

   if (m_nRefCount)
      return;

   delete [] this;
}

inline BYTE SimpleListEntry::RefCount()
{
   return m_nRefCount;
}

inline BYTE SimpleListEntry::DetachCount()
{
   return m_nDetachCount;
}

inline void SimpleListEntry::SetID( UINT nID )
{
   m_nID = nID;

   if ( m_nID > GetList()->m_nNextAvailableID )
      GetList()->m_nNextAvailableID = m_nID;
}

////////////////////////////////////////////////////////////////////
// SimpleListBase

inline SimpleListBase::SimpleListBase()
{
   m_nNextAvailableID = 0;
   m_pBegin = NULL;
   m_pEnd = NULL;
   m_nSize = 0;
   m_pOwner = NULL;
}

inline SimpleListBase::~SimpleListBase()
{
   Clear();
}

inline SimpleListEntry *SimpleListBase::Begin() const
{
   return m_pBegin;
}

inline SimpleListEntry * SimpleListBase::End() const
{
   return m_pEnd;
}

inline void SimpleListBase::Clear() //RemoveAll
{
   SimpleListEntry *pEntry, *pNext;
   for (pEntry = m_pBegin; pEntry; pEntry = pNext)
   {
      if (pEntry->m_pBlockHead)
      {
         SimpleListEntry *pAfterTail = pEntry->m_pBlockTail->m_pNext;
         delete [] pEntry;
         pNext = pAfterTail;
      }
      else
      {
         pNext = pEntry->m_pNext;
         delete pEntry;
      }
   }
   m_nNextAvailableID = 0;
//   m_unusedIDs.RemoveAll();
   m_pBegin = NULL;
   m_pEnd = NULL;
   m_nSize = 0;
   OnChanged();
}

inline void SimpleListBase::Erase(SimpleListEntry *pEntry)
{
   Detach(pEntry);
   pEntry->Purge();
}

inline void SimpleListBase::Detach(SimpleListEntry *pEntry)
{
#ifdef DEBUG
   if ((pEntry->m_pBlockHead) && (pEntry->m_pBlockHead != pEntry))
   {
      ASSERT(FALSE);// dont allow removing elements from block, just the header
   }
#endif
   // will handle scenario where you never reference after a pushback or you do reference after a pushback
   ASSERT(pEntry->m_nDetachCount>0);
   if (pEntry->m_nDetachCount==1)
   {
      //m_unusedIDs.Add(pEntry->m_nID);
      if (pEntry == m_pBegin)
      {
         BOOL bClearList = FALSE;
         if (!pEntry->m_pBlockHead)
         {
            m_pBegin = pEntry->m_pNext;
            if (pEntry == m_pEnd)// if only one element in list clear list
               bClearList = TRUE;
            else
               pEntry->m_pNext->m_pPrevious = NULL;
         }
         else
         {
            m_pBegin = pEntry->m_pBlockTail->m_pNext;
            if (!pEntry->m_pBlockTail->m_pNext)
               bClearList = TRUE;
            else
               pEntry->m_pBlockTail->m_pNext->m_pPrevious = NULL;
         }
         if (bClearList)
         {
            m_pBegin = NULL;
            m_pEnd = NULL;
            m_nSize = 0;
            pEntry->m_nDetachCount--;
            return;                     
         }
      }
      else if ((pEntry == m_pEnd)||(pEntry->m_pBlockTail == m_pEnd))
      {
         if (!pEntry->m_pBlockHead)
         {
            m_pEnd = pEntry->m_pPrevious;
            m_pEnd->m_pNext = NULL;
         }
         else
         {
            m_pEnd = pEntry->m_pBlockHead->m_pPrevious;
            m_pEnd->m_pNext = NULL;
         }
      }
      else
      {
         if (!pEntry->m_pBlockHead)
         {
            if (pEntry->m_pPrevious)
               pEntry->m_pPrevious->m_pNext = pEntry->m_pNext;
            if (pEntry->m_pNext)
               pEntry->m_pNext->m_pPrevious = pEntry->m_pPrevious;
         }
         else
         {
            if (pEntry->m_pBlockHead->m_pPrevious)
               pEntry->m_pBlockHead->m_pPrevious->m_pNext = pEntry->m_pBlockHead->m_pBlockTail->m_pNext;
            if (pEntry->m_pBlockHead->m_pBlockTail->m_pNext)
               pEntry->m_pBlockHead->m_pBlockTail->m_pNext->m_pPrevious = pEntry->m_pBlockHead->m_pPrevious;
         }
      }

      if (!pEntry->m_pBlockHead)
      {
         m_nSize--;
         pEntry->m_pPrevious = NULL;
         pEntry->m_pNext = NULL;
      }
      else
      {
         m_nSize -= pEntry->m_pBlockHead->m_nBlockSize;
         pEntry->m_pBlockHead->m_pPrevious = NULL;
         pEntry->m_pBlockHead->m_pBlockTail->m_pNext = NULL;
      }
      pEntry->OnDetach();
      OnChanged();
   }
   pEntry->m_nDetachCount--;  
}

inline void SimpleListBase::ReInsert(SimpleListEntry *pEntry)
{
   ASSERT(pEntry->m_pList==this);// tried to insert element back into wrong list
   ASSERT(pEntry->m_nDetachCount>=0);// could be that element wasn't referenced after pushback
   if (pEntry->m_nDetachCount==0)
   {
      if (!pEntry->m_pBlockHead)
      {
         if (m_pEnd)
         {
            m_pEnd->SetNext(pEntry);
            pEntry->SetPrevious(m_pEnd);
            m_pEnd = pEntry;
         }
         else
         {
            m_pBegin = pEntry;
            m_pEnd = pEntry;
         }
         m_nSize++;
      }
      else
      {
         if (m_pEnd)
         {
            m_pEnd->SetNext(pEntry);
            pEntry->SetPrevious(m_pEnd);
            m_pEnd = pEntry->m_pBlockTail;
         }
         else
         {
            m_pBegin = pEntry;
            m_pEnd = pEntry->m_pBlockTail;
         }
         m_nSize += pEntry->m_nBlockSize;
      }
      pEntry->OnReInsert();
      OnChanged();
   }
   ASSERT(pEntry->m_nDetachCount < 255);
   pEntry->m_nDetachCount++;
}

inline void SimpleListBase::GetNewID(UINT &nID)
{
    ASSERT(m_nNextAvailableID < 0xFFFFFFFF);   
    nID = m_nNextAvailableID++;
}

////////////////////////////////////////////////////////////////////
// SimpleList

template< class SIMPLELISTENTRY_TYPE >
inline SimpleList< SIMPLELISTENTRY_TYPE >::SimpleList()
{
}

template< class SIMPLELISTENTRY_TYPE >
inline SimpleList< SIMPLELISTENTRY_TYPE >::~SimpleList()
{
}

template< class SIMPLELISTENTRY_TYPE >
inline SIMPLELISTENTRY_TYPE *SimpleList< SIMPLELISTENTRY_TYPE >::Begin() const
{
   return (SIMPLELISTENTRY_TYPE*)m_pBegin;
}

template< class SIMPLELISTENTRY_TYPE >
inline SIMPLELISTENTRY_TYPE * SimpleList< SIMPLELISTENTRY_TYPE >::End() const
{
   return (SIMPLELISTENTRY_TYPE*)m_pEnd;
}

template< class SIMPLELISTENTRY_TYPE >
inline void SimpleList< SIMPLELISTENTRY_TYPE >::PushBack(SIMPLELISTENTRY_TYPE *pNewEntry)
{
   if (m_pEnd)
   {
      m_pEnd->SetNext(pNewEntry);
      pNewEntry->SetPrevious(m_pEnd);
      m_pEnd = pNewEntry;
   }
   else
   {
      m_pBegin = pNewEntry;
      m_pEnd = pNewEntry;
   }
   UINT nID;
   GetNewID(nID);
   pNewEntry->m_nID = nID;

   m_nSize++;
   pNewEntry->m_pList = this;
   OnChanged();
 }

/*
template< class SIMPLELISTENTRY_TYPE >
inline void SimpleList< SIMPLELISTENTRY_TYPE >::PushBack(SIMPLELISTENTRY_TYPE *pNewEntry,UINT nSize)
{
   SIMPLELISTENTRY_TYPE *pEntry = pNewEntry;
   for (UINT i = 0; i < nSize; i++)
   {
      pEntry = &pNewEntry[i];
      pEntry->m_pBlockHead = pNewEntry;
      pEntry->m_pList = this;
      ASSERT( nSize < 65536 ); 
      pEntry->m_nBlockSize = nSize;
      if (m_pEnd)
      {
         m_pEnd->SetNext(pEntry);
         pEntry->SetPrevious(m_pEnd);
         m_pEnd = pEntry;
      }
      else
      {
         m_pBegin = pEntry;
         m_pEnd = pEntry;
      }
      UINT nID;
      GetNewID(nID);
      pEntry->m_nID = nID;
      m_nSize++;
   }
   pNewEntry->m_pBlockTail = pEntry;
   OnChanged();
}
*/

template< class SIMPLELISTENTRY_TYPE >
inline void SimpleList< SIMPLELISTENTRY_TYPE >::PushFront(SIMPLELISTENTRY_TYPE *pNewEntry)
{
   if (m_pBegin)
   {
      m_pBegin->SetPrevious(pNewEntry);
      pNewEntry->SetNext(m_pBegin);
      m_pBegin = pNewEntry;
   }
   else
   {
      m_pBegin = pNewEntry;
      m_pEnd = pNewEntry;
   }
   UINT nID;
   GetNewID(nID);
   pNewEntry->m_nID = nID;
   pNewEntry->m_pList = this;

   m_nSize++;
   OnChanged();
}

template< class SIMPLELISTENTRY_TYPE >
inline void SimpleList< SIMPLELISTENTRY_TYPE >::InsertBefore(SIMPLELISTENTRY_TYPE *pNewEntry,SIMPLELISTENTRY_TYPE *pAtEntry)
{
   if (!pAtEntry) PushFront(pNewEntry);

   if(pAtEntry->m_pPrevious)
   {
      SIMPLELISTENTRY_TYPE *pPreviousEntry = (SIMPLELISTENTRY_TYPE*)pAtEntry->m_pPrevious;

      pAtEntry->m_pPrevious = pNewEntry;
      pNewEntry->m_pPrevious = pPreviousEntry;
      pNewEntry->m_pNext = pAtEntry;
      pPreviousEntry->m_pNext = pNewEntry;
      UINT nID;
      GetNewID(nID);
      pNewEntry->m_nID = nID;
      m_nSize++;
      OnChanged();
   }
   else
      PushFront(pNewEntry);
}

template< class SIMPLELISTENTRY_TYPE >
inline void SimpleList< SIMPLELISTENTRY_TYPE >::InsertAfter(SIMPLELISTENTRY_TYPE *pNewEntry,SIMPLELISTENTRY_TYPE *pAtEntry)
{
   if (!pAtEntry) PushBack(pNewEntry);

   if(pAtEntry->m_pNext)
   {
      SIMPLELISTENTRY_TYPE *pNextEntry = (SIMPLELISTENTRY_TYPE*)pAtEntry->m_pNext;

      pAtEntry->m_pNext = pNewEntry;
      pNewEntry->m_pNext = pNextEntry;
      pNewEntry->m_pPrevious = pAtEntry;
      pNewEntry->m_pPrevious = pNewEntry;
      UINT nID;
      GetNewID(nID);
      pNewEntry->m_nID = nID;
      pNewEntry->m_pList = this;
      m_nSize++;
      OnChanged();
   }
   else
      PushBack(pNewEntry);
}

template< class SIMPLELISTENTRY_TYPE >
inline SIMPLELISTENTRY_TYPE *SimpleList< SIMPLELISTENTRY_TYPE >::CreateBlock(UINT nSize)
{
   SIMPLELISTENTRY_TYPE *pNewEntry = SNEW SIMPLELISTENTRY_TYPE[nSize];

   SIMPLELISTENTRY_TYPE *pEntry;
   for (UINT i = 0; i < nSize; i++)
   {
      pEntry = &pNewEntry[i];
      pEntry->m_pBlockHead = pNewEntry;
      ASSERT( nSize < 65536 );
      pEntry->m_nBlockSize = nSize;
      pEntry->m_nID = i;
      if (i > 0)
         pEntry->SetPrevious(&pNewEntry[i-1]);
      if (i < nSize-1)
         pEntry->SetNext(&pNewEntry[i+1]);
   }

   pNewEntry->m_pBlockTail = pEntry;

   return pNewEntry;
}

template< class SIMPLELISTENTRY_TYPE >
inline void SimpleList< SIMPLELISTENTRY_TYPE >::PushBackBlock(SIMPLELISTENTRY_TYPE *pNewEntry)
{

   if (m_pEnd)
   {
      pNewEntry->SetPrevious(m_pEnd);
      m_pEnd->SetNext(pNewEntry);
      m_pEnd = pNewEntry->BlockTail();
   }
   else
   {
      m_pBegin = pNewEntry;
      m_pEnd = pNewEntry->BlockTail();
   }
   m_nSize+=pNewEntry->GetBlockSize();

   SIMPLELISTENTRY_TYPE *pEntry = pNewEntry;
   for (UINT i = 0; i < nSize; i++)
   {
      pEntry = &pNewEntry[i];
      pEntry->m_pList = this;
      UINT nID;
      GetNewID(nID);
      pEntry->m_nID = nID;
   }
   OnChanged();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SimpleMap
template< class SIMPLELISTENTRY_TYPE >
SIMPLELISTENTRY_TYPE *SimpleMap< SIMPLELISTENTRY_TYPE >::Find(UINT nID)
{
   if (m_bDirty)
      RebuildMap();

   SIMPLELISTENTRY_TYPE *pEntry;

   if (m_idmap.Lookup( nID, pEntry ))
      return pEntry;
   else
      return NULL;
}

template< class SIMPLELISTENTRY_TYPE >
void SimpleMap< SIMPLELISTENTRY_TYPE >::RebuildMap()
{
   m_idmap.RemoveAll();
   m_idmap.InitHashTable(1.2*GetSize());

   for ( SIMPLELISTENTRY_TYPE *pEntry=Begin(); pEntry; pEntry=pEntry->Next() )
      m_idmap.SetAt(pEntry->GetID(), pEntry);
   m_bDirty = FALSE;
}
