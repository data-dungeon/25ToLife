////////////////////////////////////////////////////////////////////////////
//
// TupString
//
// A dynamic string
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "tupstring.h"
#include "tuparray.h"
#include "elements.h"
#include "Hash.h"

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

/////////////////////////////////////////////////////////////////////////////
// static class data, special inlines


// For an empty string, m_pchData will point here
// (note: avoids special case of checking for NULL m_pchData)
// empty string data (and locked)
static char tupStringNil = '\0';
static int tupStringInitData[] = { -1, 0, 0, 0 };
TupStringData* pTupStringDataNil = (TupStringData*)&tupStringInitData;
const char *pTupStringPchNil = (const char *)(((char*)&tupStringInitData)+sizeof(TupStringData));
// special function to make afxEmptyString work even during initialization
const TupString& TupGetEmptyString(void)
{
	return *(TupString*)&pTupStringPchNil;
}

TupString::TupString(const TupString& stringSrc)
{
	assert(stringSrc.GetData()->nRefs != 0);
	if (stringSrc.GetData()->nRefs >= 0)
	{
		assert(!stringSrc.GetData()->bNil);
		m_pchData = stringSrc.m_pchData;
		GetData()->nRefs++;
	}
	else
	{
		Init();
		*this = stringSrc.m_pchData;
	}
}

void TupString::AllocBuffer(int nLen)
// always allocate one extra character for '\0' termination
// assumes [optimistically] that data length will equal allocation length
{
	assert(nLen >= 0);
	assert(nLen <= 2147483647-1);    // max size (enough room for 1 extra)

	if (nLen == 0)
		Init();
	else
	{
		TupStringData* pData;
		{
#ifdef USE_TUP_MEMORY_MANAGER
			pData = (TupStringData*) TupMemoryManager::Allocate(sizeof(TupStringData) + (nLen+1)*sizeof(char));
#else
			pData = (TupStringData*) new unsigned char[sizeof(TupStringData) + (nLen+1)*sizeof(char)];
#endif
			pData->nAllocLength = nLen;
		}
		pData->bNil = 0;
		pData->nRefs = 1;
		pData->data()[nLen] = '\0';
		pData->nDataLength = nLen;
		m_pchData = pData->data();
	}
}

void TupString::FreeData(TupStringData* pData)
{
#ifdef USE_TUP_MEMORY_MANAGER
	TupMemoryManager::FreeBuffer(pData);
#else
	delete[] (unsigned char*)pData;
#endif
}

void TupString::Release()
{
	if (!GetData()->bNil)
	{
		assert(GetData()->nRefs != 0);
		GetData()->nRefs--;
		if (GetData()->nRefs <= 0)
			FreeData(GetData());
		Init();
	}
}

void TupString::Release(TupStringData* pData)
{
	if (!pData->bNil)
	{
		assert(pData->nRefs != 0);
		pData->nRefs--;
		if (pData->nRefs <= 0)
			FreeData(pData);
	}
}

void TupString::Empty()
{
	if (GetData()->nDataLength == 0)
		return;
	if (GetData()->nRefs >= 0)
		Release();
	else
		*this = &tupStringNil;
	assert(GetData()->nDataLength == 0);
	assert(GetData()->nRefs < 0 || GetData()->nAllocLength == 0);
}

void TupString::CopyBeforeWrite()
{
	if (GetData()->nRefs > 1)
	{
		TupStringData* pData = GetData();
		Release();
		AllocBuffer(pData->nDataLength);
		memcpy(m_pchData, pData->data(), (pData->nDataLength+1)*sizeof(char));
	}
	assert(GetData()->nRefs <= 1);
}

void TupString::AllocBeforeWrite(int nLen)
{
	if (GetData()->nRefs > 1 || nLen > GetData()->nAllocLength)
	{
		Release();
		AllocBuffer(nLen);
	}
	assert(GetData()->nRefs <= 1);
}

