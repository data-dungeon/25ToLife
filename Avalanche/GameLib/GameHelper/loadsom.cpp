/***************************************************************************/
// SOM loading utilities
/***************************************************************************/

#include "GameHelper/GameHelperPCH.h"

#include "Game/Animation/MotionChannel.h"
#include "Game/Database/VisRegion.h"
#include "Game/Database/SceneNode.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"

// Our singleton
LoadSOM g_loadSOM;

/***************************************************************************/
/***************************************************************************/
LoadSOM::LoadSOM(void)
{
	worldHandle = INVALID_HANDLE;
	m_partitionLoading = NULL;
	m_partitionParsing = NULL;
	m_lastInstanceName[ 0 ] = 0;
	m_lastInstance = NULL;
}

/***************************************************************************/
/***************************************************************************/
LoadSOM::~LoadSOM(void)
{
	Clear();
	m_notifyList.RemoveAll( );
}

/***************************************************************************/
// Clear out everything
/***************************************************************************/
void LoadSOM::Clear(void)
{
	m_currPartitionList.RemoveAll( );
	m_newPartitionList.RemoveAll( );
	m_flushPartitionList.RemoveAll( );
	m_loadPartitionList.RemoveAll( );
	m_memoryList.RemoveAll( );
	m_lastInstanceName[ 0 ] = 0;
	m_lastInstance = NULL;
	worldHandle = INVALID_HANDLE;
}

