////////////////////////////////////////////////////////////////////////////
//
// TokenizeString
//
// Class build an array of string that are created from a string with token string between tokens
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: TokenizeString.h $
* 
* *****************  Version 2  *****************
* User: Adam Clayton Date: 11/22/02   Time: 11:39a
* Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
*/

////////////////////////////////////////////////////////////////////////////

#pragma once
#define TOKENIZE_STRING_H

class TokenizeString
{
public:
	static void BuildArray(const char *pSourceString,const char *pTokenString,TupArray<TupString> &destArray);
};