TupString::~TupString()
//  free any attached data
{
	if (!GetData()->bNil)
	{
		GetData()->nRefs--;
		if (GetData()->nRefs <= 0)
			FreeData(GetData());
	}
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for the rest of the implementation

void TupString::AllocCopy(TupString& dest, int nCopyLen, int nCopyIndex,
	 int nExtraLen) const
{
	// will clone the data attached to this string
	// allocating 'nExtraLen' characters
	// Places results in uninitialized string 'dest'
	// Will copy the part or all of original data to start of new string

	int nNewLen = nCopyLen + nExtraLen;
	if (nNewLen == 0)
	{
		dest.Init();
	}
	else
	{
		dest.AllocBuffer(nNewLen);
		memcpy(dest.m_pchData, m_pchData+nCopyIndex, nCopyLen*sizeof(char));
	}
}

//////////////////////////////////////////////////////////////////////////////
// More sophisticated construction

TupString::TupString(const char * lpsz)
{
	Init();
	int nLen = SafeStrlen(lpsz);
	if (nLen != 0)
	{
		AllocBuffer(nLen);
		memcpy(m_pchData, lpsz, nLen*sizeof(char));
	}
}


//////////////////////////////////////////////////////////////////////////////
// Assignment operators
//  All assign a new value to the string
//      (a) first see if the buffer is big enough
//      (b) if enough room, copy on top of old buffer, set size and type
//      (c) otherwise free old string data, and create a new one
//
//  All routines return the new string (but as a 'const TupString&' so that
//      assigning it again will cause a copy, eg: s1 = s2 = "hi there".
//

void TupString::AssignCopy(int nSrcLen, const char * lpszSrcData)
{
	AllocBeforeWrite(nSrcLen);
	memcpy(m_pchData, lpszSrcData, nSrcLen*sizeof(char));
	GetData()->nDataLength = nSrcLen;
	m_pchData[nSrcLen] = '\0';
}

const TupString& TupString::operator=(const TupString& stringSrc)
{
	if (m_pchData != stringSrc.m_pchData)
	{
		if ((GetData()->nRefs < 0 && !GetData()->bNil) ||
			stringSrc.GetData()->nRefs < 0)
		{
			// actual copy necessary since one of the strings is locked
			AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pchData);
		}
		else
		{
			// can just copy references around
			Release();
			assert(!stringSrc.GetData()->bNil);
			m_pchData = stringSrc.m_pchData;
			GetData()->nRefs++;
		}
	}
	return *this;
}

const TupString& TupString::operator=(const char * lpsz)
{
	AssignCopy(SafeStrlen(lpsz), lpsz);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// concatenation

// NOTE: "operator+" is done as friend functions for simplicity
//      There are three variants:
//          TupString + TupString
// and for ? = char, const char *
//          TupString + ?
//          ? + TupString

void TupString::ConcatCopy(int nSrc1Len, const char * lpszSrc1Data,
	int nSrc2Len, const char * lpszSrc2Data)
{
  // -- master concatenation routine
  // Concatenate two sources
  // -- assume that 'this' is a new TupString object

	int nNewLen = nSrc1Len + nSrc2Len;
	if (nNewLen != 0)
	{
		AllocBuffer(nNewLen);
		memcpy(m_pchData, lpszSrc1Data, nSrc1Len*sizeof(char));
		memcpy(m_pchData+nSrc1Len, lpszSrc2Data, nSrc2Len*sizeof(char));
	}
}

TupString operator+(const TupString& string1, const TupString& string2)
{
	TupString s;
	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData,
		string2.GetData()->nDataLength, string2.m_pchData);
	return s;
}

TupString operator+(const TupString& string, const char * lpsz)
{
	TupString s;
	s.ConcatCopy(string.GetData()->nDataLength, string.m_pchData,
		TupString::SafeStrlen(lpsz), lpsz);
	return s;
}

