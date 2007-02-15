/**********************************************************************
 *<
	FILE: mffio.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/
#pragma warning (disable : 4530)
#pragma warning (disable : 4786)



#include "mffio.h"
#include "decomp.h"
#include "applyvc.h"
//#include <string>
#include "String.h"
#include <splshape.h>

#define DEGTORAD  (3.14f / 180.0f)
#define RADTODEG  (180.0f / 3.14f)

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

int Mffio::DoImport(const TCHAR *filename,ImpInterface *i,
						Interface *gi, BOOL suppressPrompts)
{
	//TODO: Implement the actual file import here and 
	//		return TRUE If the file is imported properly

//	if(!suppressPrompts)
//		DialogBoxParam(hInstance, 
//				MAKEINTRESOURCE(IDD_PANEL), 
//				GetActiveWindow(), 
//				MffioOptionsDlgProc, (LPARAM)this);

	m_mff = new mffTools;

	if (!m_mff->ImportFile((char*)filename))
	{
		delete m_mff; 
		return FALSE;
	};

	int numRefs = m_mff->GetNumReferences();
	m_refNodes.resize(numRefs);
	m_refObjects.resize(numRefs);
	for (int index=0;index<numRefs;index++)
	{
		m_refNodes[index] = NULL;
		m_refObjects[index] = NULL;
	}

	IntMap nodemap;
	std::vector<INode*> nodes;
	std::vector<int> attrindicies;

	for (unsigned j = 0; j < m_mff->GetNumInstances(); j++)
	{
		mffNode *node = m_mff->GetInstanceNode(j);

		while (node->m_parentindex>=0)
		{
			node = m_mff->GetInstanceNode(node->m_parentindex);
		}
		if (!nodemap.HasKey((int)node))
		{
			nodes.clear();
			CreateBranch(gi,node,NULL,nodes,attrindicies);
			nodemap.SetValue((int)node,1);
		}
	}

	delete m_mff;

	return TRUE;
}

void Mffio::CreateBranch(Interface *mainI,mffNode *node,mffNode *parent,std::vector <INode*> &nodes,std::vector<int> attrindicies)
{
	if (node->GetNodeType() == kMODELINSTANCE)
	{
		mffModelInstance *model = (mffModelInstance *)node;
		if (model->GetModelType() == kPOLYGONMODELINSTANCE)
		{
			INode *inode = NULL;
			mffPolyModelInstance *polymodelinstance = (mffPolyModelInstance *)node;

			if (!polymodelinstance)
				return;

			mffPolyModel *polymodel = (mffPolyModel *)m_mff->GetPolyModel(polymodelinstance->m_refidx);
			if (polymodel)
			{
				// do the m_refNodes in this routine...
				inode = CreatePolygonModel(polymodel,polymodelinstance,mainI);
			}

			Matrix3 tm;
			tm.IdentityMatrix();

			mffGroupTransformInstance *groupinstance;

			if (parent)
				groupinstance = (mffGroupTransformInstance *)parent;

			TSTR name = groupinstance->m_name;

			inode->SetName(name);

			Control *tmc = NewDefaultMatrix3Controller();
			inode->SetTMController(tmc);
			inode->SetNodeTM(0,tm);
				
			nodes.push_back(inode);

		}
		else if (model->GetModelType() == kCURVEMODELINSTANCE)
		{
			INode *inode = NULL;
			mffCurveModelInstance *curvemodelinstance = (mffCurveModelInstance *)node;

			if (!curvemodelinstance)
				return;

			mffCurveModel *curvemodel = (mffCurveModel *)m_mff->GetReferenceNode(curvemodelinstance->m_refidx);
			if (curvemodel)
			{
				// do the m_refNodes in this routine...				
				inode = CreateCurve(curvemodel,mainI);
			}

			Matrix3 tm;
			tm.IdentityMatrix();

			mffGroupTransformInstance *groupinstance;

			if (parent)
				groupinstance = (mffGroupTransformInstance *)parent;

			TSTR name = groupinstance->m_name;

			inode->SetName(name);

			Control *tmc = NewDefaultMatrix3Controller();
			inode->SetTMController(tmc);
			inode->SetNodeTM(0,tm);
				
			nodes.push_back(inode);

		}
	}
	else if (node->GetNodeType() == kGROUPTRANSFORMINSTANCE)
	{
		if (node->m_childindicies.size()>0)
		{
			if (attrindicies.size()==0)
				if (node->m_attrindicies.size()>0)
					attrindicies = node->m_attrindicies;

			int numChildren = node->m_childindicies.size();
			for (unsigned i = 0; i < numChildren; i++)
			{
				std::vector <INode*> a;
				CreateBranch(mainI,m_mff->GetInstanceNode(node->m_childindicies[i]),node,a,attrindicies);

				for (unsigned i = 0; i < a.size(); i++)
					nodes.push_back(a[i]);
			}

			if (nodes.size()==0)
				return;

//			INodeTab nodetab;
//			for (int j = 0; j < nodes.size(); j++)
//			{
//				const char*debug = nodes[j]->GetName();
//				nodetab.Append(1,&nodes[j]);
//			}					
//			INode *inode = mainI->GroupNodes(&nodetab,&str,FALSE);

				Matrix3 tm;
				tm.IdentityMatrix();

				mffGroupTransformInstance *groupinstance = (mffGroupTransformInstance *)node;
				tm = GetTransformMatrix(m_mff->GetGroupTransform(groupinstance->m_refidx)->m_transform);
				
				unsigned numNodes = nodes.size();
				for (unsigned j = 0; j < nodes.size(); j++)
				{
					Matrix3 tm2;
					tm2 = nodes[j]->GetNodeTM(0);
					tm2 *= tm;
					nodes[j]->SetNodeTM(0,tm2);

					TSTR test = nodes[j]->GetName();

					int numAttributes = groupinstance->m_attrindicies.size();

					if (numAttributes>0)
					{
						for (unsigned k = 0; k < numAttributes; k++)
						{
							AddAttribute(m_mff->GetAttribute(groupinstance->m_attrindicies[k]),nodes[j]);
						}
					}
				}
		}
	}
	else if (node->GetNodeType() == kLIGHTINSTANCE)
	{
		GenLight *object;

		mffLightInstance *lightinstance = (mffLightInstance*)node;
		mffLight *light = (mffLight*)m_mff->GetReferenceNode(lightinstance->m_refidx);

		INode *inode;

		if (m_refObjects[lightinstance->m_refidx]==NULL)
		{
			if (light->m_type == mffLight::omni)
			{
				object = mainI->CreateLightObject(OMNI_LIGHT);
			}
			else if (light->m_type == mffLight::spot)
			{
				object = mainI->CreateLightObject(FSPOT_LIGHT);
			}
			else
			{
				object = mainI->CreateLightObject(DIR_LIGHT);
			}

			object->SetUseLight(1);
			object->Enable(1);

			inode = mainI->CreateObjectNode(object);

			m_refObjects[lightinstance->m_refidx] = object;
			m_refNodes[lightinstance->m_refidx] = inode;
		}
		else
		{
			inode = mainI->CreateObjectNode(m_refObjects[lightinstance->m_refidx]);
		}

		Matrix3 tm;
		mffGroupTransformInstance *groupinstance;

		tm.IdentityMatrix();

		groupinstance = m_mff->GetGroupTransformInstance(lightinstance->m_parentindex);

		TSTR name = groupinstance->m_name;

		inode->SetName(name);

		tm = GetTransformMatrix(m_mff->GetGroupTransform(groupinstance->m_refidx)->m_transform,1);

		Control *tmc = NewDefaultMatrix3Controller();
		inode->SetTMController(tmc);
		inode->SetNodeTM(0,tm);
	}
}

void Mffio::AddAttribute(mffAttribute *attr,INode *node)
{
	TSTR maxbuffer;

	node->GetUserPropBuffer(maxbuffer);

	String mybuffer = maxbuffer;

	if (strcmp(attr->m_name,"collision_type")==0)
	{
		mffEnum *mffenum = (mffEnum*)attr;

		if (mffenum->m_value == 0)
		{
			AddKeyValue(mybuffer,"CollisionType","UseMesh");
			AddKeyValue(mybuffer,"DefaultCollisionType","UseMesh");
		}
		else if (mffenum->m_value == 1)
		{
			AddKeyValue(mybuffer,"CollisionType","UseMesh");
			AddKeyValue(mybuffer,"DefaultCollisionType","Disabled");
		}
		else if (mffenum->m_value == 4)
		{
			AddKeyValue(mybuffer,"CollisionType","UseMesh");
			AddKeyValue(mybuffer,"DefaultCollisionType","TerrainAndOBB");
		}
		else if (mffenum->m_value == 3)
		{
			AddKeyValue(mybuffer,"CollisionType","UseMesh");
			AddKeyValue(mybuffer,"DefaultCollisionType","OBBOnly");
		}
		else
		{
			AddKeyValue(mybuffer,"CollisionType","UseMesh");
			AddKeyValue(mybuffer,"DefaultCollisionType","TerrainOnly");
		}
	}
	else if (strcmp(attr->m_name,"display_layer_render_first")==0)
	{
		mffInt *mffint = (mffInt*)attr;

		char value[128];
		itoa(mffint->m_value,value,10);
		AddKeyValue(mybuffer,"display_layer","render_first");
		AddKeyValue(mybuffer,"render_first_layer",value);
	}
	else if (strcmp(attr->m_name,"display_layer_render_last")==0)
	{
		mffInt *mffint = (mffInt*)attr;

		char value[128];
		itoa(mffint->m_value,value,10);
		AddKeyValue(mybuffer,"display_layer","render_last");
		AddKeyValue(mybuffer,"render_last_layer",value);
	}
	else if (strcmp(attr->m_name,"collision_only")==0)
	{
		mffBoolean *mffbool = (mffBoolean*)attr;

		if (mffbool->m_value)
			AddKeyValue(mybuffer,"collision_only","True");
		else
			AddKeyValue(mybuffer,"collision_only","False");
	}
	else if (strcmp(attr->m_name,"OBB_Value")==0)
	{
		mffInt *mffint = (mffInt*)attr;

		char value[128];
		itoa(mffint->m_value,value,10);

		AddKeyValue(mybuffer,"OBBValue",value);
	}
	else if (strcmp(attr->m_name,"no_fog")==0)
	{
		mffBoolean *mffbool = (mffBoolean*)attr;

		if (mffbool->m_value)
			AddKeyValue(mybuffer,"instance_no_fog","True");
		else
			AddKeyValue(mybuffer,"instance_no_fog","False");
	}
	else if (strcmp(attr->m_name,"no_far_clip")==0)
	{
		mffBoolean *mffbool = (mffBoolean*)attr;
		if (mffbool->m_value)
			AddKeyValue(mybuffer,"instance_no_far_clip","True");
		else
			AddKeyValue(mybuffer,"instance_no_far_clip","False");
	}
	else if (strcmp(attr->m_name,"notes")==0)
	{
		mffString *mffstring = (mffString*)attr;
		StringArray notelines;

		String notes = mffstring->m_value;

		String temp = "";
		for (unsigned i = 0; i < notes.size(); i++)
		{
			if (notes.substr(i,1) != "\n")
				temp += notes.substr(i,1);
		}
		temp.Split('\r',notelines);

		for (i = 0; i < notelines.size(); i++)
		{
			StringArray parse;
			notelines[i].Split('=',parse);

			if (parse.size() == 2)
			{
				AddKeyValue(mybuffer,parse[0],parse[1]);
			}
		}
	}
	node->SetUserPropBuffer(mybuffer.c_str());
}


void Mffio::AddKeyValue(String &mybuffer,String key,String value)
{
	StringArray notelines;
	
	String temp = "";
	for (unsigned i = 0; i < mybuffer.length(); i++)
	{
		if (mybuffer.substr(i,1) != "\n")
			temp += mybuffer.substr(i,1);
	}

	temp.Split('\r',notelines);

	const char *debugkey = key.c_str();
	const char *debugvalue = value.c_str();

	String newbuffer;

	String line = key;
	line += String("=") + value;

	int idx = FindLine("BEGIN OBJECTATTRIBUTES",notelines);
	if (idx<0)
	{
		notelines.push_back("BEGIN OBJECTATTRIBUTES");
		notelines.push_back(line);
		notelines.push_back("END OBJECTATTRIBUTES");
		
		for (i = 0; i < notelines.size(); i++)		
			newbuffer += notelines[i] + String("\r\n");

		mybuffer = newbuffer;
		return;
	}

	const char *pKey = key.c_str();

	String oldvalue;
	int keyidx = FindKey(key,notelines,oldvalue);

	if (keyidx < 0)
	{
		notelines.Insert(line,idx+1);
	}
	else
	{
		notelines[keyidx] = line;
		const char *debug = notelines[keyidx].c_str();
	}
	
	for (i = 0; i < notelines.size(); i++)
		newbuffer += notelines[i] + String("\r\n");
	
	mybuffer = newbuffer;
}

int Mffio::FindKey(String key,StringArray &notelines,String &value)
{
	const char *pKey = key.c_str();

	for (unsigned i = 0; i < notelines.size(); i++)
	{
		const char *debug = notelines[i].c_str();

		StringArray noteparse;
		notelines[i].Split('=',noteparse);

		if (noteparse.size()<=1)
			continue;

		if (key == noteparse[0])
		{
			value = noteparse[1];
			return i;
		}
	}
	return -1;
}

int Mffio::FindLine(String line,StringArray &notelines)
{
	for (unsigned i = 0; i < notelines.size(); i++)
	{
		if (line == notelines[i])
		{
			return i;
			break;
		}	
	}
	return -1;
}


double GetUnitRatio()//THIS IS HORRIBLE, MAX UNITS SUCK!
{
	return GetMasterScale(UNITS_CENTIMETERS);
}

// type: 0 - object, 1 - light
Matrix3 Mffio::GetTransformMatrix(const mffTransform &transform,int type)
{
	double unitratio = GetUnitRatio() * 0.0001;

	Matrix3 tm;
	tm.IdentityMatrix();

	// swap y & z axis; flip z
	mffVector transVector(transform.translate.x,-transform.translate.z,transform.translate.y);
	mffVector scaleVector(transform.scale.x*100,transform.scale.z*100,transform.scale.y*100);	

	mffVector pivotVector(transform.rotatepivot.x,-transform.rotatepivot.z,transform.rotatepivot.y);

	// build translation and scale
	Point3 transPoint(transVector.x*unitratio,transVector.y*unitratio,transVector.z*unitratio);
	Point3 scalePoint(scaleVector.x*unitratio,scaleVector.y*unitratio,scaleVector.z*unitratio);

	Point3 pivotPoint(-pivotVector.x*unitratio,-pivotVector.y*unitratio,-pivotVector.z*unitratio);

	tm.Translate(pivotPoint);

	// build matrix
	tm.Scale(scalePoint);

	if (type == 1)
	{
		tm.RotateX(DEGTORAD * 90.0f);
		tm.RotateX(DEGTORAD * transform.rotate.x);
		tm.RotateZ(DEGTORAD * transform.rotate.y);
		tm.RotateY(-DEGTORAD * transform.rotate.z);
	}
	else
	{
		tm.RotateX(DEGTORAD * transform.rotate.x);
		tm.RotateZ(DEGTORAD * transform.rotate.y);
		tm.RotateY(-DEGTORAD * transform.rotate.z);
	}
	tm.Translate(-pivotPoint);

	tm.Translate(transPoint);
	return tm;
}

INode *Mffio::CreatePolygonModel(mffPolyModel *polymodel,mffPolyModelInstance *polymodelinstance,Interface *mainI)
{
	if (!polymodel)
		return NULL;

	Object *vcObject;
	Object *object;

	mffVertexColorNode *vcnode = (mffVertexColorNode*)m_mff->GetReferenceNode(polymodelinstance->m_vcid);
	int numVertexColors = vcnode->m_VertexColors.size();
	INode *pNode;

	// check if mesh has already been done
	object = m_refObjects[polymodelinstance->m_refidx];
	if (object)
	{
		// if vertex color object has also been done
		vcObject = m_refObjects[polymodelinstance->m_vcid];
		if (vcObject)
		{
			pNode = mainI->CreateObjectNode(vcObject);
		}
		// if I need to add in vertex colors
		else if (numVertexColors>0)
		{
			ApplyVCMod* mod = (ApplyVCMod*)CreateInstance(OSM_CLASS_ID, APPLYVC_MOD_CLASS_ID);
			Mesh *mesh = &((TriObject *)object)->GetMesh();
			FillVertexColors(*mesh,*mod,*vcnode,*polymodel);
			IDerivedObject* dObject = CreateDerivedObject(object);
			dObject->AddModifier(mod);
			m_refObjects[polymodelinstance->m_vcid] = dObject;
			pNode = mainI->CreateObjectNode(dObject);
		}
		// no vertex colors
		else
		{
			pNode = mainI->CreateObjectNode(object);
		}
		// grab material from previous instance
		pNode->SetMtl(m_refNodes[polymodelinstance->m_refidx]->GetMtl());
	}
	// mesh has not been done
	else
	{
		// if we don't have a mesh object, create one
		object = (Object *)mainI->CreateInstance(GEOMOBJECT_CLASS_ID, Class_ID(EDITTRIOBJ_CLASS_ID, 0));
		m_refObjects[polymodelinstance->m_refidx] = object;

		Mesh *mesh = &((TriObject *)object)->GetMesh();

		MtlList loadMtls; // the materials that have been created
		CreateMaterials(mainI,polymodel->m_shaders,loadMtls);
		int validShaders = CreatePolygonMesh(mesh,*polymodel);
		if (numVertexColors==0)
		{
			pNode = mainI->CreateObjectNode(object);
		}
		// I have vertex colors
		else
		{
			ApplyVCMod* mod = (ApplyVCMod*)CreateInstance(OSM_CLASS_ID, APPLYVC_MOD_CLASS_ID);
			FillVertexColors(*mesh,*mod,*vcnode,*polymodel);
			IDerivedObject* dObject = CreateDerivedObject(object);
			dObject->AddModifier(mod);
			m_refObjects[polymodelinstance->m_vcid] = dObject;
			pNode = mainI->CreateObjectNode(dObject);
//			pNode = mainI->CreateObjectNode(object);
		}
		if (validShaders)
			AssignMaterials(mainI,pNode,mesh,loadMtls);
		else
			pNode->SetMtl(NULL);
		// set node that did the mesh the first time
		m_refNodes[polymodelinstance->m_refidx] = pNode;
	}
	return pNode;
}

Point3 ConvVector(const mffVector &vect,float scale=1.0f)
{
	Point3 temp;
	temp.x = (float)vect.x*scale;
	temp.y = (float)-vect.z*scale;
	temp.z = (float)vect.y*scale;
	return temp;
}

int Mffio::CreatePolygonMesh(Mesh *mesh,mffPolyModel &model)
{
	int validShaders = TRUE;

	double scale = GetUnitRatio()*0.0001;
	
	if (model.m_VertexCounts.size()==0)
		return 0;

	int numVertsInPool = model.m_VertexArray.size();

	mesh->setNumVerts(numVertsInPool);

	for (unsigned i = 0;i < numVertsInPool; i++)
	{
		mffVector *vect = &model.m_VertexArray[i];
		mesh->setVert(i,ConvVector(*vect,scale));
	}

/*	int numUVVertsInPool = model.m_UVArray.size();
	if (numUVVertsInPool!=0)
	{
		mesh->setMapSupport(1);
		
		mesh->setNumMapVerts(1,numUVVertsInPool);
		for (i = 0; i < numUVVertsInPool; i++)
		{		
			mffVector2 *vect = &model.m_UVArray[i];
			Point3 pt;
			pt[0] = vect->x;
			pt[1] = vect->y;
			pt[2] = 0.0;
			mesh->setMapVert(1,i,pt);
		}
	}*/


