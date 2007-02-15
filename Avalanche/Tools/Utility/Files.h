#pragma once

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifndef UTILITYEXPORT
#define UTILITYEXPORT
#endif

BOOL UTILITYEXPORT PathExists( const String &strPathName );
BOOL UTILITYEXPORT FileExists( const String &strFileName );
BOOL UTILITYEXPORT MakeFileNamePath( const String &strFileName );
BOOL UTILITYEXPORT CanWriteToFile( const String &strFileName );
catch_msg void UTILITYEXPORT DeleteFile( const String &strFileName );
BOOL UTILITYEXPORT RenameFile( const String &strOldName, const String &strNewName );

BOOL UTILITYEXPORT IsRelativePath( const String &strPath );
String UTILITYEXPORT SetBasePath( const String &strBasePath );
String UTILITYEXPORT GetBasePath(); 
String UTILITYEXPORT GetAbsoluteFileName( const String &strRelativeFileName );
String UTILITYEXPORT GetRelativeFileName( const String &strAbsoluteFileName );

void UTILITYEXPORT ResetPathMap();
BOOL UTILITYEXPORT ValidateFileName( String &strFileName, BOOL *bChanged=NULL);
