#ifndef __HGROUP_H
#define __HGROUP_H

#ifndef __HSTATUS_H
#include "hstatus.h"
#endif
#ifndef __HATTR_H
#include "hattr.h"
#endif
#ifndef __HINTMAP_H
#include "hintmap.h"
#endif

class HPatchArray;

class HGroup
{
public:
	struct gflags {
		unsigned long	kIsAGP					: 1;
	} flags;


	HGroup();
	HGroup(const HGroup &other);
	~HGroup();

	void			clear();
	bool			hasDecals();

	HGroup &operator = ( const HGroup &other );
	friend std::ostream		&operator << (std::ostream &os,HGroup &group);

	void			readHGroup(std::ifstream &fin,HPatchArray *patches);

	UString				name;
	HIntMap				patchIndicies;
	std::vector<int>	decalIndicies;
	HAttr				attribute;
};

class HGroupArray
{
public:

	HGroupArray ();
	HGroupArray(const HGroupArray &other);
	~HGroupArray ();

	void			clear(void);
	unsigned		length(void) const;
	unsigned		append(const HGroup &other);

	HGroup			*get(unsigned index);
	HGroup			*begin();
	HGroup			*end();

	void			readHGroupArray(std::ifstream &fin, HPatchArray *patches);

	HGroup			operator[](unsigned index) const;
	HGroup			&operator[](unsigned index);
	HGroupArray		&operator = ( const HGroupArray &other );

	std::vector<HGroup> classList;

};

#endif