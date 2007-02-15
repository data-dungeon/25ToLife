//***************************************************************************
// The code timer allows you to setup multiple pages of timers on the
// auxillary text screen.
//***************************************************************************

/******************* includes ****************************/

#include "profile/ProfilePCH.h"
#include "Layers/Memory.h"





// Make sure we declare the global
CodeTimer g_codeTimer;

// Only compile this code if we are enabled
#ifdef CODETIMER_ON

//***************************************************************************
// Init stuff
//***************************************************************************
CodeTimer::CodeTimer(void) :
#if defined(MEM_USE_WIN32_MANAGER)
pageList(true)
#else
pageList(false)//djr: this should be true - to destruct pages, but this crashes engine memory manager. false will leak memory at program exit, which is cleaned up by OS.
#endif
{
}

//***************************************************************************
// Terminate stuff
//***************************************************************************
CodeTimer::~CodeTimer(void)
{
}

//***************************************************************************
// Register a page of code sections, once you've built a page
// you can't add more timers to it.
//***************************************************************************
TIMERPAGEHANDLE CodeTimer::RegisterSections(
char *pageName,
CODESECTION *sectionList,
int sections)
{
	// Duh?
	if (sectionList == NULL || sections <= 0)
		return(NULL);

	// Allocate a new page
	MEM_SET_ONE_SHOT_NEW_NAME("CodeTimerPage");
	CodeTimerPage *page = new CodeTimerPage(pageName);
	if (!page)
		return(NULL);

   // Register the sections
   for (int i = 0; i < sections; i++)
      page->RegisterSection(sectionList[i].name,
										sectionList[i].format,
										sectionList[i].show,
										sectionList[i].allowCumulative);

   // Setup the reset
  	page->Reset(120, false);

	// Add it into the list
	pageList.AddTail(page);

	// Done!
	return((TIMERPAGEHANDLE)page);
}

//***************************************************************************
// Destroy a page of timers, will crash if you use the handle
// after this call...watch out
//***************************************************************************
void CodeTimer::DestroySections(
TIMERPAGEHANDLE pageHandle)
{
	// Make sure page is valid
	CodeTimerPage *page = (CodeTimerPage *)pageHandle;
	if (!page)
		return;

	// Get rid of the page
	pageList.Unlink(page);
	page->Destroy();
	delete page;
}

//***************************************************************************
// Display the results
//***************************************************************************
void CodeTimer::Display(void)
{
	CodeTimerPage *page = pageList.Head();
	while (page)
	{
		page->ComputeStats();
		page->UpdateScreen();
		page = page->next;
	}
}

#endif
