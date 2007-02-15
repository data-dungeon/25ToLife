#include "StdAfx.h"
#include "MemoryStream.h"

MemoryStream::MemoryStream()
{
   Init();
   SetStreamMode(BS_ASCII);
}

MemoryStream::~MemoryStream()
{
   Empty();
}

MemoryStream::MemoryStream( UINT nSize )
{
   Init();
   SetSize(nSize);
   SetStreamMode(BS_ASCII);
}

void MemoryStream::Init()
{
   m_pBuffer = NULL;
   m_nPos = m_nLength = m_nBlockLength = 0;
}

void MemoryStream::SetSize(int nSize)
{   
   Empty();
   m_nBlockLength = nSize;

   m_pBuffer = (char *)SNEW char[m_nBlockLength];         
}

void MemoryStream::ShrinkLength()
{
   ASSERT(m_pBuffer);

   m_nLength = (UINT)strlen( m_pBuffer );
}

void MemoryStream::Empty()
{
   if (m_pBuffer)
     delete m_pBuffer;

   Init();
}

BOOL MemoryStream::Open( UINT nStreamMode )
{
   SetStreamMode( nStreamMode );
   
   if ( IsOutputStream()) {
      if (!IsAppendStream())
         Empty();
      else
         m_nPos = m_nLength;
   } 
   else
      m_nPos = 0;

   return TRUE;
}

void MemoryStream::Open( const String &strFileName, UINT nStreamMode )
{
   SetStreamMode( nStreamMode );
  
   if ( !FileExists( strFileName ) ) {
      String msg("Unable to open file [%1], file does not exist.");
      msg.Replace("%1", strFileName);
      THROW_FATAL_ERROR(msg);
   }
   m_strFileName = strFileName;

   CStdioFile cfile( m_strFileName.Get(), CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary );
   m_nFileSize = (UINT)cfile.GetLength();
   
   SetSize(m_nFileSize);
   cfile.Read( m_pBuffer, m_nFileSize );
   m_nLength = m_nFileSize;
}

BOOL MemoryStream::Open( char *szInitBuf, UINT nCount, UINT nStreamMode )
{
   SetStreamMode( nStreamMode );
   
   if ( IsAppendStream() )
      m_nPos = m_nLength;
   else
      Empty();

   Write( szInitBuf, nCount );
   
   if ( !IsAppendStream() )
      m_nPos = 0;
   
   return TRUE;
}

void MemoryStream::Write(const void *pBuffer, UINT nCount )
{
   if ( m_nPos + nCount > m_nBlockLength ) {
      if (nCount > 32768)
         m_nBlockLength += nCount;
      else   
         m_nBlockLength += 32768;

      char *pTempBuffer = SNEW char[m_nBlockLength];

	   if (m_pBuffer) {
         CopyMemory( pTempBuffer, m_pBuffer, m_nPos );
         delete m_pBuffer;
	   }
      m_pBuffer = pTempBuffer;
   }

   CopyMemory( &m_pBuffer[m_nPos], (const char *)pBuffer, nCount );

   m_nPos += nCount;

   if (m_nPos > m_nLength)
      m_nLength = m_nPos;
}

UINT MemoryStream::Read( void *pOutBuffer, UINT nCount )
{
   if ( nCount > m_nLength - m_nPos )
      nCount = m_nLength - m_nPos;

   CopyMemory( pOutBuffer, &m_pBuffer[m_nPos], nCount );
   m_nPos += nCount;

   return nCount;
}


int MemoryStream::ParseToken( char cDelimit )
{
   UINT nCount = 0;
   BOOL bDel;
   
   while ( !Eof() && m_pBuffer[m_nPos] == cDelimit )
      m_nPos++;

   if (Eof())
      return 0;   
   
   do {
      m_tokenarray[nCount] = m_pBuffer[m_nPos++];
      bDel = (m_tokenarray[nCount++] == cDelimit);
   } while ( !bDel && !Eof() && nCount < GetTokenSize()-1 );

   if (bDel)
      m_tokenarray[--nCount] = '\0';
   else
      m_tokenarray[nCount] = '\0';
   
   while ( !Eof() && m_pBuffer[m_nPos] == cDelimit )
      m_nPos++;

   return nCount;
}

