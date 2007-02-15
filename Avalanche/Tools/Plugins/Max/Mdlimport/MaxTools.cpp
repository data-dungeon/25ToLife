////////////////////////////////////////////////////////////////////////////
//
// MaxTools
//
// Handle converting between mdllib and 3dsMax
//
////////////////////////////////////////////////////////////////////////////

#include <iostream.h> 

#include "max.h"
#include "stdmat.h"
#include "units.h"
#include "bmmlib.h"
#include "guplib.h"

#include "maxtools.h"
#include "mdlobject.h"
#include "filepath.h"

#include "hgroup.h"
#include "hmacros.h"
#include "max_macros.h"
#include "hstamp.h"
#include "hpolyarray.h"
#include <iparamb2.h>
#include <iiksys.h>
#include <ikctrl.h>
#include "decomp.h"
#include <shaders.h>
#include <iskin.h>
#include <splshape.h>
#include <modstack.h>

//#include <C:\Program Files\3dsmax4\maxsdk\Samples\Modifiers\EditPatch\dllmain.h>
//#include <C:\Program Files\3dsmax4\maxsdk\Samples\Modifiers\EditPatch\editpat.h>
#define EDITPATCH_CLASS_ID			0x00070

MaxTools::MaxTools(mdlObject &m) 
{
	theModel = &m;

	memcpy(&ui_flags,&m.ui_flags,sizeof(unsigned long));
	ui_numsubdivisions = m.ui_numsubdivisions;
	unitratio = GetUnitRatio();
}

MaxTools::~MaxTools(){}

static Point3 ConvVector(const HVector &vector,float scale=1.0f)
{
	Point3 temp;
	temp.x = (float)vector.x*scale;
	temp.y = (float)vector.y*scale;
	temp.z = (float)vector.z*scale;
	return temp;
}

static Point3 ConvVector2(const HVector &vector,float scale=1.0f)
{
	Point3 temp;
	temp.x = (float)vector.x*scale;
	temp.y = (float)-vector.z*scale;
	temp.z = (float)vector.y*scale;
	return temp;
}

static Point3 ConvVector(const HVector2D &vector,float scale=1.0f)
{
	Point3 temp;
	temp.x = (float)vector.x*scale;
	temp.y = (float)vector.y*scale;
	temp.z = 0.0f;
	return temp;
}

void MaxTools::ConvertHSplineToSpline3D(HSpline *from,Spline3D *to,double unitratio) {
	HVector pos,it,ot;
	Point3 p,inVec,outVec;
	bool loop = false;
	
	for (HCP *iter = from->head(); iter != NULL; iter = iter->next()) {
		if (!iter->flags.kIsLoop) {
			pos = from->splineArray->vertices[from->splineArray->drillForVertexIndex(iter)];
		}
		else {
			pos = from->splineArray->vertices[from->splineArray->drillForVertexIndex(iter->head())];
			loop = true;
		}
		if (!loop) {
			p = ConvVector2(pos,unitratio);
			inVec = ConvVector2((pos - (iter->it/3.0)),unitratio);
			outVec = ConvVector2((pos + (iter->ot/3.0)),unitratio);
			to->AddKnot(SplineKnot(KTYPE_BEZIER,LTYPE_CURVE,p,inVec,outVec));
		}
	}
	if (loop) {
		to->SetClosed();

		p = ConvVector2(pos,unitratio);
		inVec = ConvVector2((pos - ((from->tail())->it/3.0)),unitratio);
		outVec = ConvVector2((pos + ((from->head())->ot/3.0)),unitratio);
		to->SetKnot(0,SplineKnot(KTYPE_BEZIER,LTYPE_CURVE,p,inVec,outVec));

	}
	to->ComputeBezPoints();
}

HStatus MaxTools::CreateSplineCage(BezierShape *beziershape) {
	HStatus status;

	Spline3D *maxspline;

	status = theModel->calculateSplineTangents();
	if (status != HStatus::kSuccess)
		return status;
	
	beziershape->NewShape();

	for (HSpline *iter = theModel->splineArray.begin(); iter != NULL; iter = iter->next()) {
		maxspline = beziershape->NewSpline();
		ConvertHSplineToSpline3D(iter,maxspline,unitratio);
		if (status != HStatus::kSuccess)
			return status;
	}

	beziershape->UpdateSels();
	beziershape->InvalidateGeomCache();
	
	return status;
}

