#include "stdafx.h"
#include "Hxt.h"   
#include "../hashtools/hashtools.h" 
#include "matrix34.h"
#include "sdk/hproject.h"
     
BOOL CHxtApp::Evaluate(HTreeObject *htreeobject)
{
	HGroup *hg = (HGroup *) htreeobject;
	HModelCache *hmc = (HModelCache *)hg->GetParentOfType(HOT_MODEL);
	HProject *project = (HProject*)hmc->GetParentOfType(HOT_PROJECT);

	HashDecals alldecals;

	HashTools::GetAllDecals(hmc,alldecals);

	HashPatches selpatches;

	HashTools::GetPatchesInGroup(selpatches,hg,hmc);

	if (selpatches.size()==0)
		return FALSE;//No patches selected

	HashDecals patchdecals;
	HashStamps stamps;
	HashBasePatchUVs patchuvs;
	Vector2 uvs[12];

	for (int i = 0; i < selpatches.size(); i++)
	{
		HashTools::GetDecalsFromPatch(patchdecals,selpatches[i],alldecals);
		if (patchdecals.size() > 0)
		{
			HDecal *decal_on_top = patchdecals.back();
			HashTools::GetStampsFromDecalThatBelongToPatch(decal_on_top,stamps,selpatches[i]);
			HDecalStamp *stamp_on_top = stamps.back();
			HashTools::GetBasePatchUVsFromStamp(stamp_on_top,patchuvs);
			Vector2 midpoint = CalcBoundingBoxMidpoint(patchuvs);
			for (int j = 0; j < patchuvs.size(); j++) {
				patchuvs[j]->GetUVs(uvs);
				FlipUVs(uvs,midpoint);
				patchuvs[j]->SetUVs(uvs);
			}
		}
	}
	project->UpdateAllViews(true);

	return TRUE;
}

Vector2 CHxtApp::FlipUV(const Vector2 uv,const Vector2 midpoint) {
	Matrix34 mat;
	mat.SetIdentity();
	mat.SetTranslate(Vector(-1*midpoint.x,-1*midpoint.y,0));
	Vector v(uv.x,uv.y,1);
	TransformVector(mat,v);

	Vector scalev(-1.0f,1.0f,1.0f);
	v *= scalev;
	
	mat.SetIdentity();
	mat.SetTranslate(Vector(midpoint.x,midpoint.y,0));
	TransformVector(mat,v);
	return Vector2(v.x,v.y);
}

void CHxtApp::FlipUVs(Vector2 *uvs,const Vector2 midpoint) {
	for (int i = 0; i < 12; i++)
		uvs[i] = FlipUV(uvs[i],midpoint);
}

Vector2 CHxtApp::CalcBoundingBoxMidpoint(HashBasePatchUVs &patchuvs)
{
	double xmax=0,xmin=0,ymax=0,ymin=0;
	Vector2 uvs[12];

	for (int j = 0; j < patchuvs.size(); j++)
	{
		patchuvs[j]->GetUVs(uvs);
		for (int k = 0; k < 12; k++)
		{
			if (uvs[k].x < xmin)
				xmin = uvs[k].x;
			if (uvs[k].x > xmax)
				xmax = uvs[k].x;
			if (uvs[k].y < ymin)
				ymin = uvs[k].y;
			if (uvs[k].y > ymax)
				ymax = uvs[k].y;
		}
	}

	return ((xmax-xmin)*0.5)+xmin,((ymax-ymin)*0.5)+ymin;
}
