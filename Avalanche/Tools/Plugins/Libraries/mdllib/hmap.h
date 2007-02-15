//////////////////////////////////////////////////////////////////////////////////////////
//
// HMAP CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HMAP_H
#define __HMAP_H

#ifndef __UBERSTRING_H
#include "uberstring.h"
#endif
#ifndef __HVECTOR2D_H
#include "hvector2d.h"
#endif

class HMapArray;

class HMap
{
public:
	HMap ();
	HMap(const HMap &other);
	~HMap ();

	void		clear();

	HMap		*next();
	HMap		*previous();
	HMap		*begin();
	HMap		*end();

	HMap				&operator = ( const HMap &other );
	bool				operator == ( const HMap &other ) const;
	bool				operator != ( const HMap &other ) const;
	friend std::ostream		&operator << (std::ostream &os,HMap &other);

	enum mType { COLORMAP, COOKIECUTMAP, DIFFUSEMAP, SPECULARSIZEMAP, SPECULARINTENSITYMAP, TRANSPARENCYMAP, BUMPMAP,  
       AMBIANCEMAP, DISPLACEMENTMAP, REFLECTIONMAP, NONE };

	UString				name,path;
	mType				type;
	HVector2D			repeat;

	HMapArray			*mapArray;
};

//////////////////////////////////////////////////////////////////////////////////////////
//
// HMAP ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////
typedef std::vector<HMap> mapVector;
typedef mapVector::iterator mapVectorIt;

class HMapArray
{
public:
	HMapArray ();
	~HMapArray ();

	void				clear(void);
	unsigned			length(void) const;

	unsigned			add(HMap &map);
	unsigned			append(HMap map);

	HMap				*get(unsigned index);
	HMap				*begin();
	HMap				*end();

	HMapArray			&operator = ( const HMapArray &other );
	bool				operator == ( const HMapArray &other ) const;
	bool				operator != ( const HMapArray &other ) const;
	friend std::ostream		&operator << (std::ostream &os,HMapArray &other);

	HMap				operator[](unsigned index) const;
	HMap				&operator[](unsigned index);

	std::vector<HMap> classList;
};

#endif