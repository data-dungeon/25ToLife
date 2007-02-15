/*
 *  $History: hvectormap.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Tony Jones   Date: 12/31/01   Time: 10:51a
 * Updated in $/Avalanche/tools/Plugins/Libraries/mdllib
 * 
 * *****************  Version 3  *****************
 * User: Tony Jones   Date: 12/21/01   Time: 12:13p
 * Updated in $/Avalanche/tools/Plugins/Libraries/mdllib
 * Lots of stuff......
 * 
 * *****************  Version 2  *****************
 * User: Tony Jones   Date: 10/31/01   Time: 3:56p
 * Updated in $/Avalanche/tools/Plugins/Libraries/mdllib
 * 
 * *****************  Version 1  *****************
 * User: Tony Jones   Date: 9/26/01    Time: 10:49a
 * Created in $/Avalanche/tools/Plugins/Maya/mdlio
 */

#include "Stdafx.h"
#include "hvectormap.h"


HVectorMap::HVectorMap(double epsilon) {
	m_epsilon = epsilon;
	m_capacity = 16;
	m_full = 0;
	m_nodes = new HVectorMapNode[m_capacity];

	for (int i = 0; i < m_capacity; i++) {
		m_nodes[i].m_used = 0;
	}
}

HVectorMap::HVectorMap(HVectorMap& other) {
	m_epsilon = other.m_epsilon;
	m_capacity = other.m_capacity;
	m_full = other.m_full;
	m_nodes = new HVectorMapNode[m_capacity];
	memcpy(m_nodes, other.m_nodes, sizeof(HVectorMapNode) * m_capacity);
}

HVectorMap::~HVectorMap() {
	delete[] m_nodes;
}

void HVectorMap::clear() {
	for (int i = 0; i < m_capacity; i++) {
		m_nodes[i].m_used = 0;
	}
}

unsigned HVectorMap::length() const {
	return m_full;
}

bool HVectorMap::has_key(HVector key, int &value) {
	unsigned ix;

	if (m_full == 0)
		return false;

	if (key_pos(key, ix)) {
		value = m_nodes[ix].m_value;
		return true;
	}
	return false;
}

void HVectorMap::set_value(HVector key, int value) {
	unsigned ix;

	if (key_pos(key, ix)) {
		m_nodes[ix].m_value = value;
		return;
	}

	if (m_full >= m_capacity / 4) {
		int old_capacity = m_capacity;
		HVectorMapNode *old_nodes = m_nodes;

		m_capacity *= 8;
		m_nodes = new HVectorMapNode[m_capacity];
		m_full = 0;

		for (int i = 0; i < m_capacity; i++) {
			m_nodes[i].m_used = 0;
		}
		
		for (i = 0; i < old_capacity; i++) {
			if(old_nodes[i].m_used) {
				set_value(old_nodes[i].m_key, old_nodes[i].m_value);
			}
		}

		delete[] old_nodes;
	}

	ix = hash_vector(key.x, key.y, key.z);
	while(m_nodes[ix].m_used) {
		ix++;
		if (ix >= (unsigned)m_capacity) {
			ix = 0;
		}
	}

	m_nodes[ix].m_used = 1;
	m_nodes[ix].m_key = key;
	m_nodes[ix].m_value = value;

	m_full++;
}

bool HVectorMap::key_pos(HVector key, unsigned &index) {
	if (find_vector(key, index, hash_vector(key.x - m_epsilon, key.y - m_epsilon, key.z - m_epsilon)))
		return true;
	if (find_vector(key, index, hash_vector(key.x - m_epsilon, key.y - m_epsilon, key.z + m_epsilon)))
		return true;
	if (find_vector(key, index, hash_vector(key.x - m_epsilon, key.y + m_epsilon, key.z - m_epsilon)))
		return true;
	if (find_vector(key, index, hash_vector(key.x - m_epsilon, key.y + m_epsilon, key.z + m_epsilon)))
		return true;
	if (find_vector(key, index, hash_vector(key.x + m_epsilon, key.y - m_epsilon, key.z - m_epsilon)))
		return true;
	if (find_vector(key, index, hash_vector(key.x + m_epsilon, key.y - m_epsilon, key.z + m_epsilon)))
		return true;
	if (find_vector(key, index, hash_vector(key.x + m_epsilon, key.y + m_epsilon, key.z - m_epsilon)))
		return true;
	if (find_vector(key, index, hash_vector(key.x + m_epsilon, key.y + m_epsilon, key.z + m_epsilon)))
		return true;

	return false;
}

bool HVectorMap::find_vector(HVector key, unsigned &ret_index, unsigned start_index) {
	int i = start_index;
	while(m_nodes[i].m_used) {
//		if (key.distanceTo(m_nodes[i].m_key) <= m_epsilon) {
		if (key.isWithinTolerance(m_nodes[i].m_key,m_epsilon)) {
			ret_index = i;
			return true;
		}
		i++;
		if (i >= m_capacity) {
			i = 0;
		}
	}

	return false;
}

unsigned HVectorMap::hash_vector(double x, double y, double z) {
	unsigned hx = hash_double(x);
	unsigned hy = hash_double(y);
	unsigned hz = hash_double(z);

	return (hx ^ hy ^ hz) & (m_capacity - 1);
}

unsigned HVectorMap::hash_double(double d) {
	double div = m_epsilon * 4.0;
	double v = floor(d / div) * div;
	unsigned short *s = (unsigned short *)&v;

	return s[0] ^ s[1] ^ s[2] ^ s[3];
}


HVectorMap &HVectorMap::operator = ( const HVectorMap &other ){
	if (this != &other) {
		delete[] m_nodes;

		m_epsilon = other.m_epsilon;
		m_capacity = other.m_capacity;
		m_full = other.m_full;
		m_nodes = new HVectorMapNode[m_capacity];
		memcpy(m_nodes, other.m_nodes, sizeof(HVectorMapNode) * m_capacity);
	}

	return *this;
}

std::ostream &operator << (std::ostream &os,HVectorMap &vmap){
	return os;
}

