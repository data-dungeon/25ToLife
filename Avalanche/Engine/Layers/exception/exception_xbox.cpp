//--------------------------------------------------------------//
// Exception_xbox.cpp
//
// Xbox Setup Exception trapping
//
//--------------------------------------------------------------//

#include "Layers/LayersPCH.h"
#include "Layers/exception/exception.h"
#include "Layers/exception/platformexception.h"

#ifndef DISABLE_EXCEPTIONS

#include "Display/DirectX/DirectX.h"

#include "EngineHelper/drawutil.h"
#include "Display/ColorConversions.h"
#include "platform/systemi.h"
#include "platform/gameapp.h"
#include "gamehelper/stringhelper.h"

//--------------------------------------------------------------//

// defines
#define	COLOR_RED     RGBA_TO_COLOR(255,0,0,255)
#define	COLOR_GREEN   RGBA_TO_COLOR(0,255,0,255)
#define	COLOR_BLUE    RGBA_TO_COLOR(0,0,255,255)
#define	COLOR_WHITE   RGBA_TO_COLOR(255,255,255,255)
#define	COLOR_YELLOW  RGBA_TO_COLOR(255,0,255,255)

#define MAX_EXCEPTION_CODES	16

// vars
ControllerInputDriver *pad;

//--------------------------------------------------------------//

const	char*	ExceptionText[MAX_EXCEPTION_CODES] = {

	{"Access Violation"},
	{"Array Bounds Exceeded"},
	{"Breakpoint"},
	{"Float Denormal Operand"},
	{"Float Divide By Zero"},
	{"Float Inexact Result"},
	{"Float Invalid Operation"},
	{"Float Overflow"},
	{"Float Stack Check"},
	{"Float Underflow"},
	{"Illegal Instruction"},
	{"Int Divide By Zero"},
	{"Int Overflow"},
	{"Invalid Disposition"},
	{"Noncontinuable Exception"},
	{"Single Step"},
};

int	ExceptionMapTable[MAX_EXCEPTION_CODES] = { 
	
	EXCEPTION_ACCESS_VIOLATION,
	EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
	EXCEPTION_BREAKPOINT,
	EXCEPTION_FLT_DENORMAL_OPERAND,
	EXCEPTION_FLT_DIVIDE_BY_ZERO,
	EXCEPTION_FLT_INEXACT_RESULT,
	EXCEPTION_FLT_INVALID_OPERATION,
	EXCEPTION_FLT_OVERFLOW,
	EXCEPTION_FLT_STACK_CHECK,
	EXCEPTION_FLT_UNDERFLOW,
	EXCEPTION_ILLEGAL_INSTRUCTION,
	EXCEPTION_INT_DIVIDE_BY_ZERO,
	EXCEPTION_INT_OVERFLOW,
	EXCEPTION_INVALID_DISPOSITION,
	EXCEPTION_NONCONTINUABLE_EXCEPTION,
	EXCEPTION_SINGLE_STEP,
};

//--------------------------------------------------------------//

void PlatformException::Init(void)
{
	// set our custom assert handler
	SetUnhandledExceptionFilter(PlatformException::Filter);

	// get controller
	pad = g_inputPtr->GetUnlockedController( 0);
}

//--------------------------------------------------------------//

const char*	PlatformException::GetExceptionText( int Code )
{
	int	i;

	for (i = 0; i < MAX_EXCEPTION_CODES; i++)
	{
		if (ExceptionMapTable[i] == Code)
			return (ExceptionText[i]);
	}

	return 0;
}

//--------------------------------------------------------------//

