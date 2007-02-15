/***************************************************************************/
// The master collision system for the entire level.  Wraps up the OBB and
// terrain checks into simple, unified calls.  Also allows the individual
// objects to be touched.
/***************************************************************************/

#include "collide/List.h"
#include "collide/Event.h"
#include "collide/QueryEntity.h"
#include "stage/cast.h"
#include "stage/castmember.h"
#include "stage/cactor.h"

/***************************************************************************/
// Various test flavors. Each of the tests pairs up various entities and
// tests the pair. One member of a pair is a query entity and the other
// is a target entity.
/***************************************************************************/

/***************************************************************************/
// Possibly test a single entity against the collision system protagonists,
// the static entity and the terrain entity using a specified stack. Get
// back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CCollisionStack* i_stack,
		CStageEntity* i_queryEntity, CStageEntity::ESelect i_select)
{
	CCollisionList* list = CCollisionList::New(i_stack);
	if (list == NULL)
		return(NULL);

	Test(list, i_stack, i_queryEntity, i_select);

	if ((list != NULL) && list->Empty())
	{
		CCollisionStackItem::Delete(list);
		list = NULL;
	}

	return(list);
}

/***************************************************************************/
// Possibly test an entire cast against the collision system protagonists,
// the static entity and the terrain entity using a specified stack. Get
// back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CCollisionStack* i_stack,
		CCast* i_queryCast, CStageEntity::ESelect i_select)
{
	ASSERT(i_queryCast != NULL);

	CCollisionList* list = CCollisionList::New(i_stack);
	if (list == NULL)
		return(NULL);

	CCastMember* member = NULL;
	while((member = i_queryCast->GetNextMember(member)) != NULL)
		Test(list, i_stack, member->GetActor(), i_select);

	if ((list != NULL) && list->Empty())
	{
		CCollisionStackItem::Delete(list);
		list = NULL;
	}

	return(list);
}

/***************************************************************************/
// Possibly test a single entity against another entity, the static
// entity and the terrain entity using a specified stack. Get back a
// collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CCollisionStack* i_stack,
		CStageEntity* i_queryEntity, CStageEntity* i_targetEntity,
		CStageEntity::ESelect i_select)
{
	CCollisionList* list = CCollisionList::New(i_stack);
	if (list == NULL)
		return(NULL);

	Test(list, i_stack, i_queryEntity, i_targetEntity, i_select);

	if ((list != NULL) && list->Empty())
	{
		CCollisionStackItem::Delete(list);
		list = NULL;
	}

	return(list);
}

/***************************************************************************/
// Possibly test a single entity against a cast, the static entity and the
// terrain entity using a specified stack. Get back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CCollisionStack* i_stack,
		CStageEntity* i_queryEntity, CCast* i_targetCast,
		CStageEntity::ESelect i_select)
{
	ASSERT(i_targetCast != NULL);

	CCollisionList* list = CCollisionList::New(i_stack);
	if (list == NULL)
		return(NULL);

	Test(list, i_stack, i_queryEntity, i_targetCast,
			i_targetCast->GetNextMember(NULL), i_select);

	if ((list != NULL) && list->Empty())
	{
		CCollisionStackItem::Delete(list);
		list = NULL;
	}

	return(list);
}

/***************************************************************************/
// Possibly test each member of a cast against the other members of the cast,
// the static entity and the terrain entity using a specified stack. Get
// back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::TestCast(CCollisionStack* i_stack,
		CCast* i_cast, CStageEntity::ESelect i_select)
{
	ASSERT(i_cast != NULL);

	CCollisionList* list = CCollisionList::New(i_stack);
	if (list == NULL)
		return(NULL);

	CCastMember* member = NULL;
	while((member = i_cast->GetNextMember(member)) != NULL)
	{
		Test(list, i_stack, member->GetActor(), i_cast,
				i_cast->GetNextMember(member), i_select);
	}

	if ((list != NULL) && list->Empty())
	{
		CCollisionStackItem::Delete(list);
		list = NULL;
	}

	return(list);
}

