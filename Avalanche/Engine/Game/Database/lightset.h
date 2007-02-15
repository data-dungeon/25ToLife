//
// File				-> lightset.h
// Created			-> Feb 26, 2003
// Author			-> Dwain Skinner
// Description		-> adds the ability to load/manage light sets that were exported by the filter chain
//

#ifndef __LIGHTSET__H
#define __LIGHTSET__H

#include "platform/CompilerControl.h"	// for NAMED_SECTION
#include "container/Darray.h"

#define MAX_NUM_LIGHT_INFLUENCES		5
#define MAX_AMBIENT_INFLUENCES		2
#define MAX_POINT_INFLUENCES			3
#define MAX_DIRECTIONAL_INFLUENCES	3
#define MAX_LIGHT_NAME					64

#define ENABLE_BIT								0x00000001
#define VOLUME_SPHERE_ENABLE_BIT				0x00000002
#define VOLUME_CUBE_ENABLE_BIT				0x00000004
#define AFFECTS_ENVIRONMENTAL_OBJECTS_BIT	0x00000010
#define AFFECTS_STATIC_INSTANCES_BIT		0x00000020
#define AFFECTS_TERRAIN_BIT					0x00000040
#define DELETE_LIGHT_BIT						0x80000000
#define DYNAMIC_LIGHT_BIT						0x40000000

// --- define to debug the lights in the scene
//#define DEBUGLIGHTING


#pragma pack( push, savepack )

class CActor;

class CLightRecord
{

#pragma pack(4)	// pack byte-by-byte

public:
	CLightRecord										( ) { memset( this, 0, sizeof( *this ) ); };
	CLightRecord										( u32 type, Vector3& initialPos, Vector4& initialColor );
	~CLightRecord										( ) {};
	

private:
	Vector4Packed 		m_lightColor;
	Vector4Packed		m_origLightColor;
	Vector3Packed		m_lightPosition;				// still want to know where the light was placed in Maya even if it is a directional light
	Vector3Packed		m_lightOrigPosition;
	Vector3Packed		m_lightDirection;				// parallel light source direction
	Vector3Packed		m_lightBonePosition;			// position of a bone we are going to attach to 
	Vector3Packed		m_boneOffset;					// vector to offset the bone position
	u32					m_lightType;
	float					m_lightConstantAttenuation;
	float					m_lightLinearAttenuation;
	float					m_lightQuadraticAttenuation;
	float					m_lightCubicAttenuation;
	float					m_lightIntensity;
	float					m_origLightIntensity;
	float					m_lightIntensityNoiseRange;
	float					m_lightVolumeSphereRadius;
	float					m_lightVolumeCubeWidthHeight;
	u32					m_lightFlags;
	char					m_lightName[ MAX_LIGHT_NAME ];
	CActor				*m_pAttachToActor;
	ts_Bone				*m_pBone;
public:

	enum LightTypes
	{
		LIGHT_OMNI,
		LIGHT_TARGET_SPOT,
		LIGHT_FREE_SPOT,
		LIGHT_TARGET_DIRECT,
		LIGHT_FREE_DIRECT,
		LIGHT_AMBIENT,
		LIGHT_AREA,
		NUM_LIGHT_TYPES,
	};

