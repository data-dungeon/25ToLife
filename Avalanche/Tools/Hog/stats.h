//***************************************************************************
// Show stats
//***************************************************************************
#ifndef STATS_H
#define STATS_H

#include <stdarg.h>
#include "toolhelper\dynstr.h"

class Stats
{
public:
	Stats();
	~Stats() {}

	enum VERBOSITY
	{
		VERBOSITY_SILENT = 0x0,
		VERBOSITY_ERROR = 0x1,
		VERBOSITY_NOTIFY = 0x2,
		VERBOSITY_DEBUG = 0x3
	};	
	void SetVerbosity(VERBOSITY newVerbosity)
		{ verbosity = newVerbosity; }

	// Show a message
	void __cdecl Log(VERBOSITY theVerbosity, const char *format, ...);
	void __cdecl Echo(const char *format, ...);
	void __cdecl Error(const char *format, ...);
	void __cdecl Notify(const char *format, ...);
	void __cdecl Debug(const char *format, ...);

	// Update progress
	void SetProgressText(const char *progressMessage);
	void UpdateProgress(int on, int of);

private:
	VERBOSITY		verbosity;
	bool				onProgressLine;
	DynString		progress;

	// the workhorse
	void LogMessage(VERBOSITY theVerbosity, const char *format, va_list args);
};

#endif
