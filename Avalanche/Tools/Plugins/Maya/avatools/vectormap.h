#pragma warning (disable : 4786)

#ifndef __VectorMap_H
#define __VectorMap_H

#ifndef __Vector_H
#include "vector.h"
#endif

struct VectorMapNode
{
	Vector m_key;
	unsigned m_value;
	bool m_used;
};

class VectorMap
{
public:

	VectorMap(double epsilon = 1e-6);
	VectorMap(const VectorMap &other);
	~VectorMap();

	void			Clear();
	unsigned		Length() const;

	bool			HasKey(Vector key);
	bool			GetKeyValue(Vector key, int &value);
	void			CreateKey(Vector key, int value);

	VectorMap		&operator = ( const VectorMap &other );

private:
	bool key_pos(Vector key, unsigned &index);
	bool find_vector(Vector key, unsigned &ret_index, unsigned start_index);
	unsigned hash_vector(double x, double y, double z);
	unsigned hash_double(double d);

	double m_epsilon;
	int m_capacity, m_full;
	VectorMapNode *m_nodes;
};

#include "vectormap.hpp"

#endif