/***************************************************************************/
// Possibly test the member of one cast against the members of another cast,
// the static entity and the terrain entity using a specified stack. Get
// back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CCollisionStack* i_stack,
		CCast* i_queryCast, CCast* i_targetCast, CStageEntity::ESelect i_select)
{
	ASSERT((i_queryCast != NULL) && (i_targetCast != NULL));

	CCollisionList* list = CCollisionList::New(i_stack);
	if (list == NULL)
		return(NULL);

	CCastMember* member = NULL;
	while((member = i_queryCast->GetNextMember(member)) != NULL)
	{
		Test(list, i_stack, member->GetActor(), i_targetCast,
				i_targetCast->GetNextMember(NULL), i_select);
	}

	if ((list != NULL) && list->Empty())
	{
		CCollisionStackItem::Delete(list);
		list = NULL;
	}

	return(list);
}

/***************************************************************************/
// Possibly test a single entity against the collision system protagonists,
// the static entity and the terrain entity using the collision system
// stack. Get back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CStageEntity* i_queryEntity,
		CStageEntity::ESelect i_select)
{
	return(Test(&d_stack, i_queryEntity, i_select));
}

/***************************************************************************/
// Possibly test an entire cast against the collision system protagonists,
// the static entity and the terrain entity using the collision system
// stack. Get back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CCast* i_queryCast,
		CStageEntity::ESelect i_select)
{
	return(Test(&d_stack, i_queryCast, i_select));
}

/***************************************************************************/
// Possibly test a single entity against another entity, the static
// entity and the terrain entity using the collision system stack. Get
// back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CStageEntity* i_queryEntity,
		CStageEntity* i_targetEntity, CStageEntity::ESelect i_select)
{
	return(Test(&d_stack, i_queryEntity, i_targetEntity, i_select));
}

/***************************************************************************/
// Possibly test a single entity against a cast, the static entity and the
// terrain entity using the collision system stack. Get back a collision
// list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CStageEntity* i_queryEntity,
		CCast* i_targetCast, CStageEntity::ESelect i_select)
{
	return(Test(&d_stack, i_queryEntity, i_targetCast, i_select));
}

/***************************************************************************/
// Possibly test each member of a cast against the other members of the cast,
// the static entity and the terrain entity using the collision system
// stack. Get back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::TestCast(CCast* i_cast,
		CStageEntity::ESelect i_select)
{
	return(TestCast(&d_stack, i_cast, i_select));
}

/***************************************************************************/
// Possibly test the member of one cast against the members of another cast,
// the static entity and the terrain entity using the collision system
// stack. Get back a collision list.
/***************************************************************************/
inline CCollisionList* CollisionSystem::Test(CCast* i_queryCast,
		CCast* i_targetCast, CStageEntity::ESelect i_select)
{
	return(Test(&d_stack, i_queryCast, i_targetCast, i_select));
}

/***************************************************************************/
/***************************************************************************/
inline void CollisionSystem::DrawTarget(CCollisionList* i_list)
{
	if (i_list != NULL)
	{
		CCollisionEvent* event = i_list->GetHeadEvent();
		while (event != NULL)
		{
			event->GetTargetEntity()->Draw(event);
			event = event->GetNext();
		}
	}
}

/***************************************************************************/
/***************************************************************************/
inline void CollisionSystem::Draw(CStageEntity* i_queryEntity,
		CStageEntity::ESelect i_select)
{
	CCollisionList* list = Test(&d_stack, i_queryEntity, i_select);
	DrawTarget(list);
	CCollisionStackItem::Delete(list);
}

/***************************************************************************/
/***************************************************************************/
inline void CollisionSystem::Draw(CStageEntity* i_queryEntity,
		CCast* i_targetCast, CStageEntity::ESelect i_select)
{
	CCollisionList* list = Test(&d_stack, i_queryEntity, i_targetCast, i_select);
	DrawTarget(list);
	CCollisionStackItem::Delete(list);
}

/***************************************************************************/
/***************************************************************************/
#ifdef COLLISION_SYSTEM_STATS
inline int CollisionSystem::GetTotalTestCount(void)
{
	// I hate resetting this in here, but the collision system doesnt really
	// know about the passage of time currently.
	int count = TotalTestCount;
	TotalTestCount = 0;
	return count;
}
#endif

