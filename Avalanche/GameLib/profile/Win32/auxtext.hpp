//***************************************************************************
// The implementation for the auxillary text window
//***************************************************************************

//***************************************************************************
// Innards for rendering
//***************************************************************************
inline int AuxTextScreen::AssembleLine(
unsigned char *screen,
unsigned char *attr,
char *line,
int maxWidth)
{
	// Get the current attribute
	int len = 0;
	*attr = screen[1];
	do {
		if (screen[0] == '\0')
			line[len++] = ' ';
		else
			line[len++] = screen[0];
		screen += 2;
		}
	while (screen[1] == *attr && len < maxWidth);

	return(len);
}

//***************************************************************************
// Convert an attribute color into a windows color
//***************************************************************************
inline COLORREF AuxTextScreen::AttrColor(
unsigned char attr)
{
	unsigned char r, g, b;

	if (attr & 0x08)
	{
		r = (attr & 0x4) ? 255 : 0;
		g = (attr & 0x2) ? 255 : 0;
		b = (attr & 0x1) ? 255 : 0;
	}
	else
	{
		r = (attr & 0x4) ? 130 : 0;
		g = (attr & 0x2) ? 130 : 0;
		b = (attr & 0x1) ? 130 : 0;
	}

	return(RGB(r, g, b));
}

//***************************************************************************
// Convert a rect in pixels to text (row/column) based rect
//***************************************************************************
inline void AuxTextScreen::PixelRectToTextRect(
LPRECT pixelRect,
LPRECT textRect)
{
	textRect->left = pixelRect->left / fontWidth;
	textRect->top = pixelRect->top / fontHeight;
	textRect->right = (pixelRect->right + fontWidth) / fontWidth;
	textRect->bottom = (pixelRect->bottom + fontHeight) / fontHeight;
}

//***************************************************************************
// Convert a text (row/column) based rect into a pixel rect
//***************************************************************************
inline void AuxTextScreen::TextRectToPixelRect(
LPRECT textRect,
LPRECT pixelRect)
{
	pixelRect->left = textRect->left * fontWidth;
	pixelRect->top = textRect->top * fontHeight;
	pixelRect->right = textRect->right * fontWidth;
	pixelRect->bottom = textRect->bottom * fontHeight;
}


