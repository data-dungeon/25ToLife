/********************************************************************
	created:	2004/09/27
	created:	27:9:2004   21:59
	filename: 	c:\dev\Ttl\code\environ\PlayerSpawnMgr.h
	file path:	c:\dev\Ttl\code\environ
	file base:	PlayerSpawnMgr
	file ext:	h
	author:		Shon C. Love
	
	purpose:	An object to manager player spawn points.
*********************************************************************/

#if !defined( _PLAYERSPAWNMGR_H )
#define _PLAYERSPAWNMGR_H

#include "EngineHelper/EngineHelperPCH.h"
#include "container/pool.h"
#include "container/dllist.h"
#include "EngineHelper/Singleton.h"
#include "sim/TeamingModel.h"
#include "EngineHelper/drawutil.h"

class PlayerSpawnMgr
{
public:
	enum ESpawnResult
	{
		NONE_AVAILABLE,
		ALL_OCCUPIED,
		FOUND_VACANCY
	};

	PlayerSpawnMgr( void );
	~PlayerSpawnMgr( void );

	ESpawnResult GetRandomSpawnPoint(uint8 i_participantId, Vector3 &o_location,
			float &o_heading, bool i_ignoreOccupancy = false);
	ESpawnResult GetBestSpawnPoint(uint8 i_participantId, Vector3 &o_location,
			float &o_heading, bool i_ignoreOccupancy = false);

	// multi-player spawn boxes
	void AddBox( CInstance *pInstance, CTeamingModel::ERole role, const char *szGameMode );

	// single-player spawn box
	void AddBox( const Vector3 &vPosition, const Vector3 &vFacing );

	void ClearBoxes( void );

	void ControlSpawn(Vector3CRef i_pos, CTeamingModel::ERole i_role);
	void ControlSpawn(Vector3CRef i_pos, uint8 i_participantId);
	CTeamingModel::ERole FindRoleOfNearestSpawnPoint( Vector3CRef pos );
	int HowManySpawnPoints(uint8 i_participantId);
	int HowManyRoleSpawnPoints(CTeamingModel::ERole i_role);
	int TotalSpawnPoints(void);

	// draw all the boxes for debugging
#ifndef CONSUMER_BUILD
	void Draw(Vector3CRef i_center);
#endif

protected:
	struct PlayerSpawnBox
	{
		CObbEntity			m_ObbEntity;
		float					m_Offset;
		float					m_desirability;
		bool					m_considered;

		// Members required to use DoublyLinkedList container.
		PlayerSpawnBox*	prev;
		PlayerSpawnBox*	next;
	};

	typedef DoublyLinkedList<PlayerSpawnBox> TSpawnPointList;

	TSpawnPointList* Reset(uint8 i_participantId);
	void ComputeDesirability(TSpawnPointList &i_spawnPoints,
			uint8 i_participantId);
	PlayerSpawnBox* GetDesirable(TSpawnPointList &i_spawnPoints);
	PlayerSpawnBox* GetRandom(TSpawnPointList &i_spawnPoints);
	bool TestVacancy(CObbEntity &i_obbEntity, uint8 i_participantId);
	PlayerSpawnBox* FindNearestInList(Vector3CRef i_pos,
			TSpawnPointList &i_spawnPoints, float &o_distSq);
	PlayerSpawnBox* FindNearest(Vector3CRef i_pos, int &o_list);
	void DrawBox(PlayerSpawnBox* i_box, DrawUtility::COLOR i_color);

	Pool<PlayerSpawnBox> m_SpawnPointPool;
	TSpawnPointList m_SpawnPointList[CTeamingModel::NUM_ROLES + 1];
};

DefineSingleton( PlayerSpawnMgr )
#define g_PlayerSpawnMgr GetSingleton( PlayerSpawnMgr )

#endif	// _PLAYERSPAWNMGR_H