	inline void					setType							( u32 type )			{ m_lightType = type; }
	inline void					setColor							( Vector4& color )	{ m_lightColor = color; }
	inline void					setOrigColor					( Vector4& color )	{ m_origLightColor = color; }
	inline void					setPosition						( Vector3CRef pos )	{ m_lightPosition = pos; }
	inline void					setOrigPosition				( Vector3CRef pos )	{ m_lightOrigPosition = pos; }
	inline void					setDirection					( Vector3CRef dir )	{ m_lightDirection = dir; }
	inline void					setConstantAttenuation		( float f )				{ m_lightConstantAttenuation = f; }
	inline void					setLinearAttenuation			( float f )				{ m_lightLinearAttenuation = f; }
	inline void					setQuadraticAttenuation		( float f )				{ m_lightQuadraticAttenuation = f; }
	inline void					setCubicAttenuation			( float f )				{ m_lightCubicAttenuation = f; }
	inline void					setAttenuation					( float f )				{ m_lightQuadraticAttenuation = f; }
	inline void					setLightName					( const char *s )		{ ASSERT( strlen( s ) < MAX_LIGHT_NAME ); memset( m_lightName, 0, MAX_LIGHT_NAME ); memcpy( m_lightName, s, strlen( s ) ); }
	inline void					setIntensity					( float intensity )	{ m_lightIntensity = intensity; };
	inline void					setOrigIntensity				( float intensity )	{ m_origLightIntensity = intensity; };
	inline void					setIntensityNoise				( float noise )		{ m_lightIntensityNoiseRange = noise; };
	inline void					setVolumeSphereRadius		( float radius )		{ if( radius > 0.0f ){ m_lightFlags |= VOLUME_SPHERE_ENABLE_BIT; m_lightVolumeSphereRadius = radius; } else{ m_lightFlags &= ( ~VOLUME_SPHERE_ENABLE_BIT ); }	}
	inline void					setVolumeCubeWidthHeight	( float wh )			{ if( wh > 0.0f ){ m_lightFlags |= VOLUME_CUBE_ENABLE_BIT; m_lightVolumeCubeWidthHeight = wh; } else{ m_lightFlags &= ( ~VOLUME_CUBE_ENABLE_BIT ); }	}
	inline void					setLightFlags					( u32 val )				{ m_lightFlags = val; }
	inline u32&					getType							( )						{ return( m_lightType ); }
	inline Vector4Packed&	getColor							( )						{ return( m_lightColor ); }
	inline Vector4Packed&	getOrigColor					( )						{ return( m_origLightColor ); }
	inline Vector3Packed&	getPosition						( )						{ return( m_lightPosition ); }
	inline Vector3Packed&	getOrigPosition				( )						{ return( m_lightOrigPosition ); }
	inline Vector3Packed&	getDirection					( )						{ return( m_lightDirection ); }
	inline float&				getConstantAttenuation		( )						{ return( m_lightConstantAttenuation ); }
	inline float&				getLinearAttenuation			( )						{ return( m_lightLinearAttenuation ); }
	inline float&				getQuadraticAttenuation		( )						{ return( m_lightQuadraticAttenuation ); }
	inline float&				getCubicAttenuation			( )						{ return( m_lightCubicAttenuation ); }
	inline float&				getAttenuation					( )						{ return( m_lightQuadraticAttenuation ); }
	inline char*				getLightName					( )						{ return( m_lightName ); }
	inline float&				getIntensity					( )						{ return( m_lightIntensity );	}
	inline float&				getOrigIntensity				( )						{ return( m_origLightIntensity );	}
	inline float&				getIntensityNoise				( )						{ return( m_lightIntensityNoiseRange ); }
	void							enableLight						( bool val );
	inline bool					isLightEnabled					( )						{ return( ( m_lightFlags & ENABLE_BIT ) == ENABLE_BIT ); }
	inline u32&					getLightFlags					( )						{ return( m_lightFlags ); }
	inline float&				getVolumeSphereRadius		( )						{ return( m_lightVolumeSphereRadius ); }
	inline float&				getVolumeCubeWidthHeight	( )						{ return( m_lightVolumeCubeWidthHeight ); }
	inline bool					isVolumeSphereEnabled		( )						{ return( ( m_lightFlags & VOLUME_SPHERE_ENABLE_BIT ) == VOLUME_SPHERE_ENABLE_BIT ); }
	inline bool					isVolumeCubeEnabled			( )						{ return( ( m_lightFlags & VOLUME_CUBE_ENABLE_BIT ) == VOLUME_CUBE_ENABLE_BIT ); }
	inline void					attachLightToActor			( CActor *pCActor )	{ m_pAttachToActor = pCActor; }
	inline void					detachLightToActor			( )						{ m_pAttachToActor = NULL; }
	inline bool					isAttachedToActor				( )						{ return( m_pAttachToActor != NULL ); }
	inline CActor*				getAttachedActor				( )						{ return( m_pAttachToActor ); }
	inline void					setBonePosition				( Vector3CRef pos )	{ m_lightBonePosition = pos; }
	inline Vector3Packed&	getBonePosition				( )						{ return( m_lightBonePosition ); }
	inline void					setBone							( ts_Bone *pBone )	{ m_pBone = pBone; }
	inline ts_Bone*			getBone							( )						{ return( m_pBone ); }
	inline void					setBonePositionOffset		( Vector3CRef pos )	{ m_boneOffset = pos; }
	inline Vector3Packed&	getBonePositionOffset		( )						{ return( m_boneOffset ); }
	inline const bool			isDynamicLight					( void )					{ return( ( m_lightFlags & DYNAMIC_LIGHT_BIT ) != 0 ); }

};

