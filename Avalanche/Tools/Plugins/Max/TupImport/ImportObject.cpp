////////////////////////////////////////////////////////////////////////////
//
// ImportObject
//
// Class to handle the objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportObject.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 */

#include "TupImportPch.h"
#include "TupImport.h"
#include "shadgen.h"
#include "bones.h"
#include "filepath.h"

int TupImport::AddObject(Object *pObject)
{
	if (!pObject) return -1;
	// use pObject pointer value for the pointer table
	int index = m_pObjectPoolPtrs->GetIndex(pObject);
	if (index==-1) // if it is not in the list
	{
		index = m_pObjectPoolPtrs->Add(pObject);
		TupperwareAggregate *pObjectAgg = m_pObjectPool->AddObjectAggregate(index);
		TupObject tupObject(pObjectAgg);

		// if our object is a derived object (holds modifiers)
		if (pObject->SuperClassID()== GEN_DERIVOB_CLASS_ID)	
		{
			ObjectState objectState;

			IDerivedObject *pDerivedObject = (IDerivedObject *)pObject;
			int numModifiers = pDerivedObject->NumModifiers();
			int modifierIndex;
			if (numModifiers)
			{
				TupArray<int> modifierEnableFlags;
				modifierEnableFlags.SetSize(numModifiers);
				for (modifierIndex=0;modifierIndex<numModifiers;modifierIndex++)
				{
					Modifier *pModifier = pDerivedObject->GetModifier(modifierIndex);
					modifierEnableFlags[modifierIndex] = pModifier->IsEnabled();
				}
				for (modifierIndex=0;modifierIndex<numModifiers;modifierIndex++)
				{
					objectState = pObject->Eval(m_pInterface->GetTime()); // see what the pipe looks like at this modifier
					if (!objectState.obj->IsSubClassOf(triObjectClassID)) 
					{
						break; // stop if the state at this modifier is not a tri mesh
					}
					// make sure we know about this modifier
					Modifier *pModifier = pDerivedObject->GetModifier(modifierIndex);
					if (TupImport::GetModifierType(pModifier)==MODIFIER_UNKNOWN)
					{
						break; // stop if we don't know this modifier
					}
					pModifier->DisableMod(); // disable for the next pass
				}
				int numActiveModifiers = modifierIndex;
				// re enable the modifiers
				for (modifierIndex=0;modifierIndex<numModifiers;modifierIndex++)
				{
					Modifier *pModifier = pDerivedObject->GetModifier(modifierIndex);
					if (modifierEnableFlags[modifierIndex])
						pModifier->EnableMod();
				}
				// if we stopped then there must be a problem
				if (numActiveModifiers!=numModifiers)
				{
					if (numActiveModifiers)
					{
						tupObject.SetObjectType(TupObject::DERIVED);
						AddDerived(pObjectAgg,pDerivedObject,numActiveModifiers,false); // add the derived object with the active modifiers
						// disable modifiers that were just saved out
						for (modifierIndex=0;modifierIndex<numActiveModifiers;modifierIndex++)
						{
							Modifier *pModifier = pDerivedObject->GetModifier(modifierIndex);
							pModifier->DisableMod();
						}
						objectState = pObject->Eval(m_pInterface->GetTime()); // see what the pipe looks like now
						int otherIndex = m_pObjectPoolPtrs->Add((Object *)(((int *)pObject)+1)); // use the address of pObject+4
						TupperwareAggregate *pOtherObjectAgg = m_pObjectPool->AddObjectAggregate(otherIndex);
						TupObject otherTupObject(pOtherObjectAgg);
						// add stuff below the modifiers using the object state
						AddNonDerivedObject(objectState.obj,otherTupObject);
						// re enable the modifiers
						for (modifierIndex=0;modifierIndex<numActiveModifiers;modifierIndex++)
						{
							Modifier *pModifier = pDerivedObject->GetModifier(modifierIndex);
							if (modifierEnableFlags[modifierIndex])
								pModifier->EnableMod();
						}
						// set the object reference
						TupDerived tupDerived(pObjectAgg);
						tupDerived.AddObjectReference(otherIndex);
					}
					// no good modifiers - just save out what it looks like at the end of the pipeline
					else
					{
						objectState = pObject->Eval(m_pInterface->GetTime()); // see what the pipe looks like at this modifier
						AddNonDerivedObject(objectState.obj,tupObject);
					}
				}
				else
				{
					tupObject.SetObjectType(TupObject::DERIVED);
					AddDerived(pObjectAgg,pDerivedObject,numModifiers,true); // add the derived object and all the modifiers
				}
			}
			else
			{
				tupObject.SetObjectType(TupObject::DERIVED);
				AddDerived(pObjectAgg,pDerivedObject,0,true); // add the derived object with no modifiers
			}
		}
		else
		{
			AddNonDerivedObject(pObject,tupObject);
		}
	}
	return index;
}