//	MAPPING
	int numUVVertsInPool = 0;
	mffUVMap *uvmap = NULL;
	if (model.m_UVMaps.size() > 0)
	{
		uvmap = &model.m_UVMaps[i];

		numUVVertsInPool = uvmap->m_UVArray.size();
		if (numUVVertsInPool!=0)
		{
			mesh->setMapSupport(1);
			
			mesh->setNumMapVerts(1,numUVVertsInPool);
			for (i = 0; i < numUVVertsInPool; i++)
			{		
				mffVector2 *vect = &uvmap->m_UVArray[i];
				Point3 pt;
				pt[0] = vect->x;
				pt[1] = vect->y;
				pt[2] = 0.0;
				mesh->setMapVert(1,i,pt);
			}
		}
	}

	// calculate number of faces
	int numFaces=0;
	int pointTotal;

	for (i = 0; i < model.m_VertexCounts.size(); i++)
	{
		pointTotal = model.m_VertexCounts[i];
		if (pointTotal<3)continue;
		numFaces += pointTotal-2; // 3->1 4->2 5->3
	}

	mesh->setNumFaces(numFaces);
	TVFace *uvFaces;
	if (numUVVertsInPool>0)
	{
		mesh->setNumMapFaces(1,numFaces);
		uvFaces = mesh->mapFaces(1);
	}

	int faceIndex = 0;
	int edge0vis,edge2vis;

	unsigned count = 0;

	for (unsigned j = 0; j < model.m_VertexCounts.size(); j++) 
	{
		pointTotal = model.m_VertexCounts[j];
		if (pointTotal<3) continue; // kick out if this one is empty has less than 3 sides
		for (int i=2;i<pointTotal;i++)
		{
			mesh->faces[faceIndex].setVerts(model.m_PolyVertexIndicies[count],
											model.m_PolyVertexIndicies[count+i-1],
											model.m_PolyVertexIndicies[count+i]);
			if (numUVVertsInPool>0)
			{
				uvFaces[faceIndex].setTVerts(uvmap->m_PolyUVIndicies[count],
											 uvmap->m_PolyUVIndicies[count + i-1],
											 uvmap->m_PolyUVIndicies[count + i]);
			}
			
			edge0vis = (i==2) ? EDGE_VIS : EDGE_INVIS;
			edge2vis = (i==pointTotal-1) ? EDGE_VIS : EDGE_INVIS;

			mesh->faces[faceIndex].setSmGroup(1); // all smoothed the same

			if (model.m_ShaderIndicies[j] !=-1)
				mesh->faces[faceIndex].setMatID(model.m_ShaderIndicies[j]);
			else
			{
				mesh->faces[faceIndex].setMatID(0);
				validShaders = FALSE;
			}


			mesh->faces[faceIndex++].setEdgeVisFlags(edge0vis, 1, edge2vis);
		}
		count+=model.m_VertexCounts[j];
	}
	mesh->buildNormals();
	mesh->buildBoundingBox();
	mesh->InvalidateEdgeList();

	return validShaders; // return TRUE if all the shader indices were not -1
}

