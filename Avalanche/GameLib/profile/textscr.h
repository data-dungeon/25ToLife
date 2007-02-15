//***************************************************************************
// A set of classes for writing to a text screen.  Relies on some underlying
// mechanism for actually displaying the text.
//***************************************************************************
#ifndef TEXTSCR_H
#define TEXTSCR_H

#include <string.h>

// Here are the sizes we support
#define TEXTSCR_WIDTH	80
#define TEXTSCR_HEIGHT	43

// Here are some pre-defined text attributes

// These are base
#define ATTR_BLUE			0x01
#define ATTR_GREEN		0x02
#define ATTR_RED			0x04
#define ATTR_CYAN			(ATTR_BLUE | ATTR_GREEN)
#define ATTR_MAGENTA		(ATTR_BLUE | ATTR_RED)
#define ATTR_YELLOW		(ATTR_GREEN | ATTR_RED)
#define ATTR_WHITE		(ATTR_BLUE | ATTR_GREEN | ATTR_RED)

// This can be |'ed in
#define ATTR_HILITE     0x08

// Some handy standard ones
#define ATTR_BRIGHT     (ATTR_NORMAL | ATTR_HILITE)
#define ATTR_NORMAL     0x07
#define ATTR_REVERSE    0x70

// Tomfoolery for the compiler
#if defined(WIN32) && !defined(CDECL)
#define CDECL __cdecl
#else
#define CDECL
#endif

class TextScreen
{
public:
	TextScreen(void);
	virtual ~TextScreen(void) {}

	// Is the text screen enabled? (TEXT==Yes in the environment)
	bool Enabled(void)
		{ return(enabled); }

	// Clear the screen
	void Clear(unsigned char clearChar = ' ', unsigned char clearAttr = ATTR_NORMAL);

	// Write to the screen at a given X, Y. X, Y are [0-79] [0-24]
	// respectively
	inline void WriteXY(int x, int y, char *str, unsigned char attr = ATTR_NORMAL)
		{ WriteXYN(x, y, str, strlen(str), attr); }

	// Write N characters to the screen at XY
	void WriteXYN(int x, int y, char *str, int len, unsigned char attr = ATTR_NORMAL);

	//Write a string at X, Y and then clear the next N Characters that follow
	void CleanWriteXY(int x, int y, char *str, short n=10, unsigned char attr = ATTR_NORMAL);

	// Print a formatted string to the screen at XY
	void CDECL PrintXY(int x, int y, char *fmt, ...);

	// Scroll the screen up one line!
	void Scroll(void);

	// If you know what you are doing
	unsigned char *GetAddr(void)
		{ return(textAddr); }

	// Call this before/after you update screen memory
	virtual void BeginUpdate(void) = 0;
	virtual void EndUpdate(int left = 0, int top = 0,
									int right = TEXTSCR_WIDTH - 1, int bottom = TEXTSCR_HEIGHT - 1) = 0;

protected:
	// Is the text screen enabled?
	bool enabled;

	// Address of the text screen, the subclass must somehow provide
	// actual memory for this
	unsigned char *textAddr;
};

#endif
