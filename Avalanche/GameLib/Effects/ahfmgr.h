/////////////////////////////////////////////////////////////////////////////
// Manages a list of animated height fields
/////////////////////////////////////////////////////////////////////////////
#ifndef AHFMGR_H
#define AHFMGR_H

#include "container/dllist.h"
#include "stage/cactor.h"
#include "Effects/anihf.h"
#include "Effects/canihf.h"
#include "Effects/Water/WaterMgr.h"

// Forwards
//class AnimatedHeightField;

/////////////////////////////////////////////////////////////////////////////
// The screen space effect manager
/////////////////////////////////////////////////////////////////////////////
class AniHFMgr
{
public:
	AniHFMgr(void);
	~AniHFMgr(void);

	// Add an hf to the the list
	bool Add(AnimatedHeightField *hf);

	// Remove the hf from the list (doesn't delete it)
	bool Remove(AnimatedHeightField *hf);

	// Find a hf by name (name of script that created it)
	AnimatedHeightField *Find(char *scriptName);

	// Find a hf from the associated pool number (PAB)
	AnimatedHeightField *Find(int PoolNumber);

	// Create a height field based on a script
	bool Create(char *directory, char *scriptName);

	// Create a height field based off Tylers params - PAB
	bool Create(const WaterParams& params, const char *name, int x, int z);

	// Create all of the "water*.var" height fields in a directory
	bool CreateAll(char *directory);

	// Test (hits all of them)
	void Doink(float force = 1.0f);

	// Test (hits all of them)
	void Doink(Vector3 &point, float force = 1.0f);

	// Add an actor
	void AddActor(CActor *actor, float maxSpeed, float force);

	// Remove an actor
	void RemoveActor(CActor *actor);

		//Change the surface height of a specific pool
	void ChangeSurfaceHeight(int poolNumber, float targetHeight, float metersPerSecond);

	//Get the surface height of a specific pool
	float GetSurfaceHeight(int poolNumber);


	// Add a generic disturbing force
	typedef void *DisturberHandle;
	DisturberHandle AddDisturber(Vector3 *pos, Vector3 *velocity, float maxSpeed, float force);
	void RemoveDisturber(DisturberHandle disturber);

	// Diddle with parameters
	void SetDisturberMaxSpeed(DisturberHandle disturber, float maxSpeed);
	void SetDisturberForce(DisturberHandle disturber, float force);

	// Call all of the updates
 	void Update(Graphics4x4 &camera);

	// Collide a segment with all the height fields
	// Returns false if no collision
	bool Test(Segment &seg, AnimatedHeightField::Collision &collision);
	
	// returns the Pool that we collided with
	int GetCollisionPoolNumber(Segment &seg,AnimatedHeightField::Collision &collision);


	const char* IncrementDebugState(void);

	int		WaterLayersUsed;
	int		GetWaterLayersUsed() { return WaterLayersUsed; }

private:
	// The list of managed hf's
	DoublyLinkedList<AnimatedHeightField>	list;

	typedef enum
	{
		UNDEFINED = 0,
		RECTANGLE,
		PATHED
	} HFType;

	struct SignalParams
	{
		float 			freq;
		float 			amp;
		char				type[32]; // NONE, SIN, COS, NOISE, FASTNOISE, FASTSMOOTHNOISE
	};

	struct HFMaterial
	{
		// Setup the mapping technique
		int				textureIndex;			// Texture index in .dbl file
		char 	  			method[32];        	// REPLACE, MODULATE, DECAL, CONSTANTALPHA, INTERPOLATE
		char			  	blend[32];	  			// DEFAULT, DIFFUSE, NONE, FIXEDALPHA
		bool				noFilter;				// Texture filtering on or off?
		bool				diffuseMap;				// Diffuse mapping?
		float				uWrap;					// Number of times to wrap u
		float				vWrap;					// Number of times to wrap v
		char 				mapType[32];			// uv calculation DECAL, SPECULAR, ENVIRONMENT,	HEIGHT, NEGHEIGHT, ABSHEIGHT, TURBULENCE
		Vector3			light;	 				// negated light in world coordinates
		float 			lowHeight;           //
		float 			highHeight;				// for HEIGHT maps
		float 			turbXFactor;			// turbulence X factor
		float 			turbYFactor;         // turbulence Y factor

		// Setup uv variance for a texture set
		SignalParams	uSignal;
		SignalParams	vSignal;

