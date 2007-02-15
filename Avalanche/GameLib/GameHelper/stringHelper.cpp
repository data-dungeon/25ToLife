//--------------------------------------------------------------//
// StringHelper.cpp
//
// convert between string formats (unicode, ascii, etc)
//
//--------------------------------------------------------------//

#include "GameHelper/GameHelperPCH.h"
#include "GameHelper/StringHelper.h"

static	char	StringHelper_Buffer[512];

//--------------------------------------------------------------//
void	CStringHelper::UnicodeToAscii(short* Uni, char* Ascii)
{
	while (Uni[0] != 0)
	{
		Ascii[0] = (char)Uni[0];
		Ascii++; Uni++;
	}
	Ascii[0] = 0;
}
//--------------------------------------------------------------//
void	CStringHelper::AsciiToUnicode(char* Ascii, short* Uni)
{
	while (Ascii[0] != 0)
	{
		Uni[0] = (short)Ascii[0];
		Ascii++; Uni++;
	}
	Uni[0] = 0;
}
//--------------------------------------------------------------//
char*		CStringHelper::UnicodeToAscii(short* Uni)
{
	char*	t = StringHelper_Buffer;
	while (Uni[0] != 0)
	{
		t[0] = (char)Uni[0];
		t++; Uni++;
	}
	t[0] = 0;
	return ((char*)StringHelper_Buffer);
}
//--------------------------------------------------------------//
short*	CStringHelper::AsciiToUnicode(char* Ascii)
{
	short*	t = (short*)StringHelper_Buffer;
	while (Ascii[0] != 0)
	{
		t[0] = (short)Ascii[0];
		Ascii++; t++;
	}
	t[0] = 0;
	return ((short*)StringHelper_Buffer);
}
