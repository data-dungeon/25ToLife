///////////////////////////////////////////////////////////////////////////+//
// Simple (but true 3d) leaf effect
/////////////////////////////////////////////////////////////////////////////
#ifndef LEAFBLOW_H
#define LEAFBLOW_H

#include <stdlib.h>	//to def "rand()" on PS2

#include "platform/BaseType.h"
#include "Math/Matrix.h"
#include "GameHelper/loadsom.h"
#include "container/dllist.h"
#include "Effects/leaf.h"

// the maxium number of affectors we can have in the world
#define MAX_LEAFAFFECTORS 	8

// Forwards
class SplinePath;

class LeafBlower
{
public:
	LeafBlower(void);
	~LeafBlower(void);

	// Initialize the system
	bool Initialize(void);

	// Shut down everything
	void Terminate(void);

	// Call this every frame for updates
	void Update(const Graphics4x4 &camera, const Graphics4x4 *camera2 = NULL);

	// Add a leaf model into the system.
	// Leaf models should be in the XZ plane (y = 0)
	// Failure is inidcated by a false return
	bool LoadModel(LoadSOM &loadSOM, const char *modelName,
						float heightTweak = 0.05f, float density = 1.0f, float spinImpulse = 20.0f);

	// Add a list of things that can affect the leaf piles
	bool AddAffector(CActor *actor,
							float maxSpeed,
							float forceBubbleRadius = 3.0f,
							float force = 50.0f,
							float wakeD = 1.0f);

	// A follow effect returns a handle
	typedef void *FollowHandle;

	// Add a leaf effect that follows an entity, leaves are reciruclated
	// The viewport specifies what camera to use for culling, if you pass in
	// -1 the "best" camera will be used
	// If pathToFollow is specified, leaves are scattered along the path
	FollowHandle AddFollowEffect(CActor *actor,
											int viewPort,
											float maxSpeed,
					 						float distributeWidth,
											float distributeLength,
											float cullDistance,
											bool active = true,
											int leaves = 50,
											const char *pathToFollow = NULL,
											float forceBubbleRadius = 3.0f,
											float force = 50.0f,
											float wakeD = 1.0f);

	// Hide/show a follow effect
	void HideShow(FollowHandle handle, bool show);

	// Reset a follow effect
	void Reset(FollowHandle handle);

	// Remove a follow effect
	void RemoveFollowEffect(FollowHandle handle);

	// Setup the alpha fade for follow effects
	void SetupFollowAlphaFade(float minAlphaDistance, float maxAlphaDistance, float fadeRate = 2.0f);

	// Add a leaf pile to the world
	bool AddLeafPile(Vector3CRef worldPos,
							float radiusActivate,
							float pileRadius,
							int leaves = 20,
							bool oneShot = true);

private:
	// Our basic info per model
	DoublyLinkedList<LeafModel>	modelList;
	int									models;

	// Affectors are objects that can disturb leaves
	struct Affector
	{
		CActor		*actor;
		int			viewPort;
		float			forceD;
		float			force;
		float			wakeD;
		float			dSquared;
		float			speedSquared;
		float			invMaxSpeedSquared;
		Vector3		dir;
		Vector3		right;
		float			speedRatio;

		// If the affector is associated with a path, keep
		// track of it here
		SplinePath	*path;
		float			pathD;
	};
	Affector								affector[MAX_LEAFAFFECTORS];
	int									affectors;
	void UpdateAffectors(void);
	inline void UpdateAffector(Affector &a);

	// A structure to merge all the leaf dataparts
	struct LeafData
	{
		CInstance			instance;
		ts_bRGBA				vertexColor[4];
		Leaf					leaf;
		char					pad[16 - ((sizeof(CInstance) + sizeof(ts_bRGBA) * 4 + sizeof(Leaf)) & 0xf)];
	};

	// leaf array handing
	LeafData *AllocateLeafArray(int leaves);
	void FreeLeafArray(LeafData *leafArray, int leaves);

	// leaf utilities
	inline void ShowLeaf(LeafData &leaf, bool show);
	inline bool LeafHidden(LeafData &leaf);
	void ShowLeaves(LeafData *leafArray, int leaves, bool show);
	inline void AddToScene(LeafData &leaf);
	inline void RemoveFromScene(LeafData &leaf);
	void AddToScene(LeafData *leafArray, int leaves);
	void RemoveFromScene(LeafData *leafArray, int leaves);
	void SetColor(LeafData &leaf, u16 color);

