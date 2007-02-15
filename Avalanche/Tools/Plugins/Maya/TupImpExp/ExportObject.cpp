////////////////////////////////////////////////////////////////////////////
//
// ExportObject
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportObject.cpp $
 * 
 * *****************  Version 39  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 38  *****************
 * User: Adam Clayton Date: 8/05/03    Time: 2:05p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * polymesh object type
 * 
 * *****************  Version 37  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:59a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 36  *****************
 * User: Adam Clayton Date: 7/30/03    Time: 12:45p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * polygon export support
 * 
 * *****************  Version 35  *****************
 * User: Adam Clayton Date: 7/02/03    Time: 3:32p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * make y up working
 * 
 * *****************  Version 34  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vector3 issues
 * assert issues
 * add audio clips
 * 
 * *****************  Version 33  *****************
 * User: Adam Clayton Date: 6/09/03    Time: 2:55p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * camera lookat node references
 * 
 * *****************  Version 32  *****************
 * User: Adam Clayton Date: 6/06/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * instanced clip curves
 * camera lookat
 * 
 * *****************  Version 31  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first phase of animation export
 * 
 * *****************  Version 30  *****************
 * User: Adam Clayton Date: 4/24/03    Time: 2:52p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * use dag path as maya object name
 * 
 * *****************  Version 29  *****************
 * User: Adam Clayton Date: 4/22/03    Time: 10:51a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * impovement on handling boned models
 * 
 * *****************  Version 28  *****************
 * User: Adam Clayton Date: 4/16/03    Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * new consolidation code
 * start of getting prebound mesh
 * 
 * *****************  Version 27  *****************
 * User: Adam Clayton Date: 3/20/03    Time: 2:34p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 3/17/03    Time: 2:19p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 3/17/03    Time: 11:56a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * wrong index
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 3/17/03    Time: 11:13a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * consolidate shapes
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 3/06/03    Time: 12:24p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 3/04/03    Time: 3:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * don't save out a mesh map unless we have verts
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 3:44p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * map channel start
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 2/06/03    Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export reflection maps
 * export lights
 * 
 * *****************  Version 19  *****************
 * User: Tony Jones   Date: 1/31/03    Time: 11:29a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 3:53p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * layered texture output
 * uv sets output
 * 
 * *****************  Version 17  *****************
 * User: Tony Jones   Date: 1/15/03    Time: 2:13p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 1/13/03    Time: 3:55p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 1/13/03    Time: 12:03p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fixed child bone matrix lookup (use x axis due to converted maya
 * matrix)
 * dot product uses unit vectors
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 12/18/02   Time: 2:30p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 12/18/02   Time: 12:13p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting face attribute modifiers
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 12/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone matrix output correct
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 12/05/02   Time: 3:48p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone pose exporting 
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 11/18/02   Time: 2:20p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * clamp vertex colors
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 11/18/02   Time: 12:59p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bezier curve fixes
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 11/15/02   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vertex color modifiers
 * able to disable object consolidation
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/14/02   Time: 3:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bezier shape
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/14/02   Time: 12:55p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * attribute fixes
 * consolidate based on name (ignore numbers on end)
 * save material from mesh to use on other instances
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/13/02   Time: 4:21p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of materials, maps, images, and attributes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * node, mesh output working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/06/02   Time: 3:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of mesh stuff
 */

#include "TupImpExpPch.h"
#include "TupExport.h"
#include "../../libraries/MTools/MTools.h"
#include "UnknownObject.h"
#include "MeshManager.h"
#include "SkinManager.h"

static const float SCALEFACTOR = .01f;

