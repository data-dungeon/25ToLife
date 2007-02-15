/////////////////////////////////////////////////////////////////////////////////////////
//	IntHash INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////

inline IntHash::IntHash()
{
	m_capacity = 0;
	m_full = 0;
	m_filled = NULL;
	m_key = NULL;
	m_value = NULL;
}

inline IntHash::IntHash(const IntHash &other)
{
	*this = other;	
}

inline IntHash::~IntHash()
{
	if (m_filled)
		delete[] m_filled;
	if (m_key)
		delete[] m_key;
	if (m_value)
		delete[] m_value;

}	

inline void IntHash::Clear()
{
	if (m_filled)
		delete[] m_filled;
	if (m_key)
		delete[] m_key;
	if (m_value)
		delete[] m_value;

	m_capacity = 0;
	m_full = 0;

	m_filled = NULL;
	m_key = NULL;
	m_value = NULL;
}

inline unsigned IntHash::Length() const
{
	return m_full;	
}

inline void IntHash::Get(const int idx,int &key,int &value)
{
	int at = 0;
	int count = -1;

	while ((unsigned)at < m_capacity)
	{
		if (m_filled[at >> 3] & (1 << (at & 7))) 
		{
			count++;
			if (count == idx)
			{
				key = m_key[at];
				value = m_value[at];
			}
		}
		at++;
	}
}

inline bool IntHash::FindKey(const int key,int &value) const
{
	if (m_capacity==0)
		return false;

	int at;
	
	at = key & (m_capacity - 1);

	/*  This check is redundant with the loop below, but it will handle the common-case faster.  */
	if ((m_key[at] == key) && (m_filled[at >> 3] & (1 << (at & 7))))
	{
		value = m_value[at];
		return true;
	}

	for (; m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1))
	{
		if (m_key[at] == key)
		{
			value = m_value[at];
			return true;
		}
	}

	return false;
}

inline int *IntHash::FindKey(const int key)
{
	if (m_capacity==0)
		return false;

	int at;

	at = key & (m_capacity - 1);

	/*  This check is redundant with the loop below, but it will handle the common-case faster.  */
	if ((m_key[at] == key) && (m_filled[at >> 3] & (1 << (at & 7))))
	{
		return &m_value[at];
	}

	for (; m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1))
	{
		if (m_key[at] == key)
		{
			return &m_value[at];
		}
	}

	return NULL;
}

inline void IntHash::DeleteKey(const int key)
{
	int at;

	unsigned char *oldm_filled;
	int *oldm_key;
	int *oldm_value;
	int oldm_capacity;

	oldm_filled = m_filled;
	oldm_key = m_key;
	oldm_value = m_value;
	oldm_capacity = m_capacity;

	if (m_full < m_capacity / 4)
		m_capacity /= 8;

	if (m_capacity < 64)
		m_capacity = 64;

	m_filled = new unsigned char[m_capacity / 8];
	memset(m_filled, 0, m_capacity / 8);
	m_key = new int[m_capacity];
	m_value = new int[m_capacity];

	m_full = 0;

	for (at = 0; at < oldm_capacity; at++)
	{
		if (oldm_filled[at / 8] & (1 << (at & 7)))
		{
			if (oldm_key[at]!=key)
				AddKey(oldm_key[at], oldm_value[at]);
		}
	}

	delete[] oldm_filled;
	delete[] oldm_key;
	delete[] oldm_value;

	if (m_full==0)
		Clear();
}

inline void IntHash::Grow(const int newcapacity)
{
	int at;

	unsigned char *oldm_filled;
	int *oldm_key;
	int *oldm_value;
	int oldm_capacity;

	oldm_filled = m_filled;
	oldm_key = m_key;
	oldm_value = m_value;
	oldm_capacity = m_capacity;

	m_filled = new unsigned char[newcapacity];
	memset(m_filled, 0, newcapacity);
	m_key = new int[newcapacity];
	m_value = new int[newcapacity];
	m_capacity = newcapacity;
	m_full = 0;

	for (at = 0; at < oldm_capacity; at++)
	{
		if (oldm_filled[at / 8] & (1 << (at & 7)))
		{
			AddKey(oldm_key[at], oldm_value[at]);
		}
	}

	delete[] oldm_filled;
	delete[] oldm_key;
	delete[] oldm_value;
}

inline void IntHash::AddKey(const int key, const int value)
{
	int at;

	/*  Once we get to one-fourth full, we want to increase capacity by a factor of eight to keep performance good.  */
	if (m_full >= m_capacity / 4)
	{
		if (m_capacity == 0)
			Grow(64);
		else
			Grow(m_capacity * 8);
	}

	for (at = key & (m_capacity - 1); m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1))
	{
		/* nothing */
	}

	m_full++;
	m_filled[at / 8] |= 1 << (at & 7);
	m_key[at] = key;
	m_value[at] = value;
}


inline bool IntHash::HasKey(const int key)
{
	if (m_capacity==0)
		return false;
	
	int at;
	for (at = key & (m_capacity - 1); m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1))
	{
		if (m_key[at] == key)
			return true;
	}
	return false;
}

inline IntHash &IntHash::operator = (const IntHash &other)
{
	if (m_filled == other.m_filled)
		return *this;

	if (m_filled)
		delete[] m_filled;
	if (m_key)
		delete[] m_key;
	if (m_value)
		delete[] m_value;

	m_capacity = other.m_capacity;
	m_full = other.m_full;

	m_filled = new unsigned char[m_capacity / 8];
	m_key = new int[m_capacity];
	m_value = new int[m_capacity];

	memcpy(m_filled, other.m_filled, m_capacity / 8);
	memcpy(m_key, other.m_key, m_capacity * sizeof(int));
	memcpy(m_value, other.m_value, m_capacity * sizeof(int));

	return *this;
}
