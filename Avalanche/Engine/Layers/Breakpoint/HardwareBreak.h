//============================================================================
//=
//= HardwareBreak.h - Support for hardware breakpoints
//=
//============================================================================

#ifndef _HARDWARE_BREAK_H
#define _HARDWARE_BREAK_H

#include "platform/BaseType.h"

#if defined(PS2)
typedef void (*TrapFunc)(u32 stat, u32 cause, u32 epc, u32 bva, u32 bpa, u128* gpr);
#endif

namespace HardwareBreak
{
	void Initialize();
	void Enable(bool write, bool read, u32 address, u32 length);
	void Enable(bool write, bool read, u32 address, u32 length, u32 data, u32 datamask);
	void Disable();
	void FaultTest();

#if defined(PS2)
	void SetTrapHandler(TrapFunc func);
#endif
};

#endif //_HARDWARE_BREAK_H
