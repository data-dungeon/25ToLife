#ifndef __IntIntMap_H
#define __IntIntMap_H

class IntIntMap {
public:
	IntIntMap();
	~IntIntMap();

	void			Clear();
	void			SetValue(int key1, int key2, int value);
	int				GetValue(int key1, int key2);
	bool			HasKey(int key1, int key2);

	IntIntMap		&operator = ( const IntIntMap other );

private:
	unsigned m_capacity;
	unsigned m_full;

	unsigned char *m_filled;
	int *m_key1;
	int *m_key2;
	int *m_value;
};

#include "intintmap.hpp"

#endif
