//////////////////////////////////////////////////////////////////////////////////////////
//			mdl fileio methods
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "harg.h"
#include "hfiletools.h"
#include "hmacros.h"
#include "hspline.h"
#include "himage.h"
#include "hpatch.h"
#include "hgroup.h"
#include "hdecal.h"
#include "hattr.h"
#include "hsplinearray.h"
#include "hpatcharray.h"
#include "hhook.h"
#include "hbonearray.h"
#include "mdlobject.h"
#include "hfiletools2.h"
#include "hbonechainarray.h"
#include "hdynamicproperties.h"
#include "huserproperties.h"

void HBias::readHBias(std::ifstream &fin)
{
	HArgArray args;

	fileTools::parseLine(fin,args);

	if (args.length() == 3){
		this->alpha = args.getFloat(0);
		this->gamma = args.getFloat(1);
		this->magnitude = args.getFloat(2);
	}
	else{
		this->alpha = args.getFloat(0);
		this->gamma = args.getFloat(0);
		this->magnitude = args.getFloat(0);
	}
}

void HCP::readHCP(std::ifstream &fin)
{
	HArgArray args;
	UString fline;
	UStringArray sarray;
	float hookpos;

	fileTools::parseLine(fin,args);

	if ( args.length() != 3 ) {
		if (args.length() == 4) {
			hookpos = (float)args.getFloat(3);
			if (hookpos != 0.5f) {
//				THROW_FATAL_ERROR("Hook position isn't in the middle of spline segment. [cp]");
			}
		}
		else{
			THROW_FATAL_ERROR("Incorrect number of arguments. Could be a corrupt file. [cp]");
		}
	}

	this->setFlagsFromOldFlags(args.getInt(0));

	int isInstance = args.getInt(1); // cp instance, not used
	
	fin >> fline;
	
	std::vector<int> n;
	HCP cp;

	if (!isInstance) {
		fileTools::parseLine(fline,args);

		if (args.length() != 3) {
			THROW_FATAL_ERROR("Incorrect number of arguments. Could be a corrupt file. [cp]");
		}

		HVector pos;
		pos.x = args.getFloat(0);
		pos.y = args.getFloat(1);
		pos.z = args.getFloat(2);

		this->create_UHNC(pos);
	}
	else 
		this->createInstance_UHNC(splineArray->HashCPIndexToMyCPIndex(fline.asInt()));

	this->inBias.readHBias(fin);
	this->outBias.readHBias(fin);
}

