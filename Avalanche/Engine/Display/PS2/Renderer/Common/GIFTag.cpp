#include "GIFTag.h"


#ifdef DEFINE_GIF_WRITE
void GIFTagW::write( ostream &str) const
{
	str.write( (char *) &d_GIFBits, 8);
	str.write( (char *) &d_GIFRegs[0], 4);
	str.write( (char *) &d_GIFRegs[1], 4);
}
#endif
