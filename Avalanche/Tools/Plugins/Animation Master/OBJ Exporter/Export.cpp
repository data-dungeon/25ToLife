// DAN  1/8/99  \DanB\Plugin\IE\PLY\Export.cpp

#include "stdafx.h"
#include "IE.h"
#include "HashFile.h"
#include "IEModel.h"
#include "HashHand.h"
#include "ExportModelDialog.h"
#include "ExportModelsDialog.h"
#include "ExportActionsDialog.h"
#include "F_Protos.h"

BOOL CIEApp::OnExportModelCacheHandle(ModelCacheHandle *mch)
{

   CExportModelDialog cdlg(mch);
   if (cdlg.DoModal()!=IDOK)
      return TRUE;
   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   m_iemp.m_subtype = (SubType)cdlg.m_subtype;

   String modelmapfilename = cdlg.m_filename.GetWithoutSuffix() + ".bmp";
   IEPolyModel *model = mch->BuildExportModel(&m_iemp, modelmapfilename.Get());
   if (model==NULL) {
      ReportExportError(cdlg.m_filename);
      return FALSE;
   }

   BOOL result = ExportOBJ(model, cdlg.m_filename);
   mch->DeleteIEModel(model);

   if (!result)
      ReportExportError(cdlg.m_filename);

   return result;
}

BOOL CIEApp::OnExportActionCacheHandle(ActionCacheHandle *ach)
{
   ModelCacheHandle *mch = ach->GetDefaultModelCacheHandle();

   if (mch==NULL) {
      CString msg, title;
      msg.LoadString(IDS_ACTIONHASNOMODELASSIGNED);
	  String temp = (LPCTSTR)msg;
	  temp.Replace("%1", ach->GetName());
	  msg = temp.Get();
      title.LoadString(IDS_EXPORTTITLE);

      MessageBox(GetMainApplicationWnd(), msg, title, MB_OK);
      return FALSE;
   }

   CExportModelDialog cdlg(mch);
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   m_iemp.m_subtype = (SubType)cdlg.m_subtype;

   ProjectHandle *ph = GetProjectHandle();
   float fps = ph->GetFPS();
   Time totaltime = ach->GetTotalTime();
   String basefilename = cdlg.m_filename;

   // Must delete (HashHandle::DeleteHandle(ac)) this ActionHandle when done using it
   ActionHandle *ac = ach->BuildActionHandle(mch); // builds an action instance with the mch using the ach

   for (Time time; time<=totaltime; time.StepFrame(fps)) {
      int frame = time.GetFrameInt(fps);
      String filename = basefilename.NumberFrame(frame);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPolyModel *model = mch->BuildPosedExportModel(&m_iemp, ac, time, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         HashHandle::DeleteHandle(ac);
         return FALSE;
      }

      BOOL result = ExportOBJ(model, filename);
      mch->DeleteIEModel(model);

      if (!result) {
         ReportExportError(filename);
         HashHandle::DeleteHandle(ac);
         return FALSE;
      }

   }
   HashHandle::DeleteHandle(ac);
   return TRUE;
}

BOOL CIEApp::OnExportObjectCacheContainerHandle(ObjectCacheContainerHandle *occh)
{
   CExportModelsDialog cdlg(occh);
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   m_iemp.m_subtype = (SubType)cdlg.m_subtype;

   for (POSITION pos = cdlg.m_selected.GetHeadPosition(); pos; ) {
      ModelCacheHandle *mch = cdlg.m_selected.GetNext(pos);
      String filename = mch->GetName();
      filename = filename.GetNameOnlyWithoutSuffix() + ".obj";
      filename = CombineNameToPath(cdlg.m_dir, filename);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPolyModel *model = mch->BuildExportModel(&m_iemp, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         return FALSE;
      }
      BOOL result = ExportOBJ(model, filename);
      mch->DeleteIEModel(model);
      if (!result) {
         ReportExportError(filename);
         return FALSE;
      }
   }
   return TRUE;
}