void HSpline::readHSpline(std::ifstream &fin) {
	HStatus status;
	UString label;
	HArgArray args;
	unsigned cpcount;
	HCP cp,headcp;
	bool isCPHookBase = false;
	HHook hook,*hookptr;
	unsigned oldcpindex;

	fileTools2::goToAttr(fin,"CPs",args,"[ENDMESH]");

	cpcount = args.getInt(0);

	for (unsigned nCP = 0; nCP < cpcount; nCP++) {
		cp.clear();
		cp.splineArray = this->splineArray;
		cp.readHCP(fin);

		if (nCP == 0)
			cp.flags.kIsHead = 1;

		headcp.clear();
		hook.clear();

		if ((nCP == cpcount-1)&&(!cp.flags.kIsLoop))
			cp.flags.kIsTail = 1;

		bool loop = false;

		if ((nCP == cpcount-1)&&(cp.flags.kIsLoop)) {
			headcp.create_looping_cp(*(this->head()));
			cp.flags.kIsLoop = 0;
			cp.flags.kIsHashLoop = 1;
			loop = true;
		}

		if (isCPHookBase) {
			cp.flags.kIsHookbaseNeighbor = 1;
			hook.hookbaseidx = oldcpindex;
			hookptr = this->splineArray->hookList.get(classList[classList.size()-1].hookIndex);
			hookptr->hookbaseneighboridx.push_back(cp.cpindex);
		}

		cp.initialize_hook(hook);
		this->add(cp);

		if (loop) {
			hook.clear();
			if (cp.flags.kIsHookbase) {
				headcp.flags.kIsHookbaseNeighbor = 1;
				hook.hookbaseidx = cp.cpindex;
				
				if ((((*this).head())->flags.kIsHook)||(((*this).head())->flags.kIsHookbase)||
					(((*this).head())->flags.kIsHookbaseNeighbor)) {

					((*this).head())->flags.kIsHookbaseNeighbor = 1;
					hookptr = this->splineArray->hookList.get(classList[0].hookIndex);
					hookptr->hookbaseidx = cp.cpindex;
					hookptr->flags.kIshookbaseneighbor = 1;
				}
				else {
					HHook hook2;
					((*this).head())->flags.kIsHookbaseNeighbor = 1;
					hook2.hookbaseidx = cp.cpindex;
					(*((*this).head())).initialize_hook(hook2);
				}

				hook.hookbaseidx = cp.cpindex;

				hookptr = this->splineArray->hookList.get(classList[classList.size()-1].hookIndex);
				hookptr->hookbaseneighboridx.push_back(headcp.cpindex);
				hookptr->hookbaseneighboridx.push_back((*(*this).head()).cpindex);
			}
			headcp.inBias = (this->head())->inBias;
			headcp.flags.kIsTail = 1;
			headcp.initialize_hook(hook);
			this->add(headcp);
		}

		if (cp.flags.kIsHookbase) // this flag is for the next pass through the loop
			isCPHookBase = true;
		else
			isCPHookBase = false;

		oldcpindex = cp.cpindex;
	}
}

void HSplineArray::analyzeMesh(std::ifstream &fin)
{
	UString fline,label;
	std::vector<int> cpindicies;
	HArgArray args;

	hashCPTable = new std::vector<int>;

	fin >> fline;
	fileTools2::goToAttr(fin,"Splines",args,"[ENDMESH]");
//	fileTools::parseLine(fin,"Splines",args);
	unsigned splinecount = args.getInt(0);
	for (unsigned nSpline = 0; nSpline < splinecount; nSpline++) {
		fileTools::parseLine(fin,"CPs",args);
		unsigned cpcount = args.getInt(0);
		for (unsigned nCP = 0; nCP < cpcount; nCP++) {	
			fileTools::parseLine(fin,args);

			hashCPTable->push_back(args.getInt(2));

			fin >> fline;
			fin >> fline;
			fin >> fline;
		}
	}
}

void HSplineArray::readHSplineArray(std::ifstream &fin)
{
	if (hashCPTable->size() == 0) {
//		THROW_FATAL_ERROR("No geometry detected.")
		return;
	}

	HStatus status;
	UString fline,label;
	HArgArray args;

	////////////////////////////////////////////////////////////////////////////////////
	// Initialize main index table
	////////////////////////////////////////////////////////////////////////////////////

	this->abscpindex = 0;

	cpIdxMap = new HIntMap();
	cpIndexTable = new std::vector<int>;

//	unsigned maxv = hashCPTable->maxValue();

	unsigned maxv = *std::max_element(hashCPTable->begin(),hashCPTable->end());

	cpIndexTable->resize(maxv+1);

	////////////////////////////////////////////////////////////////////////////////////
	// cpIdxTable makes the method HashCPIndexToMyCPIndex quick!
	////////////////////////////////////////////////////////////////////////////////////
	
//	HIntIterator iter;
	std::vector<int> values;

//	for (iter.reset(*hashCPTable);!iter.isDone(); iter++)
//		values.append(**iter);
	for (unsigned i = 0; i < hashCPTable->size(); i++)
		values.push_back((*hashCPTable)[i]);


	cpIdxMap->setValues(*hashCPTable,values);

	////////////////////////////////////////////////////////////////////////////////////

	fileTools2::goToAttr(fin,"Splines",args,"[ENDMESH]");

	unsigned splineCount = args.getInt(0);

	HSpline spline;

	for (unsigned nSpline = 0; nSpline < splineCount; nSpline++) {
		spline.clear();
		spline.splineArray = this;
		spline.index = nSpline;
		spline.readHSpline(fin);
	 	this->append(spline);
	}

	this->setupCPInfo();
	this->setHooks();
	fileTools::skipTo(fin,"[ENDMESH]");
	
	delete hashCPTable;

}

