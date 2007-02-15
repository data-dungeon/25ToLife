//////////////////////////////////////////////////////////////////////////////////////////
//
// HBONE ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HBONEARRAY_H
#define __HBONEARRAY_H

#ifndef __HBONE_H
#include "hbone.h"
#endif

class HBoneArray
{
public:
	HBoneArray ();
	~HBoneArray ();

	void				clear(void);
	unsigned			length(void) const;
	void				resize(const unsigned s);
	unsigned			append(HBone &other);

	HBone				*get(unsigned index);
	HBone				*begin();
	HBone				*end();

	unsigned			addBone(HBone &other) { return append(other); };
	unsigned			addBoneWParent(unsigned parent,HBone &other) { other.flags.kHasParent=1; other.parentIndex = parent; return append(other); };

	HBone				operator[]( unsigned index ) const;
 	HBone				&operator[]( unsigned index );
	HBoneArray			&operator = ( const HBoneArray &other );

	friend std::ostream		&operator << (std::ostream &os,HBoneArray &other);

	std::vector<HBone> classList;
};

#endif