int TupExport::AddObject(MDagPath mDagPath,int &materialRef)
{
	materialRef = -1;
	MFnDependencyNode fnNode(mDagPath.node());
	int objectPtr = (int)MayaHelper::GetMPtrBase(fnNode);

	// check if we have already done this object before
	int objectPtrIndex = m_pUniqueObjectPtrs->GetIndex(objectPtr);
	if (objectPtrIndex!=-1)
	{
		UniqueObjectRefs &uniqueObjectRefs = (*m_pUniqueObjectRefs)[objectPtrIndex];
		materialRef = uniqueObjectRefs.m_materialRef;
		return uniqueObjectRefs.m_objectRef;
	}
	else
	{
		// add the unique value for this object into the keeper
		m_pUniqueObjectPtrs->Add(objectPtr);
		UniqueObjectRefs uniqueObjectRefs; // holds the references for this object

		if (mDagPath.apiType()==MFn::kMesh)
		{
			MDagPath mPBDagPath = GetPreBoundMesh(mDagPath); // don't do this yet
			int meshManagerIndex = m_pMeshManager->AddMesh(mDagPath,mPBDagPath);

			materialRef = m_pMeshManager->GetShaderID(meshManagerIndex);
			int parentMeshIndex = m_pMeshManager->GetParentMeshIndex(meshManagerIndex);
			// if we don't have a parent mesh
			if (parentMeshIndex==-1)
			{
				uniqueObjectRefs.m_objectRef = m_pObjectPool->AddObjectAggregate();
				TupperwareAggregate *pObjectAgg = m_pObjectPool->GetObjectAggregate(uniqueObjectRefs.m_objectRef);
				pObjectAgg->AddScalarString("MayaObjectName",NULL,mDagPath.fullPathName().asChar());
				TupObject tupObject(pObjectAgg);
#ifdef POLYGON_EXPORT
				tupObject.SetObjectType(TupObject::POLY_MESH);
#else
				tupObject.SetObjectType(TupObject::TRI_MESH);
#endif
				// add in the mesh information and get a material reference
				AddMesh(pObjectAgg,meshManagerIndex);
				// tell the mesh manager about my refs
				m_pMeshManager->SetMeshObjectRef(meshManagerIndex,uniqueObjectRefs.m_objectRef);
				m_pMeshManager->SetMeshMaterialRef(meshManagerIndex,materialRef);
				if (m_pMeshManager->MeshHasVertexColors(meshManagerIndex))
				{
					// add in the vertex color information
					uniqueObjectRefs.m_objectRef = AddVertexColorModifier(meshManagerIndex,uniqueObjectRefs.m_objectRef);

					// add in the shape name to the derived object
					TupperwareAggregate *pObjectAgg = m_pObjectPool->GetObjectAggregate(uniqueObjectRefs.m_objectRef);
					pObjectAgg->AddScalarString("MayaObjectName",NULL,fnNode.name().asChar());

					// tell the mesh manager about this ref
					m_pMeshManager->SetVertexColorObjectRef(meshManagerIndex,uniqueObjectRefs.m_objectRef);
				}
			}
			else
			{
				// we have a parent mesh
				uniqueObjectRefs.m_objectRef = m_pMeshManager->GetMeshObjectRef(parentMeshIndex);

				// add in this shape name underneath the parent
				TupperwareAggregate *pObjectAgg = m_pObjectPool->GetObjectAggregate(uniqueObjectRefs.m_objectRef);
				TupperwareScalar *pConsolidatedNameScalar = pObjectAgg->AddScalarString("MayaConsolidatedObjectName",NULL,mDagPath.fullPathName().asChar());
				TupperwareScalar *pMayObjectNameScalar = pObjectAgg->FindScalarByKey("MayaObjectName");
				pObjectAgg->InsertChild(pConsolidatedNameScalar,pMayObjectNameScalar);

				if (m_pMeshManager->MeshHasVertexColors(meshManagerIndex))
				{
					int parentVertexColorIndex = m_pMeshManager->GetParentVertexColorIndex(meshManagerIndex);
					// if we don't have a parent mesh for vertex colors
					if (parentVertexColorIndex==-1)
					{
						// add in the vertex color information
						uniqueObjectRefs.m_objectRef = AddVertexColorModifier(meshManagerIndex,uniqueObjectRefs.m_objectRef);

						// add in the shape name to the derived object
						TupperwareAggregate *pObjectAgg = m_pObjectPool->GetObjectAggregate(uniqueObjectRefs.m_objectRef);
						pObjectAgg->AddScalarString("MayaObjectName",NULL,mDagPath.fullPathName().asChar());

						// tell the mesh manager about this ref
						m_pMeshManager->SetVertexColorObjectRef(meshManagerIndex,uniqueObjectRefs.m_objectRef);
					}
					else // if we have a parent mesh for vertex colors
					{
						// get the reference from the parent vertex color mesh (should point to the same base mesh)
						uniqueObjectRefs.m_objectRef = m_pMeshManager->GetVertexColorObjectRef(parentVertexColorIndex);

						// add in the shape name to the derived object (under the parent's name)
						TupperwareAggregate *pObjectAgg = m_pObjectPool->GetObjectAggregate(uniqueObjectRefs.m_objectRef);
						TupperwareScalar *pConsolidatedNameScalar = pObjectAgg->AddScalarString("MayaConsolidatedObjectName",NULL,mDagPath.fullPathName().asChar());
						TupperwareScalar *pMayObjectNameScalar = pObjectAgg->FindScalarByKey("MayaObjectName");
						pObjectAgg->InsertChild(pConsolidatedNameScalar,pMayObjectNameScalar);
					}
				}
			}
			// try to add these if they exist
			uniqueObjectRefs.m_objectRef = AddSkinModifier(mDagPath,uniqueObjectRefs.m_objectRef);
			uniqueObjectRefs.m_objectRef = AddFaceAttributeModifier(mDagPath,uniqueObjectRefs.m_objectRef);
		}
		else
		{
			// not a mesh (these are not consolidated)
			uniqueObjectRefs.m_objectRef = m_pObjectPool->AddObjectAggregate();
			TupperwareAggregate *pObjectAgg = m_pObjectPool->GetObjectAggregate(uniqueObjectRefs.m_objectRef);
			pObjectAgg->AddScalarString("MayaObjectName",NULL,mDagPath.fullPathName().asChar());
			TupObject tupObject(pObjectAgg);

			switch (mDagPath.apiType())
			{
				case MFn::kNurbsCurve:
					{
						tupObject.SetObjectType(TupObject::SPLINE_SHAPE);
						AddSplineShape(pObjectAgg,mDagPath);
					}
					break;
				case MFn::kJoint:
					{
						tupObject.SetObjectType(TupObject::BONE);
						AddBone(pObjectAgg,mDagPath);
					}
					break;
				case MFn::kPointLight:
				case MFn::kDirectionalLight:
				case MFn::kSpotLight:
				case MFn::kAreaLight:
				case MFn::kAmbientLight:
					{
						tupObject.SetObjectType(TupObject::LIGHT);
						AddLight(pObjectAgg,mDagPath);
					}
					break;
				case MFn::kCamera:
					{
						tupObject.SetObjectType(TupObject::CAMERA);
						AddCamera(pObjectAgg,mDagPath);
					}
					break;
				case MFn::kLocator:
					{
						tupObject.SetObjectType(TupObject::LOCATOR);
						AddLocator(pObjectAgg,mDagPath);
					}
					break;
				case MFn::kLookAt:
					{
						tupObject.SetObjectType(TupObject::LOOKAT);
						AddLookAt(pObjectAgg,mDagPath);
					}
					break;
				default:
					{
						tupObject.SetObjectType(TupObject::UNKNOWN);
						AddUnknownObject(pObjectAgg,mDagPath);
					}
					break;
			}
		}
		uniqueObjectRefs.m_materialRef = materialRef;
		
		m_pUniqueObjectRefs->Add(uniqueObjectRefs); // add me to the table of objects
		return uniqueObjectRefs.m_objectRef;
	}
}


