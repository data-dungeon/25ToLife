//***************************************************************************
// Pages are collections of windows that cover the entire text screen.
// This class allows you to setup multiple "pages" of multiple windows and
// simply switch between them.
//***************************************************************************

/******************* includes ****************************/

#include "profile/ProfilePCH.h"
#include "Layers/Memory.h"





// Statics for the page system
#if defined(MEM_USE_WIN32_MANAGER)
DoublyLinkedList<TextPage> TextPageSystem::pageList;
#else
DoublyLinkedList<TextPage> TextPageSystem::pageList(false);//djr: this should be true - to destruct pages, but this crashes engine memory manager. false will leak memory at program exit, which is cleaned up by OS.
#endif
TextPage *TextPageSystem::currentPage = NULL;

//***************************************************************************
//***************************************************************************
TextPage::TextPage(void) : 
windowList(true) 
{
}

//***************************************************************************
//***************************************************************************
TextPage::~TextPage(void)
{
}

//***************************************************************************
// Make a new window on a new page
//***************************************************************************
TextPage *TextPageSystem::CreatePage(void)
{
	TextPage *page = new TextPage;
	if (page)
	{
		pageList.AddTail(page);
		if (!currentPage)
			ActivatePage(page);
	}
	return(page);
}

//***************************************************************************
// Allocate me a new window (use the page to ovverride the page)
//***************************************************************************
TextPage *TextPageSystem::CreatePage(
char *theTitle,
BORDERTYPE border)
{
	// Make the window
	MEM_SET_ONE_SHOT_NEW_NAME("TextWindow");
	TextWindow *window = new TextWindow(0, 0, TEXTSCR_WIDTH - 1, TEXTSCR_HEIGHT - 1, theTitle, border, false);
	if (window == NULL)
		return(NULL);

	// Make ourselves a page entry
	MEM_SET_ONE_SHOT_NEW_NAME("TextPage");
	TextPage *page = new TextPage;
	if (page == NULL)
	{
		delete window;
		return(NULL);
	}

	// Add the window to the page
	page->AddWindow(window);

	// Add the page to our list
	pageList.AddHead(page);

	// If we don't have an active page, make it active.
	if (!currentPage)
		ActivatePage(page);
	else
		DrawEnable(page, false);

	return(page);
}

//***************************************************************************
// Destroy this window
//***************************************************************************
void TextPageSystem::DestroyPage(
TextPage *page)
{
	// Did they give us a page
	if (!page)
		return;

	// Make sure the page is not active
	if (page == currentPage)
		NextPage();

	// Kill it!
	pageList.Remove(page);
}

//***************************************************************************
// Really change the displayed page
//***************************************************************************
void TextPageSystem::ActivatePage(
TextPage *page)
{
	// Make sure we aren't doing extra crap
	if (page == currentPage)
		return;

	// Turn off drawing on the old page
	DrawEnable(currentPage, false);

	// Clear the screen
	TextScreen *screen = TextWindow::GetScreen();
	if (screen)
		screen->Clear('²');

	// Turn on drawing for the new page
	currentPage = page;
	DrawEnable(currentPage, true);
}

//***************************************************************************
// Go to the next page
//***************************************************************************
void TextPageSystem::NextPage(void)
{
 	TextPage *activate = currentPage;
	if (activate)
		activate = activate->next;

	// Did we wrap?
	if (!activate)
		activate = pageList.Head();

 	ActivatePage(activate);
}

//***************************************************************************
// Go to the previous page
//***************************************************************************
void TextPageSystem::PreviousPage(void)
{
 	TextPage *activate = currentPage;
	if (activate)
		activate = activate->prev;

	// Did we wrap?
	if (!activate)
		activate = pageList.Tail();

 	ActivatePage(activate);
}

//***************************************************************************
// Set the enables for all the windows on this page
//***************************************************************************
void TextPageSystem::DrawEnable(
TextPage *page,
bool enableIt)
{
	// Duh?
	if (!page)
		return;

	// Loop through the windows
	TextWindow *window = page->Root();
	while (window)
	{
		// set the enable
		if (enableIt)
			window->Unhide();
		else
			window->Hide();

		// See what is next
		window = window->next;
	}
}
