/***************************************************************************/
// Unsigned 24-bit integer
/***************************************************************************/

#include "platform/BaseType.h"

#ifndef U24_H
#define U24_H

#if !defined(PS2) || !defined(SN)
#pragma pack(1)
#endif
class u24
{
public:
	inline u24(void) {}
	inline u24(u32 i);

	// assign it
	inline u24 &operator=(const u24 &i);

	// compare it
	inline int operator==(const u32 i) const;
	inline int operator==(const u24 &i) const;
	inline int operator!=(const u32 i) const;
	inline int operator!=(const u24 &i) const;
	inline int operator>=(const u32 i) const;
	inline int operator>(const u32 i) const;
	inline int operator<=(const u32 i) const;
	inline int operator<(const u32 i) const;

	// cast it
	inline operator u32(void) const;

private:
	u8				data[3];
};
#if !defined(PS2) || !defined(SN)
#pragma pack()
#endif

#include "container/u24.hpp"

#endif