void TupExport::AddMesh(TupperwareAggregate *pObjectAgg,int meshManagerIndex)
{
#ifdef POLYGON_EXPORT
	TupPolyMesh tupPolyMesh(pObjectAgg);

	int numFaces = m_pMeshManager->GetMeshFaceFlags(meshManagerIndex).GetSize();
	
	tupPolyMesh.AddPositionVerts((const float *)m_pMeshManager->GetMeshPositionVertices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshPositionVertices(meshManagerIndex).GetSize());
	tupPolyMesh.AddPositionIndices(m_pMeshManager->GetMeshPositionIndices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshPositionIndices(meshManagerIndex).GetSize());
	
	tupPolyMesh.AddNormalVerts((const float *)m_pMeshManager->GetMeshNormalVertices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshNormalVertices(meshManagerIndex).GetSize());
	tupPolyMesh.AddNormalIndices(m_pMeshManager->GetMeshNormalIndices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshNormalIndices(meshManagerIndex).GetSize());
	
	tupPolyMesh.AddFaceNormalVerts((const float *)m_pMeshManager->GetMeshFaceNormalVertices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshFaceNormalVertices(meshManagerIndex).GetSize());
	tupPolyMesh.AddFaceNormalIndices(m_pMeshManager->GetMeshFaceNormalIndices(meshManagerIndex).GetData(),numFaces);
	
	tupPolyMesh.AddFaceFlags(m_pMeshManager->GetMeshFaceFlags(meshManagerIndex).GetData(),numFaces);
	
	tupPolyMesh.AddFaceMaterialIndices(m_pMeshManager->GetMeshFaceMaterialIndices(meshManagerIndex).GetData(),numFaces);

	tupPolyMesh.AddFaceVertexCounts(m_pMeshManager->GetMeshFaceVertexCounts(meshManagerIndex).GetData(),numFaces);
	tupPolyMesh.AddFaceTriangleCounts(m_pMeshManager->GetMeshFaceTriangleCounts(meshManagerIndex).GetData(),numFaces);
	tupPolyMesh.AddTrianglePointIndices(m_pMeshManager->GetMeshTrianglePointIndices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshTrianglePointIndices(meshManagerIndex).GetSize());
	
	int numUVSets = m_pMeshManager->GetNumUVSets(meshManagerIndex);
	// add in the uv sets
	for (int uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		// make sure there are some vertices to do
		if (m_pMeshManager->GetMeshUVVertices(meshManagerIndex,uvSetIndex).GetSize())
		{
			TupperwareAggregate *pUVMeshMapAgg = tupPolyMesh.AddMeshMap(TupPolyMeshMap::MAP_CHANNEL_TEXTURE_UV_START+uvSetIndex);
			TupPolyMeshMap tupPolyMeshMap(pUVMeshMapAgg);
			tupPolyMeshMap.AddMapName(m_pMeshManager->GetMeshUVName(meshManagerIndex,uvSetIndex));
			tupPolyMeshMap.AddMapFlags(TupPolyMeshMap::MESHMAP_FLAG_TEXTURE);
			tupPolyMeshMap.AddMapIndices(m_pMeshManager->GetMeshUVIndices(meshManagerIndex,uvSetIndex).GetData(),m_pMeshManager->GetMeshUVIndices(meshManagerIndex,uvSetIndex).GetSize());
			tupPolyMeshMap.AddMapVerts((const float *)m_pMeshManager->GetMeshUVVertices(meshManagerIndex,uvSetIndex).GetData(),m_pMeshManager->GetMeshUVVertices(meshManagerIndex,uvSetIndex).GetSize());
		}
	}

#else

	TupTriMesh tupTriMesh(pObjectAgg);

	int numFaces = m_pMeshManager->GetMeshFaceFlags(meshManagerIndex).GetSize();

	tupTriMesh.AddPositionVerts((const float *)m_pMeshManager->GetMeshPositionVertices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshPositionVertices(meshManagerIndex).GetSize());
	tupTriMesh.AddPositionIndices(m_pMeshManager->GetMeshPositionIndices(meshManagerIndex).GetData(),numFaces);

	tupTriMesh.AddNormalVerts((const float *)m_pMeshManager->GetMeshNormalVertices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshNormalVertices(meshManagerIndex).GetSize());
	tupTriMesh.AddNormalIndices(m_pMeshManager->GetMeshNormalIndices(meshManagerIndex).GetData(),numFaces);

	tupTriMesh.AddFaceNormalVerts((const float *)m_pMeshManager->GetMeshFaceNormalVertices(meshManagerIndex).GetData(),m_pMeshManager->GetMeshFaceNormalVertices(meshManagerIndex).GetSize());
	tupTriMesh.AddFaceNormalIndices(m_pMeshManager->GetMeshFaceNormalIndices(meshManagerIndex).GetData(),numFaces);

	tupTriMesh.AddFaceFlags(m_pMeshManager->GetMeshFaceFlags(meshManagerIndex).GetData(),numFaces);

	tupTriMesh.AddFaceShadingGroups(m_pMeshManager->GetMeshFaceShadingGroups(meshManagerIndex).GetData(),numFaces);
	tupTriMesh.AddFaceMaterialIndices(m_pMeshManager->GetMeshFaceMaterialIndices(meshManagerIndex).GetData(),numFaces);

	int numUVSets = m_pMeshManager->GetNumUVSets(meshManagerIndex);

	// add in the uv sets
	for (int uvSetIndex = 0; uvSetIndex < numUVSets; uvSetIndex++)
	{
		// make sure there are some vertices to do
		if (m_pMeshManager->GetMeshUVVertices(meshManagerIndex,uvSetIndex).GetSize())
		{
			TupperwareAggregate *pUVMeshMapAgg = tupTriMesh.AddMeshMap(TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START+uvSetIndex);
			TupTriMeshMap tupTriMeshMap(pUVMeshMapAgg);
			tupTriMeshMap.AddMapFlags(TupTriMeshMap::MESHMAP_FLAG_TEXTURE);
			tupTriMeshMap.AddMapIndices(m_pMeshManager->GetMeshUVIndices(meshManagerIndex,uvSetIndex).GetData(),numFaces);
			tupTriMeshMap.AddMapVerts((const float *)m_pMeshManager->GetMeshUVVertices(meshManagerIndex,uvSetIndex).GetData(),m_pMeshManager->GetMeshUVVertices(meshManagerIndex,uvSetIndex).GetSize());
		}
	}
#endif
}