INode *Mffio::CreateCurve(mffCurveModel *curvemodel,Interface *mainI)
{

	double unitratio = GetUnitRatio() * 0.0001;

	Object *object = (Object *)mainI->CreateInstance(SHAPE_CLASS_ID,Class_ID(SPLINESHAPE_CLASS_ID,0));
	if(!object)
		return NULL;

	BezierShape *beziershape = &((SplineShape *)object)->GetShape();

	Point3 inVec,outVec;

	if (curvemodel->m_degree!=3)
		return 0;

	Spline3D *spline;

	spline = beziershape->NewSpline();

	int cnt = 0;
	for (unsigned i = 2; i < (curvemodel->m_KnotArray.size()-2); i++)
	{
		Point3 pos,prev,next;
		pos = Point3(curvemodel->m_KnotArray[i].x,-curvemodel->m_KnotArray[i].z,curvemodel->m_KnotArray[i].y);

		inVec = Point3(0,0,0);
		outVec = Point3(0,0,0);

		if (i == 2)
		{
			if (curvemodel->m_form == 1)
			{
				next = Point3(curvemodel->m_KnotArray[i+1].x,-curvemodel->m_KnotArray[i+1].z,curvemodel->m_KnotArray[i+1].y);
				outVec = next - pos;
				outVec /= 3.0;
				outVec += pos;
			}
			else
			{
				prev = Point3(curvemodel->m_KnotArray[1].x,-curvemodel->m_KnotArray[1].z,curvemodel->m_KnotArray[1].y);
				next = Point3(curvemodel->m_KnotArray[curvemodel->m_KnotArray.size()-2].x,-curvemodel->m_KnotArray[curvemodel->m_KnotArray.size()-2].z,curvemodel->m_KnotArray[curvemodel->m_KnotArray.size()-2].y);

				outVec = next - prev;
				outVec *= 0.5;
				outVec /= 3.0;
				outVec += pos;

				inVec = prev - next;
				inVec *= 0.5;
				inVec /= 3.0;
				inVec += pos;				
			}
		}
		else if (i == (curvemodel->m_KnotArray.size()-2))
		{
			if (curvemodel->m_form == 1)
			{
				prev = Point3(curvemodel->m_KnotArray[i-1].x,-curvemodel->m_KnotArray[i-1].z,curvemodel->m_KnotArray[i-1].y);
				inVec = prev - pos;
				inVec /= 3.0;
				inVec += pos;
			}
			else
			{
				next = Point3(curvemodel->m_KnotArray[1].x,-curvemodel->m_KnotArray[1].z,curvemodel->m_KnotArray[1].y);
				prev = Point3(curvemodel->m_KnotArray[curvemodel->m_KnotArray.size()-2].x,-curvemodel->m_KnotArray[curvemodel->m_KnotArray.size()-2].z,curvemodel->m_KnotArray[curvemodel->m_KnotArray.size()-2].y);

				double itlen = (prev-pos).Length();
				double otlen = (next-pos).Length();

				outVec = next - prev;
				double sum = outVec.Length();
				outVec *= (otlen/sum);
				outVec /= 3.0;
				outVec += pos;

				inVec = prev - next;
				inVec *= (itlen/sum);
				inVec /= 3.0;
				inVec += pos;				
			}
		}
		else
		{
			prev = Point3(curvemodel->m_KnotArray[i-1].x,-curvemodel->m_KnotArray[i-1].z,curvemodel->m_KnotArray[i-1].y);
			next = Point3(curvemodel->m_KnotArray[i+1].x,-curvemodel->m_KnotArray[i+1].z,curvemodel->m_KnotArray[i+1].y);

			Point3 dir = next - prev;
			double itlen = (prev-pos).Length();
			double otlen = (next-pos).Length();

			outVec = next - prev;
			double sum = outVec.Length();
			outVec *= (otlen/sum);
//			outVec *= 0.5;
			outVec /= 3.0;
			outVec += pos;

			inVec = prev - next;
			inVec *= (itlen/sum);
//			inVec *= 0.5;
			inVec /= 3.0;
			inVec += pos;
		}

		spline->AddKnot(SplineKnot(KTYPE_BEZIER,LTYPE_CURVE,pos,inVec,outVec));
	}

	if (curvemodel->m_form != 1)
		spline->SetClosed(1);
	spline->ComputeBezPoints();

	beziershape->UpdateSels();
	beziershape->InvalidateGeomCache();

	return mainI->CreateObjectNode(object);
}

