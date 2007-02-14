/*
**
**  File:   GameEnv.cpp
**  Date:   March 31, 2004
**  By:     Bryant Collard
**  Desc:   Wrap game environment stuff in a class.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/GameEnv.cpp $
**  $Revision: #101 $
**  $DateTime: 2005/08/24 14:41:20 $
**    $Author: Food $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// Engine headers
#include "Game/Managers/ResourceManager.h"

// GameLib headers
#include "GameHelper/FileLoader.h"
#include "environ/ConfigMgr.h"
#include "main/CharacterLoader.h"
#include "frontend/frontend.h"
#include "main/cheats.h"

CGameEnv g_gameEnvironment;

const char *CGameEnv::s_defaultMusic = "!#track01;track02;track04;track05;track06;track07;track08;track09;track10;track11;track12;track14;track15;track16;track17;track18;track19;track20;track21;track22;track23;track24;track25;track26;track27;track28;track29;track30;track31;track32;track33;track34;track36;track37;track38;track39;track40;track41;track42;track43;track44";

const char *c_musicFrontend = "!#track29;track06;track05;track15;track18;track09;track12";
const char *c_musicSP_warehouse = "!track05;track20;track33;track25;track38";
const char *c_musicSP_apartment ="!track23;track14;track10;track41;track29";
const char *c_musicSP_downtown = "!track18;track19;track40;track31;track26";
const char *c_musicSP_culdesac = "!track04;track11;track17;track32;track09";
const char *c_musicSP_club = "!track24;track15;track16;track41;track44";
const char *c_musicSP_subway = "!track08;track10;track38;track20;track09";
const char *c_musicSP_tijuana = "!track06;track22;track34;track37;track19;track25";
const char *c_musicSP_casino = "!track21;track24;track07;track28;track36";
const char *c_musicSP_penthouse = "!track27;track02;track25;track43;track39;track12";
const char *c_musicSP_prison = "!track01;track28;track30;track42;track05";
const char *c_musicSP_mall = "!track25;track31;track42;track24;track39";
const char *c_musicSP_hacienda = "!track12;track02;track10;track08;track34;track29";
const char *c_music_beatdown = "!track01;track11;track02;track14;track05;track04";

const char *c_musicMP = 	"!#track06;track22;track34;track33;track25;track40;track43;track44;track37;track36;track39;track41;track05;track09;track10;track11;track12;track14;track15;track16;track17;track18;track19;track23;track20;track29;track27;track26;track02;track07;track08;track31;track04;track30";

#define GAME_ENV_PRELOAD_GROUP 31

#if !defined(CDROM)
//#define LEVEL_SWITCH_MEM_DEBUG
#endif

//////////////////////////////////////////////////////////////////////////////
#ifdef DIRECTX_PC
#define MPLEVEL_COUNT		( 18 )
#else
#define MPLEVEL_COUNT		( 16 )
#endif // DIRECTX_PC
#define GAMEMODE_COUNT	(  5 )

static const char* s_MPLevelDescription[ MPLEVEL_COUNT ] =
{
	/*"mp_warehouse",*/     "Revisit your roots!  Best for 8 to 12 people.",
	/*"mp_apartments",*/    "Battle it out in and around the housing projects.  Best for 6 to 10 people.",
	/*"mp_downtown",*/      "Fight in and out of the bank and the surrounding commercial district.  Best for 6 to 10 people.",
	/*"mp_culdesac",*/      "The action is poppin’ in this little neighborhood.  Be careful, there is only one way in…or out.  Best for 12 to 16 people.",
	/*"mp_club",*/          "Things have gotten a little out of control at da Club.  Best for 8 to 12 people.",
	/*"mp_subway",*/        "Mayhem breaks loose on the subway levels.  Best for 12 to 16 people.",
	/*"mp_tijuana",*/       "Battle for control of the T.J. streets.  Best for 6 to 10 people.",
	/*"mp_casino",*/        "No time to play while the carnage rages on in this major casino.  Best for 12 to 16 people.",
	/*"mp_penthouse",*/     "Defend your pimp penthouse apartment from Law Enforcement.  Best for 8 to 12 people.",
	/*"mp_prison",*/        "All hell has broken out in a high security prison.  Best for 8 to 12 people.",
	/*"mp_mall",*/          "Havoc has broken out in Any Mall, USA.  Best for 8 to 12 people.",
	/*"mp_hacienda",*/      "Bust into this kingpin’s crib and show him who is really in charge…or stop Law Enforcement dead in their tracks.  Best for 12 to 16 people.",
	/*"mp_urbandecay",*/    "The fight for control of the rat infested part of town is on.  Make that turf yours.  Best for 12 to 16 people.",
	/*"mp_trainyard",*/     "Crime has broken out in this small train yard.  Best for 4 to 8 people.",
	/*"mp_bordello",*/      "The cartel’s hideout has been discovered.  Find out who is really in control.  Best for 4 to 8 people.",
	/*"mp_villas",*/        "A big surprise is in store for these suburban residents, as the battle is brought to them.  Best for 8 to 12 people.",
};