#define FIND_ALL_LIGHTS					0x00
#define AMBIENT_LIGHTS_SET				0x01
#define POINT_LIGHTS_SET				0x02
#define DIRECTIONAL_LIGHTS_SET		0x04
#define LIGHTS_INITIALIZED				0x08


// --- allow lighting effects without having to add or modify the CLightRecord class, since the filter chain
// --- has to be rebuilt everytime the CLightRecord changes
class CLightEffects
{

public:
	CLightEffects													( CLightRecord *pLightRecord );
	~CLightEffects													( );

	void							updateLightEffect				( void );

	// --- accessor functions
	inline void					enableFlicker					( bool onOff, u32 lifeTime )						{ m_flicker = onOff; m_flickerLifeTime = lifeTime; }
	inline void					enableOverBrighten			( bool onOff, float chance, u32 lifeTime )	{ m_overBrighten = onOff; m_overBrightenChance = chance; m_overBrightenLifeTime = lifeTime; }
	inline void					enableGlowReduction			( bool onOff, u32 lifeTime )						{ m_glowReduction = onOff; m_glowReductionLifeTime = lifeTime; }
	inline void					enableRespawn					( bool onOff, float time ) 						{ m_respawn = onOff; m_respawnDelay = time; }
	inline CLightRecord*		getLightRecord					( )														{ return( m_pLightRecord ); }
	bool							addEffects						( );
	inline void					setEffectsPos					( Vector3CRef pos )									{ m_effectsPos = pos; }

private:
	Vector3Packed													m_effectsPos;
	CLightRecord													*m_pLightRecord;			// --- the light we want the effects on
	int																m_flickerLifeTime;		// --- time remaining to flicker
	u32																m_overBrightenLifeTime;	// --- time to over brighten before it goes out
	u32																m_glowReductionLifeTime;// --- time to reduce intensity to 0
	float																m_overBrightenChance;	// --- probability of light over brightening
	float																m_respawnDelay;			// --- number of seconds before respawning
	float																m_intensity;
	u32																m_gameTime;
	bool																m_flicker;					// --- do we want light flicker
	bool																m_overBrighten;			// --- do we want over brightening
	bool																m_glowReduction;			// --- do we want glow reduction
	bool																m_respawn;					// --- do we want this light to respawn
};


// --- a class to manage all the influences that are affecting an object
class CLightInfluences
{


public:
	CLightInfluences											( );
	~CLightInfluences											( );
																	
	void					getLightInfluences				( Vector3& bodyInWorld, u32 instanceLightBits, bool distCheck );
	void					useLightInfluences				( bool distanceCheck, float attenuationScale, float camDist );
	void					getDynamicInfluences				( Vector3& bodyInWorld, u32 instanceLightBits );
	void					cacheAllLights						( u32 instanceLightBits );
	inline bool			lightsInitialized					( )															{ return( ( m_lightGatheringBits & LIGHTS_INITIALIZED ) != 0 ); };
	void					addLightInfluence					( int maxInfluences, CLightRecord *pLightRecord );
	inline u32			getNumPointInfluences			( )															{ return( m_numPointInfluences ); }
	inline u32			getNumDirectionalInfluences	( )															{ return( m_numDirectionalInfluences ); }
	void					getLightColorAtLocation			( Vector3& location, Vector3& destColor );
#ifndef CONSUMER_BUILD
	void					drawInfluencesRays				( bool distcheck, Vector3& vec, u32 instanceLightBits );
#endif

	Vector3				getShadowLight						( Vector3 &pos );

private:

	struct Influence
	{
		CLightRecord		*pLight;
		CLightRecord		*pLightStatic;
		float					distToLightSquared;
		float					distToLightSquaredStatic;
	};

	u32						m_numInfluences;
	u32						m_numAmbientInfluences;
	Influence				m_ambientInfluences[ MAX_AMBIENT_INFLUENCES ];
								
	u32						m_numPointInfluences;
	Influence				m_pointInfluences[ MAX_POINT_INFLUENCES ];
								
