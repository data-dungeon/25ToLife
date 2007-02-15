// Dan  1/25/01  \Dan90\Plugin\HXT\AV2\Export.cpp

#include "stdafx.h"
#include "Hxt.h"
#include "HashFile.h"
#include "IEModel.h"
#include "ExportModelDialog.h"
#include "ExportModelsDialog.h"
#include "ExportActionsDialog.h"
#include "F_Protos.h"
#include "sdk\HModel.h"
#include "sdk\HAction.h"
#include "sdk\Misc.h"
#include "sdk\HProject.h"
#include "sdk\HProgres.h"
#include "sdk\HChor.h"

BOOL CHxtApp::OnExportModelCache(HModelCache *hmc)
{

   CExportModelDialog cdlg(hmc->GetName());
   if (cdlg.DoModal()!=IDOK)
      return TRUE;
   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   String modelmapfilename = cdlg.m_filename.GetWithoutSuffix() + ".bmp";

   IEPolyModel *model = hmc->BuildExportModel(&m_iemp, modelmapfilename.Get());
   if (model==NULL) {
      ReportExportError(cdlg.m_filename);
      return FALSE;
   }

   BOOL result = ExportOBJ(model, cdlg.m_filename);
   DeleteIEModel(model);

   if (!result)
      ReportExportError(cdlg.m_filename);

   return result;
}

BOOL CHxtApp::OnExportActionCache(HActionCache *hac)
{
   HModelCache *hmc = hac->GetDefaultModelCache();

   if (hmc==NULL) {
      CString msg, title;
      msg.LoadString(IDS_ACTIONHASNOMODELASSIGNED);
      String temp = (LPCTSTR)msg;
      temp.Replace("%1", hac->GetName());
	   msg = temp.Get();
      title.LoadString(IDS_EXPORTTITLE);

      MessageBox(GetMainApplicationWnd(), msg, title, MB_OK);
      return FALSE;
   }

   CExportModelDialog cdlg(hmc->GetName());
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;

   HProject *hp = GetHProject();
   float fps = hp->GetFPS();
   Time totaltime = hac->GetTotalTime();
   String basefilename = cdlg.m_filename;

   HModel *hm = hac->EditWithModelCache(hmc);

   for (Time time; time<=totaltime; time.StepFrame(fps)) {
      int frame = time.GetFrameInt(fps);
      String filename = basefilename.NumberFrame(frame);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPolyModel *model = hm->BuildExportModel(&m_iemp, time, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         hac->EndEditWithModelCache();
         return FALSE;
      }

      BOOL result = ExportOBJ(model, filename);
      DeleteIEModel(model);

      if (!result) {
         ReportExportError(filename);
         hac->EndEditWithModelCache();
         return FALSE;
      }
   }
   hac->EndEditWithModelCache();
   return TRUE;
}

BOOL CHxtApp::OnExportObjectCacheContainer(HObjectCacheContainer *hocc)
{
   CExportModelsDialog cdlg(hocc);
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;

   for (POSITION pos = cdlg.m_selected.GetHeadPosition(); pos; ) {
      HModelCache *hmc = cdlg.m_selected.GetNext(pos);
      String filename = hmc->GetName();
      filename = filename.GetNameOnlyWithoutSuffix() + ".OBJ";
      filename = CombineNameToPath(cdlg.m_dir, filename);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPolyModel *model = hmc->BuildExportModel(&m_iemp, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         return FALSE;
      }
      BOOL result = ExportOBJ(model, filename);
      DeleteIEModel(model);
      if (!result) {
         ReportExportError(filename);
         return FALSE;
      }
   }
   return TRUE;
}

