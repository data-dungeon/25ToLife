////////////////////////////////////////////////////////////////////////////
//
// TupString
//
// A dynamic string
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

inline TupStringData* TupString::GetData() const
	{ assert(m_pchData != NULL); return ((TupStringData*)m_pchData)-1; }
inline void TupString::Init(void)
	{ m_pchData = tupEmptyString.m_pchData; }
inline TupString::TupString(void)
	{ m_pchData = tupEmptyString.m_pchData; }
inline TupString::TupString(const unsigned char* lpsz)
	{ Init(); *this = (const char*)lpsz; }
inline const TupString& TupString::operator=(const unsigned char* lpsz)
	{ *this = (const char*)lpsz; return *this; }

inline int TupString::GetLength() const
	{ return GetData()->nDataLength; }
inline int TupString::GetAllocLength() const
	{ return GetData()->nAllocLength; }
inline bool TupString::IsEmpty() const
	{ return GetData()->nDataLength == 0; }
inline TupString::operator const char *() const
	{ return m_pchData; }
inline int TupString::SafeStrlen(const char * lpsz)
	{ return (lpsz == NULL) ? 0 : (int) strlen(lpsz); }

// TupString support
inline int TupString::Compare(const char *lpsz) const
	{ return strcmp(m_pchData, lpsz); }
inline int TupString::CompareNoCase(const char * lpsz) const
	{return stricmp(m_pchData, lpsz); }

inline char TupString::GetAt(int nIndex) const
{
	assert(nIndex >= 0);
	assert(nIndex < GetData()->nDataLength);
	return m_pchData[nIndex];
}
inline char TupString::operator[](int nIndex) const
{
	// same as GetAt
	assert(nIndex >= 0);
	assert(nIndex < GetData()->nDataLength);
	return m_pchData[nIndex];
}
inline bool operator==(const TupString& s1, const TupString& s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const TupString& s1, const char * s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const char * s1, const TupString& s2)
	{ return s2.Compare(s1) == 0; }
inline bool operator!=(const TupString& s1, const TupString& s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const TupString& s1, const char * s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const char * s1, const TupString& s2)
	{ return s2.Compare(s1) != 0; }
inline bool operator<(const TupString& s1, const TupString& s2)
	{ return s1.Compare(s2) < 0; }
inline bool operator<(const TupString& s1, const char * s2)
	{ return s1.Compare(s2) < 0; }
inline bool operator<(const char * s1, const TupString& s2)
	{ return s2.Compare(s1) > 0; }
inline bool operator>(const TupString& s1, const TupString& s2)
	{ return s1.Compare(s2) > 0; }
inline bool operator>(const TupString& s1, const char * s2)
	{ return s1.Compare(s2) > 0; }
inline bool operator>(const char * s1, const TupString& s2)
	{ return s2.Compare(s1) < 0; }
inline bool operator<=(const TupString& s1, const TupString& s2)
	{ return s1.Compare(s2) <= 0; }
inline bool operator<=(const TupString& s1, const char * s2)
	{ return s1.Compare(s2) <= 0; }
inline bool operator<=(const char * s1, const TupString& s2)
	{ return s2.Compare(s1) >= 0; }
inline bool operator>=(const TupString& s1, const TupString& s2)
	{ return s1.Compare(s2) >= 0; }
inline bool operator>=(const TupString& s1, const char * s2)
	{ return s1.Compare(s2) >= 0; }
inline bool operator>=(const char * s1, const TupString& s2)
	{ return s2.Compare(s1) <= 0; }
