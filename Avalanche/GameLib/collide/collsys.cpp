/***************************************************************************/
// The master collision system for the entire level.  Wraps up the OBB and
// terrain checks into simple, unified calls.  Also allows the individual
// objects to be touched.
/***************************************************************************/
#include "collide/CollidePCH.h"

#include "collide/collsys.h"
#include "collide/cpsys.h"
#include "Layers/Timing.h"
#include "CollideHelper/XZEntity.h"

// A global for accessing this bad-boy
DeclareSingleton(CollisionSystem);

// The collision stack size
#define COLLISION_STACK_SIZE (384 * 1024)

/***************************************************************************/
// Create and destroy
/***************************************************************************/
CollisionSystem::CollisionSystem(void) :
	d_terrainEntity(d_layerMask),
	d_staticEntity(d_layerMask),
	d_dbList(true)
{
	// The all important layer mask
	d_layerMask = 1;

	// No layer change callback
	ClearLayerChangeCallback();
}

/***************************************************************************/
/***************************************************************************/
CollisionSystem::~CollisionSystem(void)
{
}

/***************************************************************************/
// A function to take a world database file and create a collision database file
/***************************************************************************/
void CollisionSystem::MakeDatabaseName(
const char *path,
const char *worldFile,
char *collisionDatabase)
{
	// Build the main part
	strcpy(collisionDatabase, path);
	if (collisionDatabase[strlen(collisionDatabase) - 1] != '\\')
		strcat(collisionDatabase, "\\");
	strcat(collisionDatabase, worldFile);
	strcat(collisionDatabase, "\\");
	strcat(collisionDatabase, worldFile);

	// Strip off the extension
	int len = strlen(collisionDatabase) - 1;
	int stopSearch = len - 4;
	while (len > 0 && len > stopSearch)
	{
		if (collisionDatabase[len] == '.')
		{
			collisionDatabase[len] = '\0';
			break;
		}
		--len;
	}
}

/***************************************************************************/
// Initialize everything
/***************************************************************************/
bool CollisionSystem::Initialize(t_Handle i_worldDblHandle)
{
	// Load in the tps
	int dataSize;
	d_partitionSystem = (CollidePartitionSystem*)dblGetChunkData(DB_CPH,
			DEFAULT_CHUNK_KEY_STRING, i_worldDblHandle, &dataSize);

	if (!d_partitionSystem || dataSize == 0)
	{
		// Can't load collision system data
		ASSERT(FALSE);
		return(false);
	}

#ifdef GCN
	// Monkey with the GCN
	d_partitionSystem->EndianSwap();
#endif

	// This is a good time to avert disaster!
	ASSERT(d_partitionSystem->version == CPS_VERSION);

	d_stack.Initialize(COLLISION_STACK_SIZE);
	d_terrainEntity.Initialize();
	d_staticEntity.Initialize(&d_stack);
	XZRect worldRect = d_partitionSystem->bounds.ToRect();
	d_dynamicEntities.Initialize(&worldRect);

	// Tell the XZEntity about the world extents
	CXZEntity::SetWorldExtents(d_partitionSystem->bounds.minV.Y() + 0.5f,
										d_partitionSystem->bounds.maxV.Y() - 0.5f);

	// Activate the root database
	Activate(0, i_worldDblHandle);

	// No layer change callback
	ClearLayerChangeCallback();

	return(true);
}

/***************************************************************************/
// Clean up everything
/***************************************************************************/
void CollisionSystem::Terminate(void)
{
	d_terrainEntity.Terminate();
	d_staticEntity.Terminate();
	d_dynamicEntities.Terminate();
	d_stack.Terminate();
	d_dbList.RemoveAll();
}

/***************************************************************************/
// Load a database
/***************************************************************************/
CollisionSystem::DBHandle CollisionSystem::Activate(
int partition,
t_Handle dblHandle)
{
	// Create the db
	Database *db = new Database;
	db->d_partition = partition;
	db->d_handle = dblHandle;
	db->d_terrain = d_terrainEntity.Activate(dblHandle);
	db->d_object = d_staticEntity.Activate(dblHandle);

	// put it in the tracking list
	d_dbList.AddTail(db);

	// Give them back the handle
	DBHandle handle = (DBHandle)db;
	return(handle);
}

