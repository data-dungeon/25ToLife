//***************************************************************************
// Here is a class that makes a text screen actually appear as a Microsoft
// Windows window.
//***************************************************************************

/******************* includes ****************************/

#include "profile/ProfilePCH.h"





// Our window class name
const char *AuxTextScreen::myClassName = "AUXTEXTWIN";
int AuxTextScreen::defaultFontWidth = 8;
int AuxTextScreen::defaultFontHeight = 11;

// Registry key for window positon
#define AUXTEXT_REGKEY 	"AuxTextWin"

//***************************************************************************
// Make the window
//***************************************************************************
AuxTextScreen::AuxTextScreen(
HINSTANCE hInst,
bool topMost,
int wantFontWidth,
int wantFontHeight)
{
	// Clear the buffer!
	memset(screenBuffer, '\0', sizeof(screenBuffer));
	nextUpdate = 0;

	// Register the class
	RegisterClass(hInst);

	// Create the window
	hWnd = CreateWindow(myClassName, "Aux Text", WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT, CW_USEDEFAULT,
								CW_USEDEFAULT, CW_USEDEFAULT,
								NULL, NULL, hInst, (LPVOID)this);

	// Create the font
	hFont = CreateFont(hWnd, wantFontWidth, wantFontHeight,
								&fontWidth, &fontHeight);

	// make sure it is the right size
	SizeWindow(hWnd);

	// Do they want it topmost?
	if (topMost)
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	// Load the size
	LoadSize(AUXTEXT_REGKEY, hWnd);

	// Show/update it
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

//***************************************************************************
// Clean up everything
//***************************************************************************
AuxTextScreen::~AuxTextScreen(void)
{
	// Delete the font
	if (hFont)
		DeleteObject((HGDIOBJ)hFont);

	// Destroy the window
	if (hWnd)
		DestroyWindow(hWnd);
}

//***************************************************************************
// Update a portion of the screen
//***************************************************************************
void AuxTextScreen::UpdateScreen(
int left,
int top,
int right,
int bottom)
{
	// Is the window still here?
	if (hWnd == NULL)
		return;

	// Clip
	if (left < 0)
		left = 0;
	if (++right > AUXTEXT_WIDTH)
		right = AUXTEXT_WIDTH;
	if (top < 0)
		top = 0;
	if (++bottom > AUXTEXT_HEIGHT)
		bottom = AUXTEXT_HEIGHT;

	// Assign the rect
	RECT textRect;
	if (left <= right)
	{
		textRect.left = left;
		textRect.right = right;
	}
	else
	{
		textRect.left = right;
		textRect.right = left;
	}
	if (top <= bottom)
	{
		textRect.top = top;
		textRect.bottom = bottom;
	}
	else
	{
		textRect.top = bottom;
		textRect.bottom = top;
	}

	// Do the conversion
	RECT updateRect;
	TextRectToPixelRect(&textRect, &updateRect);

	// Tell windows about it
	InvalidateRect(hWnd, &updateRect, FALSE);

	// Time to really do it?
	if (timeGetTime() >= nextUpdate ||
 		(left == 0 && top == 0 && right == AUXTEXT_WIDTH && bottom == AUXTEXT_HEIGHT))
	{
		// Reset for next time
		nextUpdate = timeGetTime() + AUXTEXT_DELAYMS;

		// Do the real work
		UpdateWindow(hWnd);
	}
}

//***************************************************************************
// The actual windows procedure
//***************************************************************************
LONG FAR AuxTextScreen::WindowProc(
HWND hWnd,
UINT msg,
WPARAM wParam,
LPARAM lParam)
{
	AuxTextScreen *me;

	// Setup the GWL_USER
	if (msg == WM_CREATE)
	{
		// Get the class object pointer
		me = (AuxTextScreen *)(((LPCREATESTRUCT)lParam)->lpCreateParams);

		// Set the pointer to me
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)me);
	}

	// Who are we?
	me = (AuxTextScreen *)GetWindowLong(hWnd, GWL_USERDATA);
	if (me)
	{
		// Dispatch the message
		switch (msg)
		{
			case WM_PAINT:
				// Handle the paint
				return(me->OnPaint());

			case WM_GETMINMAXINFO:
				return(me->OnMinMaxInfo((LPMINMAXINFO)lParam));

			case WM_DESTROY:
				// Save our pos
				SaveSize(me->hWnd, AUXTEXT_REGKEY);

				// Make sure we clear this
				me->hWnd = NULL;
				break;
		}
	}

	// Just do the default thing
	return(::DefWindowProc(hWnd, msg, wParam, lParam));
}

