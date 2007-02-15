#ifndef P_ASSERT_H
#define P_ASSERT_H

#if defined( WIN32_TOOLS_DEBUG ) || defined( WIN32_TOOLS_RELEASE ) || defined( WIN32_TOOLS_HYBRID )

// these are for the tools
// the filterchain includes some libraries from the engine
#include <crtdbg.h>
#define ASSERT _ASSERT
#define ASSERTS(expr, string) ASSERT(expr)
#define ASSERTF __assertf
#define ASSERT_PTR(expr)				((void)0)
#define ASSERTS_PTR(expr, string)	((void)0)
#define ASSERTF_PTR(expr, string)	((void)0)
#define LOCKUP() while(1);
#define CHECK_PTR(addr) (addr)
#define HARDWARE_BREAKPOINT( addr, read, write, setVal, val, blockSize ) ((void)0)

inline void __assertf(bool condition, ...)
{
	// ok, so Im avoiding doing a real one here :)...
	//
	// ned says: this used to __assertf(bool condition, char *format, ...), and it called ASSERTS(condition, format),
	// but i found that even that was too ambitous-- it couldn't handle the parentheses that we have to use in
	// and ASSERTF call around the format and optional parameters, so it's even simpler now
	ASSERT(condition);
}
#else

/* assert macro */

// logic here is:
// !ps2 builds: if !debug, then no asserts
// ps2 builds: if cdrom, then no asserts

#if defined(PS2) && !defined(_DEBUG) && ( defined(ROBN) || defined(NIGHTLY) )
//#define DISABLE_ASSERTS
#endif

#if defined(PS2) && !defined(_DEBUG) && ( defined(AVUSER_TJC) )
//#define DISABLE_ASSERTS
#endif

#if defined(PS2) && defined(CDROM) && !defined(_DEBUG)
#define DISABLE_ASSERTS
#endif

#if defined(_XBOX) && defined(CDROM)
#define DISABLE_ASSERTS
#endif

#if (!defined(GCN)) && (!defined(PS2)) && (!defined(_DEBUG) && (!defined(WIN32)) ) || defined( TOOLS )
#define DISABLE_ASSERTS
#endif

// GameCube does not like asserts
#if (defined(GCN) && (defined(CDROM)))
#define DISABLE_ASSERTS
#endif

#if defined(GCN)
#include <Dolphin/os.h>
#define LOCKUP() { asm { opword 0 } }
#define LOCKUP_MEM() { asm { opword 0 } }
#define HARDWARE_BREAKPOINT( addr, read, write, setVal, val, blockSize ) ((void)0)
#elif defined(CW)
#include <libgraph.h>
#define LOCKUP() asm("breakc 0x0; nop;")
#define LOCKUP_MEM( ) { while( 1 ){ DPUT_GS_BGCOLOR( 0x000000ff ); } }
#define HARDWARE_BREAKPOINT( addr, read, write, setVal, val, blockSize ) ((void)0)
#elif ( defined(PS2) && defined(SN) )
#include <libgraph.h>
#define LOCKUP_MEM( ) { while( 1 ){ DPUT_GS_BGCOLOR( 0x000000ff ); } }
#define LOCKUP() asm("break 1")

#elif defined(WIN32) && !defined (_XBOX)
#define LOCKUP() __asm { int 3 }
#define LOCKUP_MEM() __asm { int 3 }
#elif defined (_XBOX)
//#include "Display/DirectX/DirectX.h"
#ifndef CONSUMER_BUILD
void LOCKUP(); 
void LOCKUP_MEM(); 
#else
inline void LOCKUP() { while (1); }
inline void LOCKUP_MEM() { while (1); }
#endif
#define HARDWARE_BREAKPOINT( addr, read, write, setVal, val, blockSize ) ((void)0)
#else
#define LOCKUP() while(1);
#define LOCKUP_MEM() while(1);
#define HARDWARE_BREAKPOINT( addr, read, write, setVal, val, blockSize ) ((void)0)
#endif

