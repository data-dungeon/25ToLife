// Dan  1/25/01  \Dan90\Plugin\HXT\AV2\Export.cpp

#include "stdafx.h"
#include "Hxt.h"
#include "ExportModelDialog.h"
#include "ExportModelsDialog.h"
#include "ExportActionsDialog.h"
#include "F_Protos.h"
#include "sdk\HCP.h"
#include "sdk\HModel.h"
#include "sdk\HAction.h"
#include "sdk\Misc.h"
#include "sdk\HProject.h"
#include "sdk\HProgres.h"
#include "sdk\HChor.h"

#pragma warning (disable : 4800)
#pragma warning (disable : 4786)

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

   IEPatchModel *model = hmc->BuildExportModel(&m_iemp, modelmapfilename.Get());
   if (model==NULL) {
      ReportExportError(cdlg.m_filename);
      return FALSE;
   }

   BOOL result = ExportAV4(model, hmc, cdlg.m_filename);
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
      IEPatchModel *model = hm->BuildExportModel(&m_iemp, time, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         hac->EndEditWithModelCache();
         return FALSE;
      }

      BOOL result = ExportAV4(model, hmc, filename);
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
      filename = filename.GetNameOnlyWithoutSuffix() + ".AV4";
      filename = CombineNameToPath(cdlg.m_dir, filename);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPatchModel *model = hmc->BuildExportModel(&m_iemp, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         return FALSE;
      }
      BOOL result = ExportAV4(model, hmc, filename);
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
      basefilename = basefilename.GetNameOnlyWithoutSuffix() + "-0000.AV4";
      basefilename = CombineNameToPath(cdlg.m_dir, basefilename);

      HModel *hm = hac->EditWithModelCache(hmc);

      for (Time time; time<=totaltime; time.StepFrame(fps)) {
         int frame = time.GetFrameInt(fps);
         String filename = basefilename.NumberFrame(frame);
         String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
         IEPatchModel *model = hm->BuildExportModel(&m_iemp, time, modelmapfilename.Get());
         if (model==NULL) {
            ReportExportError(filename);
            hac->EndEditWithModelCache();
            return FALSE;
         }

         BOOL result = ExportAV4(model, hmc, filename);
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
      IEPatchModel *model = hchor->BuildExportModel(&m_iemp, time, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         return FALSE;
      }

      BOOL result = ExportAV4(model, NULL, filename);
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

// This exported is just the AV@ file exporter, but with examples on how to retrieve the 
// Property information this plugin added.
BOOL CHxtApp::ExportAV4(IEPatchModel *model, HModelCache *hmc, const String &filename)
{
   FileStream fs;
   LONG totalitems, curritem = 0;

#ifdef KEEPLOG
	log.open("AV4LOG.TXT");
	log << "AV4LogFile.\n";
#endif

	m_collectdata.Collect(hmc,model,&m_ModelProperties,&m_GroupProperties);

   if (!fs.Open( (String)filename, HashStream::HS_BINARY | HashStream::HS_STORE))
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
		m_collectdata.GetVertexCount() +
		model->m_controlvertexlist.GetSize() +
		m_collectdata.GetPatchCount() +
		m_collectdata.GetBoneCount();

   fs << "AV4" << nl;//file version
   fs << 4L << nl;//version number

	SaveAttributes(fs, model, totalitems, curritem);
	SaveMaterials(fs, model, totalitems, curritem);
	
	SaveShaders(fs, model, hmc, totalitems, curritem);

	SaveTextureMaps(fs, model, totalitems, curritem);
	SaveVertexList(fs, model, totalitems, curritem);
	SaveControlVertexList(fs, model, totalitems, curritem);

	SavePatches(fs, model, totalitems, curritem);

	SaveBones(fs, totalitems, curritem);

   if (m_hprogressbar)
   {
      m_hprogressbar->Uninitialize();
      m_hprogressbar = NULL;
   }
   SetBasePath(oldpath);

#ifdef KEEPLOG
	log.close();
#endif	

	if (m_collectdata.GetVertexBoneCount() !=0)
	{
		char temp[256];
		itoa(m_collectdata.GetVertexBoneCount(),temp,10);
		CString message = temp + CString(" vertex bones created and saved.");
		MessageBox(NULL,LPCTSTR(message), "FYI", 0  );
	}

   return TRUE;
}

void CHxtApp::SaveMaterials(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
	//Material ID is no longer available in v9.0, so save 0 as the total number
	//of materials
	fs << (int)0 << nl;
}

void CHxtApp::SaveAttributes(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
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

void CHxtApp::SaveTextureMaps(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_maplist.GetSize();
   fs << count << nl;
   IEMap *maparray = model->m_maplist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      String mapname = maparray[i].m_filename;
      mapname = mapname.GetRelativePath();
      fs.Write(mapname.Get(), mapname.length());
      fs << '\0';
      fs << maparray[i].m_width << sp << maparray[i].m_height << nl;
   }
}

void CHxtApp::SaveVertexList(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
	std::vector<Vector> *vertices = m_collectdata.GetVertexPool();

   int count = vertices->size();
   fs << count << nl;
   for (int i=0; i < count; i++) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
	  Vector test = (*vertices)[i];
      fs << (*vertices)[i] << nl;
   }
}

