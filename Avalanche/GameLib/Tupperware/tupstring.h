////////////////////////////////////////////////////////////////////////////
//
// TupString
//
// A dynamic string
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_STRING_H
#define TUPPERWARE_STRING_H

#ifndef TUPPERWARE_MEMORY_MANAGER_H
#include "memorymanager.h"
#endif

#if defined(PS2) || defined(GCN)
#define __cdecl
#endif
#ifdef GCN
#include <string.h>
#include <Assert.h>
#include <stdarg.h>
#endif
#if defined(PS2) && defined(SN)
#include <stdarg.h>
#endif

struct TupStringData
{
	unsigned int bNil:1;	// if this string is nil
	int nRefs:31;           // reference count
	int nDataLength;        // length of data (including terminator)
	int nAllocLength;       // length of allocation
	// char data[nAllocLength]

	char* data()           // char* to managed data
		{ return (char*)(this+1); }
};

class TupString;
const TupString& TupGetEmptyString(void);

#define tupEmptyString TupGetEmptyString()


class TupString
{
public:
// Constructors

	// constructs empty TupString
	TupString(void);
	// copy constructor
	TupString(const TupString& stringSrc);
	// from a single character
	TupString(char ch, int nRepeat = 1);
	// from an ANSI string (converts to char)
	TupString(const char * lpsz);
	// subset of characters from an ANSI string (converts to char)
	TupString(const char * lpch, int nLength);
	// from unsigned characters
	TupString(const unsigned char* psz);

// Attributes & Operations

	// get data length
	int GetLength(void) const;
	// TRUE if zero length
	bool IsEmpty(void) const;
	// clear contents to empty
	void Empty(void);

	// return single character at zero-based index
	char GetAt(int nIndex) const;
	// return single character at zero-based index
	char operator[](int nIndex) const;
	// set a single character at zero-based index
	void SetAt(int nIndex, char ch);
	// return pointer to const string
	operator const char *() const;

	// overloaded assignment

	// ref-counted copy from another TupString
	const TupString& operator=(const TupString& stringSrc);
	// set string content to single character
	const TupString& operator=(char ch);
	// copy string content from ANSI string (converts to char)
	const TupString& operator=(const char * lpsz);
	// copy string content from unsigned chars
	const TupString& operator=(const unsigned char* psz);

	// string concatenation

	// concatenate from another TupString
	const TupString& operator+=(const TupString& string);

	// concatenate a character string
	const TupString& operator+=(const char *lpsz);

	// concatenate a single character
	const TupString& operator+=(char ch);

	friend TupString operator+(const TupString& string1,
			const TupString& string2);
	friend TupString operator+(const TupString& string, char ch);
	friend TupString operator+(char ch, const TupString& string);
	friend TupString operator+(const TupString& string, const char * lpsz);
	friend TupString operator+(const char * lpsz, const TupString& string);

	// string comparison

	// straight character comparison
	int Compare(const char * lpsz) const;
	// compare ignoring case
	int CompareNoCase(const char * lpsz) const;
	// NLS aware comparison, case sensitive
	int Collate(const char * lpsz) const;
	// NLS aware comparison, case insensitive
	int CollateNoCase(const char * lpsz) const;

	// simple sub-string extraction

	// return nCount characters starting at zero-based nFirst
	TupString Mid(int nFirst, int nCount) const;
	// return all characters starting at zero-based nFirst
	TupString Mid(int nFirst) const;
	// return first nCount characters in string
	TupString Left(int nCount) const;
	// return nCount characters from end of string
	TupString Right(int nCount) const;

	//  characters from beginning that are also in passed string
	TupString SpanIncluding(const char * lpszCharSet) const;
	// characters from beginning that are not also in passed string
	TupString SpanExcluding(const char * lpszCharSet) const;

	// upper/lower/reverse conversion

	// NLS aware conversion to uppercase
	void MakeUpper(void);
	// NLS aware conversion to lowercase
	void MakeLower(void);
	// reverse string right-to-left
	void MakeReverse(void);

	// trimming whitespace (either side)

	// remove whitespace starting from right edge
	void TrimRight(void);
	// remove whitespace starting from left side
	void TrimLeft(void);

	// trimming anything (either side)

