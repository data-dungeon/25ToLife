//////////////////////////////////////////////////////////////////////////////////////////
//
// HPATCH ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "hpatcharray.h"
#include "hpolyarray.h"
#include "mdlobject.h"

HPatchArray::HPatchArray() {
	model = NULL;
}

HPatchArray::HPatchArray(const HPatchArray &other) {
	classList = other.classList;
	imageArray = other.imageArray;
	groupArray = other.groupArray;
	attrArray = other.attrArray;
	stampArray = other.stampArray;
	decalArray = other.decalArray;

	boneArray = other.boneArray;
	boneChainArray = other.boneChainArray;

	model = other.model;
}

HPatchArray::~HPatchArray() {
}

void HPatchArray::clear() {
	model = NULL;
	classList.clear();
	imageArray.clear();
	groupArray.clear();
	attrArray.clear();
	stampArray.clear();
	decalArray.clear();
}

unsigned HPatchArray::length() const {
	return classList.size();
}

void HPatchArray::append(HPatch &patch) {
	patch.patchArray = this;
	patch.patchIndex = classList.size();
	classList.push_back(patch);
}

// calculate tangents for all patches
void HPatchArray::calculateTangents() {
	for (HPatch *iter = this->begin(); iter != NULL; iter = iter->next())
		iter->calculateTangents();
}

HPatch *HPatchArray::get(unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.");
	}
	return &classList[index];
}

void HPatchArray::derivePatchArraySegments() {
	HSplineArray *splines = &this->model->splineArray;

//	Setup CP Neighbor table
	splines->neighbors.resize(splines->vertices.length());

//	Setup lookup table for stacks
	splines->stackList = new HNeighborArray(splines->vertices.length());

	HIntMap stack;
	int vi;
	HNeighbor *n;
	for (unsigned i = 0;i < splines->absCpiTable.size(); i++) {
		unsigned cpindex = splines->absCpiTable[i];
		vi = splines->drillForVertexIndex(cpindex);
		n = splines->stackList->get(vi);
		if (n->cpids.length() == 0) {
			splines->fillCPStack(splines->absCpiTable[i],stack);
			n->cpids = stack;
		}
	}

//	Derive patch segments for each patch
	for (HPatch *iter = this->begin(); iter != NULL; iter = iter->next())
		iter->derivePatchSegments();

	delete splines->stackList;
}

bool HPatchArray::derivePatchIndicies(std::vector<int> &cparray, bool deriveAsGroup, HIntMap &patchIndicies,unsigned uvsidx) {
	std::vector<int> list;
	HIntMap cpstack;
	HSplineArray *splines = &this->model->splineArray;
	int value;

	HIntMap map0;
	HNeighbor *n;
	for (unsigned i = 0; i < cparray.size(); i++)
	{
		n = splines->neighbors.get(splines->drillForVertexIndex(cparray[i]));

		if (!map0.hasKey((int)n))
		{
			for (unsigned j = 0; j < n->nsp.length(); j++)
			{
				value = n->nsp.getKey(j);
				list.push_back(value);
			}
			map0.setValue((int)n,1);
		}
	}
	
	if (!list.size())
		return false;

	std::vector<int>::iterator iter = std::max_element(list.begin(),list.end());
	unsigned max = *iter+1;

	std::vector<int> list2;
	list2.resize(max);

	for (i = 0; i < list.size(); i++)
		list2[list[i]] += 1;

	bool patchfound = false;
	unsigned numCps = 3;

	for (i = 0; i < (int)list2.size(); i++) {
		if (list2[i] >= (int)numCps) {

			if (deriveAsGroup)
				patchIndicies.append(i);
			else
				patchIndicies.setValue(i,uvsidx);

			patchfound = true;
		}
	}
	if (patchfound)
		return true;

	return false;
}

void HPatchArray::setPatchGroupIndicies(HGroupArray &groups, unsigned groupidx) {
	int value;
	HGroup *group = groups.get(groupidx);
	HPatch *patch;
	for (unsigned a = 0; a < group->patchIndicies.length(); a++) {
		value = group->patchIndicies.getKey(a);
		patch = this->get(value);
		patch->groupIndicies.push_back(groupidx);
	}
}

void HPatchArray::setPatchDecalIndicies(HDecalArray &decals, unsigned decalidx) {
	HStamp *stamp;
	HDecal *decal = decals.get(decalidx);
	HPatch *patch;
	unsigned value;

	for (unsigned a = 0; a < decal->stampIndicies.size(); a++) {
		stamp = this->stampArray.get(decal->stampIndicies[a]);
		for (unsigned b = 0; b < stamp->patchIndicies.length(); b++) {
			value = stamp->patchIndicies.getKey(b);
			patch = this->get(value);
			patch->decalIndicies.push_back(decalidx);
		}
	}
}

