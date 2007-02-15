//////////////////////////////////////////////////////////////////////////////////////////
// IntMap CLASS
//////////////////////////////////////////////////////////////////////////////////////////

#pragma warning (disable : 4786)

#ifndef __IntMap_H
#define __IntMap_H

#include <memory.h>

class IntMap {
public:
	IntMap();
	IntMap(const IntMap &map);
	~IntMap();

	void			clear();
	void			append(const int key);
	unsigned		length() const;

	bool			hasKey(const int key);
	int				getKey(const int idx) const;

	void			setValue(const int key, const int value);
	int				getValue(const int key) const;

	IntMap			&operator = (const IntMap &other);
	const bool		operator == (IntMap &other);

private:
	unsigned _capacity;
	unsigned _full;
	unsigned char *_filled;
	int *_key;
	int *_value;
};

#include "intmap.hpp"

#endif
