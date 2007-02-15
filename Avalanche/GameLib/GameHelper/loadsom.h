/***************************************************************************/
// SOM loading utilities
/***************************************************************************/
#ifndef LOADSOM_H
#define LOADSOM_H

#include "container/dllist.h"
#include "collide/collsys.h"

class CVisRegion;

class LoadSOM
{
public:
	//---------------------------------------------------
	// Non-static stuff (buffers path/master textrefile)
	//---------------------------------------------------
	LoadSOM(void);
	~LoadSOM(void);

	// Clear out everything (but the dbk's)
	void Clear(void);

	// Load a world
	bool World(const char *worldFile);

	// Get data out of the world
	void *GetCDB(int *pnDataSize = NULL);
	void *GetODB(int *pnDataSize = NULL);

	void SetVisibilityList( unsigned long partitionID);

	// Load a model
	static ts_Geometry *Geometry(const char *somFile);
	void Destroy(ts_Geometry* geometry);

	// Returns pathFile (this is totally independent of SOM stuff
	static char *MakePath(char *pathFile, const char *path, const char *file, const char *ext = NULL);

	// returns handle of world database. the ASSERT makes sure that a world database has been loaded
	t_Handle GetWorldHandle(void) { return(worldHandle);}

	// find stuff in the world dbl
	CInstance* FindInstanceByName( const char *pName, const char *pChunkName = NULL);
	ts_NamedPosition* FindNamedPoint( const char *pName, const char *pChunkName = NULL);

	// streaming world data functions
	void Daemon( void);

	void AddVisibilitySet( CVisRegion *visRegion);

	void RegisterPartitionChangeNotify( void ( *func)( void *data), void *data);

	bool PartitionsLoading( void);

private:

	class CMemoryArea
	{
		public:
			CMemoryArea( )
			{
				m_buffer = NULL;
			}

			~CMemoryArea( )
			{
				if( m_buffer != NULL)
				{
					memFree( m_buffer);
				}
			}

			CMemoryArea			*prev;
			CMemoryArea			*next;

			void		  			*m_buffer;
	};

	class CPartition
	{
		public:
			CPartition			*prev;
			CPartition			*next;

			char					m_fileName[256];

			s32		 			m_partitionID;

			CVisRegion			*m_visRegion;

			t_Handle	 			m_handle;			//handle returned by dblLoadAsync
			CollisionSystem::DBHandle				m_collisionHandle;

			CMemoryArea			*m_memoryArea;
			s32					m_partitionSize;
	};

	class CNotifyList
	{
		public:
			CNotifyList			*prev;
			CNotifyList			*next;

			void					( *notifyFunction)( void *data);
			void					*data;

	};

	t_Handle worldHandle;	// returned handle from dblLoad to identify world database

	// used for partition streaming
	char m_worldPath[256];
	DoublyLinkedList<CPartition>		m_currPartitionList;	//partitions loaded and processed
	DoublyLinkedList<CPartition>		m_newPartitionList; //partitions wanted this frame
	DoublyLinkedList<CPartition>		m_flushPartitionList; //partitions loaded and not on wanted list
	DoublyLinkedList<CPartition>		m_loadPartitionList; //partitions wanted and not loaded
	DoublyLinkedList<CPartition>		m_parsePartitionList; //partitions loaded and needing to be parsed
	bool										m_partitionListChanged;
	CPartition								*m_partitionLoading;
	CPartition								*m_partitionParsing;
	MediaFile								*m_mediaFile;

	DoublyLinkedList<CNotifyList>		m_notifyList;

	DoublyLinkedList<CMemoryArea>		m_memoryList;

	CInstance* FindInstanceByNameArray( ts_RecordList *pList, const char *pName);
	ts_NamedPosition* FindNamedPointArray( ts_RecordList *pNamedPointList, const char *pName);

	char * MakePartitionName( char *partName, s32 partitionID);

	void AddVisRegion( CVisRegion *visRegion);
	bool PartitionOnList( DoublyLinkedList<CPartition> &partitionList, s32 partitionID);

	void UpdateFlushList( void);
	void UpdateLoadList( void);
	void ProcessPartitionLoading( void);
	void ProcessPartitionParsing( void);
	void LoadNextPartition( void);
	LoadSOM::CMemoryArea * GetMemoryArea( void);
	void ReleaseMemoryArea( CMemoryArea *area);
	void AllocatePartitionSpace( void);

	static void DBLLoadAsyncCallback( void *owner, t_Handle handle);

	char				m_lastInstanceName[ 64 ];
	CInstance*	m_lastInstance;
};

// Our singleton
extern LoadSOM g_loadSOM;

#endif
