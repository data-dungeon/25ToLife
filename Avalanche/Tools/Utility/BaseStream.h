#pragma once

#ifndef UTILITYEXPORT
#define UTILITYEXPORT
#endif

#include "AString.h"
#include "ADataType.h"
#include "Files.h"

class MemoryStream;
class Vector2;
class Vector;
class Quaternion;
class Time;

inline char *STRCHR(const char *szString, int c) // use this to not delimit NULL
{
   if (c == '\x00')
      return NULL;

   return strchr((char *)szString, c);
}

class CharPtr
{
public:
   char *m_buf;
   
   CharPtr() {
      m_buf = NULL;
   }
   
   CharPtr(const CharPtr &other)
   {
      m_buf = other.m_buf;
   }

   CharPtr(char *buf)
   {
      m_buf = buf;
   }
   
   int operator == (const CharPtr &other) const
   {
      return !strcmp(m_buf, other.m_buf);
   }
   
   CharPtr &operator = (const CharPtr &other)
   {
      m_buf = other.m_buf;
      return *this;
   }
};

class LabelArray
{
public:
   LONG    id;
   CharPtr text;
};

class UTILITYEXPORT Label {
public:
   LONG   id;
   char   string[80];
   
   Label() {}
   Label( LONG id );
   Label( char *text );
   char *GetText() { return string; }
};

class UTILITYEXPORT Header {
public:
   LONG   id;
   char   string[80];
   
   Header() {}
   Header( LONG id );
   Header( LONG pid, const char *buffer )
   {
      id = pid;
      strcpy( string, buffer );
   }
   int operator == ( const Header &header ) const { return (id == header.id); }
   char *GetText() { return string; }
};

class UTILITYEXPORT StreamBOOL {
public:
   BOOL m_bool;

   StreamBOOL() {}
   StreamBOOL( BOOL pbool ) { m_bool = pbool; }
};

void UTILITYEXPORT SetLabelArray( LabelArray array[], UINT tablesize, UINT hashsize );
void UTILITYEXPORT SetHeaderArray( LabelArray array[], UINT tablesize, UINT hashsize );

LONG GetLabelIDFromString( char *string );
LONG GetHeaderIDFromString( char *string );
BOOL GetLabelStringFromID( LONG pid, char *string );
void GetHeaderStringFromID( LONG pid, char *string );

class UTILITYEXPORT BaseStream
{
protected:
   UINT m_nFileSize;
   UINT m_nStreamMode;
public:
   int m_nMargin;
   char *m_tokenarray; // This is a member for threading reasons. It exist for speed.  It is allocated to MAXSTRING on thread construction
   UINT m_nTokenSize; 
   float m_fProductVersion;

   enum { BS_OUTPUT = 0x00000000,//Default
          BS_ASCII  = 0x00000000,//Default       
          BS_INPUT  = 0x00000001,
          BS_BINARY = 0x00000002,
          BS_APPEND = 0x00000004,
   };
   enum SeekType { ST_BEGINNING, ST_END, ST_OFFSET };

   String m_strFileName;

// Constructors
            BaseStream();
   virtual ~BaseStream();

// Operations
   BOOL IsInputStream() const { return m_nStreamMode & BS_INPUT; }
   BOOL IsOutputStream() const { return !IsInputStream(); }
   BOOL IsAppendStream() const { return m_nStreamMode & BS_APPEND; } 

   BOOL IsBinaryStream() const { return m_nStreamMode & BS_BINARY; }
   BOOL IsAsciiStream() const { return !IsBinaryStream(); } 

   UINT GetStreamMode() const { return m_nStreamMode; }
   void SetStreamMode(UINT nStreamMode) { m_nStreamMode = nStreamMode; }

   void GetToken(String &strOutToken) { strOutToken = m_tokenarray; }
   void SetTokenSize(UINT nSize);
   UINT GetTokenSize() { return m_nTokenSize; }

   void WriteMargin();
   void ReadMargin();
   BOOL SkipHeader( Header &header );

// Overridables (Pure)
   virtual BOOL Eof() = 0;
   virtual int  GetEOLSize() const = 0;
   virtual UINT Read(void *pOutBuffer, UINT nCount) = 0;
   virtual void Write(const void *pBuffer, UINT nCount) = 0;
   virtual void Flush() = 0;
   virtual void Close() = 0;
   virtual BOOL ReadLine( String &strLine ) = 0;
   virtual BOOL ReadLine( MemoryStream &stream ) = 0;
   virtual void Seek( int nPos, SeekType seektype ) = 0;
   virtual UINT GetPos() = 0;
   virtual BOOL ParseTokenWhiteSpace() = 0; // fills m_tokenarray
   virtual int ParseToken(char cDelimit) = 0; // fills m_tokenarray

   BaseStream& operator<<( BaseStream &(__cdecl * function)(BaseStream &))
    { (* function )( *this ); return *this; }
};

UTILITYEXPORT BaseStream &mar( BaseStream &bs );
UTILITYEXPORT BaseStream &sp( BaseStream &bs );
UTILITYEXPORT BaseStream &spb( BaseStream &bs );
UTILITYEXPORT BaseStream &nl( BaseStream &bs );
UTILITYEXPORT BaseStream &tab( BaseStream &bs );
UTILITYEXPORT BaseStream &tabb( BaseStream &bs );

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, SBYTE value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, SBYTE &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, UBYTE value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, UBYTE &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, SWORD value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, SWORD &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, UWORD value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, UWORD &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, LONG value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, LONG &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, ULONG value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, ULONG &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, float value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, float &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, double value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, double &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, int value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, int &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, UINT value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, UINT &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, char value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, char &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, char *value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, const Time &time);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, Time &time);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, const Vector2 &value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, Vector2 &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, const Vector &value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, Vector &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, const Quaternion &q);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, Quaternion &q);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, const Label &value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, Label &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, const Header &value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, Header &value);

UTILITYEXPORT BaseStream &operator << (BaseStream &bs, const StreamBOOL &value);
UTILITYEXPORT BaseStream &operator >> (BaseStream &bs, StreamBOOL &value);
