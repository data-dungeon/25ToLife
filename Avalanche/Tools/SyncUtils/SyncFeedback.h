////////////////////////////////////////////////////////////////////////////
//
// SyncFeedback
//
// Class used dealing with the feedback while synching the database
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncFeedback.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 10:44a
 * Updated in $/Avalanche/tools/SyncUtils
 * Error Handling
 * Feedback on directories
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/16/02   Time: 4:04p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync feedback fixes (ready to test)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/16/02   Time: 2:44p
 * Updated in $/Avalanche/tools/SyncUtils
 * SyncDialog foundation
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/14/02   Time: 4:00p
 * Updated in $/Avalanche/tools/SyncUtils
 * simple feedback classes
 */

#ifndef SYNC_UTILS_DLL_SYNC_FEEDBACK_H
#define SYNC_UTILS_DLL_SYNC_FEEDBACK_H

//-----------------------------------------------------------------------------
// SyncFeedback class declaration:

class LogFlags
{
public:
	enum
	{
		FONT_STYLE_BOLD					= 1<<3,
		FONT_STYLE_ITALIC				= 1<<4,
		FONT_STYLE_STRIKEOUT			= 1<<5,
		FONT_STYLE_UNDERLINE			= 1<<6,

		FONT_COLOR_SHIFT				= 8,
		FONT_COLOR_MASK					= 0xff,
		FONT_COLOR_BLACK				= 0<<8,
		FONT_COLOR_DK_GREY				= 1<<8,
		FONT_COLOR_LT_GREY				= 2<<8,
		FONT_COLOR_WHITE				= 3<<8,
		FONT_COLOR_RED					= 4<<8,
		FONT_COLOR_LT_RED				= 5<<8,
		FONT_COLOR_DK_RED				= 6<<8,
		FONT_COLOR_ORANGE				= 7<<8,
		FONT_COLOR_LT_ORANGE			= 8<<8,
		FONT_COLOR_DK_ORANGE			= 9<<8,
		FONT_COLOR_YELLOW				= 10<<8,
		FONT_COLOR_LT_YELLOW			= 11<<8,
		FONT_COLOR_DK_YELLOW			= 12<<8,
		FONT_COLOR_GREEN				= 13<<8,
		FONT_COLOR_LT_GREEN				= 14<<8,
		FONT_COLOR_DK_GREEN				= 15<<8,
		FONT_COLOR_CYAN					= 16<<8,
		FONT_COLOR_LT_CYAN				= 17<<8,
		FONT_COLOR_DK_CYAN				= 18<<8,
		FONT_COLOR_BLUE					= 19<<8,
		FONT_COLOR_LT_BLUE				= 20<<8,
		FONT_COLOR_DK_BLUE				= 21<<8,
		FONT_COLOR_MAGENTA				= 22<<8,
		FONT_COLOR_LT_MAGENTA			= 23<<8,
		FONT_COLOR_DK_MAGENTA			= 24<<8,
	};
};

class AFX_EXT_CLASS SyncFeedback
{
public:
	enum SyncMode
	{
		SYNC_MODE_NULL,
		SYNC_MODE_COPY_LEFT_TO_RIGHT,
		SYNC_MODE_COPY_RIGHT_TO_LEFT,
		SYNC_MODE_DELETING_LEFT,
		SYNC_MODE_DELETING_RIGHT,
	};

	SyncMode m_syncMode;
	TupString m_sourceFile;
	TupString m_destFile;
	__int64 m_totalFileBytes;
	__int64 m_totalFileBytesCopied;
	__int64 m_totalBytes;
	__int64 m_totalBytesCopied;
	CTime m_startTime;
	int m_errorCount;
	bool m_bPreviewMode;
	bool m_bIncludePath;

	SyncFeedback(void);
	virtual ~SyncFeedback();

	virtual void UpdateStats(void)=0;
	virtual void UpdateError(void)=0;
	virtual void ClearStats(void)=0;
	virtual void AddToLog(int flags,const char *pFormatString, ...)=0;
	virtual void ClearLog(void)=0;
	virtual bool GetAbortFlag(void) const=0;

};

//-----------------------------------------------------------------------------

#endif