/***************************************************************************/
// Resolve a dblHandle into a DBHandle
/***************************************************************************/
CollisionSystem::DBHandle CollisionSystem::GetHandle(
int partition,
t_Handle dblHandle)
{
	Database *db = d_dbList.Head();
	while (db)
	{
		// Scan the list
		if (db->d_partition == partition && db->d_handle == dblHandle)
			return((DBHandle)db);
		db = db->next;
	}

	// Didn't find it
	return(NULL);
}

/***************************************************************************/
// Unload a database
/***************************************************************************/
void CollisionSystem::Deactivate(
DBHandle handle)
{
	// Freeing NULL is okay
	if (!handle)
		return;

	// Make us into a database pointer
	Database *db = (Database *)handle;

	d_terrainEntity.Deactivate(db->d_terrain);
	d_staticEntity.Deactivate(db->d_object);

	// Kill ourselves
	d_dbList.Remove(db);
}

/***************************************************************************/
// What is the size of the world
// (will be incorrect if you haven't loaded one yet)
/***************************************************************************/
XZRect CollisionSystem::WorldSize()
{
	if (d_partitionSystem)
		return d_partitionSystem->bounds.ToRect();
	else
	{
		// Asking for the world size before the world is loaded, very bad
		ASSERT(false);
		return XZRect(-10000.0f, -10000.0f, 10000.0f, 10000.0f);
	}
}

/***************************************************************************/
// What is the size of the world
// (will be incorrect if you haven't loaded one yet)
/***************************************************************************/
AABB CollisionSystem::WorldBox()
{
	if (d_partitionSystem)
		return d_partitionSystem->bounds;
	else
	{
		// Asking for the world size before the world is loaded, very bad
		ASSERT(false);
		AABB box;
		box.MakeCube(Vector3(0.0f, 0.0f, 0.0f), 10000.0f);
		return box;
	}
}