INode *MaxTools::CreateSplinePatchModel(Interface *mainI,HStatus &status)
{
	Object *object;
	INode *node;

	status = HStatus::kSuccess;

	object = (Object *)mainI->CreateInstance(SHAPE_CLASS_ID,Class_ID(SPLINESHAPE_CLASS_ID,0));
	if(!object)
	{
		status = HStatus::kFailure;
		return NULL;
	}
	
	BezierShape *beziershape = &((SplineShape *)object)->GetShape();

	CreateSplineCage(beziershape);

	Modifier *surface_modifier = (Modifier *)mainI->CreateInstance(OSM_CLASS_ID,Class_ID(0x13285684,0x5dc6752e));

	IDerivedObject *derivedObject = CreateDerivedObject(object);
		
	node = mainI->CreateObjectNode(derivedObject);

//	int nummods = derivedObject->NumModifiers();
		
	derivedObject->AddModifier(surface_modifier,NULL,0);

	IParamArray* paramarray = surface_modifier->GetParamBlock();

	IParamBlock* pblock = paramarray->GetParamBlock();

	int numParams = pblock->NumParams();
		
	pblock->SetValue(0,0,0.001f);	//weld tolerance
//	pblock->SetValue(1,0,true);		//flip normals
	pblock->SetValue(2,0,true);		//remove interior patches
	pblock->SetValue(3,0,0);		//number of subdivisions

	Modifier *PatchEditModifier = (Modifier *)mainI->CreateInstance(OSM_CLASS_ID,Class_ID(EDITPATCH_CLASS_ID,0));
//	EditPatchMod *PatchEditModifier = (EditPatchMod *)mainI->CreateInstance(OSM_CLASS_ID,MYEDITPATCH_CLASS_ID);

	mainI->SetCommandPanelTaskMode(TASK_MODE_MODIFY);
	mainI->SelectNode(node);

	derivedObject->AddModifier(PatchEditModifier,NULL,0);

	ModContext *mc = NULL;
	ObjectState os;

	mainI->SetCommandPanelTaskMode(TASK_MODE_MODIFY);
	mainI->SelectNode(node);

	mainI->ForceCompleteRedraw(0);
/*
	PatchObject *pobj;

	for (int i = 0; i < derivedObject->NumModifiers(); i++)
	{	
		Modifier* mod = derivedObject->GetModifier(i);
		if (mod == PatchEditModifier)
		{
			mc = derivedObject->GetModContext(i);
			os = derivedObject->Eval(0,i);
			pobj = (PatchObject *)os.obj;
			break;
		}
	}
	
	PatchMesh *mesh = &pobj->GetPatchMesh(0);
	int numpatches = mesh->getNumPatches();

//	mainI->SetSubObjectLevel(PO_PATCH);

//	IPatchSelect *ips = (IPatchSelect*)pobj->GetInterface(I_PATCHSELECT);
//	ips->SetSelLevel(PO_PATCH);
	
//	IPatchSelectData *ips2 = (IPatchSelectData*)pobj->GetInterface(I_PATCHSELECTDATA);

	BitArray bitarray;
	bitarray.SetSize(numpatches);
	bitarray.SetAll();
//	ips2->SetPatchSel(bitarray,ips,0);


//	mesh->patchSel = bitarray;
//	pobj->DoFlipNormals(-1);

//	for (i = 0; i < numpatches/2; i++)
//	{
//		mesh->FlipPatchNormal(i);
//	}
//	assert(mesh->buildLinkages());
//	mesh->computeInteriors();

	Color c(255,0,0);

	pobj->SetPatchColor(c);
	mesh->InvalidateGeomCache();
	pobj->InvalidateMesh();

//	if (!mc)
//	{
//		status = HStatus::kFailure;
//		return NULL;
//	}		

 //	EditPatchData *patchData = (EditPatchData*)mc->localData;

//	if (!patchData)
//	{
//		status = HStatus::kFailure;
//		return NULL;
//	}

	// If the mesh isn't yet cache, this will cause it to get cached.
//	PatchMesh *patch = patchData->TempData(PatchEditModifier)->GetPatch(0);
//	if (!patch)
//	{
//		status = HStatus::kFailure;
//		return NULL;
//	}

//	int numpatches = patch->getNumPatches();

//	for (int j = 0; j < numpatches; j++)
//	{
//		PatchEditModifier->DoFlipNormals(patch,j,true);
//	}

	mainI->ForceCompleteRedraw(0);
*/	
	return node;
}


double MaxTools::GetUnitRatio() {
	return 1.0/GetMasterScale(UNITS_CENTIMETERS);
}

