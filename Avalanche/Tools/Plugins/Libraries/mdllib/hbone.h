//////////////////////////////////////////////////////////////////////////////////////////
//
// HBONE CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HBONE_H
#define __HBONE_H

#ifndef __UBERSTRING_H
#include "uberstring.h"
#endif
#ifndef __HVECTOR_H
#include "hvector.h"
#endif
#ifndef __HCOLOR_H
#include "hcolor.h"
#endif
#ifndef __HQUATERNION_H
#include "hquaternion.h"
#endif

class HQuaternion;
class HBoneArray;
class HBoneChainArray;

class HBone
{
public:
	struct bflags {
		unsigned long	kHasIKParent				: 1,
						kHasParent					: 1,
						kHasChainIndex				: 1;													  ;
	} flags;

	UString				name;
	std::vector<int>	cps;
	std::vector<int>	vids;
	HVector				pivot,end;
	HVector				rollhandle;

	HQuaternion			rotate;
	double				length;
	double				falloff;
	HColor				color;

	unsigned			ikParentIndex,parentIndex;

	int					chainIndex;

	HBoneArray			*boneArray;

	bool				V9Bone;

//////////////////////////////////////////////////////////////////////////////////////////
// CLASS METHODS
//////////////////////////////////////////////////////////////////////////////////////////
	
	HBone();
	HBone(const HBone &other);
	~HBone();

	HVector				getPivot() { return pivot; };
	HVector				getRollHandle();
	HVector				getEnd();

	void				clear();

	HBone				*previous();

	void				setChainIndex(int index) { flags.kHasChainIndex = 1; chainIndex = index; };

	HBone				&operator = ( const HBone &other );
	friend std::ostream		&operator << (std::ostream &os,HBone &other);
};

#endif
