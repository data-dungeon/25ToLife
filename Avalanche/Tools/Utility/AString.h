#pragma once

#ifndef UTILITYEXPORT
#define UTILITYEXPORT
#endif

#include "AMacros.h"
#include "ATime.h"
#include <afxtempl.h>

#define MAXSTRING 255
#define NATIVESEPARATOR '\\' 
//#define NATIVESEPARATOR '/' 

class StringList;
class StringArray;
class Vector;
class StringTokenList;

class UTILITYEXPORT String {
private:
   char *m_pData;

public:
   String() { m_pData = NULL; }
   String( const String & );
   String( const char * );
   String( char );
   String( short number );
   String( ULONG );
   String( UINT );
   String( LONG );
   String( float );
   String( double );
   String( double, double precision );
   String( int );
   String( char, int nCount );
   String( const char *, int nCount );
   String( const Vector & );
   void Set(const char *other, int nLen );
   ~String() { Empty(); }
   char *Get() const { return m_pData; }
   char operator [] ( int nIndex ) const { return m_pData[nIndex]; }
   char &operator [] ( int nIndex ) { return m_pData[nIndex]; }
   char *GetAt( UINT nIndex ) const { return &m_pData[nIndex]; }
   void SetAt( UINT nIndex, char c ) const { m_pData[nIndex] = c; } 

   char *Alloc(size_t size) const {	return (char *)calloc(size, sizeof(char)); }
   BOOL AllocBuf(size_t size) { m_pData = Alloc( size ); return m_pData != NULL; }
   BOOL IsEmpty() const { return (m_pData == NULL) || (*m_pData=='\0'); }
   void Empty() { if (m_pData) free(m_pData); m_pData = NULL; }

   BOOL operator==( const String & ) const;
   BOOL operator!=( const String &other ) const { return !(*this == other); }
   String &operator=( const String & );  
   String &operator=( const char * );  
   void operator+=( const String & );
   
   UINT Length() const;
   void SetLength( UINT nLength ) { Empty(); m_pData = Alloc( nLength+1 ); m_pData[nLength] = '\0';}

   String Left( int count ) const;
   String Right( int count ) const;
   String Mid( int start, int count ) const;
   String ToUpper() const;
   String ToLower() const;
   String GetPathOnly() const;
   String GetNameOnly() const;
   String GetSuffixOnly() const;
   String GetWithoutSuffix() const;
   String GetNameOnlyWithoutSuffix() const;
   String GetAbbreviated( int cchMax, BOOL bAtLeastName = TRUE ) const;
   String GetAbbreviated( CWnd *window ) const;
   String GetWithoutIntOnEnd() const;
   int    GetIntOnEnd() const;
   float  GetCM() const;
   Time   GetTime( double fps, Time::TimeUnit timeunits, BOOL treatasdelta=FALSE ) const;
   void   RemoveWhiteSpace();
   void   RemoveName();
   void   RemoveChar( char find );
   int    GetLineCount() const;

   long AsInt() const;
   unsigned long AsUnsigned() const;
   float AsFloat() const;
   bool AsBool() const;
   double AsDouble() const;

   bool StripIntOnEnd(int &endint,String &substr) const;

   int LoadString(UINT id);
   static String GetResourceString(UINT id);

   BOOL operator < ( const String & ) const;
   BOOL operator > ( const String & ) const;
   BOOL operator <= ( const String & ) const;
   BOOL operator >= ( const String & ) const;

   friend String operator + ( const String &, const String & );

   int Find(char ch) const;               // like "C" strchr -1 if not found
   int FindNotInQuotes( char find ) const;  // like "C" strchr -1 if not found
   int Find(int startindex, char ch) const;
   int Find(const String &sub) const;     // like "C" strstr
   int Find(int startindex, const String &sub) const;     // like "C" strstr
   int ReverseFind(char ch) const;
   void Format(const String &format, ...);
   BOOL Replace(char find, char replace, BOOL bIncludeThoseFoundInQuotes=TRUE);
   BOOL Replace(const String &find, const String &replace);
 
   StringArray *SplitToArray(char separator) const;
   StringList *Split(char separator) const;
   StringTokenList *Split( char *separators, int nSeperatorCount ) const;
   StringList *FindFiles();
   void Split(const char separator,StringArray &array) const;

   String StripTabs();
   String StripQuotes();

   void AppendName( const String &name );
   void AppendToAppFolder();

   static String GetAppFolder();
   static String GetTempFolder();
   static String GetTempFileName();
   static String GetOsFolder();
};

class BaseStream;
UTILITYEXPORT BaseStream &operator << (BaseStream &bs, const String &value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, String &value);

class StringToken
{
public:
   String m_strValue;
   char   m_cToken;

   StringToken() {}
   StringToken( const String &strValue, char cToken)
   {
      m_strValue = strValue;
      m_cToken = cToken;
   }
};

class StringTokenList : public CList<StringToken>
{
public:
};

class StringList : public CList<String>
{
public:
};

class StringArray : public CArray<String>
{
public:
   String Combine(char c,int start,int stop);
};

enum { UNIT_NONE, UNIT_CENTIMETERS, UNIT_METERS, UNIT_FEET, UNIT_INCHES, UNIT_FEETANDINCHES, UNIT_PERCENTAGE, UNIT_ANGLE };
UTILITYEXPORT int GetMeasurementUnits();
UTILITYEXPORT void SetMeasurementUnits( int pmeasurementunits );
UTILITYEXPORT CString UnitToString( float cm, int pmeasurementunits=-1, float modulo=.01f );
UTILITYEXPORT void AFXAPI DDX_Unit(CDataExchange *pDX, int nIDC, float &cm);
UTILITYEXPORT void AFXAPI DDV_MinMaxUnit(CDataExchange* pDX, float const& value, float minVal, float maxVal);

#include "AString.hpp"


