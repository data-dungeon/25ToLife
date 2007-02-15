//***************************************************************************
// A class to help you time sections of code
//***************************************************************************
/******************* includes ****************************/

#include "profile/ProfilePCH.h"





#define REMEMBER_WIDTH

//***************************************************************************
//***************************************************************************
CodeTimerPage::CodeTimerPage(
char *pageName)
{
	// None of these are used
	entries = 0;
	maxTagLen = 0;

	// Setup the sample stuff
	frameNumber = 0;
	resetFrame = 0;
	resetOneShot = true;

	// Setup the width stuff
	widthMemory = false;

	// Create the window
	timerPage = TextPageSystem::CreatePage(pageName, BORDER_FANCY);
}

//***************************************************************************
//***************************************************************************
CodeTimerPage::~CodeTimerPage(void)
{
	// Get rid of the window
	// Don't do this, we are global
	//TextPageSystem::DestroyPage(timerPage);
}

//***************************************************************************
//***************************************************************************
void CodeTimerPage::Destroy(void)
{
	TextPageSystem::DestroyPage(timerPage);
}

//***************************************************************************
// Use this to "name" a section for later reference
//***************************************************************************
int CodeTimerPage::RegisterSection(
char *sectionName,
TIMEFORMAT timeFormat,
int showTime,
bool allowCumulative)
{
	// Is there space?
	if (entries == TIMEENTRIES)
		return(-1);

	// Setup the record
	int i = entries;
	memset(&timeEntry[i], '\0', sizeof(timeEntry[i]));
	strncpy(timeEntry[i].tag, sectionName, sizeof(timeEntry[i].tag) - 1);
	timeEntry[i].timeFormat = timeFormat;
	timeEntry[i].allowCumulative = allowCumulative;
	timeEntry[i].len = strlen(timeEntry[i].tag);
	if (timeEntry[i].len > maxTagLen)
		maxTagLen = timeEntry[i].len;
	timeEntry[i].showTime = showTime;
	timeEntry[i].time[TIME_BEST] =  (CPUTIME)1000000;

	// One more entry
	++entries;

	// Return the index
	return(i);
}

//***************************************************************************
//***************************************************************************
void CodeTimerPage::EnableWidthMemory(void)
{
	if (widthMemory)
		return;
	widthMemory = true;

	int i, j;
	int minWidth = 1;
	int minCount = 10;
	for (i = 0; i < entries; i++)
	{
		if (timeEntry[i].timeFormat == TIMEFORMAT_AUTO)
		{
			minWidth = 3;
			minCount = 1000;
		}
		else if (timeEntry[i].timeFormat == TIMEFORMAT_SEC)
		{
			minWidth = 4;
			minCount = 10000;
			break;
		}
	}

	for (j = 0; j < TIME_NUM; j++)
		timeWidth[j] = minWidth;
	callsWidth = 1;

	CPUTIME minTime;
	for (i = 0; i < entries; i++)
	{
		switch (timeEntry[i].timeFormat)
		{
			case TIMEFORMAT_CLOCKCYCLES:
				minTime = g_CPUTimer.FromClockCycles(minCount);
				break;
			case TIMEFORMAT_MICROSEC:
				minTime = g_CPUTimer.FromMicroSeconds(minCount);
				break;
			case TIMEFORMAT_SEC:
				minTime = g_CPUTimer.FromSeconds((float)minCount);
				break;
			default:
			case TIMEFORMAT_MILLISEC:
			case TIMEFORMAT_AUTO:
				minTime = g_CPUTimer.FromMilliSeconds(minCount);
				break;
		}

		for (j = 0; j < TIME_NUM; j++)
			timeEntry[i].widthMaxTime[j] = minTime;
		timeEntry[i].widthMaxCalls = 10;
	}
}

//***************************************************************************
//***************************************************************************
void CodeTimerPage::DisableWidthMemory(void)
{
	widthMemory = false;
}

//***************************************************************************
// Compute all the statistics
//***************************************************************************
void CodeTimerPage::ComputeStats(void)
{
	// Increment the sample number
	for (int i = 0; i < entries; i++)
	{
		if (timeEntry[i].sampleFrame == frameNumber)
		{
			char temp[64];

			// Handle the sample number
			++timeEntry[i].sampleNumber;

			// Do the average time
			if (timeEntry[i].showTime & TIME_SHOWAVG)
				timeEntry[i].time[TIME_AVG] += (timeEntry[i].time[TIME_CURRENT] - timeEntry[i].time[TIME_AVG]) / timeEntry[i].sampleNumber;

			// What about worst time?
			if (timeEntry[i].showTime & TIME_SHOWWORST && timeEntry[i].time[TIME_CURRENT] > timeEntry[i].time[TIME_WORST])
			{
				timeEntry[i].time[TIME_WORST] = timeEntry[i].time[TIME_CURRENT];
				FormatTime(timeEntry[i].time[TIME_WORST], timeEntry[i].timeFormat, temp, timeWidth[TIME_WORST]);
				sprintf(timeEntry[i].worstTimeStr, " (worst %s)", temp);
			}

			// What about the best time?
			if (timeEntry[i].showTime & TIME_SHOWBEST && timeEntry[i].time[TIME_CURRENT] < timeEntry[i].time[TIME_BEST])
			{
				timeEntry[i].time[TIME_BEST] = timeEntry[i].time[TIME_CURRENT];
				FormatTime(timeEntry[i].time[TIME_BEST], timeEntry[i].timeFormat, temp, timeWidth[TIME_BEST]);
				sprintf(timeEntry[i].bestTimeStr, " (best %s)", temp);
			}
		}
	}
}

