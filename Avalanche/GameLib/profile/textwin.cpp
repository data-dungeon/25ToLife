//*************************************************************************
// A text window API
//*************************************************************************

/******************* includes ****************************/

#include "profile/ProfilePCH.h"





// Statics
TextScreen *TextWindow::textScreen = NULL;
unsigned char *TextWindow::text = NULL;

// Character defines
#define CHAR_UPPERLEFT     0
#define CHAR_TOP           1
#define CHAR_UPPERRIGHT    2
#define CHAR_LEFT          3
#define CHAR_RIGHT         4
#define CHAR_BOTTOMLEFT    5
#define CHAR_BOTTOM        6
#define CHAR_BOTTOMRIGHT   7
#define CHAR_TITLELEFT     8
#define CHAR_TITLERIGHT    9

// The look of the windows
char *TextWindow::borderChar[] =
{
	{ "ÚÄ¿³³ÀÄÙ´Ã" },
	{ "ÉÍ»ººÈÍ¼µÆ" },
	{ "ÕÍ¸³³ÀÄÙµÆ" }
};

//*************************************************************************
// Must steup the whole system with a TextScreen (since it is platform
// specific!)
//*************************************************************************
bool TextWindow::Setup(
TextScreen *useTextScreen)
{
	if (useTextScreen)
	{
		textScreen = useTextScreen;
		textScreen->Clear();
		text = textScreen->GetAddr();
	}

	return(useTextScreen != NULL);
}

//*************************************************************************
// Instantiate a text window
//*************************************************************************
TextWindow::TextWindow(
int left,
int top,
int right,
int bottom,
char *theTitle,
BORDERTYPE border,
bool visible)
{
	Create(left, top, right, bottom, theTitle, border, visible);
}

//*************************************************************************
//*************************************************************************
TextWindow::TextWindow(
TextRect &rect,
char *theTitle,
BORDERTYPE border,
bool visible)
{
	Create(rect.left, rect.top, rect.right, rect.bottom, theTitle, border, visible);
}

//*************************************************************************
// Free the back buffer
//*************************************************************************
TextWindow::~TextWindow(void)
{
	delete [] backBuffer;
}

//*************************************************************************
// Set the title
//*************************************************************************
bool TextWindow::SetTitle(
char *theTitle)
{
	// Set the title string
	if (theTitle == NULL || strlen(theTitle) == 0)
		titleLen = 0;
	else
	{
		// Save off the title
		strncpy(title, theTitle, sizeof(title) - 1);
		title[sizeof(title) - 1] = '\0';
		titleLen = strlen(theTitle);
		Border();
	}

	return(true);
}

//*************************************************************************
// Outputting data
//*************************************************************************
bool TextWindow::XYWriteN(
int x,
int y,
char *text,
int len,
unsigned char attr)
{
	// Are we allowed to write?
	if (!enabled)
		return(false);

	// Put the coordindates into screen space
	x += currentSize.left;
	y += currentSize.top;

	// Is it even in the window?
	if (x > currentSize.right || (x + len) < currentSize.left ||
			y >= currentSize.bottom || y < currentSize.top)
		return(false);

	// Clip to the left hand side of the window
	int clip = currentSize.left - x;
	if (clip > 0)
	{
		text += clip;
		x += clip;
		len -= clip;
	}

	// Clip to the right hand side of the window
	clip = (x + len) - currentSize.right;
	if (clip > 0)
		len -= clip;

	// Start the paint
	BeginUpdate();

	// Write the data
	Write(x, y, text, len, attr);

	// End the paint
	EndUpdate(x, y, x + len, y);

	return(true);
}

//*************************************************************************
// Write TTY style
//*************************************************************************
bool TextWindow::TTYWrite(
char *text,
bool wrap,
unsigned char attr)
{
	// Are we allowed to write?
	if (!enabled)
		return(false);

	// Setup stuff
	int left = currentSize.left;
	int right = currentSize.right;
	int bottom = currentSize.bottom - 1;

	// Start the paint
	BeginUpdate();

	int startX = cursorX;
	int startY = cursorY;

	while (*text)
	{
		// We support \r and  \n
		if (*text == '\r')
			cursorX = left;
		else if (*text == '\n')
		{
			// Do a new line
			if (!TTYNewLine(left, bottom))
			break;
		}
		else if (*text == '\t')
		{
			int space = 0;

			// Expand the tab
			while (space++ < tabExpand && cursorX < right)
			{
				Write(cursorX, cursorY, " ", 1, attr);
				if (++cursorX == right && wrap)
				{
					if (!TTYNewLine(left, bottom))
						break;
				}
			}
		}
		else if (cursorX < right)
		{
			// Just write the data
			Write(cursorX, cursorY, text, 1, attr);
			if (++cursorX == right && wrap)
			{
				if (!TTYNewLine(left, bottom))
					break;
			}
		}

		// The text thingie
		text++;
	}

	// Done with the paint
	if (startY == cursorY)
		EndUpdate(startX, startY, cursorX, cursorY);
	else
		EndUpdate(0, startY, TEXTSCR_WIDTH, cursorY);

	return(true);
}

