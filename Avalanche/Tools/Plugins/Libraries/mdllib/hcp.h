//////////////////////////////////////////////////////////////////////////////////////////
//
//	HCP CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HCP_H
#define __HCP_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif
#ifndef __HBIAS_H
#include "hbias.h"
#endif
#ifndef __HHOOK_H
#include "hhook.h"
#endif

class HSplineArray;

class HCP
{
public:
	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS FLAGS
	////////////////////////////////////////////////////////////////////////////////////////

	struct cpflags {
		unsigned long	kIsSmooth				: 1,
						kIsHashLoop				: 1,
						kIsLoop					: 1,
						kIsHook					: 1,
						kIsHookbase				: 1,
						kIsHookbaseNeighbor		: 1,
						kIs5PointPatchVertex    : 1,
						kIsHead					: 1,
						kIsTail					: 1,
						kHasParent				: 1,
						kHasChild				: 1,
						kIsInstance				: 1;
	} flags;

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS METHODS
	////////////////////////////////////////////////////////////////////////////////////////

	HCP();
	HCP(const HCP &other);
	~HCP();

	void				clear();

	void				create(HVector pos);
	void				create(HCP other);
	void				createInstance(unsigned cpidx);
	void				createInstance(HCP other);
	void				create_looping_cp(HCP other);

//	USE HASH'S NUMBERING CONVENTION METHODS
	void				create_UHNC(HVector pos);
	void				createInstance_UHNC(unsigned cpidx);

	void				initialize_hook(HHook hook);

	void				setFlags(unsigned long flags);
	void				setFlags(HCP::cpflags flags);
	unsigned long		getFlags();
	void				setFlagsFromOldFlags(unsigned long flags);

	void				readHCP(std::ifstream &fin);

	HCP					*next();
	HCP					*previous();
	HCP					*head();
	HCP					*tail();
	bool				isDone();

	HCP &operator = ( const HCP &other );
	friend std::ostream		&operator << (std::ostream &os,HCP &cp);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	unsigned index;
	int vindex,cpindex;
	unsigned instanceIndex;
	unsigned hookIndex;
	unsigned parentIndex,childIndex;
	unsigned splineIndex;

	HVector it,ot;		// incoming tangents, outgoing tangents
	HBias inBias;		//incoming bias
	HBias outBias;		//outgoing bias

	HSplineArray *splineArray;
};

#endif