void HImage::readHImage(std::ifstream &fin)
{
	UString fline;
	UString label;
	HArgArray args;

	fin >> fline;

	fileTools::parseLine(fline,label,args);

	if (label == "Name")
		if (fline == "Name=")
			this->pathname = "";
		else
			this->pathname = args.getWholeString();
	else if (label == "FileName")
		this->pathname = args.getWholeString();
	else {
		THROW_FATAL_ERROR("Corrupt file.");
	}

	if (model->version < 9.0f)
		this->name = this->pathname;
	else
		this->name = this->pathname.getFileNameWithoutExt();


	fileTools::skipTo(fin,"[ENDSTILL]");
}

void HPatch::readHPatch(std::ifstream &fin)
{
	UString fline;
	UString label;
	HArgArray args;
	HStatus status;

	fin >> fline;
	fileTools::parseLine(fline,args);

	if (args.length() > 1) {
		THROW_FATAL_ERROR("Incorrect number of arguments. Could be a corrupt file. [patch]");
	}
	this->setFlagsFromOldFlags(args.getInt(0));

	fin >> fline;

	fileTools::parseLine(fline,args);
	std::vector<int> cparray;

	if (this->flags.kIs5point) {
		this->cpid1.resize(5);
		this->cpid2.resize(5);
		for (unsigned n = 0; n < 5; n++) {
			this->cpid1[n] = patchArray->model->splineArray.HashCPIndexToMyCPIndex(args.getInt(n));
//			this->nid[n] = args.getInt(n+5)-1;//normals
		}
		this->attrIndex = args.getInt(10);
	}
	else {
		this->cpid1.resize(4);
		this->cpid2.resize(4);

		for (unsigned n = 0; n < 4; n++) {
			this->cpid1[n] = patchArray->model->splineArray.HashCPIndexToMyCPIndex(args.getInt(n));
//			this->nid[n] = args.getInt(n+4)-1;//normals
		}

		// FLAG 3 POINT PATCH BECAUSE HASH DOESN'T
		if (this->cpid1[3] == this->cpid1[0])
			this->flags.kIs3point = 1;

		this->attrIndex = args.getInt(8);
	}
}

void HPatchArray::readHPatchArray(std::ifstream &fin)
{
	UString label;
	HArgArray args;
	unsigned count3pt=0,count4pt=0,count5pt=0;

	fileTools::parseLine(fin,"Count",args);

	unsigned patchCount = args.getInt(0);// Number of quad patches

	HPatch patch;

	unsigned total_patch_count = patchCount;

	// READ IN 4 POINT AND 3 POINT PATCHES

	for (unsigned nPatch = 0; nPatch < patchCount; nPatch++)
	{
		patch.patchArray = this;
		patch.readHPatch(fin);
	 	this->append(patch);

		if (patch.flags.kIs3point)
			count3pt ++;
		else
			count4pt ++;
	}

	// READ IN 5 POINT PATCHES

	fileTools::parseLine(fin,args);

	patchCount = args.getInt(0);//Number of 5 point patches

	total_patch_count += patchCount;

	for (nPatch = 0; nPatch < patchCount; nPatch++)
	{
		patch.patchArray = this;
		patch.readHPatch(fin);
	 	this->append(patch);
		count5pt++;
	}

	fileTools::skipTo(fin,"[ENDPATCHES]");
}