BOOL CHxtApp::OnExportActionCacheContainer(HActionCacheContainer *hacc)
{
   CExportActionsDialog cdlg(hacc);
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;

   HProject *hp = GetHProject();
   float fps = hp->GetFPS();

   for (POSITION pos = cdlg.m_selected.GetHeadPosition(); pos; ) {
      HActionCache *hac = cdlg.m_selected.GetNext(pos);
      HModelCache *hmc = hac->GetDefaultModelCache();

      if (hmc==NULL) {
         CString msg, title;
         msg.LoadString(IDS_ACTIONHASNOMODELASSIGNED);
		   String temp = (LPCTSTR)msg;
		   temp.Replace("%1", hac->GetName());
		   msg = temp.Get();
         title.LoadString(IDS_EXPORTTITLE);
         MessageBox(GetMainApplicationWnd(), msg, title, MB_OK);
         return FALSE;
      }

      Time totaltime = hac->GetTotalTime();
      String basefilename = hac->GetName();
      basefilename = basefilename.GetNameOnlyWithoutSuffix() + "-0000.OBJ";
      basefilename = CombineNameToPath(cdlg.m_dir, basefilename);

      HModel *hm = hac->EditWithModelCache(hmc);

      for (Time time; time<=totaltime; time.StepFrame(fps)) {
         int frame = time.GetFrameInt(fps);
         String filename = basefilename.NumberFrame(frame);
         String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
         IEPolyModel *model = hm->BuildExportModel(&m_iemp, time, modelmapfilename.Get());
         if (model==NULL) {
            ReportExportError(filename);
            hac->EndEditWithModelCache();
            return FALSE;
         }

         BOOL result = ExportOBJ(model, filename);
         DeleteIEModel(model);

         if (!result) {
            ReportExportError(filename);
            hac->EndEditWithModelCache();
            return FALSE;
         }

      }
      hac->EndEditWithModelCache();
   }
   return TRUE;
}

BOOL CHxtApp::OnExportChor(HChor *hchor)
{
   CExportModelDialog cdlg(hchor->GetName());
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;

   HProject *hp = GetHProject();
   float fps = hp->GetFPS();
   HTimeProperty *totaltime = hchor->GetTotalTime();
   String basefilename = cdlg.m_filename;

   for (Time time; time<=totaltime->GetValue(); time.StepFrame(fps)) {
      int frame = time.GetFrameInt(fps);
      String filename = basefilename.NumberFrame(frame);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPolyModel *model = hchor->BuildExportModel(&m_iemp, time, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         return FALSE;
      }

      BOOL result = ExportOBJ(model, filename);
      DeleteIEModel(model);

      if (!result) {
         ReportExportError(filename);
         return FALSE;
      }
   }
   return TRUE;
}

void CHxtApp::ReportExportError(const String &filename)
{
   CString caption;
   caption.LoadString(IDS_EXPORTTITLE);
   CString text;
   text.LoadString(IDS_ERROREXPORTINGMODEL);
   String temp = (LPCTSTR)text;
   temp.Replace("%1", filename);
   text = temp.Get();
   MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
}
/*
static void CountBoneBranch(IEBone *bone, LONG *count)
{
   for (;bone;bone=bone->m_sibling) {
      (*count)++;
      if (bone->m_child)
         CountBoneBranch(bone->m_child, count);
   }
}

LONG CHxtApp::CountBones(IEPolyModel *model)
{
   LONG count=0;
   CountBoneBranch(model->m_bonehead, &count);
   return count;
}
*/
BOOL CHxtApp::ExportOBJ(IEPolyModel *model, const String &filename)
{
	FileStream fs,fsmat;
	LONG totalitems, curritem = 0;

	String matfname = filename.GetWithoutSuffix() + ".mtl";

//	open '.OBJ' file
	if (!fs.Open( (String)filename, HashStream::HS_ASCII | HashStream::HS_STORE))
		return FALSE;

//	open '.MTL' file
	if (!fsmat.Open((String)matfname, HashStream::HS_ASCII | HashStream::HS_STORE))
		return FALSE;

	String oldpath = GetBasePath();
	SetBasePath(filename); // used to make texturemap filenames relative to model exported location
	// used in conjuction with String::GetRelativePath

	m_hprogressbar = GetProgressBar();
	if (m_hprogressbar) {
		m_hprogressbar->Initialize();
		CString text;
		text.LoadString(IDS_EXPORTTITLE);
		m_hprogressbar->SetMessage1((LPCTSTR)text);
		m_hprogressbar->SetMessage2((LPCTSTR)filename.GetNameOnly().Get());
	}
   
//	Get total items for percent bar
	totalitems = model->m_attrlist.GetSize() +
		model->m_maplist.GetSize() +
		model->m_vertexlist.GetSize() +
		model->m_normallist.GetSize() +
		model->m_polyarray.GetSize() * model->m_maplist.GetSize() +
		model->m_polyarray.GetSize() * 4;

	fs << "# Alias|Wavefront Geometry File (OBJ)" << nl << nl;

	fs << "# material file" << nl;
	fs << "mtllib " << matfname.GetNameOnly() << nl;


	fsmat << "# Alias|Wavefront Material File (MTL)" << nl << nl;

	// default material
	fsmat << "newmtl default" << nl << "Ns 32" << nl << "d 1 " << nl << "illum 2" << nl;
	fsmat << "Kd 0.8 0.8 0.8" << nl << "Ks 0 0 0" << nl << "Ka 0 0 0" << nl;
   
 
//   SaveAttributes(fs, model, totalitems, curritem);
	SaveTextureMaps(fs, fsmat, model, totalitems, curritem);
	SaveVertexList(fs, model, totalitems, curritem);
	SaveNormalList(fs, model, totalitems, curritem);
	SaveUVList(fs, model, totalitems, curritem);
	SavePolyList(fs, model, totalitems, curritem);
   
	if (m_hprogressbar) {
		m_hprogressbar->Uninitialize();
		m_hprogressbar = NULL;
	}

	SetBasePath(oldpath);

	return TRUE;
}

