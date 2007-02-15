//
// File				-> lightset.cpp
// Created			-> Feb 26, 2003
// Author			-> Dwain Skinner
// Description		-> adds the ability to load/manage light sets that were exported by the filter chain
//

#include "Game/GamePCH.h"
#include "EngineHelper/timer.h"

CLightSet g_lightSet;

static ts_RecordList *recList;

static ts_fRGB colorBlack = {0.0f, 0.0f, 0.0f};
static bool pointLightsOn = true;
static u32 currentCycle = 0;
static u32 lastLightUpdateTime = 0;
#define TEST_DYNAMIC
// -----------------------------------------------------------------------------------

CLightRecord::CLightRecord( u32 type, Vector3& initialPos, Vector4& initialColor )
{
	memset( this, 0, sizeof( CLightRecord ) );
	m_lightType			= type;
	m_lightPosition	= initialPos;
	m_lightColor		= initialColor;
	m_origLightColor	= initialColor;
	m_origLightIntensity = 1.0f;
	m_lightIntensity	= 1.0f;
	m_lightDirection.Set( 0.0f, 0.0f, 0.0f );
	m_lightConstantAttenuation = 0.0f;
	m_lightLinearAttenuation = 0.0f;
	m_lightQuadraticAttenuation = 0.5f;
	m_pAttachToActor	= NULL;
	g_lightSet.addDynamicLight( this );
}

// -----------------------------------------------------------------------------------

void CLightRecord::enableLight( bool val )
{
	if( val )
	{ 
		m_lightFlags |= ENABLE_BIT; 
	} 
	else
	{ 
		m_lightFlags &= (~ENABLE_BIT );
	}
	lastLightUpdateTime = g_timer.GetOSTime( );
}

// -----------------------------------------------------------------------------------

CLightInfluences::CLightInfluences( )
{
	memset( this, 0 , sizeof( *this ) );
	m_updateCycle = currentCycle;
	currentCycle++;
	currentCycle &= 0x03;
}

// -----------------------------------------------------------------------------------

CLightInfluences::~CLightInfluences( )
{
}

// -----------------------------------------------------------------------------------

void CLightInfluences::cacheAllLights( u32 instanceLightBits )
{
	m_lightGatheringBits			= FIND_ALL_LIGHTS;
	m_numAmbientInfluences		= 0;
	m_numPointInfluences			= 0;
	m_numDirectionalInfluences	= 0;

	if( !( g_lightSet.lightSetIsLoaded( ) ) ) return;

	m_lightGatheringBits |= LIGHTS_INITIALIZED;

	// --- lets see if we can pre-load all the ambient lights
	if( instanceLightBits & INSTANCE_LIGHTING_AMBIENT )
	{
		if( ( g_lightSet.getNumAmbientLights( ) <= MAX_AMBIENT_INFLUENCES ) )
		{
			for( u32 i = 0; i < g_lightSet.getNumLights( ); i++ )
			{
				CLightRecord *pLightRecord = g_lightSet.getLightRecord( i );
				if( pLightRecord->getType( ) == CLightRecord::LIGHT_AMBIENT )
				{
					if( pLightRecord->isLightEnabled( ) )
					{
						m_ambientInfluences[ m_numAmbientInfluences ].pLight = pLightRecord;
						m_ambientInfluences[ m_numAmbientInfluences++ ].distToLightSquared = 0;
					}
				}
			}
			m_lightGatheringBits |= AMBIENT_LIGHTS_SET;
		}
	}
	else
	{
		m_lightGatheringBits |= AMBIENT_LIGHTS_SET;
	}

	// --- lets see if we can pre-load all the point lights;
	if( instanceLightBits & INSTANCE_LIGHTING_POINT )
	{
		if( ( g_lightSet.getNumPointLights( ) <= MAX_POINT_INFLUENCES ) )
		{
			for( u32 i = 0; i < g_lightSet.getNumLights( ); i++ )
			{
				CLightRecord *pLightRecord = g_lightSet.getLightRecord( i );
				if( pLightRecord->getType( ) == CLightRecord::LIGHT_OMNI )
				{
					if( pLightRecord->isLightEnabled( ) )
					{
						m_pointInfluences[ m_numPointInfluences ].pLight = pLightRecord;
						m_pointInfluences[ m_numPointInfluences++ ].distToLightSquared = 0;
					}
				}
			}
			m_lightGatheringBits |= POINT_LIGHTS_SET;
		}
	}
	else
	{
		m_lightGatheringBits |= POINT_LIGHTS_SET;
	}

	// --- lets see if we can pre-load all the directional lights;
	if( instanceLightBits & INSTANCE_LIGHTING_DIRECTIONAL )
	{
		if( g_lightSet.getNumDirectionalLights( ) <= MAX_DIRECTIONAL_INFLUENCES )
		{
			for( u32 i = 0; i < g_lightSet.getNumLights( ); i++ )
			{
				CLightRecord *pLightRecord = g_lightSet.getLightRecord( i );
				if( pLightRecord->getType( ) == CLightRecord::LIGHT_FREE_DIRECT )
				{
					if( pLightRecord->isLightEnabled( ) )
					{
						m_directionalInfluences[ m_numDirectionalInfluences ].pLight = pLightRecord;
						m_directionalInfluences[ m_numDirectionalInfluences++ ].distToLightSquared = 0;
					}
				}
			}
			m_lightGatheringBits |= DIRECTIONAL_LIGHTS_SET;
		}
	}
	else
	{
		m_lightGatheringBits |= DIRECTIONAL_LIGHTS_SET;
	}
}

// -----------------------------------------------------------------------------------
static Vector3 bodyInWorld;
static u32 instanceLightBits;