//***************************************************************************
// Here's what controls the screen
//***************************************************************************
void CodeTimerPage::UpdateScreen(void)
{
	TextWindow *timerWin = timerPage->Root();

	// Are we enabled?
	if (timerWin->Hidden())
		return;

	int i, line;
#ifdef REMEMBER_WIDTH
	if (widthMemory)
	{
		for (i = 0; i < TIME_NUM; i++)
			FindTimeWidth(i);
		FindCallWidth();
	}
#endif

	for (i = 0, line = 0; i < entries; i++)
	{
		// Write the tag
		timerWin->XYWriteN(maxTagLen + 1 - timeEntry[i].len, line, timeEntry[i].tag, timeEntry[i].len);
		timerWin->XYWriteN(maxTagLen + 1, line, ": ", 2);

		// Format the time
		char buff[128];
		FormatTimes(timeEntry[i], buff);
		timerWin->XYWrite(maxTagLen + 3, line, buff);
		line++;
	}
}

//*************************************************************************
// Make sure all the times are formatted
//*************************************************************************
void CodeTimerPage::FormatTimes(
TIMEENTRY &t,
char *buff)
{
	// Always have to do current time
	FormatTime(t.time[TIME_CURRENT], t.timeFormat, buff, timeWidth[TIME_CURRENT]);

	// Show the calls?
	if (t.showTime & TIME_SHOWCALLS)
	{
#ifdef REMEMBER_WIDTH
		if (widthMemory)
			sprintf(buff + strlen(buff), " (calls %*d)", callsWidth, t.calls);
		else
#endif
			sprintf(buff + strlen(buff), " (calls %d)", callsWidth, t.calls);
	}

	// Do the average time
	if (t.showTime & TIME_SHOWAVG)
	{
		char temp[64];
		FormatTime(t.time[TIME_AVG], t.timeFormat, temp, timeWidth[TIME_AVG]);
		sprintf(buff + strlen(buff), " (avg %s)", temp);
	}

	// What about worst time?
	if (t.showTime & TIME_SHOWWORST)
		strcat(buff, t.worstTimeStr);

	// What about the best time?
	if (t.showTime & TIME_SHOWBEST)
		strcat(buff, t.bestTimeStr);

	strcat(buff, "                  ");
}

//*************************************************************************
// Format a single time
//*************************************************************************
void CodeTimerPage::FormatTime(
CPUTIME timeDiff,
TIMEFORMAT format,
char *buff,
int width)
{
	// Format the string

#ifdef REMEMBER_WIDTH
	if (widthMemory)
	{
		switch (format)
		{
			case TIMEFORMAT_CLOCKCYCLES:
				sprintf(buff, "%*d cs", width, g_CPUTimer.ToClockCycles(timeDiff));
				break;

			case TIMEFORMAT_MICROSEC:
				sprintf(buff, "%*d æs", width, g_CPUTimer.ToMicroSeconds(timeDiff));
				break;

			case TIMEFORMAT_SEC:
				sprintf(buff, "%*.2f s", width, g_CPUTimer.ToSeconds(timeDiff));
				break;

			case TIMEFORMAT_MILLISEC:
				sprintf(buff, "%*d ms", width, g_CPUTimer.ToMilliSeconds(timeDiff));
				break;

			default:
			case TIMEFORMAT_AUTO:
			{
				int mSec = g_CPUTimer.ToMilliSeconds(timeDiff);
				if (mSec != 0)
					sprintf(buff, "%*d ms", width, mSec);
				else
					sprintf(buff, "%*d æs", width, g_CPUTimer.ToMicroSeconds(timeDiff));
				break;
			}
		}
	}
	else
#endif
	{
		switch (format)
		{
			case TIMEFORMAT_CLOCKCYCLES:
				sprintf(buff, "%d cs", g_CPUTimer.ToClockCycles(timeDiff));
				break;

			case TIMEFORMAT_MICROSEC:
				sprintf(buff, "%d æs", g_CPUTimer.ToMicroSeconds(timeDiff));
				break;

			case TIMEFORMAT_SEC:
				sprintf(buff, "%4.2f s", g_CPUTimer.ToSeconds(timeDiff));
				break;

			case TIMEFORMAT_MILLISEC:
				sprintf(buff, "%d ms", g_CPUTimer.ToMilliSeconds(timeDiff));
				break;

			default:
			case TIMEFORMAT_AUTO:
			{
				int mSec = g_CPUTimer.ToMilliSeconds(timeDiff);
				if (mSec != 0)
					sprintf(buff, "%3d ms", mSec);
				else
					sprintf(buff, "%3d æs", g_CPUTimer.ToMicroSeconds(timeDiff));
				break;
			}
		}
	}
}

