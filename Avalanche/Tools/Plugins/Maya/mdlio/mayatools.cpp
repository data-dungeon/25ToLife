#include "stdafx.h"
#include "mayatools.h"

mayaTools::mayaTools()
{
	model = new mdlObject;
}

mayaTools::~mayaTools()
{
	delete model;
}

mdlObject *mayaTools::GetModel()
{
	return model;
}

HStatus mayaTools::BezierToCubicNurbCurve(HSpline *spline,unsigned nSpline) {
	std::vector<int> patchIndicies;
	for (HCP *iter = spline->head(); iter != NULL; iter = iter->next()) {
		HSplineSegment seg = spline->splineArray->getSplineSegment(iter->cpindex,iter->next()->cpindex,patchIndicies);
		seg.finalize();
		BezierSegmentToCubicNurbCurve(seg,nSpline);
	}
	return HStatus::kSuccess;
}

HStatus mayaTools::createSplineSegments() {
	HStatus status;

	status = model->calculateSplineTangents();

	if (status != HStatus::kSuccess)
		return status;
	
	for (HSpline *iter = model->splineArray.begin(); iter != NULL; iter = iter->next()) {
		status = BezierToCubicNurbCurve(iter,iter->index);
		if (status != HStatus::kSuccess)
			return status;
	}
	return HStatus::kSuccess;
}

HStatus mayaTools::createNurbPatches() {
	HStatus status;

	status = model->calculatePatchTangents();
	if (status != HStatus::kSuccess)
		return status;

	for (HPatch *iter = model->patchArray->begin();iter != NULL;iter = iter->next()) {
		status = BezierPatchToMayaCubicNurbPatches(iter,iter->patchIndex);
		if (status != HStatus::kSuccess)
			return status;
	}
	return HStatus::kSuccess;
}

void mayaTools::convertHVectorArrayToMPointArray(HVectorArray &hv, MPointArray &mv) {
	HVectorIterator iter(hv);
	MPoint p;
	HVector *v;

	mv.clear();

	for (;!iter.isDone();iter++) {
		v = *iter;
		p.x = v->x;
		p.y = v->y;
		p.z = v->z;
		mv.append(p);
	}
}

void mayaTools::convertHVectorArrayToMVectorArray(HVectorArray &hv, MVectorArray &mv) {
	HVectorIterator iter(hv);
	MPoint p;
	HVector *v;

	mv.clear();

	for (;!iter.isDone();iter++) {
		v = *iter;
		p.x = v->x;
		p.y = v->y;
		p.z = v->z;
		mv.append(p);
	}
}

void mayaTools::convertHIntArrayToMIntArray(HIntArray &hi,MIntArray &mi) {
	HIntIterator iter(hi);
	int i;

	mi.clear();

	for (;!iter.isDone();iter++) {
		i = **iter;
		mi.append(i);
	}
}

HStatus mayaTools::createShadingGroups(MObjectArray &SGList) {
	HShader *shader;
	MObjectArray shaderlist;

	SGList.clear();
	shaderlist.clear();

	for (unsigned i = 0; i < model->polygonArray->shaders.length(); i++) {
		shader = model->polygonArray->shaders.get(i);
		createShadingGroup(shader,i,SGList,shaderlist);
	}
	return HStatus::kSuccess;
}

