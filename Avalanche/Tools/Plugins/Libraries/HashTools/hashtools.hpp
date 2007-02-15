// Gets all GROUPS except temporary groups made by selections
// "gsib" - Any Group Sibling
inline void HashTools::GetAllGroups(HModelCache *hmc,HashGroups &groups) {
	groups.clear();
	if (hmc)
	{
		HGroup *groupcontainer = (HGroup *)hmc->GetChildGroupContainer();
		if (!groupcontainer)
			return;
		HGroup *firstsibling = (HGroup *)groupcontainer->GetChild();
		if (!firstsibling)
			return;
		for (HGroup *group = firstsibling; group; group = (HGroup *)group->GetSiblingSame()) {
	//		Skip "untitled" selections
			if (!group->IsTempGroup())
				groups.push_back(group);
		}
	}
}

// Gets all DECALS
// "gsib" - Any Group Sibling
inline void HashTools::GetAllDecals(HModelCache *hmc,HashDecals &decals) {
	decals.clear();
	HHashObject *decalcontainer = (HHashObject*)hmc->GetChildDecalContainer();
	if (!decalcontainer)
		return;
	HDecal *firstsibling = (HDecal *)decalcontainer->GetChild();

	if (firstsibling) {
		for (HDecal *decal = firstsibling; decal; decal = (HDecal *)decal->GetSiblingSame())
			if (decal)
				decals.push_back(decal);
	}
}

inline void HashTools::GetAllPatches(HModelCache *hmc,HashPatches &patches) {
	patches.clear();
	int patchcount = hmc->GetPatchCount();
	for (int i = 0; i < patchcount; i++) {
		HPatch *patch = hmc->GetPatch(i);
		patches.push_back(patch);
	}
}

inline void HashTools::GetAllCPs(HModelCache *hmc,HashCPs &cps)
{
	cps.clear();

	for (HSpline *hspline = hmc->GetHeadSpline(); hspline; hspline = hspline->GetNextSpline())
	{
		for (HCP *hcp = hspline->GetHeadCP(); hcp; hcp = hcp->GetNext())
		{
			int cpid = hcp->GetID();
			if (hcp->IsLoop())
			{
				cps.push_back(hcp);
				break;
			}
			cps.push_back(hcp);
		}
	}
}

inline void HashTools::GetAllBones(HModelCache *hmc,HashBones &parents,HashBones &bones)
{
	bones.clear();
	parents.clear();
	HHashObject *bonecontainer = (HHashObject*)hmc->GetChildBoneCacheContainer();
	if (!bonecontainer)
		return;
	GetBone(bonecontainer,parents,bones);
}

inline void HashTools::GetBone(HTreeObject *htreeobject,HashBones &parents,HashBones &bones)
{
	if (htreeobject->IsBone())
	{
		bones.push_back((HBoneCache*)htreeobject);
		if ((htreeobject->GetTreeParent())&&(htreeobject->GetTreeParent()->IsBone()))
			parents.push_back((HBoneCache*)htreeobject->GetTreeParent());
		else
			parents.push_back(NULL);
	}
	for (HBoneCache *hbonecache = (HBoneCache*)htreeobject->GetTreeProgeny(); hbonecache; hbonecache = hbonecache->GetSiblingBoneCache())
	{
		GetBone((HTreeObject*)hbonecache,parents,bones);
	}
}

//  Get a patch array of all patches belonging to group.
// "patches" do not get cleared
inline void HashTools::GetPatchesInGroup(HashPatches &patches,HGroup *group,HModelCache *hmc) {
	patches.clear();
	unsigned patchcount = hmc->GetPatchCount();
	for (unsigned i = 0; i < patchcount; i++) {
		HPatch *patch = hmc->GetPatch(i);
		if (group->IsPatchInGroup(patch))
			patches.push_back(patch);
	}	
}

inline void HashTools::GetGroupsFromPatch(HashGroups &groups,HPatch *patch,HashGroups &allGroups) {
	groups.clear();
	for (unsigned i = 0; i < allGroups.size(); i++) {
		if (allGroups[i]->IsPatchInGroup(patch))
			groups.push_back(allGroups[i]);
	}
}