void HPatchArray::tessellate(HPolyArray &polygons,unsigned numsubdivisions) {
	std::vector<int> shaderIndicies;
//	Create grey default shader
	HShader defaultshader;
	defaultshader.name = "defaultshader";
	defaultshader.diffusecolor = HColor(0.5,0.5,0.5);
	model->polygonArray->shaders.append(defaultshader);
	model->polygonArray->tjuncts = new HTJunctionArray();

//	Setup tessellated patch corner array
	this->tesscorners = new HTessCornerArray(model->splineArray.vertices.length());

	HPatch *patch;
	for (unsigned i = 0;i < this->length(); i++) {
		patch = this->get(i);

		shaderIndicies.clear();

		if (patch->hasDecals())
			addShaderFromDecals(patch,polygons,shaderIndicies);
		else
			if (patch->hasGroups())
				addShaderFromGroups(patch,polygons,shaderIndicies);

		if (shaderIndicies.size() == 0)
			shaderIndicies.push_back(0);

		patch->tessellate(polygons,shaderIndicies,numsubdivisions);
	}

	if (model->ui_flags.fixtjunctions)
		model->polygonArray->fixTJunctions();

	deriveBoneVids();
	fillInTheHoles();
	
	delete this->tesscorners;
	delete model->polygonArray->tjuncts;
}

void HPatchArray::deriveBoneVids()
{
	HBoneArray *bones = &this->model->patchArray->boneArray;
	int hookvindex;

	HIntMap bonevidmap;
	
	for (int i = 0; i <(int)bones->length(); i++)
	{
		HBone *bone = bones->get(i);

		for (int j = 0; j <(int) bone->cps.size(); j++)
		{
//			HIntMap bonevidmap;//Use this to debug whether more than one bone is getting the same vertex

			int cpid = bone->cps[j];

			HIntMap cpstack;
			this->model->splineArray.fillCPStack(bone->cps[j],cpstack);

			for (int k = 0; k <(int)cpstack.length(); k++)
			{
				int cpindex = cpstack.getKey(k);

				if (this->model->splineArray.isHook(cpid))
					continue;

				int vid = this->getVIDFromCPID(cpindex,hookvindex);

				if (vid>=0)
				{
					if (!bonevidmap.hasKey(vid))
					{
						bone->vids.push_back(vid);
						bonevidmap.setValue(vid,1);
					}
				}
				if (hookvindex > 0)
				{
					if (!bonevidmap.hasKey(hookvindex))
					{
						bone->vids.push_back(hookvindex);
						bonevidmap.setValue(hookvindex,1);
					}
				}

				if (fivepointintmap.hasKey(cpid))
				{
					int value = fivepointintmap.getValue(cpid);
					if (!bonevidmap.hasKey(value))
					{
						bone->vids.push_back(value);
						bonevidmap.setValue(value,1);
					}
				}
			}
		}
	}
}

void HPatchArray::fillInTheHoles() {
	HPolyArray *polys = model->polygonArray;
	int verticeslength = polys->vertices.length();

	std::vector<int> bonelist;

	bonelist.resize(verticeslength);

	for (int i = 0; i < (int)bonelist.size(); i++)
		bonelist[i] = -1;

	HBone *bone;
	for (i = 0; i < (int)this->boneArray.length(); i++)
	{
		bone = boneArray.get(i);
		for (int j = 0; j < (int)bone->vids.size(); j++)
		{
			bonelist[bone->vids[j]] = i;
		}
	}

	int closestvid;
	for (i = 0; i < (int)bonelist.size(); i++)
	{
		if (bonelist[i] < 0)
		{
			closestvid = getVertexClosest(i,bonelist);
			if (closestvid>=0)
			{
				bone = boneArray.get(bonelist[closestvid]);
				bone->vids.push_back(i);
			}
		}
	}
}

int HPatchArray::getVertexClosest(int vid,const std::vector<int> &flags)
{
	HPolyArray *polys = model->polygonArray;

	HVector *vect = &polys->vertices[vid];

	int closest = -1;
	double closestdistance = -1.0;

	for (int i = 0; i < (int)polys->vertices.length(); i++)
	{
		if (flags[i]>0)
		{
			double distance = vect->distanceTo(polys->vertices[i]);

			if ((distance < closestdistance)||(closestdistance < 0))
			{
				closest = i;
				closestdistance = distance;
			}
		}
	}
	return closest;
}


int HPatchArray::getVIDFromCPID(unsigned cpid,int &hookvindex) {
	HSplineArray *splines = &model->splineArray;
	unsigned hookcpindex;
	hookvindex = -1;
	bool ishook = splines->drillForHook(cpid,hookcpindex);
	if (ishook)
	{
		hookvindex = splines->drillForVertexIndex(hookcpindex);
		HTessCorner *tesscorner = this->tesscorners->get(hookvindex);
		if (tesscorner->flags.kHasVertexIndex)
			hookvindex = tesscorner->vertexIndex;
	}
	int vid = splines->drillForVertexIndex(cpid);
	HTessCorner *tesscorner = this->tesscorners->get(vid);
	if (tesscorner->flags.kHasVertexIndex)
		return tesscorner->vertexIndex;
	return -1;
}