HStatus mayaTools::createShadingGroup(HShader *shader,unsigned sgindex,MObjectArray &SGList,MObjectArray &shaderlist) {
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MDagModifier dagModifier;
	MFnSet fnSet;
	MFnBlinnShader blinn;
	MString shadername;
	MColor diffusecolor;

	selection.clear();

	MObject shadingGroup = fnSet.create(selection, MFnSet::kRenderableOnly, &status);
	if (status != MS::kSuccess) {
		DISPLAY_ERROR("createshadinggroup: Couldn't create shading group set.");
	}

	fnSet.clear();

	SGList.append(shadingGroup);
	
	shadername = shader->name.asChar();

	status = dgModifier.renameNode(SGList[sgindex],shadername+"_SG");
	if (status != MS::kSuccess) {
		DISPLAY_ERROR("createshadinggroup: Couldn't rename shader.");
	}

	MObject blinnShader = blinn.create(TRUE,&status);
	if (status != MS::kSuccess) {
		DISPLAY_ERROR("createshadinggroup: Couldn't create blinn shader.");
	}

	MFnBlinnShader blinnS(blinnShader);

	diffusecolor = MColor(MColor::kRGB,shader->diffusecolor.r,shader->diffusecolor.g,shader->diffusecolor.b);

	status = blinnS.setColor(diffusecolor);
	if (status != MS::kSuccess) {
		DISPLAY_ERROR("createshadinggroup: Couldn't set diffuse color.");
	}

	MColor black = MColor(MColor::kRGB,0.0,0.0,0.0);
	
	status = blinnS.setSpecularColor(black);
	if (status != MS::kSuccess) {
		DISPLAY_ERROR("createshadinggroup: Couldn't set specular color.");
	}

	MColor transparency = MColor(MColor::kRGB,shader->transparency.r,shader->transparency.g,shader->transparency.b);
	status = blinnS.setTransparency(transparency);

	dgModifier.doIt();

	shaderlist.append(blinnShader);

	status = dgModifier.renameNode(blinnShader,shadername);
	if (status != MS::kSuccess) {
		DISPLAY_ERROR("createshadinggroup: Couldn't rename shader.");
	}

	MString temp = SGList[sgindex].apiTypeStr();
	if (status != MS::kSuccess) {
		DISPLAY_ERROR("createshadinggroup: Couldn't get api type string.");
	}

	MFnSet fnShadingGroup(SGList[sgindex]);

	MGlobal::executeCommand (
		MString ("defaultNavigation -force true -connectToExisting -source ") +
		blinnS.name() +
		MString (" -destination ") +
		fnShadingGroup.name() );

	dgModifier.doIt();

	if (shader->hasMaps())
	{
		HMap *map = model->polygonArray->maps.get(shader->mapIndicies.back());
		if ((map->type == HMap::COLORMAP)||(map->type == HMap::COOKIECUTMAP))
				linkUpMapNode(blinnS.name(),".color",MString(map->path.asChar()),map->repeat);
		dgModifier.doIt();
	}

	return HStatus::kSuccess;
}

HStatus mayaTools::linkUpMapNode(MString shaderNode, MString attrName, MString mapName, HVector2D repeat) {
	MString mapNode;

	MGlobal::executeCommand (
			MString ("shadingNode -asTexture file"),
			mapNode );

	MGlobal::executeCommand (
			MString ("connectAttr ") + mapNode + MString (".outColor ") +
			shaderNode + attrName );

	MGlobal::executeCommand (
			MString ("setAttr -type \"string\" ") +
			mapNode + MString (".fileTextureName \"") +
			mapName + MString ("\"") );

	createPlacementNode(mapNode,(float)repeat.x,(float)repeat.y);

	return HStatus::kSuccess;
}

HStatus mayaTools::createPlacementNode(MString filenode, float x_repeat, float y_repeat) {
	MString placementnode;

	MGlobal::executeCommand (
		MString ("shadingNode -asUtility place2dTexture"),placementnode );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".coverage ") +
			filenode + MString (".coverage") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".translateFrame ") +
			filenode + MString (".translateFrame") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".rotateFrame ") +
			filenode + MString (".rotateFrame") );

//	MGlobal::executeCommand (
//		MString ("connectAttr ") + placementnode + MString (".mirror ") +
//			filenode + MString (".mirror") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".stagger ") +
			filenode + MString (".stagger") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".wrapU ") +
			filenode + MString (".wrapU") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".wrapV ") +
			filenode + MString (".wrapV") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".repeatUV ") +
			filenode + MString (".repeatUV") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".offset ") +
			filenode + MString (".offset") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".rotateUV ") +
			filenode + MString (".rotateUV") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".outUV ") +
			filenode + MString (".uv") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".outUvFilterSize ") +
			filenode + MString (".uvFilterSize") );

	MGlobal::executeCommand (
			MString ("setAttr ") +  placementnode + MString (".repeatU ") +
				MString ("-type double ") + x_repeat);

	MGlobal::executeCommand (
			MString ("setAttr ") +  placementnode + MString (".repeatV ") +
				MString ("-type double ") + y_repeat);
	
	return HStatus::kSuccess;
}