	// remove continuous occurrences of chTarget starting from right
	void TrimRight(char chTarget);
	// remove continuous occcurrences of characters in passed string,
	// starting from right
	void TrimRight(const char * lpszTargets);
	// remove continuous occurrences of chTarget starting from left
	void TrimLeft(char chTarget);
	// remove continuous occcurrences of characters in
	// passed string, starting from left
	void TrimLeft(const char * lpszTargets);

	// advanced manipulation

	// replace occurrences of chOld with chNew
	int Replace(char chOld, char chNew);
	// replace occurrences of substring lpszOld with lpszNew;
	// empty lpszNew removes instances of lpszOld
	int Replace(const char * lpszOld, const char * lpszNew);
	// remove occurrences of chRemove
	int Remove(char chRemove);
	// insert character at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, char ch);
	// insert substring at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, const char * pstr);
	// delete nCount characters starting at zero-based index
	int Delete(int nIndex, int nCount = 1);

	// searching

	// find character starting at left, -1 if not found
	int Find(char ch) const;
	// find character starting at right
	int ReverseFind(char ch) const;
	// find character starting at zero-based index and going right
	int Find(char ch, int nStart) const;
	// find first instance of any character in passed string
	int FindOneOf(const char * lpszCharSet) const;
	// find first instance of substring
	int Find(const char * lpszSub) const;
	// find first instance of substring starting at zero-based index
	int Find(const char * lpszSub, int nStart) const;

	// simple formatting

	// printf-like formatting using passed string
	void __cdecl Format(const char * lpszFormat, ...);
	// printf-like formatting using variable arguments parameter
	void FormatV(const char * lpszFormat, va_list argList);

	// Access to string implementation buffer as "C" character array

	// get pointer to modifiable buffer at least as long as nMinBufLength
	char * GetBuffer(int nMinBufLength);
	// release buffer, setting length to nNewLength (or to first nul if -1)
	void ReleaseBuffer(int nNewLength = -1);
	// get pointer to modifiable buffer exactly as long as nNewLength
	char * GetBufferSetLength(int nNewLength);
	// release memory allocated to but unused by string
	void FreeExtra();

	// Use LockBuffer/UnlockBuffer to turn refcounting off

	// turn refcounting back on
	char * LockBuffer(void);
	// turn refcounting off
	void UnlockBuffer(void);

// Implementation
public:
	~TupString();
	int GetAllocLength() const;

#ifdef USE_TUP_MEMORY_MANAGER
	void *operator new(size_t size);
	void operator delete(void *pBuffer);
#endif

protected:
	char * m_pchData;   // pointer to ref counted string data

	// implementation helpers
	TupStringData* GetData() const;
	void Init();
	void AllocCopy(TupString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void AllocBuffer(int nLen);
	void AssignCopy(int nSrcLen, const char * lpszSrcData);
	void ConcatCopy(int nSrc1Len, const char * lpszSrc1Data, int nSrc2Len, const char * lpszSrc2Data);
	void ConcatInPlace(int nSrcLen, const char * lpszSrcData);
	void CopyBeforeWrite();
	void AllocBeforeWrite(int nLen);
	void Release();
	static void Release(TupStringData* pData);
	static int SafeStrlen(const char * lpsz);
	static void FreeData(TupStringData* pData);
};

// Compare helpers
bool operator==(const TupString& s1, const TupString& s2);
bool operator==(const TupString& s1, const char * s2);
bool operator==(const char * s1, const TupString& s2);
bool operator!=(const TupString& s1, const TupString& s2);
bool operator!=(const TupString& s1, const char * s2);
bool operator!=(const char * s1, const TupString& s2);
bool operator<(const TupString& s1, const TupString& s2);
bool operator<(const TupString& s1, const char * s2);
bool operator<(const char * s1, const TupString& s2);
bool operator>(const TupString& s1, const TupString& s2);
bool operator>(const TupString& s1, const char * s2);
bool operator>(const char * s1, const TupString& s2);
bool operator<=(const TupString& s1, const TupString& s2);
bool operator<=(const TupString& s1, const char * s2);
bool operator<=(const char * s1, const TupString& s2);
bool operator>=(const TupString& s1, const TupString& s2);
bool operator>=(const TupString& s1, const char * s2);
bool operator>=(const char * s1, const TupString& s2);

#include "tupstring.hpp"

#endif