	u32						m_numDirectionalInfluences;
	Influence				m_directionalInfluences[ MAX_DIRECTIONAL_INFLUENCES ];
								
	u32						m_lightGatheringBits;
	u32						m_lastUpdated;
	u32						m_updateCycle;
	u32						m_lastUpdateTime;
};


// --- a class to manage all the lights in the scene.  These are a combination of lights that
// --- are placed in Maya and marked as InGame lights, as well as dynamic lights that are created
// --- by game logic.
class CLightSet
{

public:
	CLightSet													( );
	~CLightSet													( );
	
	void							loadLightSet				( void );

#ifndef CONSUMER_BUILD
	void							drawLightSet				( );
	void							drawLight					( CLightRecord * pLightRecord );
#endif

	inline bool					lightSetIsLoaded			( )										{ return( m_bDataBaseLoaded && m_bLightSetEnabled ); };
	inline u32					getNumLights				( )										{ return( m_numLights ); };
	inline u32					getNumDynamicLights		( )										{ return( m_numDynamicLights ); };
	inline CLightRecord*		getLightRecord				( u32 i )								{ return( m_pMainLightSet[ i ] ); };
	inline CLightRecord*		getDynamicLightRecord	( u32 i )								{ return( m_pDynamicLightSet[ i ] );	};
	inline u32					getNumAmbientLights		( )										{ return( m_numAmbientLights ); };
	inline u32					getNumPointLights			( )										{ return( m_numPointLights ); };
	inline u32					getNumDirectionalLights	( )										{ return( m_numDirectionalLights ); };
	void							addDynamicLight			( CLightRecord *pLightRecord );
	void							removeDynamicLight		( CLightRecord *pLightRecord );
	void							removeDynamicLights		( bool force );
	void							setTestLight				( );
	void							unLoad						( );
	CLightRecord*				findLightByName			( const char *name );
	void							updateLightSet				( );
	void							disableLights				( int from );
	void							attachClosestLight		( CActor *pCActor, ts_Bone *pBone, Vector3 *offset );
	void							getLightColorAtLocation	( Vector3& location, Vector3& destColor );
	void							updateLight					( CLightRecord *pLightRecord );
	inline void					enableLightSet				( bool val )							{ m_bLightSetEnabled = val; }
	inline u32					getNumLightEffectLights	( void )									{ return( m_numLightEffectLights ); }
	inline CLightEffects*	getLightEffectRecord		( u32 i )								{ return( m_pLightEffectsSet[ i ] ); }
	bool							addLightEffects			( CLightEffects *pLightEffects );
	void							removeLightEffects		( CLightEffects *pLightEffectsRecord );
	void							togglePointLights			( );
	Vector3						getSunLightDirection		( ) { return( m_sunDir ); }
	inline ts_fRGB				*getAmbientLight			( ) { return( &m_ambientLight ); }
	inline bool					sunLightExists				( ) { return( m_sunLightExists ); }
	inline bool					getLightColorMult			( float &fLightColorMult )			{ fLightColorMult = m_lightColorMult; return m_bLightColorMultDirty; }
	inline void					setLightColorMult			( float colorMult )					{ m_bLightColorMultDirty = true; m_lightColorMult = colorMult; }
	void							adjustLights				( ts_fRGB color, float lightColorMult );
	void							restoreLights				( void );
	bool							inDynamicLightSet			( CLightRecord *pLight );

private:
	u32									m_numLights;
	u32									m_numDynamicLights;
	u32									m_numAmbientLights;
	u32									m_numDirectionalLights;
	u32									m_numPointLights;
	float									m_lightColorMult;
	Vector3								m_sunDir;
	CLightRecord						**m_pMainLightSet;
	DynamicArray<CLightRecord*>	m_pDynamicLightSet;
	DynamicArray<CLightEffects*>	m_pLightEffectsSet;
	u32									m_numLightEffectLights;
	u32									m_dynamicNumber;
	bool									m_bLightSetEnabled;
	bool									m_bDataBaseLoaded;
	ts_fRGB								m_ambientLight;
	bool									m_sunLightExists;
	bool									m_bLightColorMultDirty;

};



extern CLightSet g_lightSet;

#pragma pack(pop, savepack)
#endif // __LIGHTSET__H

