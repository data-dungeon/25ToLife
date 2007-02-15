//***************************************************************************
// Here is a class that makes a text screen actually appear as a Microsoft
// Windows window.
//***************************************************************************
#ifndef AUXTEXT_H
#define AUXTEXT_H

#define WIN32_LEAN_AND_MEAN
#ifdef _XBOX
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#else
#include <windows.h>
#endif

// The width and height of the screen
#define AUXTEXT_WIDTH		80
#define AUXTEXT_HEIGHT		43

// Time to automatically block between updates
#define AUXTEXT_DELAYMS		33

class AuxTextScreen
{
public:
	// The constructor/destructors
	AuxTextScreen(HINSTANCE hInst, bool topMost = false, int wantFontWidth = defaultFontWidth, int wantFontHeight = defaultFontHeight);
	~AuxTextScreen(void);

	// If you know what you are doing you can get at this
	inline unsigned char *GetAddr(void)
		{ return(screenBuffer); }

	// Does the window exist?
	inline bool Exist(void)
		{ return(hWnd != NULL); }

	// Update the whole screen from the buffer memory
	inline void UpdateScreen(void)
		{ UpdateScreen(0, 0, AUXTEXT_WIDTH - 1, AUXTEXT_HEIGHT - 1); }

	// Update a portion of the screen
	inline void UpdateScreen(RECT rect)
		{ UpdateScreen(rect.left, rect.top, rect.right, rect.bottom); }
	void UpdateScreen(int left, int top, int right, int bottom);

private:
	unsigned char screenBuffer[AUXTEXT_WIDTH * AUXTEXT_HEIGHT * 2];	// The actual screen
	HWND	hWnd;											// Our window
	HFONT hFont;										// Our font
	int 	fontWidth, fontHeight;					// Font width and height
	DWORD nextUpdate;									// Next time to really update

	static const char *myClassName;				// My window class
	static int defaultFontWidth;					// Default font width and height
	static int defaultFontHeight;

	// The actual windows procedure
	static LONG FAR WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Handle the paint
	LONG OnPaint(void);
	int OnMinMaxInfo(LPMINMAXINFO minMax);

	// Register our windows class
	bool RegisterClass(HINSTANCE hInst);

	// Create our font
	static HFONT CreateFont(HWND hWnd, int desiredWidth, int desiredHeight, int *realWidth, int *realHeight);
	static bool GetFontMetrics(HWND hWnd, HFONT hFont, int *realWidth, int *realHeight);

	// Handle the sizing of the window
	bool SizeWindow(HWND hWnd);
	void GetWindowRect(LPRECT rect);

	// Innards for rendering
	inline int AssembleLine(unsigned char *screen, unsigned char *attr, char *line, int maxWidth);
	inline COLORREF AttrColor(unsigned char attr);

	// Rect conversion stuff
	inline void PixelRectToTextRect(LPRECT pixelRect, LPRECT textRect);
	inline void TextRectToPixelRect(LPRECT textRect, LPRECT pixelRect);

	// Saving and restoring of size
	static bool LoadSize(const char *registryName, HWND hWnd);
	static bool SaveSize(HWND hWnd, const char *registryName);

	static bool WriteProfileInt(const char *section, const char *var, long value);
	static bool ReadProfileInt(const char *section, const char *var, long *value);
};

// Implementation spoken in here
#include "auxtext.hpp"

#endif
