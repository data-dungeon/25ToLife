/********************************************************************
	created:	2004/09/27
	created:	27:9:2004   22:10
	filename: 	c:\dev\Ttl\code\PlayerSpawnMgr.cpp
	file path:	c:\dev\Ttl\code
	file base:	PlayerSpawnMgr
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implement a player spawn point database.
*********************************************************************/

#include "TTLPCH.h"
#include "environ/PlayerSpawnMgr.h"
#include "environ/ConfigMgr.h"
#include "main/TTL.h"

DeclareSingleton(PlayerSpawnMgr);

#define MAX_SPAWN_ADJUSTS 8

struct sSpawnAdjust
{
	float x, z;
};

static sSpawnAdjust s_spawnAdjust[MAX_SPAWN_ADJUSTS] =
{
	{ 0.0f, 0.0f },
	{ 1.0f, 0.0f },
	{-1.0f, 0.0f },
	{ 0.0f, 1.0f },
	{ 0.0f, -1.0f },
	{ 0.7071f, 0.7071f },
	{ -0.7071f, -0.7071f },
	{ -0.7071f, 0.7071f }
};

// ----------------------------------------------------------------------------
PlayerSpawnMgr::PlayerSpawnMgr(void) :
	m_SpawnPointPool(0, 1)
{
	for(int i = 0; i < CTeamingModel::NUM_ROLES + 1; i++)
	{
		m_SpawnPointList[i].CallDestructors(false);
	}
}

// ----------------------------------------------------------------------------
PlayerSpawnMgr::~PlayerSpawnMgr(void)
{
	ClearBoxes();
}

// ----------------------------------------------------------------------------
PlayerSpawnMgr::ESpawnResult PlayerSpawnMgr::GetRandomSpawnPoint(
		uint8 i_participantId, Vector3 &o_location, float &o_heading,
		bool i_ignoreOccupancy)
{
	ESpawnResult result = NONE_AVAILABLE;
	TSpawnPointList* spawnPoints = Reset(i_participantId);
	if (spawnPoints != NULL)
	{
		PlayerSpawnBox* pBox;
		while((pBox = GetRandom(*spawnPoints)) != NULL)
		{
			result = ALL_OCCUPIED;
			if (i_ignoreOccupancy ||
					TestVacancy(pBox->m_ObbEntity, i_participantId))
			{
				Vector3 facing = pBox->m_ObbEntity.Obb().Orientation().GetRow2();
				o_heading = Math::ArcTan2(facing.X(), facing.Z());
				o_location = pBox->m_ObbEntity.Obb().Position();
				uint8 adjust = i_participantId % MAX_SPAWN_ADJUSTS;
				o_location.X(o_location.X() + s_spawnAdjust[adjust].x);
				o_location.Y(o_location.Y() + pBox->m_Offset);
				o_location.Z(o_location.Z() + s_spawnAdjust[adjust].z);
				return(FOUND_VACANCY);
			}
		}
	}
	return(result);
}

// ----------------------------------------------------------------------------
PlayerSpawnMgr::ESpawnResult PlayerSpawnMgr::GetBestSpawnPoint(
		uint8 i_participantId, Vector3 &o_location, float &o_heading,
		bool i_ignoreOccupancy)
{
	ESpawnResult result = NONE_AVAILABLE;
	TSpawnPointList* spawnPoints = Reset(i_participantId);
	if (spawnPoints != NULL)
	{
		ComputeDesirability(*spawnPoints, i_participantId);

		PlayerSpawnBox* pBox;
		while((pBox = GetDesirable(*spawnPoints)) != NULL)
		{
			result = ALL_OCCUPIED;
			if (i_ignoreOccupancy ||
					TestVacancy(pBox->m_ObbEntity, i_participantId))
			{
				Vector3 facing = pBox->m_ObbEntity.Obb().Orientation().GetRow2();
				o_heading = Math::ArcTan2(facing.X(), facing.Z());
				o_location = pBox->m_ObbEntity.Obb().Position();
				uint8 adjust = i_participantId % MAX_SPAWN_ADJUSTS;
				o_location.X(o_location.X() + s_spawnAdjust[adjust].x);
				o_location.Y(o_location.Y() + pBox->m_Offset);
				o_location.Z(o_location.Z() + s_spawnAdjust[adjust].z);
				return(FOUND_VACANCY);
			}
		}
	}
	return(result);
}


