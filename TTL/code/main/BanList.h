/********************************************************************
	created:	2004/12/14
	created:	14:12:2004   13:42
	filename: 	C:\dev\branches\ttlstateoverhaul\TTL\code\main\BanList.h
	file path:	C:\dev\branches\ttlstateoverhaul\TTL\code\main
	file base:	BanList
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Record banned stations, and stations from which one has
	been banned.
*********************************************************************/

#if !defined( __BANLIST_H )
#define __BANLIST_H

#include "EngineHelper/Singleton.h"

class BanList
{
public:
	BanList( void );
	virtual ~BanList( void );

	/*! Prepare to use the ban list.
	*/
	void Initialize( void );

	/*! Prepare to shut down the ban list.
	*/
	void Uninitialize( void );

	/*! Resets (clears) all of the ban records.
	*/
	void Reset( void );

	/*! Ban incoming connections from the named station to this station.
	Use this method on a host to ban incoming connections from
	the named station.
	*/
	void BanConnectionsFrom( const char *szRemoteStationId );

	/*! Ban outgoing connections to the named station from this station.
	Use this method on a client when a host bans this station from connecting.
	*/
	void BanConnectionsTo( const char *szRemoteStationId );

	/*! Check if the named station has been banned from incoming connections to this station.
	*/
	bool IsBannedFrom( const char *szRemoteStationId );

	/*! Test if this station has been banned from making outgoing connections to the named station.
	*/
	bool IsBannedTo( const char *szRemoteStationId );

protected:
private:
	typedef struct _BanList_Record
	{
		/*! Used to determine the sense of this ban record.
		If <b>true</b> the ban blocks incoming connection attempts from the named station.
		If <b>false</b> the ban blocks outgoing connection attempts to the named station.
		*/
		bool		m_bConnectionsBannedFrom;

		/*! A station identification string.
		*/
		char		m_szRemoteStationId[ 128 ];
	};
#define BANLIST_MAX_RECORDS ( 16 )
	static unsigned int		s_iBannedStationRecordCount;
	static unsigned int		s_iNextAvailableBannedRecord;
	static _BanList_Record	s_BannedStationRecords[ BANLIST_MAX_RECORDS ];
};

#define g_banlist GetSingleton(BanList)

#endif __BANLIST_H