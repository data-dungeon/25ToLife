/////////////////////////////////////////////////////////////////////////////////////////
//	IntMap INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////

inline IntMap::IntMap() {
	_capacity = 64;
	_full = 0;
	_filled = new unsigned char[_capacity / 8];
	memset(_filled, 0, _capacity / 8);
	_key = new int[_capacity];
	_value = new int[_capacity];
}

inline IntMap::IntMap(const IntMap &other) {
	_capacity = other._capacity;
	_full = other._full;
	_filled = new unsigned char[_capacity / 8];
	_key = new int[_capacity];
	_value = new int[_capacity];
	memcpy(_filled, other._filled, _capacity / 8);
	memcpy(_key, other._key, _capacity * sizeof(int));
	memcpy(_value, other._value, _capacity * sizeof(int));
}

inline IntMap::~IntMap() {
	delete[] _filled;
	delete[] _key;
	delete[] _value;
}	

inline void IntMap::clear() {
	memset(_filled, 0, _capacity / 8);
	_full = 0;
}

inline void IntMap::append(const int key) {
	if (!hasKey(key))
	 setValue(key,_full+1);
}

inline unsigned IntMap::length() const {
	return _full;	
}

inline int IntMap::getKey(const int idx) const {
	int at = 0;
	int count = -1;

	while ((unsigned)at < _capacity) {
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

inline int IntMap::getValue(const int key) const {
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

inline void IntMap::setValue(const int key, const int value) {
	int at;

	if (hasKey(key))
		return;


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


inline bool IntMap::hasKey(const int key) {
	int at;
	for (at = key & (_capacity - 1); _filled[at / 8] & (1 << (at & 7)); at = (at + 1) & (_capacity - 1)) {
		if (_key[at] == key)
			return true;
	}
	return false;
}

inline IntMap &IntMap::operator = (const IntMap &other) {
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

inline const bool IntMap::operator == (IntMap &other) {
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
