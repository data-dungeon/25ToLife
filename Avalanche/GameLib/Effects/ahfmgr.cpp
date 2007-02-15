/////////////////////////////////////////////////////////////////////////////
// Manages a list of animated height fields
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"

#include "Effects/Water/WaterMgr.h"			//pab

#include "EngineHelper/timer.h"
#include "cmdcon/cmdcon.h"


// The singleton
DeclareSingleton(AniHFMgr);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AniHFMgr::AniHFMgr(void) :
list(false),
actorList(false),
disturberList(false)
{
	debugState = AnimatedHeightField::NO_DEBUG;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AniHFMgr::~AniHFMgr(void)
{
	Actor *a = actorList.Head();
	while (a)
	{
		Actor *next = a->next;
		actorList.Unlink(a);
		delete a;
		a = next;
	}

	Disturber *d = disturberList.Head();
	while (d)
	{
		Disturber *next = d->next;
		disturberList.Unlink(d);
		delete d;
		d = next;
	}

	// Update the hf's
	AnimatedHeightField *hf = list.Head();
	while (hf)
	{
		AnimatedHeightField *killMe = hf;
		hf = hf->next;
		list.Unlink(killMe);
		delete killMe;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Add an hf to the list
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::Add(
AnimatedHeightField *hf)
{
	if (!hf)
		return(false);

	list.AddTail(hf);
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Remove the hf from the list (doesn't delete it)
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::Remove(
AnimatedHeightField *hf)
{
	if (!hf)
		return(false);

	list.Unlink(hf);
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Find a hf by name (name of script that created it)
/////////////////////////////////////////////////////////////////////////////
AnimatedHeightField *AniHFMgr::Find(char *scriptName)
{
	AnimatedHeightField *hf = list.Head();
	while (hf)
	{
		if (stricmp(scriptName, hf->GetScriptName()) == 0)
			return(hf);
		hf = hf->next;
	}
	return(NULL);	// not found
}

/////////////////////////////////////////////////////////////////////////////
// Find a hf by pool number - PAB from Tylers water
/////////////////////////////////////////////////////////////////////////////
AnimatedHeightField *AniHFMgr::Find(int PoolNumber)
{
	AnimatedHeightField *hf = list.Head();
	while (hf)
	{
		if (PoolNumber == hf->mPoolNumber)
			return(hf);
		hf = hf->next;
	}
	return(NULL);	// not found
}

/////////////////////////////////////////////////////////////////////////////
// Create a height field based on a script (return false if the script
// doesn't exist)
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::Create(
char *directory,
char *scriptName)
{
	// Allocate a parameter field
	HFParams params;
	Defaults(params);

	// Load in the parameters
	if (!LoadScript(directory, scriptName, params))
		return(false);

	// Create the height field!
	AnimatedHeightField *hf = CreateHeightField(params, false);

	// Did we get one?
	if (!hf)
		return(false);

	// Add it to the scene
	SceneMgr::AddInstance(hf->Instance());

	// record script name for possible later loop up
	hf->SetScriptName(scriptName);

	// Add it to our list
	return(Add(hf));
}

//////////////////////////////////////////////////////////////////////////////////
// This function takes Tyler's param type and fills out Food's water params
// where possible. The rest are using defaults for now. 
// 
// This is primarily for Gamecube to cut down memory requirements for water.
//
// Author: PAB 7/2/03
//////////////////////////////////////////////////////////////////////////////////


bool AniHFMgr::Create(const WaterParams& params, const char* name, int x, int z)
{
	// Allocate a parameter field
	HFParams fparams;
	Defaults(fparams);

	WaterLayersUsed = 0;

	// fill out params from Tylers stuff  - 
	if (1)
//	if (x != -1)			// this fixed Loks training.. hopefully doesnt break anything else.. seems ok
	{
		// generate correct position...
		Vector3 offset;
		Vector3 mPosition;
		offset.Set(x * params.cellSizeX, 0.0f, z * params.cellSizeZ);
		Graphics4x4 rotmtx;
		rotmtx.SetYRotation(params.angle);
		offset = offset * rotmtx;
		mPosition = params.pos + offset;
		fparams.pos.Set(mPosition);		// position
	}
	else
	{
		fparams.pos.Set(params.pos);	// position
	}

	// get correct angle
	fparams.euler.Set(90, Math::Rad2Deg(params.angle), 0);		// angle

	fparams.xGridSize = params.cellSizeX;		
	fparams.yGridSize = params.cellSizeZ;		

	// set number of distortion points based on size of water
	if (fparams.xGridSize < 10 && fparams.yGridSize < 10)
	{
		fparams.xPoints = 2; 
		fparams.yPoints = 2;		
	}
	else if (fparams.xGridSize < 20 && fparams.yGridSize < 20)
	{
		fparams.xPoints = 10; 
		fparams.yPoints = 10;		
	}
	else if (fparams.xGridSize > 200 && fparams.yGridSize > 200)
	{
		fparams.xPoints = 30; 
		fparams.yPoints = 30;
	}
	else
	{
		fparams.xPoints = 15; 
		fparams.yPoints = 15;			
	}

   
	fparams.lockEdges = true;
	fparams.wantVertexColors = true;
	fparams.castShadows = true;
	fparams.usePhysics = 1;

	fparams.geometryAnimateLOD = 10.0f;
	fparams.textureAnimateLOD = 20.0f;
	fparams.waveSpeed = 0.5f; // params.physicsWaveSpeed;
	fparams.viscosity = 0.02f; //params.physicsElasticity;		// ??
//	fparams.waveSpeed = params.physicsWaveSpeed;
//	fparams.viscosity = params.physicsElasticity;		// ??

	// clear the var
	WaterLayersUsed = 0;

	// setup the main water layers now
	int i;
	for ( i=0; i<MAX_LAYERS; i++)
	{
		int	method = 0;

		// do we have a valid texture pass.. if not dont increment the count
		method = GetTextureMethod( params.layers[i].mMethod);
		if (method != -1)
		{
			// layer0
			fparams.material[WaterLayersUsed].textureIndex = params.layers[i].mTextureLayer;
			strcpy (fparams.material[WaterLayersUsed].blend, params.layers[i].mBlendFunc);
			fparams.material[WaterLayersUsed].uWrap = params.layers[i].mScaleU;
			fparams.material[WaterLayersUsed].vWrap = params.layers[i].mScaleV;
			fparams.material[WaterLayersUsed].uFlow = params.layers[i].mFlowU;
			fparams.material[WaterLayersUsed].vFlow = params.layers[i].mFlowV;
			strcpy (fparams.material[WaterLayersUsed].method, params.layers[i].mMethod);
			strcpy (fparams.material[WaterLayersUsed].mapType, params.layers[i].mLayerType);
			WaterLayersUsed++;
	
		}
	}

	int	textureIndex = 0;

	// if we have lava leave this index at 0 else set the ripples for water
	if (!strstr(params.textureFile, "lava"))
		textureIndex = 4;

	// set sprite layers (1 for now..)
	for ( i=0; i< (MAX_LAYERS - WaterLayersUsed); i++)			//pab
	{
		fparams.material[WaterLayersUsed + i].textureIndex = textureIndex;	
		strcpy (fparams.material[WaterLayersUsed + i].blend, "DEFAULT");
		fparams.material[WaterLayersUsed + i].uFlow = 0.0;
		fparams.material[WaterLayersUsed + i].vFlow = 0.0;
		fparams.material[WaterLayersUsed + i].uWrap = 1.0f;
		fparams.material[WaterLayersUsed + i].vWrap = 1.0f;
		strcpy (fparams.material[WaterLayersUsed + i].method, "MODULATE");
		strcpy (fparams.material[WaterLayersUsed + i].mapType, "DECAL");
	}


	// default from Food.. 
	fparams.waveSignal.freq = 0.5f;
	fparams.waveSignal.amp = 0.5f;
	strcpy (fparams.waveSignal.type, "NOISE");

	fparams.xSignal.freq = 1.5f;
	fparams.xSignal.amp = 0.20f;
	strcpy (fparams.xSignal.type, "NOISE");

	fparams.ySignal.freq = 1.5f;
	fparams.ySignal.amp = 0.20f;
	strcpy (fparams.ySignal.type, "NOISE");

	fparams.material[0].uSignal.freq = 1.0f;
	fparams.material[0].uSignal.amp = 0.02f;
	strcpy (fparams.material[0].uSignal.type, "NOISE");

	fparams.material[0].vSignal.freq = 0.5f;
	fparams.material[0].vSignal.amp = 0.4f;
	strcpy (fparams.material[0].vSignal.type, "NONE");

	// texture file
	strcpy (fparams.textureFile,params.textureFile);

	fparams.mWaterLayersUsed = WaterLayersUsed;

	// Create the height field!
	AnimatedHeightField *hf = CreateHeightField(fparams, true);

	// Did we get one?
	if (!hf)
		return(false);

	hf->mWaterLayersUsed = WaterLayersUsed;

	// Add it to the scene
	SceneMgr::AddInstance(hf->Instance());

	// record script name for possible later loop up
	hf->SetScriptName( (char*)name);

	// Add it to our list
	return(Add(hf));
}


//Change the surface height of the specified pool
void AniHFMgr::ChangeSurfaceHeight(int poolNumber, float targetHeight, float metersPerSecond)
{
	AnimatedHeightField *A = Find(poolNumber);
	ASSERT(A);

	if (A != NULL)
	{
		A->ChangeSurfaceHeight(targetHeight, metersPerSecond);
	}
}

//Get the surface height of the specified
float AniHFMgr::GetSurfaceHeight(int poolNumber)
{
	AnimatedHeightField *A = Find(poolNumber);
	ASSERT(A);

	if (A !=NULL)
	{
		return(A->CurrentSurfaceHeight());
	} else
		return(-1.0);
}

/////////////////////////////////////////////////////////////////////////////
// Create all of the "water*.var" height fields in a directory
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::CreateAll(
char *directory)
{
	// Check for scripts
	for (int i = 0; i < 16; i++)
	{
		char script[256];
		sprintf(script, "water%d", i);

		// does the script exist?
		char pathFile[256];
		PathFile(directory, script, pathFile);
		if (!g_console.ScriptExists(pathFile))
			continue;

		// We should probably care about this return,
		// but what can we do?
		Create(directory, script);
	}

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Test
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::Doink(
float force)
{
	// Update the hf's
	AnimatedHeightField *hf = list.Head();
	while (hf)
	{
		hf->Doink(force);
		hf = hf->next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Test
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::Doink(
Vector3 &point,
float force)
{
	// Update the hf's
	AnimatedHeightField *hf = list.Head();
	while (hf)
	{
		hf->Doink(point, force);
		hf = hf->next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Add an actor
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::AddActor(
CActor *actor,
float maxSpeed,
float force)
{
	// duh
	if (!actor)
		return;

	// Make sure they don't add twice
	RemoveActor(actor);

	// allocate a record
	Actor *a = new Actor;
	if (!a)
		return;

	// fill it in
	a->actor = actor;
	a->maxSpeed = maxSpeed;
	a->force = force;

	// Put it in the list
	actorList.AddTail(a);
}

/////////////////////////////////////////////////////////////////////////////
// Remove an actor
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::RemoveActor(
CActor *actor)
{
	// duh
	if (actor == NULL)
		return;

	// find it in the list
	Actor *a = actorList.Head();
	while (a)
	{
		// is this our match?
		if (a->actor == actor)
		{
			actorList.Unlink(a);
			delete a;
			return;
		}

		// Try the next one
		a = a->next;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AniHFMgr::DisturberHandle AniHFMgr::AddDisturber(
Vector3 *pos,
Vector3 *velocity,
float maxSpeed,
float force)
{
	// duh
	if (!pos)
		return(NULL);

	// allocate a record
	Disturber *d = new Disturber;
	if (!d)
		return(NULL);

	// fill it in
	d->pos = pos;
	d->velocity = velocity;
	d->maxSpeed = maxSpeed;
	d->force = force;

	// Put it in the list
	disturberList.AddTail(d);

	return((DisturberHandle)d);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::RemoveDisturber(
DisturberHandle disturber)
{
	Disturber *d = (Disturber *)disturber;
	if (!d)
		return;

	disturberList.Unlink(d);
	delete d;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::SetDisturberMaxSpeed(
DisturberHandle disturber,
float maxSpeed
)
{
	Disturber *d = (Disturber *)disturber;
	if (d)
		d->maxSpeed = maxSpeed;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::SetDisturberForce(
DisturberHandle disturber,
float force
)
{
	Disturber *d = (Disturber *)disturber;
	if (d)
		d->force = force;
}

/////////////////////////////////////////////////////////////////////////////
// Call all of the updates
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::Update(
Graphics4x4 &camera)
{

	float deltaTime = g_timer.GetFrameSec( );

	if( g_timer.IsPaused() )
		return;

	Profiler__Start(PROFILE_ANIHF);

	// Update the actors
	Actor *a = actorList.Head();
	while (a)
	{
		// Compute this once
		Vector3 v = a->actor->GetVelocityInWorld();
		v.y( v.y() * 0.25f);
		float currentSpeed = v.Length();
		float speedRatio = Math::Clamp(currentSpeed / a->maxSpeed, 0.0f, 1.0f);

		// Tell each hf about each actor
		if (!Math::Zero(speedRatio))
		{
			float realForce = speedRatio * a->force;
			AnimatedHeightField *hf = list.Head();
			while (hf)
			{
				hf->ApplyForce(a->actor->GetBodyInWorld(), realForce);
				hf = hf->next;
			}
		}

		// Go to the next actor
		a = a->next;
	}

	// Update the disturbers
	Disturber *d = disturberList.Head();
	while (d)
	{
		// Compute this once
		float speedRatio;
		if (!d->velocity)
			speedRatio = 1.0f;
		else
		{
			float currentSpeed = d->velocity->Length();
			speedRatio = Math::Clamp(currentSpeed / d->maxSpeed, 0.0f, 1.0f);
		}

		// Tell each hf about each actor
		if (!Math::Zero(speedRatio))
		{
			float realForce = speedRatio * d->force;
			AnimatedHeightField *hf = list.Head();
			while (hf)
			{
				hf->ApplyForce(*d->pos, realForce);
				hf = hf->next;
			}
		}

		// Go to the next actor
		d = d->next;
	}

	// Update the hf's
	AnimatedHeightField *hf = list.Head();
	while (hf)
	{
		hf->Update(deltaTime, camera);
		hf = hf->next;
	}

	Profiler__Stop(PROFILE_ANIHF);
}

/////////////////////////////////////////////////////////////////////////////
// Collide a segment with all the height fields
// Returns false if no collision
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::Test(
Segment &seg,
AnimatedHeightField::Collision &collision)
{
	Profiler__Start(PROFILE_ANIHF);

	// No collision yet
	collision.hf = NULL;
	collision.d = 100000.0f;

	// Check each individual hf
	AnimatedHeightField *hf = list.Head();
	while (hf)
	{
		AnimatedHeightField::Collision localCollision;
		bool localHit = hf->Test(seg, localCollision);
		if (localHit && localCollision.d < collision.d)
			collision = localCollision;

		hf = hf->next;
	}

	Profiler__Stop(PROFILE_ANIHF);
	return(collision.hf != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Collide a segment with all the height fields
// Returns the Pool Number if a collision exists
/////////////////////////////////////////////////////////////////////////////
int AniHFMgr::GetCollisionPoolNumber(
Segment &seg,
AnimatedHeightField::Collision &collision)
{
	Profiler__Start(PROFILE_ANIHF);

	// No collision yet
	collision.hf = NULL;
	collision.d = 100000.0f;

	// Check each individual hf
	AnimatedHeightField *hf = list.Head();
	while (hf)
	{
		AnimatedHeightField::Collision localCollision;
		bool localHit = hf->Test(seg, localCollision);
		if (localHit && localCollision.d < collision.d)
			collision = localCollision;

		hf = hf->next;
	}

	Profiler__Stop(PROFILE_ANIHF);
	
	if (collision.hf == NULL)
		return -1;
	else
		return collision.hf->mPoolNumber;
}
/////////////////////////////////////////////////////////////////////////////
// initialize the params
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::Defaults(
HFParams &params)
{
	// clear this
	memset(&params, '\0', sizeof(HFParams));

	params.xGridSize = 1.0f;
	params.yGridSize = 1.0f;

	params.shadowLightSource.Set(0.0f, 1.0f, 0.0f);
	params.shadowColor.rgba[0] = 100;
	params.shadowColor.rgba[1] = 100;
	params.shadowColor.rgba[2] = 100;
	params.shadowColor.rgba[3] = 100;
	params.shadowLightYFactor = 1.0f;
	params.shadowTestD = 100.0f;

	params.baseColor.rgba[0] = 255;
	params.baseColor.rgba[1] = 255;
	params.baseColor.rgba[2] = 255;
	params.baseColor.rgba[3] = 255;

	params.defaultVertexColor.rgba[0] = 255;
	params.defaultVertexColor.rgba[1] = 255;
	params.defaultVertexColor.rgba[2] = 255;
	params.defaultVertexColor.rgba[3] = 255;

	params.waveSpeed = 0.5f;
	params.viscosity = 0.01f;

	params.geometryAnimateLOD = 10.0f;
	params.textureAnimateLOD = 20.0f;
	params.nothingLOD = 100.0f;

	params.isSeeThru = true;

	// Now init the material
	for (int i = 0; i < ANIHF_MAXTEXTURES; i++)
		Defaults(params.material[i]);
}

/////////////////////////////////////////////////////////////////////////////
// Setup the defaults for the material
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::Defaults(
HFMaterial &material)
{
	// assume it is already 0's
	material.uWrap = 1.0f;
	material.vWrap = 1.0f;
	strcpy(material.mapType, "DECAL");
	material.light.Set(0.0f, 1.0f, 0.0f);
}

/////////////////////////////////////////////////////////////////////////////
// Load the params from a script
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::LoadScript(
char *directory,
char *scriptName,
HFParams &params)
{
	// Alias the context
	char *context = scriptName;

	// Setup all the variables
	SetupVariables(context, params);

	// Build the script file name & bind it
	char pathFile[256];
	PathFile(directory, scriptName, pathFile);
	g_console.BindScript(context, pathFile);

	// Execute the script
	bool ret = g_console.ExecuteContext(context);

	// Now clear this context (so we don't get dangling pointers)
	g_console.DeleteContext(context);

	// If we couldn't load a file, we've failed
	return(ret);
}

/////////////////////////////////////////////////////////////////////////////
// Infer the type
/////////////////////////////////////////////////////////////////////////////
AniHFMgr::HFType AniHFMgr::InferType(
HFParams &params)
{
	// error conditions (this should be more robust)
	if (params.xPoints < 2 || params.yPoints < 2)
		return(UNDEFINED);

	// Must have two paths to be a pathed
	if (params.sidePathA[0] != '\0' && params.sidePathB[0] != '\0')
		return(PATHED);
	else
		return(RECTANGLE);
}

/////////////////////////////////////////////////////////////////////////////
// Create a rectangular height field
/////////////////////////////////////////////////////////////////////////////
AnimatedHeightField *AniHFMgr::CreateHeightField(
HFParams &params,
bool getLayersFromParams)
{
	// What type are they asking for
	HFType type = InferType(params);
	if (type == UNDEFINED)
		return(NULL);

	// create the height field
	AnimatedHeightField *hf;
	if (type != PATHED)
		hf = new AnimatedHeightField;
	else
		hf = new CurvedAnimatedHeightField;
	if (!hf)
		return(NULL);

	// set the WaterLayersUsed now (PAB)
	if( getLayersFromParams)
	{
		hf->mWaterLayersUsed = params.mWaterLayersUsed;			
	}

	// Setup the materials
	if (!SetupMaterials(*hf, params))
	{
		delete hf;
		return(NULL);
	}

	if( !getLayersFromParams)
	{
		hf->mWaterLayersUsed = params.mWaterLayersUsed;			
	}
	

	// Build the vertex color
	ts_bRGBA vertexColor = MakeColor(params.defaultVertexColor);

	// Build the geometry
	if (!hf->BuildGeometry(params.xPoints, params.yPoints,
									params.xGridSize, params.yGridSize,
									params.lighting, params.lockEdges,
									params.wantVertexColors, params.isSeeThru,
									&vertexColor))
	{
		delete hf;
		return(NULL);
	}

	// Position the object in the world
	if (type == PATHED)
	{
		// Stretch it between some paths
		if (!SplineFit((CurvedAnimatedHeightField &)*hf, params))
		{
			delete hf;
			return(NULL);
		}
	}
	else
	{
		// Just place the rectangle
		Graphics4x4 m;
		m.SetXYZRotationDeg(params.euler);
		m.CatTranslation(params.pos.x(), params.pos.y(), params.pos.z());
		hf->Place(m);
	}

	// expand height of totally flat animated height fields so that their bounding box has a bit of volume for
	// placement in visibility region
	if (Math::Equal(hf->instance->BoundingBox().Max.y(), hf->instance->BoundingBox().Min.y(), 0.05f))
		hf->instance->BoundingBox().Max.y( hf->instance->BoundingBox().Min.y() + 0.05f );

	// Now setup all the other stuff
	if (!SetupGeometrySignals(*hf, params) ||
			!SetupUVSignals(*hf, params) ||
			!SetupLOD(*hf, params) ||
			!SetupPhysics(*hf, params))
	{
		delete hf;
		return(NULL);
	}

	// Cast shadows
	if (params.castShadows && params.wantVertexColors)
		CastShadows(*hf, params);

	// Find surfaces
	if (params.autoFindSurfaces && params.usePhysics)
		FindSurfaces(*hf, params);

	// Power through positive real estate!
	return(hf);
}

/////////////////////////////////////////////////////////////////////////////
// Setup all the variables
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::SetupVariables(
char *context,
HFParams &params)
{
	// what is our type?
	g_console.CreateVar(context, "position", (Vector3Packed*)&params.pos);
	g_console.CreateVar(context, "euler", (Vector3Packed*)&params.euler);

	// Or conversely, our paths?
	g_console.CreateVar(context, "sidePathA", params.sidePathA, sizeof(params.sidePathA) - 1);
	g_console.CreateVar(context, "sidePathB", params.sidePathB, sizeof(params.sidePathB) - 1);

	// How many points do we have?
	g_console.CreateVar(context, "xPoints", params.xPoints);
	g_console.CreateVar(context, "yPoints", params.yPoints);

	// What is our grid size?
	g_console.CreateVar(context, "xGridSize", params.xGridSize);
	g_console.CreateVar(context, "yGridSize", params.yGridSize);

	// Our base color
	g_console.CreateVar(context, "baseColorR", params.baseColor.rgba[0]);
	g_console.CreateVar(context, "baseColorG", params.baseColor.rgba[1]);
	g_console.CreateVar(context, "baseColorB", params.baseColor.rgba[2]);
	g_console.CreateVar(context, "baseColorA", params.baseColor.rgba[3]);

	// Some flags
	g_console.CreateVar(context, "lighting", params.lighting);
	g_console.CreateVar(context, "lockEdges", params.lockEdges);
	g_console.CreateVar(context, "isSeeThru", params.isSeeThru);

	// Vertex color stuff
	g_console.CreateVar(context, "wantVertexColors", params.wantVertexColors);
	g_console.CreateVar(context, "defaultVertexColorR", params.defaultVertexColor.rgba[0]);
	g_console.CreateVar(context, "defaultVertexColorG", params.defaultVertexColor.rgba[1]);
	g_console.CreateVar(context, "defaultVertexColorB", params.defaultVertexColor.rgba[2]);
	g_console.CreateVar(context, "defaultVertexColorA", params.defaultVertexColor.rgba[3]);

	// Shadow stuff
	g_console.CreateVar(context, "castShadows", params.castShadows);
	g_console.CreateVar(context, "shadowLightSource", (Vector3Packed*)&params.shadowLightSource);
	g_console.CreateVar(context, "shadowColorR", params.shadowColor.rgba[0]);
	g_console.CreateVar(context, "shadowColorG", params.shadowColor.rgba[1]);
	g_console.CreateVar(context, "shadowColorB", params.shadowColor.rgba[2]);
	g_console.CreateVar(context, "shadowColorA", params.shadowColor.rgba[3]);
	g_console.CreateVar(context, "shadowLightYFactor", params.shadowLightYFactor);
	g_console.CreateVar(context, "shadowTestD", params.shadowTestD);

	// Physics parameters
	g_console.CreateVar(context, "usePhysics", params.usePhysics);
	g_console.CreateVar(context, "autoFindSurfaces", params.autoFindSurfaces);
	g_console.CreateVar(context, "waveSpeed", params.waveSpeed);
	g_console.CreateVar(context, "viscosity", params.viscosity);

	// LOD stuff
	g_console.CreateVar(context, "geometryAnimateLOD", params.geometryAnimateLOD);
	g_console.CreateVar(context, "textureAnimateLOD", params.textureAnimateLOD);
	g_console.CreateVar(context, "nothingLOD", params.nothingLOD);

	// Setup the noise parameters for Z
	SetupSignalVariables(context, "wave", params.waveSignal);

	// Setup the XY positional variance
	SetupSignalVariables(context, "xSignal", params.xSignal);
	SetupSignalVariables(context, "ySignal", params.ySignal);

	// Setup all the materals
	g_console.CreateVar(context, "textureFile", params.textureFile, sizeof(params.textureFile) - 1);
	for (int i = 0; i < ANIHF_MAXTEXTURES; i++)
		SetupMaterialVariables(context, i, params.material[i]);
}

/////////////////////////////////////////////////////////////////////////////
// Setup a material
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::SetupMaterialVariables(
char *context,
int layer,
HFMaterial &m)
{
	char temp[64];

	// Setup the mapping technique
	g_console.CreateVar(context, VariableName("textureIndex", layer, temp), m.textureIndex);

	g_console.CreateVar(context, VariableName("method", layer, temp), m.method, sizeof(m.method) - 1);
	g_console.CreateVar(context, VariableName("blend", layer, temp), m.blend, sizeof(m.blend) - 1);
	g_console.CreateVar(context, VariableName("noFilter", layer, temp), m.noFilter);
	g_console.CreateVar(context, VariableName("diffuseMap", layer, temp), m.diffuseMap);

	g_console.CreateVar(context, VariableName("uWrap", layer, temp), m.uWrap);
	g_console.CreateVar(context, VariableName("vWrap", layer, temp), m.vWrap);

	g_console.CreateVar(context, VariableName("mapType", layer, temp), m.mapType, sizeof(m.mapType) - 1);

	g_console.CreateVar(context, VariableName("light", layer, temp), (Vector3Packed*)&m.light);

	g_console.CreateVar(context, VariableName("lowHeight", layer, temp), m.lowHeight);
	g_console.CreateVar(context, VariableName("highHeight", layer, temp), m.highHeight);

	g_console.CreateVar(context, VariableName("turbXFactor", layer, temp), m.turbXFactor);
	g_console.CreateVar(context, VariableName("turbYFactor", layer, temp), m.turbYFactor);

	SetupSignalVariables(context, VariableName("uSignal", layer, temp), m.uSignal);
	SetupSignalVariables(context, VariableName("vSignal", layer, temp), m.vSignal);

	// Setup the flow
	g_console.CreateVar(context, VariableName("uFlow", layer, temp), m.uFlow);
	g_console.CreateVar(context, VariableName("vFlow", layer, temp), m.vFlow);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::SetupSignalVariables(
char *context,
char *signalName,
SignalParams &signal)
{
	char variableName[64];
	g_console.CreateVar(context, VariableName(signalName, "Freq", variableName), signal.freq);
	g_console.CreateVar(context, VariableName(signalName, "Amp", variableName), signal.amp);
	g_console.CreateVar(context, VariableName(signalName, "Type", variableName), signal.type, sizeof(signal.type) - 1);
}

/////////////////////////////////////////////////////////////////////////////
// What is the full path file?
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::PathFile(
const char *path,
const char *file,
char *pathFile)
{
	// Build the script file name
	strcpy(pathFile, path);
	int len = strlen(pathFile);
	if (len > 0 && pathFile[len - 1] != '\\')
		strcat(pathFile, "\\");
	strcat(pathFile, file);
}

/////////////////////////////////////////////////////////////////////////////
// Make a variable name out of a base and string/number
/////////////////////////////////////////////////////////////////////////////
char *AniHFMgr::VariableName(
const char *base,
char *extra,
char *variableName)
{
	strcpy(variableName, base);
	strcat(variableName, extra);
	return(variableName);
}

/////////////////////////////////////////////////////////////////////////////
// Make a variable name out of a base and string/number
/////////////////////////////////////////////////////////////////////////////
char *AniHFMgr::VariableName(
const char *base,
int number,
char *variableName)
{
	sprintf(variableName, "Layer%d_%s", number, base);
	return(variableName);
}

/////////////////////////////////////////////////////////////////////////////
// Translate a signal type string into a type
/////////////////////////////////////////////////////////////////////////////
SigGen::TYPE AniHFMgr::GetSignalType(
SignalParams &s)
{
	// Special case
	if (Math::Zero(s.amp) || Math::Zero(s.freq))
		return(SigGen::NONE);

	if (strcmpi(s.type, "SIN") == 0)
		return(SigGen::SIN);
	else if (strcmpi(s.type, "COS") == 0)
		return(SigGen::COS);
	else if (strcmpi(s.type, "NOISE") == 0)
		return(SigGen::NOISE);
	else if (strcmpi(s.type, "FASTNOISE") == 0)
		return(SigGen::FASTNOISE);
	else if (strcmpi(s.type, "FASTSMOOTHNOISE") == 0)
		return(SigGen::FASTSMOOTHNOISE);
	else
		return(SigGen::NONE);
}

/////////////////////////////////////////////////////////////////////////////
// Translate texture method
/////////////////////////////////////////////////////////////////////////////
int AniHFMgr::GetTextureMethod(
const char *method)
{
	if (strcmpi(method, "REPLACE") == 0)
		return(TS_METHOD_REPLACE);
	else if (strcmpi(method, "MODULATE") == 0)
		return(TS_METHOD_MODULATE);
	else if (strcmpi(method, "DECAL") == 0)
		return(TS_METHOD_DECAL);
	else if (strcmpi(method, "CONSTANTALPHA") == 0)
		return(TS_METHOD_CONSTANTALPHA);
	else if (strcmpi(method, "INTERPOLATE") == 0)
		return(TS_METHOD_INTERPOLATE);
	else if (strcmpi(method, "DOTPRODUCT") == 0)
		return(TS_METHOD_DOTPRODUCT);
	else
		return(-1);
}

/////////////////////////////////////////////////////////////////////////////
// Translate texture blend
/////////////////////////////////////////////////////////////////////////////
int AniHFMgr::GetTextureBlend(
const char *blend)
{
	if (strcmpi(blend, "DIFFUSE") == 0)
		return(RS_BLENDFUNC_DIFFUSE);
	else if (strcmpi(blend, "NONE") == 0)
		return(RS_BLENDFUNC_NONE);
	else if (strcmpi(blend, "SATURATE") == 0)
		return(RS_BLENDFUNC_SATURATE);
	else if (strcmpi(blend, "FIXEDALPHA") == 0)
		return(RS_BLENDFUNC_FIXEDALPHA);
	else
		return(RS_BLENDFUNC_DEFAULT);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AnimatedHeightField::MAPTYPE AniHFMgr::GetMapType(
const char *mapType)
{
	if (strcmpi(mapType, "SPECULAR") == 0)
		return(AnimatedHeightField::SPECULAR);
	else if (strcmpi(mapType, "ENVIRONMENT") == 0)
		return(AnimatedHeightField::ENVIRONMENT);
	else if (strcmpi(mapType, "HEIGHT") == 0)
		return(AnimatedHeightField::HEIGHT);
	else if (strcmpi(mapType, "NEGHEIGHT") == 0)
		return(AnimatedHeightField::NEGHEIGHT);
	else if (strcmpi(mapType, "ABSHEIGHT") == 0)
		return(AnimatedHeightField::ABSHEIGHT);
	else if (strcmpi(mapType, "TURBULENCE") == 0)
		return(AnimatedHeightField::TURBULENCE);
	else if (strcmpi(mapType, "BUMPMAP") == 0)
		return(AnimatedHeightField::BUMPMAP);
	else
		return(AnimatedHeightField::DECAL);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::SetupMaterials(
AnimatedHeightField &hf,
HFParams &params)
{
	// Create the baseColor
	ts_bRGBA baseColor = MakeColor(params.baseColor);

	// Build our layer init from the specified materials
	AnimatedHeightField::TextureLayerInit layer[ANIHF_MAXTEXTURES];
	int layers = 0;
	for (int i = 0; i < ANIHF_MAXTEXTURES; i++)
	{
		// Is this a valid material?
		if (Convert(params.material[i], layer[layers]))
			++layers;
	}

	params.mWaterLayersUsed = layers;

	// TJC - a bug was introduced by the combination of the sprite layers 
	// begin added, and the new engine.  If
	// this isnt here, water created via var files wont tile the textures.
	if(hf.mWaterLayersUsed == 0)
		hf.mWaterLayersUsed = layers;

	// Do we have layers
	if (layers == 0 || params.textureFile[0] == '\0')
		return(hf.SetMaterial(baseColor));
	else
		return(hf.SetMaterial(baseColor, params.textureFile, layers, layer));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::SetupGeometrySignals(
AnimatedHeightField &hf,
HFParams &params)
{
	// Setup the noise parameters for Z
	SigGen::TYPE type = GetSignalType(params.waveSignal);
	hf.SetWave(params.waveSignal.freq, params.waveSignal.amp, type);

	// Setup the XY positional variance
	SigGen::TYPE xType = GetSignalType(params.xSignal);
	SigGen::TYPE yType = GetSignalType(params.ySignal);
	hf.SetXYVariance(params.xSignal.freq, params.xSignal.amp, xType,
							params.ySignal.freq, params.ySignal.amp, yType);

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::SetupUVSignals(
AnimatedHeightField &hf,
HFParams &params)
{
	// Build our layer init from the specified materials
	AnimatedHeightField::TextureLayerInit temp;
	int layer = 0;
	for (int i = 0; i < ANIHF_MAXTEXTURES; i++)
	{
		HFMaterial &m = params.material[i];

		// Is this a valid material?
		if (Convert(m, temp))
		{
			// Setup uv variance for a texture set
			SigGen::TYPE uType = GetSignalType(m.uSignal);
			SigGen::TYPE vType = GetSignalType(m.vSignal);
			hf.SetUVVariance(layer,
									m.uSignal.freq, m.uSignal.amp, uType,
									m.vSignal.freq, m.vSignal.amp, vType);

			// Setup the flow
			hf.SetUVFlow(layer, m.uFlow, m.vFlow);
			++layer;
		}
	}

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Setup culling
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::SetupLOD(
AnimatedHeightField &hf,
HFParams &params)
{
	hf.SetLODDistance(params.geometryAnimateLOD, params.textureAnimateLOD, params.nothingLOD);
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::SetupPhysics(
AnimatedHeightField &hf,
HFParams &params)
{
	if (params.usePhysics)
		hf.SetupPhysics(params.waveSpeed, params.viscosity);

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Convert a material into a texturelayerinit
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::Convert(
HFMaterial &material,
AnimatedHeightField::TextureLayerInit &init)
{
	// What is the mehod (not specified means NO material here
	init.method = GetTextureMethod(material.method);
	if (init.method < 0)
		return(false);

	// copy everything else over
	init.textureIndex = material.textureIndex;
	init.blend = GetTextureBlend(material.blend);
	if (material.noFilter)
		init.filter = TS_FILTER_POINTSAMPLE;
	else
		init.filter = TS_FILTER_BILINEAR;
	if (material.diffuseMap)
		init.flags = TEXTURE_IS_DIFFUSE_MAP;
	else
		init.flags = 0;
	init.uWrap = material.uWrap;
	init.vWrap = material.vWrap;
	init.mapType = GetMapType(material.mapType);
	switch (init.mapType)
	{
		case AnimatedHeightField::SPECULAR:
			init.light = material.light;
			break;

		case AnimatedHeightField::HEIGHT:
		case AnimatedHeightField::NEGHEIGHT:
		case AnimatedHeightField::ABSHEIGHT:
			init.low = material.lowHeight;
			init.high = material.highHeight;
			break;

		case AnimatedHeightField::TURBULENCE:
			init.turbXFactor = material.turbXFactor;
			init.turbYFactor = material.turbYFactor;
			break;
	};

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline ts_bRGBA AniHFMgr::MakeColor(
HFColor &c)
{
	ts_bRGBA color;

	color.R = c.rgba[0];
	color.G = c.rgba[1];
	color.B = c.rgba[2];
	color.A = c.rgba[3];

	return(color);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline ts_bRGBA AniHFMgr::MakeColor(
HFColor &a,
HFColor &b,
float scale)
{
	scale = Math::Clamp(scale);

	float red = (float)a.rgba[0] + ((float)b.rgba[0] - (float)a.rgba[0]) * scale;
	float green = (float)a.rgba[1] + ((float)b.rgba[1] - (float)a.rgba[1]) * scale;
	float blue = (float)a.rgba[2] + ((float)b.rgba[2] - (float)a.rgba[2]) * scale;
	float alpha = (float)a.rgba[3] + ((float)b.rgba[3] - (float)a.rgba[3]) * scale;

	ts_bRGBA color;
	color.R = (int)red;
	color.G = (int)green;
	color.B = (int)blue;
	color.A = (int)alpha;

	return(color);
}


/////////////////////////////////////////////////////////////////////////////
// Build and place a hf around a spline
/////////////////////////////////////////////////////////////////////////////
bool AniHFMgr::SplineFit(
CurvedAnimatedHeightField &hf,
HFParams &params)
{
	// make sure there is a path collection
	PathCollection *pathCollection = PathCollection::Collection();
	if (!pathCollection)
		return(false);

	// Get our two paths
	SplinePath *pathA = pathCollection->Named(params.sidePathA);
	SplinePath *pathB = pathCollection->Named(params.sidePathB);
	if (!pathA || !pathB)
		return(false);

	// Set this bad boy up
	return(hf.SetSplines(*pathA, *pathB));
}

/////////////////////////////////////////////////////////////////////////////
// cast shadows
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::CastShadows(
AnimatedHeightField &hf,
HFParams &params)
{
	// setup our node info
	AnimatedHeightField::NodeInfo nodeInfo;

	Vector3 up( 0.0f, 1.0f, 0.0f );
	up *= 10.0f;

	Vector3 dir = params.shadowLightSource;
	dir.y( dir.y() * params.shadowLightYFactor);
	dir.Normalize();
	dir *= params.shadowTestD;

	hf.mVtxBuffer.Lock();

	for (int y = 0; y < hf.YPoints(); y++)
		for (int x = 0; x < hf.XPoints(); x++)
		{
			g_collisionSystem.ResetStack();
			CLineEntity entity;
			entity.SetPositiveInclude(WORLD_CATEGORY);
			entity.Line().SetFindMode(CLineCollisionObject::FIND_ANY);
			entity.Line().SetOriginBasis(hf.PointInWorld(x, y), up);
			if (g_collisionSystem.Test(&entity) != NULL)
			{
				nodeInfo.override = AnimatedHeightField::OVERRIDE_COLOR;
				nodeInfo.color = MakeColor(params.shadowColor, params.defaultVertexColor, 0);
				hf.ModifyNode(x, y, nodeInfo);
			}
			
			entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
			entity.Line().Basis() = dir;
			if (g_collisionSystem.Test(&entity) != NULL)
			{
				float d = entity.Line().FoundPoint() / params.shadowTestD;
				nodeInfo.color = MakeColor(params.shadowColor, params.defaultVertexColor, d);
				nodeInfo.override = AnimatedHeightField::OVERRIDE_COLOR;
				if (d < 0.5f)
				{
					nodeInfo.override |= AnimatedHeightField::OVERRIDE_SHADOWED;
					nodeInfo.shadowed = true;
				}
				hf.ModifyNode(x, y, nodeInfo);
			}
		}

	hf.mVtxBuffer.UnLock();
}

/////////////////////////////////////////////////////////////////////////////
// Find surfaces for physics
/////////////////////////////////////////////////////////////////////////////
void AniHFMgr::FindSurfaces(
AnimatedHeightField &hf,
HFParams &params)
{
	// setup our node info
	AnimatedHeightField::NodeInfo nodeInfo;
	nodeInfo.override = AnimatedHeightField::OVERRIDE_WAVESPEED;
	nodeInfo.waveSpeed = 0.0f;

	Vector3 up( 0.0f, 1.0f, 0.0f );
	up *= 1.5f;

	hf.mVtxBuffer.Lock();

	for (int y = 0; y < hf.YPoints(); y++)
		for (int x = 0; x < hf.XPoints(); x++)
		{
			g_collisionSystem.ResetStack();
			CLineEntity detectEntity;
			detectEntity.SetPositiveInclude(WORLD_CATEGORY);
			detectEntity.Line().SetFindMode(CLineCollisionObject::FIND_ANY);
			detectEntity.Line().SetOriginBasis(hf.PointInWorld(x, y), up);
			if (g_collisionSystem.Test(&detectEntity) != NULL)
				hf.ModifyNode(x, y, nodeInfo);
		}

	hf.mVtxBuffer.UnLock();
}

/////////////////////////////////////////////////////////////////////////////
// Cycle through debug states
/////////////////////////////////////////////////////////////////////////////
const char* AniHFMgr::IncrementDebugState(void)
{
	if (++debugState > AnimatedHeightField::ALL_DEBUG)
		debugState = AnimatedHeightField::NO_DEBUG;

	AnimatedHeightField *hf = list.Head();
	while (hf)
	{
		hf->debugState = debugState;
		hf = hf->next;
	}

#if 0
	switch(debugState)
	{
	 case AnimatedHeightField::NO_DEBUG:
		return("Water Debug: OFF");
	 case AnimatedHeightField::NO_PHYSICS:
		return("Water Debug: NO PHYSICS ");
	 case AnimatedHeightField::NO_RENDER:
		return("Water Debug: NO RENDER");
	 case AnimatedHeightField::ALL_DEBUG:
		return("Water Debug: NO PHYSICS or RENDER");
	}
#else
	switch(debugState)
	{
	 case 0:
		return("Water Debug: OFF");
	 case 1:
		return("Water Debug: NO PHYSICS ");
	 case 2:
		return("Water Debug: NO RENDER");
	 case 3:
		return("Water Debug: NO PHYSICS or RENDER");
	}
#endif
	return("Water Debug: ERROR");
}