static const char* s_MPLevelIndexByName[ MPLEVEL_COUNT ] =
{
	"mp_warehouse",
	"mp_apartments",
	"mp_downtown",
	"mp_culdesac",
	"mp_club",
	"mp_subway",
	"mp_tijuana",
	"mp_casino",
	"mp_penthouse",
	"mp_prison",
	"mp_mall",
	"mp_hacienda",
	"mp_urbandecay",
	"mp_trainyard",
	"mp_bordello",
	"mp_villas",
#ifdef DIRECTX_PC
	"mp_railstation",
	"mp_southoftheborder",
#endif
};

static int s_GetMPLevelIndexByName( const char *szName )
{
	ASSERT( szName != NULL );

	for ( int i = 0; i < MPLEVEL_COUNT; ++i )
	{
		if ( strcmp( szName, s_MPLevelIndexByName[ i ] ) == 0 )
		{
			return( i );
		}
	}

	return( -1 );
}

static const char* s_ModeIndexByName[ GAMEMODE_COUNT ] =
{
	"war",
	"raid",
	"robbery",
	"tag",
	"wipeout"
};

static int s_GetModeIndexByName( const char *szName )
{
	ASSERT( szName != NULL );

	for ( int i = 0; i < GAMEMODE_COUNT; ++i )
	{
		if ( strcmp( szName, s_ModeIndexByName[ i ] ) == 0 )
		{
			return( i );
		}
	}

	return( -1 );
}

static bool s_AllowedModeLevel[ MPLEVEL_COUNT ][ GAMEMODE_COUNT ] =	/* [Level_Index][Mode_Index] */
{
								/*war*/		/*raid*/		/*robbery*/	/*tag*/	/*wipeout*/
	/*mp_warehouse*/	{	true,			false,		false,	true,		true },
	/*mp_apartments*/	{	true,			true,			true,		true,		true },
	/*mp_downtown*/	{	true,			true,			true,		true,		true },
	/*mp_culdesac*/	{	true,			true,			true,		true,		true },
	/*mp_club*/			{	true,			true,			true,		true,		true },
	/*mp_subway*/		{	true,			true,			true,		true,		true },
	/*mp_tijuana*/		{	true,			true,			true,		true,		true },
	/*mp_casino*/		{	true,			true,			true,		true,		true },
	/*mp_penthouse*/	{	true,			true,			true,		true,		true },
	/*mp_prison*/		{	true,			true,			true,		true,		true },
	/*mp_mall*/			{	true,			true,			true,		true,		true },
	/*mp_hacienda*/	{	true,			true,			true,		true,		true },
	/*mp_urbandecay*/	{	true,			true,			true,		true,		true },
	/*mp_trainyard*/	{	true,			true,			true,		true,		true },
	/*mp_bordello*/	{	true,			true,			true,		true,		true },
	/*mp_villas*/		{	true,			true,			true,		true,		true },
#ifdef DIRECTX_PC
	/*mp_railstation*/	{		true,			true,			true,		true,		true },
	/*mp_southoftheborder*/{	true,			true,			true,		true,		true },
#endif

};