Matrix3 MaxTools::GetTransformMatrixFromHash(const HVector &translate,
											 const HVector &rotate,
											 const HVector &scale) {

	double unitratio = GetUnitRatio();

	Matrix3 tm;
	tm.IdentityMatrix();

	// swap y & z axis; flip z
	HVector transVector(translate.x,-translate.z,translate.y);
	HVector rotVector(rotate.x,-rotate.z,rotate.y);
	HVector scaleVector(scale.x,scale.z,scale.y);	

	// build translation and scale (make sure we scale)
	Point3 transPoint(transVector.x*unitratio,transVector.y*unitratio,transVector.z*unitratio);
	Point3 scalePoint(scaleVector.x*unitratio,scaleVector.y*unitratio,scaleVector.z*unitratio);

	// build matrix
	tm.Scale(scalePoint);
	tm.RotateY(rotVector.y*piOver180);
	tm.RotateX(rotVector.x*piOver180);
	tm.RotateZ(rotVector.z*piOver180);
	tm.Translate(transPoint);
	return tm;
}

void MaxTools::SetAssignDfltMatrixControllerFromHash(INode *node, TimeValue t,
								const HVector &translate,
								const HVector &rotate,
								const HVector &scale) {
	Matrix3 tm = MaxTools::GetTransformMatrixFromHash(translate,rotate,scale);
	Control *tmc = NewDefaultMatrix3Controller();
	node->SetTMController(tmc);
	node->SetNodeTM(t,tm);
}

void MaxTools::SetTransformMatrixFromHash(INode *node, TimeValue t,
								const HVector &translate,
								const HVector &rotate,
								const HVector &scale) {
	Matrix3 tm = MaxTools::GetTransformMatrixFromHash(translate,rotate,scale);
	node->SetNodeTM(t,tm);
}

// boneobj_params IDs
enum { 
	boneobj_width, boneobj_height, boneobj_taper, boneobj_length,
	boneobj_sidefins, boneobj_sidefins_size, boneobj_sidefins_starttaper, boneobj_sidefins_endtaper,
	boneobj_frontfin, boneobj_frontfin_size, boneobj_frontfin_starttaper, boneobj_frontfin_endtaper,
	boneobj_backfin,  boneobj_backfin_size,  boneobj_backfin_starttaper,  boneobj_backfin_endtaper,
	boneobj_genmap };


bool MaxTools::isReferenceOfType(RefTargetHandle ref,TSTR type) {
	TSTR classname;
	ref->GetClassName(classname);
	if (type == classname)
		return true;
	return false;
}

INode *MaxTools::CreateBone(HBone *bone,Interface *mainI,bool endbone,HStatus &status,float &boneLengthFill) {
	Object *boneobject = (Object *)mainI->CreateInstance(GEOMOBJECT_CLASS_ID,BONE_OBJ_CLASSID);
	if(!boneobject) {
		status = HStatus::kFailure;
		return NULL;
	}
	int numRefs = boneobject->NumRefs();

	RefTargetHandle ref;
	IParamBlock2 *pblock2;

	if (numRefs>0) {
		ref = boneobject->GetReference(0);
		if (isReferenceOfType(ref,"ParamBlock2")) {
			pblock2 = (IParamBlock2*)ref;
		}
	}
	if (pblock2) {
		//HASH BONE AXIS
		Point3 rollhandle = ConvVector2(bone->getRollHandle(),unitratio);
		Point3 bone_pivot = ConvVector2(bone->getPivot(),unitratio);
		Point3 bone_end = ConvVector2(bone->getEnd(),unitratio);

		Point3 rolHndlVect = rollhandle - bone_pivot;
		Point3 boneVect = bone_end - bone_pivot;
		Point3 invisAxis = CrossProd(rolHndlVect,boneVect);

		float bonelength,bonewidth;

		if (!endbone) {
			bonewidth = bonelength = Length(boneVect);
			bonewidth/=4;
		}
		else {
			bonewidth = bonelength = Length(boneVect)/4;
		}
		boneLengthFill = bonelength;
		
		//SWITCH TO MAX BONE AXIS
		Point3 yAxis = Normalize(invisAxis);
		Point3 zAxis = Normalize(rolHndlVect);
		Point3 xAxis = Normalize(boneVect);
		
		if (endbone)
			bone_pivot = bone_end;

		Matrix3 tm(xAxis,yAxis,zAxis,bone_pivot);

		pblock2->SetValue(boneobj_length,0,bonelength);
		pblock2->SetValue(boneobj_height,0,bonewidth);
		pblock2->SetValue(boneobj_width,0,bonewidth);
		
		INode *bonenode = mainI->CreateObjectNode(boneobject);
	
//		bonenode->ShowBone(1);

		bonenode->SetNodeTM(0,tm);

		TSTR boneName = bone->name.asChar();

		if (!endbone)
			bonenode->SetName(boneName);
		else {
			TSTR temp = "iktail";
			mainI->MakeNameUnique(temp);
			bonenode->SetName(temp);
		}

		if (skinModifier && !endbone)
		{
			ISkinImportData *skinImp = (ISkinImportData*)skinModifier->GetInterface(I_SKINIMPORTDATA);
			
			if(skinImp)
			{
				if (!skinImp->AddBoneEx(bonenode,TRUE))
				{
					DISPLAY_IN_LISTENER("ERROR: Couldn't add bone to skin.")
				}
				else
				{
					maxbonelist.push_back(bonenode);
					mybonelist.push_back(bone);
				}
			}

		}

		status = HStatus::kSuccess;

		return bonenode;
	}
	status = HStatus::kFailure;
	return NULL;
}