inline bool HashTools::IsPatchInStamp(HDecalStamp *stamp,HPatch *patch) {
	for (HBasePatchUV *patchuv = stamp->GetFirstPatchUV();patchuv;patchuv = stamp->GetNextPatchUV(patchuv))
		if (patchuv->GetPatch() == patch)
			return true;
	return false;
}

inline bool HashTools::IsPatchInDecal(HDecal *decal,HPatch *patch) {
	HashStamps stamps;
	GetStampsFromDecal(decal,stamps);
	for (unsigned i = 0; i < stamps.size(); i++)
		if (IsPatchInStamp(stamps[i],patch))
			return true;
	return false;
}

inline void HashTools::GetPatchesInStamp(HDecalStamp *stamp,HashPatches &patches) {
	patches.clear();
	for (HBasePatchUV *patchuv = stamp->GetFirstPatchUV();patchuv;patchuv = stamp->GetNextPatchUV(patchuv))
		patches.push_back(patchuv->GetPatch());
}


inline void HashTools::GetStampsFromDecalThatBelongToPatch(HDecal *decal,HashStamps &stamps,HPatch *patch) {
	stamps.clear();
	HHashObject *stampcontainer = decal->GetChildDecalStampContainer();
	HDecalStamp *firststamp = (HDecalStamp *)stampcontainer->GetChild();
	for (HDecalStamp*stamp = firststamp;stamp;stamp = (HDecalStamp*)stamp->GetSiblingSame())
		if (IsPatchInStamp(stamp,patch))
			stamps.push_back(stamp);
}
												  
inline void HashTools::GetDecalsFromPatch(HashDecals &decals,HPatch *patch,HashDecals &allDecals) {
	decals.clear();
	for (unsigned i = 0; i < allDecals.size(); i++) {
		if (IsPatchInDecal(allDecals[i],patch))
			decals.push_back(allDecals[i]);
	}
}

inline void HashTools::GetStampsFromDecal(HDecal *decal,HashStamps &stamps) {
	stamps.clear();
	HHashObject *stampcontainer = decal->GetChildDecalStampContainer();
	if (stampcontainer) {
		HDecalStamp *firststamp = (HDecalStamp *)stampcontainer->GetChild();
		for (HDecalStamp*stamp = firststamp;stamp;stamp = (HDecalStamp*)stamp->GetSiblingSame())
			stamps.push_back(stamp);
	}
}

inline void HashTools::GetClipsFromDecal(HDecal *decal,HashDecalClips &clips,bool clear) {
	if (clear)
		clips.clear();
	HHashObject *clipcontainer = decal->GetChildDecalClipContainer();
	if (clipcontainer) {
		HDecalClip *firstclip = (HDecalClip *)clipcontainer->GetChild();
		for (HDecalClip *clip = firstclip;clip;clip = (HDecalClip*)clip->GetSiblingSame())
			clips.push_back(clip);
	}
}

inline void HashTools::GetClipsFromGroup(HGroup *group,HashDecalClips &clips,bool clear) {
	if (clear)
		clips.clear();
	HHashObject *clipcontainer = group->GetChildDecalClipContainer();
	if (clipcontainer) {
		HDecalClip *firstclip = (HDecalClip *)clipcontainer->GetChild();
		for (HDecalClip *clip = firstclip;clip;clip = (HDecalClip*)clip->GetSiblingSame())
			clips.push_back(clip);
	}
}

inline void HashTools::GetAllClips(HModelCache *hmc,HashClips &clips) {
	clips.clear();
	HashDecals hdecals;
	GetAllDecals(hmc,hdecals);

	HashDecalClips hdecalclips;
	
	for (unsigned i = 0; i < hdecals.size(); i++)
	{
		HDecal *hdecal = hdecals[i];
		GetClipsFromDecal(hdecal,hdecalclips,false);
	}

	for (i = 0; i < hdecalclips.size(); i++)
	{
		HDecalClip *hdecalclip = hdecalclips[i];

		HClip *hclip = hdecalclip->GetClip()->GetClip();
		clips.push_back(hclip);
	}

	HashGroups hgroups;
	GetAllGroups(hmc,hgroups);

	for (i = 0; i < hgroups.size(); i++)
	{
		HGroup *hgroup = hgroups[i];
		GetClipsFromGroup(hgroup,hdecalclips,false);
	}

	for (i = 0; i < hdecalclips.size(); i++)
	{
		HDecalClip *hdecalclip = hdecalclips[i];

		HClip *hclip = hdecalclip->GetClip()->GetClip();
		clips.push_back(hclip);
	}
}

