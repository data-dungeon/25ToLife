#ifndef __VIF_CREATOR_H
#define __VIF_CREATOR_H

#include "VIFCode.h"

namespace VIFCreator
{								

#if !defined(PS2) || !defined(SN)
#include <ostream>
using namespace std;
#endif


const unsigned int 	V_NOP 		(0);
const unsigned int 	V_STCYCL 	(0x01<<24);
const unsigned int	V_OFFSET		(0x02<<24);
const unsigned int 	V_BASE		(0x03<<24);
const unsigned int 	V_ITOP		(0x04<<24);
const unsigned int 	V_MODE		(0x05<<24);
const unsigned int	V_MASK		(0x07<<24);

const unsigned int	V_FLUSHE		(0x10<<24);
const unsigned int 	V_FLUSH		(0x11<<24);
const unsigned int	V_FLUSHA		(0x13<<24);

const unsigned int 	V_MSCAL		(0x14<<24);
const unsigned int	V_MSCALF		(0x15<<25);
const unsigned int 	V_MSCNT		(0x17<<24);

const unsigned int	V_STROW		(0x30<<24);
const unsigned int	V_STCOL		(0x31<<24);
const unsigned int 	V_DIRECT		(0x50<<24);
const unsigned int 	V_UNPACK		(0x60<<24); //needs to be combined with the cmd and mask fields

typedef unsigned int VCNum;
typedef unsigned int VCFlag;
typedef unsigned int VCAddr;

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
	V4_5
} VIFUnpackCode;

inline VIFCode createMASK( VCFlag intFlag = 0)
{
	return VIFCode( V_MASK | (intFlag << 31));
}

inline VIFCode createSTROW( VCFlag intFlag = 0)
{
	return VIFCode( V_STROW | (intFlag << 31));
}

inline VIFCode createSTCOL( VCFlag intFlag = 0)
{
	return VIFCode( V_STCOL | (intFlag << 31));
}
	
inline VIFCode createSTCYCL( VCNum wl, VCNum cl, VCFlag intFlag = 0 )
{
	return VIFCode( V_STCYCL | cl | (wl << 8) | (intFlag << 31));
}

inline VIFCode createMSCAL( VCAddr vuaddr, VCFlag intFlag = 0 )
{
	return VIFCode( V_MSCAL | (vuaddr >> 3) | (intFlag << 31));
}

inline VIFCode createMSCALF( VCAddr vuaddr, VCFlag intFlag = 0 )
{
	return VIFCode( V_MSCALF | (vuaddr >> 3) | (intFlag << 31));
}

inline VIFCode createMSCNT( VCFlag intFlag = 0 )
{
	return VIFCode( V_MSCNT | (intFlag << 31));
}

inline VIFCode createNOP( VCFlag intFlag = 0 )
{
	return VIFCode( V_NOP | (intFlag << 31));
}

inline VIFCode createUNPACK( VCAddr vuaddr, VIFUnpackCode cmd, VCNum num, VCFlag useTOPS = 0, 
		VCFlag mask = 0, VCFlag isUnsigned = 0, VCFlag intFlag = 0  )
{
	return VIFCode( V_UNPACK | (cmd << 24) | (num << 16) | (useTOPS << 15) | 
				(isUnsigned << 14) |	(vuaddr) | (mask << 28) | (intFlag << 31));
}

inline VIFCode createFLUSH( VCFlag intFlag = 0 )
{
	return VIFCode( V_FLUSH | (intFlag << 31));
}

inline VIFCode createFLUSHA( VCFlag intFlag = 0 )
{
	return VIFCode( V_FLUSHA | (intFlag << 31));
}

inline VIFCode createFLUSHE( VCFlag intFlag = 0 )
{
	return VIFCode( V_FLUSHE | (intFlag << 31));
}


inline VIFCode createDIRECT( VCNum size, VCFlag flush = 0, VCFlag intFlag = 0 )
{
	return VIFCode( V_DIRECT | size | (flush << 24) | (intFlag << 31));
}

inline VIFCode createOFFSET( VCNum offset, VCFlag intFlag = 0 )
{
	return VIFCode( V_OFFSET | offset | (intFlag << 31));
}

inline VIFCode createBASE( VCNum base, VCFlag intFlag = 0 )
{
	return VIFCode( V_BASE | base | (intFlag << 31));
}

inline VIFCode createITOP( VCAddr vuaddr, VCFlag intFlag = 0 )
{
	return VIFCode( V_ITOP | vuaddr | (intFlag << 31));
}

inline VIFCode createMODE( VCNum mode, VCFlag intFlag = 0 )
{
	return VIFCode( V_MODE | mode | (intFlag << 31));
}
} 

#endif