void TupExport::AddSplineShape(TupperwareAggregate *pObjectAgg,MDagPath mDagPath)
{
	TupSplineShape tupSplineShape(pObjectAgg);
		
	MFnDependencyNode fnNode(mDagPath.node());
	MStatus status;
	MFnNurbsCurve fnCurve(mDagPath,&status);
	if (!status)
		return;

	int degree = fnCurve.degree(&status);
	if (degree<1 || degree>8)
		return;
	int numCVs = fnCurve.numCVs(&status);
	if (!numCVs)
		return;
	int numKnots = fnCurve.numKnots(&status);
	if (!numKnots)
		return;
	int form = fnCurve.form(&status);

	TupperwareAggregate *pSplineAgg = tupSplineShape.AddSpline(0);
	TupSpline tupSpline(pSplineAgg);

	int splineFlags = 0;
	switch (degree)
	{
		case 1:
			splineFlags |= TupSpline::FLAGS_DEGREE_ONE;
			break;
		case 2:
			splineFlags |= TupSpline::FLAGS_DEGREE_TWO;
			break;
		case 3:
			splineFlags |= TupSpline::FLAGS_DEGREE_THREE;
			break;
		case 4:
			splineFlags |= TupSpline::FLAGS_DEGREE_FOUR;
			break;
		case 5:
			splineFlags |= TupSpline::FLAGS_DEGREE_FIVE;
			break;
		case 6:
			splineFlags |= TupSpline::FLAGS_DEGREE_SIX;
			break;
		case 7:
			splineFlags |= TupSpline::FLAGS_DEGREE_SEVEN;
			break;
	}

	switch (form)
	{
		case MFnNurbsCurve::kOpen:
			splineFlags |= TupSpline::FLAGS_FORM_OPEN;
			break;
		case MFnNurbsCurve::kClosed:
			splineFlags |= TupSpline::FLAGS_FORM_CLOSED;
			break;
		case MFnNurbsCurve::kPeriodic:
			splineFlags |= TupSpline::FLAGS_FORM_PERIODIC;
			break;
	}

	splineFlags |= TupSpline::FLAGS_TYPE_B_SPLINE;

	MDoubleArray mKnotArray;
	fnCurve.getKnots(mKnotArray);

	MPointArray mCVArray;
	fnCurve.getCVs(mCVArray);

	TupArray<Vector4> controlPoints;
	controlPoints.SetSize(numCVs);

	TupArray<float> knots;
	knots.SetSize(numKnots);

	for (int cvIndex=0;cvIndex<numCVs;cvIndex++)
	{
		MPoint &src = mCVArray[cvIndex];
		Vector4 &dest = controlPoints[cvIndex];
#ifdef MAKE_Z_UP
		dest.Set((float)src.x,-(float)src.z,(float)src.y,(float)src.w);
#else
		dest.Set((float)src.x,(float)src.y,(float)src.z,(float)src.w);
#endif
		dest *= SCALEFACTOR;
	}

	for (int knotIndex=0;knotIndex<numKnots;knotIndex++)
	{
		knots[knotIndex] = (float)mKnotArray[knotIndex];
	}

	tupSpline.AddControlPoints((float*)controlPoints.GetData(),numCVs);
	tupSpline.AddKnots((float *)knots.GetData(),numKnots);
	tupSpline.AddFlags(splineFlags);

#ifdef ADD_OLD_BEZIER_SPLINE_DATA
	{
		TupperwareAggregate *pBezierSplineAgg = pSplineAgg->AddAggregate("BezierSpline",NULL);
		TupBezierSpline tupBezierSpline(pBezierSplineAgg);
		// add flags
		int splineFlags = 0;
		if (fnCurve.form() != MFnNurbsCurve::kOpen)
			splineFlags |= TupBezierSpline::FLAGS_CLOSED;
		tupBezierSpline.AddFlags(splineFlags);

		double param;
		MPoint point;
		int knotIndex;
		// build an array of knot points
		TupArray<Vector3> knotPoints;
		for (knotIndex = 0;knotIndex<numKnots;knotIndex++)
		{
			param = fnCurve.knot(knotIndex,&status);
			if (!status)
				continue;
			fnCurve.getPointAtParam(param,point,MSpace::kObject);

			Vector3 pos;
#ifdef MAKE_Z_UP
			pos.Set((float)point.x,(float)-point.z,(float)point.y);
#else
			pos.Set((float)point.x,(float)point.y,(float)point.z);
#endif
			pos *= SCALEFACTOR;
			knotPoints.Add(pos);
		}
		numKnots = knotPoints.GetSize(); // set size again

		if (numKnots==0)
			return;

		TupArray<Vector3> pointLocations;
		TupArray<Vector3> inVectors;
		TupArray<Vector3> outVectors;
		TupArray<int> pointFlags;

		// use the knot points to build the Bezier curve
		// we may want to make a new kind of object that is a nurb curve if we need to go back to Maya...
		for (knotIndex = 2;knotIndex<numKnots-2;knotIndex++)
		{

			Vector3 pos,prev,next;
			pos = knotPoints[knotIndex];

			Vector3 inVec;
			inVec.Set(0.0f,0.0f,0.0f);
			Vector3 outVec;
			outVec.Set(0.0f,0.0f,0.0f);

			if (knotIndex == 2)
			{
				if (fnCurve.form() == MFnNurbsCurve::kOpen)
				{
					next = knotPoints[knotIndex+1];
					outVec = next - pos;
					outVec /= 3.0f;
					outVec += pos;
				}
				else
				{
					prev = knotPoints[1];
					next = knotPoints[numKnots-2];

					outVec = next - prev;
					outVec *= 0.5f;
					outVec /= 3.0f;
					outVec += pos;

					inVec = prev - next;
					inVec *= 0.5f;
					inVec /= 3.0f;
					inVec += pos;				
				}
			}
			else if (knotIndex == numKnots-2)
			{
				if (fnCurve.form() == MFnNurbsCurve::kOpen)
				{
					prev = knotPoints[knotIndex-1];
					inVec = prev - pos;
					inVec /= 3.0f;
					inVec += pos;
				}
				else
				{
					next = knotPoints[1];
					prev = knotPoints[numKnots-2];

					float itLen = (prev-pos).Length();
					float otLen = (next-pos).Length();

					outVec = next - prev;
					float sum = outVec.Length();
					outVec *= (otLen/sum);
					outVec /= 3.0f;
					outVec += pos;

					inVec = prev - next;
					inVec *= (itLen/sum);
					inVec /= 3.0f;
					inVec += pos;				
				}
			}
			else
			{
				prev = knotPoints[knotIndex-1];
				next = knotPoints[knotIndex+1];

				float itLen = (prev-pos).Length();
				float otLen = (next-pos).Length();

				outVec = next - prev;
				float sum = outVec.Length();
				outVec *= (otLen/sum);
				outVec /= 3.0;
				outVec += pos;

				inVec = prev - next;
				inVec *= (itLen/sum);
				inVec /= 3.0f;
				inVec += pos;
			}
			pointLocations.Add(pos);
			inVectors.Add(inVec);
			outVectors.Add(outVec);
			pointFlags.Add(TupBezierSpline::FLAGS_LINE_TYPE_CURVE|TupBezierSpline::FLAGS_KNOT_TYPE_BEZIER);
		}
		tupBezierSpline.AddPointLocations((float *)pointLocations.GetData(),pointLocations.GetSize());
		tupBezierSpline.AddInVectors((float *)inVectors.GetData(),inVectors.GetSize());
		tupBezierSpline.AddOutVectors((float *)outVectors.GetData(),outVectors.GetSize());
		tupBezierSpline.AddPointFlags(pointFlags.GetData(),pointFlags.GetSize());
	}
#endif
}