HStatus MaxTools::AssignWeights(INode *meshNode,Interface *mainI)
{
		if (skinModifier)
		{
			ISkinImportData *skinImp = (ISkinImportData*)skinModifier->GetInterface(I_SKINIMPORTDATA);
			
			if(skinImp)
			{
				for (int i = 0; i < maxbonelist.size(); i++)
				{
					HBone *bone = mybonelist[i];

					for (int j = 0; j < mybonelist[i]->vids.size(); j++)
					{
						int vid = mybonelist[i]->vids[j];
						
						Tab<INode*> boneImp;
						Tab<float> weightImp;
						boneImp.Append(1,&maxbonelist[i]);
						float weight = 1.0f;
						weightImp.Append(1,&weight);
						if (!skinImp->AddWeights(meshNode,vid,boneImp,weightImp))
						{
							DISPLAY_IN_LISTENER("ERROR: Couldn't assign weight to vertex.\n")
						}
					}
				}
			}
		}
		return HStatus::kSuccess;
}

HStatus MaxTools::AssignController(INode *node,IKMasterControl *master,bool makeEE) {
	AffineParts parts;
	Point3 angles;
	IKSlaveControl *slave;

	SuspendSetKeyMode();

	slave = CreateIKSlaveControl(master, node);

	Matrix3	tm = node->GetNodeTM(0) * Inverse(node->GetParentTM(0));
	decomp_affine(tm, &parts);
	QuatToEuler(parts.q, angles, EULERTYPE_XYZ);
		
	slave->SetInitPos(parts.t);
	slave->SetInitRot(angles);		
	slave->SetDOF(5, TRUE);

	node->SetTMController(slave);

	if (makeEE)
		slave->MakeEE(TRUE,1,node->GetNodeTM(0).GetTrans(), Quat());

	ResumeSetKeyMode();

	return HStatus::kSuccess;
}

