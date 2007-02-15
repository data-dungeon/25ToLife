// The definitions in this file provide portability support to GameCube apps
// compiled with the CodeWarrior compilers for "standard" type functions
// not supplied by the CW compiler suite

/*******************************************************************************
	INCLUDE
*******************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"





/*******************************************************************************
	DEFINE
*******************************************************************************/

/*******************************************************************************
	TYPEDEF
*******************************************************************************/

/*******************************************************************************
	EXTERN
*******************************************************************************/

/*******************************************************************************
	FUNCTIONS
*******************************************************************************/

/* return 0 if strings are the same, negative if string1 is alphabetically before string2, positive if string1 is
alphabetically after string2. ignore case */

int _stricmp( const char *string1, const char *string2 )
{
	int nIndex = 0;
	while (true)
	{
		char c1 = string1[nIndex];
		char c2 = string2[nIndex];
		nIndex++;
		if (c1 == c2)
		{
			/* if we've reached end of string1 they are the same (since c1 == c2, both are 0) */

			if (c1 == 0)
				return(0);
		}
		else
		{
			/* both characters to lowercase */

			if ((c1 >= 'A') && (c1 <= 'Z'))
				c1 += 'a' - 'A';
			if ((c2 >= 'A') && (c2 <= 'Z'))
				c2 += 'a' - 'A';

			/* if characters are different, return positive or negative. this also handles one string ending before the other */

			if (c1 != c2)
				return(c1 - c2);	// negative if c1 is before c2 alphabetically, positive otherwise
		}
	}
}	

/* return 0 if strings are the same, negative if string1 is alphabetically before string2, positive if string1 is
alphabetically after string2. ignore case, only consider the first <count> characters */

int _strnicmp( const char *string1, const char *string2, size_t count)
{
	int nIndex = 0;
	while (nIndex < count)
	{
		char c1 = string1[nIndex];
		char c2 = string2[nIndex];
		nIndex++;
		if (c1 == c2)
		{
			/* if we've reached end of string1 they are the same (since c1 == c2, both are 0) */

			if (c1 == 0)
				return(0);
		}
		else
		{
			/* both characters to lowercase */

			if ((c1 >= 'A') && (c1 <= 'Z'))
				c1 += 'a' - 'A';
			if ((c2 >= 'A') && (c2 <= 'Z'))
				c2 += 'a' - 'A';

			/* if characters are different, return positive or negative. this also handles one string ending before the other */

			if (c1 != c2)
				return(c1 - c2);	// negative if c1 is before c2 alphabetically, positive otherwise
		}
	}

	/* same through <count> characters */

	return(0);
}

char * _strupr( char *string)
{

	int		iChar;

	for( iChar = 0; iChar < strlen( string); iChar++)
	{
		string[iChar] = toupper( string[iChar]);
	}

	return string;
}

char * _strlwr( char *string)
{

	int		iChar;

	for( iChar = 0; iChar < strlen( string); iChar++)
	{
		string[iChar] = tolower( string[iChar]);
	}

	return string;
}

char * _strset( char *string, int c)
{

	char	*pChar = string;

	while( *pChar)
	{
		*pChar = c;
		pChar++;
	}
	
	return( string);
}





