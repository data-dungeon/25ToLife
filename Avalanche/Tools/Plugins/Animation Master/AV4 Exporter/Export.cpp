// DAN  1/8/99  \DanB\Plugin\IE\AV4\Export.cpp

#include "stdafx.h"
#include "IE.h"
#include "HashFile.h"
#include "IEModel.h"
#include "HashHand.h"
#include "ExportModelDialog.h"
#include "ExportModelsDialog.h"
#include "ExportActionsDialog.h"
#include "F_Protos.h"
// shader Header
#include "..\AGPSha~1\AGPShader.h"
#include "Shader.h"
//#include "..\..\shaders\AGP\resource.h"

BOOL CIEApp::OnExportModelCacheHandle(ModelCacheHandle *mch)
{

   CExportModelDialog cdlg(mch);
   if (cdlg.DoModal()!=IDOK)
      return TRUE;
   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   String modelmapfilename = cdlg.m_filename.GetWithoutSuffix() + ".bmp";
   IEPatchModel *model = mch->BuildExportModel(&m_iemp, modelmapfilename.Get());
   if (model==NULL) {
      ReportExportError(cdlg.m_filename);
      return FALSE;
   }

   BOOL result = ExportAV3(model, cdlg.m_filename);
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
	  msg=temp.Get();
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
      IEPatchModel *model = mch->BuildPosedExportModel(&m_iemp, ac, time, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         HashHandle::DeleteHandle(ac);
         return FALSE;
      }

      BOOL result = ExportAV3(model, filename);
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

   for (POSITION pos = cdlg.m_selected.GetHeadPosition(); pos; ) {
      ModelCacheHandle *mch = cdlg.m_selected.GetNext(pos);
      String filename = mch->GetName();
      filename = filename.GetNameOnlyWithoutSuffix() + ".AV4";
      filename = CombineNameToPath(cdlg.m_dir, filename);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPatchModel *model = mch->BuildExportModel(&m_iemp, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         return FALSE;
      }
      BOOL result = ExportAV3(model, filename);
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
		 msg=temp.Get();
         title.LoadString(IDS_EXPORTTITLE);
         MessageBox(GetMainApplicationWnd(), msg, title, MB_OK);
         return FALSE;
      }

      Time totaltime = ach->GetTotalTime();
      String basefilename = ach->GetName();
      basefilename = basefilename.GetNameOnlyWithoutSuffix() + "-0000.AV4";
      basefilename = CombineNameToPath(cdlg.m_dir, basefilename);

      // Must delete (HashHandle::DeleteHandle(ac)) this ActionHandle when done using it
      ActionHandle *ac = ach->BuildActionHandle(mch); // builds an action instance with the mch using the ach

      for (Time time; time<=totaltime; time.StepFrame(fps)) {
         int frame = time.GetFrameInt(fps);
         String filename = basefilename.NumberFrame(frame);
         String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
         IEPatchModel *model = mch->BuildPosedExportModel(&m_iemp, ac, time, modelmapfilename.Get());
         if (model==NULL) {
            ReportExportError(filename);
            HashHandle::DeleteHandle(ac);
            return FALSE;
         }

         BOOL result = ExportAV3(model, filename);
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
   text=temp.Get();
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

LONG CIEApp::CountBones(IEPatchModel *model)
{
   LONG count=0;
   CountBoneBranch(model->m_bonehead, &count);
   return count;
}


BOOL CIEApp::ExportAV3(IEPatchModel *pmodel, const String &filename)
{
   FileStream fs;
   LONG totalitems, curritem = 0;
   IEPatchModel2 *model = (IEPatchModel2 *)pmodel; // OK because IEVERSION == 3

   if (!fs.Open( (String)filename, HashStream::HS_BINARY | HashStream::HS_STORE))
      return FALSE;

   String oldpath = GetBasePath();
   SetBasePath(filename);

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
    model->m_materiallist.GetSize() +
    model->m_vertexlist.GetSize() +
    model->m_controlvertexlist.GetSize() +
    model->m_patcharray.GetSize() + CountBones(model);


   fs << "AV4" << nl;
   fs << VERSION << nl;
//	fs << 2 << nl;
   
   SaveAttributes(fs, model, totalitems, curritem);
   SaveMaterials(fs, model, totalitems, curritem);
   SaveShaders(fs, model, totalitems, curritem, filename);
   SaveTextureMaps(fs, model, totalitems, curritem);
   SaveVertexList(fs, model, totalitems, curritem);
   SaveControlVertexList(fs, model, totalitems, curritem);
   SavePatches(fs, model, totalitems, curritem);
   SaveBones(fs, model, totalitems, curritem);
   
   if (m_progressbarhandle) {
      m_progressbarhandle->Uninitialize();
      m_progressbarhandle = NULL;
   }
   SetBasePath(oldpath);
   return TRUE;
}

void CIEApp::SaveAttributes(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
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

void CIEApp::SaveMaterials(FileStream &fs, IEPatchModel2 *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_materiallist.GetSize();
   fs << count << nl;
   for (int i=0; i < count; i++) {
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));
      String matname = model->m_materiallist[i]->m_name;
      matname = matname.GetRelativePath();
      fs.Write(matname.Get(), matname.length());
      fs << '\0';
   }
}

// New
void CIEApp::SaveShaders(FileStream &fs, IEPatchModel2 *model, LONG &totalitems, LONG &curritem, String fname)
{
   int totalshaders = model->m_shaderlist.GetSize();
   //walk all the shaders on the patch and look for an Avalanche shader
   AGPShader *agp = NULL;
   AGPShaderExport agpexport;
   AGPShaderExport_2 agpexport2;

	fs << totalshaders << nl;

   for (int i=0; i < totalshaders; i++) {
      Shader *shader = model->m_shaderlist[i];
      String pluginname = shader->GetClassName();
      if (pluginname == "Avalanche\\GameProps") {
         agp = (AGPShader *)shader;
		 //Change environment map path to a relative path by comparing export path and current environ map path
			if(strcmp(agp->m_environmappath,"<NONE>")!=0)
			{
				unsigned int dupindex=0;
				unsigned int slashindex=0;

				//convert file name to capitals and using backslash
				for (unsigned int i=0; i<fname.length(); i++)
				{
					//If it's a forward slash, convert to backslash
					if(fname.buf[i]==47)
						fname.buf[i]=92;
					//If it's lowercase, change to uppercase
					if((fname.buf[i]>96) && (fname.buf[i]<123))
						fname.buf[i]-=32;
				}
				//convert environmental path to capitals and using backslash
				for(i=0; i<strlen(agp->m_environmappath); i++)
				{
					//If it's a forward slash, convert to backslash
					if(agp->m_environmappath[i]==47)
						agp->m_environmappath[i]=92;
					//If it's lowercase, change to uppercase
					if((agp->m_environmappath[i]>96) && (agp->m_environmappath[i]<123))
						agp->m_environmappath[i]-=32;
				}
				//Find the last backslash in the duplicated part of the path
				while(agp->m_environmappath[dupindex]==fname.buf[dupindex])
				{
					if(fname.buf[dupindex]==92)
						slashindex=dupindex;
					dupindex++;
					if((dupindex>fname.length()) || (dupindex>strlen(agp->m_environmappath)))
						break;
				};
				if((slashindex > 0) && (slashindex<strlen(agp->m_environmappath)))
				{
					//Move the slashindex forward one to skip the '\'
					slashindex++;
					for (i=0; i<strlen(agp->m_environmappath)-slashindex; i++)
					{
						agp->m_environmappath[i]=agp->m_environmappath[slashindex+i];
					}
					//End the string so it doesn't run into the previous data.
					agp->m_environmappath[i]='\0';
				}
				
			}
			//Zero out all flags before filling so that we don't get any strange information
			memset(&agpexport.bfFlags,0,sizeof(agpexport.bfFlags));
			//set up the agpexport structure
			agpexport.bfFlags.collision=agp->m_collision;
			agpexport.bfFlags.terrain=agp->m_terrain;
			agpexport.bfFlags.layer=agp->m_layer;
			agpexport.bfFlags.tessadj=agp->m_tessadj;
			agpexport.bfFlags.flat=agp->m_flat;
			agpexport.bfFlags.deletepatch=agp->m_deletepatch;
			agpexport.bfFlags.dnclip=agp->m_dnclip;
			agpexport.bfFlags.dnfog=agp->m_dnfog;
			agpexport.bfFlags.twosided=agp->m_twosided;
			agpexport.bfFlags.zbias=agp->m_zbias;
			agpexport.bfFlags.lod=agp->m_lod;
			agpexport.bfFlags.environmap=agp->m_environmap;
			agpexport.bfFlags.invisible=agp->m_invisible;
			agpexport.bfFlags.mapprivate=agp->m_private;
			agpexport.bfFlags.alpha=agp->m_alpha;
			agpexport.bfFlags.vc32bit=agp->m_32bitvc;
			agpexport.bfFlags.overridecolor=agp->m_coloroverride;
			agpexport.siPretess				= agp->m_pretess;
			agpexport.siEnvironMapPercent	= agp->m_environmentmappercent;
			strcpy(agpexport2.strProjectName,agp->m_projecttext);
			if (agp->m_collision==0)
			{
				if (strcmp(agp->m_terraintext,"<NONE>")==0)
					strcpy(agp->m_terraintext,"Default");
				if (strcmp(agp->m_terraintext,"")==0)
					strcpy(agp->m_terraintext,"Default");
			}

			strcpy(agpexport2.strTerrainType,agp->m_terraintext);
			strcpy(agpexport.strEnvironMapPath,agp->m_environmappath);
			fs.Write(&agpexport,sizeof(AGPShaderExport));
			fs.Write(&agpexport2,sizeof(AGPShaderExport_2));
      }
   }

}

void CIEApp::SaveTextureMaps(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_maplist.GetSize();
   fs << count << nl;
   IEMap *maparray = model->m_maplist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));
      String mapname = maparray[i].m_filename;
      mapname = mapname.GetRelativePath();
      fs.Write(mapname.Get(), mapname.length());
      fs << '\0';
      fs << maparray[i].m_width << sp << maparray[i].m_height << nl;
   }
}

