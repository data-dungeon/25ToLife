#ifndef __GIFTAG_H
#define __GIFTAG_H

#include "PRIMReg.h"
#ifndef WIN32
#include <eetypes.h>
#endif

#ifdef DEFINE_GIF_WRITE
#include <ostream>
using namespace std;
#endif


#include "CompilerControl.h"



class GIFTag
{
public:
	enum FLGID
	{
		FLG_PACKED = 0,
		FLG_REGLIST,
		FLG_IMAGE,
		FLG_DISABLE
	} ;

	enum RegisterID
	{
		REG_PRIM = 0x00,
		REG_RGBAQ,
		REG_ST,
		REG_UV,
		REG_XYZF2,
		REG_XYZ2,
		REG_TEX0_1,
		REG_TEX0_2,
		REG_CLAMP_1,
		REG_CLAMP_2,
		REG_FOG,
		REG_RESERVED_1,
		REG_XYZF3,
		REG_XYZ3,
		REG_A_D,
		REG_NOP
	} ;

protected:
	struct _GIFBits
	{
		unsigned int 	_NLOOP 		: 	15 ;
		unsigned int	_EOP			: 	1	;
		unsigned int	_Pad1			:	16	;
		unsigned int	_Pad2			:  14 ;
		unsigned int	_PRE			:	1	;
		unsigned int	_PRIM			:	11	;
		unsigned int	_FLG			: 	2	;
		unsigned int	_NREG			: 	4	;
#ifdef WIN32
	} d_GIFBits;
#else
	} d_GIFBits __attribute__((aligned(16)));
#endif

	unsigned int		d_GIFRegs[2];

	inline void			set( const GIFTag &defTag);
public:
	inline GIFTag( );
	inline GIFTag( unsigned int defNLOOP, unsigned int defEOP, unsigned int defPRE,
				unsigned int defPRIM, unsigned int defFLG, unsigned int defNREGS, unsigned int *pRegs);

	inline GIFTag( unsigned int defNLOOP, unsigned int defEOP, unsigned int defPRE,
				unsigned int defPRIM, unsigned int defFLG, unsigned int defNREGS, unsigned int Regs1, unsigned int Regs2 = 0);

	inline GIFTag( const GIFTag &defTag);

	inline GIFTag & operator=( const GIFTag &defTag);

	void 					setNLOOP( unsigned int NLOOP) { d_GIFBits._NLOOP = NLOOP; }
	void					setEOP( unsigned int EOP)		{ d_GIFBits._EOP = EOP; }
	void					setPRE( unsigned int PRE)		{ d_GIFBits._PRE = PRE; }
	void					setPRIM( unsigned int PRIM)	{ d_GIFBits._PRIM = PRIM; setPRE(1); }
	void					setPRIM( const PRIMReg &PRIM) { setPRIM((unsigned int)PRIM); }
	void					setFLG( unsigned int FLG)		{ d_GIFBits._FLG = FLG; }
	void					setNREGS( unsigned int NREG)	{ d_GIFBits._NREG = NREG; }

	inline void			setRegs( unsigned int numRegs, unsigned int *pRegs);
	void					setRegs( unsigned int numRegs, unsigned int Regs1, unsigned int Regs2 = 0) 
		{ d_GIFBits._NREG = numRegs; d_GIFRegs[0] = Regs1; d_GIFRegs[1] = Regs2; }

	inline void			setReg( unsigned int RegNo, RegisterID RegVal);

	inline RegisterID	getReg( unsigned int RegNo) const;

	unsigned int 		NLOOP( void ) const 				{ return d_GIFBits._NLOOP; }
	unsigned int		EOP( void ) const					{ return d_GIFBits._EOP; }
	unsigned int		PRE( void ) const					{ return d_GIFBits._PRE; }
	unsigned int 		PRIM( void ) const				{ return d_GIFBits._PRIM; }
	unsigned int		FLG( void ) const					{ return d_GIFBits._FLG; }
	unsigned int		NREG( void ) const				{ return d_GIFBits._NREG; }

} ;

#ifdef DEFINE_GIF_WRITE
class GIFTagW : public GIFTag
{

public:
	GIFTagW( ) : GIFTag( ) { }
	GIFTagW( const GIFTag &defTag) : GIFTag( defTag) { }

	GIFTagW( unsigned int defNLOOP, unsigned int defEOP, unsigned int defPRE, unsigned int defPRIM, 
					unsigned int defFLG, unsigned int defNREGS, unsigned int *pRegs) :
		GIFTag( defNLOOP, defEOP, defPRE, defPRIM, defFLG, defNREGS, pRegs) { }

	GIFTagW( unsigned int defNLOOP, unsigned int defEOP, unsigned int defPRE, unsigned int defPRIM, 
					unsigned int defFLG, unsigned int defNREGS, unsigned int Regs1, unsigned int Regs2 = 0) :
		GIFTag( defNLOOP, defEOP, defPRE, defPRIM, defFLG, defNREGS, Regs1, Regs2) { }

	virtual			void write( ostream &str) const;
} ;

inline ostream & operator<<( ostream &str, const GIFTagW &tag)
{

	tag.write( str);

	return str;

}
#endif

#include "GIFTag.hpp"


#endif