LONG WINAPI	PlatformException::Filter(IN struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	const char*	ExceptionString = 0;
	int			continuable = 0;

	int	ExceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
	int	ExceptionFlags = ExceptionInfo->ExceptionRecord->ExceptionFlags;
	void*	ExceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;
	

	// continuable?
	if (ExceptionFlags == 0)
		continuable = 1;
	else
		continuable = 0;

	// get exception text
	ExceptionString = GetExceptionText(ExceptionCode);

	// display the text.. allow option to continue
	while (1)
	{
		LPDIRECT3DSURFACE8 pFrontBuffer;
		DirectX_Device->GetBackBuffer(-1,D3DBACKBUFFER_TYPE_MONO,&pFrontBuffer);
		g_Font.Begin();

		g_Font.SetScaleFactors (2.0f, 2.0f );

		// title
		g_Font.DrawText(200,20,COLOR_RED,(WCHAR*)CStringHelper::AsciiToUnicode("EXCEPTION!"), 0,0);
		g_Font.SetScaleFactors (1.0f, 1.0f );

		// type
		g_Font.DrawText(50,80,COLOR_GREEN,(WCHAR*)CStringHelper::AsciiToUnicode("Exception:"), 0,0);
		g_Font.DrawText(180,80,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode((char*)ExceptionString), 0,0);

		// address
		char	buff[256];
		g_Font.DrawText(50,100,COLOR_GREEN,(WCHAR*)CStringHelper::AsciiToUnicode("Address:"), 0,0);
		sprintf (buff, "0x%x", ExceptionAddress);
		g_Font.DrawText(180, 100,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode( buff), 0,0);

		// read / write 
		if (ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		{
			if (ExceptionInfo->ExceptionRecord->ExceptionInformation[0])
				g_Font.DrawText(340, 100,COLOR_YELLOW,(WCHAR*)CStringHelper::AsciiToUnicode("On Write" ), 0,0);
			else
				g_Font.DrawText(340, 100,COLOR_YELLOW,(WCHAR*)CStringHelper::AsciiToUnicode("On Read" ), 0,0);
		}

		// display registers
		g_Font.SetScaleFactors (2.0f, 2.0f );
		g_Font.DrawText(220, 180,COLOR_RED,(WCHAR*)CStringHelper::AsciiToUnicode("Registers:"), 0,0);

		g_Font.SetScaleFactors (1.0f, 1.0f );

		// 1st line
		sprintf (buff, "Edi: 0x%x", ExceptionInfo->ContextRecord->Edi);
		g_Font.DrawText(50, 240,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(buff), 0,0);

		sprintf (buff, "Esi: 0x%x", ExceptionInfo->ContextRecord->Esi);
		g_Font.DrawText(240, 240,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode( buff), 0,0);

		sprintf (buff, "Ebx: 0x%x", ExceptionInfo->ContextRecord->Ebx);
		g_Font.DrawText(430, 240,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(buff), 0,0);

		// 2nd line
		sprintf (buff, "Edx: 0x%x", ExceptionInfo->ContextRecord->Edx);
		g_Font.DrawText(50, 270,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(buff), 0,0);

		sprintf (buff, "Ecx: 0x%x", ExceptionInfo->ContextRecord->Ecx);
		g_Font.DrawText(240, 270,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(buff), 0,0);

		sprintf (buff, "Eax: 0x%x", ExceptionInfo->ContextRecord->Eax);
		g_Font.DrawText(430, 270,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(buff), 0,0);

		// 3rd line
		sprintf (buff, "Ebp: 0x%x", ExceptionInfo->ContextRecord->Ebp);
 		g_Font.DrawText(50, 300,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(buff), 0,0);

		sprintf (buff, "Eip: 0x%x", ExceptionInfo->ContextRecord->Eip);
		g_Font.DrawText(240, 300,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode( buff), 0,0);

		sprintf (buff, "SegC: 0x%x", ExceptionInfo->ContextRecord->SegCs);
		g_Font.DrawText(430, 300,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(buff), 0,0);

		// 4th line
		sprintf (buff, "Efg: 0x%x", ExceptionInfo->ContextRecord->EFlags);
 		g_Font.DrawText(50, 330,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode( buff), 0,0);

		sprintf (buff, "Esp: 0x%x", ExceptionInfo->ContextRecord->Esp);
		g_Font.DrawText(240, 330,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode(buff), 0,0);

		sprintf (buff, "SegS: 0x%x", ExceptionInfo->ContextRecord->SegSs);
		g_Font.DrawText(430, 330,COLOR_WHITE,(WCHAR*)CStringHelper::AsciiToUnicode( buff), 0,0);

		// try to continue?
// 		g_Font.DrawText(170, 380,COLOR_YELLOW,AsciiToUnicode("Press 'A' to try to continue"), 0,0);
		
		// done
		g_Font.End();
		pFrontBuffer->Release();
		DirectX_Device->Present( NULL, NULL, NULL, NULL );
		DirectX_Device->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
										D3DCOLOR_XRGB( (int)0, (int)0, (int)0 ), 1.0f, 0L );
		Sleep(201);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

//--------------------------------------------------------------//

#endif // DISABLE_EXCEPTIONS
