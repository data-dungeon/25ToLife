//*************************************************************************
// A text window
//
// Note that data is always clipped within a window, but windows are not
// clipped onto the screen!
//*************************************************************************
#ifndef TEXTWIN_H
#define TEXTWIN_H

#include <string.h>

#include "profile/textscr.h"
#include "profile/textrect.h"

// Border types
typedef enum
{
	BORDER_NONE = -1,
	BORDER_SINGLE = 0,
	BORDER_DOUBLE = 1,
	BORDER_FANCY = 2
} BORDERTYPE;

// The default tab expansion
#define DEFAULT_TAB_EXPAND		3

// text window class
class TextWindow
{
public:
	// Must steup the whole system with a TextScreen (since it is platform specific!)
	static bool Setup(TextScreen *useTextScreen);

	// constructor/destructor
	TextWindow(int left, int top, int right, int bottom, char *theTitle = NULL, BORDERTYPE border = BORDER_NONE, bool visible = true);
	TextWindow(TextRect &rect, char *theTitle = NULL, BORDERTYPE border = BORDER_NONE, bool visible = true);
	~TextWindow(void);

	// Title stuff
	bool SetTitle(char *theTitle);

	// Methods for outputting data
	bool XYWriteN(int x, int y, char *text, int len, unsigned char attr = ATTR_NORMAL);
	inline bool XYWrite(int x, int y, char *text, unsigned char attr = ATTR_NORMAL)
		{ return(XYWriteN(x, y, text, strlen(text), attr)); }
	bool TTYWrite(char *text, bool wrap = true, unsigned char attr = ATTR_NORMAL);
	inline void SetTabExpand(int newTabExpand)
		{ tabExpand = newTabExpand; }

	// Clear the window
	void Clear(char clearChar = ' ', unsigned char attr = ATTR_NORMAL);

	// Force a redraw
	inline void Redraw(void);

	// Set the cursor position
	bool SetCursor(int x, int y);

	// Get the cursor position
	inline int GetCursorX(void)
		{ return(cursorX); }
	inline int GetCursorY(void)
		{ return(cursorY); }

	// Hide the window (but allow it to draw)
	inline void Hide(void);
	void Unhide(void);
	inline bool Hidden(void)
		{ return(hidden); }

	// Turn on/off drawing
	inline bool DrawEnable(void);
	inline bool DrawDisable(void);
	inline bool DrawEnabled(void)
		{ return(enabled); }

	// Scroll up
	bool ScrollUp(int lines = 1);

	// Scroll down
	bool ScrollDown(int lines = 1);

	// Lock the window from scrolling
	inline void ScrollLock(bool on)
		{ scrollLock = on; }

	// Get the update rectangle
	inline void GetRect(TextRect &rect)
		{ rect = currentSize; rect.right += 1; rect.bottom += 1; }

	// Handy thing to pass up the text screen
	static TextScreen *GetScreen(void)
		{ return(textScreen); }

private:
	// Stuff for the actual text device
	static TextScreen 	*textScreen;
	static unsigned char *text;

	// The look of the windows
	static char 	*borderChar[];

	// Stuff the describes the window
	BORDERTYPE  	borderType;
	char        	title[80];
	int         	titleLen;
	TextRect      	currentSize;

	int         	cursorX, cursorY;
	bool        	scrollLock;
	bool 				enabled;
	int		  		tabExpand;

	unsigned char	*screen;
	int				screenStride;
	bool				hidden;
	unsigned char 	*backBuffer;
	int		  		backStride, backHeight;

	// create the window
	bool Create(int left, int top, int right, int bottom, char *theTitle, BORDERTYPE border, bool visible);

	// Draw the border
	void Border(void);

	// The real write
	inline void Write(int x, int y, char *text, int len, unsigned char attr = ATTR_NORMAL);

	// A macro to put a character with the default video attribute
	void PutChar(unsigned char *addr, char c, char attr = ATTR_NORMAL)
		{ addr[0] = c; addr[1] = attr; }

	// A macro for scrolling with TTY
	inline bool TTYNewLine(int left, int bottomLessOne);

	// Make the text screen the real screen
	void SetScreenText(void);
	void SetScreenBackBuffer(void);

	// Really do the work of a hide
	void ReallyHide(bool saveFront);

	// Compute the offset to write into video memory
	inline unsigned char *WriteOffset(int x, int y);

	// Crazy stuff for the back buffer
	bool MakeBackBuffer(void);

	// Begin/end update
	inline void BeginUpdate(void)
		{ if (textScreen) textScreen->BeginUpdate(); }
	inline void EndUpdate(int left = 0, int top = 0, int right = TEXTSCR_WIDTH - 1, int bottom = TEXTSCR_HEIGHT - 1)
		{ if (textScreen) textScreen->EndUpdate(left, top, right, bottom); }

public:
	// Perhaps this is cheesy, perhaps you could sue me.
	// These need to exist in a list, and it's 1:37 am.
	TextWindow		*prev, *next;
};

// Include the relevant implementation functions
#include "profile/textwin.hpp"

#endif
