/**********************************************************************
 *<
	FILE: mffio.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "mffio.h"
#include "intmap.h"
#include "intintmap.h"
#include "decomp.h"

#include <string>
#include <vector>

#define DEGTORAD  (3.14 / 180.0)
#define RADTODEG  (180.0 / 3.14)

#define MAX_MESHMAPS 100

static MffioClassDesc MffioDesc;
ClassDesc2* GetMffioDesc() { return &MffioDesc; }


BOOL CALLBACK MffioOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static Mffio *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (Mffio *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}


//--- Mffio -------------------------------------------------------
Mffio::Mffio()
{

}

Mffio::~Mffio() 
{

}

int Mffio::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *Mffio::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("MFF");
}

const TCHAR *Mffio::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Max/Maya Exchange");
}
	
const TCHAR *Mffio::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("Max/Maya Exchange");
}

const TCHAR *Mffio::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Avalanche Software");
}

const TCHAR *Mffio::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("Copyright 2002 Avalanche Software");
}

const TCHAR *Mffio::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *Mffio::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int Mffio::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void Mffio::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL Mffio::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}



int	Mffio::DoExport(const TCHAR *filename,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

//	if(!suppressPrompts)
//		DialogBoxParam(hInstance, 
//				MAKEINTRESOURCE(IDD_PANEL), 
//				GetActiveWindow(), 
//				MffioOptionsDlgProc, (LPARAM)this);

	m_mff = new mffTools;

	BOOL exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	IntMap imap;

	if (exportSelected)
	{
		int nodeCount = i->GetSelNodeCount();
		int nodeIndex;
		// add nodes to pools as well as children nodes (don't fill in yet)
		for (nodeIndex=0;nodeIndex<nodeCount;nodeIndex++)
		{
			polys.clear();
			edges.clear();
			m_matmap.Clear();

			INode *pSelectedNode = i->GetSelNode(nodeIndex);
			if (!pSelectedNode)
				continue;

			Object *pObject = pSelectedNode->GetObjectRef();

			if (!imap.HasKey((int)pObject))
			{
				int refidx = ExportNode(pSelectedNode,i);
				imap.SetValue((int)pObject,refidx);
			}
			int refidx = imap.GetValue((int)pObject);
		if (pObject->SuperClassID() == GEOMOBJECT_CLASS_ID)
				ExportPolyInstance(refidx,pSelectedNode);
		else if (pObject->SuperClassID() == LIGHT_CLASS_ID)
				ExportLightInstance(refidx,pSelectedNode);
		}
	}
	else
	{
		INode *pRootNode = i->GetRootNode();

		int nodeCount = pRootNode->NumChildren();
		int nodeIndex;
		for (nodeIndex=0;nodeIndex<nodeCount;nodeIndex++)
		{
			polys.clear();
			edges.clear();
			m_matmap.Clear();

			INode *pChildNode = pRootNode->GetChildNode(nodeIndex);
			if (!pChildNode) 
				continue;

			const char *debug = pChildNode->GetName();

			Object *pObject = pChildNode->GetObjectRef();

			if (!imap.HasKey((int)pObject))
			{
				int refidx = ExportNode(pChildNode,i);
				imap.SetValue((int)pObject,refidx);
			}
			int refidx = imap.GetValue((int)pObject);

			if (pObject->SuperClassID() == GEOMOBJECT_CLASS_ID)
				ExportPolyInstance(refidx,pChildNode);
			else if (pObject->SuperClassID() == LIGHT_CLASS_ID)
				ExportLightInstance(refidx,pChildNode);
		}
	}

	int success = m_mff->ExportFile((char*)filename);
	
	delete m_mff;

	return TRUE;
}

double GetUnitRatio()
{
	return GetMasterScale(UNITS_CENTIMETERS);
}

// flag: 0 - object, 1 - light
void Mffio::ExportTransform(INode *pNode,mffTransform &transform,int flag)
{
	double scale = GetUnitRatio();

	Matrix3 tm = pNode->GetObjTMAfterWSM(0);

//	Matrix3 parenttm = Inverse(pNode->GetParentTM(0));

//	Matrix3 tm = pNode->GetObjTMAfterWSM(0) * parenttm;

	Point3 p = tm.GetTrans();

	const float *pSrcMatrix = (const float *)tm.GetAddr();

	transform.translate.x = p.x * scale;
	transform.translate.y = p.z * scale;
	transform.translate.z = -p.y * scale;

	AffineParts parts;
	decomp_affine(tm, &parts);
	
	Matrix3 rotmat;
	rotmat.IdentityMatrix();
	
	RotateMatrix(rotmat,parts.q);

	float rotx = 0.0f,roty = 0.0f,rotz = 0.0f;

	rotmat.GetYawPitchRoll(&rotx,&roty,&rotz);

	if (flag == 1)
	{
		transform.rotate.x = (RADTODEG * roty)-90.0f;
		transform.rotate.y = RADTODEG * rotz;
		transform.rotate.z = RADTODEG * -rotx;
	}
	else
	{
		transform.rotate.x = RADTODEG * roty;
		transform.rotate.y = RADTODEG * rotz;
		transform.rotate.z = RADTODEG * -rotx;
	}

	Matrix3 stm = ScaleMatrix(parts.k);

	pSrcMatrix = (const float *)stm.GetAddr();

	transform.scale.x = pSrcMatrix[0];
	transform.scale.z = pSrcMatrix[4];
	transform.scale.y = pSrcMatrix[8];
}

int Mffio::ExportPolyInstance(int refidx,INode *pNode)
{

	mffGroupTransform group; 

	strcpy(group.m_name,(char*)pNode->GetName());

	ExportTransform(pNode,group.m_transform);

	std::vector<int> attrindicies;

	int index = m_mff->AddGroupTransformInstance(-1,group,attrindicies);

	mffPolyModelInstance polymodelinstance;

	strcpy(polymodelinstance.m_name,(char*)pNode->GetName());

	polymodelinstance.m_parentindex = index;

	polymodelinstance.m_refidx = refidx;

	mffNode *node = m_mff->GetInstanceNode(index);

	std::vector<int> attrindicies2;

	index = m_mff->AddPolyModelInstance(polymodelinstance,attrindicies2);

	node->m_childindicies.push_back(index);

	return 1;
}

int Mffio::ExportLightInstance(int refidx,INode *pNode)
{

	mffGroupTransform group; 

	strcpy(group.m_name,(char*)pNode->GetName());

	ExportTransform(pNode,group.m_transform,1);

	std::vector<int> attrindicies;

	int index = m_mff->AddGroupTransformInstance(-1,group,attrindicies);

	mffLightInstance *lightinstance = new mffLightInstance;

	strcpy(lightinstance->m_name,(char*)pNode->GetName());

	lightinstance->m_parentindex = index;

	lightinstance->m_refidx = refidx;

	mffNode *node = m_mff->GetInstanceNode(index);

	lightinstance->m_index = m_mff->m_instances.size();

	m_mff->m_instances.push_back(lightinstance);

	index = m_mff->m_instances.size()-1;	

	node->m_childindicies.push_back(index);

	return 1;
}

int Mffio::ExportNode(INode *pNode,Interface *pInterface)
{
	int index = -1;

	Object *pObject = pNode->GetObjectRef();

	bool found = false;
	switch(pObject->SuperClassID()) 
	{
		case SYSTEM_CLASS_ID:
			// XRef object
			if (pObject->ClassID() == Class_ID(XREFOBJ_CLASS_ID,0))
			{
//				IXRefObject *pXRef = (IXRefObject *)pObject;
			}
			break;

		// geometry
		case GEOMOBJECT_CLASS_ID: 
			// check if it a bone...
			if (pObject->ClassID() == BONE_OBJ_CLASSID)
			{
			}
			else if (pObject->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
			{

				mffPolyModel polymodel;

				strcpy(polymodel.m_name,(char*)pNode->GetName());

				Mtl *pMaterial = pNode->GetMtl();
				ExportMaterial(pMaterial,(mffModel*)&polymodel);
	

				TriObject *pTri = (TriObject *) pObject->ConvertToType(pInterface->GetTime(), Class_ID(TRIOBJ_CLASS_ID, 0));
				ExportPolygonMesh(&pTri->GetMesh(),pInterface,&polymodel);
				index = m_mff->AddPolyModel(polymodel);

				if (pObject != pTri) // delete if it was just created
					delete pTri;

				return index;
			}
			break;
		case LIGHT_CLASS_ID:
				GenLight *pLight = (GenLight *)pObject;

				mffLight light;

				if (pLight->Type() == OMNI_LIGHT)
					light.m_type = mffLight::omni;
				else if (pLight->Type() == TSPOT_LIGHT)
					light.m_type = mffLight::spot;
				else if (pLight->Type() == FSPOT_LIGHT)
					light.m_type = mffLight::spot;
				else// (pLight->Type() == DIR_LIGHT)
					light.m_type = mffLight::directional;

				Point3 c;

				c = pLight->GetRGBColor(0);

				light.m_color = mffColor(c.x,c.y,c.z,1.0);

				light.m_intensity = pLight->GetIntensity(0);

				strcpy(light.m_name,(char*)pNode->GetName());
				index = m_mff->AddLight(light);

				return index;
			break;

	}
	return 0;
}

mffVector ConvVector(Point3 point,float scale)
{
	mffVector temp;
	temp.x = (float)point.x*scale;
	temp.y = (float)point.z*scale;
	temp.z = (float)-point.y*scale;
	return temp;
}

int Mffio::ExportMaterial(Mtl *pMaterial,mffModel *model)
{
	if (!pMaterial) return 0;

	if (pMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) 
	{
		GetStandardMaterial((StdMat2*)pMaterial,model);

	}
	else if (pMaterial->ClassID() == Class_ID(MULTI_CLASS_ID, 0)) 
	{
		GetMultiSubMaterial((MultiMtl*)pMaterial,model);
	}
	else
	{
		return 0;
	}

	return 1;
}

int Mffio::GetStandardMaterial(StdMat2 *pMaterial,mffModel *model)
{
	if (!m_matmap.HasKey((int)pMaterial))
	{
		mffShader shader;

		strcpy(shader.m_name,pMaterial->GetName());

		Color diffuse = pMaterial->GetDiffuse(0);

		shader.m_color = mffColor(diffuse.r,diffuse.g,diffuse.b,1.0);
		
		Texmap *texmap = (BitmapTex*)pMaterial->GetSubTexmap(ID_DI);
		if (texmap)
			GetMap(texmap,shader);

		model->m_shaders.push_back(shader);
		m_matmap.SetValue((int)pMaterial,model->m_shaders.size()-1);
	}
	return 1;
}

int Mffio::GetMultiSubMaterial(MultiMtl *pMaterial,mffModel *model)
{

	int numMaterials = pMaterial->NumSubMtls();
	for (int i=0;i<numMaterials;i++)
	{
		Mtl *subMaterial = pMaterial->GetSubMtl(i);
		if (subMaterial)
		{
			GetStandardMaterial((StdMat2*)subMaterial,model);
		}
	}
	return 1;
}

int Mffio::GetMap(Texmap *pMap,mffShader &shader)
{
	Class_ID classID = pMap->ClassID();

	if (classID == Class_ID(BMTEX_CLASS_ID, 0)) 
	{
		GetBitmapMap((BitmapTex*)pMap,shader);
	}
	else if (classID == Class_ID(COMPOSITE_CLASS_ID, 0)) 
	{
		GetCompositeMap((MultiTex*)pMap,shader);
	}
	else if (classID == Class_ID(MIX_CLASS_ID, 0))
	{
		GetMixMap((MultiTex*)pMap,shader);
	}
	return 1;
}

int Mffio::GetBitmapMap(BitmapTex *pBitmapTex,mffShader &shader)
{
	mffStd *standard = new mffStd;

	mffMap *pMap = &standard->m_map;

	char *mapname = pBitmapTex->GetMapName();

	if (mapname)
		strcpy(pMap->m_filename,(char*)pBitmapTex->GetMapName());
	else
		strcpy(pMap->m_filename,"untitled");
	
	StdUVGen *uv = pBitmapTex->GetUVGen();

	pMap->m_repeat.x = uv->GetUScl(0);
	pMap->m_repeat.y = uv->GetVScl(0);

	int flags = uv->GetTextureTiling();

	if (flags & U_WRAP)
		pMap->m_wrapu = 1;
	if (flags & V_WRAP)
		pMap->m_wrapv = 1;
	
	shader.m_colorMap = standard;

	return 1;
}

int Mffio::GetCompositeMap(MultiTex *pMultiTex,mffShader &shader)
{
	int numMaps = pMultiTex->NumSubTexmaps();
	for (int i=0;i<numMaps; i++) 
	{
		Texmap *texMap = pMultiTex->GetSubTexmap(i);
		if (texMap)
		{
			if (!shader.m_colorMap) // do only if one is not set yet
				GetMap(texMap,shader);
		}
	}
	return 1;
}

int Mffio::GetMixMap(MultiTex *pMultiTex,mffShader &shader)
{
	Texmap *texMap1 = pMultiTex->GetSubTexmap(0);
	if (texMap1 && !shader.m_colorMap)
	{
		GetMap(texMap1,shader);
	}
	Texmap *texMap2 = pMultiTex->GetSubTexmap(1);
	if (texMap2 && !shader.m_colorMap)
	{
		GetMap(texMap2,shader);
	}
	return 1;
}

int Mffio::ExportPolygonMesh(Mesh *mesh,Interface *pInterface,mffPolyModel *polymodel)
{
	double scale = GetUnitRatio();
	int numVerts,numFaces;
	Point3 vertex;

	numVerts = mesh->getNumVerts();
	numFaces = mesh->getNumFaces();

	mesh->buildNormals();

	mffVector v;

//	Add Vertex Pool
	for (unsigned i = 0; i < numVerts; i++)
	{
		vertex = mesh->getVert(i);
		v = ConvVector(vertex,scale);
		polymodel->m_VertexArray.push_back(v);
	}

	Point3 faceNormal,vertexNormal;
	
	IntIntMap edgemap;

	for (i = 0; i < numFaces; i++) 
	{
		int vert0,vert1,vert2;

		Point3 faceNormal;  // Face normal
		Point3 vertexNormal;  // Vertex normal
		Face &face = mesh->faces[i];
		tPoly poly;

//		faceNormal = mesh->getFaceNormal(i);
//		polymodel->m_FaceNormalArray.push_back(mffVector(faceNormal.x,faceNormal.y,faceNormal.z));		
//		poly.facenormalindex = polymodel->m_FaceNormalArray.size()-1;

		vert0 = face.getVert(0);
//		vertexNormal = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert0));
//		polymodel->m_VertexNormalArray.push_back(mffVector(vertexNormal.x,vertexNormal.y,vertexNormal.z));
		poly.nindicies.push_back(polymodel->m_VertexNormalArray.size()-1);

		vert1 = face.getVert(1);
//		vertexNormal = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert1));
//		polymodel->m_VertexNormalArray.push_back(mffVector(vertexNormal.x,vertexNormal.y,vertexNormal.z));
//		poly.nindicies.push_back(polymodel->m_VertexNormalArray.size()-1);

		vert2 = face.getVert(2);
//		vertexNormal = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert2));
//		polymodel->m_VertexNormalArray.push_back(mffVector(vertexNormal.x,vertexNormal.y,vertexNormal.z));
//		poly.nindicies.push_back(polymodel->m_VertexNormalArray.size()-1);

		poly.vindicies.push_back(vert0);
		poly.vindicies.push_back(vert1);
		poly.vindicies.push_back(vert2);

		int evis0 = face.getEdgeVis(0);
		int evis1 = face.getEdgeVis(1);
		int evis2 = face.getEdgeVis(2);

		if (!edgemap.HasKey(vert0,vert1))
		{
			tEdge edge;
			edge.v0 = vert0;
			edge.v1 = vert1;
			edge.vis = evis0;
			edge.adjedgeindex = -1;
			edge.polyindex = i;
			edge.reledgeindex = 0;
			edges.push_back(edge);
			poly.eindicies.push_back(edges.size()-1);
			if (!evis0)
			{
				edgemap.SetValue(vert1,vert0,edges.size()-1);
				edgemap.SetValue(vert0,vert1,edges.size()-1);
			}
		}
		else
		{
			int edgeidx = edgemap.GetValue(vert0,vert1);
			tEdge edge;
			edge.v0 = vert0;
			edge.v1 = vert1;
			edge.vis = evis0;
			edge.adjedgeindex = edgeidx;
			edge.polyindex = i;
			edge.reledgeindex = 0;
			tEdge *adjedge = &edges[edgeidx];
			adjedge->adjedgeindex = edges.size();
			edges.push_back(edge);
			poly.eindicies.push_back(edges.size()-1);
		}

		if (!edgemap.HasKey(vert1,vert2))
		{
			tEdge edge;
			edge.v0 = vert1;
			edge.v1 = vert2;
			edge.vis = evis1;
			edge.adjedgeindex = -1;
			edge.polyindex = i;
			edge.reledgeindex = 1;
			edges.push_back(edge);
			poly.eindicies.push_back(edges.size()-1);
			if (!evis1)
			{
				edgemap.SetValue(vert1,vert2,edges.size()-1);
				edgemap.SetValue(vert2,vert1,edges.size()-1);
			}
		}
		else
		{
			int edgeidx = edgemap.GetValue(vert1,vert2);
			tEdge edge;
			edge.v0 = vert1;
			edge.v1 = vert2;
			edge.vis = evis1;
			edge.adjedgeindex = edgeidx;
			edge.polyindex = i;
			edge.reledgeindex = 1;
			tEdge *adjedge = &edges[edgeidx];
			adjedge->adjedgeindex = edges.size();
			edges.push_back(edge);
			poly.eindicies.push_back(edges.size()-1);
		}

		if (!edgemap.HasKey(vert2,vert0))
		{
			tEdge edge;
			edge.v0 = vert2;
			edge.v1 = vert0;
			edge.vis = evis2;
			edge.adjedgeindex = -1;
			edge.polyindex = i;
			edge.reledgeindex = 2;
			edges.push_back(edge);
			poly.eindicies.push_back(edges.size()-1);
			if (!evis2)
			{
				edgemap.SetValue(vert0,vert2,edges.size()-1);
				edgemap.SetValue(vert2,vert0,edges.size()-1);
			}
		}
		else
		{
			int edgeidx = edgemap.GetValue(vert2,vert0);
			tEdge edge;
			edge.v0 = vert2;
			edge.v1 = vert0;
			edge.vis = evis2;
			edge.adjedgeindex = edgeidx;
			edge.polyindex = i;
			edge.reledgeindex = 2;
			tEdge *adjedge = &edges[edgeidx];
			adjedge->adjedgeindex = edges.size();
			edges.push_back(edge);
			poly.eindicies.push_back(edges.size()-1);
		}

		int shaderindex = (face.flags>>FACE_MATID_SHIFT)&FACE_MATID_MASK;

		if (poly.shaderindex > polymodel->m_shaders.size())
		{
//			MessageBox(GetActiveWindow(), "Shader index mismatch.","", MB_OK);
//			return 0;
		}
		
		poly.shaderindex = shaderindex % polymodel->m_shaders.size();

		polys.push_back(poly);
	}

	mffUVMap mffuvmap;
// loop for multiple maps
//	for (int mapIndex = 0; mapIndex < MAX_MESHMAPS-1; mapIndex++)

		if (mesh->mapSupport(1)) 
		{
			MeshMap &meshMap = mesh->Map(1);
			int numMapVerts = meshMap.getNumVerts();
			int numMapFaces = meshMap.getNumFaces();
			if (numMapVerts || numMapFaces)
			{
				Point3 tp;
				for (i = 0; i < numMapFaces; i++)
				{
						tp = meshMap.tv[meshMap.tf[i].t[0]];
						mffuvmap.m_UVArray.push_back(mffVector2(tp.x,tp.y));
						polys[i].uvindicies.push_back((mffuvmap.m_UVArray.size()-1));

						tp = meshMap.tv[meshMap.tf[i].t[1]];
						mffuvmap.m_UVArray.push_back(mffVector2(tp.x,tp.y));
						polys[i].uvindicies.push_back((mffuvmap.m_UVArray.size()-1));

						tp = meshMap.tv[meshMap.tf[i].t[2]];
						mffuvmap.m_UVArray.push_back(mffVector2(tp.x,tp.y));
						polys[i].uvindicies.push_back((mffuvmap.m_UVArray.size()-1));
				}
			}
		}

	std::vector<tPoly> newpolys;

//	for (i = 0; i < polys.size(); i++)
//	{
//		if (LegalPoly(&polys[i],edges,polymodel->m_VertexArray))
//			newpolys.push_back(polys[i]);
//	}

//	polys = newpolys;

	newpolys.clear();

	for (i = 0; i < polys.size(); i++)
	{
		if (!polys[i].visited)
		{
			tPoly newpoly = polys[i];
			EvalPoly(&polys[i],newpoly);
			newpolys.push_back(newpoly);
		}
	}

	polys = newpolys;

	// create tables for mff interface
	for (i = 0; i < polys.size(); i++)
	{
		tPoly *pPoly = &polys[i];

		numVerts = pPoly->vindicies.size();

		polymodel->m_VertexCounts.push_back(numVerts);

		polymodel->m_ShaderIndicies.push_back(pPoly->shaderindex);

		for (int j = 0; j < numVerts; j++)
		{
			polymodel->m_PolyVertexIndicies.push_back(pPoly->vindicies[j]);
			if (polymodel->m_VertexNormalArray.size()>0)
				polymodel->m_VertexNormalIndicies.push_back(pPoly->nindicies[j]);
			if (mffuvmap.m_UVArray.size()>0)
				mffuvmap.m_PolyUVIndicies.push_back(pPoly->uvindicies[j]);
		}
	}

	polymodel->m_UVMaps.push_back(mffuvmap);

	return 1;
}

int Mffio::EvalPoly(tPoly *poly,tPoly &newpoly)
{
	for (int i = 0; i < poly->eindicies.size(); i++)
	{
		tEdge *edge = &edges[poly->eindicies[i]];
		if (!edge->vis)
		{
			int test = CombinePolyAtEdge(edge,poly,newpoly);
			if (!test)
				return 0;

			tPoly temp = newpoly;

			for (int j = 0; j < temp.eindicies.size(); j++)
			{	
				tEdge *e = &edges[temp.eindicies[j]];
			}
			
			EvalPoly(&temp,newpoly);

			poly->visited = 1;

			return 1;
		}
	}

	poly->visited = 1;
		
	return 1;
}

int Mffio::CombinePolyAtEdge(tEdge *edge,tPoly *poly0,tPoly &newpoly)
{
	int index;

	tPoly combinedpoly;

	tEdge *adjedge = &edges[edge->adjedgeindex];
	tPoly *poly1 = &polys[adjedge->polyindex];

	if (poly0->shaderindex != poly1->shaderindex)
		return 0;

	if (poly1->vindicies.size()==0)
		return 0;

	int eidx0 = edge->reledgeindex;
	int eidx1 = adjedge->reledgeindex;

	if (poly1->visited)
		return 1;

	int cnt = 0;

	for (int i = 0; i < poly0->eindicies.size()-1; i++)
	{
		index = (eidx0+1)+i;
		if ( index >= poly0->eindicies.size() )
			index = index - (poly0->eindicies.size());

		tEdge *edge0 = &edges[poly0->eindicies[index]];

		edge0->reledgeindex = cnt++;

		combinedpoly.vindicies.push_back(poly0->vindicies[index]);
		combinedpoly.uvindicies.push_back(poly0->uvindicies[index]);
		combinedpoly.eindicies.push_back(poly0->eindicies[index]);
	}

	for (i = 0; i < poly1->eindicies.size()-1; i++)
	{
		index = (eidx1+1)+i;
		if ( index >= poly1->eindicies.size() )
			index = index - (poly1->eindicies.size());

		tEdge *edge0 = &edges[poly1->eindicies[index]];

		edge0->reledgeindex = cnt++;

		combinedpoly.vindicies.push_back(poly1->vindicies[index]);
		combinedpoly.uvindicies.push_back(poly1->uvindicies[index]);
		combinedpoly.eindicies.push_back(poly1->eindicies[index]);
	}

	for (i = 0; i < combinedpoly.eindicies.size(); i++)
	{
		for (int j = 0; j < combinedpoly.eindicies.size(); j++)
		{
			tEdge *edge0 = &edges[combinedpoly.eindicies[i]];
			tEdge *edge1 = &edges[combinedpoly.eindicies[j]];

			if (i != j)
			{
				if (EdgesEqual(edge0,edge1))
				{
					std::vector<int> eindicies,vindicies,uvindicies;

					for (int k = 0; k < combinedpoly.eindicies.size(); k++)
					{
						if ((k != i)&&(k!=j))
						{
							eindicies.push_back(combinedpoly.eindicies[k]);
							vindicies.push_back(combinedpoly.vindicies[k]);
							uvindicies.push_back(combinedpoly.uvindicies[k]);
						}
					}
					combinedpoly.eindicies = eindicies;
					combinedpoly.vindicies = vindicies;
					combinedpoly.uvindicies = uvindicies;
				}
			}
		}

	}

	combinedpoly.shaderindex = poly0->shaderindex;

	newpoly = combinedpoly;

	poly0->visited = 1;
	poly1->visited = 1;

	return 1;
}

int Mffio::EdgesEqual(tEdge *edge0, tEdge *edge1)
{
	if ((edge0->v0 == edge1->v0)&&(edge0->v1 == edge1->v1)||
		(edge0->v0 == edge1->v1)&&(edge0->v1 == edge1->v0))
		return 1;
	return 0;
}

double Mffio::GetEdgeLength(tEdge *e,std::vector<mffVector> &positions)
{
	double x,y,z;

	x = positions[e->v1].x - positions[e->v0].x;
	y = positions[e->v1].y - positions[e->v0].y;
	z = positions[e->v1].z - positions[e->v0].z;

	return (x*x + y*y + z*z);
}

int Mffio::LegalPoly(tPoly *poly,std::vector<tEdge> &edges,std::vector<mffVector> &positions)
{
	tEdge *e0 = &edges[poly->eindicies[0]];
	tEdge *e1 = &edges[poly->eindicies[1]];
	tEdge *e2 = &edges[poly->eindicies[2]];

	if ((GetEdgeLength(e0,positions) > 0.0)&&(GetEdgeLength(e1,positions) > 0.0)&&(GetEdgeLength(e2,positions) > 0.0))
		return 1;
	return 0;
}

Point3 Mffio::GetVertexNormal(Mesh *mesh, int faceNo,RVertex* rv)
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
