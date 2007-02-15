////////////////////////////////////////////////////////////////////////////
//
// SimpleLog
//
// class to handle the log within a dll app
//
////////////////////////////////////////////////////////////////////////////


#include "TupperChainDLLPch.h"
#include "simplelog.h"

SimpleLog::SimpleLog(const char *pLogFilename) : m_verboseLevel(0)
{
   m_bValidLogName = false;
	if (pLogFilename)
	{
      m_logFilename = pLogFilename;
		FILE *pFile = fopen(pLogFilename,"wb");
      if (pFile)
      {
         fprintf(pFile,"Created LogFile\r\n");
         fclose(pFile);
         m_bValidLogName = true;
      }
	}
}

SimpleLog::~SimpleLog(void)
{
}

void SimpleLog::Add(int flags,const char *pcFmt, ...)
{
   if (m_bValidLogName)
   {
	   int verboseLevel = (flags>>LogFlags::VERBOSE_LEVEL_SHIFT)&LogFlags::VERBOSE_LEVEL_MASK;

//	   if (verboseLevel<=m_verboseLevel)
//	   {
		   int nLen;
		   static char acMessage[800];
		   va_list PrintArgs;

		   /* create message */

		   va_start(PrintArgs, pcFmt);
		   vsprintf(acMessage, pcFmt, PrintArgs);
		   va_end(PrintArgs);

		   /* anything to do? */
		   if ((nLen = strlen(acMessage)) == 0)
			   return;

         FILE *pFile = fopen(m_logFilename,"ab");
         if (pFile)
         {
            fprintf(pFile,"%s\r\n",acMessage);
            fclose(pFile);
         }
//	   }
   }
}

void SimpleLog::Clear(void)
{
}

int SimpleLog::GetVerboseLevel(void)
{
	return m_verboseLevel;
}

void SimpleLog::SetVerboseLevel(int verboseLevel)
{
	m_verboseLevel = verboseLevel;
}

void SimpleLog::SetProgressBar(int min,int max, int value)
{
}

void SimpleLog::SetProgressText(const char *pFormatString, ...)
{
}

bool SimpleLog::GetAbortFlag(void)
{
	return false;
}

