////////////////////////////////////////////////////////////////////////////
//
// LightMeshes
//
// Class to handle the breaking up of meshes into Lit and Unlit
//
////////////////////////////////////////////////////////////////////////////

#include "RealTimeLightingPch.h"
#include "LightMeshes.h"
#include "RealTimeLighting.h"

LightMeshesManager::LightMeshesManager( TupNodePool &tupNodePool, TupObjectPool &tupObjectPool, TupModifierPool &tupModifierPool,int tupRootNodeRef,TupConfigRoot &tupConfigRoot, LightManager &lightManager, const RealTimeLightingSettings &settings ) :
	m_tupNodePool(tupNodePool), m_tupObjectPool(tupObjectPool), m_tupModifierPool(tupModifierPool), m_tupRootNodeRef(tupRootNodeRef),m_tupConfigRoot(tupConfigRoot),m_lights(lightManager.GetLights()),m_settings(settings)
{
}


// ---------------------------------------------------------------------------------
// ---                  Check all the meshes for realtime lighting               ---
// --- If a mesh is to be realtime lit we go through the mesh and strip out all  ---
// --- the faces that were marked for realtime lighting.  These faces are then   ---
// --- gathered together into a new instance that will have normals.             ---
// ---------------------------------------------------------------------------------
void LightMeshesManager::LightMarkedNodes(void)
{
	int totalNodes = m_tupNodePool.GetNumNodes();
	int nodeIndex;

	// this pass will count how many nodes reference each base mesh
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);

		// --- if empty slot - continue
		if ( !pNodeAgg )
			continue;

		// --- check to see if the mesh may need to be broken up into multiple pieces
		TupperwareScalar *pBreakUpScalar = pNodeAgg->FindScalarByKey( "RealTimeLighting_BreakUpFlag" );
		if( pBreakUpScalar )
		{
			// remove break up flag
			pNodeAgg->DeleteAtomsByKey("RealTimeLighting_BreakUpFlag");

			float instanceAttenuationScale = pNodeAgg->FindScalarByKey( "RealTimeLighting_AttenuationScale")->GetAsFloat();
			// avoid divide by 0 error
			if (instanceAttenuationScale<0.0001f)
			{
				instanceAttenuationScale=0.0001f;
			}

			TupNode tupNode( pNodeAgg );
			int objectRef;
			tupNode.GetObjectReference( objectRef );
			if( objectRef!=-1 )
			{
				TupMatrix4x4f localToWorldMatrix;

				TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );
				if( pTransformAgg )
				{
					TupTransform tupTransform( pTransformAgg );
					if( tupTransform.GetTransformType( ) == TupTransform::STATIC_MATRIX )
					{
						TupStaticMatrix tupStaticMatrix( pTransformAgg );
						float *pMatrix;
						tupStaticMatrix.GetMatrix(&pMatrix);
						memcpy(&localToWorldMatrix,pMatrix,16*sizeof(float));
					}
				}

				int newBaseRef = AnnotateHelper::FindBaseObjectRef( m_tupObjectPool, objectRef );
				TupperwareAggregate *pBaseMeshAgg = m_tupObjectPool.GetObjectAggregate( newBaseRef );
				TupTriMesh tupTriMesh(pBaseMeshAgg);

				TupString meshName;
				TupperwareScalar *pMayaObjectNameScalar = pBaseMeshAgg->FindScalarByKey("MayaObjectName");
				if (pMayaObjectNameScalar)
				{
					meshName = pMayaObjectNameScalar->GetAsString();
				}

				int tempValue;
				int numPositionVerts, numNormalVerts;
				int *pFaceFlags;
				TupVertex3f *pPositionVerts, *pNormalVerts;
				int *pPositionIndices, *pNormalIndices;
				int numFaces;

				tupTriMesh.GetFaceFlags( &pFaceFlags, numFaces );
				tupTriMesh.GetPositionVerts( (float **)&pPositionVerts, numPositionVerts );
				tupTriMesh.GetPositionIndices( &pPositionIndices, tempValue );
				tupTriMesh.GetNormalVerts( (float **)&pNormalVerts, numNormalVerts );
				tupTriMesh.GetNormalIndices( &pNormalIndices, tempValue );

				// transform position and normals into world space
				TupArray<TupVertex3f> worldPositionVerts;
				TupArray<TupVertex3f> worldNormalVerts;

				worldPositionVerts.SetSize(numPositionVerts);
				worldNormalVerts.SetSize(numNormalVerts);
				memcpy(worldPositionVerts.GetData(),pPositionVerts,numPositionVerts*sizeof(TupVertex3f));
				memcpy(worldNormalVerts.GetData(),pNormalVerts,numNormalVerts*sizeof(TupVertex3f));

				localToWorldMatrix.TransformVertices(worldPositionVerts);
				localToWorldMatrix.TransformNormals(worldNormalVerts);

				// this array of arrays keeps tracks of the best lights for each face 
				TupArray<TupArray<int> > faceBestLights;
				faceBestLights.SetSize(numFaces);

				int faceIndex;
				int faceIndex3;
				for (faceIndex=0,faceIndex3=0;faceIndex<numFaces;faceIndex++,faceIndex3+=3)
				{
					// if this is a staged face ignore it here
					if ((pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_STAGE_MASK)!=0)
						continue;

					TupArray<int> bestLights;
					TupArray<float> bestLightFactors;
					bestLights.SetSize(m_settings.m_maxAssignedPointLights);
					bestLightFactors.SetSize(m_settings.m_maxAssignedPointLights);
					int bestLightIndex;
					for (bestLightIndex=0;bestLightIndex<m_settings.m_maxAssignedPointLights;bestLightIndex++)
					{
						bestLights[bestLightIndex] = -1;
					}

					// find the best n lights for the face
					for (int lightIndex=0;lightIndex<m_lights.GetSize();lightIndex++)
					{
						const LightManager::Light &light = m_lights[lightIndex];

						// light needs to affect terrain
						if (!light.m_bAffectsTerrain)
							continue;

						float bestLightFactor;

						// find the highest factor for this light using all 3 points of the face individually
						for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
						{
							TupVertex3f worldPos = worldPositionVerts[pPositionIndices[faceIndex3+facePointIndex]];
							TupVertex3f worldNormal = worldNormalVerts[pNormalIndices[faceIndex3+facePointIndex]];

							TupVertex3f lightVector = light.m_worldPosition-worldPos;
							float distance = lightVector.Length();
							// if we are really close to the light don't get a divide by 0 error
							if (distance<0.001f) distance = 0.001f;
							// normalize vector to the light
							lightVector/=distance;
							float lightFactor = TupVertex3f::DotProduct(worldNormal,lightVector)/(distance*distance*light.m_quadraticAttenuation*instanceAttenuationScale);
							if (facePointIndex == 0 || lightFactor>bestLightFactor)
							{
								bestLightFactor = lightFactor;
							}
						}
						// if light gives at least a minimum amount
						if (bestLightFactor>m_settings.m_pointLightThreshold)
						{
							// keep the list of best lights sorted where the highest light is first and so on
							for (bestLightIndex=0;bestLightIndex<m_settings.m_maxAssignedPointLights;bestLightIndex++)
							{
								// if slot empty
								if (bestLights[bestLightIndex] == -1)
								{
									bestLights[bestLightIndex] = lightIndex;
									bestLightFactors[bestLightIndex] = bestLightFactor;
									break;
								}
								// if better than slot
								else if (bestLightFactors[bestLightIndex]<bestLightFactor)
								{
									// move other lights down
									for (int moveLightIndex=m_settings.m_maxAssignedPointLights-2;moveLightIndex>=bestLightIndex;moveLightIndex--)
									{
										bestLights[moveLightIndex+1] = bestLights[moveLightIndex];
										bestLightFactors[moveLightIndex+1] = bestLightFactors[moveLightIndex];
									}
									bestLights[bestLightIndex] = lightIndex;
									bestLightFactors[bestLightIndex] = bestLightFactor;
									break;
								}
							}
						}
					}
					// now we have a list of lights that affect the face 
					// add to the array for the face in order of the light's index
					TupArray<int> &bestLightsForFace = faceBestLights[faceIndex];
					for (bestLightIndex=0;bestLightIndex<m_settings.m_maxAssignedPointLights;bestLightIndex++)
					{
						int bestLight = bestLights[bestLightIndex];
						// found a light to add
						if (bestLight !=-1)
						{
							int insertPos = -1; // default to add to end
							for (int scanIndex=0;scanIndex<bestLightsForFace.GetSize();scanIndex++)
							{
								if (bestLight<bestLightsForFace[scanIndex])
								{
									insertPos = scanIndex;
									break;
								}
							}
							// we need to insert it
							if (insertPos != -1)
							{
								bestLightsForFace.InsertAt(insertPos,bestLight);
							}
							else
							{
								bestLightsForFace.Add(bestLight);
							}
						}
						else
						{
							// stop when not more lights
							break;
						}
					}
				}
				TupArray<int> lightGroupForFace;
				lightGroupForFace.SetSize(numFaces);

				TupArray<LightGroup> lightGroups;

				LightGroup noLightsGroup;
				noLightsGroup.m_numFaces = 0;

				StageFaceManager stageFaceManager(tupTriMesh);

				// start with the most lights and then go towards none used
				for (int numOfLightsUsed = m_settings.m_maxAssignedPointLights;numOfLightsUsed>=0;numOfLightsUsed--)
				{
					// find and add all faces that use a certain number of lights
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						// if this is a staged face ignore it here
						if ((pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_STAGE_MASK)!=0)
							continue;
						// uses the correct amount of lights
						if (faceBestLights[faceIndex].GetSize()==numOfLightsUsed)
						{
							// try to find an existing light group that works
							int foundGroupIndex = -1;
							for (int lightGroupIndex=0;lightGroupIndex<lightGroups.GetSize();lightGroupIndex++)
							{
								LightGroup &lightGroup = lightGroups[lightGroupIndex];
								if (numOfLightsUsed != 0)
								{
									// if this group is the same size (it needs to be an exact match then)
									if (lightGroup.m_lights.GetSize()==numOfLightsUsed)
									{
										bool bFoundGroup = true;
										for (int lightSearchIndex=0;lightSearchIndex<numOfLightsUsed;lightSearchIndex++)
										{
											if (lightGroup.m_lights[lightSearchIndex]!=faceBestLights[faceIndex][lightSearchIndex])
											{
												bFoundGroup = false;
												break;
											}
										}
										if (bFoundGroup)
										{
											foundGroupIndex = lightGroupIndex;
											break;
										}
									}
									// if this group is larger than the number of lights I am using
									else
									{
										bool bFoundGroup = true;
										int myLightSearchIndex = 0;
										// search through lights in group
										for (int lightSearchIndex=0;lightSearchIndex<lightGroup.m_lights.GetSize();lightSearchIndex++)
										{
											// if found a match (keep going)
											if (lightGroup.m_lights[lightSearchIndex]==faceBestLights[faceIndex][myLightSearchIndex])
											{
												myLightSearchIndex++;
												// stop if done with face lights
												if (myLightSearchIndex == faceBestLights[faceIndex].GetSize())
												{
													break;
												}
											}
											// if group light is greater than face light (not a match)
											else if (lightGroup.m_lights[lightSearchIndex]>faceBestLights[faceIndex][myLightSearchIndex])
											{
												bFoundGroup = false;
												break;
											}
											// if we are here loop again and try to match next group light with current face light
										}
										if (bFoundGroup)
										{
											foundGroupIndex = lightGroupIndex;
											break;
										}
									}
								}
								// if we need to find a group with no lights
								else
								{
									if (lightGroup.m_lights.GetSize()==0)
									{
										foundGroupIndex = lightGroupIndex;
										break;
									}
								}
							}
							// if no group found then we will make one
							if (foundGroupIndex==-1)
							{
								LightGroup newLightGroup;
								newLightGroup.m_lights = faceBestLights[faceIndex];
								foundGroupIndex = lightGroups.GetSize();
								lightGroups.Add(newLightGroup);
							}

							// add all staged faces to group
							int numStages = stageFaceManager.GetNumStages(faceIndex);
							for (int stageIndex=0;stageIndex<numStages;stageIndex++)
							{
								int stageFaceIndex = stageFaceManager.GetStageFace(faceIndex,stageIndex);
								lightGroupForFace[stageFaceIndex] = foundGroupIndex;
								lightGroups[foundGroupIndex].m_numFaces++;
							}
						}
					}
				}

				// if the mesh needs to be broken up
				if (lightGroups.GetSize()>1)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Breaking up %s into %d parts based on point lights.",(const char *)meshName,lightGroups.GetSize());

					CopyObjectsManager copyObjectsManager( m_tupNodePool, m_tupObjectPool, m_tupModifierPool, m_tupRootNodeRef );
					DeleteFaceManager deleteFaceManager( m_tupNodePool, m_tupObjectPool, m_tupModifierPool );

					for (int lightGroupIndex=0;lightGroupIndex<lightGroups.GetSize();lightGroupIndex++)
					{
						LightGroup &lightGroup = lightGroups[lightGroupIndex];

						// build an array for the actual light indices for the light group
						TupArray<int> lightsUsedInGroup;
						lightsUsedInGroup.SetSize(lightGroup.m_lights.GetSize());
						for (int lightIndex=0;lightIndex<lightGroup.m_lights.GetSize();lightIndex++)
						{
							lightsUsedInGroup[lightIndex] = m_lights[lightGroup.m_lights[lightIndex]].m_lightIndex;
						}

						int newNodeRef = nodeIndex; // start with source node
						// if not the last one make a copy from the source node
						if (lightGroupIndex!=lightGroups.GetSize()-1)
						{
							newNodeRef = copyObjectsManager.CopyNode( nodeIndex );
						}

						TupperwareAggregate *pNewNodeAgg = m_tupNodePool.GetNode(newNodeRef);

						if (m_settings.m_bAssignLightsToInstance)
						{
							// tell everyone which lights are needed for this group
							pNewNodeAgg->AddListInt("RealTimeLighting_Lights",NULL,lightsUsedInGroup);
						}

						// if no lights used
						if (lightsUsedInGroup.GetSize() == 0)
						{
							// turn off point lighting if requested
							TupperwareScalar *pTurnOffUnlit = pNewNodeAgg->FindScalarByKey("RealTimeLighting_BreakUpFlagTurnOffUnlit");
							assert(pTurnOffUnlit);
							if (pTurnOffUnlit->GetAsInt()==1)
							{
								// turn off point lighting on ones that use no lights
								TupperwareScalar *pPointLightingScalar = pNewNodeAgg->FindScalarByKey( "RealTimeLighting_Point");
								assert(pPointLightingScalar);
								pPointLightingScalar->SetAsInt(0);
							}
						}
						TupNode newNode(pNewNodeAgg);

						int objectRef;	
						newNode.GetObjectReference(objectRef);

						int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);
						TupperwareAggregate *pNewMeshAgg = m_tupObjectPool.GetObjectAggregate(baseRef);

						TupperwareScalar *pMayaObjectNameScalar = pNewMeshAgg->FindScalarByKey("MayaObjectName");
						if (pMayaObjectNameScalar)
						{
							TupString newMeshName;
							newMeshName = pMayaObjectNameScalar->GetAsString();

							if (lightsUsedInGroup.GetSize()==0)
							{
								TupperwareScalar *pTurnOffUnlit = pNewNodeAgg->FindScalarByKey("RealTimeLighting_BreakUpFlagTurnOffUnlit");
								if (pTurnOffUnlit->GetAsInt()==1)
								{
									newMeshName += "(no light(off))";
								}
								else
								{
									newMeshName += "(no light(on))";
								}
							}
							else
							{
								newMeshName += "(light ";
								for (int lightIndex=0;lightIndex<lightGroup.m_lights.GetSize();lightIndex++)
								{
									newMeshName += m_lights[lightGroup.m_lights[lightIndex]].m_name;
									if (lightIndex < lightGroup.m_lights.GetSize()-1)
									{
										newMeshName += ", ";
									}
									else
									{
										newMeshName += ")";
									}
								}
							}
							PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"New Mesh %s using %d faces.",(const char *)newMeshName,lightGroup.m_numFaces);

							pMayaObjectNameScalar->SetAsString(newMeshName);
						}
						// remove turn off unlit 
						pNewNodeAgg->DeleteAtomsByKey("RealTimeLighting_BreakUpFlagTurnOffUnlit");

						deleteFaceManager.DeleteNodeFaces( lightGroupForFace, lightGroupIndex, newNodeRef);
					}
				}
				// if one group (all the faces use the same one)
				else if (lightGroups.GetSize()==1)
				{
					LightGroup &lightGroup = lightGroups[0];

					// build an array for the actual light indices for the light group
					TupArray<int> lightsUsedInGroup;
					lightsUsedInGroup.SetSize(lightGroup.m_lights.GetSize());
					for (int lightIndex=0;lightIndex<lightGroup.m_lights.GetSize();lightIndex++)
					{
						lightsUsedInGroup[lightIndex] = m_lights[lightGroup.m_lights[lightIndex]].m_lightIndex;
					}

					if (m_settings.m_bAssignLightsToInstance)
					{
						// tell everyone which lights are needed for this group
						pNodeAgg->AddListInt("RealTimeLighting_Lights",NULL,lightsUsedInGroup);
					}

					// if no lights used
					if (lightsUsedInGroup.GetSize() == 0)
					{
						// turn off point lighting if requested
						TupperwareScalar *pTurnOffUnlit = pNodeAgg->FindScalarByKey("RealTimeLighting_BreakUpFlagTurnOffUnlit");
						assert(pTurnOffUnlit);
						if (pTurnOffUnlit->GetAsInt()==1)
						{
							// turn off point lighting on ones that use no lights
							TupperwareScalar *pPointLightingScalar = pNodeAgg->FindScalarByKey( "RealTimeLighting_Point");
							assert(pPointLightingScalar);
							pPointLightingScalar->SetAsInt(0);
						}
					}
					// remove turn off unlit 
					pNodeAgg->DeleteAtomsByKey("RealTimeLighting_BreakUpFlagTurnOffUnlit");
				}
			}
		}
	}
}


