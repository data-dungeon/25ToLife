//***************************************************************************
// The implementation of the text screen stuff
//***************************************************************************

/******************* includes ****************************/

#include "profile/ProfilePCH.h"





// Local defines
#define ROW_SIZE        (TEXTSCR_WIDTH * 2)
#define SCREEN_SIZE     (TEXTSCR_HEIGHT * ROW_SIZE)

//***************************************************************************
// Figure out if we are enabled/exist
//***************************************************************************
TextScreen::TextScreen(void)
{
	enabled = false;
	textAddr = NULL;
}

//***************************************************************************
// Clear the screen
//***************************************************************************
void TextScreen::Clear(
unsigned char clearChar,
unsigned char clearAttr)
{
	// Don't do anything if we aren't enabled
	if (!enabled)
		return;

	// Loop through and do it!
	for (int i = 0; i < SCREEN_SIZE; i += 2)
	{
		textAddr[i] = clearChar;
		textAddr[i + 1] = clearAttr;
	}
}

//***************************************************************************
// Write N characters to the screen at XY
//***************************************************************************
void TextScreen::WriteXYN(
int x,
int y,
char *str,
int len,
unsigned char attr)
{
	// Don't do anything if we aren't enabled
	if (!enabled)
		return;

	// Figure out the address
	unsigned char *addr = textAddr + x * 2 + y * ROW_SIZE;

	// Loop through and write it!
	for (int i = 0; i < len; i++)
	{
		*addr++ = str[i];
		*addr++ = attr;
	}
}

//***************************************************************************
//***************************************************************************
void CDECL TextScreen::PrintXY(
int x,
int y,
char *fmt,
...)
{
	char buff[256 + 1];

	va_list args;
	va_start(args, fmt);
	vsprintf(buff, fmt, args);
	va_end(args);

	WriteXY(x, y, buff);
}

//***************************************************************************
//***************************************************************************
void TextScreen::CleanWriteXY(
int x,
int y,
char *str,
short n,
unsigned char attr)
{
	// Don't do anything if we aren't enabled
	if (!enabled)
		return;

	// Figure out the address
	unsigned char *addr = textAddr + x * 2 + y * ROW_SIZE;

	// Loop through and write it!
	int i, len = strlen(str);
	for (i = 0; i < len; i++)
	{
		*addr++ = str[i];
		*addr++ = attr;
	}

	// write blank spaces
	for (i = 0; i < n; i++)
	{
		*addr++ = ' ';
		*addr++ = attr;
	}
}

//*************************************************************************
// Scroll the screen up one line!
//*************************************************************************
void TextScreen::Scroll(void)
{
	// Don't do anything if we aren't enabled
	if (!enabled)
		return;

	// move the screen up
	int off;
	long *copy = (long *)textAddr;
	for (off = 0; off < (SCREEN_SIZE - ROW_SIZE) / 4; off++)
		copy[off] = copy[off + ROW_SIZE / 4];

	// clear the last line
	for (off = SCREEN_SIZE - ROW_SIZE; off < SCREEN_SIZE; off += 2)
		textAddr[off] = ' ';
}


