
#include "PRIMReg.h"

const unsigned int			PRIMReg::totalPRIMBits = 11;
const unsigned int			PRIMReg::maskPRIMBits = (1 << (totalPRIMBits + 1)) - 1;

PRIMReg::PRIMReg( unsigned int defPRIM, unsigned int defIIP, unsigned int defTME, 
				unsigned int defFGE,	unsigned int defABE, unsigned int defAA1,
				unsigned int defFST, unsigned int defCTXT, unsigned int defFIX)

{
	d_PRIMBits._PRIM = defPRIM;
	d_PRIMBits._IIP = defIIP;
	d_PRIMBits._TME = defTME;
	d_PRIMBits._FGE = defFGE;
	d_PRIMBits._ABE = defABE;
	d_PRIMBits._AA1 = defAA1;
	d_PRIMBits._FST = defFST;
	d_PRIMBits._CTXT = defCTXT;
	d_PRIMBits._FIX = defFIX;
} 
	
PRIMReg::PRIMReg( )
{

	d_PRIMBits._PRIM = 0;
	d_PRIMBits._IIP = 0;
	d_PRIMBits._TME = 0;
	d_PRIMBits._FGE = 0;
	d_PRIMBits._ABE = 0;
	d_PRIMBits._AA1 = 0;
	d_PRIMBits._FST = 0;
	d_PRIMBits._CTXT = 0;
	d_PRIMBits._FIX = 0;

}

void PRIMReg::set( const PRIMReg &defPRIM)
{

	d_PRIMBits = defPRIM.d_PRIMBits;
	
}