BOOL MemoryStream::ParseTokenWhiteSpace()
{
   while ((m_pBuffer[m_nPos]==' ' || m_pBuffer[m_nPos]=='\x09' || m_pBuffer[m_nPos]=='\x0A' || m_pBuffer[m_nPos]=='\x0D') && !Eof())
      m_nPos++;

   if (Eof())
      return FALSE;

   BOOL bInQuotes=FALSE, bHasQuotes=FALSE;
   int nStart = m_nPos;
   while (m_pBuffer[m_nPos]!='\x09' && m_pBuffer[m_nPos]!='\x0A' && m_pBuffer[m_nPos]!='\x0D' && !Eof())
   {
      if (m_pBuffer[m_nPos]=='"')
      {
         bInQuotes = !bInQuotes;
         bHasQuotes = TRUE;
      }
      else if (!bInQuotes && m_pBuffer[m_nPos]==' ')
         break;

      m_nPos++;
   }

   UINT nLen = m_nPos - nStart;
   if (nLen+1 > GetTokenSize())
      SetTokenSize(nLen+1);
   memcpy( m_tokenarray, &m_pBuffer[nStart], nLen );
   m_tokenarray[nLen] = '\0';
   //if (bHasQuotes)
   //{
   //   String strQuoteString( m_tokenarray );
   //   strQuoteString.Replace( "\"", "" );
   //   strcpy( m_tokenarray, strQuoteString.Get() );
   //}

   while ((m_pBuffer[m_nPos]==' ' || m_pBuffer[m_nPos]=='\x09' || m_pBuffer[m_nPos]=='\x0A' || m_pBuffer[m_nPos]=='\x0D') && !Eof())
      m_nPos++;

   return TRUE;
}

BOOL MemoryStream::ReadLine( String &strLine ) 
{
   if (Eof())
      return 0;   
   
   if (IsBinaryStream()) {
      UWORD nLen;
     *this >> nLen;
     strLine.SetLength( nLen );
     Read( strLine.Get(), nLen);
     return nLen != 0;
   }
   else {
      int nStart = m_nPos;
      while (m_pBuffer[m_nPos]!='\x0A' && m_pBuffer[m_nPos]!='\x0D' && !Eof())
         m_nPos++;

      int nLen = m_nPos - nStart;
      if (nLen)
         strLine.Set( &m_pBuffer[nStart], nLen );

      while ((m_pBuffer[m_nPos]=='\x0A' || m_pBuffer[m_nPos]=='\x0D') && !Eof())
         m_nPos++;

      if ( m_pBuffer[m_nPos]=='\x0D' && !Eof())
         m_nPos++;

      return nLen != 0;
   }   
}

BOOL MemoryStream::ReadLine( MemoryStream &stream )
{
   static char delims[] = "\x0A\x0D";

   stream.Seek(0, ST_BEGINNING);

   if (Eof())
      return FALSE;

   int nStart = m_nPos;
   while (m_pBuffer[m_nPos]!='\x0A' && m_pBuffer[m_nPos]!='\x0D' && !Eof())
      m_nPos++;

   UINT nLen = m_nPos - nStart;
   if (nLen+1 > stream.GetBlockLength())
      stream.SetSize( nLen+1 );

   if (nLen)
   {
      memcpy( stream.GetBuffer(), &m_pBuffer[nStart], nLen );
      stream.GetBuffer()[nLen] = '\0';
      stream.ShrinkLength();
   }

   while ((m_pBuffer[m_nPos]=='\x0A' || m_pBuffer[m_nPos]=='\x0D') && !Eof()) // Can't do this for NULL strings kills binary sockets (Dan)
      m_nPos++;

   if ( m_pBuffer[m_nPos]=='\x0D' && !Eof() )
      m_nPos++;

   if (stream.GetLength())
      return TRUE;
   else
      return FALSE;
}

void MemoryStream::Seek( int nPos, SeekType seektype )
{
   switch (seektype) {
   case ST_BEGINNING:
      m_nPos = nPos;
      break;
   case ST_OFFSET:
      m_nPos += nPos;
      break;
   case ST_END:
      m_nPos = m_nLength - nPos;
      break;
   }
}

BOOL BaseStream::SkipHeader(Header &header)
{
   MemoryStream ms( 1024 );
   Header readheader;
   LONG pos = GetPos();
   int count = 0;

   if (IsAsciiStream()) {
      String match = (String)"END"+header.string;
      
      while (ReadLine( ms )) {
         if (!ms.IsHeader())
            continue;
         ms >> readheader;
         if (strcmp(readheader.string, header.string)==0) {
            count++;
            continue;
         }
         else if (_stricmp(match.Get(), readheader.string)==0) {
            if (count == 0)
               return TRUE;
            count--;
         }
      }
      Seek(pos, ST_BEGINNING);
   }
   return FALSE;   
}

