#include "StdAfx.h"
#include "FileStream.h"
#include "MemoryStream.h"

FileStream::FileStream()
{
}

FileStream::~FileStream()
{
   Close();
}

void FileStream::Open( const String &strFileName, UINT nStreamMode/* = BS_OUTPUT|BS_ASCII*/ )
{
   SetStreamMode(nStreamMode);
   
   if ( IsOutputStream()) {
      if (FileExists( strFileName )) {
         CFileStatus status;
         CFile::GetStatus( strFileName.Get(), status );
         if (status.m_attribute & CFile::readOnly) {
            String msg;
            msg.LoadString( AFX_IDP_FAILED_ACCESS_WRITE );
            msg.Replace( "%1", strFileName );
            THROW_FATAL_ERROR(msg);
         }
      }

      m_strTempFileName = String::GetTempFileName();

      CFileException e;
      UINT flags = 0;
      if (IsAppendStream()) {
         flags |= CFile::modeNoTruncate;

         if (FileExists( m_strTempFileName ))
            DeleteFile(m_strTempFileName);

         if (FileExists( strFileName ))
            CopyFile(strFileName.Get(), m_strTempFileName.Get(), FALSE);
      }
      if (!m_file.Open( m_strTempFileName.Get(), flags | CFile::modeCreate | CFile::shareExclusive | CFile::modeWrite | (IsBinaryStream() ? CFile::typeBinary : CFile::typeText), &e )) {
         DeleteFile(m_strTempFileName);
         TCHAR szCause[255];
         e.GetErrorMessage(szCause, 255);
         THROW_FATAL_ERROR(szCause);
      }
   }
   else { // Input Stream
      if (!FileExists( strFileName )) {
         String msg("Unable to open file [%1], file does not exist.");
         msg.Replace("%1", strFileName);
         THROW_FATAL_ERROR(msg);
      }

      if (!m_file.Open( strFileName.Get(), CFile::modeRead | CFile::shareDenyWrite | (IsBinaryStream() ? CFile::typeBinary : CFile::typeText))) {
         String msg("Unable to open file [%1].");
         msg.Replace("%1", strFileName);
         THROW_FATAL_ERROR(msg);
      }

      m_nFileSize = (UINT)m_file.GetLength();
   }
   
   m_strFileName = strFileName;

   if (IsAppendStream())
      Seek(0, BaseStream::ST_END);
}

UINT FileStream::Read( void *pOutBuffer, UINT nCount )
{
   return m_file.Read( (char *)pOutBuffer, nCount );
}

void FileStream::Write(const void *pBuffer, UINT nCount )
{
   m_file.Write( (const char *)pBuffer, nCount );
}

BOOL FileStream::Eof()
{
   return m_file.GetPosition() >= (DWORD)m_nFileSize;
}

void FileStream::Flush()
{
   m_file.Flush();
}

void FileStream::Close()
{
   if (m_file.m_pStream)
      m_file.Close();
   
   if (IsOutputStream()) {
      if (FileExists(m_strTempFileName.Get())) {
         try {
            if (FileExists(m_strFileName))
               DeleteFile(m_strFileName);
            RenameFile(m_strTempFileName, m_strFileName);
         }
         catch( CFileException *e ) {
            e->m_strFileName = m_strFileName.Get();
            e->ReportError();
         }
      }
   }
}

int FileStream::ParseToken( char cDelimit )
{
   int nCount = 0;
   BOOL bDel;
   
   if (Eof())
      return 0;

   do {
      m_file.Read( &m_tokenarray[nCount], 1 );
      bDel = (m_tokenarray[nCount++] == cDelimit);
   } while ( !bDel && !Eof() && nCount < MAXSTRING );

   if ( bDel )
      m_tokenarray[--nCount] = '\0';
   else
      m_tokenarray[nCount] = '\0';

   return nCount;
}

BOOL FileStream::ParseTokenWhiteSpace()
{
   static char delims[] = " \x09\x0A\x0D";
   char *pDel;
   int nCount = 0;
 
   if (Eof())
      return FALSE;
   
   do {
      m_file.Read( &m_tokenarray[nCount], 1 );
      pDel = STRCHR( delims, m_tokenarray[nCount++] );
   } while ( (pDel == NULL) && !Eof() && nCount < MAXSTRING );

   if ( pDel )
      m_tokenarray[nCount-1] = '\0';
   else
      m_tokenarray[nCount] = '\0';
   
   return TRUE;
}

BOOL FileStream::ReadLine( String &strLine ) 
{
   if (IsBinaryStream()) {
      UWORD nLen;
      *this >> nLen;
      strLine.SetLength( nLen );
      Read( strLine.Get(), nLen );
   }
   else {
      char buf[MAXSTRING];
   
      if ( !m_file.ReadString( buf, MAXSTRING ))
         return FALSE;

      UINT nLen = (UINT)strlen(buf) - 1;

      if ( buf[nLen] == '\x0D' )
         buf[nLen] = '\0';
      
      strLine = buf;
   }
   return TRUE;
}

BOOL FileStream::ReadLine( MemoryStream &stream )
{
   stream.Seek(0, ST_BEGINNING);

   if (!m_file.ReadString( stream.GetBuffer(), stream.GetBlockLength() ))
      return FALSE;

   stream.ShrinkLength();
   
   if (stream.GetLength() == 0)
      return FALSE;

   return TRUE;
}

void FileStream::Seek( int nOffset, SeekType seektype )
{
   switch (seektype) {
   case ST_BEGINNING:
      m_file.Seek( nOffset, CFile::begin );
      break;
   case ST_OFFSET:
      m_file.Seek( nOffset, CFile::current );
      break;
   case ST_END:
      m_file.Seek( -nOffset, CFile::end );
      break;
   }
}

UINT FileStream::GetPos()
{
   return (UINT)m_file.GetPosition();
}


