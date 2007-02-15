////////////////////////////////////////////////////////////////////////////
//
// PluginLog
//
// class for all plugin logs
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: PluginLog.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Abort checks within system
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 3:16p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Progress Bar
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/01/02    Time: 12:05p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Rich Edit Control for the Log.  Added additional flags for font styles
 * and colors.
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
 */
  
#ifndef __PLUGIN_LOG_H
#define __PLUGIN_LOG_H

class LogFlags
{
public:
	enum
	{
		VERBOSE_LEVEL_SHIFT				= 0,
		VERBOSE_LEVEL_MASK				= 0x03,
		VERBOSE_LEVEL_NONE				= 0<<0,
		VERBOSE_LEVEL_LOW				= 1<<0,
		VERBOSE_LEVEL_MED				= 2<<0,
		VERBOSE_LEVEL_HIGH				= 3<<0,

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

class TupperChainLogBase
{
public:
	TupperChainLogBase(void) {}
	virtual ~TupperChainLogBase() {}

	virtual void Add(int flags,const char *pFormatString, ...) = 0;
	virtual void Clear(void) = 0;
	virtual int GetVerboseLevel(void) = 0;
	virtual void SetProgressBar(int min,int max, int value) = 0;
	virtual void SetProgressText(const char *pFormatString, ...) = 0;
	virtual bool GetAbortFlag(void) = 0;
};

class PluginLog
{
	static TupperChainLogBase *m_pLog;
public:
	static void SetLog(TupperChainLogBase *pLog);
	static void Add(int verboseLevel,const char *pFormatString, ...);
	static void Clear(void);
	static int GetVerboseLevel(void);
	static void SetProgressBar(int min,int max, int value);
	static void SetProgressText(const char *pFormatString, ...);
	static bool GetAbortFlag(void);
};

#endif