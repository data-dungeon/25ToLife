//============================================================================
//=
//= HardwareBreak.cpp - Support for hardware breakpoints
//=
//============================================================================

#include "Layers/LayersPCH.h"
#include "HardwareBreak.h"

#if !defined(CONSUMER_BUILD)
	#if defined(PS2)
		#include "HardwareBreak_ps2.cpp"
	 	#define HARDWARE_BREAK_SUPPORTED
	#elif defined(_XBOX)
	#elif defined(GCN)
	#elif defined(WIN32)
	#endif
#endif

#if !defined(HARDWARE_BREAK_SUPPORTED)
void HardwareBreak::Initialize();
void HardwareBreak::Enable(bool write, bool read, u32 addr, u32 len) {}
void HardwareBreak::Enable(bool write, bool read, u32 addr, u32 len, u32 data, u32 datamask) {}
void HardwareBreak::Disable() {}
void HardwareBreak::FaultTest() {}
#if defined(PS2)
void SetTrapHandler(TrapFunc func) {}
#endif
#endif