BOOL CIEApp::OnExportActionCacheContainerHandle(ActionCacheContainerHandle *acch)
{
   CExportActionsDialog cdlg(acch);
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   m_iemp.m_subtype = (SubType)cdlg.m_subtype;

   ProjectHandle *ph = GetProjectHandle();
   float fps = ph->GetFPS();

   for (POSITION pos = cdlg.m_selected.GetHeadPosition(); pos; ) {
      ActionCacheHandle *ach = cdlg.m_selected.GetNext(pos);
      ModelCacheHandle *mch = ach->GetDefaultModelCacheHandle();

      if (mch==NULL) {
         CString msg, title;
         msg.LoadString(IDS_ACTIONHASNOMODELASSIGNED);
		 String temp = (LPCTSTR)msg;
         temp.Replace("%1", ach->GetName());
		 msg = temp.Get();
         title.LoadString(IDS_EXPORTTITLE);
         MessageBox(GetMainApplicationWnd(), msg, title, MB_OK);
         return FALSE;
      }

      Time totaltime = ach->GetTotalTime();
      String basefilename = ach->GetName();
      basefilename = basefilename.GetNameOnlyWithoutSuffix() + "-0000.obj";
      basefilename = CombineNameToPath(cdlg.m_dir, basefilename);

      // Must delete (HashHandle::DeleteHandle(ac)) this ActionHandle when done using it
      ActionHandle *ac = ach->BuildActionHandle(mch); // builds an action instance with the mch using the ach

      for (Time time; time<=totaltime; time.StepFrame(fps)) {
         int frame = time.GetFrameInt(fps);
         String filename = basefilename.NumberFrame(frame);
         String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
         IEPolyModel *model = mch->BuildPosedExportModel(&m_iemp, ac, time, modelmapfilename.Get());
         if (model==NULL) {
            ReportExportError(filename);
            HashHandle::DeleteHandle(ac);
            return FALSE;
         }

         BOOL result = ExportOBJ(model, filename);
         mch->DeleteIEModel(model);

         if (!result) {
            ReportExportError(filename);
            HashHandle::DeleteHandle(ac);
            return FALSE;
         }

      }
      HashHandle::DeleteHandle(ac);
   }
   return TRUE;
}

void CIEApp::ReportExportError(const String &filename)
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
 
static void CountBoneBranch(IEBone *bone, LONG *count)
{
   for (;bone;bone=bone->m_sibling) {
      (*count)++;
      if (bone->m_child)
         CountBoneBranch(bone->m_child, count);
   }  
}

LONG CIEApp::CountBones(IEPolyModel *model)
{
   LONG count=0;
   CountBoneBranch(model->m_bonehead, &count);
   return count;
}


BOOL CIEApp::ExportOBJ(IEPolyModel *model, const String &filename)
{
   FileStream fs,fsmat;
   LONG totalitems, curritem = 0;
   String matfname;

   matfname = filename.GetWithoutSuffix() + ".mtl";

   if (!fs.Open( (String)filename, HashStream::HS_ASCII | HashStream::HS_STORE))
      return FALSE;

   if (!fsmat.Open((String)matfname, HashStream::HS_ASCII | HashStream::HS_STORE))
	   return FALSE;

   String oldpath = GetBasePath();
   SetBasePath(filename); // used to make texturemap filenames relative to model exported location
   // used in conjuction with String::GetRelativePath

   m_progressbarhandle = GetProgressBarHandle();
   if (m_progressbarhandle) {
      m_progressbarhandle->Initialize();
      CString text;
      text.LoadString(IDS_EXPORTTITLE);
      m_progressbarhandle->SetMessage1((LPCTSTR)text);
      m_progressbarhandle->SetMessage2((LPCTSTR)filename.GetNameOnly().Get());
   }
   
// Get total items for percent bar
   totalitems = model->m_attrlist.GetSize() +
   model->m_maplist.GetSize() +
   model->m_vertexlist.GetSize() +
   model->m_normallist.GetSize() +
   model->m_polyarray.GetSize() + CountBones(model);

   fs << "# Alias Wavefront OBJ" << nl << nl;
   fs << "#version 0.1 - antony jones." << nl <<nl;


   fsmat << "#Wavefront Material File." << nl;
   fsmat << "#version 0.1 - antony jones." << nl <<nl;

   // default material
   fsmat << "newmtl default" << nl << "Ns 32" << nl << "d 1 " << nl << "illum 2" << nl;
   fsmat << "Kd 0.8 0.8 0.8" << nl << "Ks 0 0 0" << nl << "Ka 0 0 0" << nl;
   
//   fs << "mtllib " << matfname.GetAbsolutePath() << nl << nl;
   fs << "mtllib " << matfname.GetNameOnly() << nl << nl;

//   SaveAttributes(fs, model, totalitems, curritem);
   SaveTextureMaps(fs, fsmat, model, totalitems, curritem);
   SaveVertexList(fs, model, totalitems, curritem);
   SaveNormalList(fs, model, totalitems, curritem);
   SavePolys(fs, model, totalitems, curritem);
//   SaveBones(fs, model, totalitems, curritem);

   fsmat.Close();
   
   if (m_progressbarhandle) {
      m_progressbarhandle->Uninitialize();
      m_progressbarhandle = NULL;
   }
   SetBasePath(oldpath);
   return TRUE;
}

