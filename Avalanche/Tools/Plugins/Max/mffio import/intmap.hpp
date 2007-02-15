/////////////////////////////////////////////////////////////////////////////////////////
//	IntMap INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////

inline IntMap::IntMap() {
	m_capacity = 64;
	m_full = 0;

	m_filled = new unsigned char[m_capacity / 8];
	m_key = new int[m_capacity];
	m_value = new int[m_capacity];

	memset(m_filled, 0, m_capacity / 8);
}

inline IntMap::IntMap(const IntMap &other) {
	if (m_filled == other.m_filled)
		return;

	delete[] m_filled;
	delete[] m_key;
	delete[] m_value;

	m_capacity = other.m_capacity;
	m_full = other.m_full;

	m_filled = new unsigned char[m_capacity / 8];
	m_key = new int[m_capacity];
	m_value = new int[m_capacity];

	memcpy(m_filled, other.m_filled, m_capacity / 8);
	memcpy(m_key, other.m_key, m_capacity * sizeof(int));
	memcpy(m_value, other.m_value, m_capacity * sizeof(int));
}

inline IntMap::~IntMap() {
	delete[] m_filled;
	delete[] m_key;
	delete[] m_value;
}	

inline void IntMap::Clear() {
//	delete[] m_filled;
//	delete[] m_key;
//	delete[] m_value;

	m_capacity = 64;
	m_full = 0;

	m_filled = new unsigned char[m_capacity / 8];
	m_key = new int[m_capacity];
	m_value = new int[m_capacity];

	memset(m_filled, 0, m_capacity / 8);
}

inline unsigned IntMap::Length() const {
	return m_full;	
}

inline int IntMap::GetKey(const int idx) {
	int at = 0;
	int count = -1;

	while ((unsigned)at < m_capacity) {
		if (m_filled[at >> 3] & (1 << (at & 7))) {
			count++;
			if (count == idx) {
				return m_key[at];
			}
		}
		at++;
	}
	return 0;
}

inline int IntMap::GetValue(const int key)
{
	int at;

	at = key & (m_capacity - 1);

	/*  This check is redundant with the loop below, but it will handle the common-case faster.  */
	if ((m_key[at] == key) && (m_filled[at >> 3] & (1 << (at & 7)))) {
		return m_value[at];
	}

	for (; m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1)) {
		if (m_key[at] == key) {
			return m_value[at];
		}
	}

	return -1;
}

inline void IntMap::DeleteKey(const int key)
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

	for (at = 0; at < oldm_capacity; at++) {
		if (oldm_filled[at / 8] & (1 << (at & 7))) {
			if (oldm_key[at]!=key)
				SetValue(oldm_key[at], oldm_value[at]);
		}
	}

	delete[] oldm_filled;
	delete[] oldm_key;
	delete[] oldm_value;
}

inline void IntMap::SetValue(const int key, const int value) {
	if (HasKey(key))
		return;

	int at;

	/*  Once we get to one-fourth full, we want to increase capacity by a factor of eight to keep performance good.  */
	if (m_full >= m_capacity / 4)
	{
		unsigned char *oldm_filled;
		int *oldm_key;
		int *oldm_value;
		int oldm_capacity;

		oldm_filled = m_filled;
		oldm_key = m_key;
		oldm_value = m_value;
		oldm_capacity = m_capacity;

		m_filled = new unsigned char[m_capacity / 8 * 8];
		memset(m_filled, 0, m_capacity / 8 * 8);
		m_key = new int[m_capacity * 8];
		m_value = new int[m_capacity * 8];
		m_capacity *= 8;
		m_full = 0;

		for (at = 0; at < oldm_capacity; at++) {
			if (oldm_filled[at / 8] & (1 << (at & 7))) {
				SetValue(oldm_key[at], oldm_value[at]);
			}
		}

		delete[] oldm_filled;
		delete[] oldm_key;
		delete[] oldm_value;
	}

	for (at = key & (m_capacity - 1); m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1)) {
		/* nothing */
	}

	m_full++;
	m_filled[at / 8] |= 1 << (at & 7);
	m_key[at] = key;
	m_value[at] = value;
}


inline bool IntMap::HasKey(const int key) {
	int at;
	for (at = key & (m_capacity - 1); m_filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (m_capacity - 1))
	{
		if (m_key[at] == key)
			return true;
	}
	return false;
}

inline IntMap &IntMap::operator = (const IntMap &other) {
	if (m_filled == other.m_filled)
		return *this;

	delete[] m_filled;
	delete[] m_key;
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