void TupImport::AddNonDerivedObject(Object *pObject,TupObject &tupObject)
{
	TupperwareAggregate *pObjectAgg = tupObject.GetBaseAggregate();

	// We look at the super class ID to determine the type of the object.
	bool found = false;
	switch(pObject->SuperClassID()) 
	{

		case SYSTEM_CLASS_ID:
			// XRef object
			if (pObject->ClassID() == Class_ID(XREFOBJ_CLASS_ID,0))
			{
				IXRefObject *pXRef = (IXRefObject *)pObject;
				tupObject.SetObjectType(TupObject::XREF);
				AddXRef(pObjectAgg,pXRef);
				found = true;
			}
			break;

		// geometry
		case GEOMOBJECT_CLASS_ID: 
			// check if it a bone...
			if (pObject->ClassID() == BONE_OBJ_CLASSID)
			{
				tupObject.SetObjectType(TupObject::BONE);
				BoneObj *pBoneObj = (BoneObj *)pObject;
				AddBone(pObjectAgg,pBoneObj);
				found = true;
			}
			else if (pObject->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
			{
				TriObject *pTri = (TriObject *) pObject->ConvertToType(m_pInterface->GetTime(), Class_ID(TRIOBJ_CLASS_ID, 0));
				tupObject.SetObjectType(TupObject::TRI_MESH);
				AddMesh(pObjectAgg,pTri);
				if (pObject != pTri) // delete if it was just created
					delete pTri;
				found = true;
			}
			break;

		// camera
		case CAMERA_CLASS_ID:
			{
				GenCamera *pCamera = (GenCamera *)pObject;
				tupObject.SetObjectType(TupObject::CAMERA);
				AddCamera(pObjectAgg,pCamera);
				found = true;
			}
			break;

		// light
		case LIGHT_CLASS_ID:
			{
				GenLight *pLight = (GenLight *)pObject;
				tupObject.SetObjectType(TupObject::LIGHT);
				AddLight(pObjectAgg,pLight);
				found = true;
			}
			break;

		// shapes, paths, etc
		case SHAPE_CLASS_ID:
			{
				ShapeObject *pShapeObject = (ShapeObject *) pObject;
				if(pShapeObject->CanMakeBezier())
				{
					BezierShape bezierShape;
					pShapeObject->MakeBezier(m_pInterface->GetTime(),bezierShape);

					tupObject.SetObjectType(TupObject::BEZIER_SHAPE);
					AddBezierShape(pObjectAgg,bezierShape);
					found = true;
				}
			}
			break;
		// ???
		case HELPER_CLASS_ID:
			break;
	}
	// if we could not find it then add an unknown object
	if (!found)
	{
		tupObject.SetObjectType(TupObject::UNKNOWN);
		AddUnknownObject(pObjectAgg,pObject);
	}
}

void TupImport::AddMesh(TupperwareAggregate *pObjectAgg,TriObject *pTriObject)
{
	int i;
	Mesh* mesh = &pTriObject->GetMesh();
	mesh->buildNormals();

	TupMesh tupMesh(pObjectAgg);

	int numFaces = mesh->getNumFaces();
	tupMesh.AddPositionVerts((float *)mesh->verts,mesh->getNumVerts());
	TVFace *vertexIndicies = new TVFace[numFaces];
	for (i=0;i<numFaces;i++)
	{
		vertexIndicies[i].t[0] = mesh->faces[i].v[0];
		vertexIndicies[i].t[1] = mesh->faces[i].v[1];
		vertexIndicies[i].t[2] = mesh->faces[i].v[2];
	}
	tupMesh.AddPositionIndices((int *)vertexIndicies,numFaces);

	int *faceIndicies = new int[numFaces];
	Keeper<Point3> vertexNormalArray;
	Keeper<Point3> faceNormalArray;

	// Face and vertex normals.
	// In MAX a vertex can have more than one normal (but doesn't always have it).
	// This is depending on the face you are accessing the vertex through.
	// To get all information we need to export all three vertex normals
	// for every face.
	for (i=0; i<numFaces; i++) 
	{
		int  vert;
		Point3 faceNormal;  // Face normal
		Point3 vertexNormal;  // Vertex normal
		Face &face = mesh->faces[i];

		faceNormal = mesh->getFaceNormal(i);
		faceIndicies[i] = faceNormalArray.Add(faceNormal);
		
		vert = face.getVert(0);
		vertexNormal = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));
		vertexIndicies[i].t[0] = vertexNormalArray.Add(vertexNormal);

		vert = face.getVert(1);
		vertexNormal = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));
		vertexIndicies[i].t[1] = vertexNormalArray.Add(vertexNormal);

		vert = face.getVert(2);
		vertexNormal = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));
		vertexIndicies[i].t[2] = vertexNormalArray.Add(vertexNormal);
	}
	tupMesh.AddNormalVerts((float *)vertexNormalArray.GetData(),vertexNormalArray.GetSize());
	tupMesh.AddNormalIndices((int *)vertexIndicies,numFaces);

	tupMesh.AddFaceNormalVerts((float *)faceNormalArray.GetData(),faceNormalArray.GetSize());
	tupMesh.AddFaceNormalIndices((int *)faceIndicies,numFaces);

	delete faceIndicies;
	delete vertexIndicies;

	int *faceFlags = new int[numFaces];
	int *faceMaterialIndicies = new int[numFaces];
	int *faceShadingGroups = new int[numFaces];
	// set up the face flags and shading groups
	for (i=0; i<numFaces; i++) 
	{
		Face &face = mesh->faces[i];
		faceFlags[i] = face.flags & 0xff; // just use the lower 8 bits
		faceMaterialIndicies[i] = (face.flags>>FACE_MATID_SHIFT)&FACE_MATID_MASK;
		faceShadingGroups[i] = face.smGroup;
	}
	tupMesh.AddFaceFlags(faceFlags,numFaces);
	tupMesh.AddFaceMaterialIndices(faceMaterialIndicies,numFaces);
	tupMesh.AddFaceShadingGroups(faceShadingGroups,numFaces);

	delete faceFlags;
	delete faceMaterialIndicies;
	delete faceShadingGroups;

	TupperwareAggregate *pMainMapAgg = NULL;
	// Save any maps we need to
	for (int mapIndex = -NUM_HIDDENMAPS; mapIndex < MAX_MESHMAPS-1; mapIndex++) 
	{
		if (mesh->mapSupport(mapIndex)) 
		{
			MeshMap &meshMap = mesh->Map(mapIndex);
			int numMapVerts = meshMap.getNumVerts();
			int numMapFaces = meshMap.getNumFaces();
			if (!numMapVerts || !numMapFaces)
				continue;
			TupperwareAggregate *pMeshMapAgg = tupMesh.AddMeshMap(mapIndex);
			TupMeshMap tupMeshMap(pMeshMapAgg);

			Keeper<Point3> mapVertexArray;
			TVFace *mapFaceArray = new TVFace[numMapFaces];
			if (mapIndex==0)
			{
				for (i=0;i<numMapFaces;i++)
				{
					mapFaceArray[i].t[0] = mapVertexArray.Add(meshMap.tv[meshMap.tf[i].t[0]]);
					mapFaceArray[i].t[1] = mapVertexArray.Add(meshMap.tv[meshMap.tf[i].t[1]]);
					mapFaceArray[i].t[2] = mapVertexArray.Add(meshMap.tv[meshMap.tf[i].t[2]]);
				}
			}
			else
			{
				Point3 tp;
				for (i=0;i<numMapFaces;i++)
				{
					tp = meshMap.tv[meshMap.tf[i].t[0]]; tp.y = 1.0f-tp.y;
					mapFaceArray[i].t[0] = mapVertexArray.Add(tp);
					tp = meshMap.tv[meshMap.tf[i].t[1]]; tp.y = 1.0f-tp.y;
					mapFaceArray[i].t[1] = mapVertexArray.Add(tp);
					tp = meshMap.tv[meshMap.tf[i].t[2]]; tp.y = 1.0f-tp.y;
					mapFaceArray[i].t[2] = mapVertexArray.Add(tp);
				}
			}
			tupMeshMap.AddMapFlags(meshMap.flags);
			tupMeshMap.AddMapVerts((float *)mapVertexArray.GetData(),mapVertexArray.GetSize());
			tupMeshMap.AddMapIndices((int *)mapFaceArray,numMapFaces);
			delete mapFaceArray;
		}
	}
}

