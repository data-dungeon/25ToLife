//***************************************************************************
// The code timer allows you to setup multiple pages of timers on the
// auxillary text screen.
//***************************************************************************
#ifndef CODETIMER_H
#define CODETIMER_H

#include "container/dllist.h"
#include "profile/ctpage.h"

// Turn on timing if debug is defined
#ifdef _DEBUG
#define CODETIMER_ON
#endif

// Sections are given in this structure
struct CODESECTION
{
   char           *name;				// name
   TIMEFORMAT     format; 				// e.g. TIMEFORMAT_MILLISEC
   int            show;             // e.g. TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST
	bool				allowCumulative;	// Allow muliple calls per frame
};

// When you register a page, you get back a handle
typedef void *TIMERPAGEHANDLE;

// Stub everything out if we are disabled
#ifndef CODETIMER_ON
class CodeTimer
{
public:
	// Init/terminate
	inline CodeTimer(void) {}
	inline ~CodeTimer(void) {}

	// Register a page of code sections, once you've built a page
	// you can't add more timers to it.
	inline TIMERPAGEHANDLE RegisterSections(char *pageName, CODESECTION *sectionList, int sections)
		{ return(0); }
	inline void DestroySections(TIMERPAGEHANDLE pageHandle) {}

	// Start/end a code section
	inline void Start(TIMERPAGEHANDLE pageHandle, int sectionIndex) {}
	inline void End(TIMERPAGEHANDLE pageHandle, int sectionIndex) {}

	// Call these at the start/end of each frame
	inline void FrameStart(void) {}
	inline void FrameEnd(void) {}

	// Call to update the display
	inline void Display(void) {}

	// Calls to operate on pages
	inline void Reset(TIMERPAGEHANDLE pageHandle, int frames, int oneShot) {}
	inline void EnableWidthMemory(TIMERPAGEHANDLE pageHandle) {}
	inline void DisableWidthMemory(TIMERPAGEHANDLE pageHandle) {}

};
#else
// We live!
class CodeTimer
{
public:
	// Init/terminate
	CodeTimer(void);
	~CodeTimer(void);

	// Register a page of code sections, once you've built a page
	// you can't add more timers to it.
	TIMERPAGEHANDLE RegisterSections(char *pageName, CODESECTION *sectionList, int sections);

	// Destroy a page of timers, will crash if you use the handle
	// after this call...watch out
	void DestroySections(TIMERPAGEHANDLE pageHandle);

	// Start/end a code section
	inline void Start(TIMERPAGEHANDLE pageHandle, int sectionIndex);
	inline void End(TIMERPAGEHANDLE pageHandle, int sectionIndex);

	// Call these at the start/end of each frame
	inline void FrameStart(void);
	inline void FrameEnd(void);

	// Call to update the display
	void Display(void);

	// Calls to operate on pages
	inline void Reset(TIMERPAGEHANDLE pageHandle, int frames, int oneShot);
	inline void EnableWidthMemory(TIMERPAGEHANDLE pageHandle);
	inline void DisableWidthMemory(TIMERPAGEHANDLE pageHandle);

private:
	// Our list of pages
	DoublyLinkedList<CodeTimerPage> pageList;
};

// Inlines
#include "profile/codetimer.hpp"

#endif

// Make a global object available
extern CodeTimer g_codeTimer;

#endif
