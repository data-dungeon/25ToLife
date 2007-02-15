/**********************************************/
/*                                            */
/* NameLibrary.cpp                            */
/* big juju  2/24/02                          */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* register stuff by name                     */
/*                                            */
/**********************************************/

#include "EngineHelper/EngineHelperPCH.h"
#include "EngineHelper/NameLibrary.h"

/* FileStream */

struct FileStreamRecord
{
	const FileStream* data;
	char  name[ 1 ];	// extends to complete filename
};

bool NameLibrary::RegisterFileStream(const FileStream *pData, const char *pName)
{
	ASSERT( pData );
	ASSERT( pName && pName[0] );
	ASSERT( FindFileStream( pName ) == NULL );
	int len = sizeof( FileStreamRecord ) + strlen( pName );
	FileStreamRecord* rec = (FileStreamRecord*) memAlloc( len );
	rec->data = pData;
	strcpy( rec->name, pName );
	DataMgr::AddOwner( rec, DATA_FILESTREAM );
	return true;
}

FileStream* NameLibrary::FindFileStream(const char *pName)
{
	for ( DataMgr::OwnerRecord* owner = DataMgr::FirstOwner() ; owner ; owner = DataMgr::NextOwner() )
	{
		if ( owner->d_type == DATA_FILESTREAM )
		{
			FileStreamRecord* file = (FileStreamRecord*) owner->d_userData;
			if ( PathFile::FilenamesMatch( pName, file->name ) )
				return (FileStream*) file->data;
		}
	}
	return NULL;
}

void NameLibrary::ReleaseFileStreams( bool removePermanent )
{
	for ( DataMgr::OwnerRecord* owner = DataMgr::FirstOwner() ; owner ; owner = DataMgr::NextOwner() )
	{
		if ( owner->d_type == DATA_FILESTREAM )
		{
			if ( !owner->IsPermanent() || removePermanent )
			{
				FileStreamRecord* file = (FileStreamRecord*) owner->d_userData;
				delete file->data;
				memFree( file );
				DataMgr::RemoveOwner( owner );
			}
		}
	}
}