HStatus mayaTools::createPolygonTables(MPointArray &pa,
									   MIntArray &polyCounts,
									   MIntArray &polyConnects,
									   MVectorArray &normals,
									   MIntArray &normalconnects,
									   MFloatArray &uArray,
									   MFloatArray &vArray,
									   MIntArray &uvIds,
									   MIntArray &uvCounts,
									   HIntArray &sgIds)
{
	MPoint p;
	HVector n;
	unsigned numVerts=0;
	HIntIterator v;
	HVector2D *uvs;
	unsigned idx;
	unsigned vtxid;
	HShader *shader;
	bool flag;

	pa.clear();
	normals.clear();
	polyCounts.clear();
	polyConnects.clear();
	uArray.clear();
	vArray.clear();
	uvIds.clear();
	uvCounts.clear();
	sgIds.clear();

	if (model->polygonArray->vertices.length() > 0)
		convertHVectorArrayToMPointArray(model->polygonArray->vertices,pa);
	else { // No geometry.
		return HStatus::kSuccess;
	}

	for (HPoly *poly = model->polygonArray->begin();poly!=NULL;poly=poly->next()) {
		if (poly->hasVertices()) {
			numVerts = poly->vertexIndicies.size();
			polyCounts.append(numVerts);

			if (poly->hasUVs())
				uvCounts.append(numVerts);

			for (unsigned i = 0; i < numVerts; i++) {
				vtxid = poly->vertexIndicies[i];
				polyConnects.append(vtxid);

				if (poly->hasNormals()) {
					int debug = poly->normalIndicies[i];
					n = model->polygonArray->normals[poly->normalIndicies[i]];
					n.normalize();
					normals.append(MVector(n.x,n.y,n.z));
				}

				if (poly->hasUVs()) {
					idx = uArray.length();
					int debug = poly->uvIndicies[i];
					uvs = model->polygonArray->uvs.get(poly->uvIndicies[i]);
					uArray.append((float)uvs->x);
					vArray.append((float)uvs->y);
					uvIds.append(idx);
				}
			}

			if (model->ui_flags.assignshaders)
			{
				flag = false;
				for (int i = poly->shaderIndicies.size()-1; i >= 0; i--)
				{
					shader = model->polygonArray->shaders.get(poly->shaderIndicies[i]);
					if (!shader->flags.kIsAGP)
					{
						sgIds.append(poly->shaderIndicies[i]);
						i = -1; //hack
						flag = true;
					}
				}
				if (!flag)
					sgIds.append(0);
			}
		}
	}
	return HStatus::kSuccess;
}

MString mayaTools::HashBoneNameToMayaBoneName(const char *bonename)
{
	MString bname = bonename;
	MTools::FindAndReplace(bname,"_","_____");
	MTools::FindAndReplace(bname," ","_");
	MTools::FindAndReplace(bname,"-","__");
	MTools::FindAndReplace(bname,"!","___");
	MTools::FindAndReplace(bname,"#","____");
	return bname;
}