void TupExport::AddBone(TupperwareAggregate *pObjectAgg,MDagPath mDagPath)
{
	TupBone tupBone(pObjectAgg);

	MMatrix tm = mDagPath.inclusiveMatrix();
	Graphics4x4 matrix;
	ConvertMayaBoneMatrix(tm,matrix);
	Vector3 bonePos,xAxis;
	bonePos = matrix.GetTranslation();
	// get the x axis of the joint
	xAxis.Set(matrix.r00,matrix.r01,matrix.r02);
	xAxis.SafeNormalize();
	
	bool bEndJointFound = false;
	int bestChildIndex = -1;
	float bestChildWeight;
	float bestBoneLength;
	int numChildren = mDagPath.childCount();
	// see if we can find the best child for computing the length
	for (int childIndex = 0; childIndex < numChildren; childIndex++)
	{
		MObject childObject = mDagPath.child(childIndex);
		MDagPath childPath = mDagPath;
		childPath.push(childObject);
		// found a child joint
		if (childPath.apiType()==MFn::kJoint)
		{
			tm = childPath.inclusiveMatrix();
			ConvertMayaBoneMatrix(tm,matrix);
			Vector3 childBonePos(matrix.GetTranslation());
			Vector3 boneVector = childBonePos-bonePos;
			float boneLength = boneVector.Length();
			boneVector.SafeNormalize();
			float dotProduct = Vector3::Dot(boneVector,xAxis);

			MFnDagNode dagNode(childPath.node());
			TupString childName(dagNode.name().asChar());
			// if the name matches it is the winner
			if (childName.Find("_end")!=-1)
			{
				bestChildIndex = childIndex;
				bestChildWeight = dotProduct;
				bestBoneLength = boneLength;
				break;
			}
			// if the name does not match keep track of the one that is closest to the z axis
			else if (bestChildIndex==-1 || bestChildWeight<dotProduct)
			{
				bestChildIndex = childIndex;
				bestChildWeight = dotProduct;
				bestBoneLength = boneLength;
			}
		}
	}
	tupBone.AddWidth(bestBoneLength/4.0f);
	tupBone.AddHeight(bestBoneLength/4.0f);
	tupBone.AddLength(bestBoneLength);
	// set these to a good default
	tupBone.AddTaper(0.9f);
	tupBone.AddSideFinsSize(bestBoneLength/2.0f);
	tupBone.AddSideFinsStartTaper(0.1f);
	tupBone.AddSideFinsEndTaper(0.1f);
	tupBone.AddFrontFinSize(bestBoneLength/2.0f);
	tupBone.AddFrontFinStartTaper(0.1f);
	tupBone.AddFrontFinEndTaper(0.1f);
	tupBone.AddBackFinSize(bestBoneLength/2.0f);
	tupBone.AddBackFinStartTaper(0.1f);
	tupBone.AddBackFinEndTaper(0.1f);
	tupBone.AddFlags(0);
}

