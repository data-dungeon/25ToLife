////////////////////////////////////////////////////////////////////////////
//
// Pool
//
// Classes to handle keeping track of atoms below a parent aggregate
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "pool.h"
#include "Aggregate.h"
#include "List.h"
#include "Iterator.h"
#include "Scalar.h"
#include "memorymanager.h"

// TupperwarePoolValueString methods

//TupperwarePoolValueString::TupperwarePoolValueString(void) // we should not use this contructor
//{
//}

TupperwarePoolValueString::TupperwarePoolValueString(TupperwareAggregate *aggregate,const char *key) :
	m_key(key), m_aggregate(aggregate)
{
	RebuildLinks();
}

TupperwarePoolValueString::~TupperwarePoolValueString()
{
}

// Find methods
TupperwareAtom* TupperwarePoolValueString::FindAtom(int index) const
{
	assert(index>=0 && index<m_atomArray.GetSize());
	return m_atomArray[index];
}

TupperwareAggregate* TupperwarePoolValueString::FindAggregate(int index) const
{
	assert(index>=0 && index<m_atomArray.GetSize());
	TupperwareAtom *a = m_atomArray[index];
	if (a)
	{
		assert(a->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE);
	}
	return (TupperwareAggregate *)a;
}

TupperwareList* TupperwarePoolValueString::FindList(int index) const
{
	assert(index>=0 && index<m_atomArray.GetSize());
	TupperwareAtom *a = m_atomArray[index];
	if (a)
	{
		assert(a->GetAtomType()==TupperwareAtom::ATOM_LIST);
	}
	return (TupperwareList *)a;
}

TupperwareScalar* TupperwarePoolValueString::FindScalar(int index) const
{
	assert(index>=0 && index<m_atomArray.GetSize());
	TupperwareAtom *a = m_atomArray[index];
	if (a)
	{
		assert(a->GetAtomType()==TupperwareAtom::ATOM_SCALAR);
	}
	return (TupperwareScalar *)a;
}

TupperwareAtom* TupperwarePoolValueString::FindAtom(const char* name ) const
{
	int index = atoi(name);
	return FindAtom(index);
}

TupperwareAggregate* TupperwarePoolValueString::FindAggregate(const char* name ) const
{
	int index = atoi(name);
	return FindAggregate(index);
}

TupperwareList* TupperwarePoolValueString::FindList(const char* name ) const
{
	int index = atoi(name);
	return FindList(index);
}

TupperwareScalar* TupperwarePoolValueString::FindScalar(const char* name ) const
{
	int index = atoi(name);
	return FindScalar(index);
}


// Add methods
TupperwareAggregate* TupperwarePoolValueString::AddAggregate(int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareAggregate *newAggregate = m_aggregate->AddAggregate(m_key,name);
	m_atomArray[index] = newAggregate;
	return newAggregate;
}

TupperwareList* TupperwarePoolValueString::AddList(int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareList *newList = m_aggregate->AddList(m_key,name);
	m_atomArray[index] = newList;
	return newList;
}

TupperwareList* TupperwarePoolValueString::AddListFloat(const float* values, int num_values,int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareList *newList = m_aggregate->AddListFloat(m_key,name,values,num_values);
	m_atomArray[index] = newList;
	return newList;
}

TupperwareList* TupperwarePoolValueString::AddListFloat(const TupArray<float> &values,int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareList *newList = m_aggregate->AddListFloat(m_key,name,values);
	m_atomArray[index] = newList;
	return newList;
}

TupperwareList* TupperwarePoolValueString::AddListInt(const int* values,int num_values,int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareList *newList = m_aggregate->AddListInt(m_key,name,values,num_values);
	m_atomArray[index] = newList;
	return newList;
}

TupperwareList* TupperwarePoolValueString::AddListInt(const TupArray<int> &values,int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareList *newList = m_aggregate->AddListInt(m_key,name,values);
	m_atomArray[index] = newList;
	return newList;
}

TupperwareList* TupperwarePoolValueString::AddListString(const char** values, int num_values,int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareList *newList = m_aggregate->AddListString(m_key,name,values,num_values);
	m_atomArray[index] = newList;
	return newList;
}

