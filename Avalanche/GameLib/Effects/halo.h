/////////////////////////////////////////////////////////////////////////////
// Here is the class the manages halo effects
/////////////////////////////////////////////////////////////////////////////
#ifndef HALO_H
#define HALO_H

#include "Effects/sseffect.h"
#include "Game/Managers/SpriteMgr.h"
#include "Effects/haloele.h"
#include "container/dllist.h"
#include "collide/collsys.h"
#include "camera/director.h"

// We have an automaton controller
class HaloAutomaton;

class Halo : public ScreenSpaceEffect
{
public:
	Halo();
	~Halo();

	// Initialize/terminate the system
	static void Initialize();
	static void Terminate();

	// Set the cull distance (in factor of far clip
	void SetCullDistance(float cullDistanceFactor)
		{ cullDFactor = cullDistanceFactor; }

	// Load the sprite halo sprites
	bool LoadSprites(char *spriteDBL);

	// load halos from the world DBL
	bool LoadHalosFromDBL(t_Handle worldHandle);

	// Disable all halos
	void DisableAll(bool disable = true)
		{ disableAll = disable; }

	// We talk about halo's using a handle
	typedef void *Handle;

	// If a halo is directional, what axis does it follow
	typedef enum
	{
		NONE,
		XAXIS,
		YAXIS,
		ZAXIS,
		NEGXAXIS,
		NEGYAXIS,
		NEGZAXIS
	} DIRTYPE;

	// Add a dynamic halo, returns null on failure
	// position and orienation are saved as POINTERS so make sure you remove
	// the halo before deleting your object.
	// if trueSizing is true, halo's are sized as if they are light sources,
	//	if trueSizing is false, halo's are sized like sprites
	// If modelOFfset is non-null, it provides an offset in model coordinates of
	// the halo.
	// Dirtype states with direction the light is facing...if it is NONE then
	// the light is omni-directional
	Handle Add(Vector3 *position, DirCos3x3 *orienation,
				 	int spriteIndex, const ts_bRGBA &color,
					float size, bool rotate = true, bool trueSizing = true,
					const Vector3 *modelOffset = NULL, DIRTYPE dirType = NONE,
					bool dynamicBlock = false, CCollisionStackArray<CStageEntity*> *ignoreList = NULL);

	// Add a haloAutomaton type halo
	Handle Add(HaloAutomaton &automaton,
				 	int spriteIndex, const ts_bRGBA &color,
					float size, bool rotate = true, bool trueSizing = true,
					const Vector3 *modelOffset = NULL, DIRTYPE dirType = NONE,
					bool dynamicBlock = false, CCollisionStackArray<CStageEntity*> *ignoreList = NULL);

	// Modify some of the static parts of a halo
	void ChangeSize(Handle haloHandle, float newSize);
	void ChangeColor(Handle haloHandle, const ts_bRGBA &newColor);
	void ChangeRotate(Handle haloHandle, bool newRotate);
	void ChangeModelOffset(Handle haloHandle, Vector3 *newModelOffset);
	void ChangeDirType(Handle haloHandle, DIRTYPE newDirType);
	void ChangeCollision(Handle haloHandle, bool dynamicBlock, CCollisionStackArray<CStageEntity*>* ignoreList);

	// Hide the halo
	void Hide(Handle haloHandle, bool hide);
	bool Hidden(Handle haloHandle);

	// Remove the halo once it is done
	bool Remove(Handle haloHandle);

	// Find and disable a static halo
	bool FindAndEnable(const AABB &insideOf, bool enable);

	// Set an ignore entity for collisions
	void SetIgnoreEntity(CStageEntity *entity)
		{ ignoreEntity = entity; }

	// Draw
	void Update(CameraScene *scene = NULL);

private:
	// Our texture
	t_Handle					hTexture;

	// Some stuff to make it faster
	float						cullDFactor;

	// Disable all
	bool						disableAll;

	// The ignore entity
	CStageEntity			*ignoreEntity;

	// Stuff for the static halos in the world
	struct WorldHalo
	{
		// Our static info
		Vector3				pos;
		Vector3				dir;
		ts_bRGBA				color;
		float					size;
		bool					inWorld;
		bool					rotate;
		bool					hasDir;
		bool					disabled;
		int					textureIndex;

		// The actual element
		HaloElement			sprite;
	};
	WorldHalo				*worldHalo;
	int						worldHalos;

	// Update a static halo
	void Update(Camera &camera, float cullDSquared, WorldHalo &halo);

	// The dynamic halo structure
	struct DynamicHalo
	{
		// Do we have an automaton controlling us?
		HaloAutomaton	*automaton;

		// Our position and orientation in the world
		Vector3			*pos;
		DirCos3x3	 	*o;

		// Do we have a model offset
		bool				hasModelOffset;
		Vector3			modelOffset;

		// Are we visible?
		bool				hidden;

		// Our color
		ts_bRGBA			color;

		// Our size
		float				size;
		bool				trueSizing;

		// Do we rotate?
		bool				rotate;

		// Our DIRTYPE
		DIRTYPE			dirType;

		// Collision crap
		bool											dynamicBlock;
		CCollisionStackArray<CStageEntity*>	*ignoreList;

		// Our actual renderable
		HaloElement		element;

		// We exist in a list
		DynamicHalo *prev, *next;
	};

	// Our list of dynamic halo's
	DoublyLinkedList<DynamicHalo>		dynamicList;

	// Allocate/free a dynmaic halo (puts it at tail of list)
	DynamicHalo *Allocate();
	void Free(DynamicHalo *halo);
	void FreeAllDynamic();

	// Update a dynamic (returns false if not drawn)
	void Update(Camera &camera, float cullDSquared, DynamicHalo &halo);
	inline Vector3 WorldPosition(DynamicHalo &halo);
	inline Vector3 WorldDir(DynamicHalo &halo);
};

// There can be only one!
extern Halo *g_haloPtr;
inline Halo &SafePtr(Halo *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_halo SafePtr(g_haloPtr)

#endif
