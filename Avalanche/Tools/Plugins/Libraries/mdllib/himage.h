#ifndef __HIMAGE_H
#define __HIMAGE_H

#ifndef __UBERSTRING_H
#include "uberstring.h"
#endif
#ifndef __HCOLOR_H
#include "hcolor.h"
#endif

class mdlObject;

class HImage
{
public:
	HImage ();
	HImage(const HImage &other);
	~HImage ();

	void				readHImage(std::ifstream &fin);

	HImage &operator = ( const HImage &other );

	UString				name;//shortcut refers to this name
	UString				pathname;
	HColor				backColor;

	mdlObject			*model;
};

class HImageArray
{
public:

	HImageArray ();
	~HImageArray ();

	void			clear();
	unsigned		length() const;
	void			append(HImage &texmap);
	bool			findImageIndexWithName(UString &imagename,unsigned &index);

	HImage			*get(unsigned index);

	HImageArray &operator = ( const HImageArray &other );

	HImage			operator[]( unsigned index ) const;
 	HImage&	 		operator[]( unsigned index );

	mdlObject		*model;

	std::vector<HImage> classList;
};

class HImageIterator
{
public:
	HImageIterator();
	HImageIterator(HImageArray &iarray);
	~HImageIterator();

	void			reset(void);
	void			reset(HImageArray &iarray);

	bool			isDone(void);

	void			insert(HImage image);
	unsigned		index();

	HImageIterator	operator  ++(int);
	HImageIterator	operator  --(int);
	HImage			*operator * ();

private:
	HImageArray *pointTo;
	std::vector<HImage>::iterator cl_iterator;
};

#endif