void Mffio::FillVertexColors(Mesh &mesh,ApplyVCMod &mod,mffVertexColorNode &vcnode,mffPolyModel &model)
{
	// calculate number of faces
	int numVertexColors = vcnode.m_VertexColors.size();

	if (numVertexColors>0)
	{
		int numFaces=mesh.getNumFaces();

		int faceIndex = 0;
		unsigned count = 0;
		FaceColorTab newFaceColors;

		for (unsigned j = 0; j < model.m_VertexCounts.size(); j++) 
		{
			int pointTotal = model.m_VertexCounts[j];
			if (pointTotal<3) continue; // kick out if this one is empty has less than 3 sides
			for (int i=2;i<pointTotal;i++)
			{
				FaceColor* faceCol = new FaceColor;
				SetFaceColorPoint(*faceCol,vcnode,count,0);
				SetFaceColorPoint(*faceCol,vcnode,count+i-1,1);
				SetFaceColorPoint(*faceCol,vcnode,count+i,2);
				newFaceColors.Append(1, &faceCol, 25);
			}
			count+=model.m_VertexCounts[j];
		}
		mod.SetColors(newFaceColors);
		for (int i=0; i<newFaceColors.Count(); i++) 
		{
			delete newFaceColors[i];
		}

	}
}

void Mffio::SetFaceColorPoint(FaceColor &faceCol,mffVertexColorNode &vcnode,int vcIndex,int pointIndex)
{
	mffColor *rgb = &vcnode.m_VertexColors[vcIndex];

	if (rgb->r>1.0)	rgb->r = 1.0;
	if (rgb->g>1.0) rgb->g = 1.0;
	if (rgb->b>1.0)	rgb->b = 1.0;

	faceCol.colors[pointIndex].r = rgb->r;
	faceCol.colors[pointIndex].g = rgb->g;
	faceCol.colors[pointIndex].b = rgb->b;
}

