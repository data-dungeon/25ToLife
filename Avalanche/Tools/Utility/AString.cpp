// SString.cpp : implementation file
//

#include "stdafx.h"
#include "AString.h"
#include "Resource.h"
#include "Vector.h"
#include "BaseStream.h"
#include <afxpriv.h>

String::String( const Vector &v )
{
   char init[64];
   
   sprintf( init, "%g %g %g", v.x, v.y, v.z );
   if (m_pData = Alloc(( strlen(init)+1 )))
      strcpy( m_pData, init ); 
}

int String::LoadString(UINT nID)
{
 	AFX_MANAGE_STATE(AfxGetAppModuleState());
   
   TCHAR szTemp[256];

	int nLength = AfxLoadString(nID, szTemp, 256);

   if (nLength)
		*this = szTemp;
   else
      *this = (String)"Error Loading String: " + (ULONG)nID;      

   return nLength;
}

String String::GetResourceString(UINT nID)
{
 	AFX_MANAGE_STATE(AfxGetAppModuleState());
   
   TCHAR szTemp[256];

	int nLength = AfxLoadString(nID, szTemp, 256);
   
	if (nLength)
		return szTemp;
   else
      return (String)"Error Loading String: " + (ULONG)nID;      
}

String &String::operator=(const String &other )
{
   if (this == &other)
      return *this;
   if (other.Get() == NULL) {
      Empty();
      return *this;
   }
   int length = (int)strlen( other.Get() ); 
   Empty();
   if (m_pData = Alloc(( length+1 )))
      strcpy( m_pData, other.Get() );
   return *this;
}

void String::operator+=(const String &addition)
{
   int length;
   char *temp;
   
   if (addition.Get() == NULL)
      return;

   if (m_pData) {
      length = (int) (strlen( m_pData ) + strlen( addition.Get() ));
      temp = Alloc(( length+1 ));
      strcpy( temp, m_pData );
   } else {
      length = (int)strlen( addition.Get() );
      temp = Alloc(( length+1 ));
   }

   strcat( temp, addition.Get() );

   Empty();
   m_pData = temp;
}

String String::Left( int count ) const
{
   String temp( *this ), result;

   int max = (int)Length();
   if (count > max)
      count = max;
   temp.m_pData[count] = '\0';
   result = temp;
   return result;
}   

String String::Right( int count ) const
{
   String result( &m_pData[Length()-count] );
   return result;
}   

String String::Mid( int start, int count ) const
{
   ASSERT(start>=0);
   ASSERT(count>0);
   int safestart = min((UINT)start, Length()-1);
   int safecount = min((UINT)count, Length()-safestart);
   String rvalue;
   rvalue.AllocBuf( safecount+1 );
   memcpy(rvalue.m_pData, m_pData+safestart, safecount);
   return rvalue; 
}   

void StringToUpper( String &strString )
{
   if (strString.Get() == NULL)
      return;
   _strupr( strString.Get() );
}

String String::ToUpper() const
{
   String result( *this );
   StringToUpper( result );
   return result;
}   

void StringToLower( String &strString )
{
   if (strString.Get() == NULL)
      return;
   _strlwr( strString.Get() );
}

String String::ToLower() const
{
   String result( *this );
   StringToLower( result );
   return result;
}   

void String::RemoveName()
{
   if (IsEmpty())
      return;

   int nIndex = ReverseFind('\\');

   if (nIndex==-1)
      nIndex = ReverseFind('/');

   if (nIndex!=-1)
      SetAt( nIndex, '\0' );
}

String String::GetPathOnly() const
{
   String strResult( *this );
   strResult.RemoveName();
   return strResult;
}   

String String::GetNameOnly() const
{
   if (IsEmpty())
      return "";

   char *strptr;

   if (strptr = strrchr( Get(), '\\' ))
      ++strptr;
   else if (strptr = strrchr( Get(), '/' ))
      ++strptr;
   else
      strptr = Get();

   return strptr;
}   

String String::GetSuffixOnly() const
{
   if (IsEmpty())
      return "";

   char *strptr;

   String strNameOnly = GetNameOnly();

   if (strptr = strrchr( strNameOnly.Get(), '.' ))
      return ++strptr;
   else
      return "";
}   