//*************************************************************************
// Clear the window region
//*************************************************************************
void TextWindow::Clear(
char clearChar,
unsigned char attr)
{
	// Are we allowed to write?
	if (!enabled)
		return;

	// Setup stuff
	int x = currentSize.left;
	int y = currentSize.top;
	int stride = screenStride - (currentSize.Width() << 1);
	unsigned char *writeAt = WriteOffset(x, y);

	// Start the paint
	BeginUpdate();

	// Clear the window
	for (; y < currentSize.bottom; y++, writeAt += stride)
		for (x = 0; x < currentSize.Width(); x++, writeAt += 2)
			PutChar(writeAt, clearChar, attr);

	// Done with the paint
	EndUpdate();

	// Reset the cursor position
	cursorX = currentSize.left;
	cursorY = currentSize.top;
}

//*************************************************************************
// Set the cursor position
//*************************************************************************
inline bool TextWindow::SetCursor(
int x,
int y)
{
	// Clip & transform the cursor position!
	cursorX = currentSize.left + max(min(x, currentSize.Width()), 0);
	cursorY = currentSize.top + max(min(y, currentSize.Height()), 0);

	return(true);
}

//*************************************************************************
//*************************************************************************
void TextWindow::Unhide(void)
{
	// Are we allowed to write?
	if (!enabled)
		return;

	if (!hidden)
		return;

	// Start the paint
	BeginUpdate();

	// Do the copy
	unsigned char *src = backBuffer;
	unsigned char *dst = text + (currentSize.left << 1) + currentSize.top * 160;
	for (int y = 0; y < backHeight; y++, dst += 160, src += backStride)
		memcpy(dst, src, backStride);

	// Make sure the border is there
	SetScreenText();
	Border();

	// End the paint
	EndUpdate();
}

//*************************************************************************
// Scroll up
//*************************************************************************
bool TextWindow::ScrollUp(
int lines)
{
	// Are we allowed to write?
	if (!enabled)
		return(false);

	// Is scrolling locked?
	if (scrollLock)
		return(false);

	// setup stuff
	int x = currentSize.left;
	int y = currentSize.top;
	int bottom = currentSize.bottom - lines;
	int width = (currentSize.Width() << 1);
	int stride = screenStride * lines;
	unsigned char *writeAt = WriteOffset(x, y);

	// Start the paint
	BeginUpdate();

	// Are they scolling the screen off?
	if (bottom <= currentSize.top)
		Clear();
	else
	{
		// Scroll up
		unsigned char *next;
		for (; y < bottom; y++, writeAt = next)
			memcpy(writeAt, (next = (writeAt + stride)), width);

		// Erase
		for (; y < currentSize.bottom; y++)
			for (; x < currentSize.right; x++, writeAt += 2)
				PutChar(writeAt, ' ');
	}

	// Done with the paint
	EndUpdate();

	return(true);
}

//*************************************************************************
// Scroll down
//*************************************************************************
bool TextWindow::ScrollDown(
int lines)
{
	// Are we allowed to write?
	if (!enabled)
		return(true);

	// Is scrolling locked?
	if (scrollLock)
		return(false);

	// setup stuff
	int x = currentSize.left;
	int y = currentSize.bottom - 1;
	int top = currentSize.top - 1 + lines;
	int width = (currentSize.Width() << 1);
	int stride = screenStride * lines;
	unsigned char *writeAt = WriteOffset(x, y);

	// Start the paint
	BeginUpdate();

	// Are they scolling the screen off?
	if (top >= currentSize.bottom)
		Clear();
	else
	{
		// Scroll up
		unsigned char *next;
		for (; y > top; y--, writeAt = next)
			memcpy(writeAt, (next = (writeAt - stride)), width);

		// Erase
		for (; y >= currentSize.top; y--)
			for (; x < currentSize.right; x++, writeAt += 2)
				PutChar(writeAt, ' ');
	}

	// Done with the paint
	EndUpdate();

	return(true);
}

//*************************************************************************
// create the window
//*************************************************************************
bool TextWindow::Create(
int left,
int top,
int right,
int bottom,
char *theTitle,
BORDERTYPE border,
bool visible)
{
	// Assume the worst
	hidden = true;	
	enabled = false;	
	backBuffer = NULL;

	// Have we be created?
	if (textScreen == NULL || text == NULL)
		return(false);

	// setup everything
	borderType = border;
	currentSize.Set(left, top, right + 1, bottom + 1);
	if (borderType != BORDER_NONE)
		currentSize.Inflate(-1, -1);

	// Allocate the back buffer
	MakeBackBuffer();

	// Allow drawing
	DrawEnable();

	// Start out hidden
	hidden = false;
	ReallyHide(false);
	SetTitle(theTitle);
	Clear();

	// Are we visible?
	if (visible)
		Unhide();

	// No scroll lock
	scrollLock = false;

	// Setup tab expansion
	tabExpand = DEFAULT_TAB_EXPAND;

	return(true);
}