void HGroup::readHGroup( std::ifstream &fin, HPatchArray *patches) {
	UString fline,label;
	HArgArray args;
	unsigned count;
	std::vector<int> cparray;
	HDecal decal;
	bool found_color = false;

	HSplineArray *splines = &patches->model->splineArray;
	fin >> fline;

	while (fline != "[ENDGROUP]") {

		fileTools::parseLine(fline,label,args);

		if (label == "Name") {
			if (fline == "Name=")
				this->name = "";
			else
				this->name = args.getString(0);
		}
		else if (label == "Count") {
			count = args.getInt(0);
			cparray.clear();
			for (unsigned i = 0; i < count; i++) {
				fileTools::parseLine(fin,args);
				cparray.push_back(splines->HashCPIndexToMyCPIndex(args.getInt(0)));
			}
			// THE CP ARRAY IN A GROUP IS A LARGE SERIES OF CP INDICIES DEFINING ALL PATCHES ASSOCIATED.
			bool successful = patches->derivePatchIndicies(cparray,true,this->patchIndicies,0);

			if (!successful)
			{
				UString error = UString("Can't find patch in group. ") + this->name;
//				THROW_FATAL_ERROR(error.asChar());
			}
		}
		if (label == "DiffuseColor") {//HASH COLORS ARE STORED AS BGR
			this->attribute.diffusecolor.r = args.getFloat(2);
			this->attribute.diffusecolor.g = args.getFloat(1);
			this->attribute.diffusecolor.b = args.getFloat(0);
			found_color = true;
		}
		else if (label == "DiffuseFalloff")
			this->attribute.diffusefalloff = (float)args.getFloat(0);
		else if (label == "Ambiance") 
			this->attribute.ambiance = (float)args.getFloat(0);
		else if (label == "Roughness")
			this->attribute.roughness = (float)args.getFloat(0);
		else if (label == "RoughnessScale")
			this->attribute.roughnesscale = (float)args.getFloat(0);
		else if (label == "Reflectivity")
			this->attribute.reflectivity = (float)args.getFloat(0);
		else if (label == "Transparency")
		{
			this->attribute.transparency.r = args.getFloat(0);
			this->attribute.transparency.g = args.getFloat(0);
			this->attribute.transparency.b = args.getFloat(0);
		}
		else if (label == "Refraction")
			this->attribute.refraction = (float)args.getFloat(0);
		else if (label == "SpecularColor")
		{
			this->attribute.specularcolor.r = args.getFloat(2);
			this->attribute.specularcolor.g = args.getFloat(1);
			this->attribute.specularcolor.b = args.getFloat(0);
		}
		else if (label == "SpecularSize")
			this->attribute.specularsize = (float)args.getFloat(0);
		else if (label == "SpecularIntensity")
			this->attribute.specularintensity = (float)args.getFloat(0);
		else if (label == "Density")
			this->attribute.density = (float)args.getFloat(0);
		else if (label == "Translucency")
			this->attribute.translucency = (float)args.getFloat(0);
		else if (label == "Radiosity")
			this->attribute.radiosity = (float)args.getFloat(0);
		else if (label == "ReflectivityFalloff")
			this->attribute.reflectivityfalloff = (float)args.getFloat(0);

		else if (label == "[DECALIMAGE]") {
			decal.readHDecalImage(fin,patches->imageArray);
			if (decal.name != "")
				this->decalIndicies.push_back(patches->decalArray.append(decal));
		}
		else if (label == "DiffuseName") {
			if (fline != "DiffuseName=")//WHAT THE CRAP?!
				if (args.getString(0) == "AGP")
					this->flags.kIsAGP = 1;
		}
		else if (label == "[DYNAMICPROPERTY]") {
			HDynamicProperties::readHDynamicProperties(fin);
		}

		fin >> fline;
	}
	if (!found_color)
		this->attribute.diffusecolor = HColor(0,1,0);
}