void TupExport::AddLight(TupperwareAggregate *pObjectAgg,MDagPath mDagPath)
{
	TupLight tupLight(pObjectAgg);

	// start with some good defaults
	int lightType = TupLight::LIGHT_AMBIENT;
	MColor lightColor(1.0f,1.0f,1.0f);
	float multiplier = 1.0f;
	float affectSurfacesContrast = 0.0f;
	float affectSurfacesSoftenDiffEdge = 0.0f;
	int projectorMapReference = -1;
	float lightConeHotspot = -1.0f;
	float lightConeFalloff = -1.0f;
	float lightConeRectangleAspectRatio = -1.0f;
	float targetDistance = -1.0f;
	float nearAttenuationStart = 0.0f;
	float nearAttenuationEnd = 40.0f;
	float farAttenuationStart = 80.0f;
	float farAttenuationEnd = 200.0f;
	int attenuationDecayType = TupLight::ATTENUATION_DECAY_NONE;
	float attenuationDecayStart = 24.0f;
	int shadowType = TupLight::SHADOW_TYPE_SHADOW_MAP;
	MColor shadowColor(0.0f,0.0f,0.0f);
	float shadowDensity = 1.0f;
	int shadowMapReference = -1;
	float atmosphereShadowsOpacity = 1.0f;
	float atmosphereShadowsColorAmount = 1.0f;
	float shadowMapBias = 1.0f;
	int shadowMapSize = 512;
	float shadowMapSampleRange = 4.0f;
	float rayTraceBias = 0.0f;
	int rayTraceMaxDepth = 1;
	int flags = TupLight::FLAGS_ENABLE|
					TupLight::FLAGS_AFFECT_SURFACES_DIFFUSE|
					TupLight::FLAGS_AFFECT_SURFACES_SPECULAR|
					TupLight::FLAGS_LIGHT_CONE_CIRCLE;


	switch (mDagPath.apiType())
	{
		case MFn::kPointLight:
			{
				MFnPointLight mfnPointLight(mDagPath);
				lightType = TupLight::LIGHT_OMNI;
				lightColor = mfnPointLight.color();
				multiplier = mfnPointLight.intensity();
			}
			break;
		case MFn::kDirectionalLight:
			{
				MFnDirectionalLight mfnDirectional(mDagPath);
				lightType = TupLight::LIGHT_FREE_DIRECT;
				lightColor = mfnDirectional.color();
				multiplier = mfnDirectional.intensity();
			}
			break;
		case MFn::kSpotLight:
			{
				MFnSpotLight mfnSpotLight(mDagPath);
				lightType = TupLight::LIGHT_FREE_SPOT;
				lightColor = mfnSpotLight.color();
				multiplier = mfnSpotLight.intensity();
			}
			break;
		case MFn::kAreaLight:
			{
				MFnAreaLight mfnAreaLight(mDagPath);
				lightType = TupLight::LIGHT_FREE_SPOT;
				lightColor = mfnAreaLight.color();
				multiplier = mfnAreaLight.intensity();
			}
			break;
		case MFn::kAmbientLight:
			{
				MFnAmbientLight mfnAmbientLight(mDagPath);
				lightType = TupLight::LIGHT_AMBIENT;
				lightColor = mfnAmbientLight.color();
				multiplier = mfnAmbientLight.intensity();
			}
			break;
	}
		
	// add the values into the tup data
	tupLight.AddLightType(lightType);
	tupLight.AddLightColor((float *)&lightColor);
	tupLight.AddMultiplier(multiplier);
	tupLight.AddAffectSurfacesContrast(affectSurfacesContrast);
	tupLight.AddAffectSurfacesSoftenDiffEdge(affectSurfacesSoftenDiffEdge);
	tupLight.AddProjectorMapReference(projectorMapReference);
	tupLight.AddLightConeHotspot(lightConeHotspot);
	tupLight.AddLightConeFalloff(lightConeFalloff);
	tupLight.AddLightConeRectangleAspectRatio(lightConeRectangleAspectRatio);
	tupLight.AddTargetDistance(targetDistance);
	tupLight.AddNearAttenuationStart(nearAttenuationStart);
	tupLight.AddNearAttenuationEnd(nearAttenuationEnd);
	tupLight.AddFarAttenuationStart(farAttenuationStart);
	tupLight.AddFarAttenuationEnd(farAttenuationEnd);
	tupLight.AddAttenuationDecayType(attenuationDecayType);
	tupLight.AddAttenuationDecayStart(attenuationDecayStart);
	tupLight.AddShadowType(shadowType);
	tupLight.AddShadowColor((float *)&shadowColor);
	tupLight.AddShadowDensity(shadowDensity);
	tupLight.AddShadowMapReference(shadowMapReference);
	tupLight.AddAtmosphereShadowsOpacity(atmosphereShadowsOpacity);
	tupLight.AddAtmosphereShadowsColorAmount(atmosphereShadowsColorAmount);
		
	tupLight.AddShadowMapBias(shadowMapBias);
	tupLight.AddShadowMapSize(shadowMapSize);
	tupLight.AddShadowMapSampleRange(shadowMapSampleRange);
	tupLight.AddRayTraceBias(rayTraceBias);
	tupLight.AddRayMaxDepth(rayTraceMaxDepth);
		
	tupLight.AddFlags(flags);
}

void TupExport::AddCamera(TupperwareAggregate *pObjectAgg,MDagPath mDagPath)
{
	TupCamera tupCamera(pObjectAgg);

	MFnCamera mfnCamera(mDagPath);

	int flags=0;

	if (mfnCamera.isOrtho())
		flags |= TupCamera::FLAGS_ORTHOGRAPHIC;

	tupCamera.AddFocalLength((float)mfnCamera.focalLength());
	tupCamera.AddCameraScale((float)mfnCamera.cameraScale());
	tupCamera.AddNearClipPlane((float)mfnCamera.nearClippingPlane());
	tupCamera.AddFarClipPlane((float)mfnCamera.farClippingPlane());
	tupCamera.AddHorizontalFilmAperture((float)mfnCamera.horizontalFilmAperture());
	tupCamera.AddVerticalFilmAperture((float)mfnCamera.verticalFilmAperture());
	tupCamera.AddLensSqueezeRatio((float)mfnCamera.lensSqueezeRatio());
	tupCamera.AddFlags(flags);
}

void TupExport::AddLocator(TupperwareAggregate *pObjectAgg,MDagPath mDagPath)
{
	TupLocator tupLocator(pObjectAgg);
	
	int flags=0;

	tupLocator.AddFlags(flags);
}

