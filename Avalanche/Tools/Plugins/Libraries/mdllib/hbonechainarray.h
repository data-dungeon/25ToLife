//////////////////////////////////////////////////////////////////////////////////////////
//
// HBONECHAINARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HBONECHAINARRAY_H
#define __HBONECHAINARRAY_H

#ifndef __HBONECHAIN_H
#include "hbonechain.h"
#endif

class HBoneArray;
class HPolyArray;

class HBoneChainArray
{
public:
	HBoneChainArray ();
	~HBoneChainArray ();

	void				clear(void);
	unsigned			length(void) const;
	void				resize(const unsigned s);
	unsigned			append(HBoneChain &other);
	unsigned			addEmptyChain(HBoneArray &bones) { 	classList.push_back(HBoneChain(&bones)); return classList.size()-1; };

	HBoneChain			*get(unsigned index);
	HBoneChain			*begin();
	HBoneChain			*end();

	void				DestroyControlBones(HBoneChainArray &bonechainarray);

	void				readHBoneChainArray(std::ifstream &fin,HPolyArray &polys,HBoneArray &bones);

	HBoneChain			operator[]( unsigned index ) const;
 	HBoneChain			&operator[]( unsigned index );
	HBoneChainArray		&operator = ( const HBoneChainArray &other );

	friend std::ostream		&operator << (std::ostream &os,HBoneChainArray &other);

	std::vector<HBoneChain> classList;


};

#endif