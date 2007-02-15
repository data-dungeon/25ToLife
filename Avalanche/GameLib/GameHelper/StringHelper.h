//--------------------------------------------------------------//
// StringHelper.h
//
// convert between string formats (unicode, ascii, etc)
//
//--------------------------------------------------------------//

#ifndef _STRINGHELPER_H
#define _STRINGHELPER_H


class CStringHelper
{
public:
	static	void	UnicodeToAscii(short* Uni, char* Ascii);
	static	void	AsciiToUnicode(char* Ascii, short* Uni);

	static	char*		UnicodeToAscii(short* Uni);
	static	short*	AsciiToUnicode(char* Ascii);
};

#endif // _STRINGHELPER_H

