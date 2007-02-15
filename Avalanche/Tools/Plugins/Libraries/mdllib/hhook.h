//////////////////////////////////////////////////////////////////////////////////////////
//
// HHOOK CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HHOOK_H
#define __HHOOK_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif

class HHook
{
public:
	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS FLAGS
	////////////////////////////////////////////////////////////////////////////////////////

	struct hflags {
		unsigned long	kIshook					: 1,
						kIshookbase				: 1,
						kIshookbaseneighbor		: 1;
	} flags;

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS METHODS
	////////////////////////////////////////////////////////////////////////////////////////

	HHook();
	HHook(const HHook &other);
	~HHook();

	void				clear();

	HHook				&operator = (const HHook &other);
	friend std::ostream		&operator << (std::ostream &os,HHook &hook);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	unsigned index;
	unsigned vindex;
	unsigned hookidx;
	unsigned hookbaseidx;
	std::vector<int> hookbaseneighboridx;
};

class HHookArray
{
public:
	HHookArray ();
	~HHookArray ();

	void				clear(void);
	unsigned			length(void) const;
	void				resize(const unsigned s);
	unsigned			append(HHook other);

	HHook				*get(unsigned index);
	HHook				*begin();
	HHook				*end();

	HHook				operator[]( unsigned index ) const;
 	HHook&				operator[]( unsigned index );
	HHookArray			&operator = ( const HHookArray &other );

	friend std::ostream		&operator << (std::ostream &os,HHookArray &earray);

	std::vector<HHook> classList;
};

#endif
