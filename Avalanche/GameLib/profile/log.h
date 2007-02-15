//***************************************************************************
// A butt-simple debugging log system that writes to an auxtext window
// and a file.
//***************************************************************************
#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>

// Windows does not default to cdecl
#ifdef WIN32
#define CCALL	__cdecl
#else
#define CCALL
#endif

// Forwards for quick compilation
class TextWindow;
class TextPage;

class Log
{
public:
	// Constructor/destructors
	Log(void);
	Log(char *logFileName, bool flushEveryWrite);
	~Log(void);

	// Open the file and create a window
	bool Open(char *logFileName, bool flushEveryWrite);

	// close everything (including the window)
	void Close(void);

	// Write a message to the window and the log
	void CCALL Write(char *format, ...);

	// Write a message to the window only using a position
	void CCALL WriteXY(int x, int y, char *format, ...);

	// va_list write
	void Write(char *format, va_list args);

	// va_list write at XY
	void WriteXY(int x, int y, char *format, va_list args);

private:
	FILE			*logFile;
	bool			flushEvery;
	TextPage 	*textPage;

	// Make the format string
	char *MakeOutputString(char *buffer, char *format, va_list args);
};

// There is a global log for all to use (better call Open on it!)
extern Log g_log;

#endif
