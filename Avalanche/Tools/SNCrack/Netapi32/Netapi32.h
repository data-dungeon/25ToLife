// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NETAPI32_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NETAPI32_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef NETAPI32_EXPORTS
#define NETAPI32_API __declspec(dllexport)
#else
#define NETAPI32_API __declspec(dllimport)
#endif

// This class is exported from the Netapi32.dll
class NETAPI32_API CNetapi32 {
public:
	CNetapi32(void);
	// TODO: add your methods here.
};

extern NETAPI32_API int nNetapi32;

NETAPI32_API int fnNetapi32(void);
