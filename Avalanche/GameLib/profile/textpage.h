//***************************************************************************
// Pages are collections of windows that cover the entire text screen.
// This class allows you to setup multiple "pages" of multiple windows and
// simply switch between them.
//***************************************************************************
#ifndef TEXTPAGE_H
#define TEXTPAGE_H

#include "profile/textwin.h"
#include "container/dllist.h"

// A page is a linked list of windows
class TextPage
{
public:
	TextPage(void);
	~TextPage(void);

	// Add a window to the page
	void AddWindow(TextWindow *window)
		{ windowList.AddTail(window); }

	// Remove a window from the page
	void DeleteWindow(TextWindow *window)
		{ windowList.Remove(window); }

	// Traverse the windows in the list
	TextWindow *Root(void)
		{ return(windowList.Head()); }

private:
	// Maintain a list of our windows
	DoublyLinkedList<TextWindow> 		windowList;

public:
	// We exist in a doubly linked list
	TextPage									*prev, *next;
};

// The page system (notice everything is static!)
// This maintains a list of pages
class TextPageSystem
{
public:
	// Allocate a new page, if a title is set, a window of the
	// entire page will be created
	static TextPage *CreatePage(void);
	static TextPage *CreatePage(char *theTitle, BORDERTYPE border = BORDER_FANCY);

	// Destroy this page
	static void DestroyPage(TextPage *page);

	// Activate a page directly (not done be default on create)
	static void ActivatePage(TextPage *page);

	// Set the next/prev page
	static void NextPage(void);
	static void PreviousPage(void);

	// Shut down everything (avoid memory leaks)
	static void DestroyAll(void);

private:
	static DoublyLinkedList<TextPage>	pageList;
	static TextPage							*currentPage;

	static void DrawEnable(TextPage *page, bool enableIt);
};

#endif
