#ifndef __HARG_H
#define __HARG_H

#ifndef __UBERSTRING_H
#include "uberstring.h"
#endif

class HArg
{
public:
	HArg();
	HArg(UString &argstr);

	~HArg();

	void				set(UString argstr);

	HArg				&operator = ( const HArg &other );

	double fvalue;
	long ivalue;
	UString arg_string;
	bool bvalue;
};

class HArgArray
{
public:

	HArgArray ();

	~HArgArray ();

	void				clear(void);
	unsigned			length(void) const;
	void				append(HArg &arg);

	long				getInt(unsigned index);
	double				getFloat(unsigned index);
	bool				getBool(unsigned index);
	UString				getString(unsigned index);
	UString				getWholeString();

	const HArg			&operator[]( unsigned index );
	HArgArray			&operator = ( const HArgArray &other );

private:
	std::vector<HArg> classList;
};

#endif
