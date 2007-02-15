#pragma warning (disable : 4786)

#ifndef __HVECTORMAP_H
#define __HVECTORMAP_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif

struct HVectorMapNode
{
	HVector m_key;
	unsigned m_value;
	bool m_used;
};

class HVectorMap
{
public:

	HVectorMap(double epsilon = 1e-6);
	HVectorMap(HVectorMap& other);
	~HVectorMap();

	void			clear();
	unsigned		length() const;

	bool			has_key(HVector key, int &value);
	void			set_value(HVector key, int value);

	HVectorMap			&operator = ( const HVectorMap &other );

	friend std::ostream		&operator << (std::ostream &os,HVectorMap &vmap);

private:
	bool key_pos(HVector key, unsigned &index);
	bool find_vector(HVector key, unsigned &ret_index, unsigned start_index);
	unsigned hash_vector(double x, double y, double z);
	unsigned hash_double(double d);

	double m_epsilon;
	int m_capacity, m_full;
	HVectorMapNode *m_nodes;
};

#endif
