#ifndef __DMATAG_H
#define __DMATAG_H

#if !defined(PS2) || !defined(SN)
#include <ostream>
#endif

using namespace std;

enum
{
	ID_REFE = 0,
	ID_CNT,
	ID_NEXT,
	ID_REF,
	ID_REFS,
	ID_CALL,
	ID_RET,
	ID_END
} ;

enum
{
	PCE_NOTHING = 0,
	PCE_RESERVED,
	PCE_DISABLED,
	PCE_ENABLED
} ;
	
class DMATag
{
											
	struct
	{
	unsigned int	_QWC			: 	16	;
	unsigned int	_Pad1			:	10	;
	unsigned int	_PCE			:	2 	;
	unsigned int	_ID			:	3	;
	unsigned int	_IRQ			:	1	;
	unsigned int	_ADDR			:	32	;
	} d_DMATag;

	void 					set( const DMATag &defTag);

public:
	DMATag( );
	DMATag( unsigned int defQWC, unsigned int defPCE, unsigned int defID,
				unsigned int defIRQ, unsigned int defADDR);

	DMATag( const DMATag &defTag)					{ set( defTag); }
	DMATag & operator=( const DMATag &defTag)	{ set( defTag); return *this; }

	inline void 		setQWC( unsigned int QWC)		{ d_DMATag._QWC = (unsigned short)QWC; }
	inline void			setPCE( unsigned int PCE)		{ d_DMATag._PCE = PCE; }
	inline void			setID( unsigned int ID)			{ d_DMATag._ID = ID; }
	inline void		  	setIRQ( unsigned int IRQ)		{ d_DMATag._IRQ = IRQ; }
	void		  			setADDR( unsigned int ADDR);

	unsigned int		QWC( void ) const					{ return d_DMATag._QWC; }
	unsigned int		PCE( void ) const					{ return d_DMATag._PCE; }
	unsigned int		ID( void ) const					{ return d_DMATag._ID; }
	unsigned int		IRQ( void ) const					{ return d_DMATag._IRQ; }
	unsigned int		ADDR( void ) const;
	
#if !defined(PS2) || !defined(SN)
	void 					write( ostream &str) const;
#endif

} ;


inline unsigned int calcMemAddr( unsigned int adr)
{
	return ((adr & 0x80000000) ? 
				((adr & 0x0fffffff) | 0x70000000) : adr);
}

inline unsigned int calcDMATagAddr( unsigned int adr)
{
	return (((adr & 0x70000000) == 0x70000000) ? 
					((adr & 0x0fffffff) | 0x80000000) : adr);
}

#if !defined(PS2) || !defined(SN)
inline ostream & operator<<( ostream &str, const DMATag &tag)
{
	tag.write( str);

	return str;
}
#endif

#endif

