//***************************************************************************
// Stats tracking structure
//***************************************************************************
#include <stdio.h>
#include "stats.h"

/***************************************************************************/
/***************************************************************************/
Stats::Stats()
{
#ifdef _DEBUG
	verbosity = VERBOSITY_DEBUG;
#else
	verbosity = VERBOSITY_NOTIFY;
#endif
	onProgressLine = false;
	SetProgressText("Working...");
}

/***************************************************************************/
// Log a message to the console
/***************************************************************************/
void __cdecl Stats::Log(
VERBOSITY theVerbosity,
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(theVerbosity, format, args);
	va_end(args);
}

/***************************************************************************/
// Log a message with debug level to the console
/***************************************************************************/
void __cdecl Stats::Echo(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(VERBOSITY_SILENT, format, args);
	va_end(args);
}

/***************************************************************************/
// Log a message with debug level to the console
/***************************************************************************/
void __cdecl Stats::Error(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(VERBOSITY_ERROR, format, args);
	va_end(args);
}

/***************************************************************************/
// Log a message with debug level to the console
/***************************************************************************/
void __cdecl Stats::Notify(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(VERBOSITY_NOTIFY, format, args);
	va_end(args);
}

/***************************************************************************/
// Log a message with debug level to the console
/***************************************************************************/
void __cdecl Stats::Debug(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(VERBOSITY_DEBUG, format, args);
	va_end(args);
}

/***************************************************************************/
/***************************************************************************/
void Stats::SetProgressText(
const char *progressMessage)
{
	progress.Cpy(progressMessage);
	progress.Cat("%d%%\r");
	if (onProgressLine)
		printf("\n");
}

/***************************************************************************/
// Update progress
/***************************************************************************/
void Stats::UpdateProgress(
int on,
int of)
{
	if (verbosity > VERBOSITY_ERROR)
	{
		int percent = (int)(((float)on / (float)of) * 100.0f);
		if (percent < 0) percent = 0; else if (percent > 100) percent = 100;
		printf(progress.Data(), percent);
		onProgressLine = true;
	}
}

/***************************************************************************/
// Log message with format/string
/***************************************************************************/
void Stats::LogMessage(
VERBOSITY theVerbosity,
const char *format,
va_list args)
{
	// this could overflow
	char temp[1024];
	vsprintf(temp, format, args);
	if (theVerbosity <= verbosity)
	{
		if (onProgressLine)
			printf("\n");
		printf(temp);
		onProgressLine = false;
	}
}