void TupExport::AddLookAt(TupperwareAggregate *pObjectAgg,MDagPath mDagPath)
{
	MStatus status;
	
	TupLookAt tupLookAt(pObjectAgg);
	MFnDependencyNode lookAtNode(mDagPath.node());

	tupLookAt.AddDistanceBetween(MayaHelper::GetFloatAttrib(lookAtNode,"distanceBetween"));
	tupLookAt.AddTwist(MayaHelper::GetFloatAttrib(lookAtNode,"twist"));
	tupLookAt.AddDistanceBetween(MayaHelper::GetFloatAttrib(lookAtNode,"distanceBetween"));

	float tempVector[3];
	tempVector[0] = MayaHelper::GetFloatAttrib(lookAtNode,"worldUpVectorX");
	tempVector[1] = MayaHelper::GetFloatAttrib(lookAtNode,"worldUpVectorY");
	tempVector[2] = MayaHelper::GetFloatAttrib(lookAtNode,"worldUpVectorZ");
	tupLookAt.AddWorldUpVector(tempVector);

	tempVector[0] = MayaHelper::GetFloatAttrib(lookAtNode,"upVectorX");
	tempVector[1] = MayaHelper::GetFloatAttrib(lookAtNode,"upVectorY");
	tempVector[2] = MayaHelper::GetFloatAttrib(lookAtNode,"upVectorZ");
	tupLookAt.AddUpVector(tempVector);

	tempVector[0] = MayaHelper::GetFloatAttrib(lookAtNode,"aimVectorX");
	tempVector[1] = MayaHelper::GetFloatAttrib(lookAtNode,"aimVectorY");
	tempVector[2] = MayaHelper::GetFloatAttrib(lookAtNode,"aimVectorZ");
	tupLookAt.AddAimVector(tempVector);

	int upLocatorNodeRef = -1;
	MPlug worldUpMatrixPlug = lookAtNode.findPlug("worldUpMatrix",&status);
	if (status==MStatus::kSuccess)
	{
		MPlugArray conns;
		worldUpMatrixPlug.connectedTo(conns,true,false,&status);
		if (status==MStatus::kSuccess && conns.length())
		{
			MFnDagNode dagNode(conns[0].node());
			MDagPathArray paths;
			dagNode.getAllPaths(paths);
			int numDagPaths = paths.length();
			for (int pathIndex=0;pathIndex<numDagPaths;pathIndex++)
			{
				MDagPath dagPath = paths[pathIndex];
				dagPath.pop(); // go up
				if (mDagPath==dagPath) // found the lookat child
				{
					upLocatorNodeRef = m_pNodePoolDagPaths->GetIndex(paths[pathIndex]);
					break;
				}
			}
		}
	}
	tupLookAt.AddUpNodeReference(upLocatorNodeRef);
	 
	int aimLocatorNodeRef = -1;
	MPlug targetPlug = lookAtNode.findPlug("targetTranslateX",&status);
	if (status==MStatus::kSuccess)
	{
		MPlugArray conns;
		targetPlug.connectedTo(conns,true,false,&status);
		if (status==MStatus::kSuccess && conns.length())
		{
			MFnDagNode dagNode(conns[0].node());
			MDagPathArray paths;
			dagNode.getAllPaths(paths);
			int numDagPaths = paths.length();
			for (int pathIndex=0;pathIndex<numDagPaths;pathIndex++)
			{
				MDagPath dagPath = paths[pathIndex];
				dagPath.pop(); // go up
				if (mDagPath==dagPath) // found the lookat child
				{
					aimLocatorNodeRef = m_pNodePoolDagPaths->GetIndex(paths[pathIndex]);
					break;
				}
			}
		}
	}

	tupLookAt.AddAimNodeReference(aimLocatorNodeRef);

	int cameraNodeRef = -1;
	MPlug rotateXPlug = lookAtNode.findPlug("constraintRotateX",&status);
	if (status==MStatus::kSuccess)
	{
		MPlugArray conns;
		rotateXPlug.connectedTo(conns,false,true,&status);
		if (status==MStatus::kSuccess && conns.length())
		{
			MFnDagNode dagNode(conns[0].node());
			MDagPathArray paths;
			dagNode.getAllPaths(paths);
			int numDagPaths = paths.length();
			for (int pathIndex=0;pathIndex<numDagPaths;pathIndex++)
			{
				MDagPath dagPath = paths[pathIndex];
				dagPath.pop(); // go up
				if (mDagPath==dagPath) // found the lookat child
				{
					cameraNodeRef = m_pNodePoolDagPaths->GetIndex(paths[pathIndex]);
					break;
				}
			}
		}
	}
	tupLookAt.AddCameraNodeReference(cameraNodeRef);


	int flags=0;

	int worldUpType = MayaHelper::GetIntAttrib(lookAtNode,"worldUpType");
	switch(worldUpType) 
	{
		case 0:
			flags |= TupLookAt::FLAGS_WORLD_UP_TYPE_SCENE_UP;
			break;
		case 1:
			flags |= TupLookAt::FLAGS_WORLD_UP_TYPE_OBJECT_UP;
			break;
		case 2:
			flags |= TupLookAt::FLAGS_WORLD_UP_TYPE_OBJECT_ROTATION_UP;
			break;
		case 3:
			flags |= TupLookAt::FLAGS_WORLD_UP_TYPE_VECTOR;
			break;
		case 4:
			flags |= TupLookAt::FLAGS_WORLD_UP_TYPE_NONE;
			break;
	}

	if (MayaHelper::GetBoolAttrib(lookAtNode,"displayConnector"))
	{
		flags |= TupLookAt::FLAGS_DISPLAY_CONNECTOR;
	}

	tupLookAt.AddFlags(flags);

}


void TupExport::AddUnknownObject(TupperwareAggregate *pObjectAgg,MDagPath mDagPath)
{
	MObject node = mDagPath.node();
	pObjectAgg->AddScalarString("MayaObjectType",NULL,node.apiTypeStr());
}