#if defined(CGN)
#define CHECK_PTR(addr) (addr)
#elif defined(PS2)
// TJC - check that its non-null and does not point below the first
// megabyte (into kernel space) and that it does not point beyond the
// 128MB mark (beyond available memory on the T10K
#define CHECK_PTR(addr) (((((u32)addr) & 0xFFF00000) != 0) && ((((u32)addr) & 0xF8000000) == 0))
#elif defined(WIN32)
// TJC - ask the OS if this pointer points into our heap.  We cant do
//this because it will also fail on pointers into the stack... we need
//to make that valid...
//#include <crtdbg.h>
//#define CHECK_PTR(addr) (addr && _CrtIsValidHeapPointer((const void*)addr))
#define CHECK_PTR(addr) (addr)
#else
#define CHECK_PTR(addr) (addr)
#endif

#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT_BUFFER_LEN 512

#ifdef DISABLE_ASSERTS

/* DISABLE_ASSERTS is defined, we want ASSERT to be an empty statement */

#define ASSERT(expr)						((void)0)
#define ASSERTS(expr, string)			((void)0)
#define ASSERTF(expr, format)			((void)0)

// check that the pointer is valid for this process
#define ASSERT_PTR(expr)				((void)0)
#define ASSERTS_PTR(expr, string)	((void)0)
#define ASSERTF_PTR(expr, string)	((void)0)

#ifndef TOOLS
// verify versions of the asserts
#define VERIFY(expr)						((void)(expr))
#define VERIFYS(expr, string)			((void)(expr))
#define VERIFY_PTR(expr) 				((void)(expr))
#define VERIFYS_PTR(expr, string)	((void)(expr))
#define VERIFYF(expr, format)			((void)(expr))
#define VERIFYF_PTR(expr, format)	((void)(expr))
#endif

#else //DISABLE_ASSERTS

/* DISABLE_ASSERTS is not defined, we want ASSERT to do something */

// this turns on some code to handle the ignore always functionality...
#define ASSERT_SUPPORT_IGNORE_ALWAYS

// this turns on code to handle emailing asserts to people...
#define ASSERT_SUPPORT_EMAIL

#define ASSERT(expr) do {																		\
	if(!(expr))																						\
	{																									\
		ASSERTVAR(assertExpression) = #expr;				\
		ASSERTVAR(assertFile) = __FILE__;					\
		if(!Assert::CallHandler(assertExpression, assertFile, __LINE__, NULL))	\
			LOCKUP();																				\
	}																									\
} while(0)

#define ASSERTS(expr, string) do {																		\
	if(!(expr))																									\
	{																												\
		ASSERTVAR(assertExpression) = #expr;							\
		ASSERTVAR(assertFile) = __FILE__;								\
		ASSERTVAR(assertString) = string;								\
		if(!Assert::CallHandler(assertExpression, assertFile, __LINE__, assertString))	\
			LOCKUP();																							\
	}																												\
} while(0)

#define ASSERTF(expr, format) do {																		\
	if(!(expr))																									\
	{																												\
		ASSERTVAR(assertExpression) = #expr;							\
		ASSERTVAR(assertFile) = __FILE__;								\
		const char *assertString = Assert::FormatString format;									\
		if(!Assert::CallHandler(assertExpression, assertFile, __LINE__, assertString))	\
			LOCKUP();																							\
	}																												\
} while(0)

// for now, just verify that they are not null
#define ASSERT_PTR(expr)				ASSERT(CHECK_PTR(expr))
#define ASSERTS_PTR(expr, string)	ASSERTS(CHECK_PTR(expr), string)
#define ASSERTF_PTR(expr, format)	ASSERTF(CHECK_PTR(expr), format)

#undef VERIFY	//for pigpen-- mfc complains in a release build
#define VERIFY(expr)						ASSERT(expr)
#define VERIFYS(expr, string)			ASSERTS(expr, string)
#define VERIFYF(expr, format)			ASSERTF(expr, format)
#define VERIFY_PTR(expr) 				ASSERT_PTR(CHECK_PTR(expr))
#define VERIFYS_PTR(expr, string)	ASSERTS_PTR(CHECK_PTR(expr), string)
#define VERIFYF_PTR(expr, format)	ASSERTF_PTR(CHECK_PTR(expr), format)