void CIEApp::SaveAttributes(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_attrlist.GetSize();
   fs << count << nl;
   Attr *attrarray = model->m_attrlist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));
      fs << (BYTE)(attrarray[i].GetDiffuseColor().red*255.0f) << nl;
      fs << (BYTE)(attrarray[i].GetDiffuseColor().green*255.0f) << nl;
      fs << (BYTE)(attrarray[i].GetDiffuseColor().blue*255.0f) << nl;
      
      fs << attrarray[i].GetDiffuseFalloff() << nl;
      
      fs << (BYTE)(attrarray[i].GetSpecularColor().red*255.0f) << nl;
      fs << (BYTE)(attrarray[i].GetSpecularColor().green*255.0f) << nl;
      fs << (BYTE)(attrarray[i].GetSpecularColor().blue*255.0f) << nl;
      
      fs << attrarray[i].GetSpecularSize() << nl;
      fs << attrarray[i].GetSpecularIntensity() << nl;
      fs << attrarray[i].GetAmbiance() << nl;
      fs << attrarray[i].GetRoughness() << nl;
      fs << attrarray[i].GetRoughnessScale() << nl;
      fs << attrarray[i].GetReflectivity() << nl;
      fs << attrarray[i].GetTransparency().x << nl;// only use x component for transparency
      fs << attrarray[i].GetRefraction() << nl;
   }
}

void CIEApp::SaveTextureMaps(FileStream &fs, FileStream &fsmat, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_maplist.GetSize();
//   fs << count << nl;
   IEMap *maparray = model->m_maplist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));
      String mapname = maparray[i].m_filename;
      mapname = mapname.GetRelativePath();
	  fsmat << nl;
	  fsmat << "newmtl " << mapname.GetNameOnlyWithoutSuffix() << nl;
	  fsmat << "Ns 6" << nl << "dl 1" << nl << "illum 2" << nl << "Kd 0.8 0.8 0.8" << nl;
	  fsmat << "Ks 0 0 0" << nl << "Ka 0 0 0" << nl;

	  fsmat << "map_Kd " << mapname.GetAbsolutePath() << nl;
//	  fsmat << "map_Kd " << mapname.GetNameOnly() << nl;

//    fs.Write(mapname.Get(), mapname.length());
//    fs << '\0';
//    fs << maparray[i].m_width << sp << maparray[i].m_height << nl;
   }
}

void CIEApp::SaveVertexList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_vertexlist.GetSize();

	fs << "# " << count << " vertices" << nl;

   Vector *vectorarray = model->m_vertexlist.GetArray();

   for (int i=0; i < count; i++) {
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));


      fs << "v " << vectorarray[i].x << " " << vectorarray[i].y << " " << vectorarray[i].z << nl;
   }
}

void CIEApp::SaveNormalList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_normallist.GetSize();

   fs << "# " << count << " normals" << nl;

   Vector *vectorarray = model->m_normallist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));

	  fs << "vn " << vectorarray[i].x << " " << vectorarray[i].y << " " << vectorarray[i].z << nl;

   }
}

