#ifndef __DMA_INLINE_H
#define __DMA_INLINE_H

#include "VIFInline.h"

namespace DMAInline
{

typedef enum _DIPriority
{
	DTAG_PCE_NOTHING = 0,
	DTAG_PCE_RESERVED,
	DTAG_PCE_DISABLED,
	DTAG_PCE_ENABLED
} DIPriority;

typedef enum _DITagID
{
	DTAG_ID_REFE = 0,
	DTAG_ID_CNT,
	DTAG_ID_NEXT,
	DTAG_ID_REF,
	DTAG_ID_REFS,
	DTAG_ID_CALL,
	DTAG_ID_RET,
	DTAG_ID_END
} DITagID;

typedef unsigned int		DIAddr;

// macros

#define dmaiIsWordAlignedM(ptr) 			((( (unsigned int) (ptr) ) & 0x03) == 0)
#define dmaiIsQWordAlignedM(ptr) 		((( (unsigned int) (ptr) ) & 0x0f) == 0)

// inlines

inline unsigned int dmaiCalculateQWC( void *pEnd, void *pStart)
{
	RENDER_ASSERT( dmaiIsQWordAlignedM( pEnd));
	RENDER_ASSERT( dmaiIsQWordAlignedM( pStart));

	return ((unsigned int) pEnd - (unsigned int) pStart) >> 4;
}

inline unsigned int cvtDMATagAddr( unsigned int adr)
{
	return (((adr & 0x70000000) == 0x70000000) ? 
					((adr & 0x0fffffff) | 0x80000000) : adr);
}

inline unsigned long createDMATag( unsigned int QWC, DITagID TagID, DIPriority Priority, DIAddr Addr, unsigned intFlag = 0)
{
	
	return (((unsigned long) cvtDMATagAddr( Addr)) << 32) | (intFlag << 31) | (TagID << 28) | (Priority << 26) | QWC;
}

inline unsigned int *dmaiNextQWordAlignedAddr( void *pAddr)
{
	unsigned int 				diff = ((unsigned int) pAddr) & 0x0f;

	if( diff)
		return (unsigned int *) (((unsigned int) pAddr) + 0x10 - diff);
	else
		return (unsigned int *) pAddr;
}

inline unsigned int *dmaiNextWordAlignedAddr( void *pAddr)
{
	unsigned int 				diff = ((unsigned int) pAddr) & 0x03;

	if( diff)
		return (unsigned int *) (((unsigned int) pAddr) + 0x04 - diff);
	else
		return (unsigned int *) pAddr;
}



// macros
#define dmaiCalculateQWCM(end,start)	( ( (unsigned int) (end) - (unsigned int) (start)) >> 4)

#define dmaiPacketStartAlignM( )\
\
	while( pVIFPtr < pPacketStart) \
		*pVIFPtr++ = VIFInline::createNOP( 0);

#define dmaiPacketEndAlignM( ) \
\
	while( (pVIFPtr  - pPacketStart) % 4) \
		*pVIFPtr++ = VIFInline::createNOP( 0);


#define dmaiPacketStartPostTagM( dmaMem, tteFlag)  \
\
	{ \
	bool					bTTEFlag; \
	u_int *				pVIFPtr; \
	u_int *				pPacketStart; \
	u_long *				pDMATag = (u_long *) (dmaMem); \
\
	bTTEFlag = tteFlag; \
	pVIFPtr = (u_int *) ((tte) ? (pDMATag + 1) : (pDMATag + 2)); \
\
  	pPacketStart = (u_int *) (pDMATag + 2); 



#define dmaiPacketEndPostTagM( dmaTagID, dmaAddr) \
\
	while( pVIFPtr < pPacketStart) \
		*pVIFPtr++ = VIFInline::createNOP( 0); \
\
	while( (pVIFPtr - pPacketStart) % 4) \
		*pVIFPtr++ = VIFInline::createNOP( 0); \
\
	unsigned int		dmaQWC = (pVIFPtr - pPacketStart) / 4;	\
\
	*pDMATag = createDMATag( dmaQWC, dmaTagID, DTAG_PCE_NOTHING, dmaAddr); \
\
	} 

#define dmaiPacketStartPreTagM( dmaMem, tteFlag, dmaTagID, dmaAddr, dmaQWC)  \
\
	{ \
	bool					bTTEFlag; \
	u_int *				pVIFPtr; \
	u_int *				pPacketStart; \
	u_long *				pDMATag = (u_long *) (dmaMem); \
\
	bTTEFlag = tte; \
\
	*pDMATag = createDMATag( dmaQWC, dmaTagID, DTAG_PCE_NOTHING, dmaAddr); \
\
	pVIFPtr = (u_int *) ((tte) ? (pDMATag + 1) : (pDMATag + 2)); \
  	pPacketStart = (u_int *) (pDMATag + 2); 



#define dmaiPacketEndPreTagM( ) \
\
	while( pVIFPtr < pPacketStart) \
		*pVIFPtr++ = VIFInline::createNOP( 0); \
\
	while( (pVIFPtr - pPacketStart) % 4) \
		*pVIFPtr++ = VIFInline::createNOP( 0); \
\
	} 

}

#endif