TupString operator+(const char * lpsz, const TupString& string)
{
	TupString s;
	s.ConcatCopy(TupString::SafeStrlen(lpsz), lpsz, string.GetData()->nDataLength,
		string.m_pchData);
	return s;
}

//////////////////////////////////////////////////////////////////////////////
// concatenate in place

void TupString::ConcatInPlace(int nSrcLen, const char * lpszSrcData)
{
	//  -- the main routine for += operators

	// concatenating an empty string is a no-op!
	if (nSrcLen == 0)
		return;

	// if the buffer is too small, or we have a width mis-match, just
	//   allocate a new buffer (slow but sure)
	if (GetData()->nRefs > 1 || GetData()->nDataLength + nSrcLen > GetData()->nAllocLength)
	{
		// we have to grow the buffer, use the ConcatCopy routine
		TupStringData* pOldData = GetData();
		ConcatCopy(GetData()->nDataLength, m_pchData, nSrcLen, lpszSrcData);
		assert(pOldData != NULL);
		TupString::Release(pOldData);
	}
	else
	{
		// fast concatenation when buffer big enough
		memcpy(m_pchData+GetData()->nDataLength, lpszSrcData, nSrcLen*sizeof(char));
		GetData()->nDataLength += nSrcLen;
		assert(GetData()->nDataLength <= GetData()->nAllocLength);
		m_pchData[GetData()->nDataLength] = '\0';
	}
}

const TupString& TupString::operator+=(const char * lpsz)
{
	ConcatInPlace(SafeStrlen(lpsz), lpsz);
	return *this;
}

const TupString& TupString::operator+=(char ch)
{
	ConcatInPlace(1, &ch);
	return *this;
}

