////////////////////////////////////////////////////////////////////////////
//
// ExportLighting
//
// Export the lighting sets into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportLighting.h"

ExportLighting::ExportLighting( const char *pDBLFileName, const FilterOptions &options, FilterDataManager &dataManager, TupConfigRoot &tupConfigRoot ) :
	m_pDBLFileName( pDBLFileName ), 
	m_options( options ), 
	m_dataManager( dataManager ), 
	m_tupConfigRoot( tupConfigRoot )
{
}


u32 ExportLighting::WriteLightingSets( TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool )
{
	int totalNodes = tupNodePool.GetNumNodes();
	int nodeIndex;

	int numLights = 0;

	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);

		// if blank entry skip
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int objectRef;
		tupNode.GetObjectReference(objectRef);

		if (objectRef==-1)
			continue;

		TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate( objectRef );	// return the aggregate using a integer index

		TupObject tupObject( pObjectAgg );
		
		// --- only care about lights
		if( tupObject.GetObjectType( ) == TupObject::LIGHT )
		{
			char *pNodeAttributesString;
			tupNode.GetUserProperties( &pNodeAttributesString );
			TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
			TupKeyValueContext tupKeyValueContext;
			tupKeyValueContext.AddKey( "instance_type" );
			m_tupConfigRoot.GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );
			if ( tupKeyValueContext.ValueValidAt( 0 ) && strcmp( tupKeyValueContext.GetValueAsString( 0 ), "world_light" ) == 0 )
			{
				numLights++;
			}
		}
	}

	// --- if we found lights lets export them
	if( numLights )
	{
		FILE *pDBLFile = DBLUtil::GetDBLFile( );
		DBLUtil::StartChunk( DB_LIGHTS, DB_LIGHTS_VERSION, DBL_CHUNK_NO_IGNORE, "1000" );
		DBLUtil::StartRecordList( numLights, RECORD_LIST_HAS_OFFSET_TABLE | RECORD_LIST_QUAD_WORD_ALIGNED, 0 );

		for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
		{
			TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);

			// if blank entry skip
			if (!pNodeAgg)
				continue;

			TupNode tupNode(pNodeAgg);

			int objectRef;
			tupNode.GetObjectReference(objectRef);

			if (objectRef==-1)
				continue;

			TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate( objectRef );	// return the aggregate using a integer index

			TupObject tupObject( pObjectAgg );
			
			// --- only care about lights
			if( tupObject.GetObjectType( ) == TupObject::LIGHT )
			{
				TupLight tupLight( pObjectAgg );
				Vector3 vPos;
				Vector3 vZaxis;
				Vector4 destColor;
				TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );
				int iLightType;
				float *color;
		
				char *pNodeAttributesString;
				tupNode.GetUserProperties( &pNodeAttributesString );
				TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
				TupKeyValueContext tupKeyValueContext;
				tupKeyValueContext.AddKey( "instance_type" );
				m_tupConfigRoot.GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );
				if ( tupKeyValueContext.ValueValidAt( 0 ) && strcmp( tupKeyValueContext.GetValueAsString( 0 ), "world_light" ) == 0 )
				{
					char *lightName;
					tupNode.GetName( &lightName );

					assert( strlen( lightName ) < MAX_LIGHT_NAME );
					tupLight.GetLightType( iLightType );
					tupLight.GetLightColor( &color );
					vPos.Set( 0.0f, 0.0f, 0.0f );
					vZaxis.Set( 0.0f, 0.0f, 0.0f );
					destColor.Set( 0.0f, 0.0f, 0.0f, 0.0f );
					if( pTransformAgg )
					{
						TupTransform tupTransform( pTransformAgg );
						if( tupTransform.GetTransformType( ) == TupTransform::STATIC_MATRIX )
						{
							TupStaticMatrix tupStaticMatrix( pTransformAgg );
							float *pMatrix;
							tupStaticMatrix.GetMatrix( &pMatrix );

							Graphics4x4 matrix;
							Graphics4x4 *pBase = ( Graphics4x4 * )pMatrix;

							// --- appears that we need a -90 rotation about X to get the Z axis lined up correct for the engine
							matrix.SetXRotation( -Math::Pi/2.0f );
							*pBase = matrix * (*pBase);

							// --- get direction of light from z axis
							vZaxis.x( pMatrix[ 8 ] );
							vZaxis.y( pMatrix[ 9 ] );
							vZaxis.z( pMatrix[ 10 ] );

							// --- get light position from translation
							vPos.x( pMatrix[12] );
							vPos.y( pMatrix[13] );
							vPos.z( pMatrix[14] );
						}
					}

					CLightRecord lightRecord;
					float constantAttenuation			= 0.0f;
					float linearAttenuation				= 0.0f;
					float quadraticAttenuation			= 0.1f;
					float cubicAttenuation				= 0.0f;
					float intensityNoise					= 1.0f;
					float sphereConstraintRadius		= 0.0f;
					float cubeConstraintWidthHeight	= 0.0f;
					float lightIntensity					= 1.0f;
					bool bAffectsEnvironmental			= true;
					bool bAffectsStaticInstances		= true;
					bool bAffectsTerrain					= true;
					char *pNodeAttributesString;

					tupNode.GetUserProperties( &pNodeAttributesString );
					TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
					TupKeyValueContext tupLightKeyValueContext;
					tupLightKeyValueContext.AddKey( "ConstantAttenuationCoefficient" );
					tupLightKeyValueContext.AddKey( "LinearAttenuationCoefficient" );
					tupLightKeyValueContext.AddKey( "QuadraticAttenuationCoefficient" );
					tupLightKeyValueContext.AddKey( "CubicAttenuationCoefficient" );
					tupLightKeyValueContext.AddKey( "IntensityNoiseCoefficient" );
					tupLightKeyValueContext.AddKey( "LightVolumeConstraint" );
					tupLightKeyValueContext.AddKey( "LightSphereConstraint" );
					tupLightKeyValueContext.AddKey( "LightCubeConstraint" );
					tupLightKeyValueContext.AddKey( "LightSphereRadius" );
					tupLightKeyValueContext.AddKey( "LightCubeLength" );
					tupLightKeyValueContext.AddKey( "LightIntensity" );
					tupLightKeyValueContext.AddKey( "LightAffectsEnvironmental" );
					tupLightKeyValueContext.AddKey( "LightAffectsStaticInstances" );
					tupLightKeyValueContext.AddKey( "LightAffectsTerrain" );
					m_tupConfigRoot.GetKeyValueContexts( tupKeyValue, tupLightKeyValueContext, false );
					
					if ( tupLightKeyValueContext.ValueValidAt( 0 ) )
					{
						constantAttenuation = tupLightKeyValueContext.GetValueAsFloat( 0 );
					}
					if ( tupLightKeyValueContext.ValueValidAt( 1 ) )
					{
						linearAttenuation = tupLightKeyValueContext.GetValueAsFloat( 1 );
					}
					if ( tupLightKeyValueContext.ValueValidAt( 2 ) )
					{
						quadraticAttenuation = tupLightKeyValueContext.GetValueAsFloat( 2 );
					}
					if ( tupLightKeyValueContext.ValueValidAt( 3 ) )
					{
						cubicAttenuation = tupLightKeyValueContext.GetValueAsFloat( 3 );
					}
					if ( tupLightKeyValueContext.ValueValidAt( 4 ) )
					{
						intensityNoise = tupLightKeyValueContext.GetValueAsFloat( 4 );
					}

					// --- see if light is to move around randomly

					if( tupLightKeyValueContext.ValueValidAt( 5 ) && !strcmp( tupLightKeyValueContext.GetValueAsString( 5 ), "True" ) )
					{
						if( tupLightKeyValueContext.ValueValidAt( 6 ) && !strcmp( tupLightKeyValueContext.GetValueAsString( 6 ),"True" ) )
						{
							if ( tupLightKeyValueContext.ValueValidAt( 8 ) )
							{
								sphereConstraintRadius = tupLightKeyValueContext.GetValueAsFloat( 8 );
							}
						}
						else if( tupLightKeyValueContext.ValueValidAt( 7 ) && !strcmp( tupLightKeyValueContext.GetValueAsString( 7 ),"True" ) )
						{
							if ( tupLightKeyValueContext.ValueValidAt( 9 ) )
							{
								cubeConstraintWidthHeight = tupLightKeyValueContext.GetValueAsFloat( 9 );
							}
						}
					}
					if ( tupLightKeyValueContext.ValueValidAt( 10 ) )
					{
						lightIntensity = tupLightKeyValueContext.GetValueAsFloat( 10 );
					}
					if ( tupLightKeyValueContext.ValueValidAt( 11 ) )
					{
						bAffectsEnvironmental = tupLightKeyValueContext.GetValueAsBool( 11 );
					}
					if ( tupLightKeyValueContext.ValueValidAt( 12 ) )
					{
						bAffectsStaticInstances = tupLightKeyValueContext.GetValueAsBool( 12 );
					}
					if ( tupLightKeyValueContext.ValueValidAt( 13 ) )
					{
						bAffectsTerrain = tupLightKeyValueContext.GetValueAsBool( 13 );
					}

					*( ( Vector3 * )&destColor ) = *( ( Vector3 * )color );
					memset( &lightRecord, 0, sizeof( lightRecord ) );
					lightRecord.setType( iLightType );
					lightRecord.setColor( destColor * lightIntensity );
					lightRecord.setOrigColor( destColor * lightIntensity );
					lightRecord.setPosition( vPos );
					lightRecord.setOrigPosition( vPos );
					lightRecord.setDirection( vZaxis );
					lightRecord.setConstantAttenuation( constantAttenuation );
					lightRecord.setLinearAttenuation( linearAttenuation );
					lightRecord.setQuadraticAttenuation( quadraticAttenuation );
					lightRecord.setCubicAttenuation( cubicAttenuation );
					lightRecord.setIntensity( 1.0f );
					lightRecord.setOrigIntensity( 1.0f );
					lightRecord.setIntensityNoise( intensityNoise );
					lightRecord.setLightName( lightName );
					lightRecord.setVolumeSphereRadius( sphereConstraintRadius );
					lightRecord.setVolumeCubeWidthHeight( cubeConstraintWidthHeight );

					if( iLightType == TupLight::LIGHT_FREE_DIRECT )
					{
						PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH,"\tLight Found,  type = %s,  Direction(%6.4f,%6.4f,%6.4f)", 
							tupLight.GetLightTypeName( ( TupLight::LightTypes )iLightType ), 
							vZaxis.x( ), vZaxis.y( ), vZaxis.z( ) );
					}
					else
					{
						PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH,"\tLight Found,  type = %s,  Position(%6.4f,%6.4f,%6.4f)", 
							tupLight.GetLightTypeName( ( TupLight::LightTypes )iLightType ), 
							vPos.x( ), vPos.y( ), vPos.z( ) );
					}
					u32 lightFlags = 0;
					if (bAffectsEnvironmental)
					{
						lightFlags |= AFFECTS_ENVIRONMENTAL_OBJECTS_BIT;
					}
					if (bAffectsStaticInstances)
					{
						lightFlags |= AFFECTS_STATIC_INSTANCES_BIT;
					}
					if (bAffectsTerrain)
					{
						lightFlags |= AFFECTS_TERRAIN_BIT;
					}
					lightRecord.setLightFlags(lightFlags);

					DBLUtil::RecordListEntry();
					DBLWrite::WriteRecord( &lightRecord, pDBLFile );
				}
			}
		}
		DBLUtil::FinishRecordList();
		return DBLUtil::FinishChunk(FALSE);
	}
	return 0;
}
