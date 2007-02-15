#ifndef __VU_MANAGED_FUNCTION_H
#define __VU_MANAGED_FUNCTION_H

#include "VUManagedCode.h"


class VUManagedFunction
{

	unsigned int				d_Offset;
	VUManagedCode &			d_Code;
	
public:
	VUManagedFunction(
		VUManagedCode &		_Code,
		unsigned int			_CallOffset
		) :
		d_Offset( _CallOffset),
		d_Code( _Code)
	{
	
	}
	inline unsigned int 		offset( void ) const 	{ return d_Offset; }
	inline VUManagedCode &	code( void ) const 		{ return d_Code; }
} ;


#endif