void HDecal::readHDecalImage(std::ifstream &fin,HImageArray &images) {
	UString fline,label;
	HArgArray args;
	UString image_name;
	bool imagefound;
	HStatus status = HStatus::kFailure;

	fin >> fline;
	while (fline != "[ENDDECALIMAGE]") {

		fileTools::parseLine(fline,label,args);

		if (label == "Name")
		{
			if (fline == "Name=")
				this->name = "";
			else
			{
				UString sarray;
				if (args.getString(0) == "Shortcut")
				{
					this->name = "";
					for (int i = 2; i < (int)args.length(); i++)
						this->name += args.getString(i);
				}
				else
					this->name = args.getWholeString();
			}
		}
		if (label == "[Image]")
		{
			while (label != "[EndImage]")
			{
				fin >> fline;
				fileTools::parseLine(fline,label,args);
				if (label == "Value")
				{
					UStringArray sarray;
					fline.split('|',sarray);
					image_name = sarray[sarray.length()-1];
					if (images.model->version > 10.02)
					{
						//strip off quotes
						UStringArray temparray;
						image_name.split('"',temparray);

						UString temp = temparray[0];
						image_name = temp.getFileNameWithoutExt();
					}

					imagefound = images.findImageIndexWithName(image_name,this->imageIndex);
					this->name = image_name;
					UString error = UString("Can't find image ") + UString(image_name)
						+ UString(" in imagelist.\n Check to see if decals or groups refer to a nonexistent image.");
					if (!imagefound)
					{
						THROW_FATAL_ERROR(error.asChar());
					}
				}
				else if (label == "Repeat")
				{
					this->repeat.x = args.getFloat(0);
					this->repeat.y = args.getFloat(1);
				}
			}
			fin >> fline;
			continue;
		}
		else if (label == "ShortcutToLinked")
		{
			image_name = args.getWholeString();
			imagefound = images.findImageIndexWithName(image_name,this->imageIndex);
			if (!imagefound) {
				THROW_FATAL_ERROR("Can't get image index.");
			}
		}
		else if (label == "DecalType")
			this->type = getDecalTypeFlag(args.getString(0),status);
		else if (label == "Value")
			this->value = (float)args.getFloat(0);
		else if (label == "Repeat") {
			this->repeat.x = args.getFloat(0);
			this->repeat.y = args.getFloat(1);
		}
		else if (label == "Seamless")
			this->seamless = args.getBool(0);

		fin >> fline;
	}
}
			
HDecal::mType HDecal::getDecalTypeFlag(UString typname, HStatus status) {
	status = HStatus::kSuccess;

	if (typname == "Color")
		return HDecal::COLORMAP;
	if (typname == "Bump")
		return HDecal::BUMPMAP;
	if (typname == "SpecularSize")
		return HDecal::SPECULARSIZEMAP;
	if (typname == "SpecularIntensity")
		return HDecal::SPECULARINTENSITYMAP;
	if (typname == "Diffuse")
		return HDecal::DIFFUSEMAP;
	if (typname == "Ambiance")
		return HDecal::AMBIANCEMAP;
	if (typname == "Transparency")
		return HDecal::TRANSPARENCYMAP;
	if (typname == "Reflectivity")
		return HDecal::REFLECTIONMAP;
	if (typname == "Cookie-Cut")
		return HDecal::COOKIECUTMAP;
	if (typname == "Displacement")
		return HDecal::DISPLACEMENTMAP;
	if (typname == "Fractal")
		return HDecal::FRACTALMAP;

	return HDecal::INVALID;
}