//*************************************************************************
// Draw the border
//*************************************************************************
inline void TextWindow::Border(void)
{
	// Are we allowed to write?
	if (!enabled || hidden)
		return;

	// Setup stuff
	unsigned char *writeAt = screen + ((currentSize.left - 1) << 1) + (currentSize.top - 1) * 160;
	int stride = screenStride - ((currentSize.Width() + 2) << 1);
	int width = currentSize.Width();
	int maxTitleWidth = width - 4;

	// Is there a border?
	if (borderType == BORDER_NONE)
		return;

	// Start the paint
	BeginUpdate();

	// Write the top part
	PutChar(writeAt, borderChar[borderType][CHAR_UPPERLEFT], ATTR_BRIGHT);
	writeAt += 2;

	// Is there a title?
	if (titleLen > 0 && maxTitleWidth > 0)
	{
		PutChar(writeAt, borderChar[borderType][CHAR_TITLELEFT], ATTR_BRIGHT);
		writeAt += 2;
		PutChar(writeAt, ' ', ATTR_BRIGHT);
		writeAt += 2;

		// Write the title
		int len;
		if (titleLen < maxTitleWidth)
			len = titleLen;
		else
			len = maxTitleWidth;

		int x;
		for (x = 0; x < len; x++, writeAt += 2)
			PutChar(writeAt, title[x], ATTR_BRIGHT);

		PutChar(writeAt, ' ', ATTR_BRIGHT);
		writeAt += 2;
		PutChar(writeAt, borderChar[borderType][CHAR_TITLERIGHT], ATTR_BRIGHT);
		writeAt += 2;

		// Finish off the line
		len = maxTitleWidth - titleLen;
		for (x = 0; x < len; x++, writeAt += 2)
			PutChar(writeAt, borderChar[borderType][CHAR_TOP], ATTR_BRIGHT);
	}
	else
	{
		// There is no title
		for (int x = 0; x < width; x++, writeAt += 2)
			PutChar(writeAt, borderChar[borderType][CHAR_TOP], ATTR_BRIGHT);
	}

	// Write the right top part
	PutChar(writeAt, borderChar[borderType][CHAR_UPPERRIGHT], ATTR_BRIGHT);
	writeAt += 2;
	writeAt += stride;

	// Write the border sides
	for (int y = 0; y < currentSize.Height(); y++, writeAt += stride)
	{
		PutChar(writeAt, borderChar[borderType][CHAR_LEFT], ATTR_BRIGHT);
		writeAt += ((width + 1) << 1);
		PutChar(writeAt, borderChar[borderType][CHAR_RIGHT], ATTR_BRIGHT);
		writeAt += 2;
	}

	// Write the bottom
	PutChar(writeAt, borderChar[borderType][CHAR_BOTTOMLEFT], ATTR_BRIGHT);
	writeAt += 2;
	for (int x = 0; x < width; x++, writeAt += 2)
		PutChar(writeAt, borderChar[borderType][CHAR_BOTTOM], ATTR_BRIGHT);
	PutChar(writeAt, borderChar[borderType][CHAR_BOTTOMRIGHT], ATTR_BRIGHT);

	// End the paint
	EndUpdate();
}

//*************************************************************************
// Save the screen to the back buffer if flag set
//*************************************************************************
void TextWindow::ReallyHide(
bool saveFront)
{
	// Already hidden?
	if (hidden)
		return;

	// Copy the screen into the back buffer?
	if (saveFront)
	{
		unsigned char *dst = backBuffer;
		unsigned char *src = text + (currentSize.left << 1) + currentSize.top * 160;
		for (int y = 0; y < backHeight; y++, src += 160, dst += backStride)
			memcpy(dst, src, backStride);
	}

	// Setup the address correctly
	SetScreenBackBuffer();
}

//*************************************************************************
//*************************************************************************
bool TextWindow::MakeBackBuffer(void)
{
	// Free the old one
	if (backBuffer)
	{
		delete [] backBuffer;
		backBuffer = NULL;
	}

	// Make the buffer
	backStride = (currentSize.right - currentSize.left) * 2;
	backHeight = currentSize.bottom - currentSize.top;
	int size = backStride * backHeight;
	backBuffer = new unsigned char[size];

	// Did it work?
	if (backBuffer == NULL)
		return(false);

	// Clear it
	memset(backBuffer, '\0', size);
	return(true);
}