/***************************************************************************/
// Load a world
/***************************************************************************/
bool LoadSOM::World( const char *worldFile )
{
	// Load in a model
	char pathFile[256];
	
	//save world path for partition loading
	MakePath( m_worldPath, "", worldFile, NULL);

	char *pFilename = strrchr( m_worldPath, '\\');
	if( pFilename == NULL)
	{
		pFilename = m_worldPath;
	}
	pFilename[6] = '\0';

	// Always load the full dbl here
	worldHandle = dblLoad( MakePath(pathFile, "", worldFile, "dbl"));

	if(worldHandle == INVALID_HANDLE)	// make sure we have some data to refer to later
		return(false);

	TextureMgr::SetWorldTextureInfo();
	RenderMgr__SetWorldDBHandle(worldHandle);	// let render manager know about world database for texture context management

	/* get pointer to model data in database */

	ts_PatchSOM *som = NULL;
	ts_RecordList *recordList = (ts_RecordList *) dblGetChunkData(DB_SOMARRAY, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	if	(!recordList)
		recordList = (ts_RecordList *) dblGetChunkData(DB_SOMARRAYBONE, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	if  (!recordList)
		recordList = (ts_RecordList *) dblGetChunkData(DB_DMASOMARRAY, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	if	(recordList)
		som = (ts_PatchSOM *)GetRecordAddress(recordList, 0);
	else
	{
		som = (ts_PatchSOM *) dblGetChunkData(DB_SNGLSOM, DEFAULT_CHUNK_KEY_STRING, worldHandle);
		if	(!som)
			som = (ts_PatchSOM *) dblGetChunkData(DB_SNGLSOMBONE, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	}
	ASSERT(som);	// error if not found-- can't patch som's

	// mesh som's do not have names, so don't check for one if it's a mesh (data_multi_som).  Just assume everything is OK.
	if( som->dh.u8DataType == DATA_MULTI_SOM )
		return( TRUE );


	if ((som->dh.u16Flags & SOMM_HAS_NAME) == 0)
	{
		// This is bad
		ASSERT("Incorrect database version" == 0);
		return(false);
	}

	AllocatePartitionSpace( );

	return(true);
}

/***************************************************************************/
// Get CDB data ptr
/***************************************************************************/
void *LoadSOM::GetCDB(int *pnDataSize /*= NULL*/)
{
	// Look up this chunk type in the world DBL
	return dblGetChunkDataAllDBLs( DB_CDB, DEFAULT_CHUNK_KEY_STRING, pnDataSize);
}

/***************************************************************************/
// Get ODB data ptr
/***************************************************************************/
void *LoadSOM::GetODB(int *pnDataSize /*= NULL*/)
{
	// Look up this chunk type in the world DBL
	return dblGetChunkDataAllDBLs( DB_ODB, DEFAULT_CHUNK_KEY_STRING, pnDataSize);
}

/***************************************************************************/
// Load a model
/***************************************************************************/
ts_Geometry *LoadSOM::Geometry( const char *somFile )
{
	if ( somFile && somFile[0] && stricmp( somFile, "none" ) != 0 )
	{
		char filename[ 256 ];
		MakePath( filename, "", somFile, "dbl" );

		// Load the data file
		t_Handle DBHandle = dblLoad( filename );
		if ( DBHandle != INVALID_HANDLE )
		{
			// Find geometry data
			static ts_Geometry geometry;
			dblGetGeometry( &geometry, DBHandle );
			if( geometry.pSOM || geometry.pLOD )
			{
				return &geometry;
			}
		}
	}

	return NULL;
}

/***************************************************************************/
// Destroy a loaded model
/***************************************************************************/
void LoadSOM::Destroy(
ts_Geometry* geometry)
{
	// Find the handle this geometry is associated with.
	t_Handle DBHandle = dblGetHandleFromGeometry(geometry);

	// Remove the database.
	dblRemove(DBHandle);
}

/***************************************************************************/
// Attach motion and/or an anim script
/***************************************************************************/

/***************************************************************************/
// Returns pathFile
/***************************************************************************/
char *LoadSOM::MakePath(
char *pathFile,
const char *path,
const char *file,
const char *ext)
{
	ASSERT_PTR( pathFile );
	ASSERT_PTR( file );

	if (!path)
		pathFile[0] = '\0';
	else
	{
		strcpy(pathFile, path);
		if (strlen(pathFile) && pathFile[strlen(pathFile) - 1] != '\\')
			strcat(pathFile, "\\");
	}

	strcat(pathFile, file);

	// append extension if there isn't one
	if (ext != NULL)
	{
		// skip to the end of the path (if one exists)
		const char* pFile = strrchr( file, '\\' );
		if ( pFile == NULL )
			pFile = file;

		if (strchr(pFile, '.') == NULL)	// any dot after last backslash?
		{
			strcat(pathFile, ".");
			strcat(pathFile, ext);
		}
	}

	// convert slashes
	for ( char* p = pathFile ; *p != 0 ; p++ )
		if ( *p == '/' )
			*p = '\\';

	return(pathFile);
}

/***************************************************************************/
/* find an instance by name, not case sensitive                            */
/***************************************************************************/
CInstance * LoadSOM::FindInstanceByName( const char *pName, const char *pChunkName)
{
	// we often do sucessive lookups on the same instance, so this is
	// a 1-deep cache to achive quick results.
	if ( stricmp( pName, m_lastInstanceName ) == 0 )
		return m_lastInstance;

	ts_RecordList*	pInstList;
	CInstance*	pInst;
	CPartition*		pPart;

	pInstList = (ts_RecordList *)dblGetChunkData( DB_INSTANCE, pChunkName, worldHandle);
	if( pInstList != NULL)
	{
		pInst = FindInstanceByNameArray( pInstList, pName);
		if( pInst != NULL)
		{
			// remember instance for next lookup
			strcpy( m_lastInstanceName, pName );
			m_lastInstance = pInst;

			return( pInst);
		}
	}

	pPart = m_currPartitionList.Head( );

	while( pPart != NULL)
	{
		pInstList = (ts_RecordList *)dblGetChunkData( DB_INSTANCE, pChunkName, pPart->m_handle);
		if( pInstList != NULL)
		{
			pInst = FindInstanceByNameArray( pInstList, pName);
			if( pInst != NULL)
			{
				// remember instance for next lookup
				strcpy( m_lastInstanceName, pName );
				m_lastInstance = pInst;

				return( pInst);
			}
		}
		pPart = pPart->next;
	}

	// not found
	return( NULL);
}

CInstance * LoadSOM::FindInstanceByNameArray( ts_RecordList *pInstList, const char *pName )
{
	for ( int i = 0 ; i < pInstList->usRecordCount ; i++ )
	{
		CInstance *pInst = (CInstance *)GetRecordAddress( pInstList, i );
		if ( stricmp( pInst->Name(), pName ) == 0 )
		{
			return( pInst);
		}
	}

	return( NULL);
}

/***************************************************************************/
/* find an point by name, not case sensitive                               */
/***************************************************************************/
ts_NamedPosition * LoadSOM::FindNamedPoint( const char *pName, const char *pChunkName)
{
	ts_RecordList		*pNamedPointList;
	ts_NamedPosition	*pNamedPoint;
	CPartition			*pPart;

	pNamedPointList = (ts_RecordList *)dblGetChunkData( DB_NAMED_POINTS, pChunkName, worldHandle);
	if( pNamedPointList != NULL)
	{
		pNamedPoint = FindNamedPointArray( pNamedPointList, pName);
		if( pNamedPoint != NULL)
		{
			return( pNamedPoint);
		}
	}

	pPart = m_currPartitionList.Head( );

	while( pPart != NULL)
	{
		pNamedPointList = (ts_RecordList *)dblGetChunkData( DB_NAMED_POINTS, pChunkName, pPart->m_handle);
		if( pNamedPointList != NULL)
		{
			pNamedPoint = FindNamedPointArray( pNamedPointList, pName);
			if( pNamedPoint != NULL)
			{
				return( pNamedPoint);
			}
		}

		pPart = pPart->next;
	}

	// not found
	return( NULL);
}

ts_NamedPosition * LoadSOM::FindNamedPointArray( ts_RecordList *pNamedPointList, const char *pName)
{
	/* look through records */

	for( uint i = 0; i < pNamedPointList->usRecordCount; i++)
	{
		ts_NamedPosition *pNamedPoint = (ts_NamedPosition *)GetRecordAddress( pNamedPointList, i);
		if( stricmp( pName, pNamedPoint->Name) == 0)	// not case sensistive
		{
			return( pNamedPoint);
		}
	}

	return( NULL);
}

char * LoadSOM::MakePartitionName( char *partName, s32 partitionID)
{
	char		index[10];

	strcpy( partName, m_worldPath);

	if (partitionID == 0)
		sprintf( index, ".DBL");
	else
		sprintf( index, "%03x.DBL", partitionID);

	strcat( partName, index);

	return( partName);
}

/***************************************************************************/
/* set the current visibility region for partition updates                 */
/***************************************************************************/
void LoadSOM::AddVisibilitySet( CVisRegion *visRegion)
{
	AddVisRegion( visRegion);

	u32	visRegionCount = visRegion->GetVisibleRegionCount( );

	for( u32 i = 0; i < visRegionCount; i++)
	{
		CVisRegion	*region = visRegion->GetVisibleRegion( i);
		AddVisRegion( visRegion);
	}
}

void LoadSOM::AddVisRegion( CVisRegion *visRegion)
{
	s32	partitionID = visRegion->GetPartitionID( );

	if( partitionID == CVisRegion::PRELOADED_PARTITION)
	{
		return;
	}

	if( !PartitionOnList( m_newPartitionList, partitionID))
	{
		CPartition		*partition = new CPartition;

		partition->m_handle = INVALID_HANDLE;
		partition->m_visRegion = visRegion;
		partition->m_partitionID = partitionID;

		m_newPartitionList.AddHead( partition);
	}
}

bool LoadSOM::PartitionOnList( DoublyLinkedList<CPartition> &partitionList, s32 partitionID)
{
	CPartition		*partition;

	partition = partitionList.Head( );

	while( partition)
	{
		if( partition->m_partitionID == partitionID)
		{
			return( true);
		}

		partition = partition->next;
	}

	return( false);
}


/***************************************************************************/
/* update partition set                                                    */
/***************************************************************************/
void LoadSOM::Daemon( void)
{
	UpdateFlushList( );
	UpdateLoadList( );
	ProcessPartitionLoading( );
	ProcessPartitionParsing( );

	if( m_partitionListChanged)
	{
		CNotifyList		*pNotify = m_notifyList.Head( );

		while( pNotify != NULL)
		{
			pNotify->notifyFunction( pNotify->data);
			pNotify = pNotify->next;
		}

		m_partitionListChanged = false;
	}
}

/***************************************************************************/
/* remove any partitions on the flush list that are back in the active list*/
/***************************************************************************/
void LoadSOM::UpdateFlushList( void)
{
	CPartition	*partition;

	partition = m_flushPartitionList.Head( );

	while( partition != NULL)
	{
		if( PartitionOnList( m_newPartitionList, partition->m_partitionID))
		{
			CPartition		*movePartition = partition;
			partition = partition->next;
			m_flushPartitionList.Unlink( movePartition);
			m_currPartitionList.AddHead( movePartition);
		}
		else
		{
			partition = partition->next;
		}
	}

	//flush all partitions on the flush list
	while( ( partition = m_flushPartitionList.Head( )) != NULL)
	{
		CSceneNode *sceneNode = (CSceneNode *)dblGetChunkData(	DB_SCENE_TREE,
																					DEFAULT_CHUNK_KEY_STRING,
																					partition->m_handle);

		sceneNode->Remove( );

		g_collisionSystem.Deactivate(	partition->m_collisionHandle);

		dblRemove( partition->m_handle);
		ReleaseMemoryArea( partition->m_memoryArea);
		m_flushPartitionList.Remove( partition);
		m_partitionListChanged = true;
	}

	//move any loaded partitions not in active list to flush list
	partition = m_currPartitionList.Head( );

	while( partition != NULL)
	{
		if( !PartitionOnList( m_newPartitionList, partition->m_partitionID))
		{
			CPartition		*movePartition = partition;
			partition = partition->next;
			m_currPartitionList.Unlink( movePartition);
			m_flushPartitionList.AddHead( movePartition);
		}
		else
		{
			partition = partition->next;
		}
	}
}

/***************************************************************************/
/* update the load list to reflect the current requested set               */
/***************************************************************************/
void LoadSOM::UpdateLoadList( void)
{
	CPartition	*partition;

	//remove any on load list not on new list
	partition =  m_loadPartitionList.Head( );

	while( partition != NULL)
	{
		if( !PartitionOnList( m_newPartitionList, partition->m_partitionID))
		{
			CPartition		*freePartition = partition;
			partition = partition->next;
			m_loadPartitionList.Remove( freePartition);
		}
		else
		{
			partition = partition->next;
		}
	}

	// add any new partitions to load list if not loaded or queued
	while( m_newPartitionList.Head( ) != NULL)
	{
		partition = m_newPartitionList.Head( );

		if( m_partitionLoading != NULL)
		{
			if( partition->m_partitionID == m_partitionLoading->m_partitionID)
			{
				m_newPartitionList.Remove( partition);
				continue;
			}
		}
	
		if( m_partitionParsing != NULL)
		{
			if( partition->m_partitionID == m_partitionParsing->m_partitionID)
			{
				m_newPartitionList.Remove( partition);
				continue;
			}
		}
	
		if(	PartitionOnList( m_currPartitionList, partition->m_partitionID) ||
				PartitionOnList( m_loadPartitionList, partition->m_partitionID) ||
				PartitionOnList( m_parsePartitionList, partition->m_partitionID))
		{
			m_newPartitionList.Remove( partition);
		}
		else
		{
			CPartition		*movePartition = partition;
			partition = partition->next;
			m_newPartitionList.Unlink( movePartition);
			m_loadPartitionList.AddTail( movePartition);
		}
	}
}

void LoadSOM::ProcessPartitionLoading( void)
{
	if( m_partitionLoading != NULL)
	{
		t_Error	error;

		if( !m_mediaFile->AwaitIO( 0, NULL, NULL, &error))
		{
			return;
		}

		if( error != MED_ERR_NONE)
		{
			m_mediaFile->Read( m_partitionLoading->m_memoryArea->m_buffer, m_partitionLoading->m_partitionSize, NULL);
			return;
		}

		m_mediaFile->Close( );

		m_parsePartitionList.AddTail( m_partitionLoading);
	}
	
	LoadNextPartition( );
}

void LoadSOM::LoadNextPartition( void)
{
	if( m_loadPartitionList.Head( ) != NULL)
	{

		CPartition		*partition = m_loadPartitionList.Head( );

		m_loadPartitionList.Unlink( partition);

		m_partitionLoading = partition;

		MakePartitionName( partition->m_fileName, partition->m_partitionID);
		m_mediaFile = Media::Open( partition->m_fileName, NULL, MEDOPEN_FLAG_NOWAIT);
		if( m_mediaFile == NULL)
		{
			dbgPrint( "Unable to open partition file %s\n", partition->m_fileName);
			delete partition;
			LoadNextPartition( );
			return;
		}

		partition->m_partitionSize = m_mediaFile->Size( );
		partition->m_memoryArea = GetMemoryArea( );

		if( partition->m_memoryArea->m_buffer == NULL)
		{
			dbgPrint( "No memory available for partition file %s, space needed = %d\n", partition->m_fileName, partition->m_partitionSize);
			m_mediaFile->Close( );
			delete partition;
			LoadNextPartition( );
			return;
		}

		m_mediaFile->Read( partition->m_memoryArea->m_buffer, partition->m_partitionSize, NULL);
		return;
	}

	m_partitionLoading = NULL;
}

LoadSOM::CMemoryArea * LoadSOM::GetMemoryArea( void)
{
	CMemoryArea		*area;
	area = m_memoryList.Head( );
	m_memoryList.Unlink( area);
	ASSERT( area != NULL);
	return( area);
}

void LoadSOM::ReleaseMemoryArea( CMemoryArea *area)
{
	m_memoryList.AddTail( area);
}

void LoadSOM::ProcessPartitionParsing( void)
{
#if 0
	if( m_partitionParsing == NULL)
	{
		CPartition	*partition = m_parsePartitionList.Head( );

		if( partition == NULL)
		{
			return;
		}

		m_parsePartitionList.Unlink( partition);
		m_partitionParsing = partition;
		dblLoadAsync( partition->m_fileName, (u8 *)partition->m_memoryArea->m_buffer, partition->m_partitionSize, this, LoadSOM::DBLLoadAsyncCallback); 
	}
#endif
}

void LoadSOM::DBLLoadAsyncCallback( void *owner, t_Handle handle)
{
	LoadSOM		*pThis = (LoadSOM *)owner;

	pThis->m_partitionParsing->m_handle = handle;

	CSceneNode *sceneNode = (CSceneNode *)dblGetChunkData(	DB_SCENE_TREE,
																				DEFAULT_CHUNK_KEY_STRING,
																				pThis->m_partitionParsing->m_handle);

	sceneNode->SetEnabled( true);

	pThis->m_partitionParsing->m_visRegion->GetBranch( )->AddChild( sceneNode);

	pThis->m_partitionParsing->m_collisionHandle = g_collisionSystem.Activate(	pThis->m_partitionParsing->m_partitionID,
																										pThis->m_partitionParsing->m_handle);

	pThis->m_currPartitionList.AddTail( pThis->m_partitionParsing);

	pThis->m_partitionParsing = NULL;

	pThis->m_partitionListChanged = true;
}

bool LoadSOM::PartitionsLoading( void)
{
	if(	m_partitionParsing != NULL ||
			m_partitionLoading != NULL ||
			m_loadPartitionList.Head( ) != NULL ||
			m_parsePartitionList.Head( ) != NULL)
	{
		return( true);
	}

	return( false);
}

void LoadSOM::RegisterPartitionChangeNotify( void ( *func)( void *data), void *data)
{
	CNotifyList		*pNotify = new CNotifyList;

	pNotify->notifyFunction = func;
	pNotify->data = data;

	m_notifyList.AddTail( pNotify);
}

void LoadSOM::AllocatePartitionSpace( void)
{
	ts_RecordList	*pVisRegionList = (ts_RecordList *)dblGetChunkData( DB_VIS_REGIONS, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	u32				maxSize = 0;
	u32				maxLoaded = 0;

	for( u16 i = 0; i < pVisRegionList->usRecordCount; i++)
	{

		CVisRegion	*pRegion = (CVisRegion *)GetRecordAddress( pVisRegionList, i);

		if( pRegion->GetPartitionID( ) != CVisRegion::PRELOADED_PARTITION)
		{

			char fileName[256];

			MakePartitionName( fileName, pRegion->GetPartitionID( ));
			MediaFile *mediaFile = Media::Open( fileName, NULL);
			if( mediaFile != NULL)
			{
				maxSize = Math::Max( (unsigned long) maxSize, (unsigned long)mediaFile->Size( ));
				mediaFile->Close( );
			}

			u32		numLoaded = 1;

			for( u32 j = 0; j < pRegion->GetVisibleRegionCount( ); j++)
			{

				CVisRegion	*pVisibleRegion = (CVisRegion *)pRegion->GetVisibleRegion( j);

				if( pVisibleRegion->GetPartitionID( ) != CVisRegion::PRELOADED_PARTITION)
				{
					numLoaded++;
				}
			}

			maxLoaded = Math::Max( maxLoaded, numLoaded);
		}
	}

	if( maxSize == 0)
	{
		return;
	}

	for( u32 k = 0; k < maxLoaded; k++)
	{
		CMemoryArea *area = new CMemoryArea;

		area->m_buffer = memAlloc( maxSize);

		ASSERT( area->m_buffer != NULL);

		m_memoryList.AddTail( area);
	}
}


	
