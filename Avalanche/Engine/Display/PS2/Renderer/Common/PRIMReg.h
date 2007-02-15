#ifndef __PRIM_REG_H
#define __PRIM_REG_H

#include "CompilerControl.h"

enum
{
	PRIM_POINT = 0,
	PRIM_LINE,
	PRIM_LINE_STRIP,
	PRIM_TRIANGLE,
	PRIM_TRIANGLE_STRIP,
	PRIM_TRIANGLE_FAN,
	PRIM_SPRITE,
	PRIM_MASK = 0x00000003
} ;

class PRIMReg
{
	static const unsigned int			totalPRIMBits;
	static const unsigned int			maskPRIMBits;
	struct
	{
	unsigned int 	_PRIM			:	3	;
	unsigned int 	_IIP			:	1	;
	unsigned int	 _TME			:	1	;
	unsigned int 	_FGE			:	1	;
	unsigned int 	_ABE			:	1	;
	unsigned int 	_AA1			:	1	;
	unsigned int 	_FST			:	1	;
	unsigned int 	_CTXT			:	1	;
	unsigned int 	_FIX			:	1	;
	} d_PRIMBits;

	void set( const PRIMReg &defREG);

public:

	PRIMReg( );
	PRIMReg( unsigned int defBits) { *((unsigned int *) &d_PRIMBits) = (defBits & maskPRIMBits); }
	
	PRIMReg( unsigned int defPRIM, unsigned int defIIP, unsigned int defTME,
				unsigned int defFGE,	unsigned int defABE, unsigned int defAA1,
				unsigned int defFST, unsigned int defCTXT, unsigned int defFIX);

	PRIMReg( const PRIMReg &defREG)					{ set( defREG); }

	PRIMReg & operator=( const PRIMReg &defREG) 	{ set( defREG); return *this; }

	operator unsigned int( void) const				{ return *((unsigned int *) &d_PRIMBits); }

	void 				setPRIM( unsigned int PRIM) 	{ d_PRIMBits._PRIM = PRIM; }
	void 				setIIP( unsigned int IIP)		{ d_PRIMBits._IIP = IIP; }
	void 				setTME( unsigned int TME)		{ d_PRIMBits._TME = TME; }
	void 				setFGE( unsigned int FGE)		{ d_PRIMBits._FGE = FGE; }
	void 				setABE( unsigned int ABE)		{ d_PRIMBits._ABE = ABE; }
	void 				setAA1( unsigned int AA1)		{ d_PRIMBits._AA1 = AA1; }
	void 				setFST( unsigned int FST)		{ d_PRIMBits._FST = FST; }
	void 				setCTXT( unsigned int CTXT)	{ d_PRIMBits._CTXT = CTXT; }
	void 				setFIX( unsigned int FIX)		{ d_PRIMBits._FIX = FIX; }

	unsigned int 	PRIM( void ) const 				{ return d_PRIMBits._PRIM; }
	unsigned int 	IIP( void ) const					{ return d_PRIMBits._IIP; }
	unsigned int	TME( void ) const					{ return d_PRIMBits._TME; }
	unsigned int	FGE( void ) const					{ return d_PRIMBits._FGE; }
	unsigned int	ABE( void ) const					{ return d_PRIMBits._ABE; }
	unsigned int	AA1( void ) const					{ return d_PRIMBits._AA1; }
	unsigned int	FST( void ) const					{ return d_PRIMBits._FST; }
	unsigned int	CTXT( void ) const	  			{ return d_PRIMBits._CTXT; }
	unsigned int	FIX( void ) const					{ return d_PRIMBits._FIX; }

} ;


// create a prim reg quickly...
#ifndef WIN32
inline PRIMReg createPRIM( unsigned char prim, bool iip, bool tme, bool fge, bool abe, bool aa1, bool fst, bool ctxt, bool fix)
{
    unsigned int    i;
    i = (unsigned int) ((fix << 10) | (ctxt << 9) | (fst << 8) | (aa1 << 7) | (abe << 6) | (fge << 5) | (tme << 4) | (iip << 3) | prim);
    return * (PRIMReg *) &i;
}
#endif


#endif