TupperwareScalar* TupperwarePoolValueString::AddScalar(int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareScalar *newScalar = m_aggregate->AddScalar(m_key,name);
	m_atomArray[index] = newScalar;
	return newScalar;
}

TupperwareScalar* TupperwarePoolValueString::AddScalarFloat(float value,int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareScalar *newScalar = m_aggregate->AddScalarFloat(m_key,name,value);
	m_atomArray[index] = newScalar;
	return newScalar;
}

TupperwareScalar* TupperwarePoolValueString::AddScalarInt(int value,int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareScalar *newScalar = m_aggregate->AddScalarInt(m_key,name,value);
	m_atomArray[index] = newScalar;
	return newScalar;
}

TupperwareScalar* TupperwarePoolValueString::AddScalarString(const char* value,int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareScalar *newScalar = m_aggregate->AddScalarString(m_key,name,value);
	m_atomArray[index] = newScalar;
	return newScalar;
}

TupperwareScalar* TupperwarePoolValueString::AddScalarBinary(const void* value, int length,int index)
{
	char name[20];
	IndexToName(index,name);
	TupperwareScalar *newScalar = m_aggregate->AddScalarBinary(m_key,name,value,length);
	m_atomArray[index] = newScalar;
	return newScalar;
}

int TupperwarePoolValueString::AddAtom( TupperwareAtom* child,int index)
{
	char name[20];
	IndexToName(index,name);
	child->SetName(name);
	m_aggregate->AddChild(child);
	m_atomArray[index] = child;
	return index;
}


// misc methods
void TupperwarePoolValueString::DeleteEntry(int index)
{
	assert(index>=0 && index<m_atomArray.GetSize());
	TupperwareAtom *entry = m_atomArray[index];
	if (entry)
	{
		entry->Detatch(); // detatch from parent
		delete entry; // delete the entry
		m_atomArray[index]=NULL;
	}
}

void TupperwarePoolValueString::RebuildLinks(void)
{
	m_atomArray.SetSize(0,256); // start with an empty array and set growby to 256
	// do only when there are children involved
	if (m_aggregate->GetFirstChildAtom())
	{
		for (TupperwareIterator i = m_aggregate->FindByKey(m_key); !i.Finished(); i.Next())
		{
			TupperwareAtom *atom = i.GetAsAtom();
			int index = atoi(atom->GetName());
			assert(index>=0);
			if (index<m_atomArray.GetSize())
			{
				assert(!m_atomArray[index]); // slot better be open
				m_atomArray[index] = atom;
			}
			else
			{
				m_atomArray.SetSize(index+1); // make room for pointer
				m_atomArray[index] = atom;
			}
		}
	}
}

int TupperwarePoolValueString::FindEmptyEntry(void)
{
	int index;
	for (index=0;index<m_atomArray.GetSize();index++)
	{
		if (m_atomArray[index]==NULL)
			return index;
	}
	// if there is no gaps in the list then give the last one
	TupperwareAtom *nullPtr = NULL;
	m_atomArray.Add(nullPtr);
	return index; // index is = to size of array prior to push_back
}

void TupperwarePoolValueString::FitForIndex(int index)
{
	assert(index>=0);
	if (m_atomArray.GetSize()<=index)
	{
		int oldSize = m_atomArray.GetSize();
		m_atomArray.SetSize(index+1);
		// clear from the old to new index
		for (int i=oldSize;i<=index;i++)
		{
			m_atomArray[i] = NULL;
		}
	}
}

void TupperwarePoolValueString::IndexToName(int &index,char *name)
{
	if (index==-1)
	{
		index = FindEmptyEntry();
	}
	else
	{
		FitForIndex(index); // make sure we have space to hold this
		assert(!m_atomArray[index]); // make sure this slot is unused
	}
#ifdef PS2
	sprintf(name, "%d", index);
#elif defined(GCN)
	sprintf(name, "%d", index);
#else
	itoa(index, name, 10);
#endif
}

int TupperwarePoolValueString::GetPoolSize(void)
{
	return m_atomArray.GetSize();
}