int Mffio::CreateMaterials(Interface *mainI,std::vector<mffShader> &Shaders,MtlList &loadMtls) 
{
	for (unsigned i = 0; i < Shaders.size(); i++)
		CreateMaterial(mainI,&Shaders[i],loadMtls);

	if (Shaders.size()==0)
	{
		mffShader defaultshader;

		defaultshader.m_color = mffColor(0.5,0.5,0.5,1.0);

		CreateMaterial(mainI,&defaultshader,loadMtls);
	}

	return 1;
}

int Mffio::CreateMaterial(Interface *mainI,mffShader *shader,MtlList &loadMtls)
{
	StdMat2 *m;
	Mesh *mesh = NULL;
	m = NewDefaultStdMat();
	m->SetName(shader->m_name);

	CreateShaderNetwork(shader,m);

	loadMtls.AddMtl(m);
	
	return 1;
}

void Mffio::CreateShaderNetwork(mffShader *shader,StdMat2 *m)
{

	m->SwitchShader(Class_ID(0x2857f470, 0));//Sets shader to Multi_Layer
	
	m->SetAmbient(Color(0,0,0),0);

	m->SetDiffuse(Color(shader->m_color.r,shader->m_color.g,shader->m_color.b),0);

	m->SetSpecular(Color(0,0,0),0);
	
	m->SetOpacity(1.0,0);

	if (shader->m_colorMap)
	{
		if (shader->m_colorMap->GetType() == mffMapObject::kSTANDARD)
		{
			mffStd *standard = (mffStd*)shader->m_colorMap;
			mffMap *map = &standard->m_map;
			BitmapTex *bmt = MakeTextureMap(map);
			m->SetSubTexmap(ID_DI,bmt);
			m->EnableMap(ID_DI,1);
		}
		else if (shader->m_colorMap->GetType() == mffMapObject::kBLENDER)
		{
			mffBlender *blender = (mffBlender*)shader->m_colorMap;
			MultiTex *compositetex = MakeMixTexture(blender);
			m->SetSubTexmap(ID_DI,compositetex);
			m->EnableMap(ID_DI,1);	
		}
	}
}

