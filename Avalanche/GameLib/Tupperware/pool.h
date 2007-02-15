////////////////////////////////////////////////////////////////////////////
//
// Pool
//
// Classes to handle keeping track of atoms below a parent aggregate
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_POOL_H
#define TUPPERWARE_POOL_H

#ifndef TUPPERWARE_ARRAY_H
#include "tuparray.h"
#endif

#ifndef TUPPERWARE_MEMORY_MANAGER_H
#include "memorymanager.h"
#endif

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareList;
class TupperwareScalar;

class TupperwarePoolValueString
{
	TupperwareAggregate *m_aggregate;
	TupArray<TupperwareAtom *> m_atomArray;
	const char *m_key;

public:
	TupperwarePoolValueString(void);
	TupperwarePoolValueString(TupperwareAggregate *aggregate,const char *key);
	~TupperwarePoolValueString();

	// Find methods
	TupperwareAtom* FindAtom(int index) const;
	TupperwareAggregate* FindAggregate(int index) const;
	TupperwareList* FindList(int index) const;
	TupperwareScalar* FindScalar(int index) const;
	TupperwareAtom* FindAtom(const char* name ) const;
	TupperwareAggregate* FindAggregate(const char* name ) const;
	TupperwareList* FindList(const char* name ) const;
	TupperwareScalar* FindScalar(const char* name ) const;

	// Add methods
	TupperwareAggregate* AddAggregate(int index=-1);
	TupperwareList* AddList(int index=-1);
	TupperwareList* AddListFloat(const float* values, int num_values,int index=-1);
	TupperwareList* AddListFloat(const TupArray<float> &values,int index=-1);
	TupperwareList* AddListInt(const int* values, int num_values,int index=-1);
	TupperwareList* AddListInt(const TupArray<int> &values,int index=-1);
	TupperwareList* AddListString(const char** values, int num_values,int index=-1);
	TupperwareScalar* AddScalar(int index=-1);
	TupperwareScalar* AddScalarFloat(float value,int index=-1);
	TupperwareScalar* AddScalarInt(int value,int index=-1);
	TupperwareScalar* AddScalarString(const char* value,int index=-1);
	TupperwareScalar* AddScalarBinary(const void* value, int length,int index=-1);

	int AddAtom( TupperwareAtom* pAtom,int index=-1);
	int GetPoolSize(void);

	// misc methods
	void DeleteEntry(int index);
	void RebuildLinks(void);

#ifdef USE_TUP_MEMORY_MANAGER
	void *operator new(size_t size);
	void operator delete(void *pBuffer);
#endif

private:
	int FindEmptyEntry(void); // find an empty entry or add another
	void FitForIndex(int index); // make sure there is room for this index
	void IndexToName(int &index,char *name);
};

class TupperwarePoolHashEntry
{
	friend class TupperwarePoolString;

	TupperwareAtom *m_atom;
	unsigned int m_hash;
	TupperwarePoolHashEntry *m_next;

public:
	TupperwarePoolHashEntry(void);
	TupperwarePoolHashEntry(const TupperwarePoolHashEntry &s);
	TupperwarePoolHashEntry &operator=(const TupperwarePoolHashEntry &s);
	~TupperwarePoolHashEntry();

	TupperwareAtom *Find(const char *name,unsigned int hash) const;
	bool Compare(const char *name,unsigned int hash) const;
	bool Add(TupperwareAtom *atom,unsigned int hash);
	bool Delete(const char *name,unsigned int hash);

#ifdef USE_TUP_MEMORY_MANAGER
	void *operator new(size_t size);
	void operator delete(void *pBuffer);
#endif

};

class TupperwarePoolString
{
	TupperwareAggregate *m_aggregate;
	TupArray<TupperwarePoolHashEntry> m_hashArray;
	const char *m_key;
	int m_hashSize;
	int m_numAtoms;

public:
	TupperwarePoolString(void);
	TupperwarePoolString(TupperwareAggregate *aggregate,const char *key,int hashSize = 257);
	~TupperwarePoolString();

	// Find methods
	TupperwareAtom* FindAtom(const char* name ) const;
	TupperwareAggregate* FindAggregate(const char* name ) const;
	TupperwareList* FindList(const char* name ) const;
	TupperwareScalar* FindScalar(const char* name ) const;

	// Add methods
	TupperwareAggregate* AddAggregate(const char *name);
	TupperwareList* AddList(const char *name);
	TupperwareList* AddListFloat(const char *name,const float* values, int num_values);
	TupperwareList* AddListFloat(const char *name,const TupArray<float> &values);
	TupperwareList* AddListInt(const char *name,const int* values, int num_values);
	TupperwareList* AddListInt(const char *name,const TupArray<int> &values);
	TupperwareList* AddListString(const char *name,const char** values, int num_values);
	TupperwareScalar* AddScalar(const char *name);
	TupperwareScalar* AddScalarFloat(const char *name,float value);
	TupperwareScalar* AddScalarInt(const char *name,int value);
	TupperwareScalar* AddScalarString(const char *name,const char* value);
	TupperwareScalar* AddScalarBinary(const char *name,const void* value, int length);
	void AddAtom(TupperwareAtom* child);

	// misc methods
	void DeleteEntry(const char* name);
	void SetHashSize(int hashSize);

#ifdef USE_TUP_MEMORY_MANAGER
	void *operator new(size_t size);
	void operator delete(void *pBuffer);
#endif

private:
	void RebuildLinks(void);
	static unsigned int GetHash(const char* name);

};


#endif