#ifdef USE_TUP_MEMORY_MANAGER
void *TupperwarePoolValueString::operator new(size_t nSize)
{
	return TupMemoryManager::Allocate(nSize); // call this one if running in the game engine
}

void TupperwarePoolValueString::operator delete(void *pBuffer)
{
	TupMemoryManager::FreeBuffer(pBuffer);
}
#endif

//	TupperwarePoolHashEntry methods
TupperwarePoolHashEntry::TupperwarePoolHashEntry(void) :
	m_atom(NULL), m_hash(0), m_next(NULL)
{
}

TupperwarePoolHashEntry::TupperwarePoolHashEntry(const TupperwarePoolHashEntry &s) :
	m_atom(s.m_atom), m_hash(s.m_hash)
{
	TupperwarePoolHashEntry *sourceEntry = s.m_next;
	TupperwarePoolHashEntry **nextLoc = &m_next;
	while (sourceEntry) // copy others without blowing out our stack
	{
		TupperwarePoolHashEntry *newEntry = new TupperwarePoolHashEntry;
		newEntry->m_atom = sourceEntry->m_atom;
		newEntry->m_hash = sourceEntry->m_hash;
		*nextLoc = newEntry;
		sourceEntry = sourceEntry->m_next;
		nextLoc = &newEntry->m_next;
	}
	*nextLoc = NULL;
}

TupperwarePoolHashEntry &TupperwarePoolHashEntry::operator=(const TupperwarePoolHashEntry &s)
{
	delete m_next;
	m_hash = s.m_hash;
	m_atom = s.m_atom;

	TupperwarePoolHashEntry *sourceEntry = s.m_next;
	TupperwarePoolHashEntry **nextLoc = &m_next;
	while (sourceEntry) // copy others without blowing out our stack
	{
		TupperwarePoolHashEntry *newEntry = new TupperwarePoolHashEntry;
		newEntry->m_atom = sourceEntry->m_atom;
		newEntry->m_hash = sourceEntry->m_hash;
		*nextLoc = newEntry;
		sourceEntry = sourceEntry->m_next;
		nextLoc = &newEntry->m_next;
	}
	*nextLoc = NULL;
	return *this;
}

TupperwarePoolHashEntry::~TupperwarePoolHashEntry()
{
	m_atom = NULL;
	while(m_next)
	{
		TupperwarePoolHashEntry *deleteEntry = m_next;
		m_next = m_next->m_next;
		deleteEntry->m_next = NULL;
		delete deleteEntry;
	}
	delete m_next;
	m_next = NULL;
}

TupperwareAtom *TupperwarePoolHashEntry::Find(const char *name,unsigned int hash) const
{
	if (Compare(name,hash)) // am I a match?
		return m_atom;
	TupperwarePoolHashEntry *entry = m_next;
	while (entry) // check others attached to me
	{
		if (entry->Compare(name,hash))
			return entry->m_atom;
		entry = entry->m_next;
	}
	return NULL;
}

bool TupperwarePoolHashEntry::Compare(const char *name,unsigned int hash) const
{
	if (m_hash == hash)
	{
		if (TupperwareAggregate::strnullcmp(name,m_atom->GetName())==0)
		{
			return true;
		}
	}
	return false;
}

bool TupperwarePoolHashEntry::Add(TupperwareAtom *atom,unsigned int hash)
{
	assert(atom);
	if (m_atom) // if I exist
	{
		TupperwarePoolHashEntry *newEntry = new TupperwarePoolHashEntry;
		newEntry->m_atom = atom;
		newEntry->m_hash = hash;
		newEntry->m_next = m_next;
		m_next = newEntry;
	}
	else // if I am null
	{
		m_atom = atom;
		m_hash = hash;
	}
	return true;
}