HStatus mayaTools::createSkeleton()
{
	MStatus status;
	HStatus stat;
	MFnTransform fnTransform;
	HBoneChain *bonechain;
	HBone *bone;
	MDGModifier dgModifier;
	MDagModifier dagModifier;

	MObject polyObjectShape = m_dagpath.child(0);
	MDagPath childdagpath;
	MFnDagNode dagnode(polyObjectShape);
	dagnode.getPath(childdagpath);

	stat = model->createSkeleton();
	if (stat != HStatus::kSuccess)
		return stat;

	int index;
	std::vector<int> parentlist;
	std::vector<bool> flaglist;
	std::vector<MDagPath> nodes;
	
	int bonechainlength;
	HBoneArray *bonearray = &model->patchArray->boneArray;

	if (bonearray->length()<=0)
		return HStatus::kSuccess;

	HBoneChainArray newbonechainarray;
	HBoneChainArray *bonechains = &newbonechainarray;
	model->patchArray->boneChainArray.DestroyControlBones(newbonechainarray);

	int sumlengths = bonearray->length()+bonechains->length();
	nodes.resize(sumlengths+bonechains->length());
	parentlist.resize(sumlengths);
	flaglist.resize(sumlengths);
	
	for (int i = 0; i < sumlengths; i++)
		flaglist[i] = false;

	// Rename Mesh
	dgModifier.renameNode(m_dagpath.node(),MString(model->filename.getFileNameWithoutExt().asChar())+MString("_mesh"));
	if (!status)
		MGlobal::displayError("Couldn't rename mesh.");

	// Create Model Bone
	MDagPath modelbone,modelendbone;
	if (!MTools::CreateBone(modelbone,MObject::kNullObj,MVector(0,0,0),NULL))
		return HStatus::kFailure;

	dgModifier.renameNode(modelbone.node(),MString(model->filename.getFileNameWithoutExt().asChar()));
	if (!status)
		MGlobal::displayError("Couldn't rename model joint.");

	if (!MTools::CreateBone(modelendbone,MObject::kNullObj,MVector(0,0,10),NULL))
		return HStatus::kFailure;
	
	dgModifier.renameNode(modelendbone.node(),MString(model->filename.getFileNameWithoutExt().asChar())+MString("_end"));
	if (!status)
		MGlobal::displayError("Couldn't rename model end joint.");

	MTools::ParentMObject(modelendbone.node(),modelbone.node());
	
//	Build Bone Tree
	for (i = 0; i < (int)bonechains->length(); i++)
	{
		bonechain = bonechains->get(i);

		bonechainlength = bonechain->length();

		MString bonename;

		for (int j = 0; j < bonechainlength; j++)
		{
			bone = bonechain->get(bonechainlength-j-1);

			index = bonechain->getIndex(bonechainlength-j-1);

			const char *debug = bone->name.asChar();

//			collect bone transform info
			HVector jointpivot = bone->getPivot();
			MVector jointpivottranslation(jointpivot.x,jointpivot.y,jointpivot.z);
			MQuaternion orientation;
			orientation.x = bone->rotate.v.x;
			orientation.y = bone->rotate.v.y;
			orientation.z = bone->rotate.v.z;
			orientation.w = bone->rotate.w;

			MDagPath newbone;
			if (MTools::CreateBone(newbone,MObject::kNullObj,jointpivottranslation,&orientation))
			{
				nodes[index] = newbone;

				bonename = HashBoneNameToMayaBoneName(bone->name.asChar());
				const char*debug = bonename.asChar();

				if (bone->flags.kHasParent)
				{
					parentlist[index] = bone->parentIndex;
					flaglist[index] = true;
				}

				dgModifier.renameNode(nodes[index].node(),bonename);
				if (!status)
					MGlobal::displayError("Couldn't rename joint.");

				dgModifier.doIt();
			}
		}
//		Add extra bone to end of chain
		if (bonechain->length()>0)
		{
//			collect bone transform info
			HVector jointpivot = bonechain->end()->getEnd();
			MVector jointpivottranslation(jointpivot.x,jointpivot.y,jointpivot.z);

			MDagPath newbone;
			if (MTools::CreateBone(newbone,MObject::kNullObj,jointpivottranslation,NULL))
			{
				nodes[bonearray->length()+i] = newbone;
				flaglist[bonearray->length()+i] = true;
				parentlist[bonearray->length()+i] = bonechain->getIndex(bonechainlength-1);//parent to last bone of chain

				MString endbonename = HashBoneNameToMayaBoneName(bonechain->end()->name.asChar()) + MString("_end");
				const char*debug = endbonename.asChar();
				
				dgModifier.renameNode(nodes[bonearray->length()+i].node(),endbonename);
				if (!status)
					MGlobal::displayError("Couldn't rename joint.");

				dgModifier.doIt();
			}
		}
	}
	
//	Assign hierarchy to bones
	for (i = 0; i < sumlengths; i++)
	{
		if (nodes[i].fullPathName() == "")
			continue;

		if (flaglist[i])
			MTools::ParentMObject(nodes[i].node(),nodes[parentlist[i]].node());
		else
			MTools::ParentMObject(nodes[i].node(),modelbone.node());
	}
	dgModifier.doIt();
	
/*	Don't create IK.
	MObject first,last;
	for (i = 0; i < bonechains->length(); i++){
		bonechain = bonechains->get(i);
		if (bonechain->length()>1) {
			bonechain = bonechains->get(i);

			first = nodes[bonechain->getIndex(0)];
			last = nodes[bonearray->length()+i];

			MDagPath firstpath,lastpath;
			status = MDagPath::getAPathTo(first,firstpath);
			if (status == MS::kSuccess)
			{
				status = MDagPath::getAPathTo(last,lastpath);
				if (status == MS::kSuccess)
				{
				MFnIkHandle ikhandle;
				ikhandle.create(firstpath,lastpath);
				}
			}
		}
	}
*/

	std::vector<MSelectionList> vertexgroups;
	std::vector<MDagPath> bones;

	MObject vertex;
	for (i = 0; i < (int)bonechains->length(); i++)
	{
		bonechain = bonechains->get(i);

		bonechainlength = bonechain->length();

		for (int j = 0; j < bonechainlength; j++)
		{
			bone = bonechain->get(j);

			index = bonechain->getIndex(j);

			if (bone->vids.size()==0)
				continue;

			bones.push_back(nodes[index]);
			
			MSelectionList vertices;
			vertices.clear();

			for (int k = 0; k < (int)bone->vids.size(); k++)
			{
				MItMeshVertex vit(childdagpath,MObject::kNullObj);
				int prev;
				vit.setIndex(bone->vids[k],prev);
				vertices.add(childdagpath,vit.vertex());
			}

			vertexgroups.push_back(vertices);
		}
//		Add end bones into BindSkin
//		MSelectionList vertices;
//		bones.push_back(nodes[bonearray->length()+i]);
//		vertexgroups.push_back(vertices);
	}

	MTools::BindSkin(m_dagpath,bones,vertexgroups);

	return HStatus::kSuccess;
}