MDagPath TupExport::GetPreBoundMesh(MDagPath meshPath)
{
	MStatus status;

	MFnDependencyNode fnMeshNode(meshPath.node());
	int objectPtr = (int)MayaHelper::GetMPtrBase(fnMeshNode);
	
	// check if this mesh has any bones attached to it
	int skinIndex = m_pSkinManager->GetSkinIndex(objectPtr);
	if (skinIndex!=-1)
	{
		// if we are bound to a set of bones (we need to go find the original mesh)
		MPlug inputMeshPlug = fnMeshNode.findPlug("inMesh",&status);
		if (!status)
			return meshPath;
		MPlugArray conns;
		if (inputMeshPlug.connectedTo(conns,true,false) && conns.length()) 
		{
			MObject testObject = conns[0].node();
			bool bFound = false;
			while (!bFound)
			{
				switch (testObject.apiType())
				{
					case MFn::kJointCluster:
						{
							MFnDependencyNode fnTestNode(testObject);
//							TupString testNodeName = fnTestNode.name().asChar();
							MPlug inputPlug = fnTestNode.findPlug("input",&status);
							if (!status)
								return meshPath;

							MPlug inputElementPlug = inputPlug.elementByLogicalIndex(0,&status);
							if (!status)
								return meshPath;

							MPlug inputGeometryPlug = inputElementPlug.child(0,&status);

							MPlugArray conns;
							if (inputGeometryPlug.connectedTo(conns,true,false) && conns.length())
							{
								testObject = conns[0].node();
							}
							else
							{
								return meshPath;
							}
						}
						break;
					case MFn::kPolyBlindData:
						{
							MFnDependencyNode fnTestNode(testObject);
//							TupString testNodeName = fnTestNode.name().asChar();
							MPlug inputGeometryPlug = fnTestNode.findPlug("inMesh",&status);
							if (!status)
								return meshPath;
							MPlugArray conns;
							if (inputGeometryPlug.connectedTo(conns,true,false) && conns.length())
							{
								testObject = conns[0].node();
							}
							else
							{
								return meshPath;
							}
						}
						break;
					case MFn::kTweak:
					case MFn::kGroupParts:
						{
							MFnDependencyNode fnTestNode(testObject);
//							TupString testNodeName = fnTestNode.name().asChar();
							MPlug inputGeometryPlug = fnTestNode.findPlug("inputGeometry",&status);
							if (!status)
								return meshPath;
							MPlugArray conns;
							if (inputGeometryPlug.connectedTo(conns,true,false) && conns.length())
							{
								testObject = conns[0].node();
							}
							else
							{
								return meshPath;
							}
						}
						break;
					case MFn::kPolyColorPerVertex:
						{
							MFnDependencyNode fnTestNode(testObject);
//							TupString testNodeName = fnTestNode.name().asChar();
							MPlug inputGeometryPlug = fnTestNode.findPlug("inputPolymesh",&status);
							if (!status)
								return meshPath;
							MPlugArray conns;
							if (inputGeometryPlug.connectedTo(conns,true,false) && conns.length())
							{
								testObject = conns[0].node();
							}
							else
							{
								return meshPath;
							}
						}
						break;
					case MFn::kMesh:
						{
							bFound = true;
						}
						break;
					default:
						return meshPath; // if we hit a snag then kick back original
				}
			}
			MDagPath newPath = meshPath;
			newPath.pop();
			status = newPath.push(testObject);
			if (!status)
				return meshPath;
//			TupString newPathString = newPath.fullPathName().asChar();
			return newPath;
		}
	}
	return meshPath; // don't need to change it or can't find it
}


#if 0
// set tests
	TupperwareAggregate *pSetsAgg = pObjectAgg->AddAggregate("Sets",NULL);

	MObjectArray sets;
	MObjectArray components;
	fnMesh.getConnectedSetsAndMembers(instanceIndex,sets,components,false);

	int numSets = sets.length();
	pSetsAgg->AddScalarInt("NumSets",NULL,numSets);
	int setIndex;
	for (setIndex=0;setIndex<numSets;setIndex++)
	{
		char string[10];
		itoa(setIndex,string,10);
		TupperwareAggregate *pSetAgg = pSetsAgg->AddAggregate("Set",string);

		MFnSingleIndexedComponent component(components[setIndex]);
		int numElements = component.elementCount();
		pSetAgg->AddScalarInt("Component Element Count",NULL,numElements);
		TupArray<int> elementArray;
		for (int elementIndex=0;elementIndex<numElements;elementIndex++)
		{
			elementArray.Add(component.element(elementIndex));
		}
		if (numElements)
		{
			pSetAgg->AddListInt("Elements",NULL,elementArray);
		}

		MFnSet set(sets[setIndex]);
		MSelectionList selectionList;
		set.getMembers(selectionList,false);

		int selectionListSize = selectionList.length();
		pSetAgg->AddScalarInt("SelectionListSize",NULL,selectionListSize);
		int selectionListIndex;
		for (selectionListIndex=0;selectionListIndex<selectionListSize;selectionListIndex++)
		{
			char string[10];
			itoa(setIndex,string,10);
			TupperwareAggregate *pSelectionAgg = pSetAgg->AddAggregate("Selection",string);

			MDagPath dagPath;
			selectionList.getDagPath(selectionListIndex,dagPath,components[setIndex]);
			pSelectionAgg->AddScalarString("DagPath",NULL,dagPath.fullPathName().asChar());
			MPlug mPlug;
			selectionList.getPlug(selectionListIndex,mPlug);
			MObject depNodeObject;
			selectionList.getDependNode(selectionListIndex,depNodeObject);
			MFnDependencyNode depNode(depNodeObject);

			UnknownObject::AddPlug(depNode,mPlug,pSelectionAgg);
		}
	}
#endif
