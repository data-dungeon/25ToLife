#pragma once
#include "BaseStream.h"

class UTILITYEXPORT MemoryStream : public BaseStream
{
private:
   char *m_pBuffer;
   UINT m_nPos;
   UINT m_nLength;
   UINT m_nBlockLength;

public:
// Constructors

            MemoryStream();
            MemoryStream( UINT nSize );
            MemoryStream( UINT nStreamMode, BOOL bUnused ) { Init(); Open(nStreamMode); } //Unused is only to make the two constructors different
            MemoryStream( MemoryStream &ms ) { Init(); Open( ms.GetBuffer(), ms.GetLength(), ms.GetStreamMode() ); }
   virtual ~MemoryStream();

// Operators
   MemoryStream &operator = ( const MemoryStream &other ) {
      Empty();
      Open( other.GetBuffer(), other.GetLength(), other.GetStreamMode() );
      return *this;
   }

// Operations
   void Init();
   void SetSize(int nSize);
   void ShrinkLength();

   BOOL Open( UINT nStreamMode );
   void Open( const String &strFileName, UINT nStreamMode = BS_OUTPUT|BS_ASCII ); // DEFAULT IS 0, WHICH IS STORING, ASCII NOBYTESWAP, NOTAPPENDING
   BOOL Open( char *pInitBuffer, UINT nCount, UINT nStreamMode );
   void Empty();
   UINT GetLength() const { return m_nLength; }
   UINT GetBlockLength() const { return m_nBlockLength; }
   char *GetBuffer() const { return m_pBuffer; }

   BOOL IsLabel()
   {
      for (char *ch=m_pBuffer+m_nPos; ch < m_pBuffer+m_nLength-1 && *ch != '='; ch++);
      return *ch == '=';
   }
   BOOL IsHeader()
   {
      return (m_pBuffer[m_nPos] == '[');
   }
   BOOL IsHeaderAt0()
   {
      return (m_pBuffer[0] == '[');
   }
   BOOL IsEndHeaderAt0()
   {
      return m_nLength>=4 && m_pBuffer[0]=='[' && m_pBuffer[1]=='E' && m_pBuffer[2]=='n' && m_pBuffer[3]=='d';
   }

// BaseStream Overrides (pure)
   virtual BOOL Eof() { return m_nPos >= m_nLength; }
   virtual int  GetEOLSize() const { return 1; }
   virtual UINT Read(void *pBuf, UINT nCount );
   virtual void Write(const void *pBuf, UINT nCount );
   virtual void Flush() {}
   virtual void Close() {}
   virtual BOOL ReadLine( String &strLine ); 
   virtual BOOL ReadLine( MemoryStream &stream );
   virtual void Seek( int nPos, SeekType seektype );
   virtual UINT GetPos() { return m_nPos; }
   virtual BOOL ParseTokenWhiteSpace(); // fills m_tokenarray
   virtual int ParseToken(char cDelimit); // fills m_tokenarray
};
