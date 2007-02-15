#ifndef __IntIntHash_H
#define __IntIntHash_H

class IntIntHash
{
public:
	IntIntHash();
	~IntIntHash();

	void			Clear();
	void			AddKey(int key1, int key2, int value);
	bool			FindKey(int key1, int key2, int &value);
	bool			HasKey(int key1, int key2);
	void			DeleteKey(const int key1,const int key2);

	IntIntHash		&operator = ( const IntIntHash &other );

private:
	unsigned m_capacity;
	unsigned m_full;

	unsigned char *m_filled;
	int *m_key1;
	int *m_key2;
	int *m_value;
};

#include "intinthash.hpp"

#endif