inline void HashTools::GetBasePatchUVsFromStamp(HDecalStamp *stamp,HashBasePatchUVs &patchuvs) {
	patchuvs.clear();
	for (HBasePatchUV *patchuv = stamp->GetFirstPatchUV();patchuv;patchuv = stamp->GetNextPatchUV(patchuv))
		patchuvs.push_back(patchuv);
}

inline void HashTools::GetMaterialAttributesFromGroup(HGroup *group,HashAttributes &attributes) {	
	HHashObject *materialshortcut = (HHashObject *)group->GetChild();
	if (materialshortcut) {
		HHashObject *material = (HHashObject *)materialshortcut->GetCache();
		if (material && material->GetObjectType() == HOT_MATERIAL) {
			for (HHashObject *attribute = (HHashObject*)material->GetChild();attribute;attribute=attribute->GetSiblingSame()) {
				if (attribute && attribute->GetObjectType()==HOT_ATTRIBUTE) {
					attributes.push_back(attribute);
				}
			}
		}
	}
}

inline HProperty *HashTools::GetSurface(HHashObject *attribute) {
	return (HProperty *)attribute->GetPropertyAt(0);
}

inline HHashObject *HashTools::GetClipContainer(HModelCache *hmc){
	HHashObject *project = hmc->GetParentOfType(HOT_PROJECT);
	return (HHashObject*)project->GetChild();
}

inline void HashTools::GetPatchCPs(HPatch *patch,HashCPs &cps) {
	cps.clear();
	cps.resize(4);
	if (patch->GetCP1())
		cps[0] = patch->GetCP1();

	if (patch->GetCP2())
		cps[1] = patch->GetCP2();

	if (patch->GetCP3())
		cps[2] = patch->GetCP3();

	if (patch->GetCP4())
		cps[3] = patch->GetCP4();
}

inline void HashTools::GetPatch5CPs(HPatch5 *patch,HashCPs &cps) {
	cps.clear();
	cps.resize(4);
	if (patch->GetCP1())
		cps[0] = patch->GetCP1();

	if (patch->GetCP2())
		cps[1] = patch->GetCP2();

	if (patch->GetCP3())
		cps[2] = patch->GetCP3();

	if (patch->GetCP4())
		cps[3] = patch->GetCP4();

	if (patch->GetCP5())
		cps.push_back(patch->GetCP5());
}

inline void HashTools::GetGroupCPs(HGroup *group,HashCPs &cps) {
	cps.clear();
	if (group)
	{
		HGroupCP *firstgroupcp = group->GetGroupCP();
		if (firstgroupcp)
		{
			for (HGroupCP *gcp = firstgroupcp; gcp; gcp = gcp->GetNext())
			{
				HCP *cp = gcp->GetCP();
				if (cp)
					cps.push_back(cp);
			}
		}
	}
}

inline void HashTools::MakeUniqueDecalName(std::string &name,HashDecals &alldecals) {
	std::string decalname,temp,temp2;
	int uniqueness = -1;
	int maxuniqueness = -1;
	bool flag = false;
	for (unsigned i = 0; i < alldecals.size(); i++) {

		decalname = alldecals[i]->GetName();

		if (decalname.size() >= name.size()) {

			int diff = decalname.size()-name.size();

			temp = decalname.substr(0,name.size());

			if (strcmp(temp.c_str(),name.c_str()) == 0) {
				flag = true;
				uniqueness = atoi(decalname.substr(name.size(),diff).c_str());

				if (uniqueness > maxuniqueness)
					maxuniqueness = uniqueness;
			}
		}
	}
	if (flag) {
		char unique[128];
		itoa((maxuniqueness+1),unique,10);
		name += unique;
	}
}