String String::GetWithoutSuffix() const
{
   if (IsEmpty())
      return "";

   String strNoSuffix(*this);

   char *ptr;

   if (ptr = strrchr( strNoSuffix.Get(), '.' ))
      *ptr = '\0';

   return strNoSuffix;
}   

String String::GetNameOnlyWithoutSuffix() const
{
   if (m_pData==NULL)
      return "";
   return GetNameOnly().GetWithoutSuffix();
}   

BOOL String::operator < (const String &other ) const 
{
   if (m_pData == NULL && other.Get() == NULL)
      return FALSE;
   if (m_pData == NULL || other.Get() == NULL)
      return (m_pData==NULL);
   return (_stricmp( m_pData, other.Get() ) < 0);
}

BOOL String::operator > (const String &other ) const
{
   if (m_pData == NULL && other.Get() == NULL)
      return FALSE;
   if (m_pData == NULL || other.Get() == NULL)
      return (other.Get()==NULL);
   return (_stricmp( m_pData, other.Get() ) > 0);
}

BOOL String::operator <= (const String &other ) const
{
   return !(*this > other);
}

BOOL String::operator >= (const String &other ) const
{
   return !(*this < other);
}

void String::AppendName( const String &name )
{
   if (!IsEmpty()) {
      if (Right(1) != NATIVESEPARATOR)
         *this += NATIVESEPARATOR;
	}

   *this+=name;
}