void HPatchArray::addShaderFromGroups(HPatch *patch,HPolyArray &polygons,std::vector<int> &shaderIndicies) {
	HShader shader;
	HMap map;
	HGroup *group;
	HDecal *decal;
	HGroupArray *groups = &patch->patchArray->groupArray;
	HDecalArray *decals = &patch->patchArray->decalArray;
	HMapArray *maps = &polygons.maps;
	
	HIntMap map0;
	int mapidx,shadidx;

	for (unsigned j = 0; j < patch->groupIndicies.size(); j++) {
		group = groups->get(patch->groupIndicies[j]);

		shader.clear();
		shader.name = group->name;
		shader.diffusecolor = group->attribute.diffusecolor;
		shader.specularcolor = group->attribute.specularcolor;
		shader.specularsize = group->attribute.specularsize;
		shader.specularintensity = group->attribute.specularintensity;
		shader.transparency = group->attribute.transparency;

		memcpy(&shader.flags,&group->flags,sizeof(unsigned long));
		
		if (group->hasDecals()) {
			HIntMap map1;
			
			int group_decal_indicies_length = group->decalIndicies.size();
			for (unsigned i = 0; i < (unsigned)group_decal_indicies_length; i++) {
				decal = decals->get(group->decalIndicies[group_decal_indicies_length - i - 1]);
				getMap(decal,map);
				mapidx = polygons.maps.add(map);
				if (!map1.hasKey(mapidx)) {
					shader.mapIndicies.push_back(mapidx);
					map1.setValue(mapidx,map1.length());
				}
			}
		}
		shadidx = polygons.shaders.add(shader);
		if (!map0.hasKey(shadidx)) {
			shaderIndicies.push_back(shadidx);
			map0.setValue(shadidx,map0.length());
		}
	}
}

void HPatchArray::addShaderFromDecals(HPatch *patch,HPolyArray &polygons,std::vector<int> &shaderIndicies) {
	HShader shader;
	HMap map;
	HDecal *decal;
	HMapArray *maps = &polygons.maps;
	HDecalArray *decals = &this->decalArray;

	shader.name = decals->get(patch->decalIndicies.front())->decal_name;
	
	int mapidx;
	HIntMap map1;

	for (unsigned i = 0; i < patch->decalIndicies.size(); i++) {
		decal = decals->get(patch->decalIndicies[patch->decalIndicies.size() - i - 1]);
		getMap(decal,map);
		mapidx = polygons.maps.add(map);
		if (!map1.hasKey(mapidx)) {
			shader.mapIndicies.push_back(mapidx);
			map1.setValue(mapidx,shader.mapIndicies.size());
		}
	}
	shaderIndicies.push_back(polygons.shaders.add(shader));
}

void HPatchArray::getMap(HDecal *decal,HMap &map) {
	HImage *image;
	HImageArray *images = &this->imageArray;
	
	map.clear();

	map.name = decal->name;
	image = images->get(decal->imageIndex);
	map.path = image->pathname;
	map.repeat = decal->repeat;

	if (decal->type == HDecal::COLORMAP)
		map.type = HMap::COLORMAP;
	if (decal->type == HDecal::COOKIECUTMAP)
		map.type = HMap::COOKIECUTMAP;
	if (decal->type == HDecal::DIFFUSEMAP)
		map.type = HMap::DIFFUSEMAP;
}

HPatch *HPatchArray::begin() {
	return &classList.front();
}

HPatch *HPatchArray::end() {
	return &classList.back();
}

void HPatchArray::updateBackPointers() {
	for (unsigned i = 0; i < this->length(); i++) {
		HPatch *patch = this->get(i);
		patch->patchArray = this;
	}
}

HPatch HPatchArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPatchArray]");
	}
	return classList[index];
}

HPatch &HPatchArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPatchArray]");
	}
	return classList[index];
}

HPatchArray &HPatchArray::operator = ( const HPatchArray &other ){
	classList = other.classList;
	imageArray = other.imageArray;
	groupArray = other.groupArray;
	attrArray = other.attrArray;
	stampArray = other.stampArray;
	decalArray = other.decalArray;

	boneArray = other.boneArray;
	boneChainArray = other.boneChainArray;

	model = other.model;
	return *this;
}

std::ostream &operator << (std::ostream &os, const HPatchArray &patches){
	for (unsigned i = 0; i < patches.length(); i++)
		os << patches[i] << std::endl;
	
	return os;
}