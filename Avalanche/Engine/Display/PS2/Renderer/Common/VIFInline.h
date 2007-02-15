#ifndef __VIF_INLINE_H
#define __VIF_INLINE_H

#include "renderassert.h"

namespace VIFInline
{

const unsigned int 	V_NOP 		(0);

const unsigned int 	V_STCYCL 	(0x01<<24);
const unsigned int	V_OFFSET		(0x02<<24);
const unsigned int 	V_BASE		(0x03<<24);
const unsigned int 	V_ITOP		(0x04<<24);

const unsigned int 	V_MODE		(0x05<<24);
const unsigned int	V_MSKPATH3	(0x06<<24);
const unsigned int	V_MARK		(0x07<<24);

const unsigned int	V_FLUSHE		(0x10<<24);
const unsigned int 	V_FLUSH		(0x11<<24);

const unsigned int	V_FLUSHA		(0x13<<24);

const unsigned int 	V_MSCAL		(0x14<<24);
const unsigned int	V_MSCALF		(0x15<<24);

const unsigned int 	V_MSCNT		(0x17<<24);

const unsigned int	V_STMASK		(0x20<<24);
const unsigned int	V_STROW		(0x30<<24);
const unsigned int	V_STCOL		(0x31<<24);

const unsigned int	V_MPG			(0x4A<<24);

const unsigned int 	V_DIRECT		(0x50<<24);

const unsigned int 	V_UNPACK		(0x60<<24); 

typedef unsigned int 	VCNum;
typedef unsigned int 	VCFlag;
typedef unsigned int 	VCAddr;

typedef enum _VIFUnpackCode
{
	S_32 = 0,
	S_16,										 
	S_8,
	VIF_UNPACK_RESERVED_1,
	V2_32,
	V2_16,
	V2_8,
	VIF_UNPACK_RESERVED_2,
	V3_32,
	V3_16,
	V3_8,
	VIF_UNPACK_RESERVED_3,
	V4_32,
	V4_16,
	V4_8,
	V4_5,

	MASKED_S_32,
	MASKED_S_16,										 
	MASKED_S_8,
	MASKED_VIF_UNPACK_RESERVED_1,
	MASKED_V2_32,
	MASKED_V2_16,
	MASKED_V2_8,
	MASKED_VIF_UNPACK_RESERVED_2,
	MASKED_V3_32,
	MASKED_V3_16,
	MASKED_V3_8,
	MASKED_VIF_UNPACK_RESERVED_3,
	MASKED_V4_32,
	MASKED_V4_16,
	MASKED_V4_8,
	MASKED_V4_5,
} VIFUnpackCode;

inline unsigned int createMPG( VCNum numInst, VCAddr vuAddr, VCFlag intFlag = 0 )
{
	return ( V_MPG | vuAddr | (numInst << 16) | (intFlag << 31) );
}

inline unsigned int createMSKPATH3( VCFlag maskedFlag, VCFlag intFlag = 0)
{
	return ( V_MSKPATH3 | (maskedFlag << 15) | (intFlag << 31) );
}

inline unsigned int createMARK( unsigned short mark, VCFlag intFlag = 0)
{
	return ( V_MARK | (intFlag << 31) | mark);
}

inline unsigned int createSTROW( VCFlag intFlag = 0)
{
	return ( V_STROW | (intFlag << 31) );
}

inline unsigned int createSTMASK( VCFlag intFlag = 0)
{
	return ( V_STMASK | (intFlag << 31) );
}

inline unsigned int createSTCOL( VCFlag intFlag = 0)
{
	return ( V_STCOL | (intFlag << 31) );
}
	
inline unsigned int createSTCYCL( VCNum wl, VCNum cl, VCFlag intFlag = 0 )
{
	return ( V_STCYCL | cl | (wl << 8) | (intFlag << 31) );
}

inline unsigned int createMSCAL( VCAddr vuaddr, VCFlag intFlag = 0 )
{
	return ( V_MSCAL | (vuaddr >> 3) | (intFlag << 31) );
}

inline unsigned int createMSCALF( VCAddr vuaddr, VCFlag intFlag = 0 )
{
	return ( V_MSCALF | (vuaddr >> 3) | (intFlag << 31) );
}

inline unsigned int createMSCNT( VCFlag intFlag = 0 )
{
	return ( V_MSCNT | (intFlag << 31) );
}

inline unsigned int createNOP( VCFlag intFlag = 0 )
{
	return ( V_NOP | (intFlag << 31) );
}

inline unsigned int createUNPACK( VCAddr vuaddr, VIFUnpackCode cmd, VCNum num, VCFlag useTOPS = 0, 
		VCFlag mask = 0, VCFlag isUnsigned = 0, VCFlag intFlag = 0  )
{
	return ( V_UNPACK | (cmd << 24) | (num << 16) | (useTOPS << 15) | 
				(isUnsigned << 14) |	(vuaddr) | (mask << 28) | (intFlag << 31) );
}

inline unsigned int createFLUSH( VCFlag intFlag = 0 )
{
	return ( V_FLUSH | (intFlag << 31) );
}

inline unsigned int createFLUSHA( VCFlag intFlag = 0 )
{
	return ( V_FLUSHA | (intFlag << 31) );
}

inline unsigned int createFLUSHE( VCFlag intFlag = 0 )
{
	return ( V_FLUSHE | (intFlag << 31) );
}

inline unsigned int createDIRECT( VCNum size, VCFlag flush = 0, VCFlag intFlag = 0 )
{
	return ( V_DIRECT | size | (flush << 24) | (intFlag << 31) );
}

inline unsigned int createOFFSET( VCNum offset, VCFlag intFlag = 0 )
{
	return ( V_OFFSET | offset | (intFlag << 31) );
}

inline unsigned int createBASE( VCNum base, VCFlag intFlag = 0 )
{
	return ( V_BASE | base | (intFlag << 31) );
}

inline unsigned int createITOP( VCAddr vuaddr, VCFlag intFlag = 0 )
{
	return ( V_ITOP | vuaddr | (intFlag << 31) );
}

inline unsigned int createMODE( VCNum mode, VCFlag intFlag = 0 )
{
	return ( V_MODE | mode | (intFlag << 31) );
}

#define vifiIsWordAlignedM(ptr) 		((( (unsigned int) (ptr) ) & 0x03) == 0)
#define vifiIsQWordAlignedM(ptr) 		((( (unsigned int) (ptr) ) & 0x0f) == 0)

inline unsigned int * vifiQWordAlignNOP( unsigned int *pVIFPtr)
{
	RENDER_ASSERT( vifiIsWordAlignedM( pVIFPtr) );

	while( ((unsigned long) pVIFPtr) & 0xf)
		*pVIFPtr++ = createNOP( 0);

	return pVIFPtr;
}

}

// QWord = 128 bits = 16 bytes = 4 words
// DMATag = 64bit	= 8 bytes = 2 words
// VIFCode = 32 bits = 4 bytes = 1 word
#endif

																							  