HStatus MaxTools::CreateSkeleton(Interface *mainI,maxModelInfo &modelinfo) {
	HStatus status;
	mainI->ForceCompleteRedraw(0);

	maxbonelist.clear();
	mybonelist.clear();

	if (theModel->patchArray->boneChainArray.length()==0) return HStatus::kSuccess;

	HBone *bone;
	HBoneChain *bonechain;
	HBoneChainArray newbonechainarray;
	
	theModel->patchArray->boneChainArray.DestroyControlBones(newbonechainarray);

	HBoneArray *bonearray = &theModel->patchArray->boneArray;
	HBoneChainArray *bonechains = &newbonechainarray;
//	HBoneChainArray *bonechains = &theModel->patchArray->boneChainArray;

	status = theModel->createSkeleton();
	if (status != HStatus::kSuccess)
		return status;
	
	if (bonechains->length()==0)
		return HStatus::kSuccess;

	int sumlengths = bonearray->length()+bonechains->length();
	
	INode **nodes;
	nodes = new INode*[sumlengths+bonechains->length()];
	float *pBoneLengths;
	pBoneLengths = new float[sumlengths+bonechains->length()];
	unsigned *parentlist = new unsigned[sumlengths];
	bool *flaglist = new bool[sumlengths];
	memset(flaglist,false,sumlengths);

	for (int k = 0; k < sumlengths+bonechains->length();k++)
		nodes[k] = NULL;

	int bonechainlength;
	INode *prevNode = NULL;
	int i,j,index;
	
//	Create bones and hierarchy arrays
	for (i = 0; i < bonechains->length(); i++) {
		bonechain = bonechains->get(i);
		bonechainlength = bonechain->length();
		for (j = 0; j < bonechainlength; j++) {
			bone = bonechain->get(bonechainlength-j-1);
			index = bonechain->getIndex(bonechainlength-j-1);
			nodes[index] = CreateBone(bone,mainI,false,status,pBoneLengths[index]);
			if (bone->flags.kHasParent) {
				parentlist[index] = bone->parentIndex;
				flaglist[index] = true;
			}
		}
//		Add extra bone to end of chain
		if (bonechain->length()>1) {
			nodes[bonearray->length()+i] = CreateBone(bonechain->end(),mainI,true,status,pBoneLengths[bonearray->length()+i]);
			if (bonechain->end()->flags.kHasParent) {
				parentlist[bonearray->length()+i] = bonechain->getIndex(bonechainlength-1);//parent to last bone of chain
				flaglist[bonearray->length()+i] = true;
			}
		}
	}
//	Create hierarchy
	for (i = 0; i < sumlengths; i++) {
		if (flaglist[i])
			nodes[parentlist[i]]->AttachChild(nodes[i]);
	}

	INode *first,*last;
	IKCmdOps *ikco = GET_IK_OPS_INTERFACE;
//  Assign IK
	for (i = 0; i < bonechains->length(); i++){
		bonechain = bonechains->get(i);
		if (bonechain->length()>1) {
//			IKMasterControl *master = CreateIKMasterControl();

			bonechain = bonechains->get(i);
//			bonechainlength = bonechain->size();

//			for (j = 0; j < bonechainlength; j++) {
//				bone = bonechain->get(bonechainlength-j-1);
//				index = bonechain->getIndex(bonechainlength-j-1);
//				AssignController(nodes[index],master,false);
//			}
//			AssignController(nodes[bonearray->size()+i],master,true);

			first = nodes[bonechain->getIndex(0)];
			last = nodes[bonearray->length()+i];

			nodes[sumlengths+i] = ikco->CreateIKChain(nodes[bonechain->getIndex(0)],nodes[bonearray->length()+i],"IKHISolver");
			pBoneLengths[sumlengths+i] = pBoneLengths[bonearray->length()+i]; // set size of link to the same as last bone
			if (!nodes[sumlengths+i]) {
				DISPLAY_IN_LISTENER("ERROR: Couldn't create IK chain.")
			}
		}
	}

	delete flaglist;
	delete parentlist;

	int numLinks = bonechains->length();
	int numBones = sumlengths;

	for (i = 0; i < numBones; i++)
		if (nodes[i])
			modelinfo.m_bones.push_back(nodes[i]);

	for (i = 0; i < numLinks; i++)
	{
		if (nodes[sumlengths+i])
		{
			modelinfo.m_links.push_back(nodes[sumlengths+i]);
			modelinfo.m_linkSizes.push_back(pBoneLengths[sumlengths+i]);
		}
	}

	delete nodes;
	delete pBoneLengths;

	mainI->ForceCompleteRedraw(0);

	mainI->SetCommandPanelTaskMode(TASK_MODE_MODIFY);
	mainI->SelectNode(modelinfo.m_node);

	AssignWeights(modelinfo.m_node,mainI);

	SetLinkSizes(mainI,modelinfo);
	
	mainI->SelectNode(modelinfo.m_node);
	mainI->ForceCompleteRedraw(0);

	return HStatus::kSuccess;
}

HStatus MaxTools::SetLinkSizes(Interface *mainI,maxModelInfo &modelinfo)
{
	int numLinks = modelinfo.m_links.size();

	GUP *gp = (GUP*)CreateInstance( GUP_CLASS_ID,Class_ID(470000002,0));
	
	for (int linkIndex=0;linkIndex<numLinks;linkIndex++)
	{
		mainI->SelectNode(modelinfo.m_links[linkIndex]);
		char commandString[80];
		sprintf(commandString,"$.transform.controller.goalSize = %f",modelinfo.m_linkSizes[linkIndex]);
		// call a maxscript function to set the size of the end effector
		gp->ExecuteStringScript(commandString);
	}

	gp->DeleteThis();

	return HStatus::kSuccess;
}

