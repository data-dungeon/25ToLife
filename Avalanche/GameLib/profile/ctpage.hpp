//***************************************************************************
// Inlines for the code timer schtuff
//***************************************************************************
#include "profile/cputimer.h"

//***************************************************************************
// Start a section of code
//***************************************************************************
inline void CodeTimerPage::SectionStart(
int sectionIndex)
{
	// Save the start time
	timeEntry[sectionIndex].startTime = g_CPUTimer.Get();
}

//***************************************************************************
// End a section of code
//***************************************************************************
inline void CodeTimerPage::SectionEnd(
int sectionIndex)
{
	// Get the end time 
	CPUTIME endTime = g_CPUTimer.Get();

	// Add it into the stats
	if (!timeEntry[sectionIndex].allowCumulative)
		timeEntry[sectionIndex].time[TIME_CURRENT] = (endTime - timeEntry[sectionIndex].startTime);
	else
	{
		timeEntry[sectionIndex].time[TIME_CURRENT] += (endTime - timeEntry[sectionIndex].startTime);
		timeEntry[sectionIndex].calls++;
	}

	timeEntry[sectionIndex].sampleFrame = frameNumber;
}

//***************************************************************************
// New frame
//***************************************************************************
inline void CodeTimerPage::FrameStart(void)
{
	// Clear all the times
	for (int i = 0; i < entries; i++)
	{
		if (timeEntry[i].allowCumulative)
		{
			// Restart this just in case
			timeEntry[i].startTime = g_CPUTimer.Get();
			timeEntry[i].time[TIME_CURRENT] = (CPUTIME)0;
			timeEntry[i].calls = 0;
		}
	}
}

//***************************************************************************
// End of frame
//***************************************************************************
inline void CodeTimerPage::FrameEnd(void)
{
	// Update the frame number
	frameNumber++;

	// Reset stuff?
	if (resetFrame == frameNumber)
	{
		Reset();
		frameNumber = 0;
		if (resetOneShot)
			resetFrame = 0;
	}
}

//***************************************************************************
// Setup a reset for avg/worst/best
//***************************************************************************
inline void CodeTimerPage::Reset(
int frames,
int oneShot)
{
	resetFrame = frames;
	resetOneShot = oneShot;
}

//*************************************************************************
// Reset all the stats
//*************************************************************************
inline void CodeTimerPage::Reset(void)
{
	for (int i = 0; i < entries; i++)
	{
		timeEntry[i].time[TIME_WORST] = 0;
		timeEntry[i].time[TIME_BEST] = (CPUTIME)1000000;
		timeEntry[i].time[TIME_AVG] = 0;
		timeEntry[i].sampleNumber = 0;
		timeEntry[i].sampleFrame = 0;
	}
}