inline HProperty *HashTools::FindPropertyInPluginProperties(const char *matchname,HHashObject *hashobject)
{
	return FindProperty(matchname,hashobject->GetPluginProperties());
}

//inline HProperty *HashTools::FindPropertyInUserProperties(const char *matchname,HHashObject *hashobject)
//{
//	return FindProperty(matchname,hashobject->GetUserProperties());
//}

inline HProperty *HashTools::FindProperty(const char *matchname,HProperty *property)
{
	if (!property)
		return NULL;

	String matchname2 = property->GetMatchName();

	if (matchname2 == matchname)
		return property;

	int index = 0;
	while (HProperty *nextproperty = property->GetPropertyAt(index++))
	{
		HProperty *result = FindProperty(matchname,nextproperty);
		if (result)
			return result;
	}

	return NULL;
}

inline HProperty *HashTools::FindProperty(const char *matchname,HTreeObject *htreeobject)
{
	int index = 0;
	while (HProperty *nextproperty = htreeobject->GetPropertyAt(index++))
	{
		HProperty *result = FindProperty(matchname,nextproperty);
		if (result)
			return result;
	}

	return NULL;
}

inline void HashTools::CopyProperties(HProperty *dstproperty,HProperty *srcproperty)
{
	String matchname = srcproperty->GetMatchName();

	ObjectType objecttype = srcproperty->GetObjectType();
	
	if ((srcproperty->GetObjectType()==dstproperty->GetObjectType())
		&&(srcproperty->GetMatchName()==dstproperty->GetMatchName()))
	{
		dstproperty->Copy(srcproperty);
	}
	
	int index = 0;
	HProperty *nextpropertysrc = srcproperty->GetPropertyAt(index), *nextpropertydst = dstproperty->GetPropertyAt(index);
	
	while (nextpropertysrc  && nextpropertydst)
	{
		index++;
		CopyProperties(nextpropertydst,nextpropertysrc);
		nextpropertysrc = srcproperty->GetPropertyAt(index);
		nextpropertydst = dstproperty->GetPropertyAt(index);
	}
}

inline void HashTools::CopyProperties(HTreeObject *dsttreeobject,HTreeObject *srctreeobject)
{
	int indexSrc = 0, indexDst;
	HProperty *nextpropertydst;
	HProperty *nextpropertysrc = srctreeobject->GetPropertyAt(indexSrc);

	while (nextpropertysrc)
	{
		indexSrc++;
		indexDst = 0;
		nextpropertydst = dsttreeobject->GetPropertyAt(indexDst);
		while (nextpropertydst)
		{
			indexDst++;
			CopyProperties(nextpropertydst,nextpropertysrc);
			nextpropertydst = dsttreeobject->GetPropertyAt(indexDst);
		}	
		nextpropertysrc = srctreeobject->GetPropertyAt(indexSrc);	
	}
}

inline HTreeObject *HashTools::FindTreeObject(const char *matchname,HTreeObject *htreeobject)
{
	ObjectType objecttype = htreeobject->GetObjectType();

	String matchname2 = htreeobject->GetMatchName();
	
	if (matchname2 == matchname)
		return htreeobject;

	for (HTreeObject *childhtreeobject = htreeobject->GetTreeProgeny(HTreeObject::SKIPNONE);childhtreeobject;childhtreeobject = childhtreeobject->GetTreeSibling(HTreeObject::SKIPNONE))
	{

		HTreeObject *result = FindTreeObject(matchname,childhtreeobject);
		if (result)
			return result;
	}
	return NULL;
}

inline HTreeObject *HashTools::FindTreeObject(ObjectType type,HTreeObject *htreeobject)
{	
	ObjectType objecttype = htreeobject->GetObjectType();

	String matchname = htreeobject->GetMatchName();
	
	if (objecttype == type)
		return htreeobject;

	for (HTreeObject *childhtreeobject = htreeobject->GetTreeProgeny(HTreeObject::SKIPNONE);childhtreeobject;childhtreeobject = childhtreeobject->GetTreeSibling(HTreeObject::SKIPNONE))
	{

		HTreeObject *result = FindTreeObject(type,childhtreeobject);
		if (result)
			return result;
	}
	return NULL;
}

