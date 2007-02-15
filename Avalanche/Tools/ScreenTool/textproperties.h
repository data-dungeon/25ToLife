#ifndef _TEXTPROPERTIES_H_
#define _TEXTPROPERTIES_H_

#include "propertyhandler.h"

class TextClassPropertyHandler : public PropertyHandler
{
public:
	virtual const char* GetKnownClass( void );
};

#endif // _TEXTPROPERTIES_H_