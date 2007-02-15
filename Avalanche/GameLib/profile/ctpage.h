//***************************************************************************
// A class to help you time sections of code
//***************************************************************************
#ifndef CTPAGE_H
#define CTPAGE_H

#include "profile/cputimer.h"
#include "profile/textpage.h"

// Here are the time formats allowed
typedef enum
{
	TIMEFORMAT_CLOCKCYCLES,
	TIMEFORMAT_MICROSEC,
	TIMEFORMAT_MILLISEC,
	TIMEFORMAT_SEC,
	TIMEFORMAT_AUTO
} TIMEFORMAT;

// Here are the things you can display
#define TIME_SHOWWORST  0x01
#define TIME_SHOWBEST   0x02
#define TIME_SHOWAVG    0x04
#define TIME_SHOWCALLS	0x08

enum
{
	TIME_CURRENT,
	TIME_WORST,
	TIME_BEST,
	TIME_AVG,
	TIME_NUM
};

// Internals for the class
// Here is how we keep track of time entries
struct TIMEENTRY
{
	char     	tag[32];
	TIMEFORMAT 	timeFormat;
	int      	len;
	CPUTIME   	startTime;
	CPUTIME   	time[TIME_NUM];
	int      	showTime;
	char     	worstTimeStr[64];
	char     	bestTimeStr[64];
	int			sampleNumber;
	int			sampleFrame;
	int			allowCumulative;
	int			calls;
	CPUTIME		widthMaxTime[TIME_NUM];
	int			widthMaxCalls;
};

// How many time entries can we have?
#define TIMEENTRIES (TEXTSCR_HEIGHT - 2)

class CodeTimerPage
{
public:
	CodeTimerPage(char *pageName);
	~CodeTimerPage(void);

	// Destroy the page
	void Destroy(void);

	// Use this to "name" a section for later reference, returns index
	// the your registered it...
	int RegisterSection(char *sectionName, TIMEFORMAT timeFormat, int showTime = TIME_SHOWAVG, bool allowCumulative = true);

	// Use these calls around the section
	inline void SectionStart(int sectionIndex);
	inline void SectionEnd(int sectionIndex);

	// Use these at the start and end of a frame
	inline void FrameStart(void);
	inline void FrameEnd(void);

	// Here's what controls the screen
	void ComputeStats(void);
	void UpdateScreen(void);

	// Setup a reset for avg/worst/best
	inline void Reset(int frames,	int oneShot);

	// Mangage width stuff
	void EnableWidthMemory(void);
	void DisableWidthMemory(void);

private:
	// Our output window
	TextPage		         *timerPage;
	TIMEENTRY            timeEntry[TIMEENTRIES];
	int						entries;
	int                  maxTagLen;

	// Reset stats
	int                  frameNumber;
	int                  resetFrame;
	int                  resetOneShot;

	// Field widths
	bool						widthMemory;
	int						timeWidth[TIME_NUM];
	int						callsWidth;

	// Make sure all the times are formatted
	void FormatTimes(TIMEENTRY &t, char *buff);

	// Format a single time
	void FormatTime(CPUTIME timeDiff, TIMEFORMAT format, char *buff, int width);

	// Find widths
	void FindTimeWidth(int index);
	void FindCallWidth(void);

	// Reset all the stats
	inline void Reset(void);

	// My own string copy
	static char *strncpy(char *dest, char *source, int maxlen);

public:
	// We exist in a list
	CodeTimerPage	*prev, *next;
};

// Include the implementation down here
#include "profile/ctpage.hpp"

#endif
