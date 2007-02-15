//============================================================================
//=
//= DataMgr.cpp
//=
//= Manage chunks of data. Whee.
//=
//============================================================================

#include "Game/GamePCH.h"

//============================================================================

#define OWNER_PERMANENT	1
#define DATA_PERMANENT	1

//============================================================================

namespace DataMgr
{
	DoublyLinkedList<OwnerRecord> s_owners( false );
	DoublyLinkedList<OwnerRecord> s_freeOwners( false );
	DoublyLinkedList<DataRecord>	s_freeData( false );
	OwnerRecord*	s_ownerArray = NULL;
	DataRecord*		s_dataArray  = NULL;
	int				s_ownerRecords = 0;
	int				s_dataRecords  = 0;
	OwnerRecord*	s_ownerSearch  = NULL;
};

//============================================================================

void DataMgr::Init( int ownerRecords, int dataRecords )
{
	int i;

	ASSERT( !s_ownerArray );
	ASSERT( !s_dataArray );

#ifdef _XBOX
	// Each owner has a dxd file now.
	dataRecords += ownerRecords;
#endif

	// init empty owners list
	s_ownerArray = new OwnerRecord[ ownerRecords ];
	for ( i = 0 ; i < ownerRecords ; i++ )
		s_freeOwners.AddTail( &s_ownerArray[i] );

	// init empty data list
	s_dataArray = new DataRecord[ dataRecords ];
	for ( i = 0 ; i < dataRecords ; i++ )
		s_freeData.AddTail( &s_dataArray[i] );

	s_ownerRecords = ownerRecords;
	s_dataRecords  = dataRecords;
}

//============================================================================

void DataMgr::Reset()
{
	ASSERT( IsEmpty( true ) );
	s_ownerSearch = NULL;
}

//============================================================================

void DataMgr::Shutdown()
{
	ASSERT( IsEmpty( false ) );
	if ( s_ownerArray )
		delete [] s_ownerArray;
	if ( s_dataArray )
		delete [] s_dataArray;
}

//============================================================================

bool DataMgr::IsEmpty( bool ignorePermanent )
{
	int activeOwners = 0;
	int activeData   = 0;
	int bogusOwners  = 0;
	int bogusData    = 0;

	for ( OwnerRecord* owner = s_owners.Head() ; owner ; owner = owner->next )
	{
		activeOwners++;
		if ( !ignorePermanent || !( owner->d_flags & OWNER_PERMANENT ) )
			bogusOwners++;

		for ( DataRecord* data = owner->FirstData() ; data ; data = data->next )
		{
			activeData++;
			if ( !ignorePermanent || !( data->d_flags & DATA_PERMANENT ) )
				bogusData++;
		}
	}

	// sanity checks
	int freeOwners = 0;
	int freeData   = 0;
	for ( OwnerRecord* owner = s_freeOwners.Head() ; owner ; owner = owner->next )
		freeOwners++;
	for ( DataRecord* data = s_freeData.Head() ; data ; data = data->next )
		freeData++;
	ASSERT( freeOwners == s_freeOwners.Items() );
	ASSERT( freeData   == s_freeData.Items() );
	ASSERT( freeOwners + activeOwners == s_ownerRecords );
	ASSERT( freeData + activeData == s_dataRecords );

	return ( bogusOwners == 0 && bogusData == 0 );
}

//============================================================================

void DataMgr::MarkPermanent()
{
	for ( OwnerRecord* owner = s_owners.Head() ; owner ; owner = owner->next )
	{
		owner->d_flags |= OWNER_PERMANENT;
		for ( DataRecord* rec = owner->FirstData() ; rec ; rec = rec->next )
			rec->d_flags |= DATA_PERMANENT;
	}
}

//============================================================================

