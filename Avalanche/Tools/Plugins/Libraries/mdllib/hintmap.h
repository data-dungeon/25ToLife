//////////////////////////////////////////////////////////////////////////////////////////
//
// HINTMAP CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma warning (disable : 4786)

#ifndef __HINTMAP_H
#define __HINTMAP_H

class HIntMap {
public:
	HIntMap();
	HIntMap(const HIntMap &map);
	~HIntMap();

	void			clear();
	void			append(const int key);
	unsigned		length() const;

	void			deleteKey(const int key);

	bool			hasKey(const int key);

	int				getKey(const int idx) const;
	void			setValue(const int key, const int value);
	int				getValue(const int key) const;

	void			setValues(std::vector<int> &keys);
	void			setValues(std::vector<int> &keys,std::vector<int> &values);

	void			commonElements(HIntMap &b,std::vector<int> &c);

	HIntMap			&operator = (const HIntMap &other);
	const bool		operator == (HIntMap &other);

private:
	unsigned _capacity;
	unsigned _full;
	unsigned char *_filled;
	int *_key;
	int *_value;
};

#include "hintmap.hpp"

#endif