bool TupperwarePoolHashEntry::Delete(const char *name,unsigned int hash)
{
	if (Compare(name,hash))
	{
		// if I am the one to delete replace my data with the next one and kill the next one (I belong to an array)
		if (m_next) // if there is another in the list
		{
			m_atom = m_next->m_atom; // copy this one on top of me
			m_hash = m_next->m_hash;
			TupperwarePoolHashEntry *deleteEntry = m_next;
			m_next = m_next->m_next; // point past it
			deleteEntry->m_next = NULL; // prepare for delete
			delete deleteEntry;
		}
		else
		{
			m_atom = NULL;
			m_hash = 0;
		}
		return true;
	}
	else
	{
		TupperwarePoolHashEntry *prevEntry = this;
		TupperwarePoolHashEntry *entry = m_next;
		while (entry)
		{
			if (entry->Compare(name,hash))
			{
				prevEntry->m_next = entry->m_next; // point past it
				entry->m_next = NULL; // prepare for delete
				delete entry;
				return true; // return after we find it (there should be only one)
			}
			prevEntry = entry;
			entry = entry->m_next;
		}
		return false;
	}
}

#ifdef USE_TUP_MEMORY_MANAGER
void *TupperwarePoolHashEntry::operator new(size_t nSize)
{
	return TupMemoryManager::Allocate(nSize); // call this one if running in the game engine
}
void TupperwarePoolHashEntry::operator delete(void *pBuffer)
{
	TupMemoryManager::FreeBuffer(pBuffer);
}
#endif

//	TupperwarePoolString methods

//TupperwarePoolString::TupperwarePoolString(void) // we should not use this contructor
//{
//}

TupperwarePoolString::TupperwarePoolString(TupperwareAggregate *aggregate,const char *key,int hashSize) :
	  m_key(key), m_aggregate(aggregate), m_hashSize(hashSize), m_numAtoms(0)
{
	m_hashArray.SetSize(hashSize);
	RebuildLinks();
}

TupperwarePoolString::~TupperwarePoolString()
{
}

// Find methods
TupperwareAtom* TupperwarePoolString::FindAtom(const char* name ) const
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	return m_hashArray[hashIndex].Find(name,hash);
}

TupperwareAggregate* TupperwarePoolString::FindAggregate(const char* name ) const
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareAtom *a = m_hashArray[hashIndex].Find(name,hash);
	if (a)
	{
		assert(a->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE);
	}
	return (TupperwareAggregate *)a;
}

TupperwareList* TupperwarePoolString::FindList(const char* name ) const
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareAtom *a = m_hashArray[hashIndex].Find(name,hash);
	if (a)
	{
		assert(a->GetAtomType()==TupperwareAtom::ATOM_LIST);
	}
	return (TupperwareList *)a;
}

TupperwareScalar* TupperwarePoolString::FindScalar(const char* name ) const
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareAtom *a = m_hashArray[hashIndex].Find(name,hash);
	if (a)
	{
		assert(a->GetAtomType()==TupperwareAtom::ATOM_SCALAR);
	}
	return (TupperwareScalar *)a;
}

// Add methods
TupperwareAggregate* TupperwarePoolString::AddAggregate(const char *name)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareAggregate *newAggregate = m_aggregate->AddAggregate(m_key,name);
	if (m_hashArray[hashIndex].Add(newAggregate,hash))
		m_numAtoms++;
	return newAggregate;
}

TupperwareList* TupperwarePoolString::AddList(const char *name)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareList *newList = m_aggregate->AddList(m_key,name);
	if (m_hashArray[hashIndex].Add(newList,hash))
		m_numAtoms++;
	return newList;
}

TupperwareList* TupperwarePoolString::AddListFloat(const char *name,const float* values, int num_values)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareList *newList = m_aggregate->AddListFloat(m_key,name,values,num_values);
	if (m_hashArray[hashIndex].Add(newList,hash))
		m_numAtoms++;
	return newList;
}

TupperwareList* TupperwarePoolString::AddListFloat(const char *name,const TupArray<float> &values)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareList *newList = m_aggregate->AddListFloat(m_key,name,values);
	if (m_hashArray[hashIndex].Add(newList,hash))
		m_numAtoms++;
	return newList;
}

TupperwareList* TupperwarePoolString::AddListInt(const char *name,const int* values, int num_values)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareList *newList = m_aggregate->AddListInt(m_key,name,values,num_values);
	if (m_hashArray[hashIndex].Add(newList,hash))
		m_numAtoms++;
	return newList;
}

