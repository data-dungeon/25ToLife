//*************************************************************************
// The ultra fast functions
//*************************************************************************

//*************************************************************************
// Force a redraw
//*************************************************************************
inline void TextWindow::Redraw(void)
{
	Border();
}

//*************************************************************************
// Hide the window (but allow it to draw)
//*************************************************************************
inline void TextWindow::Hide(void)
{
	ReallyHide(true);
}

//*************************************************************************
// Enable drawing
//*************************************************************************
inline bool TextWindow::DrawEnable(void)
{
	enabled = textScreen->Enabled();
	return(enabled);
}

//*************************************************************************
// Disable drawing
//*************************************************************************
inline bool TextWindow::DrawDisable(void)
{
	enabled = false;
	return(enabled);
}

//*************************************************************************
// The real write to text memory
//*************************************************************************
inline void TextWindow::Write(
int x,
int y,
char *text,
int len,
unsigned char attr)
{
	unsigned char *writeAt = WriteOffset(x, y);

	for (; len > 0; text++, writeAt += 2, len--)
		PutChar(writeAt, *text, attr);
}

//*************************************************************************
// New Line for TTY
//*************************************************************************
inline bool TextWindow::TTYNewLine(
int left,
int bottomLessOne)
{
	// Reset the x position
	cursorX = left;

	// Do we need to scroll?
	if (cursorY == bottomLessOne)
		return(ScrollUp());

	// One more line
	cursorY++;
	return(true);
}

//*************************************************************************
// Make the text screen the real screen
//*************************************************************************
inline void TextWindow::SetScreenText(void)
{
	screen = text;
	screenStride = 160;
	hidden = false;
}

//*************************************************************************
// Set the screen to the back buffer
//*************************************************************************
inline void TextWindow::SetScreenBackBuffer(void)
{
	screen = backBuffer;
	screenStride = backStride;
	hidden = true;
}

//*************************************************************************
// Compute the offset to write into video memory
//*************************************************************************
inline unsigned char *TextWindow::WriteOffset(
int x,
int y)
{
	// Are we hidden?
	if (hidden)
	{
		x -= currentSize.left;
		y -= currentSize.top;
	}

	// Copute the address
	return(screen + (x << 1) + y * screenStride);
}