void CHxtApp::SaveControlVertexList(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_controlvertexlist.GetSize();
   fs << count << nl;
   Vector *vectorarray = model->m_controlvertexlist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      fs << vectorarray[i] << nl;
   }
}

void CHxtApp::SavePatches(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
	IntMap *propertyMap = m_collectdata.GetPropertyMap();
	std::vector<IntArray> *propertyEntries = m_collectdata.GetPropertyEntries();

	std::vector<myPatch> *patches = m_collectdata.GetPatches();
	int patchindex;
	int count = model->m_patcharray.GetSize();
	fs << count << nl;
	for (int i=0; i < model->m_patcharray.GetSize(); i++) {

		IEPatch *patch = model->m_patcharray[i];
		myPatch *mypatch = &(*patches)[i];
		
		if (m_hprogressbar)
			m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));

		fs << patch->m_attrid << nl;
//		Save 0 for the material ID count
		fs << (int)0 << nl;

//		Save shader indicies
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		HPatch *hpatch = patch->m_origpatch;

		if (propertyMap->hasKey((int)hpatch))
		{
			patchindex = propertyMap->getValue((int)hpatch);
			int propertyCount = (*propertyEntries)[patchindex].size();
			fs << (int)(*propertyEntries)[patchindex].size() << nl;
			int debug = (int)(*propertyEntries)[patchindex].size();
			for (int j = 0; j < (*propertyEntries)[patchindex].size(); j++){
//				AGPShader debug = m_collectdata.m_agpShaders[(int)(*propertyEntries)[patchindex][j]];
				fs << (int)(*propertyEntries)[patchindex][j] << nl;
			}
		}
		else
		{
			fs << (int)0 << nl;
		}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		fs << patch->m_flags;

		for (int c=0; c < 4; c++)
			fs << (LONG)mypatch->m_vertexid[c] << nl;


		for (c=0; c < 12; c++)
			fs << patch->m_controlvertexid[c] << nl;

		int mapnodecount = patch->m_mapnodearray.GetSize();
		IEPatchMapNode **mapnodearray = patch->m_mapnodearray.GetArray();
		fs << mapnodecount << nl;
		for (int mi=0; mi < mapnodecount; mi++) {

//			char *debug = model->m_maplist[mapnodearray[mi]->m_mapid].m_filename;

			fs << mapnodearray[mi]->m_mapid << nl;
			fs << mapnodearray[mi]->m_percent << nl;
			fs << mapnodearray[mi]->m_repeat << nl;
			fs << (BYTE)mapnodearray[mi]->m_kind << nl;
			fs << (BYTE)mapnodearray[mi]->m_isseamless << nl;
			fs << (BYTE)mapnodearray[mi]->m_isgroupdecal << nl;
         
			for (c=0; c < 4; c++)
				fs << mapnodearray[mi]->m_uv[c] << nl;
			for (c=0; c < 8; c++)// only 8 corner control UV's are available
				fs << mapnodearray[mi]->m_controluv[c] << nl;
		}
	}
}

