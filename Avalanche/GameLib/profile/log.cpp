//***************************************************************************
// The debugging API
//***************************************************************************

/******************* includes ****************************/

#include "profile/ProfilePCH.h"





// Here is the main log
Log g_log;

//***************************************************************************
// Do nothing by default
//***************************************************************************
Log::Log(void)
{
	logFile = NULL;
	textPage = NULL;
}

//***************************************************************************
// Construct and open
//***************************************************************************
Log::Log(
char *logFileName,
bool flushEveryWrite)
{
	if (!Open(logFileName, flushEveryWrite))
	{
		logFile = NULL;
		textPage = NULL;
	}
}

//***************************************************************************
// Make sure we get closed
//***************************************************************************
Log::~Log(void)
{
	Close();
}

//***************************************************************************
// Open the file/create the window
//***************************************************************************
bool Log::Open(
char *logFileName,
bool flushEveryWrite)
{
#ifdef GCN
	return false;
#else
	char realFile[256];

	// Open the file
	if (!logFileName)
	{
		logFile = NULL;
		strcpy(realFile, "no file");
	}
	else
	{
		int num = 0;

		// Try multiple times to open the file
		strcpy(realFile, logFileName);
		while ((logFile = fopen(logFileName, "wt")) == NULL)
		{
			char numStr[32];
			int extPos, len;

			// Try a numbered version
			sprintf(numStr, "%d", num++);

			// Pick apart the file
			for (len = strlen(logFileName) - 1, extPos = len; len > 0; --len)
				if (logFileName[len] == '.')
				{
					extPos = len;
					break;
				}

			// Make the new file name
			strcpy(realFile, logFileName);
			strcpy(&realFile[extPos], numStr);
			strcat(realFile, &logFileName[extPos]);
		}
	}

	// Flushing every write?
	flushEvery = flushEveryWrite;

	// Make the window
	char title[128];
	sprintf(title, "Log - %s", realFile);
	textPage = TextPageSystem::CreatePage(title);

	Write("Debug session opened.\n");
	return(true);
#endif
}

//***************************************************************************
// Clean up
//***************************************************************************
void Log::Close(void)
{
	Write("Debug session closed.\n");

	// Close the file
	if (logFile)
	{
		fclose(logFile);
		logFile = NULL;
	}

	// Kill the window
	if (textPage)
	{
		TextPageSystem::DestroyPage(textPage);
		textPage = NULL;
	}
}

//***************************************************************************
// Write a message to the window and the log
//***************************************************************************
void CCALL Log::Write(
char *format,
...)
{
	va_list args;

	va_start(args, format);
	Write(format, args);
	va_end(args);
}

//***************************************************************************
// Write a message to the window at position x, y
//***************************************************************************
void CCALL Log::WriteXY(
int x,
int y,
char *format,
...)
{
	va_list args;

	va_start(args, format);
	WriteXY(x, y, format, args);
	va_end(args);
}

//***************************************************************************
// va_list debug
//***************************************************************************
void Log::Write(
char *format,
va_list args)
{
	char buff[256];

	// Format the string
	MakeOutputString(buff, format, args);

	// Write it to the window
	if (textPage)
		textPage->Root()->TTYWrite(buff);

	// Write it to the file
	if (logFile)
	{
		fputs(buff, logFile);
		if (flushEvery)
			fflush(logFile);
	}
}

//***************************************************************************
// va_list debugXY
//***************************************************************************
void Log::WriteXY(
int x,
int y,
char *format,
va_list args)
{
	// Write it out
	if (textPage)
	{
		char buff[256];

		// Format the string
		MakeOutputString(buff, format, args);
		textPage->Root()->XYWrite(x, y, buff);
	}
}

//***************************************************************************
// Make the format string
//***************************************************************************
char *Log::MakeOutputString(
char *buffer,
char *format,
va_list args)
{
	vsprintf(buffer, format, args);
	return(buffer);
}

