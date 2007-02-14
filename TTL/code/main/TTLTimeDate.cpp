/***************************************************************************/
// date and time defines
/***************************************************************************/

#if defined(PS2)
	const char* TTL_VERSION = "PS2: "   __DATE__ " " __TIME__;
#elif defined(_XBOX)
	const char* TTL_VERSION = "XBOX: "  __DATE__ " " __TIME__;
#elif defined(WIN32)
	const char* TTL_VERSION = "WIN32: " __DATE__ " " __TIME__;
#else
	const char* TTL_VERSION = "VER: "   __DATE__ " " __TIME__;
#endif