		// Setup the flow
		float 			uFlow;
		float 			vFlow;

//		bool			RenderFlag;			// 1 to render, 0 to skip
	};

	struct HFColor
	{
		int		rgba[4];
	};

	struct HFParams
	{
		// What is our position/orientation
		Vector3			pos;
		Vector3			euler;

		// Or conversely, our paths?
		char				sidePathA[32];
		char				sidePathB[32];

		// How many points do we have?
		int				xPoints, yPoints;

		// What is our grid size?
		float				xGridSize, yGridSize;

		// Our base color
		HFColor			baseColor;

		// Some flags
		bool				lighting;
		bool				lockEdges;
		bool				isSeeThru;

		// Are we going to use vertex colors
		bool				wantVertexColors;
		bool				castShadows;
		HFColor			defaultVertexColor;
		Vector3			shadowLightSource;
		HFColor			shadowColor;
		float				shadowLightYFactor;
		float				shadowTestD;

		// Physics parameters
		bool				usePhysics;
		float				waveSpeed;
		float				viscosity;
		bool				autoFindSurfaces;

		// LOD stuff
		float 			geometryAnimateLOD;
		float 			textureAnimateLOD;
		float				nothingLOD;

		// Setup the noise parameters for Z
		SignalParams	waveSignal;

		// Setup the XY positional variance
		SignalParams	xSignal;
		SignalParams	ySignal;

		// The materials
		char				textureFile[128];
		HFMaterial		material[ANIHF_MAXTEXTURES];

		int				mWaterLayersUsed;			//pab
	};

	// initialize the params
	void Defaults(HFParams &params);
	void Defaults(HFMaterial &material);

	// Load the params from a script
	bool LoadScript(char *directory, char *scriptName, HFParams &params);

	// Create the height field
	AnimatedHeightField *CreateHeightField(HFParams &params, bool getLayersFromParams);

	// Setup all the variables
	void SetupVariables(char *context, HFParams &params);

	// Setup a material
	void SetupMaterialVariables(char *context, int layer, HFMaterial &material);

	// Setup a frequency generator
	void SetupSignalVariables(char *context, char *signalName, SignalParams &signal);

	// Infer the type
	HFType InferType(HFParams &params);

	// What is the full path file?
	void PathFile(const char *path, const char *file, char *pathFile);

	// Make a variable name out of a base and string/number
	char *VariableName(const char *base, char *extra, char *variableName);
	char *VariableName(const char *base, int number, char *variableName);

	// Translate a signal into a type
	SigGen::TYPE GetSignalType(SignalParams &s);
	int GetTextureMethod(const char *method);
	int GetTextureBlend(const char *blend);
	AnimatedHeightField::MAPTYPE GetMapType(const char *mapType);

	// Setup stuff in a created height field
	bool SetupMaterials(AnimatedHeightField &hf, HFParams &params);
	bool SetupGeometrySignals(AnimatedHeightField &hf, HFParams &params);
	bool SetupUVSignals(AnimatedHeightField &hf, HFParams &params);
	bool SetupLOD(AnimatedHeightField &hf, HFParams &params);
	bool SetupPhysics(AnimatedHeightField &hf, HFParams &params);

	// Convert a material into a texturelayerinit
	bool Convert(HFMaterial &material, AnimatedHeightField::TextureLayerInit &init);

	// Register a color, if number >= 0 append number
	inline ts_bRGBA MakeColor(HFColor &c);
	inline ts_bRGBA MakeColor(HFColor &a, HFColor &b, float scale);

	// Build and place a hf around a spline
	bool SplineFit(CurvedAnimatedHeightField &hf, HFParams &params);

	// cast shadows
	void CastShadows(AnimatedHeightField &hf, HFParams &params);

	// Find surfaces for physics
	void FindSurfaces(AnimatedHeightField &hf, HFParams &params);

	// List of actors that can affect the height fields
	struct Actor
	{
		CActor	*actor;
		float		maxSpeed;
		float 	force;

		Actor *prev, *next;
	};
	DoublyLinkedList<Actor>			actorList;

	struct Disturber
	{
		Vector3	*pos;
		Vector3	*velocity;
		float		maxSpeed;
		float		force;

		Disturber *prev, *next;
	};
	DoublyLinkedList<Disturber>	disturberList;

	uint debugState;

protected:

};

// There can be only one!
DefineSingleton(AniHFMgr)
#define g_aniHFMgr GetSingleton(AniHFMgr)

#endif
