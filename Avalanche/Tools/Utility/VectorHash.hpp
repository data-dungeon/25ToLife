//#include <math.h>

inline VectorHash::VectorHash(double epsilon)
{
	m_epsilon = epsilon;
	m_capacity = 16;
	m_full = 0;
	m_nodes = new VectorHashNode[m_capacity];

	for (int i = 0; i < m_capacity; i++)
		m_nodes[i].m_used = 0;
}

inline VectorHash::VectorHash(const VectorHash &other)
{
	if (this != &other)
	{
		delete[] m_nodes;

		m_epsilon = other.m_epsilon;
		m_capacity = other.m_capacity;
		m_full = other.m_full;
		m_nodes = new VectorHashNode[m_capacity];
		memcpy(m_nodes, other.m_nodes, sizeof(VectorHashNode) * m_capacity);
	}
}

inline VectorHash::~VectorHash()
{
	delete[] m_nodes;
}

inline void VectorHash::Clear()
{
	for (int i = 0; i < m_capacity; i++)
		m_nodes[i].m_used = 0;
}

inline unsigned VectorHash::Length() const
{
	return m_full;
}

inline bool VectorHash::HasKey(Vector key)
{
	unsigned ix;

	if (m_full == 0)
		return false;

	if (key_pos(key, ix))
		return true;

	return false;	
}

inline bool VectorHash::FindKey(Vector key, int &value)
{
	unsigned ix;

	if (m_full == 0)
		return false;

	if (key_pos(key, ix))
	{
		value = m_nodes[ix].m_value;
		return true;
	}
	return false;
}

inline void VectorHash::AddKey(Vector key, int value)
{
	unsigned ix;

	if (key_pos(key, ix))
	{
		m_nodes[ix].m_value = value;
		return;
	}

	if (m_full >= m_capacity / 4)
	{
		int old_capacity = m_capacity;
		VectorHashNode *old_nodes = m_nodes;

		m_capacity *= 8;
		m_nodes = new VectorHashNode[m_capacity];
		m_full = 0;

		for (int i = 0; i < m_capacity; i++)
			m_nodes[i].m_used = 0;
	
		for (i = 0; i < old_capacity; i++)
		{
			if(old_nodes[i].m_used)
			{
				AddKey(old_nodes[i].m_key, old_nodes[i].m_value);
			}
		}

		delete[] old_nodes;
	}

	ix = hash_vector(key.x, key.y, key.z);
	while(m_nodes[ix].m_used)
	{
		ix++;
		if (ix >= (unsigned int)m_capacity)
			ix = 0;
	}

	m_nodes[ix].m_used = 1;
	m_nodes[ix].m_key = key;
	m_nodes[ix].m_value = value;

	m_full++;
}

inline bool VectorHash::key_pos(Vector key, unsigned &index)
{
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

inline bool VectorHash::find_vector(Vector key, unsigned &ret_index, unsigned start_index)
{
	int i = start_index;
	while(m_nodes[i].m_used)
	{
      Vector direction(m_nodes[i].m_key.x-key.x,m_nodes[i].m_key.y-key.y,m_nodes[i].m_key.z-key.z);
      if (direction.LengthSq() < (m_epsilon * m_epsilon))
		{
			ret_index = i;
			return true;
		}
		i++;
		if (i >= m_capacity)
			i = 0;
	}

	return false;
}

inline unsigned VectorHash::hash_vector(double x, double y, double z)
{
	unsigned hx = hash_double(x);
	unsigned hy = hash_double(y);
	unsigned hz = hash_double(z);

	return (hx ^ hy ^ hz) & (m_capacity - 1);
}

inline unsigned VectorHash::hash_double(double d)
{
	double div = m_epsilon * 4.0;
	double v = floor(d / div) * div;
	unsigned short *s = (unsigned short *)&v;

	return s[0] ^ s[1] ^ s[2] ^ s[3];
}


inline VectorHash &VectorHash::operator = ( const VectorHash &other )
{
	if (this != &other)
	{
		delete[] m_nodes;

		m_epsilon = other.m_epsilon;
		m_capacity = other.m_capacity;
		m_full = other.m_full;
		m_nodes = new VectorHashNode[m_capacity];
		memcpy(m_nodes, other.m_nodes, sizeof(VectorHashNode) * m_capacity);
	}

	return *this;
}