MultiTex *Mffio::MakeMixTexture(mffBlender *blender)
{
	MultiTex *mixtex = (MultiTex *)CreateInstance(TEXMAP_CLASS_ID, Class_ID(MIX_CLASS_ID,0));

	mffMap *map0 = &blender->m_map0;
	mffMap *map1 = &blender->m_map1;
	mffMap *map2 = &blender->m_map2;

	BitmapTex *bmt0 = MakeTextureMap(map0);
	BitmapTex *bmt1 = MakeTextureMap(map1);
	BitmapTex *bmt2 = MakeTextureMap(map2,true);

	mixtex->SetSubTexmap(0,bmt0);
	mixtex->SetSubTexmap(1,bmt1);
	mixtex->SetSubTexmap(2,bmt2);

	return mixtex;
}

BitmapTex *Mffio::MakeTextureMap(mffMap *pMap,bool invert)
{
	BitmapTex *bmt = NewDefaultBitmapTex();

	bmt->SetMapName(pMap->m_filename);

	Bitmap *bm = bmt->GetBitmap(0);

	StdUVGen *uv = bmt->GetUVGen();

//	uv->SetUOffs(uoffset,0);
//	uv->SetVOffs(voffset,0);

	uv->SetUScl(pMap->m_repeat.x,0);
	uv->SetVScl(pMap->m_repeat.y,0);

	int flags = 0;

	if (pMap->m_wrapu)
		flags |= U_WRAP;
	if (pMap->m_wrapv)
		flags |= V_WRAP;

	uv->SetTextureTiling(flags);

	if (bm)
	{
		if (bm->HasAlpha())
			bmt->SetPremultAlpha(false);

		if (invert)
		{
			TextureOutput *texout = bmt->GetTexout();
			texout->SetInvert(true);
		}
	}
	
	return bmt;
}

