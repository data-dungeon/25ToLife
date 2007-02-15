#include "stdafx.h"
#include "Hxt.h"
#include <vector>
#include "../hashtools/hashtools.h"
#include "sdk/hproject.h"
#include <afxwin.h>
#include <afxdlgs.h>
#include "patchinfo.h"

BOOL CHxtApp::Evaluate(HTreeObject *htreeobject)
{
	bool ClampMultiple = MultipleClampStretch || MultipleClampNoStretch;

	HGroup *hg = (HGroup *) htreeobject;
	HModelCache *hmc = (HModelCache *)hg->GetParentOfType(HOT_MODEL);

	HashDecals alldecals;

	HashTools::GetAllDecals(hmc,alldecals);

	HashPatches selpatches;

	HashTools::GetPatchesInGroup(selpatches,hg,hmc);

	if (selpatches.size()==0)
		return FALSE;//No patches selected

	NeighborTree ntree;

//	if (ClampMultiple) {
		ntree.GenerateNeighborInfo(selpatches);
		ntree.GenerateTopologyInfo();
		ntree.GenerateDistanceInfo();
//	}
		
	HClip *newclip;
	HDecal *newdecal;
	HDecalClip *decalclip;
	HProject *project = (HProject*)hmc->GetParentOfType(HOT_PROJECT);
	std::string filetitle;

	if (!BlendMapping)
	{
		CFileDialog dlg(true,NULL,NULL,NULL,"Images (*.tga,*.tif)|*.tga;*.tif|All Files (*.*)|*.*||",NULL);

		int result = dlg.DoModal();

		if (result == 2)
			return false;

		CString pathname = dlg.GetPathName();

		filetitle = (char*)(LPCTSTR)dlg.GetFileTitle();

		const char *clipname= (LPCTSTR)pathname;

		newclip = project->AddClip(clipname,false);
		newdecal = hmc->AddDecal(newclip);
		decalclip = (HDecalClip *)newdecal->GetChildDecalClipContainer()->GetChild();
		decalclip->GetRepeat()->GetX()->SetValue(1.00001f);
		decalclip->GetRepeat()->GetY()->SetValue(1.00001f);

	}
	else
	{
		newclip = project->AddClip(PrimaryMapPath,false);

		newdecal = hmc->AddDecal(newclip);

		decalclip = (HDecalClip *)newdecal->GetChildDecalClipContainer()->GetChild();
		decalclip->GetRepeat()->GetX()->SetValue(1.00001f);
		decalclip->GetRepeat()->GetY()->SetValue(1.00001f);
		
		newclip = project->AddClip(AlphaMapPath,false);
		decalclip = newdecal->AddDecalClip(newclip);
		decalclip->GetKind()->SetValue(HDecalClip::NEXTMAPFACTORMAP);
		decalclip->GetRepeat()->GetX()->SetValue(1.00001f);
		decalclip->GetRepeat()->GetY()->SetValue(1.00001f);
	
			
		newclip = project->AddClip(SecondaryMapPath,false);
		decalclip = newdecal->AddDecalClip(newclip);
		decalclip->GetRepeat()->GetX()->SetValue(1.00001f);
		decalclip->GetRepeat()->GetY()->SetValue(1.00001f);

		filetitle = "BlendDecal";
	}

	HashTools::MakeUniqueDecalName(filetitle,alldecals);

	newdecal->SetName(filetitle.c_str());

	Vector2 *uvs = new Vector2[12];

	int col,row;
	bool successful;

	std::vector<Vector2> uvarray;
	uvarray.resize(4);

	HDecalStamp *stamp = NULL;
	if (ClampMultiple)
		stamp = newdecal->AddDecalStamp();

	for (int i = 0; i < selpatches.size(); i++) {

		HDecalStamp *stamp2;

		if (!ClampMultiple)
			stamp2 = newdecal->AddDecalStamp();

//		if (ClampMultiple) {
			successful = ntree.GetPatchColAndRow(i,col,row);
			if (successful) {
				if (MultipleClampStretch) {
					uvarray[0] = ntree.CalcPatchUV(0,col,row);
					uvarray[1] = ntree.CalcPatchUV(1,col,row);
					uvarray[2] = ntree.CalcPatchUV(2,col,row);
					uvarray[3] = ntree.CalcPatchUV(3,col,row);
				}
				else if (MultipleClampNoStretch)
				{
					uvarray[0] = ntree.CalcPatchUVStretch(0,col,row);
					uvarray[1] = ntree.CalcPatchUVStretch(1,col,row);
					uvarray[2] = ntree.CalcPatchUVStretch(2,col,row);
					uvarray[3] = ntree.CalcPatchUVStretch(3,col,row);
				}
				else if (SingleClamp)
				{
					uvarray[0] = Vector2(0,1);
					uvarray[1] = Vector2(1,1);
					uvarray[2] = Vector2(1,0);
					uvarray[3] = Vector2(0,0);
				}
			}
			
			int offset = ntree.GetVertexOffset(i,col,row);
			ntree.ShiftIntArray(offset,uvarray);
			ClampUVToPatch(uvs,uvarray[0],uvarray[3],uvarray[2],uvarray[1]);

			if (ClampMultiple) {
				HBasePatchUV *patchuv = stamp->AddPatchUV(selpatches[i],uvs);
			}
			else {
				HBasePatchUV *patchuv = stamp2->AddPatchUV(selpatches[i],uvs);
			}
				
//		}

//		if (!ClampMultiple) {
//			ClampUVToPatch(uvs,Vector2(0,1),Vector2(1,1),Vector2(1,0),Vector2(0,0));
//			HBasePatchUV *patchuv = stamp->AddPatchUV(selpatches[i],uvs);
//		}
	}

	delete uvs;

	project->UpdateAllViews(true);

	return TRUE;
}

