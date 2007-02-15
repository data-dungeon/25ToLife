/*************************************/
/*                                   */
/*   d_GSState.h                     */
/*   new engine  04/6/01             */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   gs chip state management        */
/*                                   */
/*************************************/

#ifdef PS2	//ps2 only

#ifndef __D_GSSTATE_H
#define __D_GSSTATE_H

/******************* includes ****************************/


/* system includes */

/* engine includes */

#include "platform/BaseType.h"
#include "platform/CompilerControl.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* forward declarations ****************/

/******************* defines *****************************/

/* size of gs state packets */
// for the new renderer, this is arbitrary... set it to something that looks good &
// doesnt use too much memory.  There is a maximum but its in the thousands...:)
#define MAX_GSSTATES_IN_ONE_PACKET	32
	
#define GS_STATE_PACKET_QWC	(MAX_GSSTATES_IN_ONE_PACKET+2)			// a dma tag, a gif tag, and up to 16 gs register addr/value pairs

/******************* macros ******************************/

/******************* structures **************************/

/* a place to track current gs register values */

struct ts_GSState
{
	u64	TEST_1;				// SCE_GS_TEST_1
	u64	FRAME_1;				// SCE_GS_FRAME_1
	u64	ZBUF_1;				// SCE_GS_ZBUF_1
	u64	ALPHA_1;				// SCE_GS_ALPHA_1
	u64	TEXA;					// SCE_GS_TEXA
	u64	XYOFFSET_1;			// SCE_GS_XYOFFSET_1
	u64	SCISSOR_1;			// SCE_GS_SCISSOR_1
	u64	PRMODECONT;			// SCE_GS_PRMODECONT
	u64	COLCLAMP;			// SCE_GS_COLCLAMP
	u64	DIMX;					// SCE_GS_DIMX
	u64	POSDIMX;				// SCE_GS_DIMX (alternate, all positive version)
	u64	DTHE;					// SCE_GS_DTHE
	u32	u32ZBufferClear;	// converted z-buffer clear value (converted from api's 0.0 to 1.0 range), used in SCE_GS_XYZ2
	u16	u16LastZFUNC;		// last value passed to RenderState__Request_RS_ZFUNC, see notes in RenderState__Request_RS_ZTEST
	u16	u16LastZWRITE;		// last value passed to RenderState__Request_RS_ZWRITE, see notes in RenderState__Request_RS_ZTEST
};

/******************* global variables ********************/

extern ts_GSState GSState;

/******************* global prototypes *******************/

void GSState__Init(void);
void GSState__Set(u64 u64Address, u64 u64Data);
void GSState__Send(void) NAMED_SECTION("text.rend1");
void GSState__Clear(void);

void GSState__DebugSpew( void );
void GSState__Debug_SetTrackedRegs( void );
void GSState__Debug_ValidateTrackedRegs( void);

struct ts_ADPair
{
	u64	u64Data;			// register value
	u64	u64Address;		// register address (e.g. SCE_GS_TEX0_1)
};

/* add a register request to gs state. if already full, send previous buffer before adding new state */

extern u32 u32GSStateCount;
extern ts_ADPair *pADPairs;

// TJC - the expanded versions of these allow us to reduce
// the number of times we check for the state-buffer-full
// condition.  I had to do this to get one functions size down
// so that it would fit in the instruction cache with tons of
// other stuff..

inline void GSState__Set(u64 u64Address, u64 u64Data);

inline void GSState__Set2(
								  u64 u64Address0, u64 u64Data0,
								  u64 u64Address1, u64 u64Data1);

inline void GSState__Set3(
								  u64 u64Address0, u64 u64Data0,
								  u64 u64Address1, u64 u64Data1,
								  u64 u64Address2, u64 u64Data2);

inline void GSState__Set4(
								  u64 u64Address0, u64 u64Data0,
								  u64 u64Address1, u64 u64Data1,
								  u64 u64Address2, u64 u64Data2,
								  u64 u64Address3, u64 u64Data3);

#include "Display/PS2/GSState.hpp"

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __D_GSSTATE_H

#endif //PS2