inline void HashTools::FindTreeObject(const char *matchname,HTreeObject *htreeobject,HashTreeObjects &treeobjects)
{	
	ObjectType objecttype = htreeobject->GetObjectType();

	String matchname2 = htreeobject->GetMatchName();
	
	if (matchname2 == matchname)
		treeobjects.push_back(htreeobject);

	for (HTreeObject *childhtreeobject = htreeobject->GetTreeProgeny(HTreeObject::SKIPNONE);childhtreeobject;childhtreeobject = childhtreeobject->GetTreeSibling(HTreeObject::SKIPNONE))
		FindTreeObject(matchname,childhtreeobject,treeobjects);
}

inline void HashTools::FindTreeObject(ObjectType type,HTreeObject *htreeobject,HashTreeObjects &treeobjects)
{	
	ObjectType objecttype = htreeobject->GetObjectType();

	String matchname = htreeobject->GetMatchName();
	
	if (objecttype == type)
		treeobjects.push_back(htreeobject);

	for (HTreeObject *childhtreeobject = htreeobject->GetTreeProgeny(HTreeObject::SKIPNONE);childhtreeobject;childhtreeobject = childhtreeobject->GetTreeSibling(HTreeObject::SKIPNONE))
		FindTreeObject(type,childhtreeobject,treeobjects);
}

inline HTreeObject *HashTools::FindParentTreeObject(ObjectType type,HTreeObject *htreeobject)
{
	ObjectType objecttype = htreeobject->GetObjectType();
	String matchname = htreeobject->GetMatchName();
	
	if (objecttype == type)
		return htreeobject;

	HTreeObject *parenttreeobject = htreeobject->GetParent();
	if (!parenttreeobject)
		return NULL;

	HTreeObject *result = FindTreeObject(type,htreeobject->GetParent());
	if (result)
		return result;

	return NULL;
}

// Goes to head of stack does not stop on hooks.
inline HCP *HashTools::MyGetHead(HCP *cp)
{
	for (HCP *hcp = cp; hcp->GetPrevAttached(); hcp = hcp->GetPrevAttached());
	return hcp;
}

inline HCP *HashTools::GetHookFromStack(HCP *cp)
{
	if (cp->IsHook())
		return cp;

	for (HCP *hcp = MyGetHead(cp); hcp; hcp = hcp->GetNextAttached())
	{
		int cpindex = hcp->GetID();
		
		if (hcp->IsHook())
			return hcp;
	}
	return NULL;
}

inline HCP *HashTools::GetHookBaseFromStack(HCP *cp)
{
	if (cp->IsHookBase())
		return cp;

	for (HCP *hcp = MyGetHead(cp); hcp; hcp = hcp->GetNextAttached())
	{		
		if (hcp->IsHookBase())
			return hcp;
	}
	return NULL;
}

inline HCP *HashTools::GetHookBaseNeighborFromStack(HCP *cp)
{

	HCP *hookbase = GetHookBaseFromStack(cp);

	if (hookbase)
	{
		return hookbase->GetNext();
	}

	return NULL;
}


inline bool HashTools::IsCPInStack(HCP *cp,HCP *stackcp)
{
	for (HCP *hcp = MyGetHead(stackcp); hcp; hcp = hcp->GetNextAttached())
	{
		if (hcp->GetID() == cp->GetID())
			return true;
	}
	return false;
}