DataMgr::OwnerRecord* DataMgr::AddOwner( void* userData, u16 type )
{
	ASSERT( userData );
	OwnerRecord* owner = FindOwner( userData );
	if ( !owner )
	{
		owner = s_freeOwners.Head();
		ASSERT( owner );
		s_freeOwners.Remove( owner );

		s_owners.AddTail( owner );
		owner->d_userData = userData;
		owner->d_type     = type;
		owner->d_flags    = 0;
	}
	return owner;
}

//============================================================================

void DataMgr::RemoveOwner( DataMgr::OwnerRecord* owner )
{
	if ( owner )
	{
		ASSERT( FindOwner( owner->d_userData ) == owner );
		if ( s_ownerSearch == owner )
			s_ownerSearch = owner->next;
		s_owners.Remove( owner );
		owner->Clear();
		s_freeOwners.AddTail( owner );
	}
}

//============================================================================

DataMgr::OwnerRecord* DataMgr::FindOwner( void* userData )
{
	ASSERT( userData );
	for ( OwnerRecord* owner = s_owners.Head() ; owner ; owner = owner->next )
		if ( owner->d_userData == userData )
			return owner;
	return NULL;
}

//============================================================================

DataMgr::OwnerRecord* DataMgr::FirstOwner()
{
	OwnerRecord* first = s_owners.Head();
	s_ownerSearch = first ? first->next : NULL;
	return first;
}

//============================================================================

DataMgr::OwnerRecord* DataMgr::NextOwner()
{
	OwnerRecord* next = s_ownerSearch;
	s_ownerSearch = next ? next->next : NULL;
	return next;
}

//============================================================================

void DataMgr::OwnerRecord::Clear()
{
	d_userData = 0;
	d_type     = 0;
	d_flags    = 0;
	DataRecord* dataRec;
	while ( ( dataRec = d_data.Head() ) != NULL )
	{
		d_data.Remove( dataRec );
		dataRec->Clear();
		s_freeData.AddTail( dataRec );
	}
}

//============================================================================

bool DataMgr::OwnerRecord::IsPermanent()
{
	return d_flags & OWNER_PERMANENT;
}

//============================================================================

DataMgr::DataRecord* DataMgr::OwnerRecord::AddData( void* userData, u16 type, u16 flags, u32 size )
{
	ASSERT( userData );

	DataRecord* data = s_freeData.Head();
	ASSERT( data );
	s_freeData.Remove( data );

	d_data.AddTail( data );
	data->d_userData = userData;
	data->d_type     = type;
	data->d_flags    = flags;
	data->d_size     = size;

	return data;
}

//============================================================================

void DataMgr::OwnerRecord::RemoveData( void* userData )
{
	ASSERT( this );
	ASSERT( userData );
	if ( this && userData )
	{
		DataRecord* dataRec = FindData( userData );
		if ( dataRec )
		{
			d_data.Remove( dataRec );
			dataRec->Clear();
			s_freeData.AddTail( dataRec );
		}
	}
}

//============================================================================

DataMgr::DataRecord* DataMgr::OwnerRecord::FirstData()
{
	return this ? d_data.Head() : NULL;
}

//============================================================================

DataMgr::DataRecord* DataMgr::OwnerRecord::FindData( void* userData )
{
	if ( this && userData )
	{
		for ( DataRecord* rec = d_data.Head() ; rec ; rec = rec->next )
			if ( rec->d_userData == userData )
				return rec;
	}
	return NULL;
}

//============================================================================

DataMgr::DataRecord* DataMgr::OwnerRecord::FindDataByType( u16 type )
{
	if ( this )
	{
		for ( DataRecord* rec = d_data.Head() ; rec ; rec = rec->next )
			if ( rec->d_type == type )
				return rec;
	}
	return NULL;
}

//============================================================================

void DataMgr::DataRecord::Clear()
{
	d_userData = 0;
	d_type     = 0;
	d_flags    = 0;
	d_size     = 0;
}

//============================================================================

bool DataMgr::DataRecord::IsPermanent()
{
	return d_flags & DATA_PERMANENT;
}

//============================================================================
