/*************************************/
/*                                   */
/*   l_Struct.h                      */
/*   new engine                      */
/*   avalanche software              */
/*   originally by joe barnes        */
/*   visual c++ 6.0                  */
/*   layers structures               */
/*                                   */
/*************************************/

#ifndef __L_STRUCT_H
#define __L_STRUCT_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* defines *****************************/

/* we reserve the handle of 0xffffffff as an invalid handle. to guarantee that we can't reach it, we limit the record pool size
to 65535-- 0xfffe. see explanation of handle structure below */

#define INVALID_HANDLE			0				// ned says: used to be 0xffffffff, but it was decided that 0 (NULL) is more appropriate
#define MAX_HANDLES				65535			// max size of a manager's allocated record pool, to insure INVALID_HANDLE is indeed invalid

/* full alpha byte value-- on ps2 it's 0x80 */

#ifdef WIN32
#define BYTE_FULL_ALPHA			0xff
#elif defined(PS2)
#define BYTE_FULL_ALPHA			0x80
#elif defined(GCN)
#define BYTE_FULL_ALPHA			0xff
#endif //WIN32/PS2

/******************* macros ******************************/

#define RGBF_TO_B(fValue)		((u8) (fValue * 255.0f))						// convert from 0.0..1.0 to 0..255
#define RGBB_TO_F(u8Value)		((float) ((float) u8Value) / 255.0f)		// convert from 0..255 to 0.0..1.0

/* for extracting ts_Handle info out of a t_Handle value */

#define HANDLE_INDEX(Handle)	(((ts_Handle *) &Handle)->u16Index)
#define HANDLE_ID(Handle)		(((ts_Handle *) &Handle)->u16ID)

/******************* structures **************************/

struct ts_bRGB
{
	u8				R;
	u8				G;
	u8				B;
};

struct ts_bRGBA
{
	u8				R;
	u8				G;
	u8				B;
	u8				A;

	// TJC - a bit of magic for the PS2...
	// this divides each RGBA by 2 and returns
	// the result.  Eventually, this should be
	// replaced with a gamma curve... 
	// incase your wondering about the union and all
	// the variables, the optimized version should
	// remove those and clean up the code quite a bit..
	// the results should be quite fast..
	// At least GCC would remove them... I havent
	// tested, and dont know about CW.
#ifdef PS2
	inline ts_bRGBA PS2CorrectColor() const
	{
		register u64 temp1, temp2, temp3, dest;
		union { ts_bRGBA rgba; u64 bits; } copytemp;
		copytemp.rgba = *this;
		temp1 = copytemp.bits;
		asm("pextlb %0, $0, %1" : "=r" (temp2) : "r" (temp1));
		asm("psrlh %0, %1, 1" : "=r" (temp3) : "r" (temp2));
		asm("ppacb %0, $0, %1" : "=r" (dest) : "r" (temp3)); // t0 is used as I dont care what goes in the upper 64 bits...
		copytemp.bits = dest;
		return copytemp.rgba;
	}
#endif //PS2
};

struct ts_ABGR
{
	union
	{
		u32		ABGR;
		ts_bRGBA	c;
	};
};

struct ts_fRGB
{
	float			R;
	float			G;
	float			B;
};

struct ts_fRGBA
{
	float			R;
	float			G;
	float			B;
	float			A;
};

struct ts_Hsv {
	float	H;
	float	S;
	float	V;
};

/* handle structure. a handle is a returned value that can be used later to reference a particular data object. for example,
when a database is loaded, dblLoad returns a handle to that database that can be used to identify it later. to the user, a
handle's value is theoretically gibberish-- it just has to be usable by the engine internals to get back to the requested
record. our implementation is to use a 32-bit value, broken up into 2 16-bit values. the first is an index that can be used
to access the requested record-- more on this later. the second is an id number.

we have chosen to use an index instead of a pointer to save some bits. code-wise, i think it's better to pass around 32-bit
instead of 64-bit handles. the way all of our record allocation works is to pre-allocate a buffer of all the records you will
need for a given manager, and linking them into a single free list. when a new record is needed, it is pulled from the free
list. because the we use linked lists, records stay in the same place with respect to the originally allocated memory buffer,
even as they move from the free list to the active list and vice versa

but an index is not enough, because there's no way of telling when it becomes invalid. for example, say you register a sprite
dies after 5 seconds. you get a handle back. the sprite manager kills the sprite after 5 seconds, but there's no way for the
game side to know that the handle is invalid, so it may blithely keep updating the sprite record, even if it gets 
reallocated for another active sprite. by using an id as the other half of a handle, we have enough information to tell when
a handle is no longer valid

a 16-bit index means, of course, that a record pool can not be any larger than 65536 records long. if this becomes a problem,
we can always go to 64-bit handles.

we reserve the handle of 0xffffffff as an invalid handle. to guarantee that we can't reach it, we limit the record pool size
to 65535-- 0xfffe */

typedef u32	t_Handle;
struct ts_Handle
{
	u16	u16Index;
	u16	u16ID;
};

/* an error type */

typedef u32	t_Error;

/******************* global variables ********************/

/******************* global prototypes *******************/



#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __L_STRUCT_H