// ----------------------------------------------------------------------------
PlayerSpawnMgr::TSpawnPointList* PlayerSpawnMgr::Reset(uint8 i_participantId)
{
	TSpawnPointList* spawnPoints;
	CTeamingModel::ERole role =
			g_referee.GetTeamingModel().GetPlayerRole(i_participantId);
	if (role != CTeamingModel::INVALID_ROLE)
	{
		spawnPoints = &m_SpawnPointList[(int)role];
		if (spawnPoints->Items() > 0)
		{
			PlayerSpawnBox* pBox = spawnPoints->Head();
			while(pBox != NULL)
			{
				pBox->m_desirability = 0.0f;
				pBox->m_considered = false;
				pBox = pBox->next;
			}
			return(spawnPoints);
		}
	}

	// Look in the universal list.
	spawnPoints = &m_SpawnPointList[(int)CTeamingModel::NUM_ROLES];
	if (spawnPoints->Items() > 0)
	{
		PlayerSpawnBox* pBox = spawnPoints->Head();
		while(pBox != NULL)
		{
			pBox->m_desirability = 0.0f;
			pBox->m_considered = false;
			pBox = pBox->next;
		}
		return(spawnPoints);
	}

	return(NULL);
}

// ----------------------------------------------------------------------------
void PlayerSpawnMgr::ComputeDesirability(TSpawnPointList &i_spawnPoints,
		uint8 i_participantId)
{
	// Get the teaming model.
	CTeamingModel &teaming = g_referee.GetTeamingModel();

	// Find each enemy.
	uint8 enemyId = INVALID_PARTICIPANT;
	while((enemyId = teaming.GetNextEnemy(i_participantId, enemyId, false,
			false, false)) != INVALID_PARTICIPANT)
	{
		CActor* enemy = g_referee.GetActor(enemyId);
		if ((enemy != NULL) && g_referee.IsAlive(enemyId))
		{
			// Apply the enemy to each available spawn point.
			PlayerSpawnBox* pBox = i_spawnPoints.Head();
			while(pBox != NULL)
			{
				// Enemies closer than 3m contribute equally.
				float distSq =	(pBox->m_ObbEntity.Obb().Position() -
						enemy->GetBodyInWorld()).LengthSquared();
				if (distSq < 9.0f)
					distSq = 9.0f;
				pBox->m_desirability += 1.0f / distSq;
				pBox = pBox->next;
			}
		}
	}
}

// ----------------------------------------------------------------------------
PlayerSpawnMgr::PlayerSpawnBox* PlayerSpawnMgr::GetDesirable(
		TSpawnPointList &i_spawnPoints)
{
	float bestWeight = MAXFLOAT;
	PlayerSpawnBox* bestBox = NULL;
	int bias = 1;

	// Try each point.
	PlayerSpawnBox* pBox = i_spawnPoints.Head();
	while(pBox != NULL)
	{
		// Reject those already considered.
		if (!pBox->m_considered)
		{
			// If this point is better than anything found so far, choose it. Note
			// that the bias is reset to 1. 
			if (pBox->m_desirability < bestWeight)
			{
				bestWeight = pBox->m_desirability;
				bestBox = pBox;
				bias = 1;
			}

			// See if this point is just as desirable as the best found so far.
			else if (pBox->m_desirability == bestWeight)
			{
				// In order for the first of many equally desirable points to have
				// an equal chance of being selected, its chances of be chosen over
				// some newly found point must increase. In fact, the ratio of the
				// number of times it is chosen versus the number of times the
				// newcomer is chosen must double each time. The first time it is
				// tested, it has a 1:1 (1 out of 2) chance of being chosen. The
				// next time, a 2:1 (2 out of 3) chance, then 4:1 (4 out of 5), etc.
				if (g_random.UpTo(bias) != 0)
					bias *= 2;

				// If the newcomer won, select it and reset the bias (note that it
				// already passed the first test).
				else
				{
					bestWeight = pBox->m_desirability;
					bestBox = pBox;
					bias = 2;
				}
			}
		}
		pBox = pBox->next;
	}

	if (bestBox != NULL)
		bestBox->m_considered = true;
	return(bestBox);
}