HStatus MaxTools::CreatePolygons(Mesh *mesh) 
{
	HStatus status;
	int i;

	if (theModel->patchArray->length() == 0)
		return HStatus::kFailure;

	status = theModel->calculatePolygons();
	if (status != HStatus::kSuccess)
		return status;

	int numVertsInPool = theModel->polygonArray->vertices.length();
	if (numVertsInPool==0)
	{
		status = HStatus::kFailure;
		status.setErrorString("Polygon missing vertices.");
		return status;
	}
	mesh->setNumVerts(numVertsInPool);

	for (i=0;i<numVertsInPool;i++)
	{
		HVector vector(theModel->polygonArray->vertices[i]);
		// rotate object along x -90 degrees
		double temp = vector.y;
		vector.y = -vector.z;
		vector.z = temp;
		mesh->setVert(i,ConvVector(vector,unitratio));
	}

	int numUVVertsInPool = theModel->polygonArray->uvs.length();
	if (numUVVertsInPool!=0)
	{
		mesh->setMapSupport(1);
		
		mesh->setNumMapVerts(1,numUVVertsInPool);
		for (i=0;i<numUVVertsInPool;i++)
		{
			HVector2D vector(theModel->polygonArray->uvs[i]);
			vector.y += 1;
			mesh->setMapVert(1,i,ConvVector(vector));
		}
	}

	// calculate number of faces
	int numFaces=0;
	int pointTotal;
	// loop through polygons and count the number of faces and vertices
	for (HPoly *poly = theModel->polygonArray->begin();poly != NULL;poly=poly->next()) 
	{
		pointTotal = poly->vertexIndicies.size();
		if (pointTotal<3) continue; // kick out if this one is empty has less than 3 sides
		numFaces += pointTotal-2; // 3->1 4->2 5->3
	}
	mesh->setNumFaces(numFaces);
	mesh->setNumMapFaces(1,numFaces);
	TVFace *uvFaces = mesh->mapFaces(1);
	int faceIndex = 0;
	int edge0vis,edge2vis;
	for (poly = theModel->polygonArray->begin();poly != NULL;poly=poly->next()) 
	{
		pointTotal = poly->vertexIndicies.size();
		if (pointTotal<3) continue; // kick out if this one is empty has less than 3 sides
		for (int i=2;i<pointTotal;i++)
		{
			mesh->faces[faceIndex].setVerts(poly->vertexIndicies[0],
											poly->vertexIndicies[i-1],
											poly->vertexIndicies[i]);
			if (poly->hasUVs())
			{
				uvFaces[faceIndex].setTVerts(poly->uvIndicies[0],
											 poly->uvIndicies[i-1],
											 poly->uvIndicies[i]);
			}
			edge0vis = (i==2) ? EDGE_VIS : EDGE_INVIS;
			edge2vis = (i==pointTotal-1) ? EDGE_VIS : EDGE_INVIS;

//			mesh->faces[faceIndex].setSmGroup(1); // all smoothed the same
			mesh->faces[faceIndex].setSmGroup(poly->smoothGrpID);
			
//			if (ui_flags.assignshaders)
//			{
				int numShaderIndicies = poly->shaderIndicies.size();
				int bestShaderFactor=-1;
				int bestShaderIndex;

				// find the best shader to use for this face
				for (int shaderIndex=0;shaderIndex<numShaderIndicies;shaderIndex++)
				{
					HShader *shader = theModel->polygonArray->shaders.get(poly->shaderIndicies[shaderIndex]);

					int shaderFactor = shader->hasMaps() ? 100 : 0;
					shaderFactor += !shader->flags.kIsAGP ? 100 : 0;
					if (bestShaderFactor <= shaderFactor)
					{
						bestShaderIndex = shaderIndex;
						bestShaderFactor = shaderFactor;
					}
				}
				int temp = poly->shaderIndicies[bestShaderIndex];
				mesh->faces[faceIndex].setMatID(poly->shaderIndicies[bestShaderIndex]);

//			}
			mesh->faces[faceIndex++].setEdgeVisFlags(edge0vis, 1, edge2vis);

		}
	}
	mesh->buildNormals();
	mesh->buildBoundingBox();
	mesh->InvalidateEdgeList();
	return HStatus::kSuccess;
}

HStatus MaxTools::CreateMaterials(Interface *mainI) 
{
	HShader *shader;

	for (unsigned i = 0; i < theModel->polygonArray->shaders.length(); i++)
	{
		shader = theModel->polygonArray->shaders.get(i);
		CreateMaterial(mainI,i);
	}
	
	return HStatus::kSuccess;
}

static Color ColorFromHColor(const HColor &c) 
{
	Color a;
	a.r = c.r;
	a.g = c.g;
	a.b = c.b;
	return a;
}

HStatus MaxTools::CreateMaterial(Interface *mainI,int index)
{
	HStatus status;
	HShader *shader = theModel->polygonArray->shaders.get(index);
	StdMat2 *m;
	Mesh *mesh = NULL;
	m = NewDefaultStdMat();
	m->SetName(TSTR(shader->name.asChar()));

	SimHashShaderNetwork(shader,m);

	loadMtls.AddMtl(m);		
	return HStatus::kSuccess;
}

TSTR MaxTools::GetRelativePath(const char *mapPath) {
	TSTR relativePath,fullPath,modelFullPath,mapFullPath,name,ext;
	// compute the full path to the model from our current location
	FilePath::GetPathsToFile(relativePath,modelFullPath,name,ext,theModel->filename.asChar());
	// compute the full map path from the model location
	FilePath::GetPathsToFile(relativePath,mapFullPath,name,ext,mapPath,modelFullPath);
	// compute the relative path from our current location to the map
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,mapFullPath);
	return relativePath;
}

