#include "stdafx.h"
#include "hintarray.h"

HIntArray &HIntArray::operator = ( HIntMap &map ) {
	classList.clear();
	for (unsigned i = 0; i < map.length(); i++)
		classList.push_back(map.getKey(i));
	return *this;
}