void HStamp::readHStampData(std::ifstream &fin,HPatchArray *patches){
	UString fline,label;
	unsigned patchcount = 0;
	HArgArray args;
	std::vector<int> cparray;
	bool is5point = true;
	unsigned cpcount;
	unsigned i,j;
	unsigned patchid = 0;
	HStatus status;
	std::vector<int> patchids;
	HPatchVectors pvect;
	HVector v;
	std::vector<int> pids;
	bool patchfound;
	unsigned uvsidx = 0;
	HSplineArray *splines = &patches->model->splineArray;

	fileTools::parseLine(fin,"PatchesHit",args);
	
	patchcount = args.getInt(0);
	
	if (patchcount == 0)
		return;

	for (unsigned nPatch = 0; nPatch < patchcount; nPatch++) {
		cparray.clear();

		fileTools::parseLine(fin,args);

		cpcount = args.length();

		for (i = 0; i < cpcount; i++)
			cparray.push_back(splines->HashCPIndexToMyCPIndex(args.getInt(i)));

		pvect.clear();
		for (i = 0; i < cpcount; i++) {
			fileTools::parseLine(fin,args);
			for (j = 0; j < 3; j++){
				v.x = args.getFloat(j*3);
				v.y = args.getFloat(j*3+1);
				v.z = args.getFloat(j*3+2);
				pvect.append(v);
			}	
		}
		uvsidx = this->uvs.append(pvect);

		patchfound = patches->derivePatchIndicies(cparray,false,this->patchIndicies,uvsidx);
		if (!patchfound) {
//			std::cerr << "STAMPS: " << cparray << std::endl;
			std::cerr << "Patch could not be found for " << this->name.asChar() << "[" << nPatch << "]" << std::endl;
		}
	}
}

void HStamp::readHStamp(std::ifstream &fin,HPatchArray *patches){
	UString fline,label;
	HArgArray args;

	this->clear();

	fin >> fline;

	while ( fline != "[ENDSTAMP]" ) {
		fileTools::parseLine(fline,label,args);
		if (label == "Name") {
			if (fline == "Name=")
				this->name = "";
			else
				this->name = args.getWholeString();
		}
		if (fline == "[DATA]") {
			readHStampData(fin,patches);
		}
		fin >> fline;
	}
}

void HStampArray::readHStampArray(std::ifstream &fin,HPatchArray *patches,HDecal *decal) {
	UString fline;
	HStamp stamp;

	fin >> fline;

	while ( fline != "[ENDSTAMPS]" ) {
		if (fline == "[STAMP]") {
			stamp.readHStamp(fin,patches);
			decal->stampIndicies.push_back(this->append(stamp));
		}
		fin >> fline;
	}
}

void HDecal::readHDecal(std::ifstream &fin,HPatchArray *patches) {
	UString fline,label;
	HArgArray args;

	this->clear();

	fin >> fline;

	while ( fline != "[ENDDECAL]" ) {

		fileTools::parseLine(fline,label,args);

		if (label == "Name") {
			if (fline == "Name=") {
				this->decal_name = "";
				fin >> fline;
				continue;
			}
			this->decal_name = args.getWholeString();
		}
		else if (fline == "[DECALIMAGE]") {
			this->readHDecalImage(fin,patches->imageArray);	
		}
		else if (fline == "[STAMPS]") {
			patches->stampArray.readHStampArray(fin,patches,this);
		}
		fin >> fline;
	}
}

void HDecalArray::readHDecalArray(std::ifstream &fin,HPatchArray *patches) {
	UString fline;
	HDecal decal;
	unsigned decalidx = 0;

	fin >> fline;

	while ( fline != "[ENDDECALS]" ) {

		if (fline == "[DECAL]") {
			decal.readHDecal(fin,patches);
			if (decal.name != "") {
				decalidx = this->append(decal);
				patches->setPatchDecalIndicies(*this,decalidx);
			}
		}
		fin >> fline;
	}
}

void HGroupArray::readHGroupArray(std::ifstream &fin, HPatchArray *patches) {
	HGroup group; 
	UString fline;
	unsigned groupidx = 0;

	fin >> fline;

	while (fline != "[ENDGROUPS]") {
		if (fline == "[GROUP]") {
			group.clear();
			group.readHGroup(fin,patches);
			if (group.name != "") {
				groupidx = this->append(group);
				patches->setPatchGroupIndicies(*this,groupidx);
			}
		}

		fin >> fline;
	}
}