// ----------------------------------------------------------------------------
PlayerSpawnMgr::PlayerSpawnBox* PlayerSpawnMgr::GetRandom(
		TSpawnPointList &i_spawnPoints)
{
	int count = 0;
	PlayerSpawnBox* pBox = i_spawnPoints.Head();
	while(pBox != NULL)
	{
		if (!pBox->m_considered)
			count++;
		pBox = pBox->next;
	}

	if (count > 0)
	{
		count = g_random.UpToExcludeHigh(count);
		pBox = i_spawnPoints.Head();
		while(pBox != NULL)
		{
			if (!pBox->m_considered)
			{
				if (count == 0)
				{
					pBox->m_considered = true;
					return(pBox);
				}
				count--;
			}
			pBox = pBox->next;
		}
		ASSERT(false);
	}
	return(NULL);
}

// ----------------------------------------------------------------------------
bool PlayerSpawnMgr::TestVacancy(CObbEntity &i_obbEntity, uint8 i_participantId)
{
	i_obbEntity.SetIgnoreEntity(g_referee.GetActor(i_participantId));
	CCollisionEvent* event = NULL;
	g_collisionStack.Reset();
	CCollisionList* list = g_collisionSystem.Test(&i_obbEntity);
	if (list != NULL)
	{
		event = list->GetHeadEvent();
		while(event != NULL)
		{
			uint8 id = CParticipantIF::GetParticipantIdFromActor((CActor*)event->GetTargetEntity());
			if (g_referee.IsAlive(id) && g_referee.GetTeamingModel().IsPlayerEnemy(id) )
				break;
			event = event->GetNext();
		}
	}
	return(event == NULL);
}

// ----------------------------------------------------------------------------
void PlayerSpawnMgr::AddBox(CInstance *pInstance, CTeamingModel::ERole role,
		const char *szGameMode)
{
	// ignore irrelevant spawn boxes
	if ((pInstance == NULL) || ((szGameMode != NULL) &&
			!g_configmgr.TestOptionValue("gameModes", szGameMode)) ||
			(role == CTeamingModel::INVALID_ROLE))
		return;

	PlayerSpawnBox *pBox = m_SpawnPointPool.New();

	AABB aabb;
	Vector3 pos;
	DirCos3x3 orien;
	Vector3 size = 0.5f * (pInstance->BoundingBox().Max - pInstance->BoundingBox().Min);
	aabb.Set(-size, size);
	pBox->m_ObbEntity.Obb().SetBoundingVolume(&aabb);
	float scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ;
	pInstance->Matrix().Deconstruct(orien, pos, scaleX, scaleY, scaleZ,
			shearXY, shearXZ, shearYZ);
	pBox->m_ObbEntity.Obb().SetTransform(orien, pos);
	pBox->m_ObbEntity.SetPositiveInclude(ACTOR_CATEGORY);

	CLineEntity entity;
	CLineQuery groundCollision;
	entity.SetPositiveInclude(WORLD_CATEGORY);
	entity.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	entity.Line().SetOriginBasis(Vector3(pos.X(), pos.Y() + size.Y(), pos.Z()),
			Vector3(0.0f, -1.0f, 0.0f));
	entity.Line().SetBoundary(CLineCollisionObject::BOUNDARY_MIN_MAX, 0.0f, 5.0f);
	g_collisionSystem.ResetStack();
	if (groundCollision.Test(entity))
		pBox->m_Offset = groundCollision.intersection.Y() - pos.Y();
	else
		pBox->m_Offset = 0.0f;

	m_SpawnPointList[(int)role].AddTail(pBox);
}

// ----------------------------------------------------------------------------
void PlayerSpawnMgr::AddBox(const Vector3 &vPosition, const Vector3 &vFacing)
{
	// only call this in a single player game
	if (g_ttl->IsSinglePlayer())
	{
		ClearBoxes();

		PlayerSpawnBox *pBox = m_SpawnPointPool.New();

		AABB aabb;
		DirCos3x3 orien;
		Vector3 size(0.2f, 0.2f, 0.2f);
		aabb.Set(-size, size);
		pBox->m_ObbEntity.Obb().SetBoundingVolume(&aabb);
		orien.SetYRotation(Math::ArcTan2(vFacing.x(), vFacing.z()));
		pBox->m_ObbEntity.Obb().SetTransform(orien, vPosition);
		pBox->m_ObbEntity.SetPositiveInclude(ACTOR_CATEGORY);

		CLineEntity entity;
		CLineQuery groundCollision;
		entity.SetPositiveInclude(WORLD_CATEGORY);
		entity.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
		entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		entity.Line().SetOriginBasis(Vector3(vPosition.X(), vPosition.Y() +
				size.Y(), vPosition.Z()),	Vector3(0.0f, -1.0f, 0.0f));
		entity.Line().SetBoundary(CLineCollisionObject::BOUNDARY_MIN_MAX, 0.0f,
				5.0f);
		g_collisionSystem.ResetStack();
		if (groundCollision.Test(entity))
			pBox->m_Offset = groundCollision.intersection.Y() - vPosition.Y();
		else
			pBox->m_Offset = 0.0f;

		// Put the box in the universal list.
		m_SpawnPointList[CTeamingModel::NUM_ROLES].AddTail(pBox);
	}
}

