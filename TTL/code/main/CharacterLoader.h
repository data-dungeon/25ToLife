//============================================================================
//=
//= CharacterLoader.h - responsible for preloading, tracking, and creating
//=                     the main character models (build from pieces).
//=
//============================================================================

#pragma once

#ifndef _CHARACTERLOADER_H
#define _CHARACTERLOADER_H

namespace CharacterLoader
{
	void Initialize();
	void Terminate();
	void SetPlayerInitString( int playerId, const char* initString, const char* weapons );
	bool AllPlayersLoaded();
	void QueueLoads();
	void QueueLoads( int playerId );
	void ClearLoads();
	void Reset();
	void PauseLoads();
	void ResumeLoads();
};

#endif //_CHARACTERLOADER_H

//============================================================================
