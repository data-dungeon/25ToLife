//***************************************************************************
// Win32 Text screen in a window support
//***************************************************************************

/******************* includes ****************************/

#include "profile/ProfilePCH.h"





// Environment tag for the font
#define TEXT_ENV_TAG    		"TEXTSCREEN"
#define TEXT_ENV_FONTTAG    	"TEXTFONT"
#define TEXT_ENV_TOPMOSTTAG   "TEXTTOPMOST"

//***************************************************************************
// Figure out if we are enabled/exist
//***************************************************************************
TextScreenWin32::TextScreenWin32(
HINSTANCE hInst)
{
	// Start out with no text window
	auxText = NULL;

	// Set this 'cause we know it
	Create(hInst);
}

//***************************************************************************
// Destroy!
//***************************************************************************
TextScreenWin32::~TextScreenWin32(void)
{
	if (auxText)
		delete auxText;
}

//***************************************************************************
// Call this before you update screen memory
//***************************************************************************
void TextScreenWin32::BeginUpdate(void)
{
}

//***************************************************************************
// Call this after you update screen memory
//***************************************************************************
void TextScreenWin32::EndUpdate(
int left,
int top,
int right,
int bottom)
{
	// Tell the auxillary text window to update
	if (auxText)
		auxText->UpdateScreen(left, top, right, bottom);
}

//***************************************************************************
// Create the actual window and set the flags
//***************************************************************************
void TextScreenWin32::Create(
HINSTANCE hInst)
{
	// Do they even have us enabled?
	char *textEnv = getenv(TEXT_ENV_TAG);
	if (!textEnv || strcmpi(textEnv, "off") == 0 || strcmpi(textEnv, "false") == 0)
		return;

	// Do they want topmost?
	char *topMostTag = getenv(TEXT_ENV_TOPMOSTTAG);
	bool topMost = (topMostTag != NULL && (*topMostTag == 'Y' || *topMostTag == 'y'));

	// Do they have a font preference
	char *textFont = getenv(TEXT_ENV_FONTTAG);
	if (!textFont)
		auxText = new AuxTextScreen(hInst, topMost);
	else
	{
		// Set it up with a font
		int fontWidth, fontHeight;
		sscanf(textFont, " %d x %d ", &fontWidth, &fontHeight);
		auxText = new AuxTextScreen(hInst, topMost, fontWidth, fontHeight);
	}

	// Is the object ready?
	if (auxText != NULL)
	{
		// Setup the text address
		textAddr = auxText->GetAddr();

		// We are enabled
		enabled = true;
	}
}