//***************************************************************************
// The paint routine
//***************************************************************************
LONG AuxTextScreen::OnPaint(void)
{
	// Do we have something to do?
	RECT update;
	if (GetUpdateRect(hWnd, &update, FALSE))
	{
		// Start the paint
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);

		// Where do we update?
		RECT textRect;
		PixelRectToTextRect(&update, &textRect);
		TextRectToPixelRect(&textRect, &update);

		// Set the font
		HFONT hOldFont = (HFONT)SelectObject(ps.hdc, hFont);

		// Set the background color (which we don't really support yet)
		SetBkColor(ps.hdc, RGB(0, 0, 0));

		// Let's be careful here
		if (textRect.right > AUXTEXT_WIDTH)
			textRect.right = AUXTEXT_WIDTH;
		if (textRect.bottom > AUXTEXT_HEIGHT)
			textRect.bottom = AUXTEXT_HEIGHT;

		// Figure out the addressing
		unsigned char *scanAddr = &screenBuffer[textRect.top * 160 + (textRect.left << 1)];
		int width = textRect.right - textRect.left;
		int scanSkip = 160 - (width << 1);

		// Do the work (wish we could do this more than one char at a time
		int row, col, x, y;
		int len, widthLeft;
		for (row = textRect.top, y = update.top; row < textRect.bottom; row++, y += fontHeight, scanAddr += scanSkip)
			for (col = textRect.left, x = update.left, widthLeft = width;
					col < textRect.right;
					col += len, x += (len * fontWidth), scanAddr += (len << 1), widthLeft -= len)
			{
				// Assemble the line
				unsigned char attr;
				char line[AUXTEXT_WIDTH];
				len = AssembleLine(scanAddr, &attr, line, widthLeft);

				// Set the color based on attribue
				COLORREF color = AttrColor(attr);
				SetTextColor(ps.hdc, color);

				// Write the text
				TextOut(ps.hdc, x, y, (LPCSTR)line, len);
			}

		// Restore the font/pen
		SelectObject(ps.hdc, hOldFont);

		// We're done
		EndPaint(hWnd, &ps);
	}

	return(0);
}

//*************************************************************************
// Constrain our size for now
//*************************************************************************
int AuxTextScreen::OnMinMaxInfo(
LPMINMAXINFO minMax)
{
	// Make our size
	RECT rect;
	GetWindowRect(&rect);

	// Enforce it!
	minMax->ptMaxSize.x = rect.right - rect.left;
	minMax->ptMaxSize.y = rect.bottom - rect.top;
	minMax->ptMinTrackSize.x = minMax->ptMaxSize.x;
	minMax->ptMinTrackSize.y = minMax->ptMaxSize.y;
	minMax->ptMaxTrackSize.x = minMax->ptMaxSize.x;
	minMax->ptMaxTrackSize.y = minMax->ptMaxSize.y;

	return(0);
}

