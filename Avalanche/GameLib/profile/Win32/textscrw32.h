//***************************************************************************
// A win32 inplementaiton of the text screen shctuff
//***************************************************************************
#ifndef TEXTSCRW32_H
#define TEXTSCRW32_H

#include "../textscr.h"
#include "auxtext.h"

//***************************************************************************
// The WIN32 version of the text window (still will support writing directly
// to the text screen)
//***************************************************************************
class TextScreenWin32 : public TextScreen
{
public:
	// Our constructor destructors
	TextScreenWin32(HINSTANCE hInst);
	~TextScreenWin32(void);

	// Call this before/after you update screen memory
	void BeginUpdate(void);
	virtual void EndUpdate(int left = 0, int top = 0,
									int right = TEXTSCR_WIDTH - 1, int bottom = TEXTSCR_HEIGHT - 1);

private:
	// Stuff for supporting the WIN32 window stuff
	AuxTextScreen	*auxText;

	// Create the actual window and set the flags
	void Create(HINSTANCE hInst);
};

#endif
