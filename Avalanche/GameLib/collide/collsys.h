/***************************************************************************/
// The master collision system for the entire level.  Wraps up the OBB and
// terrain checks into simple, unified calls.  Also allows the individual
// objects to be touched.
/***************************************************************************/
#ifndef COLLSYS_H
#define COLLSYS_H

// Engine includes //
#include "Layers/Assert/Assert.h"
#include "Layers/LayersStruct.h"

// Gamelib includes //
#include "collide/Stack.h"
#include "collide/TerrainEntity.h"
#include "collide/StaticEntity.h"
#include "collide/DynamicEntities.h"
#include "container/dllist.h"
#include "EngineHelper/Singleton.h"
#include "stage/StageEntity.h"

//#ifndef CDROM
#define COLLISION_SYSTEM_STATS
//#endif


// Forward Declarations
class CCast;
class CCastMember;
class CStageEntity;
class CProtagonist;
class CCollisionList;
class CollidePartitionSystem;
class TerrainCollideLayerList;
class ObjectLayerList;
template<class T> class CCollisionStackArray;

class CollisionSystem
{
  public:
	// Create and destroy
	CollisionSystem(void);
	~CollisionSystem(void);

	// A function to take a world database file and create a collision
	// database file
	static void MakeDatabaseName(const char *path, const char *worldFile,
			char *collisionDatabase);

	// Initialize everything
	bool Initialize(t_Handle i_worldDblHandle);

	// Clean up everything
	void Terminate(void);

	// Load a partition (NULL means an error)
	typedef void *DBHandle;
	DBHandle Activate(int partition, t_Handle dblHandle);

	// Resolve a dblHandle into a DBHandle
	DBHandle GetHandle(int partition, t_Handle dblHandle);

	// Unload partition
	void Deactivate(DBHandle handle);

	// What is the size of the world 
	// (will be incorrect if you haven't loaded one yet)
	XZRect WorldSize();
	AABB WorldBox();

	// Activate/deactivate a layer
	// (layer 0 is active by default, all others are not)
	bool EnableLayer(int layer, bool enable = true);
	bool DisableLayer(int layer)
		{ return(EnableLayer(layer, false)); }

	// Notification call-back for layer changes
	typedef void (*LayerChangeCallback)(int layer, bool active, void *context);
	bool RegisterLayerChangeCallback(LayerChangeCallback cb, void *context);
	bool DeregisterLayerChangeCallback(LayerChangeCallback cb);

	// When you add/remove protagonists from the world call these functions
	bool AddProtagonist(CProtagonist *protagonist)
		{return(d_dynamicEntities.Add((CStageEntity*)protagonist));}
	bool RemoveProtagonist(CProtagonist *protagonist)
		{return(d_dynamicEntities.Remove((CStageEntity*)protagonist));}

	// Call this once per frame, after your protagonists have moved
	void ProtagonistsMoved(void)
		{d_dynamicEntities.Update();}

	// Manage collision stack
	CCollisionStack& GetStack(void) {return(d_stack);}
	void ResetStack(void) {d_stack.Reset();}

	// Get at managed entities
	CTerrainEntity* TerrainEntity(void) {return(&d_terrainEntity);}
	CStaticEntity* StaticEntity(void) {return(&d_staticEntity);}
	CDynamicEntities* DynamicEntities(void) {return(&d_dynamicEntities);}

	// Various test flavors. Each of the tests pairs up various entities and
	// tests the pair. One member of a pair is a query entity and the other
	// is a target entity.

