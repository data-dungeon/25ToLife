// Dan  8/18/2004  \DanLP110\FileIO\Files.cpp

#include "stdafx.h"
#include "AString.h"
#include "Files.h"
#include "FileDialog.h"


BOOL PathExists( const String &strPathName )
{
   ASSERT( !strPathName.IsEmpty() );
   return _access(strPathName.Get(), 0) != -1;
}

BOOL FileExists( const String &strFileName )
{
   ASSERT( !strFileName.IsEmpty() );
   return _access(strFileName.Get(), 0) != -1;
}

BOOL MakeFileNamePath( const String &strFileName )
{
   ASSERT(!strFileName.IsEmpty());

   String strPath = strFileName.GetPathOnly();
   if ( PathExists( strPath ) )
      return TRUE;

   return CreateDirectory( strPath.Get(), NULL );
}

BOOL CanWriteToFile( const String &strFileName )
{
   if (!MakeFileNamePath(strFileName))
      return FALSE;

   FILE *file = fopen( strFileName.Get(), "a+");
   
   if (file == NULL)
      return FALSE;
      
   fclose(file);
   return TRUE;
}

void DeleteFile( const String &strFileName )
{
   ASSERT( !strFileName.IsEmpty() );
   try 
   {
      CFile::Remove( strFileName.Get() );
   }
   catch (CFileException *pEx)
   {
      TCHAR   szErrorMessage[512];
      UINT    nHelpContext;
      String  strMsg;

      if (pEx->GetErrorMessage(szErrorMessage, _countof(szErrorMessage), &nHelpContext))
      {
         strMsg = "File: %1 cannot be deleted.\n  (%2)";
         strMsg.Replace( "%1", strFileName );
         strMsg.Replace( "%2", szErrorMessage );
      }
      else
      {
         strMsg = "File: %1 cannot be deleted";
         strMsg.Replace( "%1", strFileName );
      }

      pEx->Delete();

      THROW_FATAL_ERROR( strMsg );
   }
}

