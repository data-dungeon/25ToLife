////////////////////////////////////////////////////////////////////////////
//
// Keeper
//
// classes to handle keeping track of unique arrays of information
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_KEEPER_H
#define TUPPERWARE_KEEPER_H

#ifndef TUPPERWARE_ARRAY_H
#include "tuparray.h"
#endif

#ifndef TUPPERWARE_HASH_H
#include "Hash.h"
#endif

class KeeperHashEntry
{
	int m_index;
	unsigned int m_hashValue;
	KeeperHashEntry *m_pNext;

public:
	KeeperHashEntry(void);
	KeeperHashEntry(int index,unsigned int hashValue,KeeperHashEntry *pNext);
	~KeeperHashEntry();

	// copy constructor
	KeeperHashEntry(const KeeperHashEntry& src);
	// assignment operator
	const KeeperHashEntry& operator=(const KeeperHashEntry& src);

	int GetIndex(void) const;
	unsigned int GetHash(void) const;
	KeeperHashEntry *GetNextPtr(void) const;

	void SetIndex(int index);
	void SetHash(unsigned int hashValue);
	void SetNextPtr(KeeperHashEntry *pNext);
};

template <typename T>
class Keeper
{
	TupArray<T> m_vArray;
	TupArray<KeeperHashEntry *> m_hashArray;
	bool m_powerOf2;

public:
	Keeper(int hashTableSize = 256);
	~Keeper();

	// copy constructor
	Keeper(const Keeper& src);
	// assignment operator
	const Keeper& operator=(const Keeper& src);

	const T& operator[](int nIndex) const;
	T& operator[](int nIndex);

	int Add(const T& entry);
	void Add(const Keeper<T> &keeper);
	void Add(const TupArray<T> &array);
	void Add(const T *pArray,int numEntries);
	int GetIndex(const T& entry) const;
	const T* GetData(void) const;
	T* GetData(void);
	const TupArray<T> &GetArray(void) const;
	TupArray<T> &GetArray(void);
	void RemoveAll(void);
	int GetSize(void) const;
	void SetHashTableSize(int newSize);
	int GetHashTableSize(void) const;
private:
	void SetPowerOf2(void);
};

#include "keeper.hpp"

#endif