HStatus mayaTools::createPolygons()
{

	if (model->patchArray->length() == 0)
		return HStatus::kFailure;

	MFnMesh fnMesh;
	MStatus status;
	MPointArray pa;
	MVectorArray normals;
	MIntArray polyCounts,polyConnects,normalconnects;
	MIntArray facelist;
	MFloatArray uArray,vArray;
	MIntArray uvCounts;
	MIntArray uvIds;
	HStatus stat;
	MObjectArray SGList;
	HIntArray sgIds;
	MDGModifier dgModifier;
	
	pa.clear();
	normals.clear();
	polyCounts.clear();
	polyConnects.clear();
	normalconnects.clear();

	stat = model->calculatePolygons();
	if (stat != HStatus::kSuccess)
		return stat;

	if (model->ui_flags.assignshaders)
		createShadingGroups(SGList);

	createPolygonTables(pa,polyCounts,polyConnects,normals,normalconnects,uArray,vArray,uvIds,uvCounts,sgIds);

	unsigned numVerts = pa.length();
	unsigned numPolys = polyCounts.length();

	////////////////////////////////////////////////////////////////////////////////////////
	// DEBUG INFO
	////////////////////////////////////////////////////////////////////////////////////////

	if (model->ui_flags.verbose) {
		unsigned sum = 0, sum2 = 0;

		for (unsigned j = 0; j < polyCounts.length(); j++) 
			sum += polyCounts[j];

		for (j = 0; j < uvCounts.length(); j++) 
			sum2 += uvCounts[j];

			std::cerr << std::endl << "+- DEBUG PASSING TO CREATE MESH  -----------------------------------+" << std::endl;

			std::cerr << "+----- mesh.create --------------+" << std::endl;
			std::cerr << "	Length of vertex table = " << pa.length() << std::endl;
			std::cerr << "	Length of polyConnects = " << polyConnects.length() << std::endl;
			std::cerr << "	Length of polyCounts = " << polyCounts.length() << std::endl;
			std::cerr << "	Length of normals = " << normals.length() << std::endl;
			std::cerr << "	Sum of polyCounts = " << sum << std::endl;
			std::cerr << "+----- uvs.create ---------------+" << std::endl;
			std::cerr << "	Length of uvIds = " << uvIds.length() << std::endl;
			std::cerr << "	Length of uvCounts = " << uvCounts.length() << std::endl;
			std::cerr << "	Sum of uvCounts = " << sum2 << std::endl;
			std::cerr << "	Length of uArray = " << uArray.length() << std::endl;
			std::cerr << "	Length of vArray = " << vArray.length() << std::endl;
			std::cerr << "+----- shaders.create -----------+" << std::endl;
			std::cerr << "	Length of sgIds = " << sgIds.length() << std::endl;
	}

	MObject polyObject = fnMesh.create( numVerts, numPolys, pa, polyCounts, polyConnects, MObject::kNullObj, &status );

	if (!status != MStatus::kSuccess)
	{
		MGlobal::displayError("Couldn't create mesh.");
		return HStatus::kFailure;
	}

	MFnDagNode dagnode(polyObject);
	dagnode.getPath(m_dagpath);
	MFnMesh fnMesh2(m_dagpath);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// set normals
	///////////////////////////////////////////////////////////////////////////////////////////////////
	facelist.clear();
	for (unsigned a = 0; a < polyCounts.length(); a++)
		for (unsigned b = 0; b < (unsigned)polyCounts[a]; b++)
			facelist.append(a);

	status = fnMesh2.setFaceVertexNormals(normals, facelist, polyConnects, MSpace::kTransform);
	if (!status)
		MGlobal::displayError("Failed to assign normals.");


	///////////////////////////////////////////////////////////////////////////////////////////////////
	// assign UV
	///////////////////////////////////////////////////////////////////////////////////////////////////
	MString uvSet;
	status = fnMesh2.getCurrentUVSetName(uvSet);
	if (!status)
		MGlobal::displayError("Couldn't get current uv set.");

	status = fnMesh2.setUVs( uArray, vArray ,&uvSet);
	if (!status)
		MGlobal::displayError("Couldn't create uv set.");

	status = fnMesh2.assignUVs( uvCounts, uvIds ,&uvSet);
	if (!status)
		MGlobal::displayError("Couldn't assign uv's.");

	dgModifier.renameNode(m_dagpath.node(),MString(model->filename.getFileNameWithoutExt().asChar()));
	dgModifier.doIt();

	MIntArray indicies;
	indicies.clear();
	for (int i = 0; i < (int)sgIds.length(); i++)
		indicies.append(sgIds[i]);

	if (model->ui_flags.createbones)
		createSkeleton();//Maya needs to create skeleton before connecting shaders

	if (model->ui_flags.assignshaders)
		MTools::ConnectShadingGroups(m_dagpath,SGList,indicies);

	return HStatus::kSuccess;
}