const TupString& TupString::operator+=(const TupString& string)
{
	ConcatInPlace(string.GetData()->nDataLength, string.m_pchData);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Advanced direct buffer access

char * TupString::GetBuffer(int nMinBufLength)
{
	assert(nMinBufLength >= 0);

	if (GetData()->nRefs > 1 || nMinBufLength > GetData()->nAllocLength)
	{
		// we have to grow the buffer
		TupStringData* pOldData = GetData();
		int nOldLen = GetData()->nDataLength;   // AllocBuffer will tromp it
		if (nMinBufLength < nOldLen)
			nMinBufLength = nOldLen;
		AllocBuffer(nMinBufLength);
		memcpy(m_pchData, pOldData->data(), (nOldLen+1)*sizeof(char));
		GetData()->nDataLength = nOldLen;
		TupString::Release(pOldData);
	}
	assert(GetData()->nRefs <= 1);

	// return a pointer to the character storage for this string
	assert(m_pchData != NULL);
	return m_pchData;
}

void TupString::ReleaseBuffer(int nNewLength)
{
	CopyBeforeWrite();  // just in case GetBuffer was not called

	if (nNewLength == -1)
		nNewLength = strlen(m_pchData); // zero terminated

	assert(nNewLength <= GetData()->nAllocLength);
	GetData()->nDataLength = nNewLength;
	m_pchData[nNewLength] = '\0';
}

char * TupString::GetBufferSetLength(int nNewLength)
{
	assert(nNewLength >= 0);

	GetBuffer(nNewLength);
	GetData()->nDataLength = nNewLength;
	m_pchData[nNewLength] = '\0';
	return m_pchData;
}

void TupString::FreeExtra()
{
	assert(GetData()->nDataLength <= GetData()->nAllocLength);
	if (GetData()->nDataLength != GetData()->nAllocLength)
	{
		TupStringData* pOldData = GetData();
		AllocBuffer(GetData()->nDataLength);
		memcpy(m_pchData, pOldData->data(), pOldData->nDataLength*sizeof(char));
		assert(m_pchData[GetData()->nDataLength] == '\0');
		TupString::Release(pOldData);
	}
	assert(GetData() != NULL);
}

char * TupString::LockBuffer()
{
	char * lpsz = GetBuffer(0);
	GetData()->nRefs = -1;
	return lpsz;
}

void TupString::UnlockBuffer()
{
	assert(GetData()->nRefs == -1);
	if (!GetData()->bNil)
		GetData()->nRefs = 1;
}

///////////////////////////////////////////////////////////////////////////////
// Commonly used routines (rarely used routines in STREX.CPP)

int TupString::Find(char ch) const
{
	return Find(ch, 0);
}

int TupString::Find(char ch, int nStart) const
{
	int nLength = GetData()->nDataLength;
	if (nStart >= nLength)
		return -1;

	// find first single character
	char * lpsz = strchr(m_pchData + nStart, (unsigned char)ch);

	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

int TupString::FindOneOf(const char * lpszCharSet) const
{
	char * lpsz = strpbrk(m_pchData, lpszCharSet);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

void TupString::MakeUpper()
{
	CopyBeforeWrite();
	strupr(m_pchData);
}

void TupString::MakeLower()
{
	CopyBeforeWrite();
	strlwr(m_pchData);
}

void TupString::MakeReverse()
{
	CopyBeforeWrite();

	int length = strlen(m_pchData);
	for (int i=0;i<length;i++)
	{
		int end = (length-1)-i;
		char temp = m_pchData[i];
		m_pchData[i] = m_pchData[end];
		m_pchData[end] = temp;
	}
}

void TupString::SetAt(int nIndex, char ch)
{
	assert(nIndex >= 0);
	assert(nIndex < GetData()->nDataLength);

	CopyBeforeWrite();
	m_pchData[nIndex] = ch;
}

//////////////////////////////////////////////////////////////////////////////
// More sophisticated construction

TupString::TupString(char ch, int nLength)
{
	Init();
	if (nLength >= 1)
	{
		AllocBuffer(nLength);
		memset(m_pchData, ch, nLength);
	}
}

TupString::TupString(const char * lpch, int nLength)
{
	Init();
	if (nLength != 0)
	{
		AllocBuffer(nLength);
		memcpy(m_pchData, lpch, nLength*sizeof(char));
	}
}

//////////////////////////////////////////////////////////////////////////////
// Assignment operators

const TupString& TupString::operator=(char ch)
{
	AssignCopy(1, &ch);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// less common string expressions

TupString operator+(const TupString& string1, char ch)
{
	TupString s;
	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData, 1, &ch);
	return s;
}

TupString operator+(char ch, const TupString& string)
{
	TupString s;
	s.ConcatCopy(1, &ch, string.GetData()->nDataLength, string.m_pchData);
	return s;
}

//////////////////////////////////////////////////////////////////////////////
// Advanced manipulation

int TupString::Delete(int nIndex, int nCount /* = 1 */)
{
	if (nIndex < 0)
		nIndex = 0;
	int nNewLength = GetData()->nDataLength;
	if (nCount > 0 && nIndex < nNewLength)
	{
		CopyBeforeWrite();
		int nBytesToCopy = nNewLength - (nIndex + nCount) + 1;

		memmove(m_pchData + nIndex,
			m_pchData + nIndex + nCount, nBytesToCopy * sizeof(char));
		GetData()->nDataLength = nNewLength - nCount;
	}

	return nNewLength;
}

int TupString::Insert(int nIndex, char ch)
{
	CopyBeforeWrite();

	if (nIndex < 0)
		nIndex = 0;

	int nNewLength = GetData()->nDataLength;
	if (nIndex > nNewLength)
		nIndex = nNewLength;
	nNewLength++;

	if (GetData()->nAllocLength < nNewLength)
	{
		TupStringData* pOldData = GetData();
		char * pstr = m_pchData;
		AllocBuffer(nNewLength);
		memcpy(m_pchData, pstr, (pOldData->nDataLength+1)*sizeof(char));
		TupString::Release(pOldData);
	}

	// move existing bytes down
	memmove(m_pchData + nIndex + 1,
		m_pchData + nIndex, (nNewLength-nIndex)*sizeof(char));
	m_pchData[nIndex] = ch;
	GetData()->nDataLength = nNewLength;

	return nNewLength;
}

int TupString::Insert(int nIndex, const char * pstr)
{
	if (nIndex < 0)
		nIndex = 0;

	int nInsertLength = SafeStrlen(pstr);
	int nNewLength = GetData()->nDataLength;
	if (nInsertLength > 0)
	{
		CopyBeforeWrite();
		if (nIndex > nNewLength)
			nIndex = nNewLength;
		nNewLength += nInsertLength;

		if (GetData()->nAllocLength < nNewLength)
		{
			TupStringData* pOldData = GetData();
			char * pstr = m_pchData;
			AllocBuffer(nNewLength);
			memcpy(m_pchData, pstr, (pOldData->nDataLength+1)*sizeof(char));
			TupString::Release(pOldData);
		}

		// move existing bytes down
		memmove(m_pchData + nIndex + nInsertLength,
			m_pchData + nIndex,
			(nNewLength-nIndex-nInsertLength+1)*sizeof(char));
		memcpy(m_pchData + nIndex,
			pstr, nInsertLength*sizeof(char));
		GetData()->nDataLength = nNewLength;
	}

	return nNewLength;
}

int TupString::Replace(char chOld, char chNew)
{
	int nCount = 0;

	// short-circuit the nop case
	if (chOld != chNew)
	{
		// otherwise modify each character that matches in the string
		CopyBeforeWrite();
		char * psz = m_pchData;
		char * pszEnd = psz + GetData()->nDataLength;
		while (psz < pszEnd)
		{
			// replace instances of the specified character only
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz++;
		}
	}
	return nCount;
}

int TupString::Replace(const char * lpszOld, const char * lpszNew)
{
	// can't have empty or NULL lpszOld

	int nSourceLen = SafeStrlen(lpszOld);
	if (nSourceLen == 0)
		return 0;
	int nReplacementLen = SafeStrlen(lpszNew);

	// loop once to figure out the size of the result string
	int nCount = 0;
	char * lpszStart = m_pchData;
	char * lpszEnd = m_pchData + GetData()->nDataLength;
	char * lpszTarget;
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
		{
			nCount++;
			lpszStart = lpszTarget + nSourceLen;
		}
		lpszStart += strlen(lpszStart) + 1;
	}

	// if any changes were made, make them
	if (nCount > 0)
	{
		CopyBeforeWrite();

		// if the buffer is too small, just
		//   allocate a new buffer (slow but sure)
		int nOldLength = GetData()->nDataLength;
		int nNewLength =  nOldLength + (nReplacementLen-nSourceLen)*nCount;
		if (GetData()->nAllocLength < nNewLength || GetData()->nRefs > 1)
		{
			TupStringData* pOldData = GetData();
			char * pstr = m_pchData;
			AllocBuffer(nNewLength);
			memcpy(m_pchData, pstr, pOldData->nDataLength*sizeof(char));
			TupString::Release(pOldData);
		}
		// else, we just do it in-place
		lpszStart = m_pchData;
		lpszEnd = m_pchData + GetData()->nDataLength;

		// loop again to actually do the work
		while (lpszStart < lpszEnd)
		{
			while ( (lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLength - (lpszTarget - m_pchData + nSourceLen);
				memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
					nBalance * sizeof(char));
				memcpy(lpszTarget, lpszNew, nReplacementLen*sizeof(char));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
			}
			lpszStart += strlen(lpszStart) + 1;
		}
		assert(m_pchData[nNewLength] == '\0');
		GetData()->nDataLength = nNewLength;
	}

	return nCount;
}

int TupString::Remove(char chRemove)
{
	CopyBeforeWrite();

	char * pstrSource = m_pchData;
	char * pstrDest = m_pchData;
	char * pstrEnd = m_pchData + GetData()->nDataLength;

	while (pstrSource < pstrEnd)
	{
		if (*pstrSource != chRemove)
		{
			*pstrDest = *pstrSource;
			pstrDest++;
		}
		pstrSource++;
	}
	*pstrDest = '\0';
	int nCount = pstrSource - pstrDest;
	GetData()->nDataLength -= nCount;

	return nCount;
}

//////////////////////////////////////////////////////////////////////////////
// Very simple sub-string extraction

TupString TupString::Mid(int nFirst) const
{
	return Mid(nFirst, GetData()->nDataLength - nFirst);
}

TupString TupString::Mid(int nFirst, int nCount) const
{
	// out-of-bounds requests return sensible things
	if (nFirst < 0)
		nFirst = 0;
	if (nCount < 0)
		nCount = 0;

	if (nFirst + nCount > GetData()->nDataLength)
		nCount = GetData()->nDataLength - nFirst;
	if (nFirst > GetData()->nDataLength)
		nCount = 0;

	assert(nFirst >= 0);
	assert(nFirst + nCount <= GetData()->nDataLength);

	// optimize case of returning entire string
	if (nFirst == 0 && nFirst + nCount == GetData()->nDataLength)
		return *this;

	TupString dest;
	AllocCopy(dest, nCount, nFirst, 0);
	return dest;
}

TupString TupString::Right(int nCount) const
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetData()->nDataLength)
		return *this;

	TupString dest;
	AllocCopy(dest, nCount, GetData()->nDataLength-nCount, 0);
	return dest;
}