TupperwareList* TupperwarePoolString::AddListInt(const char *name,const TupArray<int> &values)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareList *newList = m_aggregate->AddListInt(m_key,name,values);
	if (m_hashArray[hashIndex].Add(newList,hash))
		m_numAtoms++;
	return newList;
}

TupperwareList* TupperwarePoolString::AddListString(const char *name,const char** values, int num_values)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareList *newList = m_aggregate->AddListString(m_key,name,values,num_values);
	if (m_hashArray[hashIndex].Add(newList,hash))
		m_numAtoms++;
	return newList;
}

TupperwareScalar* TupperwarePoolString::AddScalar(const char *name)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareScalar *newScalar = m_aggregate->AddScalar(m_key,name);
	if (m_hashArray[hashIndex].Add(newScalar,hash))
		m_numAtoms++;
	return newScalar;
}

TupperwareScalar* TupperwarePoolString::AddScalarFloat(const char *name,float value)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareScalar *newScalar = m_aggregate->AddScalarFloat(m_key,name,value);
	if (m_hashArray[hashIndex].Add(newScalar,hash))
		m_numAtoms++;
	return newScalar;
}

TupperwareScalar* TupperwarePoolString::AddScalarInt(const char *name,int value)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareScalar *newScalar = m_aggregate->AddScalarInt(m_key,name,value);
	if (m_hashArray[hashIndex].Add(newScalar,hash))
		m_numAtoms++;
	return newScalar;
}

TupperwareScalar* TupperwarePoolString::AddScalarString(const char *name,const char* value)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareScalar *newScalar = m_aggregate->AddScalarString(m_key,name,value);
	if (m_hashArray[hashIndex].Add(newScalar,hash))
		m_numAtoms++;
	return newScalar;
}

TupperwareScalar* TupperwarePoolString::AddScalarBinary(const char *name,const void* value, int length)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	TupperwareScalar *newScalar = m_aggregate->AddScalarBinary(m_key,name,value,length);
	if (m_hashArray[hashIndex].Add(newScalar,hash))
		m_numAtoms++;
	return newScalar;
}

void TupperwarePoolString::AddAtom( TupperwareAtom* child)
{
	assert(child);
	unsigned int hash = GetHash(child->GetName());
	int hashIndex = hash % m_hashSize;
	m_aggregate->AddChild(child);
	if (m_hashArray[hashIndex].Add(child,hash))
		m_numAtoms++;
}

// misc methods
void TupperwarePoolString::DeleteEntry(const char* name)
{
	unsigned int hash = GetHash(name);
	int hashIndex = hash % m_hashSize;
	if (m_hashArray[hashIndex].Delete(name,hash))
		m_numAtoms--;
}

void TupperwarePoolString::RebuildLinks(void)
{
	for (TupperwareIterator i = m_aggregate->FindByKey(m_key); !i.Finished(); i.Next())
	{
		TupperwareAtom *child = i.GetAsAtom();
		assert(child);
		unsigned int hash = GetHash(child->GetName());
		int hashIndex = hash % m_hashSize;
		if (m_hashArray[hashIndex].Add(child,hash))
			m_numAtoms++;
	}
}

void TupperwarePoolString::SetHashSize(int hashSize)
{
	m_hashArray.RemoveAll();
	m_hashSize = hashSize;
	m_hashArray.SetSize(hashSize);
	RebuildLinks();
}

unsigned int TupperwarePoolString::GetHash(const char* name)
{
	unsigned int hashValue = 0;
	while(*name != '\0' )
		hashValue = (hashValue<<5) + *name++;
	return hashValue;
}

#ifdef USE_TUP_MEMORY_MANAGER
void *TupperwarePoolString::operator new(size_t nSize)
{
	return TupMemoryManager::Allocate(nSize); // call this one if running in the game engine
}

void TupperwarePoolString::operator delete(void *pBuffer)
{
	TupMemoryManager::FreeBuffer(pBuffer);
}
#endif