static bool s_CheckMapForMode( const char *szMapName, const char *szModeName )
{
	int iMapIndex = s_GetMPLevelIndexByName( szMapName );
	int iModeIndex = s_GetModeIndexByName( szModeName );

	if ( ( iMapIndex < 0 ) || ( iModeIndex < 0 ) )
	{
		return( false );
	}
	else
	{
		return( s_AllowedModeLevel[ iMapIndex ][ iModeIndex ] );
	}
}

//////////////////////////////////////////////////////////////////////////////
const char *missionText[] =
{
	"Freeze arrived at the West Harbor Warehouses ready to do the deal, but the money drop has gone bad. Detective Maria Mendoza was waiting for you and the Colombians were dead before you got there. The Police are about to swarm the area and Mendoza is going to pin a double murder on you. Escape by any means necessary.",
	"The police are after you, but so are the 22nd Street D-boys, who have heard you are trying to get out of the crew. This is going to make meeting Shaun at your apartment more difficult. Head to your apartment but try to avoid the police and your former partners in crime.",
	"You are desperate for a large amount of money to get back your wife and child, who are being held by Calderon. You see little choice but to turn to what you have known your entire life, Crime.  Rob the 1st Terrace National Bank and escape with the cash by any means necessary.",
	"Maria Mendoza has led you into D-Boys territory to infiltrate Calderon’s house and locate the large quantity of drugs that have gone missing from the evidence locker at the precinct. As Detective Lester Williams you must Raid the house and obtain the target item, apprehending as many Criminals as possible in the process.",
	"Finding the drugs with the evidence tag on them has prompted you to investigate further by questioning Shaun Calderon in person.  Mendoza’s interrogation of a D-Boy tipped you off that Calderon is at a local Club. Track him down and find out how he got his hands on those drugs.",
	"After chasing Calderon through the Club and doing battle with countless D-Boys, he’s eluded you by heading into the Subway. Continue to pursue Shaun, who is heading for the nearest train hoping to escape.",
	"Having been forced to leave the United States by Detective Mendoza, Calderon has arrived in Mexico looking for a bigger piece of the Crime Game. As Shaun Calderon, put this plan into motion by tracking down key members of the Saragosa Cartel.  A small-time hustler named Riggs runs Tijuana, track him down and announce your presence to Saragosa.",
	"You have made your way through the streets of Tijuana, leaving a trail of enemies and dead cartel soldiers in your wake… but you still have unfinished business. Enter the Casino and Rob the vaults, cripple Saragosa’s ability to pay his men, and bring the crime lord closer towards a final confrontation.",
	"The Saragosa Cartel is in a state of chaos, and the boss is headed back to town to deal with you personally. Now there is only one thing left to do: assassinate Saragosa himself. After making your way through the service corridors of his posh highrise, you are ready to launch a surprise attack and take control of the cartel.",
	"Andre Freeze Francis wanted to leave his life of crime behind and make a better life for his family. Instead, betrayed by the dirty dealings of Maria Mendoza and Shaun Calderon, Freeze ended up in Crazy Horse State Penitentiary. Now two years later, the guards’ mistreatment of the prisoners has led to a riot - the perfect cover for an escape. Your cell door is open, make your way to freedom.",
	"You’ve successfully escaped Crazy Horse, but a snitch has tipped off Detective Mendoza that you’re loose.  She knows Freeze can blow the cover on her relationship with Calderon and potentially end her career. Upon hearing of your prison break she has dispatched an army of corrupt law enforcement officers to silence you.  Evade them and keep an eye out for Mendoza as you keep running for your life.",
	"After two long years behind bars, you escaped and paid back Mendoza for sending you to prison. But that hasn’t helped resolve the burning question of what happened to Monica and Darnell.  So you have infiltrated the grounds of Shaun’s luxurious mansion, and you want answers. Fight your way through Calderon’s men and exact revenge from your one-time best friend, now your worst enemy.",
	"Oh you think you gully now? You aint shit, so let's do this then."
};

#ifndef CDROM
const char *holoText = "This holo deck was created for your testing pleasure. Please report all bugs to Senator Charles E. Schumer (212) 486-4430.";
#else
const char *holoText = "This holo deck was created for your testing pleasure.";
#endif

//////////////////////////////////////////////////////////////////////////////

