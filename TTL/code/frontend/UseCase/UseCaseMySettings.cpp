///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMySettings.cpp
///
///  \brief UseCase for my settings screen for custom characters loop to play nice.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseMySettings.hpp"

#include "frontend/screens/screen_MySettings.h"
#include "frontend/screens/screen_CustomPlayer.h"

///////////////////////////////////////////////////////////
/// Performed whenever stop is called on the use case.
///////////////////////////////////////////////////////////
/*virtual*/
bool OnlineUI::MySettings::_deinit()
{
	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenCharSelect2 >() )
		g_FrontEnd.EndScreen( 0 );
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CHARSELECT2" );

	if( UseCase::IsScreenActive< ScreenCharSelect3 >() )
		g_FrontEnd.EndScreen( 0 );
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CHARSELECT3" );

	if( UseCase::IsScreenActive< ScreenCustomMenu >() )
		g_FrontEnd.EndScreen( 0 );
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CUSTOMMENU" );

	if( UseCase::IsScreenActive< ScreenCustomName >() )
		g_FrontEnd.EndScreen( 0 );
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CUSTOMNAME" );

	if( UseCase::IsScreenActive< ScreenCustomEdit >() )
		g_FrontEnd.EndScreen( 0 );
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CUSTOMEDIT" );

	if( UseCase::IsScreenActive< ScreenCustomPlayer >() )
		g_FrontEnd.EndScreen( 0 );
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CUSTOMPLAYER" );

	if( UseCase::IsScreenActive< ScreenCustomWeapons >() )
		g_FrontEnd.EndScreen( 0 );
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CUSTOMWEAPONS" );

	if( UseCase::IsScreenActive< ScreenMySettings >() )
		g_FrontEnd.EndScreen( 0 );
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_MYSETTINGS" );

	return true;
}