void CHxtApp::SaveBones(FileStream &fs,LONG &totalitems, LONG &curritem)
{
	m_vbonemap.clear();
	
	int vertexcount = m_collectdata.GetVertexCount();

	for (int i = 0; i < vertexcount; i++)
	{
		
		myBone *mybone =m_collectdata.GetBoneWithVid(i);	
//		if (!mybone)
//		{
//					char temp[256];
//					itoa(i,temp,10);
//					CString warning = CString("Vertex index [ ") + temp +
//						CString(" ] doesn't have a bone.");
//					MessageBox(NULL,(LPCTSTR)warning, "Warning", 0  );
//		}

	}

//	Save Root Bone
	myBone *rootbone = m_collectdata.GetRootBone();

	if (rootbone->HasChild()) {
		fs << (BYTE)1 << nl;
		fs << (BYTE)0 << nl;

		fs.Write( rootbone->m_name.c_str(), rootbone->m_name.length());
		fs << '\0';
      
		fs << rootbone->m_pivotpos << nl;
		fs << rootbone->m_endpos << nl;
		fs << rootbone->m_rollpos << nl;

		fs << (int)0<< nl;

	//	Save Children
		SaveBoneBranch(fs,rootbone,totalitems,curritem);
	}
}

void CHxtApp::SaveBoneBranch(FileStream &fs,myBone *mybone,LONG &totalitems, LONG &curritem)
{
	for (int i = 0; i < mybone->m_children.size(); i++)
	{

			if (m_hprogressbar)
				m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));

			if (mybone->m_children[i]->HasChild())
				fs << (BYTE)1 << nl;
			else
				fs << (BYTE)0 << nl;

			if (i < mybone->m_children.size()-1)
				fs << (BYTE)1 << nl;
			else
				fs << (BYTE)0 << nl;

			fs.Write( mybone->m_children[i]->m_name.c_str(), mybone->m_children[i]->m_name.length());
			fs << '\0';
      
			fs << mybone->m_children[i]->m_pivotpos << nl;
			fs << mybone->m_children[i]->m_endpos << nl;
			fs << mybone->m_children[i]->m_rollpos << nl;

			int count = mybone->m_children[i]->m_vids.size();

			fs << (int)count << nl;

			for (int j = 0; j < count; j++)
			{
				fs << mybone->m_children[i]->m_vids[j] << nl;
				if (m_vbonemap.hasKey(mybone->m_children[i]->m_vids[j]))
				{
					char temp[256];
					itoa(mybone->m_children[i]->m_vids[j],temp,10);
					CString warning = CString("Vertex index [ ") + temp +
						CString(" ] belongs to multiple bones.");
					MessageBox(NULL,(LPCTSTR)warning, "Warning", 0  );
				}
				else
				{
					m_vbonemap.setValue(mybone->m_children[i]->m_vids[j],(int)&mybone->m_children[i]);
				}
			}

#ifdef KEEPLOG
	log << "Bone [" << mybone->m_children[i]->m_name.c_str() << "]\n";
//	log << "Vertex Assigned:\n";
//	for (int k = 0; k < mybone->m_children[i]->m_vids.size(); k++)
//		log << mybone->m_children[i]->m_vids[k] << "\n";
#endif
			SaveBoneBranch(fs,mybone->m_children[i],totalitems,curritem);
	}
}


void CHxtApp::SaveShaders(FileStream &fs, IEPatchModel *model, HModelCache *hmc, LONG &totalitems, LONG &curritem)
{
	std::vector<AGPShader> *agpShaders;

	agpShaders = m_collectdata.GetShaderPool();

	int count = agpShaders->size();
	fs << (int)count << nl;
	for (int i = 0; i < count; i++)
		fs.Write(&(*agpShaders)[i].AGPStruct,sizeof(FullAGPShaderExport));

	return;
}