HStatus mayaTools::BezierSegmentToCubicNurbCurve(HSplineSegment &seg,unsigned nSpline) {
	MStatus status;
	HStatus stat;

	MDGModifier dgModifier;

//	The open and closed forms are quite similar, and in fact a closed curve
//	will become and open curve if either the first or last CV is moved so that
//	they are no longer in the same position. So to create an open or closed curve,
//	of degree N, with M spans, you must provide M+N CVs. This implies that for a
//	degree N curve, you must specify at least N+1 CVs to get a curve with a single
//	span.

//	The number of knots required for the curve will then be "M + 2*N - 1". As well, 
//	if you want the curve to start exactly at the first CV and end exactly at the
//	last CV, then the knot vector must be structured to have N degree "multiplicity"
//	at the beginning and end. This means that the first N knots must be identical,
//	and the last N knots must be identical.


	MFnNurbsCurve fnNurbCurve;

	MPointArray cvs;
	MDoubleArray uknots;

	unsigned ndegree = 3;

	HVector v0,v1,cv1,cv2;
	v0 = HVector(seg.v0.x,seg.v0.y,seg.v0.z);
	v1 = HVector(seg.v1.x,seg.v1.y,seg.v1.z);
	cv1 = HVector(seg.cv0.x,seg.cv0.y,seg.cv0.z);
	cv2 = HVector(seg.cv1.x,seg.cv1.y,seg.cv1.z);

	cvs.append(HVectorToMPoint(v0));
	cvs.append(HVectorToMPoint(cv1));
	cvs.append(HVectorToMPoint(cv2));
	cvs.append(HVectorToMPoint(v1));

	unsigned nknot;
	for (nknot = 0; nknot < 3; nknot ++) // beggining multiplicity
		uknots.append(0);

	for (nknot = 0; nknot < 3; nknot ++) // end multiplicty
		uknots.append(1);

	MObject nurbCurve = fnNurbCurve.create(cvs,uknots,ndegree,MFnNurbsCurve::kOpen,false,true,MObject::kNullObj,&status);

	if (status != MS::kSuccess) {
			DISPLAY_ERROR("Maya curve could not be created.");
	}

	char test[128];
	sprintf(test,"spline%i_0\n",nSpline);
	MString nodeName = test;
	dgModifier.renameNode(nurbCurve,nodeName);
	dgModifier.doIt();
	return HStatus::kSuccess;
}

