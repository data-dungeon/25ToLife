// TJC - 
// TJC - This is used to convince GCC to handle pure virtuals our way
// TJC - 

#include "platform/PlatformPCH.h"


extern "C"
{
	// later than 2.95.3
	void __cxa_pure_virtual() {
#ifdef DISABLE_ASSERTS
		// TJC - pure virtual function called
		LOCKUP();
#else
		ASSERTS(false,"pure virtual function called");
#endif
	}

	// 2.95.3 and earlier
	void __pure_virtual() {
		__cxa_pure_virtual();
	}

};