void HBoneChainArray::readHBoneChainArray(std::ifstream &fin,HPolyArray &polys,HBoneArray &bones) {
	UString fline,label;
	HArgArray args;
	HBone bone,*boneptr = NULL;
	HBoneChain *bc;
	unsigned cpcount;
	int level = 0;
	bool finished = false;
	bool open = true;
	int onChain = 0;

	bool hasParent = false;
	int BoneIdx = 0,BoneIdx2 = 0;

	fin >> fline;

	while ( !finished ) {

		fileTools::parseLine(fline,label,args);

		if (fline == "[SEGMENT]") {
			if (open) {
				if (hasParent) {
					BoneIdx = bones.addBoneWParent(BoneIdx,bone);	
				}
				else {
					BoneIdx = bones.addBone(bone);
					hasParent = true;
				}
				
				if (bone.flags.kHasIKParent) {
					bc = this->get(onChain);
					bc->append(BoneIdx);
					bones.get(BoneIdx)->setChainIndex(onChain);
				}
				else if (!bone.flags.kHasIKParent) {
					onChain = this->addEmptyChain(bones);
					bc = this->get(onChain);
					bc->append(BoneIdx);
					bones.get(BoneIdx)->setChainIndex(onChain);
				}
				bone.clear();
			}
			open = true;
		}
		else if (fline == "Chained=TRUE") {
			bone.flags.kHasIKParent = true;
		}
		else if (fline == "Chained=FALSE") {
			bone.flags.kHasIKParent = false;
		}
		else if (fline == "[ENDSEGMENT]") {
			if (open) {
				if (hasParent) {
					BoneIdx2 = bones.addBoneWParent(BoneIdx,bone);
				}
				else {
					BoneIdx2 = bones.addBone(bone);
				}
				boneptr = bones.end();

				if (bone.flags.kHasIKParent) {
					bc = this->get(onChain);
					bc->append(BoneIdx2);
					bones.get(BoneIdx2)->setChainIndex(onChain);
				}
				else if (!bone.flags.kHasIKParent) {
					onChain = this->addEmptyChain(bones);
					bc = this->get(onChain);
					bc->append(BoneIdx2);
					bones.get(BoneIdx2)->setChainIndex(onChain);
				}

				bone.clear();

				open = false;
			}

			if (boneptr->flags.kHasParent) {
				BoneIdx = boneptr->parentIndex;
				hasParent = true;
				boneptr = boneptr->previous();
			}
			else {
				finished = true;
				continue;
			}

			if(boneptr->flags.kHasChainIndex) {
				onChain = boneptr->chainIndex;
			}
		}
		else if (fline == "[NONSKINNEDCPS]") {
			fileTools::parseLine(fin,"Count",args);
			cpcount = args.getInt(0);
			for (unsigned i = 0; i < cpcount; i++) {
				fileTools::parseLine(fin,args);
				int cpindex = args.getInt(0);
				bone.cps.push_back(cpindex);
			}
		}
		else if (label == "Name") {
			if (fline == "Name=")
				bone.name = "";
			else
				bone.name = args.getWholeString();
		}
		else if (label == "Pivot") {
			bone.pivot.x = args.getFloat(0);
			bone.pivot.y = args.getFloat(1);
			bone.pivot.z = args.getFloat(2);
		}
		else if (label == "End") {
			bone.end.x = args.getFloat(0);
			bone.end.y = args.getFloat(1);
			bone.end.z = args.getFloat(2);
		}	
		else if (label == "RollHandle") {
			bone.rollhandle.x = args.getFloat(0);
			bone.rollhandle.y = args.getFloat(1);
			bone.rollhandle.z = args.getFloat(2);
		}
		else if (label == "Start") {
			bone.pivot.x = args.getFloat(0);
			bone.pivot.y = args.getFloat(1);
			bone.pivot.z = args.getFloat(2);
			bone.V9Bone = true;
		}
		else if (label == "Rotate") {
			bone.rotate.v.x = args.getFloat(0);
			bone.rotate.v.y = args.getFloat(1);
			bone.rotate.v.z = args.getFloat(2);
			bone.rotate.w = args.getFloat(3);
		}
		else if (label == "BoneColor") {
			bone.color.r = args.getFloat(2);
			bone.color.g = args.getFloat(1);
			bone.color.b = args.getFloat(0);
//			bone.color.a = args.getFloat(3);
		}
		else if (label == "Length") {
			bone.length = args.getFloat(0);
		}
		else if (label == "FallOff") {
			bone.falloff = args.getFloat(0);
		}
		else if (label == "[NULLOBJECT]") {
			fileTools::skipTo(fin,"[ENDNULLOBJECT]");
		}
		else if (label == "[ENDMODEL]") {
			THROW_FATAL_ERROR("WARNING: Couldn't parse skeleton.");
		}
		fin >> fline;
	}
}