HStatus mayaTools::BezierPatchToMayaCubicNurbPatches( HPatch *patch, unsigned nPatch) {
	HVector pVectors[16];

	unsigned numPatches = patch->patchVectors.length();
	unsigned numVectors;

	if (numPatches == 0)
		DISPLAY_ERROR("Can't find patch vectors to describe patch.");

	for (unsigned n = 0; n < numPatches; n++) {
		numVectors = patch->patchVectors[n].pv.length();

		if (numVectors == 16) {
			for (unsigned i = 0; i < 16; i++) {
				pVectors[i] = patch->patchVectors[n].pv[i];
			}
			BezierPatchToMayaCubicNurbPatch(pVectors, nPatch);
		}
		else {
			DISPLAY_ERROR("Can't find patch vectors to describe patch.");
		}
	}

	return HStatus::kSuccess;
}

HStatus mayaTools::BezierPatchToMayaCubicNurbPatch( HVector *patchVectors, unsigned nPatch ) {

	MStatus status;
	MDGModifier dgModifier;
	// SURFACES

	//The U and V knot vectors for NURBS surfaces are of size (spansInU + 2*degreeInU -1) and
	//(spansInV + 2*degreeInV -1). 
	//Note: spans = numCVs - degree.
	//So to create an open or closed surface, of degree N, with M spans, you must provide M+N
	//CVs. This implies that for a degree N surface, you must specify at least N+1 CVs to get
	//a surface with a single span.

	//The number of knots required for the surface will then be "M + 2*N - 1". As well, if you
	//want the surface to start exactly at the first CV and end exactly at the last CV, then
	//the knot vector must be structured to have N degree "multiplicity" at the beginning and end.
	//This means that the first N knots must be identical, and the last N knots must be identical.

	//	*controlVertices an array of control vertices 
	//	*uKnotSequences an array of U knot values 
	//	*vKnotSequences an array of V knot values 
	//	*degreeInU degree of first set of basis functions 
	//	*degreeInV degree of second set of basis functions 
	//	*formU open, closed, periodic in U 
	//	*formV open, closed, periodic in V 
	//	*createRational create as rational (true) or non-rational (false) surface 
	//

//	unsigned uknot_length = spansInU + 2*udegree-1;
//	unsigned vknot_length = spansInV + 2*vdegree-1;

	MFnNurbsSurface fnNurbSurface;

	MPointArray cvs;
	MDoubleArray uknots,vknots;
	unsigned udegree = 3,vdegree = 3;
	unsigned spansInU = 1, spansInV = 1;
	
	MPoint p0 = HVectorToMPoint(HVector(patchVectors[0]));
	MPoint p1 = HVectorToMPoint(HVector(patchVectors[12]));
	MPoint p2 = HVectorToMPoint(HVector(patchVectors[15]));
	MPoint p3 = HVectorToMPoint(HVector(patchVectors[3]));

//	if ((p0.distanceTo(p3)==0)&&(p1.distanceTo(p2)==0))// three point patch

//	if ((p0.distanceTo(p1)==0)&&(p3.distanceTo(p2)==0))// zero surface area

	///////////////////////////////////////////////////////////////////////////////

	for (unsigned i = 0; i < 16; i++)
		cvs.append(HVectorToMPoint(HVector(patchVectors[i])));

	for (unsigned nknot = 0; nknot < 3; nknot ++)
		uknots.append(0);

	for (nknot = 0; nknot < 3; nknot ++)
		uknots.append(1);

	for (nknot = 0; nknot < 3; nknot ++)
		vknots.append(0);

	for (nknot = 0; nknot < 3; nknot ++)
		vknots.append(1);

	MObject nurbPatch;

	nurbPatch = fnNurbSurface.create(cvs, uknots, vknots, udegree, vdegree, MFnNurbsSurface::kOpen,
									MFnNurbsSurface::kOpen, true, MObject::kNullObj, &status);

	if (status != MS::kSuccess) {
			DISPLAY_ERROR("Creating patch failed.");
	}

	char test[128];
	sprintf(test,"patch%i\n",nPatch);
	MString nodeName = test;
	dgModifier.renameNode(nurbPatch,nodeName);
	dgModifier.doIt();

	return HStatus::kSuccess;
}

MPoint mayaTools::HVectorToMPoint(const HVector &other) {
	MPoint p;
	p.x = other.x;	p.y = other.y;	p.z = other.z;
	return p;
}




