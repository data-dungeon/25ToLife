/***********************************/
/*                                 */
/* PigpenUtility.cpp               */
/* Pigpen particle tool  12/07/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* utility routines                */
/*                                 */
/***********************************/

/* stand-alone utility routines for the particle tool */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************* defines *****************************/

/******************* structures **************************/

/******************* local variables *********************/

/******************* externs *****************************/

/******************* prototypes **************************/

/******************* functions ***************************/

/* convert a number to a CString for display purposes */

CString& ToCString(int nValue)
{
	/* this is ugly, but step through array with each call so we can have multiple calls in a single line, */
	/* i.e. ToCString(value1) + ToCString(value2), otherwise single CString gets overwritten and result is bogus */

#define TOCSTRING_ARRAY_SIZE 8
	static CString ReturnString[TOCSTRING_ARRAY_SIZE];
	static int nIndex = 0;
	nIndex = (nIndex + 1) % (TOCSTRING_ARRAY_SIZE - 1);
	ReturnString[nIndex].Format("%d", nValue);
	return(ReturnString[nIndex]);
}
CString& ToCString(float fValue)
{
	static CString ReturnString[TOCSTRING_ARRAY_SIZE];
	static int nIndex = 0;
	nIndex = (nIndex + 1) % (TOCSTRING_ARRAY_SIZE - 1);
	ReturnString[nIndex].Format("%6.2f", fValue);
	return(ReturnString[nIndex]);
}

/* check if a string is all zeros, for atoi error checking. periods are ok, too, to pass 0.0 */

bool AllZeros(CString& String)
{
	for (int i = 0; i < String.GetLength(); i++)
	{
		char c = String.GetAt(i);
		if ((c != '0') && (c != '.'))
			return(false);
	}
	return(true);
}

/* delete all objects from CObArray list */

void DeleteArray(CObArray& Array)
{
	for (int i = 0; i < Array.GetSize(); i++)
	{
		CObject *pObject = Array.GetAt(i);
		delete pObject;	// calls destructor
	}
	Array.RemoveAll();
}

/* reverse byte order for endianness */

void SwapBytes(char *pBytes, int nCount)
{
	char cTemp;
	for (int i = 0; i < nCount / 2; i++)
	{
		cTemp = pBytes[i];
		pBytes[i] = pBytes[nCount - 1 - i];
		pBytes[nCount - 1 - i] = cTemp;
	}
}
