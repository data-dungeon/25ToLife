#ifndef __HDECAL_H
#define __HDECAL_H

#ifndef __UBERSTRING_H
#include "uberstring.h"
#endif
#ifndef __HVECTOR2D_H
#include "hvector2d.h"
#endif
#ifndef __HSTATUS_H
#include "hstatus.h"
#endif
#ifndef __HIMAGE_H
#include "himage.h"
#endif

class HGroup;
class HGroupArray;
class HPatch;
class HPatchArray;

class HDecal
{
public:
	HDecal ();
	~HDecal ();

	enum mType { COLORMAP, TRANSPARENCYMAP, BUMPMAP, SPECULARSIZEMAP, DIFFUSEMAP,
       MIRRORMAP, AMBIANCEMAP, COOKIECUTMAP, DISPLACEMENTMAP, FRACTALMAP,
       ROUGHNESSMAP, REFLECTIONMAP, WEATHERMAP, SPECULARINTENSITYMAP, INTERNALMAP, INVALID };

	void				clear();
	void				readHDecal(std::ifstream &fin,HPatchArray *patches);
	void				readHDecalImage(std::ifstream &fin,HImageArray &images);
	HDecal::mType		getDecalTypeFlag(UString typname, HStatus status);

	bool				hasStamps();

	HDecal &operator = ( const HDecal &other );
	friend std::ostream		&operator << (std::ostream &os,HDecal &other);

	UString				decal_name;
	UString				name;//shortcutname
	unsigned			imageIndex;
	mType				type;
	float				value;
	HVector2D			repeat;
	bool				seamless;
	std::vector<int>	stampIndicies;
};

class HDecalArray
{
public:
	HDecalArray ();
	~HDecalArray ();

	void			clear(void);
	unsigned		length(void) const;
	unsigned		append(HDecal decal);

	HDecal			*get(unsigned index);
	HDecal			*begin();
	HDecal			*end();

	void			readHDecalArray(std::ifstream &fin,HPatchArray *patches);

	HDecalArray			&operator = ( const HDecalArray &other );
	HDecal				operator[](unsigned index) const;
	HDecal				&operator[](unsigned index);

	friend std::ostream		&operator << (std::ostream &os,HDecalArray &other);

	mdlObject			*model;

	std::vector<HDecal> classList;
};

#endif