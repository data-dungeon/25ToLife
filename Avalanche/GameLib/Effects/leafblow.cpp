/////////////////////////////////////////////////////////////////////////////
// Simple (but true 3d) leaf effect
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"



#include "EngineHelper/timer.h"



// We are a singleton
LeafBlower *g_leafBlowerPtr = NULL;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LeafBlower::LeafBlower(void) :
modelList(false),
pileList(false),
followList(false)
{
	models = 0;
	affectors = 0;

	// Setup the alpha fade for follow effects
	SetupFollowAlphaFade(15.0f, 10.0f);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LeafBlower::~LeafBlower(void)
{
	Terminate();
}

/////////////////////////////////////////////////////////////////////////////
// Initialize the system
/////////////////////////////////////////////////////////////////////////////
bool LeafBlower::Initialize(void)
{
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Shut down everything
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::Terminate(void)
{
	// Delete all the piles
	LeafPile *pile = pileList.Head();
	while (pile)
	{
		DeletePile(pile);
		pile = pileList.Head();
	}

	// delete all the follows
	LeafFollow *follow = followList.Head();
	while (follow)
	{
		DeleteFollowEffect(follow);
		follow = followList.Head();
	}

	// no more affectors
	affectors = 0;

	// Delete all the models
	LeafModel *model = modelList.Head();
	while (model)
	{
		modelList.Unlink(model);
		delete model;
		model = modelList.Head();
	}
	models = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Call this every frame for updates
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::Update(
const Graphics4x4 &camera,
const Graphics4x4 *camera2)
{
	// Update the affectors
	UpdateAffectors();

	// We have two potential cameras
	Vector3 cameraPos[2];
	Vector3 cameraDir[2];
	int cameras = 1;

	// What is the camera position in world coordinates?
	Vector3 temp;
	temp = camera.GetTranslation();
	cameraPos[0] = temp.InvBasisTransform(camera);
	cameraPos[0].Negate();

	//	What is the camera dir?
	cameraDir[0] = camera.GetColumn2();
	cameraDir[0].Negate();

	// Did they specify another viewport?
	if (camera2)
	{
		// What is the camera position in world coordinates?
		temp = camera2->GetTranslation();
		cameraPos[1] = temp.InvBasisTransform(*camera2);
		cameraPos[1].Negate();

		//	What is the camera dir?
		cameraDir[1] = camera2->GetColumn2();
		cameraDir[1].Negate();
		++cameras;
	}

	// Update all the follow effects
	LeafFollow *follow = followList.Head();
	while (follow)
	{
		// Run it
		UpdateFollow(*follow, cameraPos, cameraDir, cameras);

		// next!
		follow = follow->next;
	}

	// Update all the piles
	LeafPile *pile = pileList.Head();
	while (pile)
	{
		// What is the next pile
		LeafPile *nextPile = pile->next;

		// Is the pile  or going active?
		if (pile->active || CheckActivate(*pile))
		{
			// run the physics
			RunPhysics(*pile);

			// See if this pile is going out of scope
			CheckDeactivate(*pile);
		}

		// next one!
		pile = nextPile;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Add a leaf model into the system.
// Leaf models should be in the XZ plane (y = 0)
// Failure is inidcated by a NULL return
/////////////////////////////////////////////////////////////////////////////
bool LeafBlower::LoadModel(
LoadSOM &loadSOM,
const char *modelName,
float heightTweak,
float density,
float spinImpulse)
{
	// Are they dumb?
	ASSERT(modelName);

	// First make sure we can even load this model
	ts_Geometry *geometry = loadSOM.Geometry(modelName);
	if (!geometry)
		return(false);

	// Create a structure for this model's data
	LeafModel *model = new LeafModel;
	if (!model)
		return(false);

	// Fill out the structure
	model->geometry = *geometry;
	model->heightTweak = heightTweak;
	model->spinImpulse = spinImpulse;

	// must have som!
	ASSERT(model->geometry.pSOM != NULL);

	// Setup the props
	model->props.Defaults();
	Vector3 size;
	size = model->geometry.pSOM->BoundingBox.Max - model->geometry.pSOM->BoundingBox.Min;
	size.y(0.0f);
	model->props.SetBody(size, density);

#ifdef _XBOX
	// Fixup for xbox
	ts_PatchSOM *som = model->geometry.pSOM;
	ts_TextureLayer &texlayer = som->pTriStripBuffer->pTextureContext->TextureLayer[0];
	texlayer.u8Method = TS_METHOD_CONSTANTALPHA;
	som->dh.u16Flags |= SOMM_DYNAMIC_GEOMETRY;
	ts_TriStripHeader *hdr = model->geometry.pSOM->pTriStripBuffer;
	hdr->u8Opacity = 100;
#endif

#if 0
	// Twiddle with the innards of this model
	ts_TriStripHeader *hdr = model->geometry.pSOM->pTriStripBuffer;
	while (hdr)
	{
		// Enlarge for visual pleasure
		Vector4 *vertex = (Vector4 *)(hdr + 1);
		for (uint v = 0; v < hdr->u16PointCount; v++)
		{
			vertex[v].x *= 2.0f;
			vertex[v].z *= 2.0f;
		}

		// Twiddle the flags
		hdr->u8Flags |= TRI_STRIP_TWO_SIDED;
		hdr = hdr->pNext;
	};
#endif

	// Add it to the list
	modelList.AddTail(model);

	// success
	++models;
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Add a list of things that can affect the leaf piles
/////////////////////////////////////////////////////////////////////////////
bool LeafBlower::AddAffector(
CActor *actor,
float maxSpeed,
float forceBubbleRadius,
float force,
float wakeD)
{
	// Adding null is A-Okay
	if (!actor)
		return(true);

	// Our of space?
	if (affectors >= MAX_LEAFAFFECTORS)
		return(false);

	// Stick it in
	affector[affectors].actor = actor;
	affector[affectors].viewPort = -1;
	affector[affectors].forceD = forceBubbleRadius;
	affector[affectors].force = force;
	affector[affectors].wakeD = wakeD;
	affector[affectors].speedSquared = 0.0f;
	affector[affectors].invMaxSpeedSquared = 1.0f / (maxSpeed * maxSpeed);
	affector[affectors].path = NULL;
	affector[affectors].pathD = 0.0f;

	++affectors;

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Add a leaf effect that follows an entity, leavesare reciruclated
/////////////////////////////////////////////////////////////////////////////
LeafBlower::FollowHandle LeafBlower::AddFollowEffect(
CActor *actor,
int viewPort,
float maxSpeed,
float distributeWidth,
float distributeLength,
float cullDistance,
bool active,
int leaves,
const char *pathToFollow,
float forceBubbleRadius,
float force,
float wakeD)
{
	// Create a new effect
	LeafFollow *follow = new LeafFollow;
	ASSERT(follow);
	if (!follow)
		return(false);

	// setup the affector
	follow->affector.actor = actor;
	follow->affector.viewPort = viewPort;
	follow->affector.forceD = forceBubbleRadius;
	follow->affector.force = force;
	follow->affector.wakeD = wakeD;
	follow->affector.speedSquared = 0.0f;
	follow->affector.invMaxSpeedSquared = 1.0f / (maxSpeed * maxSpeed);

	// Did they want a path?
	follow->path = NULL;
	if (pathToFollow)
	{
		// make sure there is a path collection
		PathCollection *pathCollection = PathCollection::Collection();

		// Try to setup our path
		if (pathCollection)
			follow->path = pathCollection->Named((char *)pathToFollow);

		// The affector needs to know this too
		follow->affector.path = follow->path;
	}

	// Copy in the caller params
	follow->distributeHalfWidth = distributeWidth * 0.5f;
	follow->distributeLength = distributeLength;
	follow->cullSquared = cullDistance * cullDistance;
	follow->leafCullSquared = Math::Square(distributeLength * 0.3f);

	// Setup our state
	follow->active = active;
	follow->visible = active;

	// Create the array of leaves
	follow->leaf = AllocateLeafArray(leaves);
	if (!follow->leaf)
	{
		// This is bad
		delete follow;
		return(NULL);
	}
	follow->leaves = leaves;

	// Add all of us to the scene
	AddToScene(follow->leaf, follow->leaves);

	// Hide all of us
	if (!active)
		ShowLeaves(follow->leaf, follow->leaves, false);
	else
	{
		UpdateAffector(follow->affector);
		for (int i = 0; i < follow->leaves; i++)
			PlaceLeafInitial(*follow, follow->leaf[i]);
	}

	// Stick it in the list
	followList.AddTail(follow);

	// Give them back the thingie
	return((FollowHandle)follow);
}

/////////////////////////////////////////////////////////////////////////////
// Hide/show a follow effect
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::HideShow(
FollowHandle handle,
bool show)
{
	// Did they give us something to work with?
	LeafFollow *follow = (LeafFollow *)handle;
	if (!follow)
		return;

	// Save this
	follow->active = show;
}

/////////////////////////////////////////////////////////////////////////////
// Reset a follow effect
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::Reset(
FollowHandle handle)
{
	// Did they give us something to work with?
	LeafFollow *follow = (LeafFollow *)handle;
	if (!follow)
		return;

	// Update our info
	UpdateAffector(follow->affector);

	// Place the leaves
	for (int i = 0; i < follow->leaves; i++)
	{
		PlaceLeafInitial(*follow, follow->leaf[i]);
	 	ShowLeaf(follow->leaf[i], true);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Remove a follow effect
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::RemoveFollowEffect(
FollowHandle handle)
{
	// Did they give us something to work with?
	LeafFollow *follow = (LeafFollow *)handle;
	if (follow)
		DeleteFollowEffect(follow);
}

/////////////////////////////////////////////////////////////////////////////
// Setup the alpha fade for follow effects
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::SetupFollowAlphaFade(
float minAlphaDistance,
float maxAlphaDistance,
float fadeRate)
{
	ASSERT(minAlphaDistance > maxAlphaDistance);

	invAlphaRange = 1.0f / (minAlphaDistance - maxAlphaDistance);
	invAlphaRange *= invAlphaRange;
	maxAlphaD = maxAlphaDistance;
	maxAlphaD *= maxAlphaD;
	alphaFadeRate = fadeRate;
}

/////////////////////////////////////////////////////////////////////////////
// Add a leaf pile to the world
/////////////////////////////////////////////////////////////////////////////
bool LeafBlower::AddLeafPile(
Vector3CRef worldPos,
float radiusActivate,
float pileRadius,
int leaves,
bool oneShot)
{
	// Create a new leaf pile
	LeafPile *pile = new LeafPile;
	ASSERT(pile);
	if (!pile)
		return(false);

	// Copy in the caller params
	pile->pos = worldPos;
	pile->radiusActivateSquared = radiusActivate * radiusActivate;
	pile->pileRadius = pileRadius;
	pile->leaves = leaves;
	pile->oneShot = oneShot;

	// Setup our state
	pile->active = false;
	pile->visible = false;

	// Create the array of leaves
	pile->leaf = AllocateLeafArray(leaves);
	if (!pile->leaf)
	{
		// This is bad
		delete pile;
		return(false);
	}
	pile->leaves = leaves;

	// Stick it in the list
	pileList.AddTail(pile);

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Update the affectors
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::UpdateAffectors(void)
{
	for (int a = 0; a < affectors; a++)
		UpdateAffector(affector[a]);
}

/////////////////////////////////////////////////////////////////////////////
// Allocate an array of leaves in a very memory friendly way
/////////////////////////////////////////////////////////////////////////////
LeafBlower::LeafData *LeafBlower::AllocateLeafArray(
int leaves)
{
	ASSERT(leaves > 0);

	// make the array
	LeafData *array = (LeafData *)memAlloc(sizeof(LeafData) * leaves);

	// Randomize the leaf model
	LeafModel *leafModel = modelList.Head();
	int randMod = ((leaves / models) + 1) / 2;
	int nextModel = (leafModel->next && randMod) ? (randMod + (rand() % randMod)) : leaves;

	// now initialize each one
	for (int i = 0; i < leaves; i++)
	{
		// Advance the model?
		if (i >= nextModel)
		{
			leafModel = leafModel->next;
			ASSERT(leafModel);
			nextModel += (leafModel->next) ? (randMod + (rand() % randMod)) : leaves;
		}

		// Build up the instance stuff
		array[i].instance.InitFromGeometry( leafModel->geometry );
		array[i].instance.Matrix().Identity();

		// force off lighting
		array[i].instance.SetDynamicallyLit( false );
		array[i].instance.SetAlphaSort( true );
		array[i].instance.SetName( "jeff-leaf" );

		// Setup the vertex coloring
		array[i].instance.SetVertexColor32Bit( true );
		array[i].instance.SetVertexColorPtr( array[i].vertexColor );
		SetColor(array[i], 0x001f);

		// Make sure we call the constructor on the leaf
		new ((void *)&array[i].leaf) Leaf;

		// Now setup the leaf
		array[i].leaf.Setup(leafModel, &array[i].instance.Matrix());
	}

	// Give them back this glorious pointer
	return(array);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::FreeLeafArray(
LeafData *leafArray,
int leaves)
{
	if (leafArray)
	{
#if 0
		// If the leaf needs a destructor, excute this loop
		for (int i = 0; i < leaves; i++)
		{
			// Call the destructor
			leafArray[i].leaf.~Leaf();
		}
#endif
		memFree(leafArray);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::ShowLeaves(
LeafData *leafArray,
int leaves,
bool show)
{
	for (int i = 0; i < leaves; i++)
		ShowLeaf(leafArray[i], show);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::AddToScene(
LeafData *leafArray,
int leaves)
{
	for (int i = 0; i < leaves; i++)
		AddToScene(leafArray[i]);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::RemoveFromScene(
LeafData *leafArray,
int leaves)
{
	for (int i = 0; i < leaves; i++)
		RemoveFromScene(leafArray[i]);
}

/////////////////////////////////////////////////////////////////////////////
// Set the color for this leaf
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::SetColor(
LeafData &leaf,
u16 color)
{
	// Simple ambient
	const ts_bRGB ambientBright = { 160, 160, 160 };
	const ts_bRGB ambientDark = { 60, 60, 60 };
	const int shadowThreshold = 128;

	// Compute the colors
	int r = (color & 0x1f) << 3;
	int g = ((color >> 5) & 0x3f) << 2;
	int b = ((color >> 11) & 0x1f) << 3;

	// Convert into a brbga
	ts_bRGBA c;
	if (g >= shadowThreshold)
	{
		c.R = Math::Clamp(ambientBright.R + r);
		c.G = Math::Clamp(ambientBright.G + g);
		c.B = Math::Clamp(ambientBright.B + b);
	}
	else
	{
		c.R = Math::Clamp(ambientDark.R + r);
		c.G = Math::Clamp(ambientDark.G + g);
		c.B = Math::Clamp(ambientDark.B + b);
	}
	c.A = 255;

	// Blast it in!
	leaf.vertexColor[0] = c;
	leaf.vertexColor[1] = c;
	leaf.vertexColor[2] = c;
	leaf.vertexColor[3] = c;
}

/////////////////////////////////////////////////////////////////////////////
// Delete a follow effect
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::DeleteFollowEffect(
LeafFollow *follow)
{
	// Did they give us something to work with?
	ASSERT(follow);

	// Remove them from the scene
	RemoveFromScene(follow->leaf, follow->leaves);

	// Clear the leaf array
	FreeLeafArray(follow->leaf, follow->leaves);

	// Clear us out now
	followList.Unlink(follow);

	// Wee
	delete follow;
}

/////////////////////////////////////////////////////////////////////////////
// What is the "best" camera for the point in question
/////////////////////////////////////////////////////////////////////////////
int LeafBlower::BestCamera(
Affector &affector,
const Vector3 *cameraPos,
const Vector3 *cameraDir,
int cameras)
{
	// If there is only one camera, use that one
	if (cameras == 1)
		return(0);

	// If they have specified a viewport for the
	// affector, just use that
	if (affector.viewPort >= 0)
		return(affector.viewPort);

	// Find the closest camera
	Vector3 pos = affector.actor->GetBodyInWorld();
	float d0 = (cameraPos[0] - pos).LengthSquared();
	float d1 = (cameraPos[1] - pos).LengthSquared();
	if (d0 < d1)
		return(0);
	else
		return(1);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool LeafBlower::FollowVisible(
LeafFollow &follow,
Vector3CRef cameraPos,
Vector3CRef cameraDir)
{
	return((follow.affector.actor->GetBodyInWorld() - cameraPos).LengthSquared() < follow.cullSquared);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::UpdateFollow(
LeafFollow &follow,
const Vector3 *cameraPos,
const Vector3 *cameraDir,
int cameras)
{
	// Update the affector data
	UpdateAffector(follow.affector);

	// What is the best camera for this follow effect
	int bestCamera = BestCamera(follow.affector, cameraPos, cameraDir, cameras);

	// Is this follow visible?
	bool visible = FollowVisible(follow, cameraPos[bestCamera], cameraDir[bestCamera]);

	// Is our overall state changing?
	if (visible != follow.visible)
	{
		// Hide all the leaves
		if (!visible)
			ShowLeaves(follow.leaf, follow.leaves, false);
		else
		{
			// Place all the leaves if we are active (otherwise leave them hidden)
			if (follow.active)
				PlaceLeaves(follow);
		}

		// our state changed
		follow.visible = visible;
	}

	// Update the physics for each leaf
	if (visible)
	{
		float t = g_timer.GetFrameSec();
		for (int i = 0; i < follow.leaves; i++)
		{
			// Update the physics
			RunPhysics(&follow.affector, follow.leaf[i], t);

			// What leaves need to be repositioned?
			Vector3 cameraToLeaf;
			bool visible;
			if (follow.path)
				visible = LeafVisiblePath(follow.leaf[i], follow, cameraPos[bestCamera], cameraToLeaf);
			else
				visible = LeafVisible(follow.leaf[i], follow.leafCullSquared, cameraPos[bestCamera], cameraDir[bestCamera], cameraToLeaf);

			// If we are visible, adjust our alpha
			if (visible)
			{
				AdjustLeafAlpha(follow.leaf[i], cameraToLeaf);
			}
			else
			{
				// Fade the alpha
				float alpha = follow.leaf[i].instance.Alpha();
				if ( alpha > 0.1f )
				{
					alpha = Math::Clamp( alpha - t * alphaFadeRate );
					follow.leaf[i].instance.SetAlpha( alpha );
				}
				else
				{
					// Recycle the leaf
					if (follow.active)
						PlaceLeaf(follow, follow.leaf[i]);
					else
						ShowLeaf(follow.leaf[i], false);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::PlaceLeaves(
LeafFollow &follow)
{
	for (int i = 0; i < follow.leaves; i++)
		PlaceLeaf(follow, follow.leaf[i]);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::PlaceLeaf(
LeafFollow &follow,
LeafData &leafData)
{
	// Ensure we are visible
	ShowLeaf(leafData, true);

	// Build the position
	Vector3 leafPos;
	if (follow.path)
	{
		// Get the position off the path
		float d = 0.5f + (g_random.Unit() + follow.affector.speedRatio) * 0.5f;
		d *= follow.distributeLength;
		d += follow.affector.pathD;
		leafData.leaf.SetD(d);
		leafPos = follow.path->PointFromDistance(d);
		float w = follow.distributeHalfWidth * g_random.NegPos();
		Vector3 binormal = Vector3::Cross(follow.path->TangentFromDistance(d), vUp);
		leafPos += binormal * w;
		leafData.instance.SetAlpha( 0.0f );
	}
	else
	{
		leafPos  = follow.affector.actor->GetBodyInWorld();
		float d = 0.5f + (g_random.Unit() + follow.affector.speedRatio) * 0.5f;
		leafPos += follow.affector.dir * (follow.distributeLength * d);
		float w = follow.distributeHalfWidth + (d * follow.distributeHalfWidth * 4.0f);
		leafPos += follow.affector.right * (g_random.NegPos() * w);
	}

	// Collide with the world
	Vector3 normal;
	u16 color;
	leafPos.y( CollideWithGround(leafPos.x(), leafPos.z(), 100000.0f, normal, color) +
				  leafData.leaf.Model()->heightTweak);

	// Set the color
	SetColor(leafData, color);

	// Build the matrix around the intersection
	AlignToNormal(normal, RandUnitVectorXZ(), leafData.instance.Matrix());

	// Finally, poke in the translation
	leafData.instance.Matrix().PokeTranslation(leafPos);

	// Reset the physics for this leaf
	leafData.leaf.Reset(true);

	leafData.instance.UpdateBoundingVolumes();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::PlaceLeafInitial(
LeafFollow &follow,
LeafData &leafData)
{
	Vector3 leafPos;
	if (follow.path)
	{
		// Get the position off the path
		float d = follow.affector.pathD + (follow.distributeLength * 1.5f * g_random.Unit());
		leafData.leaf.SetD(d);
		leafPos = follow.path->PointFromDistance(d);
		float w = follow.distributeHalfWidth * g_random.NegPos();
		Vector3 binormal = Vector3::Cross(follow.path->TangentFromDistance(d), vUp);
		leafPos += binormal * w;
		float l = binormal.Length();
	}
	else
	{
		// Build the position
		leafPos = follow.affector.actor->GetBodyInWorld();
		leafPos += follow.affector.dir * (follow.distributeLength * g_random.NegPos());
		leafPos += follow.affector.right * (follow.distributeLength * g_random.NegPos());
	}

	// Collide with the world
	Vector3 normal;
	u16 color;
	leafPos.y(
		CollideWithGround(leafPos.x(), leafPos.z(), 100000.0f, normal, color) +
		leafData.leaf.Model()->heightTweak);

	// Set the color
	SetColor(leafData, color);

	// Build the matrix around the intersection
	AlignToNormal(normal, RandUnitVectorXZ(), leafData.instance.Matrix());

	// Finally, poke in the translation
	leafData.instance.Matrix().PokeTranslation(leafPos);

	// Reset the physics for this leaf
	leafData.leaf.Reset(true);

	leafData.instance.UpdateBoundingVolumes();
}

/////////////////////////////////////////////////////////////////////////////
// the calls for maintaining piles (activates a pile if necessray)
/////////////////////////////////////////////////////////////////////////////
bool LeafBlower::CheckActivate(
LeafPile &pile)
{
	ASSERT(!pile.active);

	// Are any of the activators in range?
	for (int a = 0; a < affectors; a++)
	{
		// How far away are they?
		float dSquared = (affector[a].actor->GetBodyInWorld() - pile.pos).LengthSquared();

		// sweet!
		if (dSquared < pile.radiusActivateSquared)
		{
			ActivatePile(pile);
			return(true);
		}
	}

	return(false);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::CheckDeactivate(
LeafPile &pile)
{
	ASSERT(pile.active);

	// Are any of the activators in range?
	for (int a = 0; a < affectors; a++)
	{
		// How far away are they?
		float d = (affector[a].actor->GetBodyInWorld() - pile.pos).LengthSquared();

		// doh!
		if (d < pile.radiusActivateSquared)
			return;
	}

	// deactivate this pile
	if (pile.oneShot)
		DeletePile(&pile);
	else
	{
		// deactivate the pile
		pile.active = false;

		// hide it
		HidePile(pile);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Activate a pile!
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::ActivatePile(
LeafPile &pile)
{
	ASSERT(!pile.active);

	// turn it on!
	pile.active = true;

	// Place all the leaves
	PlacePile(pile);

	// Make sure theya are shown
	ShowPile(pile);
}

/////////////////////////////////////////////////////////////////////////////
// Kill a pile
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::DeletePile(
LeafPile *pile)
{
	if (pile)
	{
		pileList.Unlink(pile);
		if (pile->visible)	// if hidden, then already removed from scene
			RemoveFromScene(pile->leaf, pile->leaves);
		FreeLeafArray(pile->leaf, pile->leaves);
		delete pile;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Run the physics for a pile!
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::RunPhysics(
LeafPile &pile)
{
	// What is the best affector
	Affector *bestAffector = NULL;
	for (int a = 0; a < affectors; a++)
	{
		affector[a].dSquared = (pile.pos - affector[a].actor->GetBodyInWorld()).LengthSquared();

		float r = affector[a].forceD + pile.pileRadius;
		if (affector[a].dSquared < (r * r))
		{
			if (bestAffector == NULL || affector[a].dSquared < bestAffector->dSquared)
				bestAffector = &affector[a];
		}
	}

	// Update the physics for each leaf
	float t = g_timer.GetFrameSec();
	for (int i = 0; i < pile.leaves; i++)
		RunPhysics(bestAffector, pile.leaf[i], t);
}

/////////////////////////////////////////////////////////////////////////////
// Show/hide a pile
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::ShowPile(
LeafPile &pile)
{
	if (pile.visible)
		return;

	// Show them!
	AddToScene(pile.leaf, pile.leaves);

	pile.visible = true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::HidePile(
LeafPile &pile)
{
	// Can't hide if not shown
	if (!pile.visible)
		return;

	// hide it!
	RemoveFromScene(pile.leaf, pile.leaves);

	pile.visible = false;
}

/////////////////////////////////////////////////////////////////////////////
// Distribute a pile
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::PlacePile(
LeafPile &pile)
{
	for (int i = 0; i < pile.leaves; i++)
	{
		// Place it
		u16 color;
		PlaceLeaf(pile.pos,
						pile.pileRadius,
						pile.leaf[i].leaf.Model()->heightTweak,
						pile.leaf[i].instance.Matrix(),
						color);
		pile.leaf[i].leaf.Reset();
		SetColor(pile.leaf[i], color);
		pile.leaf[i].instance.UpdateBoundingVolumes();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Place a single leaf on the ground
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::PlaceLeaf(
Vector3CRef origin,
float radius,
float heightTweak,
Graphics4x4 &m,
u16 &color)
{
	// Figure out a random position
	Vector2 r;
	do
	{
		r.Set(g_random.NegPos(),
				g_random.NegPos());
	}
	while (r.LengthSquared() > 1.0f);

	// Build the position
	Vector3 leafPos;
	leafPos.x( origin.x() + r.x() * radius);
	leafPos.z( origin.z() + r.y() * radius);

	// Collide with the world
	Vector3 normal;
	leafPos.y( CollideWithGround(leafPos.x(), leafPos.z(), origin.y(), normal, color) + heightTweak +
				  // This is cheesy!
				  (1.0f - (r.x() * r.x() + r.y() * r.y())) * 0.1f * radius);

	// Build the matrix around the intersection
	AlignToNormal(normal, RandUnitVectorXZ(), m);

	// Finally, poke in the translation
	m.PokeTranslation(leafPos);
}

/////////////////////////////////////////////////////////////////////////////
// Run the physics on a single leaf!
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::RunPhysics(
Affector *affector,
LeafData &leafData,
float t)
{
	// Alias the leaf for chrissakes
	Leaf &leaf = leafData.leaf;
	leaf.ClearExternalForces();

	// apply forces
	if (affector)
	{
		// Setup some aliases, please make them go away Mr. Optimizer
		Vector3CRef leafPos = leaf.Position();
		Vector3CRef affectorPos = affector->actor->GetBodyInWorld();
		Vector3 d = leafPos - affectorPos;
		float dLengthSquared = d.LengthSquared();
		float dLength = d.Length();
		float dRatio = 1.0f - dLength / affector->forceD;

		// Figure out the force ratio
		float forceRatio = dRatio * affector->speedRatio;

		// only affect leaves in front of us
		Vector3 dNormalized = d / dLength;
		float dirDot = Vector3::Dot(affector->dir, dNormalized);
//		ASSERT(dirDot >= -1.0f && dirDot <= 1.0f);

		// Anything to do?
		if (dRatio > 0.0f && forceRatio > 0.1f)
		{
			// If the body was at rest, give a random angular impulse
			if (leaf.AtRest())
			{
				Vector3 w;
				w.Set(g_random.NegPos(), g_random.NegPos(), g_random.NegPos());
				w *= leaf.Model()->spinImpulse * ((affector->speedRatio * 0.4f) + 0.6f);
				leaf.ApplyAngularImpulse(w);
			}

			// Impart the translational force
			float force = forceRatio * affector->force;
			Vector3 dir;
			if (dirDot > 0.0f)
			{
				dNormalized.y(0.0f);
				dir = dNormalized * ((1.0f - dirDot) * 0.5f);
				dir.y(dir.y() + dirDot); //Math::Min(dirDot * 2.0f, 1.0f);
				leaf.SetExternalForce(dir * force);
			}
			else
			{
				// Wake
				if (dLength < affector->wakeD)
				{
					dir.Set(-dNormalized.x(),
							  -dirDot,
							  -dNormalized.z());
					leaf.SetExternalForce(dir * force);
				}
			}
		}
	}

	if (!leaf.AtRest())
	{
		// Do cheesy assed collision
		CLineEntity entity;
		entity.SetPositiveInclude(WORLD_CATEGORY);
		entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		entity.Line().Origin() = leaf.Position();

		// Compute the drag
		Vector3 leafUp;
		leafUp = leaf.Orientation().GetRow1();
#ifdef ROTDRAG
		// Rotational drag
		const float rotDrag = 0.001f;
		float vDot = Vector3::Dot(leaf.Velocity(), leafUp);
		if (vDot > 0.0f) vDot = -vDot;
		leaf.SetExternalTorque(leaf.AngularVelocity() * vDot * rotDrag);
#endif
#ifdef FALLDRAG
		// Drag against falling
		const Vector3 up = { 0.0f, 1.0f, 0.0f };
		const float drag = 1.0f;
		float nDot = Vector3::Dot(leafUp, up);
		if (nDot > 0.0f) nDot = -nDot;
		Vector3 dragV;
		dragV.Set(0.0f, leaf.Velocity().y * nDot * drag, 0.0f);
		leaf.AddExternalForce(dragV);
#else
		// Real drag
		const float drag = 0.33f;
		float vDot = Vector3::Dot(leaf.Velocity(), leafUp);
		if (vDot > 0.0f) vDot = -vDot;
		Vector3 dragV;
		dragV = leaf.Velocity() * vDot * drag;
		leaf.AddExternalForce(dragV);
#endif

		// Run a time step
		leaf.Run(t);

		// Collision?
		entity.Line().Basis() = leaf.Position() - entity.Line().Origin();
		g_collisionSystem.ResetStack();
		Vector3 intersection, normal;
		if ((g_collisionSystem.Test(&entity) != NULL) &&
				CLineQuery::Contact(entity.Line(), intersection, normal))
		{
			// Alias our matrix
			Graphics4x4 &m = leafData.instance.Matrix();

			// Build the matrix around the intersection
			Vector3 dir;
			dir = m.GetRow2();
			AlignToNormal(normal, dir, m);

			// Finally, poke in the translation
			intersection.y(intersection.y() + leaf.Model()->heightTweak);
			m.PokeTranslation(intersection);
			leaf.Reset(true);
		}
	}

	// Update the bounding volumes
	leafData.instance.UpdateBoundingVolumes();
}

/////////////////////////////////////////////////////////////////////////////
// Align a matrix around the passed up, using dir as a reference.
// Assumes both are unit
/////////////////////////////////////////////////////////////////////////////
void LeafBlower::AlignToNormal(
Vector3CRef up,
Vector3CRef dir,
Graphics4x4 &m)
{
	// compute the right
	Vector3 right = Vector3::Cross(up, dir);
	right.Normalize();

	// Re-fix the dir
	Vector3 fixDir = Vector3::Cross(right, up);

	// Set the matrix
	m.SetRow0(right);
	m.SetRow1(up);
	m.SetRow2(fixDir);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float LeafBlower::CollideWithGround(
float x,
float z,
float startY,
Vector3 &normal,
u16 &color,
float yScanOffset)
{
	g_collisionSystem.ResetStack();
	CXZEntity xzEntity;
	xzEntity.SetPositiveInclude(TERRAIN_CATEGORY);
	xzEntity.FindFloor(x, startY + yScanOffset, z);

	// Did we hit?
	CCollisionList* list = g_collisionSystem.Test(&xzEntity);
	if (list == NULL)
	{
		normal = vUp;
		color = 0xffff;
		return(startY);
	}

	// give them back the normal and the height
	CLineQuery::Normal(xzEntity.Line(), normal);
	color = CLineQuery::Color(xzEntity.Line(), list);
	return(xzEntity.YVal());
}

