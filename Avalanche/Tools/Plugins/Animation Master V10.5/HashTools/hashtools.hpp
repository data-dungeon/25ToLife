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


//  Get a patch array of all patches belonging to group.
// "patches" do not get cleared
inline void HashTools::GetPatchesInGroup(HashPatches &patches,HGroup *group,HModelCache *hmc) {
	patches.clear();
	int patchcount = hmc->GetPatchCount();
	for (int i = 0; i < patchcount; i++) {
		HPatch *patch = hmc->GetPatch(i);
		if (group->IsPatchInGroup(patch))
			patches.push_back(patch);
	}	
}

inline void HashTools::GetGroupsFromPatch(HashGroups &groups,HPatch *patch,HashGroups &allGroups) {
	groups.clear();
	for (int i = 0; i < allGroups.size(); i++) {
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
	for (int i = 0; i < stamps.size(); i++)
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
	for (int i = 0; i < allDecals.size(); i++) {
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
	for (int i = 0; i < alldecals.size(); i++) {

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

inline HProperty *HashTools::FindPropertyInPluginProperties(const char *matchname,HProperty *pluginproperties) {
	return FindProperty(matchname,pluginproperties);
}

inline HProperty *HashTools::FindProperty(const char *matchname,HProperty *pluginproperty) {

	HPropertyInfo *propinfo = (HPropertyInfo *)pluginproperty->GetPropertyInfo();

	String test = propinfo->GetLocalizedName();
	String test2 = propinfo->GetMatchName();

	if (test == matchname)
		return pluginproperty;

	HProperty *nextprop;
	int index = 0;
	while (HProperty *prop = pluginproperty->GetPropertyAt(index++)) {
		nextprop = FindProperty(matchname,prop);
		if (nextprop)
			return nextprop;
	}

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
