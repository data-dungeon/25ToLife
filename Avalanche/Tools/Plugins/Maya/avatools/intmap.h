//////////////////////////////////////////////////////////////////////////////////////////
// IntMap CLASS
//////////////////////////////////////////////////////////////////////////////////////////

#pragma warning (disable : 4786)

#ifndef __IntMap_H
#define __IntMap_H

#include <memory.h>

class IntMap
{
public:
	IntMap();
	IntMap(const IntMap &map);
	~IntMap();

	void			Clear();
	unsigned		Length() const;

	void			GetKeyValue(const int idx,int &key,int &value);

	bool			HasKey(const int key);

	bool			GetValue(const int key,int &value);

	int				*GetValuePtr(const int key);

	void			SetValue(const int key, const int value);

	void			DeleteKey(const int key);

	IntMap			&operator = (const IntMap &other);

protected:
	void			Grow(const int newcapacity);

private:
	unsigned m_capacity;
	unsigned m_full;
	unsigned char *m_filled;
	int *m_key;
	int *m_value;
};

#include "intmap.hpp"

#endif