BOOL RenameFile( const String &strOldName, const String &strNewName )
{
   ASSERT( !strOldName.IsEmpty() );
   ASSERT( !strNewName.IsEmpty() );

   try
   {
      MakeFileNamePath( strNewName );
      CFile::Rename( strOldName.Get(), strNewName.Get() );
   }
   catch(CFileException* pEx )
   {
   #ifdef _DEBUG
      afxDump << "File " << strOldName.Get() << " could not be renamed, cause = "
              << pEx->m_cause << "\n";
   #endif
      pEx->Delete();
      return FALSE;
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////

static UINT GetPathDepth( const String &strPath )
{
   UINT nCount = 1;
   UINT nIndex = -1;

   while ((nIndex = strPath.Find( nIndex+1, '\\' )) != -1)
      nCount++;

   return nCount;
}

static String f_strBasePath;
static UINT f_strBasePathDepth;

String SetBasePath( const String &strBasePath )
{
   String strOld = f_strBasePath;
   f_strBasePath = strBasePath;

   f_strBasePathDepth = GetPathDepth(f_strBasePath);
   return strOld;
}

String GetBasePath() 
{
   return f_strBasePath;
}

UINT GetBasePathDepth()
{
   return f_strBasePathDepth;
}

BOOL IsRelativePath( const String &strPath )
{
   ASSERT(!strPath.IsEmpty());

   if (strPath[0]=='\\' && strPath[1]=='\\')
      return FALSE;

   if (strPath[1]==':')
      return FALSE;

   return TRUE;
}

String GetAbsoluteFileName( const String &strRelativeFileName )
{
   ASSERT( !strRelativeFileName.IsEmpty() );

   if (!IsRelativePath(strRelativeFileName))
      return strRelativeFileName;

   String strBasePath = GetBasePath();

   if (strBasePath.IsEmpty())
      return strRelativeFileName;

   UINT nIndex = -1;
   UINT nLastIndex = -1;
   while( (nIndex = strRelativeFileName.Find(nIndex+1, "..\\")) != -1)
   {
      nLastIndex = nIndex;
      strBasePath.RemoveName();
   }

   char *szName;
   if (nLastIndex!=-1)
      szName = strRelativeFileName.Get() + nLastIndex + 3;
   else
      szName = strRelativeFileName.Get();

   strBasePath.AppendName( szName );

   return strBasePath;
}

static UINT GetPathMatchDepth( String strPath1, const String strPath2 )
{
   char *p1, *p2; 
   UINT nCount=0;

   for (p1 = strPath1.Get(), p2 = strPath2.Get(); *p1 && *p2 && tolower(*p1)==tolower(*p2); ++p1, ++p2)
      nCount++;
   
   if (nCount == 0) 
      return 0;

   for (p1 = strPath1.Get()+nCount, p2 = strPath2.Get()+nCount; 
    !((*p1 == 0 || *p1=='\\' ) && (*p2 == 0 || *p2=='\\' )) 
    && p1 > strPath1.Get(); --p1, --p2)
      nCount--;

   if (nCount <= 0) 
      return 0;

   strPath1.SetAt(nCount,'\0');

   return GetPathDepth( strPath1 );
}

static String ShortenPath( const String &strPath, UINT nSkip )
{
   UINT nIndex =-1;
   while ( nSkip && (nIndex = strPath.Find(nIndex+1, '\\')) != -1)
      nSkip--;

   if (nSkip || nIndex==-1)
      return strPath;

   char *szShorten = strPath.Get() + nIndex + 1;
   
   return szShorten;
}

String GetRelativeFileName( const String &strAbsoluteFileName )
{
   ASSERT( !strAbsoluteFileName.IsEmpty() );

   if (IsRelativePath(strAbsoluteFileName))
      return strAbsoluteFileName;

   String strBasePath = GetBasePath();

   if (strBasePath.IsEmpty())
      return strAbsoluteFileName;

   UINT nDepth = GetPathMatchDepth( strBasePath, strAbsoluteFileName);
   UINT nBasePathDepth = GetBasePathDepth();

   String strRelativePath;

   if (nDepth > 0)
   {
      for (UINT i=0; i<nBasePathDepth-nDepth; i++)
         strRelativePath += "..\\";
      strRelativePath+=ShortenPath( strAbsoluteFileName, nDepth );
   }
   else
      strRelativePath = strAbsoluteFileName;

   return strRelativePath;
}

/////////////////////////////////////////////////////////////////////////

static CMap<String, const String &, String, const String &> f_pathmap;

void ResetPathMap()
{
   f_pathmap.RemoveAll();
}

BOOL ValidateFileName( String &strFileName, BOOL *bChanged/*=NULL*/)
{
   if (bChanged)
      *bChanged = FALSE;

   strFileName.Replace( '/', '\\' );

   if (FileExists( strFileName ))
      return TRUE;

   String strPath = strFileName.GetPathOnly();
   String strNewPath;

   // Check mapped folder
   if (f_pathmap.Lookup(strPath, strNewPath))
   {
      strNewPath.AppendName( strFileName.GetNameOnly() );
      if (FileExists( strNewPath ))
      {
         strFileName = strNewPath;
         if (bChanged)
            *bChanged = TRUE;
         return TRUE;
      }
   }

   String strSuffix = strFileName.GetSuffixOnly();
   strSuffix.ToUpper();

   if (strSuffix.IsEmpty())
      strSuffix = '*';

   String strFilter("%1 files|*.%1|All Files|*.*||");
   strFilter.Replace("%1", strSuffix);

   String strTitleBar("Locate missing file: %1");
   strTitleBar.Replace("%1", strFileName);

   String strNewFileName;

   // Let user choose
   while (TRUE)
   {
      if (!GetOpenFileName( strFileName.GetNameOnly(), strFilter, strNewFileName, strTitleBar.Get() )) // Do some work here to Pretty it up (Title, FIlters? )
         return FALSE;

      if (FileExists( strNewFileName ))
      {
         f_pathmap.SetAt( strFileName.GetPathOnly(), strNewFileName.GetPathOnly() );

         strFileName = strNewFileName;

         if (bChanged)
            *bChanged = TRUE;
         break;
      }
   }
   return TRUE;
}
