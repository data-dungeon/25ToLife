////////////////////////////////////////////////////////////////////////////
//
// compareator
//
// A class for doing quick compares
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_COMPAREATOR_H
#define TUPPERWARE_COMPAREATOR_H

#ifndef TUPPERWARE_ARRAY_H
#include "tuparray.h"
#endif

// This class is used for setting up a multiple stage compare
// if the first pair of values are the same it will continue searching through the arrays
// to determine result
template <class T> class TupCompareator
{
public:
	TupArray<T> m_array;
	TupCompareator(void);
	TupCompareator(const TupArray<T> &array);

	bool operator==(const TupCompareator& other) const;
	bool operator<=(const TupCompareator& other) const;
	bool operator>=(const TupCompareator& other) const;
	bool operator<(const TupCompareator& other) const;
	bool operator>(const TupCompareator& other) const;
};

// Include the implementation
#include "compareator.hpp"

#endif
////////////////////////////////////////////////////////////////////////////