TupString TupString::Left(int nCount) const
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetData()->nDataLength)
		return *this;

	TupString dest;
	AllocCopy(dest, nCount, 0, 0);
	return dest;
}

// strspn equivalent
TupString TupString::SpanIncluding(const char * lpszCharSet) const
{
	return Left(strspn(m_pchData, lpszCharSet));
}

// strcspn equivalent
TupString TupString::SpanExcluding(const char * lpszCharSet) const
{
	return Left(strspn(m_pchData, lpszCharSet));
}

//////////////////////////////////////////////////////////////////////////////
// Finding

int TupString::ReverseFind(char ch) const
{
	// find last single character
	char * lpsz = strrchr(m_pchData, (int) ch);

	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

// find a sub-string (like strstr)
int TupString::Find(const char * lpszSub) const
{
	return Find(lpszSub, 0);
}

int TupString::Find(const char * lpszSub, int nStart) const
{
	int nLength = GetData()->nDataLength;
	if (nStart > nLength)
		return -1;

	// find first matching substring
	char * lpsz = strstr(m_pchData + nStart, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}


/////////////////////////////////////////////////////////////////////////////
// TupString formatting

#define TCHAR_ARG   char
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char

#ifdef _X86_
	#define DOUBLE_ARG  _AFX_DOUBLE
#else
	#define DOUBLE_ARG  double
#endif

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000

void TupString::FormatV(const char * lpszFormat, va_list argList)
{
	Empty();

// don't do anything for gamecube or code warrior
#if !defined(GCN) && !defined(PS2)

	int nLength = (int)strlen(lpszFormat);
	if(nLength == 0)
	{
		// We're done
		return;
	}

	// Default buffer size 256K, see below
	int nNewBufferSize = 128;

	char *pNewBuffer = NULL;
	int nFormat = -1; 

	// Loop until all of our text fits in the buffer
	while(nFormat == -1)
	{
		delete[] pNewBuffer;
		nNewBufferSize *= 2;
		pNewBuffer = new char[nNewBufferSize];
		nFormat = _vsnprintf(pNewBuffer, nNewBufferSize, lpszFormat, argList);
	}

	if(nFormat > 0)
	{
		*this = pNewBuffer;
	}

	delete[] pNewBuffer;
#endif
}

// formatting (using wsprintf style formatting)
void TupString::Format(const char * lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	FormatV(lpszFormat, argList);
	va_end(argList);
}


void TupString::TrimRight(const char * lpszTargetList)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

	CopyBeforeWrite();
	char * lpsz = m_pchData;
	char * lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (strchr(lpszTargetList, *lpsz) != NULL)
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz++;
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character
		*lpszLast = '\0';
		GetData()->nDataLength = lpszLast - m_pchData;
	}
}

