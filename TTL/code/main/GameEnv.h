/*
**
**  File:   GameEnv.h
**  Date:   March 31, 2004
**  By:     Bryant Collard
**  Desc:   Wrap game environment stuff in a class.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/GameEnv.h $
**  $Revision: #30 $
**  $DateTime: 2005/08/09 03:53:16 $
**    $Author: Food $
**
*/

#pragma once

#ifndef GAME_ENV_H
#define GAME_ENV_H

#include "Loop/Environment.h"
#include "GameHelper/Message.h"

class CGameEnv : public CEnvironment
{
  public:
	// Construct
	CGameEnv();

	// The types of environments
	enum Type
	{
		TYPE_SINGLEPLAYER = 0x01,
		TYPE_MULTIPLAYER = 0x02,
		TYPE_DEBUG = 0x04,
		TYPE_HIDDEN = 0x08,
#ifdef CDROM
		TYPE_NORMALMP = (TYPE_MULTIPLAYER),
		TYPE_NORMALSP = (TYPE_SINGLEPLAYER),
		TYPE_ALL = (TYPE_SINGLEPLAYER | TYPE_MULTIPLAYER | TYPE_HIDDEN)
#else
		TYPE_NORMALMP = (TYPE_MULTIPLAYER | TYPE_DEBUG),
		TYPE_NORMALSP = (TYPE_SINGLEPLAYER | TYPE_DEBUG),
		TYPE_ALL = (TYPE_SINGLEPLAYER | TYPE_MULTIPLAYER | TYPE_DEBUG | TYPE_HIDDEN)
#endif
	};

	// How many environements do we have?
	static int Environments(Type i_type = TYPE_ALL);

	void Initialize();

	// Set and get the "current" environment.
	void Clear();
#ifdef DIRECTX_PC
	void SetCheckpoint( int checkpoint ) {d_checkPoint = checkpoint; }
	int  GetCheckpoint() { return d_checkPoint; }
	void SetCheckpointFlag( bool flag ) {b_checkPointFlag = flag;}
	bool GetCheckpointFlag() { return b_checkPointFlag; }
	//char* GetCdKeyPtr() { return cdkey; }
#endif;
	bool Set(int i_environment);
	int Get() const;
	bool Valid() const;
	bool Valid( int i_environment, Type i_type, const char *szModeName = NULL );

	// This is sort of hacky
	bool StartOfGame()
		{ return Get() == 0; }
	bool EndOfGame()
		{ return Get() == 11; }
	bool BonusLevel()
		{ return BonusLevel(Get()); }
	bool BonusLevel(int level)
		{ return (level == 12); }
	bool IsWeaponMaster()
		{ return IsWeaponMaster(Get()); }
	bool IsWeaponMaster(int forLevel);

	// Manage data preloading.
	bool ProcessPreload();
	static bool ClearPreload(void);

	// Get at the data for the current environment.
	const char* GetWorldFileName() const;
	const char* GetWorldDisplayName() const;
	const char* GetMusicList() const;
	const char* GetIntroMovie() const;
	const char* GetEndingMovie() const;
	const char* GetBriefingScreen() const;
	int GetType() const;

	// Get at the data for a given environment.
	static const char* GetWorldFileName(int i_env);
	static const char* GetWorldDisplayName(int i_env);
	static const char* GetWorldDescription(int i_env);
	static const char* GetMusicList(int i_env);
	static const char* GetIntroMovie(int i_env);
	static const char* GetEndingMovie(int i_env);
	static const char* GetBriefingScreen(int i_env);
	static const char* GetMissionText(int i_env);
	static int GetType(int i_env);
	static bool Valid(int i_env);
	static int GetTypeCount( Type i_type, const char *szModeName = NULL );

	// Find an environment given a string.
	static int ParseWorldName(const char* i_name, Type i_type = TYPE_ALL);

	static int GetByName(const char* i_name, Type i_type = TYPE_ALL);
	static int GetByFilename(const char* i_name, Type i_type = TYPE_ALL);

	// Get a world by type (pass 0 for type if you want all of them)
	static int GetByType(int i_index, Type i_type, const char *szModeName = NULL);
	static int GetNextByType(int i_index, Type i_type, const char *szModeName = NULL);
	static int GetPreviousByType(int i_index, Type i_type, const char *szModeName = NULL);

	CMessageOwner	d_messageOwner;
	REF_SEND_MESSAGE_HANDLER( CChangeConfig, CGameEnv )	d_changeConfigHandler;

  private:

	enum
	{
		ALLOWED_NONE    = 1,
		ALLOWED_DEBUG   = 2,
		ALLOWED_CD      = 4,
	};

	struct SGameEnvEntry
	{
		const char* d_displayName;
		const char* d_fileName;
		const char* d_musicName;
		const char* d_introMovie;
		const char* d_endingMovie;
		const char* d_briefingScreen;
		const char* d_missionText;
		int			d_type;
	};

	// The static data
	static const char* 	s_defaultMusic;
	static const int 		s_defaultAllowed;
	static SGameEnvEntry s_entry[];
	static int 				s_envCount;

	// The current environment
	int 						d_environment;
#ifdef DIRECTX_PC
	// the checkpoint to load when loading the level
	int							d_checkPoint;
	bool						b_checkPointFlag;
	//char						cdkey[19];
#endif

	// Scan
	static const char *CGameEnv::strstri(const char *string,	const char *key);
};

extern CGameEnv g_gameEnvironment;

#endif // GAME_ENV_H
