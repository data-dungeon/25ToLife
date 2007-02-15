/////////////////////////////////////////////////////////////////////////////////////////
//	HINTMAP INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////

inline HIntMap::HIntMap() {
	_capacity = 64;
	_full = 0;
	_filled = new unsigned char[_capacity / 8];
	memset(_filled, 0, _capacity / 8);
	_key = new int[_capacity];
	_value = new int[_capacity];
}

inline HIntMap::HIntMap(const HIntMap &other) {
	_capacity = other._capacity;
	_full = other._full;
	_filled = new unsigned char[_capacity / 8];
	_key = new int[_capacity];
	_value = new int[_capacity];
	memcpy(_filled, other._filled, _capacity / 8);
	memcpy(_key, other._key, _capacity * sizeof(int));
	memcpy(_value, other._value, _capacity * sizeof(int));
}

inline HIntMap::~HIntMap() {
	delete[] _filled;
	delete[] _key;
	delete[] _value;
}	

inline void HIntMap::clear() {
	memset(_filled, 0, _capacity / 8);
	_full = 0;
}

inline void HIntMap::append(const int key) {
	if (!hasKey(key))
	 setValue(key,_full+1);
}

inline unsigned HIntMap::length() const {
	return _full;	
}

inline int HIntMap::getKey(const int idx) const {
	int at = 0;
	int count = -1;

	while (at < (int)_capacity) {
		if (_filled[at >> 3] & (1 << (at & 7))) {
			count++;
			if (count == idx) {
				return _key[at];
			}
		}
		at++;
	}
	return 0;
}

inline void HIntMap::deleteKey(const int key)
{
	int at;

	unsigned char *oldm_filled;
	int *oldm_key;
	int *oldm_value;
	int oldm_capacity;

	oldm_filled = _filled;
	oldm_key = _key;
	oldm_value = _value;
	oldm_capacity = _capacity;

	if (_full < _capacity / 4)
		_capacity /= 8;

	if (_capacity < 64)
		_capacity = 64;

	_filled = new unsigned char[_capacity / 8];
	memset(_filled, 0, _capacity / 8);
	_key = new int[_capacity];
	_value = new int[_capacity];

	_full = 0;

	for (at = 0; at < oldm_capacity; at++) {
		if (oldm_filled[at / 8] & (1 << (at & 7))) {
			if (oldm_key[at]!=key)
				setValue(oldm_key[at], oldm_value[at]);
		}
	}

	delete[] oldm_filled;
	delete[] oldm_key;
	delete[] oldm_value;
}

inline int HIntMap::getValue(const int key) const {
	int at;

	at = key & (_capacity - 1);

	/*  This check is redundant with the loop below, but it will handle the common-case faster.  */
	if ((_key[at] == key) && (_filled[at >> 3] & (1 << (at & 7)))) {
		return _value[at];
	}

	for (; _filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (_capacity - 1)) {
		if (_key[at] == key) {
			return _value[at];
		}
	}

	return 0;
}

inline void HIntMap::setValue(const int key, const int value) {
	int at;

	/*  Once we get to one-fourth full, we want to increase capacity by a factor of eight to keep performance good.  */
	if (_full >= _capacity / 4) {
		unsigned char *old_filled;
		int *old_key;
		int *old_value;
		int old_capacity;

		old_filled = _filled;
		old_key = _key;
		old_value = _value;
		old_capacity = _capacity;

		_filled = new unsigned char[_capacity / 8 * 8];
		memset(_filled, 0, _capacity / 8 * 8);
		_key = new int[_capacity * 8];
		_value = new int[_capacity * 8];
		_capacity *= 8;
		_full = 0;

		for (at = 0; at < old_capacity; at++) {
			if (old_filled[at / 8] & (1 << (at & 7))) {
				setValue(old_key[at], old_value[at]);
			}
		}

		delete[] old_filled;
		delete[] old_key;
		delete[] old_value;
	}

	for (at = key & (_capacity - 1); _filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (_capacity - 1)) {
		/* nothing */
	}

	_full++;
	_filled[at / 8] |= 1 << (at & 7);
	_key[at] = key;
	_value[at] = value;
}


inline bool HIntMap::hasKey(const int key) {
	int at;
	for (at = key & (_capacity - 1); _filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (_capacity - 1)) {
		if (_key[at] == key)
			return true;
	}
	return false;
}

inline HIntMap &HIntMap::operator = (const HIntMap &other) {
	if (_filled == other._filled)
		return *this;

	delete[] _filled;
	delete[] _key;
	delete[] _value;

	_capacity = other._capacity;
	_full = other._full;

	_filled = new unsigned char[_capacity / 8];
	_key = new int[_capacity];
	_value = new int[_capacity];

	memcpy(_filled, other._filled, _capacity / 8);
	memcpy(_key, other._key, _capacity * sizeof(int));
	memcpy(_value, other._value, _capacity * sizeof(int));

	return *this;
}

inline const bool HIntMap::operator == (HIntMap &other) {
	unsigned key;
	bool exists = false;

	for (unsigned i = 0; i < length(); i++) {
		key = this->getKey(i);
		exists = other.hasKey(key);
		if (!exists)
			return false;
	}
	return true;
}
/*
inline std::ostream &operator << (std::ostream &os, HIntMap &im){
	os << "[ ";
	unsigned debug = im.length();
	for (unsigned i = 0; i < im.length(); i++)
		os << im.getKey(i) << " ";
	os << "]";
	return os;
}
*/