CGameEnv::SGameEnvEntry CGameEnv::s_entry[] =
{
	// filename, displayName, musicName, introMovie, endingMovie, briefingScreen, allowedBits
	{ "01 Warehouse", "sp_warehouse", c_musicSP_warehouse, "%1/scene_a", NULL, "briefing/warehouse.dbl", missionText[0], TYPE_SINGLEPLAYER },
	{ "02 Apartment", "sp_apartments", c_musicSP_apartment, NULL, NULL, "briefing/apartments.dbl", missionText[1], TYPE_SINGLEPLAYER },
	{ "03 Downtown", "sp_downtown", c_musicSP_downtown, NULL, "%1/scene_b", "briefing/downtown.dbl", missionText[2], TYPE_SINGLEPLAYER },
	{ "04 Culdesac", "sp_culdesac", c_musicSP_culdesac, "%1/scene_c", NULL, "briefing/culdesac.dbl", missionText[3], TYPE_SINGLEPLAYER },
	{ "05 Club", "sp_club", c_musicSP_club, NULL, NULL, "briefing/club.dbl", missionText[4], TYPE_SINGLEPLAYER },
	{ "06 Subway", "sp_subway", c_musicSP_subway, NULL, "%1/scene_d", "briefing/subway.dbl", missionText[5], TYPE_SINGLEPLAYER },
	{ "07 Tijuana", "sp_tijuana", c_musicSP_tijuana, "%1/scene_e", NULL, "briefing/tijuana.dbl", missionText[6], TYPE_SINGLEPLAYER },
	{ "08 Casino", "sp_casino", c_musicSP_casino, NULL, NULL, "briefing/casino.dbl", missionText[7], TYPE_SINGLEPLAYER },
	{ "09 Penthouse", "sp_penthouse", c_musicSP_penthouse, NULL, "%1/scene_f", "briefing/penthouse.dbl", missionText[8], TYPE_SINGLEPLAYER },
	{ "10 Prison", "sp_prison", c_musicSP_prison, "%1/scene_g", NULL, "briefing/prison.dbl", missionText[9], TYPE_SINGLEPLAYER },
	{ "11 Mall", "sp_mall", c_musicSP_mall, NULL, NULL, "briefing/mall.dbl", missionText[10], TYPE_SINGLEPLAYER },
	{ "12 Hacienda", "sp_hacienda", c_musicSP_hacienda, NULL, "%1/scene_h", "briefing/hacienda.dbl", missionText[11], TYPE_SINGLEPLAYER },
	{ "Bonus - Beatdown", "wavebattle", c_music_beatdown, NULL, NULL, "briefing/beatdown.dbl", missionText[12], TYPE_SINGLEPLAYER },

	{ "01 Warehouse MP",		s_MPLevelIndexByName[  0 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "02 Apartment MP",		s_MPLevelIndexByName[  1 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "03 Downtown MP",		s_MPLevelIndexByName[  2 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "04 Culdesac MP",		s_MPLevelIndexByName[  3 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "05 Club MP",			s_MPLevelIndexByName[  4 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "06 Subway MP",			s_MPLevelIndexByName[  5 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "07 Tijuana MP",		s_MPLevelIndexByName[  6 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "08 Casino MP",			s_MPLevelIndexByName[  7 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "09 Penthouse MP",		s_MPLevelIndexByName[  8 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "10 Prison MP",			s_MPLevelIndexByName[  9 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "11 Mall MP",			s_MPLevelIndexByName[ 10 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "12 Hacienda MP",		s_MPLevelIndexByName[ 11 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "13 UrbanDecay MP",	s_MPLevelIndexByName[ 12 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "14 Trainyard MP",		s_MPLevelIndexByName[ 13 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "15 Bordello MP",		s_MPLevelIndexByName[ 14 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "16 Villas MP",			s_MPLevelIndexByName[ 15 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
#ifdef DIRECTX_PC
	{ "17 Railstation MP",  s_MPLevelIndexByName[ 16 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
	{ "18 SouthOfTheBorder MP",	s_MPLevelIndexByName[ 17 ], c_musicMP, NULL, NULL, NULL, NULL, TYPE_MULTIPLAYER },
#endif
	{ "HoloBri", "HoloBri", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloBryant", "HoloBryant", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloBryce", "HoloBryce", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloDan", "HoloDan", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloDwain", "HoloDwain", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloFood", "HoloFood", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloHurdles", "HoloHurdles", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloKen", "HoloKen", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloMatt", "HoloMatt", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloNPC", "HoloNPC", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloLott", "HoloLott", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloJoe", "HoloJoe", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
 	{ "HoloPaul", "HoloPaul", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloRob", "HoloRob", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloRitual", "HoloRitual", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloShon", "HoloShon", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "HoloTravis", "HoloTravis", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },
	{ "Holo", "Holo", NULL, NULL, NULL, NULL, holoText, TYPE_DEBUG },

	{ "frontend", "frontend", c_musicFrontend, NULL, NULL, NULL, NULL, TYPE_HIDDEN },
	{ "temp", "Temp", c_musicFrontend, NULL, NULL, NULL, NULL, TYPE_DEBUG }
};
int CGameEnv::s_envCount = sizeof(CGameEnv::s_entry) / sizeof(CGameEnv::SGameEnvEntry);

/***************************************************************************/
/***************************************************************************/
CGameEnv::CGameEnv() : d_messageOwner("GameEnv")
{
	Clear();
}

void CGameEnv::Initialize()
{
	d_changeConfigHandler.Initialize( "GameOptionsChanged", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this );
}

/***************************************************************************/
// How many environments do we have?
/***************************************************************************/
int CGameEnv::Environments(Type i_type)
{
	int count = 0;
	for (int i = 0; i < s_envCount; i++)
	{
		if ((i_type & s_entry[i].d_type) != 0)
			++count;
	}
	return count;
}

/***************************************************************************/
// Set and get the "current" environment.
/***************************************************************************/
void CGameEnv::Clear()
{
	d_environment = -1;
	SetWorldName(NULL);
}

/***************************************************************************/
/***************************************************************************/
bool CGameEnv::Set(
int i_environment)
{
	if (i_environment < 0 || i_environment >= s_envCount)
		return false;

	d_environment = i_environment;
	SetWorldName(s_entry[d_environment].d_fileName);
	return true;
}

/***************************************************************************/
/***************************************************************************/
int CGameEnv::Get() const
{
	return d_environment;
}

/***************************************************************************/
/***************************************************************************/
bool CGameEnv::Valid() const
{
	return Valid(d_environment);
}

/***************************************************************************/
/***************************************************************************/
bool CGameEnv::Valid( int i_environment, Type i_type, const char *szModeName )
{
	ASSERT( Valid( i_environment ) );
	if ( Valid( i_environment ) )
	{
#ifdef _DEBUG
		bool bModeNamePass = true;
#else
		bool bModeNamePass = (
			( szModeName == NULL ) ||
			( ( szModeName != NULL ) && ( s_CheckMapForMode( s_entry[ i_environment ].d_fileName, szModeName ) ) )
		);
#endif
		return( ( ( s_entry[ i_environment ].d_type & i_type ) != 0 ) && bModeNamePass );
	}
	else
	{
		return( false );
	}
}

/***************************************************************************/
/***************************************************************************/
bool CGameEnv::IsWeaponMaster(int forLevel)
{
	if (Cheats::Enabled(Cheats::WEAPONMASTER))
		return true;
	else if (BonusLevel(forLevel))
		return g_GlobalProfile.GetCompletedLevel(12);
	else if (forLevel < 0 || forLevel > 11)
		return false;
	else
	{
		// By vignette
		int vignette = (forLevel / 3);
		if (vignette == 3)
			return g_GlobalProfile.GetCompletedLevel(11);
		else
			return g_GlobalProfile.GetCompletedLevel((vignette + 1) * 3);
	}
}

/***************************************************************************/
/***************************************************************************/
bool CGameEnv::ProcessPreload()
{
	char* path = WorldRoot(".dbl", NULL);

	// If our world isn't loaded/loading, we need to load it.
	if (!g_resourceManager.IsLoaded(path) && !g_resourceManager.IsLoadPending(path))
	{
		// cancel all loads
		if (g_resourceManager.CancelLoads() == false)
			return false;

		// unload everything
		g_resourceManager.UnloadAll();
		CharacterLoader::ClearLoads();

		// some GDI resources may not be free yet, they need a few
		// frames to purge properly.
		#if defined(PS2)
		if (MasterList::DisplayListDeletesPending())
			return false;
		MasterList::Reset();	// free chunks
		#endif //PS2

		// free up temporary text resources
		Sprite::DrawCancel();
		g_FrontEnd.m_SpriteTree.ReleaseHandlesRecursively();

#if !defined(CONSUMER_BUILD)
		{
			int freeB = 0, freeH = 0, freeMax = 0, allocB = 0, allocH = 0;
			Layers__GetMemoryStats(&freeB, &freeH, &freeMax, &allocB, &allocH);
			g_debugOutput.RegisterCategory("Memory", "MEM");
			g_debugOutput.Print(false, g_debugOutput.Category("MEM"), "PRELOAD %8d free, %8d largest, %4d handles", freeB, freeMax, freeH);
		}
#endif

		// DEBUG DEBUG DEBUG
#ifdef LEVEL_SWITCH_MEM_DEBUG
		{
			extern int sessionID;
			extern void memDumpBlockInfo( const char* filename );
			static int count = 0;
			char buf[ 32 ];
			if (sessionID == 0)
				sessionID = g_random.InRange(10000, 99999);
			sprintf( buf, "%d-level-%d", sessionID, count++ );
			memDumpBlockInfo( buf );

			static int lastFreeB = 0;
			static int lastFreeMax = 0;
			int freeB, freeH, freeMax, allocB, allocH;
			Layers__GetMemoryStats(&freeB, &freeH, &freeMax, &allocB, &allocH);
			dbgPrint( "*** FREE: %d, %d [%d h]  ALLOC: %d [%d h]\n", freeB, freeMax, freeH, allocB, allocH );
			if ((lastFreeB && lastFreeB != freeB) || (lastFreeMax && lastFreeMax != freeMax))
			{
				dbgPrint( " ==> OLD FREE: %d, %d\n", lastFreeB, lastFreeMax );
				ASSERTF( false, ( "memory leak: old free(%d,%d) new(%d,%d)", lastFreeB, lastFreeMax, freeB, freeMax ) );
			}
			lastFreeB = freeB;
			lastFreeMax = freeMax;

			//g_fileLoader.LoadWorldFilesAsync(g_gameEnvironment.WorldName(), GAME_ENV_PRELOAD_GROUP);
			//while (!g_resourceManager.IsLoaded(GAME_ENV_PRELOAD_GROUP))
			//	g_resourceManager.Update();
			//g_resourceManager.UnloadGroup(GAME_ENV_PRELOAD_GROUP);
			//memDumpBlockInfo( "dump", count++ > 0 );
		}
#endif //LEVEL_SWITCH_MEM_DEBUG

		// kick our load
		g_fileLoader.LoadWorldFilesAsync(g_gameEnvironment.WorldName(), GAME_ENV_PRELOAD_GROUP);

		// and rekick the character art
		CharacterLoader::QueueLoads();
	}

	// we're done when all of our files are loaded
	return g_resourceManager.IsLoaded(GAME_ENV_PRELOAD_GROUP);
}

/***************************************************************************/
/***************************************************************************/
bool CGameEnv::ClearPreload(void)
{
	// Cancel files yet to be loaded
	if (!g_resourceManager.CancelLoads())
		return(false);

	// Clear all loaded data
	g_resourceManager.UnloadAll();
	CharacterLoader::Reset();

	return(true);
}

/***************************************************************************/
// Get at the data for the current environment.
/***************************************************************************/
const char* CGameEnv::GetWorldFileName() const
{
	return GetWorldFileName(d_environment);
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetWorldDisplayName() const
{
	return GetWorldDisplayName(d_environment);
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetMusicList() const
{
	return GetMusicList(d_environment);
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetIntroMovie() const
{
	return GetIntroMovie(d_environment);
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetEndingMovie() const
{
	return GetEndingMovie(d_environment);
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetBriefingScreen() const
{
	return GetBriefingScreen(d_environment);
}

/***************************************************************************/
/***************************************************************************/
int CGameEnv::GetType() const
{
	return GetType(d_environment);
}

/***************************************************************************/
// Get at the data for a given environment.
/***************************************************************************/
const char* CGameEnv::GetWorldFileName(int i_env)
{
	ASSERT(Valid(i_env));

	return s_entry[i_env].d_fileName;
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetWorldDisplayName(int i_env)
{
	ASSERT(Valid(i_env));

	if (s_entry[i_env].d_displayName)
		return s_entry[i_env].d_displayName;
	else
		return s_entry[i_env].d_fileName;
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetWorldDescription(int i_env)
{
	ASSERT(Valid(i_env));

	int i_index = s_GetMPLevelIndexByName( s_entry[i_env].d_fileName );
	if ( i_index != -1 )
	{
		return( s_MPLevelDescription[ i_index ] );
	}
	else
	{
		return( NULL );
	}
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetMusicList(int i_env)
{
	ASSERT(Valid(i_env));

	if (s_entry[i_env].d_musicName)
		return s_entry[i_env].d_musicName;
	else
		return s_defaultMusic;
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetIntroMovie(int i_env)
{
	ASSERT(Valid(i_env));
	return s_entry[i_env].d_introMovie;
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetEndingMovie(int i_env)
{
	ASSERT(Valid(i_env));
	return s_entry[i_env].d_endingMovie;
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetBriefingScreen(int i_env)
{
	ASSERT(Valid(i_env));
	return s_entry[i_env].d_briefingScreen;
}

/***************************************************************************/
/***************************************************************************/
const char* CGameEnv::GetMissionText(int i_env)
{
	ASSERT(Valid(i_env));
	return s_entry[i_env].d_missionText;
}

/***************************************************************************/
/***************************************************************************/
int CGameEnv::GetType(int i_env)
{
	ASSERT(Valid(i_env));

	return s_entry[i_env].d_type;
}

/***************************************************************************/
/***************************************************************************/
bool CGameEnv::Valid(int i_env)
{
	return (i_env >= 0 && i_env < s_envCount);
}

/***************************************************************************/
/***************************************************************************/
int CGameEnv::GetTypeCount( Type i_type, const char *szModeName )
{
	int iCount = 0;
	for ( int i = 0; i < s_envCount; ++i )
	{
#ifdef _DEBUG
		bool bModeNamePass = true;
#else
		bool bModeNamePass = (
			( szModeName == NULL ) ||
			( ( szModeName != NULL ) && ( s_CheckMapForMode( s_entry[ i ].d_fileName, szModeName ) ) )
			);
#endif
		//bool bModeNamePass = (
		//	( szModeName == NULL ) ||
		//	( ( szModeName != NULL ) && ( s_CheckMapForMode( s_entry[ i ].d_fileName, szModeName ) ) )
		//);
		if ( ( ( s_entry[ i ].d_type & i_type ) != 0 ) && ( bModeNamePass ) )
		{
			++iCount;
		}
	}

	return( iCount );
}

/***************************************************************************/
/***************************************************************************/
int CGameEnv::ParseWorldName(const char* i_name, Type i_type)
{
	char seps[] = " ,\t\n\"\'";
	char name[MAX_NAME_LEN];
	strncpy(name, i_name, sizeof(name));
	char* world = strtok(name, seps);

	return GetByName(world, i_type);
}

/***************************************************************************/
/***************************************************************************/
int CGameEnv::GetByName(const char* i_name, Type i_type)
{
	int best = -1;
	for (int i = 0; i < s_envCount; i++)
	{
		if ((i_type & s_entry[i].d_type) != 0)
		{
			if (strcmpi(GetWorldDisplayName(i), i_name) == 0)
				return i;
			else if (strstri(GetWorldDisplayName(i), i_name))
				best = i;
		}
	}

	return(best);
}

/***************************************************************************/
/***************************************************************************/
int CGameEnv::GetByFilename(const char* i_name, Type i_type)
{
	for (int i = 0; i < s_envCount; i++)
	{
		if ((i_type & s_entry[i].d_type) != 0 && strstri(GetWorldFileName(i), i_name))
			return(i);
	}

	return(-1);
}

/***************************************************************************/
// Get a world by type (pass 0 for type if you want all of them)
/***************************************************************************/
int CGameEnv::GetByType(
int i_index,
Type i_type,
const char *szModeName)
{
	// Doh
	ASSERT(i_index >= 0);

	// Scan the list
	for (int i = 0; i < s_envCount; i++)
	{
#ifdef _DEBUG
		bool bModeNamePass = true;
#else
		bool bModeNamePass = (
			( szModeName == NULL ) ||
			( ( szModeName != NULL ) && ( s_CheckMapForMode( s_entry[ i ].d_fileName, szModeName ) ) )
			);
#endif
		//bool bModeNamePass = (
		//	( szModeName == NULL ) ||
		//	( ( szModeName != NULL ) && ( s_CheckMapForMode( s_entry[ i ].d_fileName, szModeName ) ) )
		//);
		if ( ((i_type & s_entry[i].d_type) != 0) && ( bModeNamePass ) )
		{
			if (!i_index)
				return i;
			--i_index;
		}
	}

	return -1;
}

/***************************************************************************/
/***************************************************************************/
int CGameEnv::GetNextByType(
int i_index,
Type i_type,
const char *szModeName)
{
	int iStartIndex = i_index;

	while ( ++i_index != iStartIndex )
	{
		if ( i_index >= s_envCount )
		{
			i_index = 0;
		}

#ifdef _DEBUG
		bool bModeNamePass = true;
#else
		bool bModeNamePass = (
			( szModeName == NULL ) ||
			( ( szModeName != NULL ) && ( s_CheckMapForMode( s_entry[ i_index ].d_fileName, szModeName ) ) )
			);
#endif
		//bool bModeNamePass = (
		//	( szModeName == NULL ) ||
		//	( ( szModeName != NULL ) && ( s_CheckMapForMode( s_entry[ i_index ].d_fileName, szModeName ) ) )
		//);
		if ( ( ( i_type & s_entry[ i_index ].d_type ) != 0 ) && bModeNamePass )
		{
			break;
		}
	}

	//do {
	//	if (++i_index >= s_envCount)
	//		i_index = 0;
	//} while ((i_type & s_entry[i_index].d_type) == 0);

	return i_index;
}

/***************************************************************************/
/***************************************************************************/
int CGameEnv::GetPreviousByType(
int i_index,
Type i_type,
const char *szModeName)
{
	int iStartIndex = i_index;

	while ( --i_index != iStartIndex )
	{
		if ( i_index < 0 )
		{
			i_index = ( s_envCount - 1 );
		}

#ifdef _DEBUG
		bool bModeNamePass = true;
#else
		bool bModeNamePass = (
			( szModeName == NULL ) ||
			( ( szModeName != NULL ) && ( s_CheckMapForMode( s_entry[ i_index ].d_fileName, szModeName ) ) )
			);
#endif
		//bool bModeNamePass = (
		//	( szModeName == NULL ) ||
		//	( ( szModeName != NULL ) && ( s_CheckMapForMode( s_entry[ i_index ].d_fileName, szModeName ) ) )
		//);
		if ( ( ( i_type & s_entry[ i_index ].d_type ) != 0 ) && bModeNamePass )
		{
			break;
		}
	}
	//do {
	//	if (--i_index < 0)
	//		i_index = s_envCount - 1;
	//} while ((i_type & s_entry[i_index].d_type) == 0);

	return i_index;
}

/***************************************************************************/
// Scan string for occurrences of key
/***************************************************************************/
const char *CGameEnv::strstri(
const char *string,
const char *key)
{
	int len = strlen(string);
	int keylen = strlen(key);

	for (int i = 0; i < (len - keylen) + 1; i++)
	{
		int compare;
		for (compare = 0;
				compare < keylen && tolower(string[i + compare]) == tolower(key[compare]);
				compare++);

		// If we made the full scan, we have a match
		if (compare == keylen)
			return &string[i];
	}

	// No match
	return NULL;
}

/***************************************************************************/
/***************************************************************************/
void CGameEnv::CChangeConfig::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	int iOptionIndex = g_configmgr.GetOptionIndexByName( "gameMap" );
	if ( iOptionIndex >= 0 )
	{
		d_ref->Set( atoi( g_configmgr.GetValue( iOptionIndex ) ) );
	}
}
