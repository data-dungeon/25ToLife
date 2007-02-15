#ifndef __COMPILER_CONTROL_H__
#define __COMPILER_CONTROL_H__

#ifdef PS2

	#if defined(PS2) && defined(SN)
		#define SCRATCHPAD(type, variable)	type variable	__attribute__((section(".spad")))
	#else
		#define SCRATCHPAD(type, variable)	type __declspec(scratchpad) variable
	#endif

	// named sections only work in sn for now... Its not that CW doesnt support them, just
	// that we havent taken the time to do the support in here & the lcf file...
	#if ( defined(PS2) && defined(SN) ) && !defined( _DEBUG )
		#define NAMED_SECTION(s)	__attribute__((section(s)))
		#define __RESTRICT__			__restrict
	#else
		#define NAMED_SECTION(s)
		#define __RESTRICT__
	#endif

	// macro to set the alignment under gcc...
	#define ALIGNED(d)			__attribute__((aligned(d)))

#else // PS2

	// these are the non-ps2 versions of these macros...
	#define NAMED_SECTION(s)
	#define ALIGNED(d)
	#define SCRATCHPAD(type, variable) type variable
																												 
#endif // PS2

#endif // __COMPILER_CONTROL_H__