	// A leaf follow effect
	struct LeafFollow
	{
		// our affector
		Affector					affector;
		float		  				distributeHalfWidth;
		float						distributeLength;
		float						cullSquared;
		float						leafCullSquared;

		// Do we have a path?
		SplinePath				*path;

		// Our array of leaves
		LeafData					*leaf;
		int 						leaves;

		// Are we active?
		bool 						active;

		// Are we on camera?
		bool						visible;

		// We appear in a list
		LeafFollow				*prev, *next;
	};
	DoublyLinkedList<LeafFollow> followList;

	// Setup the alpha fade for follow effects
	float		invAlphaRange;
	float		maxAlphaD;
	float		alphaFadeRate;

	void DeleteFollowEffect(LeafFollow *follow);
	int BestCamera(Affector &affector, const Vector3 *cameraPos, const Vector3 *cameraDir, int cameras);
	bool FollowVisible(LeafFollow &follow, Vector3CRef cameraPos, Vector3CRef cameraDir);
	void UpdateFollow(LeafFollow &follow, const Vector3 *cameraPos, const Vector3 *cameraDir, int cameras);
	void PlaceLeaves(LeafFollow &follow);
	void PlaceLeaf(LeafFollow &follow, LeafData &leaf);
	void PlaceLeafInitial(LeafFollow &follow, LeafData &leaf);
	void PlaceLeafInitialPath(LeafFollow &follow, LeafData &leaf);
	inline bool LeafVisible(LeafData &leaf, float cullDistanceSquared, Vector3CRef cameraPos, Vector3CRef cameraDir, Vector3 &cameraToLeaf);
	inline bool LeafVisiblePath(LeafData &leaf, LeafFollow &follow, Vector3CRef cameraPos, Vector3 &cameraToLeaf);
	inline void AdjustLeafAlpha(LeafData &leaf, Vector3CRef cameraToLeaf);

	// A leaf pile
	struct LeafPile
	{
		// Initial parameters form caller
		Vector3					pos;
		float 					radiusActivateSquared;
		float 					pileRadius;
		bool 						oneShot;

		// Have we encountered a force yet?
		bool						active;
		bool						visible;

		// Our array of leaves
		LeafData					*leaf;
		int 						leaves;

		LeafPile					*prev, *next;
	};
	DoublyLinkedList<LeafPile>		pileList;

	// the calls for maintaining piles (activates a pile if necessray)
	bool CheckActivate(LeafPile &pile);
	void CheckDeactivate(LeafPile &pile);

	// Activate a pile!
	void ActivatePile(LeafPile &pile);

	// Run the physics for a pile!
	void RunPhysics(LeafPile &pile);

	// Kill a pile
	void DeletePile(LeafPile *pile);

	// Show/hide a pile
	void ShowPile(LeafPile &pile);
	void HidePile(LeafPile &pile);

	// Distribute a pile
	void PlacePile(LeafPile &pile);

	// Place a single leaf on the ground
	static void PlaceLeaf(Vector3CRef origin, float radius, float heightTweak, Graphics4x4 &m, u16 &color);

	// Run the physics on a single leaf!
	void RunPhysics(Affector *affector, LeafData &leafData, float t);

	// Align a matrix around the passed up, using dir as a reference.  Assumes both are unit
	static void AlignToNormal(Vector3CRef up, Vector3CRef dir, Graphics4x4 &m);

	// Collide a point with the ground, returns height and normal
	static float CollideWithGround(float x, float z, float startY, Vector3 &normal, u16 &color, float yScanOffset = 0.5f);

	// return a random number between -1 and 1 (hopefully quickly)
	inline static float RandSign(void);
	inline static Vector3 RandUnitVector(void);
	inline static Vector3 RandUnitVectorXZ(void);
	inline static float Square(float f);
	inline static float Cube(float f);
};

// Include the inlines
#include "Effects/leafblow.hpp"

// There can be only one!
extern LeafBlower *g_leafBlowerPtr;
inline LeafBlower &SafePtr(LeafBlower *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_leafBlower SafePtr(g_leafBlowerPtr)

#endif