//*************************************************************************
// Register the class for our window
//*************************************************************************
bool AuxTextScreen::RegisterClass(
HINSTANCE hInst)
{
	// Just register the class
	WNDCLASS wc;
	memset(&wc, '\0', sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = myClassName;

	return(::RegisterClass(&wc) != 0);
}

//*************************************************************************
// Create my font for the window
//*************************************************************************
HFONT AuxTextScreen::CreateFont(
HWND hWnd,
int desiredWidth,
int desiredHeight,
int *realWidth,
int *realHeight)
{
	// Fill out the log font
	LOGFONT textFont;
	memset(&textFont, '\0', sizeof(textFont));
	textFont.lfHeight = -desiredHeight;
	textFont.lfWidth = desiredWidth;
	textFont.lfCharSet = OEM_CHARSET;
	textFont.lfOutPrecision = OUT_DEVICE_PRECIS;
	textFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	textFont.lfQuality = DEFAULT_QUALITY;
	textFont.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;

	// Create it
	HFONT hFont = CreateFontIndirect(&textFont);
	if (hFont == NULL || !GetFontMetrics(hWnd, hFont, realWidth, realHeight))
	{
		// Fake the metrics
		*realWidth = desiredWidth;
		*realHeight = desiredHeight;
	}

	// Give it back to them
	return(hFont);
}

//*************************************************************************
// Get the information about this font
//*************************************************************************
bool AuxTextScreen::GetFontMetrics(
HWND hWnd,
HFONT hFont,
int *realWidth,
int *realHeight)
{
	// Get the DC for the window
	HDC hDC = GetDC(hWnd);
	if (hDC == NULL)
		return(false);

	// Select the font into the DC
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	// Get the text metrics
	TEXTMETRIC textMetrics;
	bool ret = (GetTextMetrics(hDC, &textMetrics) != 0);
	if (ret)
	{
		*realHeight = textMetrics.tmHeight;
		*realWidth = textMetrics.tmAveCharWidth;
	}

	// Restore the font
	SelectObject(hDC, hOldFont);
	ReleaseDC(hWnd, hDC);
	return(ret);
}

//*************************************************************************
// Size the window accordingly
//*************************************************************************
bool AuxTextScreen::SizeWindow(
HWND hWnd)
{
	// Duh?
	if (!hWnd)
		return(false);

	// Mess with the size
	RECT rect;
	GetWindowRect(&rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	SetWindowPos(hWnd, NULL, GetSystemMetrics(SM_CXICON) / 2,
						GetSystemMetrics(SM_CYICON) / 2,
						width, height,
						SWP_NOZORDER | SWP_NOACTIVATE);

	return(true);
}

//*************************************************************************
// Give me the size of the window rect
//*************************************************************************
void AuxTextScreen::GetWindowRect(
LPRECT rect)
{
	::GetWindowRect(hWnd, rect);
	rect->right = rect->left + AUXTEXT_WIDTH * fontWidth;
	rect->bottom = rect->top + AUXTEXT_HEIGHT * fontHeight;
	AdjustWindowRect(rect, GetWindowLong(hWnd, GWL_STYLE), GetMenu(hWnd) != NULL);
}

/***************************************************************************/
// Sizing stuff
/***************************************************************************/
bool AuxTextScreen::LoadSize(
const char *registryName,
HWND hWnd)
{
	RECT rect;

	// Load the rect
	if (!ReadProfileInt(registryName, "top", &rect.top) ||
			!ReadProfileInt(registryName, "left", &rect.left) ||
			!ReadProfileInt(registryName, "right", &rect.right) ||
			!ReadProfileInt(registryName, "bottom", &rect.bottom))
		return(false);

   int width = rect.right - rect.left;
   int height = rect.bottom - rect.top;
   SetWindowPos(hWnd, NULL,
						rect.left, rect.top,
						width, height,
						SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool AuxTextScreen::SaveSize(
HWND hWnd,
const char *registryName)
{
	RECT rect;

	::GetWindowRect(hWnd, &rect);

	// Save these (should use reg someday)
	WriteProfileInt(registryName, "top", rect.top);
	WriteProfileInt(registryName, "left", rect.left);
	WriteProfileInt(registryName, "right", rect.right);
	WriteProfileInt(registryName, "bottom", rect.bottom);

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool AuxTextScreen::WriteProfileInt(
const char *section,
const char *var,
long value)
{
	char temp[64];
	return(WriteProfileString(section, var, itoa(value, temp, 10)) != 0);
}

/***************************************************************************/
/***************************************************************************/
bool AuxTextScreen::ReadProfileInt(
const char *section,
const char *var,
long *value)
{
	char temp[64];
	if (GetProfileString(section, var, "", temp, sizeof(temp)) <= 0)
		return(false);

	*value = atoi(temp);
	return(true);
}