BitmapTex *MaxTools::MakeTextureMap(HMap *map) {
	TSTR relativePath = GetRelativePath(map->path.asChar());

	BitmapTex *bmt = NewDefaultBitmapTex();

	bmt->SetMapName(relativePath);

	Bitmap *bm = bmt->GetBitmap(0);

	StdUVGen *uv = bmt->GetUVGen();

	float uoffset,voffset;

	if ((int)map->repeat.x % 2 > 0)
		uoffset = 0;
	else
		uoffset = (1.0/map->repeat.x) * 0.5;

	if ((int)map->repeat.y % 2 > 0)
		voffset = 0;
	else
		voffset = (1.0/map->repeat.y) * 0.5;

	uv->SetUOffs(uoffset,0);
	uv->SetVOffs(voffset,0);

	uv->SetUScl(map->repeat.x,0);
	uv->SetVScl(map->repeat.y,0);

	if (bm) {
		if (bm->HasAlpha())
			bmt->SetPremultAlpha(false);
	}
	
	return bmt;
}

MultiTex *MaxTools::MakeMixTexture(int count,int index,std::vector<int> &indicies,HShader *shader) {
	MultiTex *mixtex = (MultiTex *)CreateInstance(TEXMAP_CLASS_ID, Class_ID(MIX_CLASS_ID,0));
	int index2 = index;
	
	int n = indicies[0],m = indicies[1];
	HMap *map0 = theModel->polygonArray->maps.get(shader->mapIndicies[indicies[index]]);
	HMap *map1 = theModel->polygonArray->maps.get(shader->mapIndicies[indicies[index+1]]);

	if (count>2) {
		MultiTex *mixtex0 = MakeMixTexture(count-1,index+1,indicies,shader);
		mixtex->SetSubTexmap(0,mixtex0);
		BitmapTex *bmt = MakeTextureMap(map0);
		mixtex->SetSubTexmap(1,bmt);
	}
	else {
		BitmapTex *bmt0 = MakeTextureMap(map0);
		BitmapTex *bmt1 = MakeTextureMap(map1);
		mixtex->SetSubTexmap(0,bmt0);
		mixtex->SetSubTexmap(1,bmt1);
	}
	return mixtex;
}

MultiTex *MaxTools::MakeMixTextureChain(std::vector<int> &indicies,HShader *shader) {
	return MakeMixTexture(indicies.size(),0,indicies,shader);
}

MultiTex *MaxTools::MakeRGBMultiplyTexture(int count,int index,std::vector<int> &indicies,HShader *shader) {
	MultiTex *mixtex = (MultiTex *)CreateInstance(TEXMAP_CLASS_ID, Class_ID(RGBMULT_CLASS_ID,0));
	int index2 = index;
	
	int n = indicies[0],m = indicies[1];
	HMap *map0 = theModel->polygonArray->maps.get(shader->mapIndicies[indicies[index]]);
	HMap *map1 = theModel->polygonArray->maps.get(shader->mapIndicies[indicies[index+1]]);

	if (count>2) {
		MultiTex *mixtex0 = MakeRGBMultiplyTexture(count-1,index+1,indicies,shader);
		mixtex->SetSubTexmap(0,mixtex0);
		BitmapTex *bmt = MakeTextureMap(map0);
		mixtex->SetSubTexmap(1,bmt);
	}
	else {
		BitmapTex *bmt0 = MakeTextureMap(map0);
		BitmapTex *bmt1 = MakeTextureMap(map1);
		mixtex->SetSubTexmap(0,bmt0);
		mixtex->SetSubTexmap(1,bmt1);
	}
	return mixtex;
}

MultiTex *MaxTools::MakeRGBMultiplyTextureChain(std::vector<int> &indicies,HShader *shader) {
	return MakeRGBMultiplyTexture(indicies.size(),0,indicies,shader);
}
MultiTex *MaxTools::MakeCompositeTexture(int count,HMap::mType mapType,HShader *shader) {
	MultiTex *compositetex = (MultiTex *)CreateInstance(TEXMAP_CLASS_ID, Class_ID(COMPOSITE_CLASS_ID,0));

	compositetex->SetNumSubTexmaps(count);

	int subtexmapidx = 0;
	int numMapIndicies = shader->mapIndicies.size();
	for (int i = 0; i < numMapIndicies; i++) {
		HMap *map = theModel->polygonArray->maps.get(shader->mapIndicies[numMapIndicies-i-1]);
		if (map->type == mapType) {
			BitmapTex *bmt = MakeTextureMap(map);
			compositetex->SetSubTexmap(subtexmapidx,bmt);
			subtexmapidx++;
		}
	}
	return compositetex;
}

