#include "stdafx.h"
#include "hintmap.h"

void HIntMap::setValues(std::vector<int> &keys) {
	for (unsigned i = 0; i < keys.size(); i++)
		setValue(keys[i], i);
}

void HIntMap::setValues(std::vector<int> &keys,std::vector<int> &values) {
	for (unsigned i = 0; i < keys.size(); i++)
		setValue(keys[i], values[i]);
}

void HIntMap::commonElements(HIntMap &b,std::vector<int> &c) {
	bool exists;
	int value;
	c.clear();
	for (unsigned i = 0; i < this->length(); i++) {
		value = this->getKey(i);
		exists = b.hasKey(value);
		if (exists)
			c.push_back(value);
	}
}