Point3 TupImport::GetVertexNormal(Mesh *mesh, int faceNo,RVertex* rv)
{
	Face &face = mesh->faces[faceNo];
	DWORD smGroup = face.smGroup;
	int numNormals;
	Point3 vertexNormal;
	
	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) 
	{
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
	{
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) 
		{
			vertexNormal = rv->rn.getNormal();
		}
		else 
		{
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) 
			{
				if (rv->ern[i].getSmGroup() & smGroup) 
				{
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else 
	{
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh->getFaceNormal(faceNo);
	}
	
	return vertexNormal;
}

void TupImport::AddCamera(TupperwareAggregate *pObjectAgg,GenCamera *pCamera)
{
	TupCamera tupCamera(pObjectAgg);
	float apertureWidth = m_pInterface->GetRendApertureWidth();
	float imageAspect = m_pInterface->GetRendImageAspect();

	float fov = pCamera->GetFOV(m_pInterface->GetTime());
	float lens = float((0.5f*apertureWidth)/Math::Tan(fov/2.0f));
	tupCamera.AddCameraLens(lens);
	tupCamera.AddFieldOfViewHorizontal(fov);
	tupCamera.AddFieldOfViewVertical(2.0*Math::ArcTan(Math::Tan(fov/2.0f)/imageAspect));
	float height = apertureWidth/imageAspect;
	float distance = Math::Sqrt(apertureWidth*apertureWidth + height*height);	
	tupCamera.AddFieldOfViewDiagonal(2.0*Math::ArcTan((distance/apertureWidth)*Math::Tan(fov/2.0f)));
	tupCamera.AddCameraType(pCamera->Type());
	tupCamera.AddEnvironmentNearRange(pCamera->GetEnvRange(m_pInterface->GetTime(),ENV_NEAR_RANGE));
	tupCamera.AddEnvironmentFarRange(pCamera->GetEnvRange(m_pInterface->GetTime(),ENV_FAR_RANGE));
	tupCamera.AddNearClip(pCamera->GetClipDist(m_pInterface->GetTime(),CAM_HITHER_CLIP));
	tupCamera.AddNearClip(pCamera->GetClipDist(m_pInterface->GetTime(),CAM_YON_CLIP));
	tupCamera.AddTargetDistance(pCamera->GetTDist(m_pInterface->GetTime()));
	int flags = 0;
	if (pCamera->IsOrtho()) flags |= TupCamera::FLAGS_ORTHOGRAPHIC_PROJECTION;
	if (pCamera->GetConeState()) flags |= TupCamera::FLAGS_SHOW_CONE;
	if (pCamera->GetHorzLineState()) flags |= TupCamera::FLAGS_SHOW_HORIZON;
	if (pCamera->GetEnvDisplay()) flags |= TupCamera::FLAGS_SHOW_ENVIRONMENT_RANGES;
	if (pCamera->GetManualClip()) flags |= TupCamera::FLAGS_CLIP_MANUALLY;
	tupCamera.AddFlags(flags);
}

void TupImport::AddLight(TupperwareAggregate *pObjectAgg,GenLight *pLight)
{
	TupLight tupLight(pObjectAgg);

	tupLight.AddLightType(pLight->Type());
	tupLight.AddLightColor((float *)pLight->GetRGBColor(m_pInterface->GetTime()));
	tupLight.AddMultiplier(pLight->GetIntensity(m_pInterface->GetTime()));
	tupLight.AddAffectSurfacesContrast(pLight->GetContrast(m_pInterface->GetTime()));
	tupLight.AddAffectSurfacesSoftenDiffEdge(pLight->GetDiffuseSoft(m_pInterface->GetTime()));
	tupLight.AddProjectorMapReference(AddMap(pLight->GetProjMap()));
	tupLight.AddLightConeHotspot(pLight->GetHotspot(m_pInterface->GetTime()));
	tupLight.AddLightConeFalloff(pLight->GetFallsize(m_pInterface->GetTime()));
	tupLight.AddLightConeRectangleAspectRatio(pLight->GetAspect(m_pInterface->GetTime()));
	tupLight.AddTargetDistance(pLight->GetTDist(m_pInterface->GetTime()));
	tupLight.AddNearAttenuationStart(pLight->GetAtten(m_pInterface->GetTime(),ATTEN1_START));
	tupLight.AddNearAttenuationEnd(pLight->GetAtten(m_pInterface->GetTime(),ATTEN1_END));
	tupLight.AddFarAttenuationStart(pLight->GetAtten(m_pInterface->GetTime(),ATTEN_START));
	tupLight.AddFarAttenuationEnd(pLight->GetAtten(m_pInterface->GetTime(),ATTEN_END));
	tupLight.AddAttenuationDecayType(pLight->GetDecayType());
	tupLight.AddAttenuationDecayStart(pLight->GetDecayRadius(m_pInterface->GetTime()));
	tupLight.AddShadowType(pLight->GetShadowMethod());
	tupLight.AddShadowColor((float *)pLight->GetShadColor(m_pInterface->GetTime()));
	tupLight.AddShadowDensity(pLight->GetShadMult(m_pInterface->GetTime()));
	tupLight.AddShadowMapReference(AddMap(pLight->GetShadowProjMap()));
	tupLight.AddAtmosphereShadowsOpacity(pLight->GetAtmosOpacity(m_pInterface->GetTime()));
	tupLight.AddAtmosphereShadowsColorAmount(pLight->GetAtmosColAmt(m_pInterface->GetTime()));

	ShadowType *shadowType = pLight->GetShadowGenerator();
	if (shadowType)
	{
		tupLight.AddShadowMapBias(shadowType->GetMapBias(m_pInterface->GetTime()));
		tupLight.AddShadowMapSize(shadowType->GetMapSize(m_pInterface->GetTime()));
		tupLight.AddShadowMapSampleRange(shadowType->GetMapRange(m_pInterface->GetTime()));
		tupLight.AddRayTraceBias(shadowType->GetRayBias(m_pInterface->GetTime()));
		tupLight.AddRayMaxDepth(shadowType->GetMaxDepth(m_pInterface->GetTime()));
	}

	int flags = 0;
	if (pLight->GetUseLight()) flags |= TupLight::FLAGS_ENABLE;
	if (pLight->GetShadow()) flags |= TupLight::FLAGS_CAST_SHADOWS;
	if (pLight->GetAffectDiffuse()) flags |= TupLight::FLAGS_AFFECT_SURFACES_DIFFUSE;
	if (pLight->GetAffectSpecular()) flags |= TupLight::FLAGS_AFFECT_SURFACES_SPECULAR;
	if (pLight->GetAmbientOnly()) flags |= TupLight::FLAGS_AFFECT_SURFACES_AMBIENT_ONLY;
	if (pLight->GetConeDisplay()) flags |= TupLight::FLAGS_SHOW_LIGHT_CONE;
	if (pLight->GetOvershoot()) flags |= TupLight::FLAGS_LIGHT_CONE_OVERSHOOT;
	if (pLight->GetSpotShape()) flags |= TupLight::FLAGS_LIGHT_CONE_CIRCLE;
	if (pLight->GetProjector()) flags |= TupLight::FLAGS_PROJECTOR_MAP_ENABLE;
	if (pLight->GetUseAttenNear()) flags |= TupLight::FLAGS_NEAR_ATTENUATION_USE;
	if (pLight->GetAttenNearDisplay()) flags |= TupLight::FLAGS_NEAR_ATTENUATION_SHOW;
	if (pLight->GetUseAtten()) flags |= TupLight::FLAGS_FAR_ATTENUATION_USE;
	if (pLight->GetAttenDisplay()) flags |= TupLight::FLAGS_FAR_ATTENUATION_SHOW;
//	if (pLight->GetDecayDisplay()) flags |= TupLight::FLAGS_ATTENUATION_DECAY_SHOW;
	if (pLight->GetUseGlobal()) flags |= TupLight::FLAGS_OBJECT_SHADOWS_USE_GLOBAL_SETTINGS;
	if (pLight->GetUseShadowColorMap(m_pInterface->GetTime())) flags |= TupLight::FLAGS_OBJECT_SHADOWS_MAP_ENABLE;
	if (pLight->GetLightAffectsShadow()) flags |= TupLight::FLAGS_OBJECT_SHADOWS_LIGHT_AFFECTS_SHADOW_COLOR;
	if (pLight->GetAtmosShadows(m_pInterface->GetTime())) flags |= TupLight::FLAGS_ATMOSPHERE_SHADOWS_ON;
	if (shadowType && shadowType->GetAbsMapBias(m_pInterface->GetTime())) flags |= TupLight::FLAGS_SHADOW_MAP_ABSOLUTE_MAP_BIAS;

	tupLight.AddFlags(flags);
}

void TupImport::AddDerived(TupperwareAggregate *pObjectAgg,IDerivedObject *pDerivedObject,int numModifiers,bool bAddObjectReference)
{
	TupDerived tupDerived(pObjectAgg);
	if (bAddObjectReference)
		tupDerived.AddObjectReference(AddObject(pDerivedObject->GetObjRef()));
	for (int i=0;i<numModifiers;i++)
	{
		TupperwareAggregate *pDerivedModifierAgg = tupDerived.AddModifier(i);
		TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
		Modifier *pModifier = pDerivedObject->GetModifier(i);
		ModContext *pModifierContext = pDerivedObject->GetModContext(i);
		TupperwareAggregate *pDerivedModifierContextAgg = tupDerivedModifier.AddModifierContext();
		tupDerivedModifier.AddModifierReference(AddModifier(pModifier,pDerivedModifierContextAgg,pModifierContext));
	}
}

static void ChangeExtension(TupString &path,const TCHAR *extension)
{
	TCHAR fdrive[_MAX_DRIVE];
	TCHAR fdir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR fext[_MAX_EXT];
	TCHAR fpath[_MAX_PATH];

	_splitpath(path,fdrive,fdir,fname,fext);
	_makepath(fpath,fdrive,fdir,fname,extension);
	path = fpath;
}

void TupImport::AddXRef(TupperwareAggregate *pObjectAgg,IXRefObject *pXRefObject)
{
	TupXRef tupXRef(pObjectAgg);
	TupString filename = pXRefObject->GetFileName();

	TupString relativePath,fullPath1,fullPath2,name,ext;
	// get full path of xref
	FilePath::GetPathsToFile(relativePath,fullPath1,name,ext,filename,m_sourceFilename);
	// get relative path from output filename to mapname
	FilePath::GetPathsToFile(relativePath,fullPath2,name,ext,fullPath1,m_outputFilename);

	fullPath2.MakeLower(); // make lower case

	// add full path name to the keeper
	m_pXRefFilenameStrings->Add(fullPath2);
	// change extension to tup for our sake
//	ChangeExtension(relativePath,".tup");
//	tupXRef.AddFilename(relativePath);
	ChangeExtension(fullPath2,".tup");
	tupXRef.AddFilename(fullPath2); // use full path name for now

	tupXRef.AddNodeName(pXRefObject->GetObjName());
}

void TupImport::AddBezierShape(TupperwareAggregate *pObjectAgg,BezierShape &bezierShape)
{
	TupBezierShape tupBezierShape(pObjectAgg);
	int totalSplines = bezierShape.SplineCount();
	for (int splineIndex =0;splineIndex<totalSplines;splineIndex++)
	{
		Spline3D *pSpline = bezierShape.GetSpline(splineIndex);
		TupperwareAggregate *pSplineAgg = tupBezierShape.AddSpline(splineIndex);
		TupSpline tupSpline(pSplineAgg);
		// add flags
		int splineFlags = 0;
		if (pSpline->Closed())
			splineFlags |= TupSpline::FLAGS_CLOSED;
		tupSpline.AddFlags(splineFlags);
		// add knots
		int totalKnots = pSpline->KnotCount();
		if (totalKnots)
		{
			Point3 *pPointLocations = new Point3[totalKnots];
			Point3 *pInVectors = new Point3[totalKnots];
			Point3 *pOutVectors = new Point3[totalKnots];
			int *pPointFlags = new int[totalKnots];
			for (int knotIndex = 0;knotIndex<totalKnots;knotIndex++)
			{
				SplineKnot splineKnot = pSpline->GetKnot(knotIndex);
				pPointLocations[knotIndex] = splineKnot.Knot();
				pInVectors[knotIndex] = splineKnot.InVec();
				pOutVectors[knotIndex] = splineKnot.OutVec();
				int knotFlags = 0;
				switch (splineKnot.Ktype())
				{
					case KTYPE_AUTO:
						knotFlags |= TupSpline::FLAGS_KNOT_TYPE_AUTO;
						break;
					case KTYPE_CORNER:
						knotFlags |= TupSpline::FLAGS_KNOT_TYPE_CORNER;
						break;
					case KTYPE_BEZIER:
						knotFlags |= TupSpline::FLAGS_KNOT_TYPE_BEZIER;
						break;
					case KTYPE_BEZIER_CORNER:
						knotFlags |= TupSpline::FLAGS_KNOT_TYPE_BEZIER_CORNER;
						break;
				}
				switch (splineKnot.Ltype())
				{
					case LTYPE_CURVE:
						knotFlags |= TupSpline::FLAGS_LINE_TYPE_CURVE;
						break;
					case LTYPE_LINE:
						knotFlags |= TupSpline::FLAGS_LINE_TYPE_LINE;
						break;
				}
				pPointFlags[knotIndex] = knotFlags;
			}
			tupSpline.AddPointLocations((float *)pPointLocations,totalKnots);
			tupSpline.AddInVectors((float *)pInVectors,totalKnots);
			tupSpline.AddOutVectors((float *)pOutVectors,totalKnots);
			tupSpline.AddPointFlags(pPointFlags,totalKnots);
			delete [] pPointLocations;
			delete [] pInVectors;
			delete [] pOutVectors;
			delete [] pPointFlags;
		}
	}
}

void TupImport::AddBone(TupperwareAggregate *pObjectAgg,BoneObj *pBoneObj)
{
	int flags = 0;
	IParamBlock2 *pBlock = pBoneObj->GetParamBlock(0); // get paramater block for bone
	TupBone tupBone(pObjectAgg);
	// General Properties
	tupBone.AddWidth(pBlock->GetFloat(boneobj_width,m_pInterface->GetTime()));
	tupBone.AddHeight(pBlock->GetFloat(boneobj_height,m_pInterface->GetTime()));
	tupBone.AddTaper(pBlock->GetFloat(boneobj_taper,m_pInterface->GetTime()));
	tupBone.AddLength(pBlock->GetFloat(boneobj_length,m_pInterface->GetTime()));
	// Side Fins
	if (pBlock->GetInt(boneobj_sidefins,m_pInterface->GetTime()))
		flags |= TupBone::SIDE_FINS;
	tupBone.AddSideFinsSize(pBlock->GetFloat(boneobj_sidefins_size,m_pInterface->GetTime()));
	tupBone.AddSideFinsStartTaper(pBlock->GetFloat(boneobj_sidefins_starttaper,m_pInterface->GetTime()));
	tupBone.AddSideFinsEndTaper(pBlock->GetFloat(boneobj_sidefins_endtaper,m_pInterface->GetTime()));
	// Front Fin
	if (pBlock->GetInt(boneobj_frontfin,m_pInterface->GetTime()))
		flags |= TupBone::FRONT_FIN;
	tupBone.AddFrontFinSize(pBlock->GetFloat(boneobj_frontfin_size,m_pInterface->GetTime()));
	tupBone.AddFrontFinStartTaper(pBlock->GetFloat(boneobj_frontfin_starttaper,m_pInterface->GetTime()));
	tupBone.AddFrontFinEndTaper(pBlock->GetFloat(boneobj_frontfin_endtaper,m_pInterface->GetTime()));
	// Back Fin
	if (pBlock->GetInt(boneobj_backfin,m_pInterface->GetTime()))
		flags |= TupBone::BACK_FIN;
	tupBone.AddBackFinSize(pBlock->GetFloat(boneobj_backfin_size,m_pInterface->GetTime()));
	tupBone.AddBackFinStartTaper(pBlock->GetFloat(boneobj_backfin_starttaper,m_pInterface->GetTime()));
	tupBone.AddBackFinEndTaper(pBlock->GetFloat(boneobj_backfin_endtaper,m_pInterface->GetTime()));
	// Generate Mappings
	if (pBlock->GetInt(boneobj_genmap,m_pInterface->GetTime()))
		flags |= TupBone::GEN_MAP;
	tupBone.AddFlags(flags);
}

void TupImport::AddUnknownObject(TupperwareAggregate *pObjectAgg,Object *pObject)
{
	TSTR className;
	pObject->GetClassName(className);
	pObjectAgg->AddScalarString("ClassName",NULL,className.data());
	Class_ID classID = pObject->ClassID();
	char string[40];
	sprintf(string,"0x%x",classID.PartA());
	pObjectAgg->AddScalarString("ClassID","PartA",string);
	sprintf(string,"0x%x",classID.PartB());
	pObjectAgg->AddScalarString("ClassID","PartB",string);
	SClass_ID superClassID = pObject->SuperClassID();
	sprintf(string,"0x%x",superClassID);
	pObjectAgg->AddScalarString("SuperClassID",NULL,string);
}