#if defined(PS2) && defined(SN)

#define HARDWARE_BREAKPOINT( addr, read, write, setVal, val, blockSize ) Assert::HardwareBreakpoint( addr, read, write, setVal, val, blockSize );
//#ifdef PS2

#ifdef CW
#pragma define_section assert ".assert_istr_dummy" ".assert" far_absolute RW
#endif // CW

#define ASSERT_ATTRIBUTE __attribute__ ((section (".assert")))
#define ASSERTVAR(name) static const char name[] ASSERT_ATTRIBUTE

#else // PS2

#define ASSERTVAR(name) const char* name

#endif // PS2

namespace Assert
{

	// NOTE!!!!
	// Be sure to surround all calls to these functions with
	// a #ifndef DISABLE_ASSERTS
	bool InAssert();

	// here is the definition of an assert handler
	// the bool is used to halt the processor.  If its false,
	// we halt the processer via LOCKUP()
	typedef bool Handler(const char *expr, const char *file, int line, const char* string);

	// this is the function that actually calls the assert handler
	Handler CallHandler;

	// formats strings into the global buffer Assert::AssertFBuffer
	// and returns a pointer to Assert::AssertFBuffer.  THERE IS ONLY 1 BUFFER
	// This shouldnt be a problem unless you start doing screwy things like
	// nesting asserts...
	const char* FormatString(const char *format, ...);

	// this is how you set an assert handler..
	// it needs to be set and maintained by the platform specific code.
	// for example, ps2app.cpp calls:
	// Assert::SetHandler(Assert::OnScreenPS2);
	// Be sure to surround all calls to these functions with
	// a #ifndef DISABLE_ASSERTS
	void SetHandler(Handler* handler);

	// this function translates what the compiler generates into a C style string..
	// usually, these are the same thing, so this function is usually quite.
#if !defined(PS2) || !defined(SN)
#define StringLookup(s) s
#else
	class builtstring {
	public:
		char stringbuffer[ASSERT_BUFFER_LEN];
		operator const char*() { return stringbuffer; }
		builtstring(const char* buff) { strcpy(stringbuffer, buff); }
	};
	builtstring StringLookup(const char* string);
#endif

	// here is the generic assert handler... it just uses dbgPrint() and LOCKUP()
	Handler Default;

	// here lie all the assert handlers for the specific platforms
#if defined(_XBOX)
	Handler XboxAssertHandler;

#elif defined(WIN32)

	// this version uses our own dlg resource templates and allows for IngoreAlways... :)
	// I use a larger font too to help it stand out against other dialogs on the system.
	Handler Dialog;

	// this version uses _CrtDbgReport() which does not allow for 4 buttons.. :(..
	// but otherwise is fully functional (no IngoreAlways)
	Handler CrtReport;

	// very simple version for pigpen
	Handler PigpenHandler;

#elif defined(GCN)
	Handler GCNAssertHandler;
#elif defined(PS2)

	// this is an on screen handler.  it uses the controller and stuff...
	// its cool.. :)..
	Handler OnScreen;

	// this is used to set the executable name on the PS2...
	// the executable name is used to load the assert strings
	// since they are in a NOLOAD segment
	// Be sure to surround all calls to these functions with
	// a #ifndef DISABLE_ASSERTS
	void SetExecutableName(const char* executable);
#endif

	// here are your helpers to make supporting ignore always easier for handlers...
#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
	bool IgnoreAlways(const char* expr, const char* file, int line, const char* string);
	void SetIgnoreAlways(const char* expr, const char* file, int line, const char* string, bool ignore);
#endif

#ifdef ASSERT_SUPPORT_EMAIL
	// here are helpers to make supporting emailing asserts easier for handlers
	void EmailAssert(int EMailAddressIndex, const char* expr, const char* file, int line, const char* string);
#endif

	void HardwareBreakpoint( unsigned int addr, bool read, bool write, bool setVal, unsigned int value, unsigned int blockSize );
};

#endif //DISABLE_ASSERTS

#endif // WIN32_MAX_DEBUG

#endif	// P_ASSERT_H