/*
void CHxtApp::SaveAttributes(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_attrlist.GetSize();
   fs << count << nl;
   Attr *attrarray = model->m_attrlist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      fs << (BYTE)(attrarray[i].m_diffusecolor.red*255.0f) << nl;
      fs << (BYTE)(attrarray[i].m_diffusecolor.green*255.0f) << nl;
      fs << (BYTE)(attrarray[i].m_diffusecolor.blue*255.0f) << nl;
      
      fs << attrarray[i].m_diffusefalloff << nl;
      
      fs << (BYTE)(attrarray[i].m_specularcolor.red*255.0f) << nl;
      fs << (BYTE)(attrarray[i].m_specularcolor.green*255.0f) << nl;
      fs << (BYTE)(attrarray[i].m_specularcolor.blue*255.0f) << nl;
      
      fs << attrarray[i].m_specularsize << nl;
      fs << attrarray[i].m_specularintensity << nl;
      fs << attrarray[i].m_ambiance << nl;
      fs << attrarray[i].m_roughness << nl;
      fs << attrarray[i].m_roughnessscale << nl;
      fs << attrarray[i].m_reflectivity << nl;
      fs << attrarray[i].m_transparency << nl;
      fs << attrarray[i].m_refraction << nl;
   }
}
*/

void CHxtApp::SaveTextureMaps(FileStream &fs, FileStream &fsmat, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
	int count = model->m_maplist.GetSize();
	IEMap *maparray = model->m_maplist.GetArray();
	for (int i=0; i < count; i++) {
		if (m_hprogressbar)
			m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
		String mapname = maparray[i].m_filename;
		mapname = mapname.GetRelativePath();
		fsmat << nl;
		fsmat << "newmtl " << mapname.GetNameOnlyWithoutSuffix() << nl;
		fsmat << "Ns 6" << nl << "dl 1" << nl << "illum 2" << nl << "Kd 0.8 0.8 0.8" << nl;
		fsmat << "Ks 0 0 0" << nl << "Ka 0 0 0" << nl;
		fsmat << "map_Kd " << mapname.GetAbsolutePath() << nl;
   }
}

void CHxtApp::SaveVertexList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
	int count = model->m_vertexlist.GetSize();
	fs << nl << "# vertices : " << count << nl;
	Vector *vectorarray = model->m_vertexlist.GetArray();
	for (int i=0; i < count; i++) {
		if (m_hprogressbar)
			m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
		fs << "v " << vectorarray[i].x << " " << vectorarray[i].y << " " << vectorarray[i].z << nl;
   }
}


void CHxtApp::SaveNormalList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_normallist.GetSize();

   fs << nl << "# normals : " << count << nl;

   Vector *vectorarray = model->m_normallist.GetArray();
   for (int i = 0; i < count; i++) {
 		if (m_hprogressbar)
			m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));

	  fs << "vn " << vectorarray[i].x << " " << vectorarray[i].y << " " << vectorarray[i].z << nl;

   }
}