inline bool HashTools::IsHookSegment(HCP *hook, HCP *nextcp)
{
	HCP *hookbaseneighbor = GetHookBaseNeighborFromStack(hook);
	if (hookbaseneighbor)
	{
		int hookindex = hook->GetID();
		int neighborindex = hookbaseneighbor->GetID();
		
		if (IsCPInStack(hookbaseneighbor,nextcp))
			return true;
	}
	return false;
}
/*
float HashTools::InterpolateValue(HChannelDriver *hchanneldriver,HChannelCP *hchannelcp,const Time &time)
{
	HProject *hp = GetHProject();
	float fps = hp->GetFPS();

	HChannelCP *hchannelcpnext = hchannelcp->GetNext();

	switch (hchannelcp->GetInterpolationMethod())
	{
		case CPI_HOLD:
			return hchannelcp->GetValue();
		case CPI_LINEAR:
			return hchannelcp->GetValue() + (time.GetSeconds() - hchannelcp->GetTime().GetSeconds()) /
					(hchannelcpnext->GetTime().GetSeconds() - hchannelcp->GetTime().GetSeconds()) *
					(hchannelcpnext->GetValue() - hchannelcp->GetValue());
		default:
		{
		case CPI_SPLINE:
			float evalx = time.GetSeconds();
			
			CubicCoefficients1D xcoeff( hchannelcp->GetTime().GetSeconds(), hchannelcpnext->GetTime().GetSeconds(), -hchannelcp->GetInTangent().x, -hchannelcpnext->GetOutTangent().x );
			CubicCoefficients1D ycoeff( hchannelcp->GetValue(), hchannelcpnext->GetValue(), -hchannelcp->GetInTangent().y, -hchannelcpnext->GetOutTangent().y );

			float xspan = hchannelcpnext->GetTime().GetSeconds() - hchannelcp->GetTime().GetSeconds();
			float guesst = (evalx - hchannelcp->GetTime().GetSeconds()) / xspan;
			float diffx = evalx - xcoeff.Evaluate( guesst );
			float error = (float)fabs( diffx );
			float lasterror = error;
			float maxerror = MAXFRAMEERROR / fps;
			
			while (error > maxerror)
			{
				guesst = guesst + diffx / xcoeff.Derivative(guesst);
				diffx = evalx - xcoeff.Evaluate( guesst );
				
				error = (float)fabs( diffx );
				if ( error > lasterror ) // NOT CONVERGING, PROBABLY BOGUS CHANNEL DATA, WALK USING BUMP
				{  
					maxerror *= 10;  // DON'T BE AS ACCURATE BECAUSE THIS IS MUCH SLOWER
					int steps = (int)(xspan / maxerror);
					float deltat = 1.0f / steps;
					xcoeff.InitFD( steps );
					int i;
					diffx = error;
					for ( i=0, guesst=0.0; diffx>maxerror && i<steps; guesst+=deltat, i++ )
						diffx = (float)fabs(evalx - xcoeff.BumpFD());
					break;
				}
				lasterror = error;
			}
			return ycoeff.Evaluate( guesst );
		}
	}
}

bool HashTools::Interpolate(HChannelDriver *hchanneldriver,const Time &time,float &value)
{
	HChannelCP *firstcp = hchanneldriver->GetHeadCP();
	HChannelCP *lastcp = firstcp;

	if (!firstcp)
		return false;
	if (!lastcp)
		return false;

	for (;lastcp->GetNext(); lastcp = lastcp->GetNext()){}

	float firstframe = firstcp->GetTime().GetFrame(30.0f);
	float lastframe = lastcp->GetTime().GetFrame(30.0f);
	float currentframe = time.GetFrame(30.0f);

	if (lastcp->GetTime() <= time)
	{	
		value = lastcp->GetValue();
		return true;
	}

	if (firstcp->GetTime() >= time)
	{
		value = firstcp->GetValue();
		return true;
	}

	HChannelCP *hchannelcp = hchanneldriver->GetHeadCP();

	for (; hchannelcp; hchannelcp = hchannelcp->GetNext())
	{
		if (hchannelcp->GetTime() == time)
		{
			value = hchannelcp->GetValue();
			return true;
		}
		else if (hchannelcp->GetNext()->GetTime() == time )
		{
			value = hchannelcp->GetNext()->GetValue();
			return true;
		}
		else if (hchannelcp->GetNext()->GetTime() > time)
		{
			value = InterpolateValue(hchanneldriver,hchannelcp,time);
			return true;
		}
	}
	return false;
}
*/