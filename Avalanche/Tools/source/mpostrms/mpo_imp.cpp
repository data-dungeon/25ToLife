
#include <mpo_imp.h>

mpostream & PSXStream::operator << (float f)
{
	unsigned long		ul;
	
	ul = (unsigned long) (f * fPrec);

	WriteDWORD( &ul);

	return *this;
}