static UINT GetFileName(char * lpszPathName, char * lpszTitle, UINT nMax)
{
   // always capture the complete file name including extension (if present)
   char * lpszTemp = lpszPathName;

   for (char *lpsz = lpszPathName; *lpsz != '\0'; lpsz++) {
      // remember last directory/drive separator
      if (*lpsz == '\\' || *lpsz == '/' || *lpsz == ':')
         lpszTemp = lpsz+1;
   }

   // lpszTitle can be NULL which just returns the number of bytes
   if (lpszTitle == NULL)
      return lstrlen(lpszTemp)+1;

   // otherwise copy it into the buffer provided
   lstrcpyn(lpszTitle, lpszTemp, nMax);
   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// String = C:\MYAPP\DEBUGS\C\TESWIN.C
//
// cchMax   b   Result
// ------   -   ---------
//  1- 7    F   <empty>
//  1- 7    T   TESWIN.C
//  8-14    x   TESWIN.C
// 15-16    x   C:\...\TESWIN.C
// 17-23    x   C:\...\C\TESWIN.C
// 24-25    x   C:\...\DEBUGS\C\TESWIN.C
// 26+      x   C:\MYAPP\DEBUGS\C\TESWIN.C

String String::GetAbbreviated(int cchMax, BOOL bAtLeastName) const
{
   int cchFullPath, cchFileName, cchVolName;
   const char* lpszCur;
   const char* lpszBase;
   const char* lpszFileName;
   char canon[255], sepchar[2];
   char *lpszCanon = canon;
   String rstrString;
   
   lstrcpy(lpszCanon, m_pData);

   lpszBase = lpszCanon;
   cchFullPath = lstrlen(lpszCanon);

   cchFileName = GetFileName(lpszCanon, NULL, 0) - 1;
   lpszFileName = lpszBase + (cchFullPath-cchFileName);

   // If cchMax is more than enough to hold the full path name, we're done.
   // This is probably a pretty common case, so we'll put it first.
   if (cchMax >= cchFullPath) {
      rstrString = lpszCanon;
      return rstrString;
   }

   // If cchMax isn't enough to hold at least the basename, we're done
   if (cchMax < cchFileName) {
      lstrcpy(lpszCanon, (bAtLeastName) ? lpszFileName : "");
      rstrString = lpszCanon;
      return rstrString;
   }

   // Calculate the Length of the volume name.  Normally, this is two characters
   // (e.g., "C:", "D:", etc.), but for a UNC name, it could be more (e.g.,
   // "\\server\share").
   //
   // If cchMax isn't enough to hold at least <volume_name>\...\<base_name>, the
   // result is the base filename.

   lpszCur = lpszBase + 2;                 // Skip "C:" or leading "\\"

   if (lpszBase[0] == NATIVESEPARATOR && lpszBase[1] == NATIVESEPARATOR) { // UNC pathname
      // First skip to the '\' between the server name and the share name,
      while (*lpszCur != NATIVESEPARATOR)
         lpszCur++;
   }
   // if a UNC get the share name, if a drive get at least one directory
   // make sure there is another directory, not just c:\filename.ext

   const char *temp = lpszCur;
   if (cchFullPath - cchFileName > 3) {
      temp++;
      while (*temp != NATIVESEPARATOR)
         temp++;
   }
   
   cchVolName = (int)(temp - lpszBase);

   if (cchMax >= cchVolName + 5 + cchFileName)
	   if (cchFullPath - cchFileName > 3) {
	      lpszCur++;
	      while (*lpszCur != NATIVESEPARATOR)
	         lpszCur++;
	   }

   cchVolName = (int)(lpszCur - lpszBase);
   if (cchMax < cchVolName + 5 + cchFileName) {
      if (!(lpszBase[0] == NATIVESEPARATOR && lpszBase[1] == NATIVESEPARATOR)) { // not UNC pathname
         if (cchMax < 7 + cchFileName) {
            lstrcpy(lpszCanon, lpszFileName);
            rstrString = lpszCanon;
            return rstrString;
         }
         else {
            lpszCanon[3] = '\0';
            lstrcat(lpszCanon, "...");
            sepchar[0] = NATIVESEPARATOR;
            sepchar[1] = '\0'; 
            lstrcat(lpszCanon, sepchar);
            lstrcat(lpszCanon, lpszFileName);
            rstrString = lpszCanon;
            return rstrString;
         }
      }
      lstrcpy(lpszCanon, lpszFileName);
      rstrString = lpszCanon;
      return rstrString;
   }

   // Now loop through the remaining directory components until something
   // of the form <volume_name>\...\<one_or_more_dirs>\<base_name> fits.
   //
   // Assert that the whole filename doesn't fit -- this should have been
   // handled earlier.

   while (cchVolName + 4 + (int)lstrlen(lpszCur) > cchMax) {
      do {
         lpszCur++;
      } while (*lpszCur != NATIVESEPARATOR);
   }

   // Form the resultant string and we're done.
   lpszCanon[cchVolName] = '\0';
   sepchar[0] = NATIVESEPARATOR;
   sepchar[1] = '\0'; 
   lstrcat(lpszCanon, sepchar);
   lstrcat(lpszCanon, "...");
   lstrcat(lpszCanon, lpszCur);

   rstrString = lpszCanon;
   return rstrString;
}

String String::GetAbbreviated(CWnd *wnd) const
{
   if (!wnd)
      return *this;

   CDC   *dc = wnd->GetDC();

   CRect  rcClient;
   wnd->GetClientRect( &rcClient );

   int nMaxWidth = rcClient.Width()+1;
   
   String strString = *this;

   while (dc->GetTextExtent(strString.Get()).cx > nMaxWidth && strString != GetNameOnly())
      strString = GetAbbreviated( strString.Length()-1 );

   return strString;
}

int String::Find(char ch) const
{
   if (m_pData == NULL)
      return -1;
   
   char *ptr;
   if ((ptr = strchr(m_pData, ch)) == NULL)
      return -1;
   return (int)(ptr-m_pData);
}

int String::FindNotInQuotes( char find ) const
{
   if (m_pData == NULL)
      return -1;

   BOOL bInQuotes = FALSE;
   int nLength = Length();

   for (int i=0; i<nLength; i++)
   {
      if (m_pData[i] == '"')
         bInQuotes = !bInQuotes;

      if ((m_pData[i] == find) && !bInQuotes)
         return i;
   }

   return -1;
}

int String::Find(int startindex, char ch) const
{
   char *ptr;
   if (m_pData==NULL)
      return -1;
   
   if ((ptr = strchr(m_pData+startindex, ch)) == NULL)
      return -1;
   return (int)(ptr-m_pData);
}

int String::Find(const String &find) const
{
   char *ptr;

   if (m_pData==NULL || find.m_pData==NULL)
      return -1;

   String lower = ToLower();
   
   if ((ptr = strstr(lower.m_pData, find.ToLower().Get())) == NULL)
      return -1;

   return (int)(ptr-lower.m_pData);
}

int String::Find(int startindex, const String &find) const
{
   char *ptr;
   
   if (m_pData==NULL)
      return -1;
   
   String lower = ToLower();
   
   if ((ptr = strstr(lower.m_pData+startindex, find.ToLower().Get())) == NULL)
      return -1;
   
   return (int)(ptr-lower.m_pData);
}

int String::ReverseFind(char ch) const
{
   String lower = ToLower();

	// find last single character
	LPTSTR lpsz = _tcsrchr(lower.m_pData, (_TUCHAR) ch);

	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - lower.m_pData);
}