void CIEApp::SavePolys(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
	int c = 0, mindex;
	String matname;

	IEMap *maparray = model->m_maplist.GetArray();
	IEPoly **polyarray = model->m_polyarray.GetArray();

	int patchCount = model->m_polyarray.GetSize();
	int texCount = model->m_maplist.GetSize();

//	for (int nTexture=0; nTexture < texCount; nTexture ++){

		for (int nPatch=0; nPatch < patchCount; nPatch++) {

		if (m_progressbarhandle)
			m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));


		///////////////////////////////////////////////////////////////
		// Output UV's
	  

		int mapnodecount = polyarray[nPatch]->m_mapnodearray.GetSize();

		IEMapNode **mapnodearray = polyarray[nPatch]->m_mapnodearray.GetArray();
/*	   
		fs << mapnodecount << nl;
		for (int mi=0; mi < mapnodecount; mi++) {
        fs << mapnodearray[mi]->m_mapid << nl;
        fs << mapnodearray[mi]->m_percent << nl;
        fs << mapnodearray[mi]->m_repeat << nl;
        fs << (BYTE)mapnodearray[mi]->m_kind << nl;
        fs << (BYTE)mapnodearray[mi]->m_isseamless << nl;
        for (c=0; c < 4; c++)
			fs << mapnodearray[mi]->m_uv[c] << nl;
      }
*/
		if (mapnodecount > 0) 
			for ( c = 0; c < 4; c++ )
				fs << "vt " << mapnodearray[0]->m_uv[c].x << " " << -mapnodearray[0]->m_uv[c].y << " 0" << nl;
			else
				fs << "vt 0 1 0" << nl << "vt 0 0 0" << nl << "vt 1 0 0" << nl << "vt 1 1 0" << nl;

		} // end of nPatch loop

		fs << "# " << patchCount << " faces" << nl;

	for (int nTexture=0; nTexture <= texCount; nTexture ++){
		String textureListName;

		if (nTexture >0) {

			textureListName = maparray[nTexture-1].m_filename;

			fs << "usemtl " << textureListName.GetNameOnlyWithoutSuffix() << nl;

		}
		else 
			fs << "usemtl default" << nl;

		for (int nPatch=0; nPatch < patchCount; nPatch++) {


				if (m_progressbarhandle)
					m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));

				// Output faces

//				fs << "g Group0" << nl;

				int mapnodecount = polyarray[nPatch]->m_mapnodearray.GetSize();

				if (mapnodecount >0) {

					if (nTexture>0) {

						IEMapNode **mapnodearray = polyarray[nPatch]->m_mapnodearray.GetArray();

						mindex = mapnodearray[0]->m_mapid;

						String textureName = maparray[mindex].m_filename;

//						fs << "texturenamelist = " << textureNameList << ", texturename = " << textureName << nl;
			
						if (textureName.GetNameOnlyWithoutSuffix() == textureListName.GetNameOnlyWithoutSuffix()) {
							
							fs << "f";
		
							for (c = 0; c < 4; c++) {
		
								// Vertex Index

								fs << " " << polyarray[nPatch]->m_vertexid[c]+1;

								// UV Index

								fs << "/" << ((nPatch*4)+c)+1;

								// Normal Index

								fs << "/" << polyarray[nPatch]->m_normalid[c]+1;

							}// for loop c

							fs << nl;

						} //if textureName == textureNameList

					} // if nTexture>0

				} // if mapnodecount>0

				else {
					fs << "f";
		
					for (c = 0; c < 4; c++) {
		
						// Vertex Index

						fs << " " << polyarray[nPatch]->m_vertexid[c]+1;

						// UV Index

						fs << "/" << ((nPatch*4)+c)+1;

						// Normal Index

						fs << "/" << polyarray[nPatch]->m_normalid[c]+1;

						}// for loop c

					fs << nl;

					}

		}// for nPatch loop

	}//for nTexture loop

}

void CIEApp::SaveBoneBranch(FileStream &fs, IEBone *bone, LONG &totalitems, LONG &curritem)
{
   for (; bone; bone = bone->m_sibling) {
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));
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

void CIEApp::SaveBones(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   SaveBoneBranch(fs, model->m_bonehead, totalitems, curritem);
}
