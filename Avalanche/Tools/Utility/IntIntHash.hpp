inline IntIntHash::IntIntHash()
{
	m_capacity = 64;
	m_full = 0;

	m_filled = new unsigned char[m_capacity / 8];
	memset(m_filled, 0, m_capacity / 8);

	m_key1 = new int[m_capacity];
	m_key2 = new int[m_capacity];
	m_value = new int[m_capacity];
}


inline IntIntHash::~IntIntHash()
{
	delete[] m_filled;
	delete[] m_key1;
	delete[] m_key2;
	delete[] m_value;
}	

inline void IntIntHash::Clear()
{
	delete[] m_filled;
	delete[] m_key1;
	delete[] m_key2;
	delete[] m_value;

	m_capacity = 64;
	m_full = 0;

	m_filled = new unsigned char[m_capacity / 8];
	memset(m_filled, 0, m_capacity / 8);

	m_key1 = new int[m_capacity];
	m_key2 = new int[m_capacity];
	m_value = new int[m_capacity];
}


inline bool IntIntHash::FindKey(int key1, int key2, int &value)
{
	int at;

	at = (key1 ^ key2) & (m_capacity - 1);

	/*  This check is redundant with the loop below, but it will handle the common-case faster.  */
	if ((m_key1[at] == key1) && (m_key2[at] == key2) && (m_filled[at >> 3] & (1 << (at & 7))))
	{
		value = m_value[at];
		return true;
	}

	for (; m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1))
	{
		if ((m_key1[at] == key1) && (m_key2[at] == key2))
		{
			value = m_value[at];
			return true;
		}
	}

	return false;
}


inline void IntIntHash::AddKey(int key1, int key2, int value)
{
	int at;

	/*  Once we get to one-fourth full, we want to increase capacity by a factor of eight to keep performance good.  */
	if (m_full >= m_capacity / 4)
	{
		unsigned char *old_filled;
		int *old_key1;
		int *old_key2;
		int *old_value;
		int old_capacity;

		old_filled = m_filled;
		old_key1 = m_key1;
		old_key2 = m_key2;
		old_value = m_value;
		old_capacity = m_capacity;

		m_filled = new unsigned char[m_capacity / 8 * 8];
		memset(m_filled, 0, m_capacity / 8 * 8);
		m_key1 = new int[m_capacity * 8];
		m_key2 = new int[m_capacity * 8];
		m_value = new int[m_capacity * 8];
		m_capacity *= 8;
		m_full = 0;

		for (at = 0; at < old_capacity; at++)
		{
			if (old_filled[at / 8] & (1 << (at & 7)))
			{
				AddKey(old_key1[at], old_key2[at], old_value[at]);
			}
		}

		delete[] old_filled;
		delete[] old_key1;
		delete[] old_key2;
		delete[] old_value;
	}

	for (at = (key1 ^ key2) & (m_capacity - 1); m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1))
	{
		/* nothing */
	}

	m_full++;
	m_filled[at / 8] |= 1 << (at & 7);
	m_key1[at] = key1;
	m_key2[at] = key2;
	m_value[at] = value;
}


inline bool IntIntHash::HasKey(int key1, int key2)
{
	int at;

	for (at = (key1 ^ key2) & (m_capacity - 1); m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1))
	{
		if ((m_key1[at] == key1) && (m_key2[at] == key2))
			return true;
	}

	return false;
}

inline void IntIntHash::DeleteKey(const int key1,const int key2)
{
	int at;

	unsigned char *old_filled;
	int *old_key1;
	int *old_key2;
	int *old_value;
	int old_capacity;

	old_filled = m_filled;
	old_key1 = m_key1;
	old_key2 = m_key2;
	old_value = m_value;
	old_capacity = m_capacity;

	if (m_full < m_capacity / 4)
		m_capacity /= 8;
	
	if (m_capacity < 64)
		m_capacity = 64;

	m_filled = new unsigned char[m_capacity / 8];
	memset(m_filled, 0, m_capacity / 8);
	m_key1 = new int[m_capacity];
	m_key2 = new int[m_capacity];
	m_value = new int[m_capacity];
	m_full = 0;

	for (at = 0; at < old_capacity; at++)
	{
		if (old_filled[at / 8] & (1 << (at & 7)))
		{
			if (!((old_key1[at]==key1)&&(old_key2[at]==key2)))
				AddKey(old_key1[at], old_key2[at], old_value[at]);
		}
	}

	delete[] old_filled;
	delete[] old_key1;
	delete[] old_key2;
	delete[] old_value;
}

inline IntIntHash &IntIntHash::operator = ( const IntIntHash &other )
{
	if (m_filled == other.m_filled)
		return *this;

	delete[] m_filled;
	delete[] m_key1;
	delete[] m_key2;
	delete[] m_value;

	m_capacity = other.m_capacity;
	m_full = other.m_full;

	m_filled = new unsigned char[m_capacity / 8];
	m_key1 = new int[m_capacity];
	m_key2 = new int[m_capacity];
	m_value = new int[m_capacity];

	memcpy(m_filled, other.m_filled, m_capacity / 8);
	memcpy(m_key1, other.m_key1, m_capacity * sizeof(int));
	memcpy(m_key2, other.m_key2, m_capacity * sizeof(int));
	memcpy(m_value, other.m_value, m_capacity * sizeof(int));

	return *this;
}
