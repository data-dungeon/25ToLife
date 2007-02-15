#include "DMATag.h"


DMATag::DMATag( )
{
	d_DMATag._QWC = 0;
	d_DMATag._PCE = 0;
	d_DMATag._ID = 0;
	d_DMATag._IRQ = 0;
	d_DMATag._ADDR = 0;
}

DMATag::DMATag( unsigned int defQWC, unsigned int defPCE, unsigned int defID,
				unsigned int defIRQ, unsigned int defADDR)
{
	d_DMATag._QWC = defQWC;
	d_DMATag._PCE = defPCE;
	d_DMATag._ID = defID;
	d_DMATag._IRQ = defIRQ;
	d_DMATag._ADDR = calcDMATagAddr( defADDR);
}

void DMATag::set( const DMATag &defTag)
{
	d_DMATag = defTag.d_DMATag;
}

void DMATag::setADDR( unsigned int ADDR)
{
	d_DMATag._ADDR = calcDMATagAddr( ADDR);
}

unsigned int DMATag::ADDR( void ) const
{
	return calcMemAddr( d_DMATag._ADDR);
}

#if !defined(PS2) || !defined(SN)
void DMATag::write( ostream &str) const
{
	str.write( (char *) &d_DMATag, 8);
}
#endif
