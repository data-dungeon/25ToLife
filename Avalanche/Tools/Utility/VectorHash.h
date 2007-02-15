#ifndef __VectorHash_H
#define __VectorHash_H

struct VectorHashNode
{
	Vector m_key;
	unsigned m_value;
	bool m_used;
};

class VectorHash
{
public:

	VectorHash(double epsilon = 1e-6);
	VectorHash(const VectorHash &other);
	~VectorHash();

	void			Clear();
	unsigned		Length() const;

	bool			HasKey(Vector key);
	bool			FindKey(Vector key, int &value);
	void			AddKey(Vector key, int value);
	double		*GetTolerance() { return &m_epsilon; }

	VectorHash		&operator = ( const VectorHash &other );

private:
	bool key_pos(Vector key, unsigned &index);
	bool find_vector(Vector key, unsigned &ret_index, unsigned start_index);
	unsigned hash_vector(double x, double y, double z);
	unsigned hash_double(double d);

	double m_epsilon;
	int m_capacity, m_full;
	VectorHashNode *m_nodes;
};

#include "VectorHash.hpp"

#endif
