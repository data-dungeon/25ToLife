#ifndef __HSTAMP_H
#define __HSTAMP_H

#ifndef __UBERSTRING_H
#include "uberstring.h"
#endif
#ifndef __HVECTOR_H
#include "hvector.h"
#endif
#ifndef __HSTATUS_H
#include "hstatus.h"
#endif
#ifndef __HPATCHVECTORS_H
#include "hpatchvectors.h"
#endif
#ifndef __HINTMAP_H
#include "hintmap.h"
#endif

class HDecal;
class HDecalArray;

class HPatch;
class HPatchArray;

class HStamp
{
public:
	HStamp ();
	~HStamp ();

	UString name;
	HPatchVectorsArray uvs;

	HIntMap patchIndicies;
	
	void				clear();
	void				readHStamp(std::ifstream &fin,HPatchArray *patches);
	void				readHStampData(std::ifstream &fin,HPatchArray *patches);
	HStamp				&operator = ( const HStamp &other );
	friend std::ostream		&operator << (std::ostream &os,HStamp &other);

};

class HStampArray
{
public:

	HStampArray();
	~HStampArray();

	void				clear(void);
	unsigned			length(void) const;
	unsigned			append(HStamp stamp);

	HStamp				*get(unsigned index);
	HStamp				*begin();
	HStamp				*end();

	void				readHStampArray(std::ifstream &fin,HPatchArray *patches,HDecal *decal);

	HStampArray			&operator = ( const HStampArray &other );
	HStamp				operator[](unsigned index) const;
	HStamp				&operator[](unsigned index);
	friend std::ostream		&operator << (std::ostream &os,HStampArray &other);

	std::vector<HStamp> classList;
};

#endif