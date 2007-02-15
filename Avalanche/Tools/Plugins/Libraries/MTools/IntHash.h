#pragma once
//////////////////////////////////////////////////////////////////////////////////////////
// IntHash CLASS
//////////////////////////////////////////////////////////////////////////////////////////

#include <memory.h>

class IntHash
{
public:
	IntHash();
	IntHash(const IntHash &map);
	~IntHash();

	void			Clear();
	unsigned		Length() const;

	void        Get(const int idx,int &key,int &value);

	bool        HasKey(const int key);

	bool        FindKey(const int key,int &value) const;

	int         *FindKey(const int key);

	void        AddKey(const int key, const int value);

	void        DeleteKey(const int key);

	IntHash      &operator = (const IntHash &other);

protected:
	void			Grow(const int newcapacity);

private:
	unsigned m_capacity;
	unsigned m_full;
	unsigned char *m_filled;
	int *m_key;
	int *m_value;
};

#include "IntHash.hpp"