void HAttr::readHAttr(std::ifstream &fin) {
	UString fline,label;
	HArgArray args;

	fin >> fline;

	while (fline != "[ENDATTRIBUTE]") {

		fileTools::parseLine(fline,label,args);

			if (label == "DiffuseColor") {//HASH COLORS ARE STORED AS BGR
				this->diffusecolor.r = args.getFloat(2);
				this->diffusecolor.g = args.getFloat(1);
				this->diffusecolor.b = args.getFloat(0);
			}
			else if (label == "DiffuseFalloff")
				this->diffusefalloff = (float)args.getFloat(0);
			else if (label == "Ambiance") 
				this->ambiance = (float)args.getFloat(0);
			else if (label == "Roughness")
				this->roughness = (float)args.getFloat(0);
			else if (label == "RoughnessScale")
				this->roughnesscale = (float)args.getFloat(0);
			else if (label == "Reflectivity")
				this->reflectivity = (float)args.getFloat(0);
			else if (label == "Transparency") {
				this->transparency.r = args.getFloat(0);
				this->transparency.g = args.getFloat(0);
				this->transparency.b = args.getFloat(0);
			}
			else if (label == "Refraction")
				this->refraction = (float)args.getFloat(0);
			else if (label == "SpecularColor") {
				this->specularcolor.r = args.getFloat(2);
				this->specularcolor.g = args.getFloat(1);
				this->specularcolor.b = args.getFloat(0);
			}
			else if (label == "SpecularSize")
				this->specularsize = (float)args.getFloat(0);
			else if (label == "SpecularIntensity")
				this->specularintensity = (float)args.getFloat(0);
			else if (label == "Density")
				this->density = (float)args.getFloat(0);
			else if (label == "Translucency")
				this->translucency = (float)args.getFloat(0);
			else if (label == "Radiosity")
				this->radiosity = (float)args.getFloat(0);
			else if (label == "ReflectivityFalloff")
				this->reflectivityfalloff = (float)args.getFloat(0);
		fin >> fline;
	}
}

void HAttrArray::readHAttrArray(std::ifstream &fin) {
	UString fline;
	HAttr attr;

	fin >> fline;

	while (fline != "[ENDATTRS]") {
		if (fline == "[ATTRIBUTE]") {
			attr.clear();
			attr.readHAttr(fin);

			this->append(attr);
		}

		fin >> fline;
	}
}

void HDynamicProperties::readHDynamicProperties(std::ifstream &fin) {
	UString fline;
	UString *label;
	UStringArray sarray;

	int count = 0;
	while (count >= 0)
	{
		fin >> fline;
		fline.split(' ',sarray);
		label = &sarray[0];
		if (*label == "[DYNAMICPROPERTY]")
			count++;
		else if (*label == "[ENDDYNAMICPROPERTY]")
			count--;
	}
}

void HUserProperties::readHUserProperties(std::ifstream &fin) {
	UString fline;
	UString *label;
	UStringArray sarray;

	int count = 0;
	while (count >= 0)
	{
		fin >> fline;
		fline.split(' ',sarray);
		label = &sarray[0];
		if (*label == "[USERPROPERTY]")
			count++;
		else if (*label == "[ENDUSERPROPERTY]")
			count--;
	}
}