BOOL String::Replace( char find, char replace, BOOL bIncludeThoseFoundInQuotes/*=TRUE*/ )
{
   BOOL bInQuotes = FALSE, bReplaced = FALSE;
   int nLength = Length();

   for (int i=0; i<nLength; i++)
   {
      if (!bIncludeThoseFoundInQuotes && m_pData[i] == '"')
         bInQuotes = !bInQuotes;

      if ((m_pData[i] == find) && (bIncludeThoseFoundInQuotes || !bInQuotes))
      {
         m_pData[i] = replace;
         bReplaced = TRUE;
      }
   }

   return bReplaced;
}

BOOL String::Replace(const String &find, const String &replace)
{
   String newstrString;
   int start, pos=0;
   BOOL bReplaced = FALSE;

   while ((start = Find(pos, find))!=-1)
   {
      bReplaced = TRUE;
      newstrString = Left(start) + replace + Right(Length()-(find.Length()+start));
      pos = start + replace.Length();
      Empty();
      if (m_pData = Alloc(( newstrString.Length()+1 )))
         strcpy( m_pData, newstrString.m_pData );
   }

   return bReplaced;
}

void String::RemoveChar( char find )
{
   char *pEnd = Get() + Length();

   for (char *pSrc=Get(), *pDst=pSrc; pSrc && pSrc<=pEnd; pSrc++)
   {
      if (*pSrc != find)
         *pDst++ = *pSrc;
   }
}

int String::GetLineCount() const
{
   int nLineCount = 1;
   char *pEnd = Get() + Length();

   for (char *pSrc=Get(); pSrc && pSrc<pEnd-2; pSrc++)
   {
      if (pSrc[0]=='\n' || (pSrc[0]=='{' && pSrc[1]=='n' && pSrc[2]=='}'))
         nLineCount++;
   }

   return nLineCount;
}

void String::RemoveWhiteSpace()
{
   RemoveChar( ' ' );

   //if (IsEmpty())
   //   return;

   //int nLength = Length();
   //int nIndex = 0;

   //char *pBuf = Alloc(nLength+1);

   //for (int i=0; i<nLength; i++) {
   //   if (m_pData[i]!= ' ')
   //      pBuf[nIndex++] = m_pData[i];
   //}
   //Empty();
   //m_pData = pBuf;
}

StringList *String::Split(char separator) const
{
   StringList *list = SNEW StringList;
   String filters = *this;

   while (TRUE) {
      int pos = filters.Find(separator);
      if (pos==-1) {
         list->AddTail(filters);
         break;
      }
      if (pos)
         list->AddTail(filters.Left(pos));
      int count = filters.Length()-pos-1;
      if (count<=0)
         break;   
      filters=filters.Right(count);
   }
   return list;
}

StringTokenList *String::Split( char *separators, int nSeperatorCount ) const
{
   StringTokenList *pList = SNEW StringTokenList;

   int nLength = Length();
   int nStart = 0;
   BOOL bInQuotes = FALSE;
   BOOL bHasQuotes = FALSE;

   for (int i=0; i<nLength; i++)
   {
      if (m_pData[i] == '"')
      {
         bInQuotes = !bInQuotes;
         bHasQuotes = TRUE;
      }

      if (bInQuotes)
         continue;

      for (int j=0; j<nSeperatorCount; j++)
         if (m_pData[i] == separators[j])
            break;

      if (j == nSeperatorCount) // didn't find separator
         continue;

      String strSplit( &m_pData[nStart], i-nStart );
      if (bHasQuotes)
         strSplit.Replace( "\"", "" ); // could be faster if we had a version that took char *'s
      pList->AddTail( StringToken(strSplit, separators[j]) );

      nStart = i+1;
   }

   String strSplit( &m_pData[nStart], i-nStart );
   if (bHasQuotes)
      strSplit.Replace( "\"", "" ); // could be faster if we had a version that took char *'s
   pList->AddTail( StringToken(strSplit, '\0') );

   return pList;
 }

