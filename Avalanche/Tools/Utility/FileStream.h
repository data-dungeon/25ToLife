#pragma once
#include "BaseStream.h"

class UTILITYEXPORT FileStream : public BaseStream
{
private:
   CStdioFile m_file;
   String m_strTempFileName;

public:
// Constructors
            FileStream();
   virtual ~FileStream();

// Operations
   catch_msg void Open( const String &strFileName, UINT nStreamMode = BS_OUTPUT|BS_ASCII ); // no return value, throws String exceptions
   UINT GetLength() {return m_nFileSize;}

// BaseStream Overrides (pure)
   virtual BOOL Eof();
   virtual int  GetEOLSize() const { return 2; }
   virtual UINT Read(void *pOutBuffer, UINT nCount );
   virtual void Write(const void *pBuffer, UINT nCount );
   virtual void Flush();
   virtual void Close();
   virtual BOOL ReadLine( String &strLine );
   virtual BOOL ReadLine( MemoryStream &stream );
   virtual void Seek( int nPos, SeekType seektype );
   virtual UINT GetPos();
   virtual BOOL ParseTokenWhiteSpace(); // fills m_tokenarray
   virtual int ParseToken(char cDelimit); // fills m_tokenarray
};