void CHxtApp::ClampUVToPatch(Vector2 *uvs,Vector2 upperleft,Vector2 upperright,Vector2 lowerright,
								Vector2 lowerleft) {

	uvs[0] = upperleft;

	Vector2 temp = (upperright-upperleft);
	temp /= 3.0;
	uvs[1] = upperleft + temp;

	temp = (upperleft-upperright);
	temp /= 3.0;
	uvs[2] = upperright + temp;

	uvs[3] = upperright;

	temp = (lowerright-upperright);
	temp /= 3.0;
	uvs[4] = upperright + temp;

	temp = (upperright-lowerright);
	temp /= 3.0;
	uvs[5] = lowerright + temp;

	uvs[6] = lowerright;

	temp = (lowerleft-lowerright);
	temp /= 3.0;
	uvs[7] = lowerright + temp;

	temp = (lowerright-lowerleft);
	temp /= 3.0;
	uvs[8] = lowerleft + temp;

	uvs[9] = lowerleft;

	temp = upperleft-lowerleft;
	temp /= 3.0;
	uvs[10] = lowerleft + temp;

	temp = (lowerleft-upperleft);
	temp /= 3.0;
	uvs[11] = upperleft + temp;

}

/*
inline Vector2 CHxtApp::calcUVWithInBoundary(double s, double t) {
	Vector2 uv;

	Vector2 uv_edgeA,uv_edgeB,uv_edgeC,uv_edgeD;

	Vector2 uv_upperleft(0,0);
	Vector2 uv_lowerleft(0,1);
	Vector2 uv_lowerright(1,1);
	Vector2 uv_upperright(1,0);

	uv_edgeB = (s * uv_lowerright) + ((1-s) * uv_lowerleft);
	uv_edgeD = (s * uv_upperright) + ((1-s) * uv_upperleft);
	Vector2 q = (t * uv_edgeB) + ((1-t) * uv_edgeD);

	uv_edgeA = (t * uv_lowerleft) + ((1-t) * uv_upperleft);
	uv_edgeC = (t * uv_lowerright) + ((1-t) * uv_upperright);
	Vector2 r = (s * uv_edgeC) + ((1-s) * uv_edgeA);

	uv = (q + r) * 0.5;

	return uv;
}
*/