	// Using a specified stack, possibly test a single entity against the
	// collision system protagonists, the static entity and the terrain entity.
	// Add the results to the provided collision list.
	void Test(CCollisionList* i_list, CCollisionStack* i_stack,
			CStageEntity* i_queryEntity,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Using a specified stack, possibly test an entity against another entity,
	// the static entity and the terrain entity. Add the results to the provided
	// collision list.
	void Test(CCollisionList* i_list, CCollisionStack* i_stack,
			CStageEntity* i_queryEntity, CStageEntity* i_targetEntity,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Using a specified stack, possibly test a single entity against the members
	// of a cast (starting with a member of the cast), the static entity and the
	// terrain entity. Add the results to the provided collision list.
	void Test(CCollisionList* i_list, CCollisionStack* i_stack,
			CStageEntity* i_queryEntity, CCast* i_targetCast,
			CCastMember* i_firstMember,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test a single entity against the collision system protagonists,
	// the static entity and the terrain entity using a specified stack. Get
	// back a collision list.
	CCollisionList* Test(CCollisionStack* i_stack, CStageEntity* i_queryEntity,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test an entire cast against the collision system protagonists,
	// the static entity and the terrain entity using a specified stack. Get
	// back a collision list.
	CCollisionList* Test(CCollisionStack* i_stack, CCast* i_queryCast,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test a single entity against another entity, the static
	// entity and the terrain entity using a specified stack. Get back a
	// collision list.
	CCollisionList* Test(CCollisionStack* i_stack, CStageEntity* i_queryEntity,
			CStageEntity* i_targetEntity,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test a single entity against a cast, the static entity and the
	// terrain entity using a specified stack. Get back a collision list.
	CCollisionList* Test(CCollisionStack* i_stack, CStageEntity* i_queryEntity,
			CCast* i_targetCast,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test each member of a cast against the other members of the cast,
	// the static entity and the terrain entity using a specified stack. Get
	// back a collision list.
	CCollisionList* TestCast(CCollisionStack* i_stack, CCast* i_cast,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test the member of one cast against the members of another cast,
	// the static entity and the terrain entity using a specified stack. Get
	// back a collision list.
	CCollisionList* Test(CCollisionStack* i_stack, CCast* i_queryCast,
			CCast* i_targetCast,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test a single entity against the collision system protagonists,
	// the static entity and the terrain entity using the collision system
	// stack. Get back a collision list.
	CCollisionList* Test(CStageEntity* i_queryEntity,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test an entire cast against the collision system protagonists,
	// the static entity and the terrain entity using the collision system
	// stack. Get back a collision list.
	CCollisionList* Test(CCast* i_queryCast,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test a single entity against another entity, the static
	// entity and the terrain entity using the collision system stack. Get
	// back a collision list.
	CCollisionList* Test(CStageEntity* i_queryEntity,
			CStageEntity* i_targetEntity,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test a single entity against a cast, the static entity and the
	// terrain entity using the collision system stack. Get back a collision
	// list.
	CCollisionList* Test(CStageEntity* i_queryEntity, CCast* i_targetCast,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test each member of a cast against the other members of the cast,
	// the static entity and the terrain entity using the collision system
	// stack. Get back a collision list.
	CCollisionList* TestCast(CCast* i_cast,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Possibly test the member of one cast against the members of another cast,
	// the static entity and the terrain entity using the collision system
	// stack. Get back a collision list.
	CCollisionList* Test(CCast* i_queryCast, CCast* i_targetCast,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Drawing
	void DrawTarget(CCollisionList* i_list);
	void Draw(CStageEntity* i_queryEntity,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);
	void Draw(CStageEntity* i_queryEntity, CCast* i_targetCast,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// profiling
	#ifdef COLLISION_SYSTEM_STATS
	int GetTotalTestCount(void);
	#endif

  private:
	// The paritition system
	CollidePartitionSystem* d_partitionSystem;

	// The collision stack
	CCollisionStack d_stack;

	// Managed entites
	CTerrainEntity d_terrainEntity;
	CStaticEntity d_staticEntity;
	CDynamicEntities d_dynamicEntities;

	// Just for tracking
	struct Database
	{
		int d_partition;
		t_Handle d_handle;
		TerrainCollideLayerList* d_terrain;
		ObjectLayerList* d_object;
		Database *prev, *next;
	};
	DoublyLinkedList<Database> d_dbList;

	// count of tests made.
	#ifdef COLLISION_SYSTEM_STATS
	int TotalTestCount;
	#endif

	// The all important layer mask (bit mask for active layers)
	unsigned int d_layerMask;
	LayerChangeCallback d_layerChangeCB;
	void* d_layerChangeContext;
	void ClearLayerChangeCallback(void);
};

// A global for accessing this bad-boy
DefineSingleton(CollisionSystem)
#define g_collisionSystem GetSingleton(CollisionSystem)

// Include the inline implementations
#include "collide/collsys.hpp"

#endif
