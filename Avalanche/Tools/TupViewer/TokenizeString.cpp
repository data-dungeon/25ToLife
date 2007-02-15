////////////////////////////////////////////////////////////////////////////
//
// TokenizeString
//
// Class build an array of string that are created from a string with token string between tokens
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TokenizeString.h"

void TokenizeString::BuildArray(const char *pSourceString,const char *pTokenString,TupArray<TupString> &destArray)
{
	int tokenStringLength = strlen(pTokenString);
	destArray.RemoveAll();
	TupString sourceString(pSourceString);
	while (sourceString.GetLength())
	{
		int foundIndex = sourceString.Find(pTokenString);
		// if we found the token
		if (foundIndex!=-1)
		{
			destArray.Add(sourceString.Left(foundIndex));
			sourceString.Delete(0,foundIndex+tokenStringLength); // get rid of text to the left of the token including the token
		}
		else
		{
			destArray.Add(sourceString); // add what is left since no tokens found
			sourceString.Empty(); // we are done
		}
	}
}
