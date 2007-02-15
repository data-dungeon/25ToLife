//***************************************************************************
// Wrapper functions for the code timer
//***************************************************************************

//***************************************************************************
// Start a section
//***************************************************************************
inline void CodeTimer::Start(
TIMERPAGEHANDLE pageHandle,
int sectionIndex)
{
	CodeTimerPage *page = (CodeTimerPage *)pageHandle;
	if (page)
		page->SectionStart(sectionIndex);
}

//***************************************************************************
// End a section
//***************************************************************************
inline void CodeTimer::End(
TIMERPAGEHANDLE pageHandle,
int sectionIndex)
{
	CodeTimerPage *page = (CodeTimerPage *)pageHandle;
	if (page)
		page->SectionEnd(sectionIndex);
}

//***************************************************************************
// Frame is starting
//***************************************************************************
inline void CodeTimer::FrameStart(void)
{
	CodeTimerPage *page = pageList.Head();
	while (page)
	{
		page->FrameStart();
		page = page->next;
	}
}

//***************************************************************************
// Frame is done
//***************************************************************************
inline void CodeTimer::FrameEnd(void)
{
	CodeTimerPage *page = pageList.Head();
	while (page)
	{
		page->FrameEnd();
		page = page->next;
	}
}

//***************************************************************************
//***************************************************************************
inline void CodeTimer::Reset(TIMERPAGEHANDLE pageHandle, int frames, int oneShot)
{
	CodeTimerPage *page = (CodeTimerPage *)pageHandle;
	if (page)
		page->Reset(frames, oneShot);
}

//***************************************************************************
//***************************************************************************
inline void CodeTimer::EnableWidthMemory(TIMERPAGEHANDLE pageHandle)
{
	CodeTimerPage *page = (CodeTimerPage *)pageHandle;
	if (page)
		page->EnableWidthMemory();
}

//***************************************************************************
//***************************************************************************
inline void CodeTimer::DisableWidthMemory(TIMERPAGEHANDLE pageHandle)
{
	CodeTimerPage *page = (CodeTimerPage *)pageHandle;
	if (page)
		page->DisableWidthMemory();
}