void MaxTools::MakeCookieCutTexture(int count,StdMat2 *m,HShader *shader) {
	int numMapIndicies = shader->mapIndicies.size();
	for (int i = 0; i < numMapIndicies; i++) {
		HMap *map = theModel->polygonArray->maps.get(shader->mapIndicies[numMapIndicies-i-1]);
		if (map->type == HMap::COOKIECUTMAP) {
			BitmapTex *bmt = MakeTextureMap(map);
			bmt->SetAlphaAsMono(true);
			m->SetSubTexmap(15,bmt);
			m->EnableMap(15,1);
			return;
		}
	}
}

HMap *MaxTools::GrabFirstMap(HMap::mType mapType,HShader *shader) {
	int numMapIndicies = shader->mapIndicies.size();
	for (int i = 0; i < numMapIndicies; i++) {
		HMap *map = theModel->polygonArray->maps.get(shader->mapIndicies[numMapIndicies-i-1]);
		if (map->type == mapType) {
			return map;
		}
	}
	return NULL;
}

void MaxTools::SimHashShaderNetwork(HShader *shader,StdMat2 *m) {

	m->SwitchShader(Class_ID(0x2857f470, 0));//Sets shader to Multi_Layer
	
	m->SetAmbient(Color(0,0,0),0);
	m->SetDiffuse(ColorFromHColor(shader->diffusecolor),0);
	m->SetSpecular(ColorFromHColor(shader->specularcolor),0);
	m->SetOpacity(1.0-shader->transparency.r,0);

	if (!shader->hasMaps()) return;

	std::vector<int> diffuse_indicies;

	int cookiecut_count = 0, color_count = 0, diffuse_count = 0;

	int numMapIndicies = shader->mapIndicies.size();
	for (int i = 0; i < numMapIndicies; i++) {
		HMap *map = theModel->polygonArray->maps.get(shader->mapIndicies[numMapIndicies-i-1]);
		if (map->type == HMap::COLORMAP)
			color_count++;
		else if (map->type == HMap::DIFFUSEMAP) {
			diffuse_count++;
			diffuse_indicies.push_back(numMapIndicies-i-1);
		}
		else if (map->type == HMap::COOKIECUTMAP)
			cookiecut_count++;
	}
	MultiTex *compositetex;

	if (color_count > 0) {
		if (color_count > 1) {
			compositetex = MakeCompositeTexture(color_count,HMap::COLORMAP,shader);
			m->SetSubTexmap(ID_DI,compositetex);
			m->EnableMap(ID_DI,1);
		}
		else {
			HMap *map = GrabFirstMap(HMap::COLORMAP,shader);
			if (map) {
				BitmapTex *bmt = MakeTextureMap(map);
				m->SetSubTexmap(ID_DI,bmt);
				m->EnableMap(ID_DI,1);
			}
		}
	}

	if (diffuse_count > 0) {
		if (diffuse_count > 1) {
			compositetex = MakeRGBMultiplyTextureChain(diffuse_indicies,shader);
			m->SetSubTexmap(2,compositetex);
			m->EnableMap(2,1);
		}
		else {
			HMap *map = GrabFirstMap(HMap::DIFFUSEMAP,shader);
			if (map) {
				BitmapTex *bmt = MakeTextureMap(map);
				m->SetSubTexmap(2,bmt);
				m->EnableMap(2,1);
			}
		}
	}

	if (cookiecut_count > 0) {
		if (cookiecut_count > 1) {
			compositetex = MakeCompositeTexture(cookiecut_count,HMap::COOKIECUTMAP,shader);
			m->SetSubTexmap(ID_DI,compositetex);
			m->EnableMap(ID_DI,1);
			MakeCookieCutTexture(cookiecut_count,m,shader);
		}
		else {
			HMap *map = GrabFirstMap(HMap::COOKIECUTMAP,shader);
			if (map) {
				BitmapTex *bmt = MakeTextureMap(map);
				m->SetSubTexmap(ID_DI,bmt);
				m->EnableMap(ID_DI,1);
				MakeCookieCutTexture(cookiecut_count,m,shader);
			}
		}
	}
}

HStatus MaxTools::AssignMaterials(Interface *mainI,INode *node,Mesh *mesh)
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
			
//			m->ClearAFlag(A_LOCK_TARGET); // keeps material from getting auto-deleted
		}
		newmat->SetName(node->GetName());
//		if (!mainI->OkMtlForScene(newmat))
//		{
//			mainI->AssignNewName(newmat);
//		}
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
	return HStatus::kSuccess;
}