// ----------------------------------------------------------------------------
void PlayerSpawnMgr::ClearBoxes(void)
{
	// dumps all our box data, but does not free memory
	for(int i = 0; i < CTeamingModel::NUM_ROLES + 1; i++)
	{
		PlayerSpawnBox* pBox;
		while((pBox = m_SpawnPointList[i].Head()) != NULL)
		{
			m_SpawnPointList[i].Unlink(pBox);
			m_SpawnPointPool.Delete(pBox);
		}
	}
}

// ----------------------------------------------------------------------------
PlayerSpawnMgr::PlayerSpawnBox* PlayerSpawnMgr::FindNearestInList(
		Vector3CRef i_pos, TSpawnPointList &i_spawnPoints, float &o_distSq)
{
	float distSq;
	PlayerSpawnBox* bestBox = NULL;

	o_distSq = MAXFLOAT;
	PlayerSpawnBox* pBox = i_spawnPoints.Head();
	while(pBox != NULL)
	{
		distSq = (pBox->m_ObbEntity.Obb().Position() - i_pos).LengthSquared();
		if (distSq < o_distSq)
		{
			o_distSq = distSq;
			bestBox = pBox;
		}
		pBox = pBox->next;
	}

	return(bestBox);
}

// ----------------------------------------------------------------------------
PlayerSpawnMgr::PlayerSpawnBox* PlayerSpawnMgr::FindNearest(Vector3CRef i_pos,
		int &o_list)
{
	float distSq;
	PlayerSpawnBox* bestBox = NULL;
	float bestDistSq = MAXFLOAT;

	o_list = -1;
	for(int i = 0; i < CTeamingModel::NUM_ROLES + 1; i++)
	{
		PlayerSpawnBox* pBox = FindNearestInList(i_pos, m_SpawnPointList[i],
				distSq);
		if ((pBox != NULL) && (distSq < bestDistSq))
		{
			bestDistSq = distSq;
			bestBox = pBox;
			o_list = i;
		}
	}

	return(bestBox);
}

// ----------------------------------------------------------------------------
void PlayerSpawnMgr::ControlSpawn(Vector3CRef i_pos, CTeamingModel::ERole i_role)
{
	if (i_role != CTeamingModel::INVALID_ROLE)
	{
		int list;
		PlayerSpawnBox* pBox = FindNearest(i_pos, list);

		if ((pBox != NULL) && (list != (int)i_role))
		{
			m_SpawnPointList[list].Unlink(pBox);
			m_SpawnPointList[(int)i_role].AddTail(pBox);
		}
	}
}

// ----------------------------------------------------------------------------
void PlayerSpawnMgr::ControlSpawn(Vector3CRef i_pos, uint8 i_participantId)
{
	ControlSpawn(i_pos,
			g_referee.GetTeamingModel().GetPlayerRole(i_participantId));
}

// ----------------------------------------------------------------------------
CTeamingModel::ERole PlayerSpawnMgr::FindRoleOfNearestSpawnPoint(Vector3CRef pos)
{
	int index;
	PlayerSpawnBox* pBox = FindNearest(pos, index);
	if (pBox != NULL)
		return((CTeamingModel::ERole)index);
	return(CTeamingModel::INVALID_ROLE);
}

// ----------------------------------------------------------------------------
int PlayerSpawnMgr::HowManySpawnPoints(uint8 i_participantId)
{
	int baseCount = m_SpawnPointList[(int)CTeamingModel::NUM_ROLES].Items();

	CTeamingModel::ERole role =
			g_referee.GetTeamingModel().GetPlayerRole(i_participantId);
	if (role != CTeamingModel::INVALID_ROLE)
		return(m_SpawnPointList[(int)role].Items() + baseCount);

	return(baseCount);
}