void CLightInfluences::getLightInfluences( Vector3& paramBodyInWorld, u32 paramInstanceLightBits, bool distcheck )
{
	if( g_lightSet.lightSetIsLoaded( ) )
	{
		m_lastUpdated++;
		m_lastUpdated &= 0x03;
		if( ( m_lastUpdated != m_updateCycle ) && ( m_lastUpdateTime > lastLightUpdateTime ) ) return;
		m_lastUpdateTime = g_timer.GetOSTime( );

		CLightRecord	*pLightRecord;
		CLightRecord	*pAmbientLight = NULL;
		u32 i;
		
		bodyInWorld			= paramBodyInWorld;
		instanceLightBits	= paramInstanceLightBits;
		
		// --- if all the ambient lights are pre-loaded, we still must update the distances to them so we can cross blend
		if( m_lightGatheringBits & AMBIENT_LIGHTS_SET )
		{
			for( i = 0; i < m_numAmbientInfluences; i++ )
			{
				m_ambientInfluences[ i ].distToLightSquared = ( m_ambientInfluences[ i ].pLight->getPosition( ) - bodyInWorld ).LengthSquared( );
			}
		}


		// --- if dynamic lights were created on the fly we may need to remove the cached bit
		if( m_lightGatheringBits & POINT_LIGHTS_SET )
		{
			if( g_lightSet.getNumDynamicLights( ) )
			{
				m_lightGatheringBits &= ( ~POINT_LIGHTS_SET );
			}
		}
		else
		{
			if( ( g_lightSet.getNumDynamicLights( ) == 0 ) && ( g_lightSet.getNumPointLights( ) < MAX_POINT_INFLUENCES ) )
			{
				cacheAllLights( paramInstanceLightBits );
			}
		}

		// --- see if we need to do anything
		if( m_lightGatheringBits == ( AMBIENT_LIGHTS_SET | POINT_LIGHTS_SET | DIRECTIONAL_LIGHTS_SET | LIGHTS_INITIALIZED  ) ) return;

		// --- reset data
		if( !( m_lightGatheringBits & AMBIENT_LIGHTS_SET ) )
		{
			for( i = 0; i < MAX_AMBIENT_INFLUENCES; i++ )
			{
				m_ambientInfluences[ i ].distToLightSquared = MAXFLOAT;
			}
			m_numAmbientInfluences		= 0;
		}

		// --- reset data
		if( !( m_lightGatheringBits & POINT_LIGHTS_SET ) || g_lightSet.getNumDynamicLights( ) != 0 )
		{
			for( i = 0; i < MAX_POINT_INFLUENCES; i++ )
			{
				m_pointInfluences[ i ].distToLightSquared = MAXFLOAT;
				m_pointInfluences[ i ].pLight = NULL;
			}
			m_numPointInfluences			= 0;
		}

		// --- reset data
		if( !( m_lightGatheringBits & DIRECTIONAL_LIGHTS_SET ) )
		{
			for( i = 0; i < MAX_DIRECTIONAL_INFLUENCES; i++ )
			{
				m_directionalInfluences[ i ].distToLightSquared = MAXFLOAT;
			}
			m_numDirectionalInfluences	= 0;
		}

		int maxInfluencesToUse = ( distcheck ? MAX_POINT_INFLUENCES - 1 : MAX_POINT_INFLUENCES );

		// --- scan all the static lights
		for( i = 0; i < g_lightSet.getNumLights( ); i++ )
		{
			pLightRecord = g_lightSet.getLightRecord( i );
			if( pLightRecord->isLightEnabled( ) )
			{
				addLightInfluence( MAX_POINT_INFLUENCES, pLightRecord );
			}

		}
		// --- save the static lights, we going to use these for shadow cast direction
		m_pointInfluences[ 0 ].distToLightSquaredStatic = m_pointInfluences[ 0 ].distToLightSquared;
		m_pointInfluences[ 1 ].distToLightSquaredStatic = m_pointInfluences[ 1 ].distToLightSquared;
		m_pointInfluences[ 2 ].distToLightSquaredStatic = m_pointInfluences[ 2 ].distToLightSquared;
		m_pointInfluences[ 0 ].pLightStatic = m_pointInfluences[ 0 ].pLight;
		m_pointInfluences[ 1 ].pLightStatic = m_pointInfluences[ 1 ].pLight;
		m_pointInfluences[ 2 ].pLightStatic = m_pointInfluences[ 2 ].pLight;

		// --- we're only going to allow 2 influences for static lights placed, but for shadowing we want to gather the 3 closest lights
		if( m_numPointInfluences == MAX_POINT_INFLUENCES ) m_numPointInfluences--;

		// --- scan all the dynamic lights
		for( i = 0; i < g_lightSet.getNumDynamicLights( ); i++ )
		{
			pLightRecord = g_lightSet.getDynamicLightRecord( i );
			if( pLightRecord->isLightEnabled( ) )
			{
				addLightInfluence( maxInfluencesToUse, pLightRecord );
			}
		}
	}
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
void CLightInfluences::addLightInfluence( int maxInfluences, CLightRecord *pLightRecord )
{
	float				dist;

	dist = ( pLightRecord->getPosition( ) - bodyInWorld ).LengthSquared( );
	switch( pLightRecord->getType( ) )
	{
		case CLightRecord::LIGHT_OMNI:
		{
			if( !pointLightsOn ) break;

			// --- see if we need to do any work
			if( !( instanceLightBits & INSTANCE_LIGHTING_POINT ) || ( m_lightGatheringBits & POINT_LIGHTS_SET ) )
				 break;

			int min = Math::Min( maxInfluences, (int)m_numPointInfluences + 1 );

			// --- we want to record the MAX_POINT_INFLUENCES we found so we can blend between them all
			for( int i = 0; i < min; i++ )
			{
				if( m_pointInfluences[ i ].distToLightSquared >= dist )
				{ 
					if( i < ( maxInfluences - 1 ) )
					{
						for( int j = maxInfluences - 1; j > i; j-- )
						{
							m_pointInfluences[ j ] = m_pointInfluences[ j - 1 ];
						}
					}
					m_pointInfluences[ i ].pLight = pLightRecord;
					m_pointInfluences[ i ].distToLightSquared = dist;
					m_numPointInfluences = Math::Min( (int)m_numPointInfluences + 1, maxInfluences );
					break;
				}
			}
		}
		break;


		case CLightRecord::LIGHT_FREE_DIRECT:
		{
			// --- see if we need to do any work
			if( !( instanceLightBits & INSTANCE_LIGHTING_DIRECTIONAL ) || ( m_lightGatheringBits & DIRECTIONAL_LIGHTS_SET ) ) break;

			u32 min = Math::Min( MAX_DIRECTIONAL_INFLUENCES, (int)m_numDirectionalInfluences + 1 );

			// --- we want to record the MAX_DIRECTIONAL_INFLUENCES we found so we can blend between them all
			for( u32 i = 0; i < min; i++ )
			{
				if( m_directionalInfluences[ i ].distToLightSquared >= dist )
				{ 
					if( i < ( MAX_DIRECTIONAL_INFLUENCES - 1 ) )
					{
						for( u32 j = i + 1; j > 0; j-- )
						{
							m_directionalInfluences[ j ] = m_directionalInfluences[ j - 1 ];
						}
					}
					m_directionalInfluences[ i ].pLight = pLightRecord;
					m_directionalInfluences[ i ].distToLightSquared = dist;
					m_numDirectionalInfluences = Math::Min( ( int )m_numDirectionalInfluences + 1, MAX_DIRECTIONAL_INFLUENCES );
					break;
				}
			}
		}
		break;

		case CLightRecord::LIGHT_AMBIENT:
		{
			// --- see if we need to do any work
			if( !( instanceLightBits & INSTANCE_LIGHTING_AMBIENT ) || ( m_lightGatheringBits & AMBIENT_LIGHTS_SET ) ) break;

			u32 min = Math::Min( MAX_AMBIENT_INFLUENCES, (int)m_numAmbientInfluences + 1 );

			// --- we want to record the MAX_AMBIENT_INFLUENCES we found so we can blend between them all
			for( u32 i = 0; i < min; i++ )
			{
				if( m_ambientInfluences[ i ].distToLightSquared >= dist )
				{ 
					if( i < ( MAX_AMBIENT_INFLUENCES - 1 ) )
					{
						for( u32 j = i + 1; j > 0; j-- )
						{
							m_ambientInfluences[ j ] = m_ambientInfluences[ j - 1 ];
						}
					}
					m_ambientInfluences[ i ].pLight = pLightRecord;
					m_ambientInfluences[ i ].distToLightSquared = dist;
					m_numAmbientInfluences = Math::Min( (int)m_numAmbientInfluences + 1, (int)MAX_AMBIENT_INFLUENCES );
					break;
				}
			}
		}
		break;

		default:
			ASSERTS( 0, "Did not find light type" );
	}
}
// -----------------------------------------------------------------------------------

void CLightInfluences::getDynamicInfluences( Vector3& vec, u32 instanceLightBits )
{
	return;
	for( u32 i = 0; i < g_lightSet.getNumDynamicLights(); i++ )
	{
		CLightRecord *pLightRecord = g_lightSet.getDynamicLightRecord( i );

		if( pLightRecord->isLightEnabled( ) )
		{
			switch( pLightRecord->getType( ) )
			{
				case CLightRecord::LIGHT_OMNI:
					if( !pointLightsOn ) break;
					if( m_numPointInfluences < MAX_POINT_INFLUENCES )
					{
						m_pointInfluences[ m_numPointInfluences ].pLight = pLightRecord;
						m_pointInfluences[ m_numPointInfluences ].distToLightSquared = ( pLightRecord->getPosition( ) - vec ).LengthSquared( );
						m_numPointInfluences++;
					}
					else
					{
						m_pointInfluences[ m_numPointInfluences - 1 ].pLight = pLightRecord;
						m_pointInfluences[ m_numPointInfluences - 1 ].distToLightSquared = ( pLightRecord->getPosition( ) - vec ).LengthSquared( );
					}
					break;

				case CLightRecord::LIGHT_FREE_DIRECT:
					break;

				case CLightRecord::LIGHT_AMBIENT:
					m_ambientInfluences[ m_numAmbientInfluences ].pLight = pLightRecord;
					m_ambientInfluences[ m_numAmbientInfluences ].distToLightSquared = ( pLightRecord->getPosition( ) - vec ).LengthSquared( );
					m_numAmbientInfluences++;

				default:
					ASSERTF( false, ( "unsupported light type on light->%s", pLightRecord->getLightFlags() ) );
					break;
			}
		}
	}
}

// -----------------------------------------------------------------------------------
//static float testDist = 3.2f;
//static float testDist = 9.0f;
static float testDist = 9.0f;

void CLightInfluences::useLightInfluences( bool distanceCheck, float attenuationScale, float camDist )
{
	u32 numLights = 0;
	u32 i;
	
	if( g_lightSet.getNumLights( ) || g_lightSet.getNumDynamicLights( ) )
	{
		// --- handle ambient lights
		if( m_numAmbientInfluences )
		{
			if( m_numAmbientInfluences >= 2 )
			{
				Vector4	color;
				float		totalDist = 0;
				float		t1, t2;

				// --- only supports 2 for now
				ASSERT( m_numAmbientInfluences < 3 );

				totalDist = m_ambientInfluences[ 0 ].distToLightSquared + m_ambientInfluences[ 1 ].distToLightSquared;
				t1 = m_ambientInfluences[ 0 ].distToLightSquared / totalDist;
				t2 = m_ambientInfluences[ 1 ].distToLightSquared / totalDist;
				color = ( m_ambientInfluences[ 0 ].pLight->getColor( ) * t2 ) + ( m_ambientInfluences[ 1 ].pLight->getColor( ) * t1 );
				RenderState__RegisterHardwareAmbientLight( *( ts_fRGB *) &color );
			}
			else
			{
				RenderState__RegisterHardwareAmbientLight( *( ts_fRGB *)&( m_ambientInfluences[ 0 ].pLight->getColor( ) ) );
			}
		}
		else
		{
			// --- we did not have any ambient light
			RenderState__RegisterHardwareAmbientLight( colorBlack );
		}

		// --- handle the point lights
		ASSERT( m_numPointInfluences <= MAX_POINT_INFLUENCES );
		for( i = 0; i < m_numPointInfluences; i++ )
		{
			// --- if we want to do a distance check from the light to the object to see if it even gets lit
			if( distanceCheck )
			{
				// --- how close is the camera to this object? < 32 meters?
				if( camDist < 32.0f )
				{
					float atten = m_pointInfluences[ i ].pLight->getQuadraticAttenuation( );
					atten *= atten;
					float dist = atten * m_pointInfluences[ i ].distToLightSquared;
					if( dist < testDist )
					{
						RenderState__RegisterLightSetHardwareLight( numLights, m_pointInfluences[ i ].pLight, attenuationScale );
						numLights++;
					}
				}
			}
			else
			{
				RenderState__RegisterLightSetHardwareLight( numLights, m_pointInfluences[ i ].pLight, attenuationScale );
				numLights++;
			}
		}

		// --- handle the directional lights
		ASSERT( m_numDirectionalInfluences <= MAX_DIRECTIONAL_INFLUENCES );
		for( i = 0; i < m_numDirectionalInfluences; i++ )
		{
			RenderState__RegisterLightSetHardwareLight( numLights, m_directionalInfluences[ i ].pLight, attenuationScale );
			numLights++;
		}

		g_lightSet.disableLights( numLights );
	}
}

// -----------------------------------------------------------------------------------

void CLightInfluences::getLightColorAtLocation( Vector3& location, Vector3& destColor )
{
	Vector3 Normal;

	// --- set normal to point straight up in the world
	Normal.Set( 0.0f, 1.0f, 0.0f );

	// --- go get all the point light influences
	for( u32 i = 0; i < m_numPointInfluences; i++ )
	{
		// --- get the light
		CLightRecord *pLightRecord = m_pointInfluences[ i ].pLight;

		// --- get its attenuation
		float attenuation = pLightRecord->getQuadraticAttenuation( );

		// --- compute the vector from the point to the light
		Vector3 lightVec = pLightRecord->getPosition( ) - location;

		// --- multiply the attenuation by the length cubed, cubed because we will normalize the light at the same time
		// --- we attenuate it by the length squared
		float dist = lightVec.Length( );
		attenuation *= ( dist * dist * dist );

		// --- clamp it
		if( attenuation < 0.00001f ) attenuation = 0.00001f;

		float intensity = lightVec.Dot( Normal );
		intensity = Math::Clamp( intensity, 0.0f, 10000.0f );
		destColor += ( ( ( Vector3 )( intensity * pLightRecord->getColor( ) ) ) / attenuation );
	}

	destColor.Set( Math::Clamp( destColor.X( ), 0.0f, 2.0f ), Math::Clamp( destColor.Y( ), 0.0f, 2.0f ), Math::Clamp( destColor.Z( ), 0.0f, 2.0f ) );
}

// -----------------------------------------------------------------------------------

CLightSet::CLightSet( ) : m_pDynamicLightSet( 48, 48 ), m_pLightEffectsSet( 48, 48 )
{
	m_pDynamicLightSet.Lock( ); // don't allow growth, this is a global
	m_pLightEffectsSet.Lock( ); // don't allow growth, this is a global

	m_numLights = m_numAmbientLights = m_numPointLights = m_numDirectionalLights = m_numDynamicLights = 0; 
	m_bDataBaseLoaded=false; 
	m_pMainLightSet = NULL; 
	m_dynamicNumber = 0;
	m_bLightSetEnabled = false;
	m_sunLightExists = true;
	m_bLightColorMultDirty = false;
	m_lightColorMult = 1.0f;
	m_sunDir.Set( 0.0f, 1.0f, 1.0f );
}

// -----------------------------------------------------------------------------------

CLightSet::~CLightSet( )
{
	unLoad( );
}

// -----------------------------------------------------------------------------------

void CLightSet::unLoad( )
{
	// --- make sure we clean up just in case there were still lights marked to be deleted
	removeDynamicLights( true );
	ASSERTS( m_numDynamicLights == 0, "There cannot be any dynamic lights left after unload" );

	// --- remove the light effects
	for ( int i = m_pLightEffectsSet.Count() - 1 ; i >= 0 ; i-- )
	{
		if ( m_pLightEffectsSet[ i ] )
		{
			delete m_pLightEffectsSet[ i ];
			m_pLightEffectsSet.Delete( i );
			m_numLightEffectLights--;
		}
	}

	m_numLights = m_numAmbientLights = m_numPointLights = m_numDirectionalLights = m_numDynamicLights = m_numLightEffectLights = 0; 
	m_bDataBaseLoaded=false; 
	m_pMainLightSet = NULL; 
	m_bLightSetEnabled = false;
	m_bLightColorMultDirty = false;
	m_lightColorMult = 1.0f;
}

// -----------------------------------------------------------------------------------

void CLightSet::loadLightSet( void )
{
	u32 numRecords = 0;
	
	m_numLights = 0;

	// --- lets load the light set
	if( !m_bDataBaseLoaded )
	{
		float maxColor = 0.0f;
		float curColor;
		m_ambientLight.R = 0.0f; m_ambientLight.G = 0.0f; m_ambientLight.B = 0.0f;

		// --- go get the light set
		ts_RecordList *pLightSetList = ( ts_RecordList * ) dblGetChunkData( DB_LIGHTS, DEFAULT_CHUNK_KEY_STRING, g_loadSOM.GetWorldHandle( ) );

		// --- we've got one
		if( pLightSetList )
		{
			// --- table of CLightRecord pointers
			m_pMainLightSet	= ( CLightRecord ** )&pLightSetList->pRecordTable;

			// --- number of lights in the chunk
			m_numLights			= pLightSetList->usRecordCount;

			// --- allow access
			m_bDataBaseLoaded	= true;
			m_bLightSetEnabled = true;

			for( u32 i = 0; i < m_numLights; i++ )
			{
				CLightRecord	*pLightRecord = m_pMainLightSet[ i ];

				ASSERT( pLightRecord );
				
				switch( pLightRecord->getType( ) )
				{
					case CLightRecord::LIGHT_OMNI:
						pLightRecord->enableLight( true );
						pLightRecord->setColor( ( Vector4 & )pLightRecord->getOrigColor() );
						pLightRecord->setIntensity( pLightRecord->getOrigIntensity( ) );

						#ifdef PS2
						pLightRecord->getColor( ) *= 2.0f;
						#endif
						pLightRecord->getColor( ) *= pLightRecord->getIntensity( );
						pLightRecord->getColor( )._w = 0.0f;
						m_numPointLights++;
						break;

					case CLightRecord::LIGHT_FREE_DIRECT:
						pLightRecord->enableLight( true );
						pLightRecord->getDirection( ).Normalize( );
						pLightRecord->setColor( ( Vector4 & )pLightRecord->getOrigColor( ) );
						pLightRecord->setIntensity( pLightRecord->getOrigIntensity( ) );

						//--- ala LightMgr double the intensity of the light for PS2
						#ifdef PS2
						pLightRecord->getColor( ) *= 2.0f;
						#endif
						pLightRecord->getColor( ) *= pLightRecord->getIntensity( );
						m_numDirectionalLights++;
						curColor = pLightRecord->getColor( ).LengthSquared( );
						if( curColor > maxColor )
						{
							maxColor = curColor;
							m_sunDir = pLightRecord->getDirection( );
						}
						pLightRecord->getColor( )._w = 0.0f;
						break;
					
					case CLightRecord::LIGHT_AMBIENT:
						pLightRecord->enableLight( true );
						pLightRecord->setColor( ( Vector4 & )pLightRecord->getOrigColor( ) );
						pLightRecord->setIntensity( pLightRecord->getOrigIntensity( ) );

						#ifdef PS2
						pLightRecord->getColor( ) *= 2.0f;
						#endif
						pLightRecord->getColor( ) *= pLightRecord->getIntensity( );
						m_ambientLight.R = pLightRecord->getColor().X();
						m_ambientLight.G = pLightRecord->getColor().Y();
						m_ambientLight.B = pLightRecord->getColor().Z();
						pLightRecord->getColor( )._w = 1.0f;
						m_numAmbientLights++;
						break;

					default:
						ASSERTF( false, ( "unsupported light type on light->%s", pLightRecord->getLightFlags() ) );
						break;
				}
			}
		}
		lastLightUpdateTime = g_timer.GetOSTime( );
	}
}

#ifndef CONSUMER_BUILD
#ifdef DEBUGLIGHTING
static bool drawRays = true;
static bool drawDirRays = true;
#else
static bool drawRays = false;
static bool drawDirRays = false;
#endif

// -----------------------------------------------------------------------------------
void CLightInfluences::drawInfluencesRays( bool distcheck, Vector3 &vec, u32 instanceLightBits )
{
	if( !drawRays ) return;
	for( u32 i = 0; i < m_numPointInfluences; i++ )
	{
		CLightRecord *pLightRecord = m_pointInfluences[ i ].pLight;
		Vector3 pos = pLightRecord->getPosition( );
		Vector4 color = pLightRecord->getColor( );
		ts_fRGB fColor;

		// --- if we want to do a distance check from the light to the object to see if it even gets lit
		float atten = m_pointInfluences[ i ].pLight->getQuadraticAttenuation( );
		atten *= atten;
		float dist = atten * m_pointInfluences[ i ].distToLightSquared;
		if( !distcheck || dist < testDist )
		{
			*( ( ts_fRGB * )&fColor ) = *( ( ts_fRGB * )&color );
			DrawUtility::Line( vec, pos, fColor );
		}
	}

	if( !drawDirRays ) return;
	for( u32 i = 0; i < m_numDirectionalInfluences; i++ )
	{
		CLightRecord *pLightRecord = m_directionalInfluences[ i ].pLight;
		Vector3 pos = pLightRecord->getPosition( );
		Vector4 color = pLightRecord->getColor( );
		ts_fRGB fColor;

		*( ( ts_fRGB * )&fColor ) = *( ( ts_fRGB * )&color );
		DrawUtility::Line( vec, pos, fColor );
	}
}
#endif //CONSUMER_BUILD
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
Vector3 CLightInfluences::getShadowLight( Vector3 &pos )
{
	bool foundLights = false;
	Vector3 m_shadowLightDir;
	m_shadowLightDir.Set( 0.0f, 0.0f, 0.0f );

	if( g_lightSet.getNumLights( ) || g_lightSet.getNumDynamicLights( ) )
	{
		for( int i = 0; i < MAX_POINT_INFLUENCES; i++ )
		{
			CLightRecord *pLightRecord = m_pointInfluences[ i ].pLightStatic;
			if( pLightRecord && !( pLightRecord->getLightFlags() & DYNAMIC_LIGHT_BIT ) )
			{
				Vector3 dir = pLightRecord->getPosition( ) - pos;
				if( dir == Vector3( 0.0f, 0.0f, 0.0f ) )
				{
					dir.Set( 0.0f, 0.1f, 0.0f );
				}
				dir.SafeNormalize( );
				float dist = m_pointInfluences[ i ].distToLightSquaredStatic;
				if( dist < 0.0001f ) dist = 0.0001f;
				dir *= ( 10.0f / dist );
				m_shadowLightDir += dir;
				foundLights = true;
			}
		}
		if( foundLights )
		{
			m_shadowLightDir.SafeNormalize( );
			return( m_shadowLightDir );
		}
		else
		{
			return( g_lightSet.getSunLightDirection( ) );
		}
	}
	return( Vector3( 0.0, 0.707f, 0.707f ) );
}
// -----------------------------------------------------------------------------------


#ifndef CONSUMER_BUILD
// -----------------------------------------------------------------------------------
// Debug code to draw all the lights in the world
//
void CLightSet::drawLightSet( )
{
	CLightRecord	*pLightRecord;

	if( lightSetIsLoaded( ) )
	{
		for( u32 i = 0; i < getNumLights( ); i++ )
		{
			pLightRecord = m_pMainLightSet[ i ];
			drawLight( pLightRecord );
		}

	}

	// --- any dynamic lights that have been created
	for( u32 i = 0; i < getNumDynamicLights( ); i++ )
	{
		pLightRecord = m_pDynamicLightSet[ i ];
		drawLight( pLightRecord );
	}
}
#endif //CONSUMER_BUILD
// -----------------------------------------------------------------------------------

#ifndef CONSUMER_BUILD
void CLightSet::drawLight( CLightRecord *pLightRecord )
{
	u32				type;
	Vector3			pos;
	Vector3			dir;
	Vector3			arrow;
	Vector4			color;

	type = pLightRecord->getType( );
	pos = pLightRecord->getPosition( );
	dir = pLightRecord->getDirection( );
	color = pLightRecord->getColor( );

	// --- directional
	if( type == CLightRecord::LIGHT_FREE_DIRECT )
	{
		arrow = -dir;
		DrawUtility::Arrow( pos, arrow, DrawUtility::WHITE, 0.10f );
	}

	// --- ambient
	if( type == CLightRecord::LIGHT_AMBIENT )
	{
		Vector3 a, b;
		ts_fRGB fColor;

		*( ( ts_fRGB * )&fColor ) = *( ( ts_fRGB * )&color );

		a = b = pos;
		a.x(a.x() + 0.2f); b.x(b.x() - 0.2f);
		DrawUtility::Line( a, b, fColor );

		a = b = pos;
		a.y(a.y() + 0.2f); b.y(b.y() - 0.2f);
		DrawUtility::Line( a, b, fColor );

		a = b = pos;
		a.z(a.z() + 0.2f); b.z(b.z() - 0.2f);
		DrawUtility::Line( a, b, fColor );

	}

#ifdef PS2
	color.X( color.X( ) / 2.0f );
	color.Y( color.Y( ) / 2.0f );
	color.Z( color.Z( ) / 2.0f );
#endif
	DrawUtility::Sphere(pos, ( ts_fRGB * )&color, 0.05f );
}
#endif //CONSUMER_BUILD
// -----------------------------------------------------------------------------------

void CLightSet::addDynamicLight( CLightRecord * pLightRecord )
{
	char buf[ 32 ];

	sprintf( buf, "dynamic Light #%ld", m_dynamicNumber );

	pLightRecord->setLightName( buf );
	pLightRecord->setColor( ( Vector4 & )( pLightRecord->getColor() * pLightRecord->getIntensity() ) );
	pLightRecord->enableLight( false );
	pLightRecord->setLightFlags( pLightRecord->getLightFlags( ) | DYNAMIC_LIGHT_BIT );
	m_pDynamicLightSet.Add( pLightRecord, m_numDynamicLights );
	m_pDynamicLightSet[ m_numDynamicLights ] = pLightRecord;
	m_numDynamicLights++;

	// --- force everyone to gather new influences next cycle
	lastLightUpdateTime = g_timer.GetOSTime( );
	m_dynamicNumber++;
}

// -----------------------------------------------------------------------------------

bool CLightSet::addLightEffects( CLightEffects *pLightEffectsRecord )
{
	if ( pLightEffectsRecord != NULL )
	{
		for ( int i = m_pLightEffectsSet.Count() - 1 ; i >= 0 ; i-- )
		{
			if ( ( m_pLightEffectsSet[ i ] )->getLightRecord( ) == pLightEffectsRecord->getLightRecord( ) )
			{
				return( false );
			}
		}
	}

	m_pLightEffectsSet.Add( pLightEffectsRecord, m_numLightEffectLights );
	m_pLightEffectsSet[ m_numLightEffectLights ] = pLightEffectsRecord;
	m_numLightEffectLights++;
	return( true );
}

// -----------------------------------------------------------------------------------

bool CLightSet::inDynamicLightSet( CLightRecord *pLight )
{
	for( int i = m_pDynamicLightSet.Count() - 1 ; i >= 0 ; i-- )
	{
		CLightRecord *pLightRecord = m_pDynamicLightSet[ i ];
		if( pLightRecord && pLightRecord == pLight )
			return( true );
	}
	return( false );
}

// -----------------------------------------------------------------------------------

void CLightSet::removeDynamicLight( CLightRecord *pLightRecord )
{
	ASSERT( pLightRecord );
	if( inDynamicLightSet( pLightRecord ) )
		pLightRecord->setLightFlags( pLightRecord->getLightFlags( ) | DELETE_LIGHT_BIT );
}


// -----------------------------------------------------------------------------------

void CLightSet::removeDynamicLights( bool force )
{
	for( int i = m_pDynamicLightSet.Count() - 1 ; i >= 0 ; i-- )
	{
		CLightRecord *pLightRecord = m_pDynamicLightSet[ i ];
		if ( pLightRecord && ( ( pLightRecord->getLightFlags( ) & DELETE_LIGHT_BIT ) || force ) )
		{
			lastLightUpdateTime = g_timer.GetOSTime( );
			delete pLightRecord;
			m_pDynamicLightSet.Delete( i );
			m_numDynamicLights--;
		}
	}
}

void CLightSet::removeLightEffects( CLightEffects *pLightEffectsRecord )
{
	if ( pLightEffectsRecord != NULL )
	{
		for ( int i = m_pLightEffectsSet.Count() - 1 ; i >= 0 ; i-- )
		{
			if ( m_pLightEffectsSet[ i ] == pLightEffectsRecord )
			{
				m_pLightEffectsSet.Delete( i );
				m_numLightEffectLights--;
			}
		}
	}
}

// -----------------------------------------------------------------------------------

CLightRecord *CLightSet::findLightByName( const char *name )
{
	for( u32 i = 0; i < getNumLights( ); i++ )
	{
		CLightRecord *pLightRecord;

		pLightRecord = m_pMainLightSet[ i ];

		ASSERT( name );
		ASSERT( pLightRecord );

		if( pLightRecord && name )
		{
			if( strcmp( pLightRecord->getLightName( ), name ) == 0 ) 
			{
				return( pLightRecord );
			}
		}
	}

	// --- we did'nt find the light that we were asked for
	ASSERTF( FALSE && "Can't findLightByName", ("%s", name) );
	return NULL;
}	
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
void CLightSet::attachClosestLight( CActor *pCActor, ts_Bone *pBone, Vector3 *offset )
{
	Vector3			actorPos = pCActor->GetBodyInWorld( );
	float				distMin = MAXFLOAT;
	float				dist;
	CLightRecord	*pClosestLight = NULL;

	if( lightSetIsLoaded( ) )
	{
		for( u32 i = 0; i < getNumLights( ); i++ )
		{
			CLightRecord *pLightRecord;

			pLightRecord = m_pMainLightSet[ i ];
			ASSERT( pLightRecord );

			dist = ( pLightRecord->getPosition( ) - actorPos ).LengthSquared( );
			if( dist < distMin )
			{
				distMin = dist;
				pClosestLight = pLightRecord;
			}
		}

		if( pClosestLight )
		{
			Vector3 pos = pCActor->GetBoneWorldStartPosition( pBone->boneID );
			Graphics4x4 mWorldToBone;
	
			mWorldToBone.FastMultiply( pBone->Matrix, pCActor->Instance()->Matrix() );	// start with bone-to-world
			
			bool bInvertSuccess = mWorldToBone.Invert();	// and invert
			ASSERT(bInvertSuccess);
			pClosestLight->setBone( pBone );
			pClosestLight->setBonePosition( pos * mWorldToBone );
			pClosestLight->setBonePositionOffset( *offset );
			pClosestLight->attachLightToActor( pCActor );
		}
	}
}
// -----------------------------------------------------------------------------------


static float curTime=0.0f;
// -----------------------------------------------------------------------------------
void CLightSet::updateLightSet( )
{
	if( lightSetIsLoaded( ) )
	{
		u32 i;

//		curTime += ( 0.1f );
		curTime += ( g_timer.GetOSFrameSec( ) );

		// --- update static lights declared through Maya
		for( i = 0; i < getNumLights( ); i++ )
		{
			CLightRecord *pLightRecord = m_pMainLightSet[ i ];
			if( pLightRecord->getType( ) == CLightRecord::LIGHT_OMNI )
				updateLight( pLightRecord );
		}

		// --- update dynamic lights
		removeDynamicLights( false );
		for( i = 0; i < getNumDynamicLights( ); i++ )
		{
			CLightRecord *pLightRecord = m_pDynamicLightSet[ i ];
			if( pLightRecord->getType( ) == CLightRecord::LIGHT_OMNI )
				updateLight( pLightRecord );
		}

		// --- update special lighting effects last
		for( i = 0; i < getNumLightEffectLights( ); i++ )
		{
			CLightEffects *pLightEffectsRecord = g_lightSet.getLightEffectRecord( i );
			ASSERTS( pLightEffectsRecord, "Better not be NULL" );
			pLightEffectsRecord->updateLightEffect( );
		}
	}
	else
	{
		ASSERTS( false, "You must create a light set for you level, otherwise lighting is undefined" );
	}
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
void CLightSet::updateLight( CLightRecord *pLightRecord )
{
	if( g_timer.IsPaused() ) return;
	
	if( pLightRecord->isLightEnabled( ) )
	{
		float noise = pLightRecord->getIntensityNoise( );
		float intensity = g_random.InRange( 1.0f - noise, 1.0f );
#if 0
		intensity = 1.0f;
//		pLightRecord->setIntensity( intensity );

		noise = .2f;
		if( noise > 0.0f )
		{
			intensity = Math::Sin( 8.0f * curTime );
			intensity += 1.0f;
			intensity *= 0.5f;
			( 1.0f - intensity ) * noise;
			//intensity = Math::Clamp( intensity, 1.0f - noise, 1.0f );
		}
#endif
		pLightRecord->setIntensity( intensity );

		// --- see if light is attached to something
		if( pLightRecord->isAttachedToActor( ) )
		{
			CActor *pCActor = ( CActor * )pLightRecord->getAttachedActor( );

			if ( pLightRecord->getBone( ) != NULL )
			{
				Graphics4x4 mBoneToWorld;
				mBoneToWorld.FastMultiply( pLightRecord->getBone( )->Matrix, pLightRecord->getAttachedActor( )->Instance()->Matrix() );
				pLightRecord->setPosition( ( pLightRecord->getBonePosition( ) * mBoneToWorld ) + pLightRecord->getBonePositionOffset( ) );
			}
			else
			{
				pLightRecord->setPosition( ( pLightRecord->getBonePosition() * pCActor->GetBodyToWorld() ) + pCActor->GetBodyInWorld() + pLightRecord->getBonePositionOffset() );
			}
		}

		// --- move light randomly
		if( pLightRecord->isVolumeSphereEnabled( ) )
		{
			Vector3 rPos;

			rPos.Set(g_random.NegPos(), g_random.NegPos(), g_random.NegPos());
			rPos *= pLightRecord->getVolumeSphereRadius( );

			pLightRecord->getPosition( ) = pLightRecord->getOrigPosition( ) + rPos;
		}
		else if( pLightRecord->isVolumeCubeEnabled( ) )
		{
			Vector3 rPos;

			rPos.Set(g_random.NegPos(), g_random.NegPos(), g_random.NegPos());
			rPos *= pLightRecord->getVolumeCubeWidthHeight( ) * 0.5f;

			pLightRecord->getPosition( ) = pLightRecord->getOrigPosition( ) + rPos;
		}
	}
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
void CLightSet::disableLights( int from )
{
	if( ( g_lightSet.getNumLights( ) + g_lightSet.getNumDynamicLights( ) ) > 0  )
	{
		RenderState__DisableHardwareLightRange( from );
	}
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
void CLightSet::getLightColorAtLocation( Vector3& location, Vector3& destColor )
{
	CLightInfluences workInfluences;

	destColor.Set( 0.0f, 0.0f, 0.0f );
	workInfluences.getLightInfluences( location, INSTANCE_LIGHTING_POINT, false );
	workInfluences.getLightColorAtLocation( location, destColor );
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
void CLightSet::togglePointLights( )
{
	pointLightsOn ^= true;
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
void CLightSet::adjustLights( ts_fRGB color, float lightColorMult )
{
	RenderState__RegisterHardwareAmbientLight( color );
	m_lightColorMult = lightColorMult;
	m_bLightColorMultDirty = true;
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
void CLightSet::restoreLights( void )
{
	RenderState__RegisterHardwareAmbientLight( m_ambientLight );
	m_lightColorMult = 1.0f;
	m_bLightColorMultDirty = true;
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
// ----------------------------- Lighting Effects Class ------------------------------
// -----------------------------------------------------------------------------------
CLightEffects::CLightEffects( CLightRecord *pLightRecord )
{
	ASSERTS( pLightRecord, "this should never be NULL" );
	m_pLightRecord = pLightRecord;
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
CLightEffects::~CLightEffects( )
{
	if( m_pLightRecord )
	{
		m_pLightRecord = NULL;
	}
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
void CLightEffects::updateLightEffect( void )
{
	CLightEffects *pLightEffectsRecord = this;
	CLightRecord *pLightRecord = pLightEffectsRecord->m_pLightRecord;
	ASSERTS( pLightRecord, "we better have a valid light record" );

	// --- make sure light is enabled
	//if( pLightRecord->isLightEnabled( ) )
	{
		// --- apply requested lighting effects
		if( pLightEffectsRecord->m_flicker )
		{
			// --- allow the light to flicker before it goes out
			if( pLightEffectsRecord->m_flickerLifeTime )
			{
				m_flickerLifeTime--;

				//float noise = pLightRecord->getIntensityNoise( );
				float intensity = g_random.InRange( 1.0f - 0.4f, 1.0f );
				pLightRecord->setIntensity( intensity );

				if( m_flickerLifeTime == 0 )
				{
					//ParticleMgr__CreateEffect( "Moths", &m_effectsPos, NULL, NULL, NULL, 0.0f );
					m_gameTime = EngineTimer__Time( );
					m_flicker = false;
				}
				return;
			}
		}

		// --- allow the light to intensify for a while
		if( m_overBrighten )
		{
			if( m_overBrightenLifeTime )
			{
				pLightRecord->setIntensity( 1.3f );
				m_overBrightenLifeTime--;
			}
			else
			{
				m_gameTime = EngineTimer__Time( );
				m_overBrighten = false;
			}
			return;
		}

		// --- allow light to dim to 0 
		if( m_glowReduction )
		{
			if( m_glowReductionLifeTime > 0 )
			{
				pLightRecord->setIntensity( 1.0f - ( 1.0f / m_glowReductionLifeTime ) );
				m_glowReductionLifeTime--;
			}
			else
			{
				m_gameTime = EngineTimer__Time( );
				m_glowReduction = false;
				m_flickerLifeTime = 5;
				m_intensity = 0;
			}
			return;
		}

		// --- light is now damaged
		if( m_flickerLifeTime > 0)
		{
			pLightRecord->setIntensity( m_intensity );
			m_flickerLifeTime--;
		}
		else if( m_flickerLifeTime < 0 )
		{
			float intensity = g_random.InRange( 0.2f, 0.6f );
			pLightRecord->setIntensity( intensity );
			m_flickerLifeTime++;
		}
		else
		{
			int rand = g_random.InRange( 0, 10 );
			if( rand <= 4 )
			{
				Vector3Packed vel( 0.0f, -1.0f, 0.0f );
				rand = g_random.InRange( 0, 10 );
				if( rand < 5 )
				{
					ParticleMgr__CreateEffect( "BrokenLightSparks", &m_effectsPos, &vel, NULL, NULL, 0.0f );
					g_messageDispatcher.SendMessageToAll("LightSparks", NULL, INVALID_OBJECT_HANDLE);
				}
				m_flickerLifeTime = -g_random.InRange( 5, 15 );
			}
			else
			{
				m_flickerLifeTime = g_random.InRange( 20, 45 );
				m_intensity = 0.0f;
			}
			pLightRecord->setIntensity( m_intensity );
			//float intensity = g_randomNumber.GetRandomReal( 0.2f, 0.6f );
			//pLightRecord->setIntensity( intensity );
			//m_flickerLifeTime = 4;
		//	m_intensity = intensity;
		}
		return;

		// --- respawn the light
		if( m_respawn )
		{
			if( ( EngineTimer__Time( ) - m_gameTime ) > ( ( u32 )( m_respawnDelay * 1000.0f ) ) )
			{
				pLightRecord->enableLight( true );
				pLightRecord->setIntensity( 1.0f );
				g_lightSet.removeLightEffects( this );
			}
		}
	}
}
// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
bool CLightEffects::addEffects( )
{
	return( g_lightSet.addLightEffects( this ) );
}

// -----------------------------------------------------------------------------------