int Mffio::AssignMaterials(Interface *mainI,INode *node,Mesh *mesh,MtlList &loadMtls)
{
	short used[256],cused[256],remap[256];
	for (int i=0; i<256; i++) used[i] = 0;
	// See if a multi-mtl is required.
	int nmtl,numMtls=0;
	for (i =0; i<mesh->numFaces; i++) 
	{
		nmtl = mesh->faces[i].getMatID();
		assert(nmtl<256);
		if (!used[nmtl]) 
		{
			used[nmtl] = 1;
			remap[nmtl] = numMtls;
			cused[numMtls++] = nmtl;
		}
	}
	if (numMtls>1) 
	{ 
		// Need a Multi-mtl
		// scrunch the numbers down to be local to this multi-mtl
		for (i =0; i<mesh->numFaces; i++)
		{
			Face &f = mesh->faces[i];
			int id = f.getMatID();
			f.setMatID(remap[id]);
		}
		// create a new multi with numMtls, and set them
		// to GetMaxMtl(cused[i]), i==0..numMtls-1
		MultiMtl *newmat = NewDefaultMultiMtl();
		newmat->SetNumSubMtls(numMtls);
		// make materials used by mesh subs of newmat
		for (i=0; i<numMtls; i++) 
		{
			Mtl *m = loadMtls[cused[i]];
			newmat->SetSubMtl(i,m);
		}
		// activate in viewports
		for (i=0; i<numMtls; i++) 
		{
			Mtl *m = loadMtls[cused[i]];
			mainI->ActivateTexture(m,m,i);
		}
		newmat->SetName(node->GetName());
		node->SetMtl(newmat);
	}
	else 
	{
		if (mesh->getNumFaces()) 
		{
			nmtl = mesh->faces[0].getMatID();
			for (i =0; i<mesh->numFaces; i++)
				mesh->faces[i].setMatID(0);
			// activate in viewports
			Mtl *m = loadMtls[nmtl];
			Texmap *tx = m->GetSubTexmap(ID_DI);
			mainI->ActivateTexture(tx,m);
			node->SetMtl(loadMtls[nmtl]);
			m->ClearAFlag(A_LOCK_TARGET); // keeps material from getting auto-deleted
		}
	}
	return 1;
}


