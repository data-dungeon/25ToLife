////////////////////////////////////////////////////////////////////////////
//
// LightMeshes
//
// Class to handle the breaking up of meshes into Lit and Unlit
//
////////////////////////////////////////////////////////////////////////////

#include "RealTimeLightingPch.h"
#include "LightManager.h"

LightManager::LightManager( TupNodePool &tupNodePool, TupObjectPool &tupObjectPool, TupModifierPool &tupModifierPool,int tupRootNodeRef,TupConfigRoot &tupConfigRoot ) :
m_tupNodePool(tupNodePool), m_tupObjectPool(tupObjectPool), m_tupModifierPool(tupModifierPool), m_tupRootNodeRef(tupRootNodeRef),m_tupConfigRoot(tupConfigRoot)
{
	int totalNodes = m_tupNodePool.GetNumNodes();
	int nodeIndex;

	int numLights = 0;

	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);

		// if blank entry skip
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int objectRef;
		tupNode.GetObjectReference(objectRef);

		if (objectRef==-1)
			continue;

		TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate( objectRef );	// return the aggregate using a integer index

		TupObject tupObject( pObjectAgg );

		// --- only care about lights
		if( tupObject.GetObjectType( ) == TupObject::LIGHT )
		{
			TupLight tupLight(pObjectAgg);

			char *pNodeAttributesString;
			tupNode.GetUserProperties( &pNodeAttributesString );
			TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
			TupKeyValueContext tupKeyValueContext;
			tupKeyValueContext.AddKey( "instance_type" );
			tupKeyValueContext.AddKey( "QuadraticAttenuationCoefficient" );
			tupKeyValueContext.AddKey( "LightAffectsStaticInstances" );
			tupKeyValueContext.AddKey( "LightAffectsTerrain" );
			m_tupConfigRoot.GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );
			if ( tupKeyValueContext.ValueValidAt( 0 ) && strcmp( tupKeyValueContext.GetValueAsString( 0 ), "world_light" ) == 0 )
			{
				int lightType;
				tupLight.GetLightType( lightType );
				// array only keeps point lights
				if (lightType == TupLight::LIGHT_OMNI)
				{
					Light newLight;
					newLight.m_lightIndex = numLights;

					TupLight tupLight( pObjectAgg );
					TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );

					char *lightName;
					tupNode.GetName( &lightName );
					newLight.m_name = lightName;
					newLight.m_worldPosition.Set(0.0f,0.0f,0.0f);

					if( pTransformAgg )
					{
						TupTransform tupTransform( pTransformAgg );
						if( tupTransform.GetTransformType( ) == TupTransform::STATIC_MATRIX )
						{
							TupStaticMatrix tupStaticMatrix( pTransformAgg );
							float *pMatrix;
							tupStaticMatrix.GetMatrix( &pMatrix );

							// --- get light position from translation
							newLight.m_worldPosition.Set(pMatrix[12],pMatrix[13],pMatrix[14]);
						}
					}

					newLight.m_quadraticAttenuation	= 0.1f;
					if ( tupKeyValueContext.ValueValidAt( 1 ) )
					{
						newLight.m_quadraticAttenuation = tupKeyValueContext.GetValueAsFloat( 1 );
						if (newLight.m_quadraticAttenuation<0.0001f)
						{
							newLight.m_quadraticAttenuation = 0.0001f;
						}
					}

					newLight.m_bAffectsStaticInstances	= true;
					if ( tupKeyValueContext.ValueValidAt( 2 ) )
					{
						newLight.m_bAffectsStaticInstances = tupKeyValueContext.GetValueAsBool( 2 );
					}
					newLight.m_bAffectsTerrain	= true;
					if ( tupKeyValueContext.ValueValidAt( 3 ) )
					{
						newLight.m_bAffectsTerrain = tupKeyValueContext.GetValueAsBool( 3 );
					}

					m_lights.Add(newLight);
				}
				numLights++; // count total lights (so that index is accurate with other lights in the scene that are not point lights)
			}
		}
	}
}

const TupArray<LightManager::Light> &LightManager::GetLights(void)
{
	return m_lights;
}