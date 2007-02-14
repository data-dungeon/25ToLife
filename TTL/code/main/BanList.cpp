/********************************************************************
	created:	2004/12/14
	created:	14:12:2004   13:47
	filename: 	C:\dev\branches\ttlstateoverhaul\TTL\code\main\BanList.cpp
	file path:	C:\dev\branches\ttlstateoverhaul\TTL\code\main
	file base:	BanList
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implement a ban list.
*********************************************************************/

#include "TTLPCH.h"
#include "BanList.h"

DeclareSingleton( BanList );

unsigned int BanList::s_iBannedStationRecordCount = 0;
unsigned int BanList::s_iNextAvailableBannedRecord = 0;
BanList::_BanList_Record BanList::s_BannedStationRecords[ BANLIST_MAX_RECORDS ];

BanList::BanList( void )
{
	Reset();
}

BanList::~BanList( void )
{
	Reset();
}

void BanList::Initialize( void )
{
	Reset();
}

void BanList::Uninitialize( void )
{
	Reset();
}

void BanList::Reset( void )
{
	s_iBannedStationRecordCount = 0;
	s_iNextAvailableBannedRecord = 0;
	for ( int i = 0; i < BANLIST_MAX_RECORDS; ++i )
	{
		s_BannedStationRecords[ i ].m_bConnectionsBannedFrom = false;
		s_BannedStationRecords[ i ].m_szRemoteStationId[ 0 ] = '\0';
	}
}

void BanList::BanConnectionsFrom( const char *szRemoteStationId )
{
}

void BanList::BanConnectionsTo( const char *szRemoteStationId )
{
}

bool BanList::IsBannedFrom( const char *szRemoteStationId )
{
	return( false );
}

bool BanList::IsBannedTo( const char *szRemoteStationId )
{
	return( false );
}