void CHxtApp::SaveUVList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem) {
	IEPoly **polyarray = model->m_polyarray.GetArray();

	int PolyCount = model->m_polyarray.GetSize();

	fs << nl << "# uvs : " << PolyCount*4 << nl;

	for (int nPoly=0; nPoly < PolyCount; nPoly++) {

		int mapnodecount = polyarray[nPoly]->m_mapnodearray.GetSize();

		IEMapNode **mapnodearray = polyarray[nPoly]->m_mapnodearray.GetArray();
		
		if (m_hprogressbar)
			m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));

		if (mapnodecount > 0)
		{
			for ( int i = 0; i < 4; i++ )
				fs << "vt " << mapnodearray[0]->m_uv[i].x << " " << -mapnodearray[0]->m_uv[i].y << " 0" << nl;
		}
		else //If no UV. Make some up.
		{
				fs << "vt 0 1 0" << nl << "vt 0 0 0" << nl << "vt 1 0 0" << nl << "vt 1 1 0" << nl;
		}
	}
}

void CHxtApp::SavePolyList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
	int mindex;
	String matname;

	int PatchCount = model->m_polyarray.GetSize();
	int TextureCount = model->m_maplist.GetSize();

	IEMap *maparray = model->m_maplist.GetArray();
	IEPoly **polyarray = model->m_polyarray.GetArray();

	fs << nl << "# faces : " << PatchCount << nl;

	for (int nTexture=0; nTexture < TextureCount; nTexture ++)
	{
		String textureListName;

		if (nTexture >0) {

			textureListName = maparray[nTexture-1].m_filename;

			fs << "usemtl " << textureListName.GetNameOnlyWithoutSuffix() << nl;
		}
		else 
			fs << "usemtl default" << nl;

		for (int nPatch=0; nPatch < PatchCount; nPatch++)
		{
			if (m_hprogressbar)
				m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));

			int mapnodecount = polyarray[nPatch]->m_mapnodearray.GetSize();

			if (mapnodecount >0)
			{
				if (nTexture>0)
				{
					IEMapNode **mapnodearray = polyarray[nPatch]->m_mapnodearray.GetArray();
					mindex = mapnodearray[0]->m_mapid;
					String textureName = maparray[mindex].m_filename;
						
					if (textureName.GetNameOnlyWithoutSuffix() ==
								textureListName.GetNameOnlyWithoutSuffix())
					{
						fs << "f";
						for (int i = 0; i < 4; i++)
						{
							// Vertex Index
							fs << " " << polyarray[nPatch]->m_vertexid[i]+1;

							// UV Index
							fs << "/" << ((nPatch*4)+i)+1;

							// Normal Index
							fs << "/" << polyarray[nPatch]->m_normalid[i]+1;
						}
						fs << nl;
					}
				}
			}
			else
			{
				fs << "f";
				for (int i = 0; i < 4; i++)
				{
					// Vertex Index
					fs << " " << polyarray[nPatch]->m_vertexid[i]+1;

					// UV Index
					fs << "/" << ((nPatch*4)+i)+1;

					// Normal Index
					fs << "/" << polyarray[nPatch]->m_normalid[i]+1;
				}
				fs << nl;
			}
		}
	}
}
/*
void CHxtApp::SaveBoneBranch(FileStream &fs, IEBone *bone, LONG &totalitems, LONG &curritem)
{
   for (; bone; bone = bone->m_sibling) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      if (bone->m_child)
         fs << (BYTE)1 << nl;
      else
         fs << (BYTE)0 << nl;
      if (bone->m_sibling)
         fs << (BYTE)1 << nl;
      else
         fs << (BYTE)0 << nl;

      fs.Write( bone->m_name, strlen(bone->m_name));
      fs << '\0';
      
      fs << bone->m_pivotpos << nl;
      fs << bone->m_endpos << nl;
      fs << bone->m_rollpos << nl;
      
      int count = bone->m_vertexidarray.GetSize();
      fs << count << nl;
      LONG *array = bone->m_vertexidarray.GetArray();
      for (int i=0; i < count; i++)
         fs << array[i] << nl;

      if (bone->m_child)      
         SaveBoneBranch( fs, bone->m_child, totalitems, curritem );
   }
}

void CHxtApp::SaveBones(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   SaveBoneBranch(fs, model->m_bonehead, totalitems, curritem);
}
*/