StringArray *String::SplitToArray(char separator) const
{
   StringList *pList = Split(separator);
   StringArray *pArray = SNEW StringArray;

   int nSize = (int)pList->GetSize();

   if (nSize==0) {
      delete pList;
      return pArray;
   }

   pArray->SetSize(nSize);

   int i=0;
   for (POSITION pos = pList->GetHeadPosition(); pos; i++)
      (*pArray)[i] = pList->GetNext(pos);

   delete pList;
   return pArray;
 }

void String::Split(const char separator,StringArray &array) const
{
   array.RemoveAll();

   String filters = *this;

   while (TRUE) {
      int pos = filters.Find(separator);
      if (pos==-1) {
         array.Add(filters);
         break;
      }
      if (pos)
         array.Add(filters.Left(pos));
      int count = filters.Length()-pos-1;
      if (count<=0)
         break;   
      filters=filters.Right(count);
   }
}

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, String& value)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate) {
      CString strString;
		int nLen = ::GetWindowTextLength(hWndCtrl);
		::GetWindowText(hWndCtrl, strString.GetBufferSetLength(nLen), nLen+1);
		strString.ReleaseBuffer();
      value = strString;
	}
	else if (value.Get())
		SetWindowText(hWndCtrl, value.Get());
}

void AFXAPI DDV_NotEmpty(CDataExchange* pDX, const String &value)
{
	if (pDX->m_bSaveAndValidate && value.IsEmpty())	{

      String prompt;
      prompt.LoadString(IDS_REQUIREDDATAMISSING);

		AfxMessageBox(prompt.Get(), MB_ICONEXCLAMATION);
		pDX->Fail();
	}
}

static String f_strAppFolder;
String UTILITYEXPORT String::GetAppFolder()
{
   if (!f_strAppFolder.IsEmpty())
      return f_strAppFolder;
   char exename[MAXSTRING];

   GetModuleFileName( NULL, exename, MAXSTRING );
   f_strAppFolder = exename;
   f_strAppFolder = f_strAppFolder.GetPathOnly();

   return f_strAppFolder;
}

static String f_strTempFolder;
String UTILITYEXPORT String::GetTempFolder()
{
   if (!f_strTempFolder.IsEmpty())
      return f_strTempFolder;

   TCHAR szPath[_MAX_PATH];

   GetTempPath(_MAX_PATH, szPath);
   f_strTempFolder = szPath;
   
   return f_strTempFolder;
}

String String::GetTempFileName()
{
   String strFolder = String::GetTempFolder();

   TCHAR szTempFileName[_MAX_FNAME];
   ::GetTempFileName(strFolder.Get(), _T("AVA"), 0, szTempFileName);

   return szTempFileName;
}

void String::AppendToAppFolder()
{
   String path = String::GetAppFolder();
   path.AppendName(*this);
   *this = path;
}

StringList *String::FindFiles()
{
   CFileFind finder;

   BOOL bWorking = finder.FindFile(Get());
   if (!bWorking)
      return NULL;

   StringList *list = SNEW StringList;
   while (bWorking)
   {
      bWorking = finder.FindNextFile();
      list->AddTail((LPCTSTR)finder.GetFilePath());
   }

   return list;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, const String &value )
{
   if (bs.IsBinaryStream())
      bs << (UWORD) value.Length();

   bs.Write( value.Get(), value.Length() );

   return bs;
}

BaseStream &operator>>( BaseStream &bs, String &value )
{
   if (bs.IsBinaryStream()) {
      UWORD nLength;
     bs >> nLength;
     value.SetLength(nLength+1);
     bs.Read(value.Get(), nLength);
   }
   else {
      bs.ParseTokenWhiteSpace();
      value = bs.m_tokenarray;
   }

   return bs;
}