void TupString::TrimRight(char chTarget)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

	CopyBeforeWrite();
	char * lpsz = m_pchData;
	char * lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (*lpsz == chTarget)
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz++;
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character
		*lpszLast = '\0';
		GetData()->nDataLength = lpszLast - m_pchData;
	}
}

void TupString::TrimRight()
{
	// find beginning of trailing spaces by starting at beginning (DBCS aware)

	CopyBeforeWrite();
	char * lpsz = m_pchData;
	char * lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (isspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz++;
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast = '\0';
		GetData()->nDataLength = lpszLast - m_pchData;
	}
}

void TupString::TrimLeft(const char * lpszTargets)
{
	// if we're not trimming anything, we're not doing any work
	if (SafeStrlen(lpszTargets) == 0)
		return;

	CopyBeforeWrite();
	const char * lpsz = m_pchData;

	while (*lpsz != '\0')
	{
		if (strchr(lpszTargets, *lpsz) == NULL)
			break;
		lpsz++;
	}

	if (lpsz != m_pchData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength+1)*sizeof(char));
		GetData()->nDataLength = nDataLength;
	}
}

void TupString::TrimLeft(char chTarget)
{
	// find first non-matching character

	CopyBeforeWrite();
	const char * lpsz = m_pchData;

	while (chTarget == *lpsz)
		lpsz++;

	if (lpsz != m_pchData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength+1)*sizeof(char));
		GetData()->nDataLength = nDataLength;
	}
}