//*************************************************************************
//*************************************************************************
void CodeTimerPage::FindTimeWidth(int index)
{
	int i;
	int maxDisplayValue = 0;
	int displayValue;
	static int showMap[] =
	{
		0x00,
		TIME_SHOWWORST,
		TIME_SHOWBEST,
		TIME_SHOWAVG
	};

	for (i = 0; i < entries; i++)
	{
		if (((index == TIME_CURRENT) || (timeEntry[i].showTime & showMap[index])) &&
				(timeEntry[i].time[index] >= timeEntry[i].widthMaxTime[index]))
		{
			switch (timeEntry[i].timeFormat)
			{
				case TIMEFORMAT_CLOCKCYCLES:
					displayValue = g_CPUTimer.ToClockCycles(timeEntry[i].time[index]);
					break;
				case TIMEFORMAT_MICROSEC:
					displayValue = g_CPUTimer.ToMicroSeconds(timeEntry[i].time[index]);
					break;
				case TIMEFORMAT_SEC:
					displayValue = (int)(g_CPUTimer.ToSeconds(timeEntry[i].time[index]) * 1000.0f);
					break;
				default:
				case TIMEFORMAT_MILLISEC:
				case TIMEFORMAT_AUTO:
					displayValue = g_CPUTimer.ToMilliSeconds(timeEntry[i].time[index]);
					break;
			}
			if (displayValue > maxDisplayValue)
				maxDisplayValue = displayValue;
		}
	}

	if (maxDisplayValue == 0)
		return;

	timeWidth[index] = 1;
	displayValue = 10;
	while (maxDisplayValue >= 10)
	{
		maxDisplayValue /= 10;
		displayValue *= 10;
		timeWidth[index]++;
	}

	char temp[64];
	for (i = 0; i < entries; i++)
	{
		switch (timeEntry[i].timeFormat)
		{
			case TIMEFORMAT_CLOCKCYCLES:
				timeEntry[i].widthMaxTime[index] = g_CPUTimer.FromClockCycles(displayValue);
				break;
			case TIMEFORMAT_MICROSEC:
				timeEntry[i].widthMaxTime[index] = g_CPUTimer.FromMicroSeconds(displayValue);
				break;
			case TIMEFORMAT_SEC:
				timeEntry[i].widthMaxTime[index] = g_CPUTimer.FromSeconds((float)displayValue * 0.001f);
				break;
			default:
			case TIMEFORMAT_MILLISEC:
			case TIMEFORMAT_AUTO:
				timeEntry[i].widthMaxTime[index] = g_CPUTimer.FromMilliSeconds(displayValue);
				break;
		}

		if ((index == TIME_WORST) && (timeEntry[i].showTime & TIME_SHOWWORST))
		{
			FormatTime(timeEntry[i].time[TIME_WORST], timeEntry[i].timeFormat, temp, timeWidth[TIME_WORST]);
			sprintf(timeEntry[i].worstTimeStr, " (worst %s)", temp);
		}
		else if ((index == TIME_BEST) && (timeEntry[i].showTime & TIME_SHOWBEST))
		{
			FormatTime(timeEntry[i].time[TIME_BEST], timeEntry[i].timeFormat, temp, timeWidth[TIME_BEST]);
			sprintf(timeEntry[i].bestTimeStr, " (best %s)", temp);
		}
	}
}

//*************************************************************************
//*************************************************************************
void CodeTimerPage::FindCallWidth(void)
{
	int i;
	int maxCalls = 0;
	for (i = 0; i < entries; i++)
	{
		if ((timeEntry[i].showTime & TIME_SHOWCALLS) && (timeEntry[i].calls >= timeEntry[i].widthMaxCalls) &&
				(timeEntry[i].calls > maxCalls))
			maxCalls = timeEntry[i].calls;
	}

	if (maxCalls == 0)
		return;

	callsWidth = 1;
	int calls = 10;
	while (maxCalls >= 10)
	{
		maxCalls /= 10;
		calls *= 10;
		callsWidth++;
	}

	for (i = 0; i < entries; i++)
	{
		timeEntry[i].widthMaxCalls = calls;
	}
}

//*************************************************************************
// A protected strncpy
//*************************************************************************
char *CodeTimerPage::strncpy(
char *dest,
char *source,
int maxlen)
{
	char *ret = ::strncpy(dest, source, maxlen);
	dest[maxlen] = '\0';
	return(ret);
}