void CIEApp::SaveVertexList(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_vertexlist.GetSize();
   fs << count << nl;
   Vector *vectorarray = model->m_vertexlist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));
      fs << vectorarray[i] << nl;
   }
}

void CIEApp::SaveControlVertexList(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_controlvertexlist.GetSize();
   fs << count << nl;
   Vector *vectorarray = model->m_controlvertexlist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));
      fs << vectorarray[i] << nl;
   }
}

void CIEApp::SavePatches(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_patcharray.GetSize();
   fs << count << nl;

   for (int i=0; i < count; i++) {
   	IEPatch2 *patch = (IEPatch2 *)model->m_patcharray[i];
      if (m_progressbarhandle)
         m_progressbarhandle->SetProgress((int)((float)curritem++/totalitems*100));
      fs << patch->m_attrid << nl;

      int materialidcount = patch->m_materialidarray.GetSize();
      fs << materialidcount << nl;
      for (int mc=0; mc < materialidcount; mc++)
         fs << patch->m_materialidarray[mc] << nl;

// new to version 3
      int shaderidcount = patch->m_shaderidarray.GetSize();
      fs << shaderidcount << nl;
      for (int sc=0; sc < shaderidcount; sc++)
         fs << patch->m_shaderidarray[sc] << nl;

      fs << patch->m_flags;

      int c;
      for (c=0; c < 4; c++)
         fs << patch->m_vertexid[c] << nl;
      for (c=0; c < 12; c++)
         fs << patch->m_controlvertexid[c] << nl;
      int mapnodecount = patch->m_mapnodearray.GetSize();
      IEPatchMapNode **mapnodearray = patch->m_mapnodearray.GetArray();
      fs << mapnodecount << nl;
      for (int mi=0; mi < mapnodecount; mi++) {
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

void CIEApp::SaveBones(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem)
{
   SaveBoneBranch(fs, model->m_bonehead, totalitems, curritem);
}
