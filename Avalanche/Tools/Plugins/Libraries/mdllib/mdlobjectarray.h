//////////////////////////////////////////////////////////////////////////////////////////
//
// MDLOBJECT ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MDLOBJECTARRAY_H
#define __MDLOBJECTARRAY_H

#ifndef __MDLOBJECT_H
#include "mdlobject.h"
#endif

class mdlObject;

class mdlObjectArray
{
public:
	mdlObjectArray();
	mdlObjectArray(const mdlObjectArray &other);
	~mdlObjectArray();

	void				clear(void);
	unsigned			length(void) const;
	void				resize(const unsigned s);
	unsigned			append(mdlObject &other);

	mdlObject			*get(unsigned index);
	mdlObject			*begin();
	mdlObject			*end();

	mdlObject			operator[]( unsigned index ) const;
 	mdlObject			&operator[]( unsigned index );
	mdlObjectArray		&operator = ( const mdlObjectArray &other );
	friend std::ostream		&operator << (std::ostream &os,mdlObjectArray &other);

	std::vector<mdlObject> classList;
};

#endif