// ----------------------------------------------------------------------------
int PlayerSpawnMgr::HowManyRoleSpawnPoints(CTeamingModel::ERole i_role)
{
	if (i_role != CTeamingModel::INVALID_ROLE)
		return(m_SpawnPointList[(int)i_role].Items());
	return(0);
}

// ----------------------------------------------------------------------------
int PlayerSpawnMgr::TotalSpawnPoints(void)
{
	int count = 0;
	for(int i = 0; i < CTeamingModel::NUM_ROLES + 1; i++)
	{
		count += m_SpawnPointList[i].Items();
	}
	return(count);
}

#ifndef CONSUMER_BUILD
// ----------------------------------------------------------------------------
void PlayerSpawnMgr::Draw(Vector3CRef i_center)
{
	// CDROM builds have a very small line buffer. Be pretty rude and throw
	// away any lines already drawn this frame.
#if defined(CDROM)
	gfxClearLineBuffer();
#endif

	// Hijack the desirability field to hold the square of the distance from the
	// center. Search for the closest at the same time.
	PlayerSpawnBox* closestBox = NULL;
	int closestList = -1;
	float closestDistSq = MAXFLOAT;
	int i;
	for(i = 0; i < CTeamingModel::NUM_ROLES + 1; i++)
	{
		PlayerSpawnBox* pBox = m_SpawnPointList[i].Head();
		while(pBox != NULL)
		{
			pBox->m_considered = false;
			pBox->m_desirability =
					(pBox->m_ObbEntity.Obb().Position() - i_center).LengthSquared();
			if (pBox->m_desirability < closestDistSq)
			{
				closestBox = pBox;
				closestList = i;
				closestDistSq = pBox->m_desirability;
			}
			pBox = pBox->next;
		}
	}

	// Draw the boxes from closest out.
	while(closestBox != NULL)
	{
		if ((CTeamingModel::ERole)closestList == CTeamingModel::BLUE)
			DrawBox(closestBox, DrawUtility::BLUE);
		else if ((CTeamingModel::ERole)closestList == CTeamingModel::RED)
			DrawBox(closestBox, DrawUtility::RED);
		else if ((CTeamingModel::ERole)closestList == CTeamingModel::AMBIENT)
			DrawBox(closestBox, DrawUtility::WHITE);
		else if ((CTeamingModel::ERole)closestList == CTeamingModel::NUM_ROLES)
			DrawBox(closestBox, DrawUtility::GREEN);
		else
			DrawBox(closestBox, DrawUtility::YELLOW);
		closestBox->m_considered = true;

		closestBox = NULL;
		closestList = -1;
		closestDistSq = MAXFLOAT;
		for(i = 0; i < CTeamingModel::NUM_ROLES + 1; i++)
		{
			PlayerSpawnBox* pBox = m_SpawnPointList[i].Head();
			while(pBox != NULL)
			{
				if (!pBox->m_considered && (pBox->m_desirability < closestDistSq))
				{
					closestBox = pBox;
					closestList = i;
					closestDistSq = pBox->m_desirability;
				}
				pBox = pBox->next;
			}
		}
	}
}

// ----------------------------------------------------------------------------
void PlayerSpawnMgr::DrawBox(PlayerSpawnBox* i_box, DrawUtility::COLOR i_color)
{
	Vector3 offset, corner1, corner2;
	OBBInstance &obb = i_box->m_ObbEntity.Obb();
	const AABB &aabb = obb.GetConstAABB();

	obb.Draw(i_color);

	offset.Set(0.0f, i_box->m_Offset, 0.0f);
	DrawUtility::Point(obb.Position() + offset, i_color, 0.1f);

	corner1.Set(aabb.maxV.X(), aabb.maxV.Y(), aabb.maxV.Z());
	corner2.Set(aabb.minV.X(), aabb.minV.Y(), aabb.maxV.Z());
	corner1 = obb.Position() + corner1 * obb.Orientation();
	corner2 = obb.Position() + corner2 * obb.Orientation();
	DrawUtility::Line(corner1, corner2, i_color);

	corner1.Set(aabb.minV.X(), aabb.maxV.Y(), aabb.maxV.Z());
	corner2.Set(aabb.maxV.X(), aabb.minV.Y(), aabb.maxV.Z());
	corner1 = obb.Position() + corner1 * obb.Orientation();
	corner2 = obb.Position() + corner2 * obb.Orientation();
	DrawUtility::Line(corner1, corner2, i_color);
}
#endif