/***************************************************************************/
/***************************************************************************/
bool CollisionSystem::EnableLayer(
int layer,
bool enable)
{
	// Make sure we are in range
	ASSERT(layer >= 0 && layer < 32);
	if (layer < 0 || layer >= 32)
		return(false);

	// Make the mask
	unsigned int mask = (1 << layer);

	// Toggle the bit
	if (enable)
		d_layerMask |= mask;
	else
		d_layerMask &= ~mask;

	// Tell the callback
	if (d_layerChangeCB)
		d_layerChangeCB(layer, enable, d_layerChangeContext);

	// This worked
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool CollisionSystem::RegisterLayerChangeCallback(
LayerChangeCallback cb,
void *context)
{
	// We only support one for right now
	if (d_layerChangeCB != NULL)
		return(false);

	d_layerChangeCB = cb;
	d_layerChangeContext = context;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool CollisionSystem::DeregisterLayerChangeCallback(
LayerChangeCallback cb)
{
	if (d_layerChangeCB != cb)
		return(false);

	ClearLayerChangeCallback();

	return(true);
}

/***************************************************************************/
// No layer change callback
/***************************************************************************/
void CollisionSystem::ClearLayerChangeCallback(void)
{
	d_layerChangeCB = NULL;
	d_layerChangeContext = NULL;
}

/***************************************************************************/
// Using a specified stack, possibly test a single entity against the
// collision system protagonists, the static entity and the terrain entity.
// Add the results to the provided collision list.
/***************************************************************************/
void CollisionSystem::Test(CCollisionList* i_list, CCollisionStack* i_stack,
		CStageEntity* i_queryEntity, CStageEntity::ESelect i_select)
{
	ASSERT((i_list != NULL) && (i_queryEntity != NULL));

	#ifdef COLLISION_SYSTEM_STATS
	TotalTestCount++;
	#endif

	Profiler__Start(PROFILE_COLLISION);

	bool isQuery = i_queryEntity->IsA(CQueryEntity::ClassType());
	if (isQuery)
		((CQueryEntity*)i_queryEntity)->BeginQuery();

	CCollisionEvent* event;
	CCollisionCull cull;

	if (i_queryEntity->ActorInteract() && i_queryEntity->GetCollisionCull(cull))
	{
		CStageEntity** dynamicList;
		int count = d_dynamicEntities.GetEntityList(cull, dynamicList);
		for (int i = 0; i < count; i++)
		{
			if ((event = CCollisionEvent::TestEntityPair(i_queryEntity,
					dynamicList[i], i_stack, i_select)) != NULL)
				i_list->AddEvent(event, true);
		}
	}

	if ((event = CCollisionEvent::TestEntityPair(i_queryEntity,
			&d_staticEntity, i_stack, i_select)) != NULL)
		i_list->AddEvent(event, true);

	if ((event = CCollisionEvent::TestEntityPair(i_queryEntity,
			&d_terrainEntity, i_stack, i_select)) != NULL)
		i_list->AddEvent(event, true);

	if (isQuery)
		((CQueryEntity*)i_queryEntity)->EndQuery();

	Profiler__Stop(PROFILE_COLLISION);
}

/***************************************************************************/
// Using a specified stack, possibly test an entity against another entity,
// the static entity and the terrain entity. Add the results to the provided
// collision list.
/***************************************************************************/
void CollisionSystem::Test(CCollisionList* i_list, CCollisionStack* i_stack,
		CStageEntity* i_queryEntity, CStageEntity* i_targetEntity,
		CStageEntity::ESelect i_select)
{
	ASSERT((i_list != NULL) && (i_queryEntity != NULL) &&
			(i_targetEntity != NULL));

	#ifdef COLLISION_SYSTEM_STATS
	TotalTestCount++;
	#endif

	Profiler__Start(PROFILE_COLLISION);

	bool isQuery = i_queryEntity->IsA(CQueryEntity::ClassType());
	if (isQuery)
		((CQueryEntity*)i_queryEntity)->BeginQuery();

	CCollisionEvent* event;

	if ((event = CCollisionEvent::TestEntityPair(i_queryEntity,
			i_targetEntity, i_stack, i_select)) != NULL)
		i_list->AddEvent(event, true);

	if ((event = CCollisionEvent::TestEntityPair(i_queryEntity,
			&d_staticEntity, i_stack, i_select)) != NULL)
		i_list->AddEvent(event, true);

	if ((event = CCollisionEvent::TestEntityPair(i_queryEntity,
			&d_terrainEntity, i_stack, i_select)) != NULL)
		i_list->AddEvent(event, true);

	if (isQuery)
		((CQueryEntity*)i_queryEntity)->EndQuery();

	Profiler__Stop(PROFILE_COLLISION);
}

/***************************************************************************/
// Using a specified stack, possibly test a single entity against the members
// of a cast (starting with a member of the cast), the static entity and the
// terrain entity. Add the results to the provided collision list.
/***************************************************************************/
void CollisionSystem::Test(CCollisionList* i_list, CCollisionStack* i_stack,
		CStageEntity* i_queryEntity, CCast* i_targetCast,
		CCastMember* i_firstMember, CStageEntity::ESelect i_select)
{
	ASSERT((i_list != NULL) && (i_queryEntity != NULL) &&
			((i_targetCast != NULL) || (i_firstMember == NULL)));

	#ifdef COLLISION_SYSTEM_STATS
	TotalTestCount++;
	#endif

	Profiler__Start(PROFILE_COLLISION);

	bool isQuery = i_queryEntity->IsA(CQueryEntity::ClassType());
	if (isQuery)
		((CQueryEntity*)i_queryEntity)->BeginQuery();

	CCollisionEvent* event;

	CCastMember* member = i_firstMember;
	while(member != NULL)
	{
		if ((event = CCollisionEvent::TestEntityPair(i_queryEntity,
				member->GetActor(), i_stack, i_select)) != NULL)
			i_list->AddEvent(event, true);
		member = i_targetCast->GetNextMember(member);
	}

	if ((event = CCollisionEvent::TestEntityPair(i_queryEntity,
			&d_staticEntity, i_stack, i_select)) != NULL)
		i_list->AddEvent(event, true);

	if ((event = CCollisionEvent::TestEntityPair(i_queryEntity,
			&d_terrainEntity, i_stack, i_select)) != NULL)
		i_list->AddEvent(event, true);

	if (isQuery)
		((CQueryEntity*)i_queryEntity)->EndQuery();

	Profiler__Stop(PROFILE_COLLISION);
}
