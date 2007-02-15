//--------------------------------------------------------------//
// DiscError_xbox.cpp
//
// Display message onscreen that disc failed
//
//--------------------------------------------------------------//

#include "Layers/LayersPCH.h"
#include "Layers/DiscErrorHandler/DiscError.h"
#include "Layers/DiscErrorHandler/platformDiscError.h"
#include "GameHelper/StringHelper.h"

#if defined (_XBOX)

#include "Display/DirectX/DirectX.h"

#include "EngineHelper/drawutil.h"
#include "Display/ColorConversions.h"
#include "platform/systemi.h"
#include "platform/gameapp.h"


// defines
#define	COLOR_RED     RGBA_TO_COLOR(255,0,0,255)
#define	COLOR_GREEN   RGBA_TO_COLOR(0,255,0,255)
#define	COLOR_BLUE    RGBA_TO_COLOR(0,0,255,255)
#define	COLOR_WHITE   RGBA_TO_COLOR(255,255,255,255)
#define	COLOR_YELLOW  RGBA_TO_COLOR(255,0,255,255)

char*	PlatformDiscError_ErrorStrings[] = {
	{ "There's a problem with the disc you're using. It may be dirty or damaged." },		// default
	{ "Open File Failed." },																			// debug strings only (disabled on SHIP_IT)
	{ "Read File Failed." },
};

//--------------------------------------------------------------//

void PlatformDiscError::Init(void)
{
}

//--------------------------------------------------------------//
bool	PlatformDiscError::DisplayError(char* Fname, int ErrorType)
{
	#if defined (_XBOX) && defined(CONSUMER_BUILD)
	ErrorType = 0;						// always just show the Default Error Message
	#endif

	// make sure DX is ready
	if (DirectX_Device == NULL)
		return false;

	while (1)
	{
		LPDIRECT3DSURFACE8 pFrontBuffer;
		DirectX_Device->GetBackBuffer(-1,D3DBACKBUFFER_TYPE_MONO,&pFrontBuffer);
		g_Font.Begin();

		g_Font.SetScaleFactors (0.8f, 0.8f );

		// title
		if (ErrorType < DiscErrorHandler::NUM_ERRORCODES)
			g_Font.DrawText(30,200,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(PlatformDiscError_ErrorStrings[ErrorType]), 0,0);	
		else
		{
			g_Font.DrawText(200,20,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(PlatformDiscError_ErrorStrings[ErrorType]), 0,0);	
			char	temp[256];
			sprintf (temp, "Xbox Error Code %d", ErrorType);
			g_Font.DrawText(300,40,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(temp), 0,0);	
		}

		// extra debug stuff ( not on a final build )
		#if !(defined (_XBOX) && defined(CONSUMER_BUILD))
		if (DiscErrorHandler_LastFilename != NULL && Fname == NULL)
			g_Font.DrawText(200,60,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(Fname), 0,0);
		else
		{
			if (DiscErrorHandler_LastFilename != NULL)
				g_Font.DrawText(200,60,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(DiscErrorHandler_LastFilename), 0,0);
		}

		#endif

		// done
		g_Font.End();
		pFrontBuffer->Release();
		DirectX_Device->Present( NULL, NULL, NULL, NULL );
		DirectX_Device->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
										D3DCOLOR_XRGB( (int)0, (int)0, (int)0 ), 1.0f, 0L );
	}

	return true;
}

//--------------------------------------------------------------//
#endif	// _XBOX
