#include "stdafx.h"
#include "Hxt.h"
#include <vector>
#include "hashtools.h"
#include "sdk/hproject.h"
        
BOOL CHxtApp::Evaluate(HTreeObject *htreeobject)
{
	HGroup *hg = (HGroup *) htreeobject;
	HModelCache *hmc = (HModelCache *)hg->GetParentOfType(HOT_MODEL);
	
	HashGroups allgroups;
	HashDecals alldecals;
	
	HashTools::GetAllDecals(hmc,alldecals);
	HashTools::GetAllGroups(hmc,allgroups);

	HashPatches selpatches;

	HashTools::GetPatchesInGroup(selpatches,hg,hmc);

//	if (patches.size()==0)
//		ERROR("No patches selected.");

	HashGroups groups;
	for (int i = 0; i < selpatches.size(); i++) {
		groups.clear();
		HashTools::GetGroupsFromPatch(groups,selpatches[i],allgroups);

		HashAttributes attributes;
		for (int j = 0; j < groups.size(); j++)
			HashTools::GetMaterialAttributesFromGroup(groups[j],attributes);

		for (j = 0; j < attributes.size(); j++) {
			HProperty *surface = HashTools::GetSurface(attributes[j]);
			HColorProperty *diffuse = (HColorProperty*)surface->GetPropertyAt(DIFFUSECOLOR);
			diffuse->GetRed()->StoreValue(0,0);
			diffuse->GetGreen()->StoreValue(0,0);
			diffuse->GetBlue()->StoreValue(0,1);
		}	
	}

	HProject *project = (HProject*)hg->GetParentOfType(HOT_PROJECT);

//	project->UpdateAllViews();
	
	HashDecals decals;
	HashStamps stamps;

	for (i = 0; i < selpatches.size(); i++) {
		HashTools::GetDecalsFromPatch(decals,selpatches[i],alldecals);
		for (int j = 0; j < decals.size(); j++) {
			char *name = decals[i]->GetName();
			for (int k = 0; k < decals.size(); k++) {
				HashTools::GetStampsFromDecal(decals[k],stamps);
				for (int n = 0; n < stamps.size(); n++) {
					char *name = stamps[n]->GetName();
				}
			}
		}
	}


	for (i = 0; i < alldecals.size(); i++)
		char *name = alldecals[i]->GetName();
	
	return TRUE;
}