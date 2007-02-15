/*
**
**  File:   Analyze.h
**  Date:   April 9, 2003
**  By:     Bryant Collard
**  Desc:   A base class used to analyze the time spent in a function.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/profile/Analyze.h $
**  $Revision: 1 $
**      $Date: 4/10/03 11:35a $
**    $Author: Bcollard $
**
*/

#ifndef ANALYZE_H
#define ANALYZE_H

//#define ANALYZE_FUNCTION
#ifdef ANALYZE_FUNCTION

#define ANALYZE_INITIALIZE_SAMPLE(object) (object).InitializeSample()
#define ANALYZE_TERMINATE_SAMPLE(object) (object).TerminateSample()
#define ANALYZE_START_SLOT(object, slot) (object).StartSlot(slot)
#define ANALYZE_STOP_SLOT(object, slot) (object).StopSlot(slot)

#include "profile/cputimer.h"
#include "cmdcon/cmdcon.h"

class CAnalyze
{
  public:
	void InitializeSample(void);
	void TerminateSample(void);
	void StartSlot(int i_slot);
	void StopSlot(int i_slot);

  protected:
	struct SBreakdown
	{
		CPUTIME d_time;
		uint d_count;
	};

	CAnalyze(uint i_numSlot, SBreakdown* i_data, char** i_label);

  private:
	CAnalyze();
	uint d_numSlot;
	SBreakdown* d_data;
	char** d_label;
	int d_slot;
	CPUTIME d_start;
};

/***************************************************************************/
/***************************************************************************/
inline CAnalyze::CAnalyze(uint i_numSlot, SBreakdown* i_data, char** i_label)
{
	d_numSlot = i_numSlot;
	d_data = i_data;
	d_label = i_label;
	d_slot = -1;
}

/***************************************************************************/
/***************************************************************************/
inline void CAnalyze::InitializeSample(void)
{
	for (uint i = 0; i < d_numSlot; i++)
	{
		d_data[i].d_time = (CPUTIME)0;
		d_data[i].d_count = 0;
	}
	d_slot = 0;
	d_start = g_CPUTimer.Get();
}

/***************************************************************************/
/***************************************************************************/
inline void CAnalyze::TerminateSample(void)
{
	d_data[d_slot].d_time += g_CPUTimer.Get() - d_start;
	d_data[d_slot].d_count++;
	d_slot = -1;

	CPUTIME grandTotal = (CPUTIME)0;
	g_console.Echo(" ");
	g_console.Echo("Average   Cnt    Total    Label");
	g_console.Echo("-------   ---   -------   --------------------------------");
	for (uint i = 0; i < d_numSlot; i++)
	{
		if (d_data[i].d_count > 0)
		{
			g_console.Echo("%7d   %3d   %7d   %s",
					g_CPUTimer.ToClockCycles(d_data[i].d_time) / d_data[i].d_count,
					d_data[i].d_count, g_CPUTimer.ToClockCycles(d_data[i].d_time),
					d_label[i]);
		}
		else
		{
			g_console.Echo("%7d   %3d   %7d   %s",
					g_CPUTimer.ToClockCycles(d_data[i].d_time),
					d_data[i].d_count, g_CPUTimer.ToClockCycles(d_data[i].d_time),
					d_label[i]);
		}
		grandTotal += d_data[i].d_time;
	}
	g_console.Echo("                -------");
	g_console.Echo("  Grand Total = %7d", grandTotal);
}

/***************************************************************************/
/***************************************************************************/
inline void CAnalyze::StartSlot(int i_slot)
{
	if (d_slot >= 0)
	{
		d_data[d_slot].d_time += g_CPUTimer.Get() - d_start;
		d_data[d_slot].d_count++;
		d_slot = i_slot;
		d_start = g_CPUTimer.Get();
	}
}

/***************************************************************************/
/***************************************************************************/
inline void CAnalyze::StopSlot(int i_slot)
{
	if (i_slot == d_slot)
	{
		d_data[d_slot].d_time += g_CPUTimer.Get() - d_start;
		d_data[d_slot].d_count++;
		d_slot = 0;
		d_start = g_CPUTimer.Get();
	}
}

#else

#define ANALYZE_INITIALIZE_SAMPLE(object)
#define ANALYZE_TERMINATE_SAMPLE(object)
#define ANALYZE_START_SLOT(object, slot)
#define ANALYZE_STOP_SLOT(object, slot)

#endif

#endif // ANALYZE_H