void TupString::TrimLeft()
{
	// find first non-space character

	CopyBeforeWrite();
	const char * lpsz = m_pchData;

	while (isspace(*lpsz))
		lpsz++;

	if (lpsz != m_pchData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength+1)*sizeof(char));
		GetData()->nDataLength = nDataLength;
	}
}

#ifdef USE_TUP_MEMORY_MANAGER
void *TupString::operator new(size_t nSize)
{
	return TupMemoryManager::Allocate(nSize); // call this one if running in the game engine
}

void TupString::operator delete(void *pBuffer)
{
	TupMemoryManager::FreeBuffer(pBuffer);
}

#endif


#ifndef TEMPLATE_SPECIALIZATION
void TupConstructElements(TupString* pElements, int nCount)
#else
template<> void TupConstructElements<TupString> (TupString* pElements, int nCount)
#endif
{
	for (; nCount--; ++pElements)
		memcpy(pElements, &tupEmptyString, sizeof(*pElements));
}

#ifndef TEMPLATE_SPECIALIZATION
void TupDestructElements(TupString* pElements, int nCount)
#else
template<> void TupDestructElements<TupString> (TupString* pElements, int nCount)
#endif
{
	for (; nCount--; ++pElements)
		pElements->~TupString();
}

#ifndef TEMPLATE_SPECIALIZATION
void TupCopyElements(TupString* pDest, const TupString* pSrc, int nCount)
#else
template<> void TupCopyElements<TupString> (TupString* pDest, const TupString* pSrc, int nCount)
#endif
{
	for (; nCount--; ++pDest, ++pSrc)
		*pDest = *pSrc;
}

#ifndef TEMPLATE_SPECIALIZATION
unsigned int GetHashValue(const TupString& entry)
#else
template<> unsigned int GetHashValue<TupString> (const TupString& entry)
#endif
{
	unsigned int nHash = 0;
	const char *pData = entry;
	while (*pData)
		nHash = (nHash<<5) + nHash + *pData++;
	return nHash;
}

