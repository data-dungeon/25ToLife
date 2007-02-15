//////////////////////////////////////////////////////////////////////////////////////////
//
// HBONE CHAIN CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HBONECHAIN_H
#define __HBONECHAIN_H

#ifndef __HBONE_H
#include "HBone.h"
#endif


class HBoneChain
{
public:
	HBoneChain();
	HBoneChain (HBoneArray *other);
	~HBoneChain();

	void				clear(HBoneArray *other);
	unsigned			length(void) const;
	void				resize(const unsigned s);
	unsigned			append(unsigned boneindex);
	void				remove(unsigned index);

	HBone				*get(unsigned index);
	unsigned			getIndex(unsigned index);

	HBone				*begin();
	HBone				*end();

	HBone				operator[]( unsigned index ) const;
 	HBone&				operator[]( unsigned index );
	HBoneChain			&operator = ( const HBoneChain &other );

	friend std::ostream		&operator << (std::ostream &os,HBoneChain &other);

	HBoneArray *boneArray;

	std::vector<unsigned> classList;
};

#endif