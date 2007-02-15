// Dan  1/26/01  \Dan90\Plugin\HXT\AV2\Import.cpp

#include "stdafx.h"
#include "Hxt.h"
#include "HashFile.h"
#include "F_Protos.h"
#include "ImportModelDialog.h"
#include "SDK\HModel.h"
#include "SDK\Misc.h"

BOOL CHxtApp::OnImportModelCache(HModelCache *hmc)
{
   CString caption, filter;
   caption.LoadString(IDS_IMPORTTITLE);
   filter.LoadString(IDS_FILEFILTERS);
   String defdir;//not used for multiselect dlg
   String ext("AV2");
   StringList *namelist;

   if (!OpenRequest((LPCTSTR)caption, (LPCTSTR)filter, defdir, ext.Get(), ext.Get(), NULL, NULL, &namelist))
      return TRUE;

   if (namelist==NULL) {
      CString caption;
      caption.LoadString(IDS_IMPORTTITLE);
      CString text;
      text.LoadString(IDS_ERRORIMPORTING);
      MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
      return FALSE;
   }

   CImportModelDialog cdlg;
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   for (POSITION namepos = namelist->GetHeadPosition();namepos;) {
      String filename = namelist->GetNext(namepos);
      String nameonly = filename.GetNameOnly();
      IEPolyModel *polymodel = ImportOBJ(filename);
      if (polymodel==NULL) {
         ReportImportError(filename);
         continue;
      }
      if (!hmc->MergeIEModel(polymodel, filename.Get(), cdlg.m_peaktolerance, TRUE, TRUE))
         ReportImportError(filename);

      delete polymodel;
   }
   DeleteStringList(namelist);

   return TRUE;
}

BOOL CHxtApp::OnImportObjectCacheContainer(HObjectCacheContainer *hocc)
{
   CString caption, filter;
   caption.LoadString(IDS_IMPORTTITLE);
   filter.LoadString(IDS_FILEFILTERS);
   String defdir;//not used for multiselect dlg
   String ext("AV2");
   StringList *namelist;

   if (!OpenRequest((LPCTSTR)caption, (LPCTSTR)filter, defdir, ext.Get(), ext.Get(), NULL, NULL, &namelist))
      return TRUE;

   if (namelist==NULL) {
      CString caption;
      caption.LoadString(IDS_IMPORTTITLE);
      CString text;
      text.LoadString(IDS_ERRORIMPORTING);
      MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
      return FALSE;
   }

   CImportModelDialog cdlg;
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   for (POSITION namepos = namelist->GetHeadPosition();namepos;) {
      String filename = namelist->GetNext(namepos);
      String nameonly = filename.GetNameOnly();
      HModelCache *hmc = HModelCache::New(nameonly.Get());
      if (hmc==NULL) {
         ReportImportError(filename);
         return FALSE;
      }
      IEPolyModel *polymodel = ImportOBJ(filename);
      if (polymodel==NULL) {
         ReportImportError(filename);
         continue;
      }
      if (!hmc->MergeIEModel(polymodel, filename.Get(), cdlg.m_peaktolerance, FALSE, TRUE))
         ReportImportError(filename);

      delete polymodel;
   }

   return TRUE;
}
void CHxtApp::ReportImportError(const String &filename)
{
   CString caption;
   caption.LoadString(IDS_IMPORTTITLE);
   CString text;
   text.LoadString(IDS_ERRORIMPORTINGMODEL);
   String temp = (LPCTSTR)text;
   temp.Replace("%1", filename);
   text = temp.Get();
   MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
}

IEPolyModel *CHxtApp::ImportOBJ(const String &pfilename)
{
/*
   int tint;
   int count;
   BYTE tbyte;
   char name[3];
   FileStream fs;
   String filename = pfilename;

   if (!fs.Open( filename, HashStream::HS_BINARY | HashStream::HS_LOAD))
      return NULL;

   IEPolyModel *model = new IEPolyModel;

   fs.Read(name, 3);

   if (!(name[0]=='A' && name[1]=='V' && name[2] == '2')) {
      CString msg, title;
      msg.LoadString(IDS_INVALIDFILEFORMAT);
	  String temp = (LPCTSTR)msg;
      temp.Replace("%1", filename);
	  msg = temp.Get();
      title.LoadString(IDS_IMPORTTITLE);
      MessageBox(GetMainApplicationWnd(), msg, title, MB_ICONEXCLAMATION);
      return NULL;
   }

   fs >> tint; //Version
   if (tint != 2) {
      CString msg, title;
      msg.LoadString(IDS_INVALIDVERSION);
      title.LoadString(IDS_IMPORTTITLE);
      MessageBox(GetMainApplicationWnd(), msg, title, MB_ICONEXCLAMATION);
      return NULL;
   }

   String oldbasepath = GetBasePath();
   SetBasePath(filename);

// Load Material Attributes List
   fs >> count; //Num Attr
   model->m_attrlist.SetSize(count);
   for (int i =0; i < count; i++) {
      Attr &attr = model->m_attrlist.ElementAt(i);
      BYTE red, green, blue;
      fs >> red >> green >> blue;
      attr.m_diffusecolor.red = (float)red/255.0f;
      attr.m_diffusecolor.green = (float)green/255.0f;
      attr.m_diffusecolor.blue = (float)blue/255.0f;
      fs >> attr.m_diffusefalloff;
      fs >> red >> green >> blue;
      attr.m_specularcolor.red = (float)red/255.0f;
      attr.m_specularcolor.green = (float)green/255.0f;
      attr.m_specularcolor.blue = (float)blue/255.0f;
      fs >> attr.m_specularsize;
      fs >> attr.m_specularintensity;
      fs >> attr.m_ambiance;
      fs >> attr.m_roughness;
      fs >> attr.m_roughnessscale;
      fs >> attr.m_reflectivity;
      fs >> attr.m_transparency;
      fs >> attr.m_refraction;
   }

// Load Texture Maps List
   fs >> count; //Num Textures
   model->m_maplist.SetSize(count);
   for (i=0; i < count; i++) {
      IEMap &map = model->m_maplist.ElementAt(i);
      char temp[256];
      fs.ParseToken(temp, '\0');
      String mapname = temp;
      mapname = mapname.GetAbsolutePath();

      strcpy(map.m_filename, mapname.Get());
      fs >> map.m_width;
      fs >> map.m_height;
   }

// Load Vertex List
   fs >> count;
   model->m_vertexlist.SetSize(count);

   for (i =0; i < count; i++)
      fs >> model->m_vertexlist[i];

   // Load Tangent Control Vertex List
   fs >> count;
   model->m_controlvertexlist.SetSize(count);
   for (i =0; i < count; i++)
      fs >>  model->m_controlvertexlist[i];

   // Load Patch List
   fs >> count;
   model->m_patcharray.SetSize(count);
   for (i =0; i < count; i++) {
      IEPatch *patch = new IEPatch(NULL);
      model->m_patcharray.SetAt(i, patch);

      fs >> patch->m_attrid;
      fs >> patch->m_vertexid[0] >> patch->m_vertexid[1] >> patch->m_vertexid[2] >> patch->m_vertexid[3];
      for (int t = 0; t < 12; t++ )
         fs >> patch->m_controlvertexid[t];

      // Load Patch Texture Maps
      int mapcount;
      fs >> mapcount;
      patch->m_mapnodearray.SetSize(mapcount);
      for (int mc=0; mc < mapcount; mc++) {
         IEPatchMapNode *mapnode = new IEPatchMapNode;
         patch->m_mapnodearray.SetAt(mc, mapnode);
         fs >> mapnode->m_mapid;
         fs >> mapnode->m_percent;
         fs >> mapnode->m_repeat;
         fs >> tbyte; // kind
         mapnode->m_kind = tbyte;
         fs >> tbyte; // seamless;
         mapnode->m_isseamless = tbyte;

         for (int uvcount = 0; uvcount < 4; uvcount++)
            fs >> mapnode->m_uv[uvcount]; // UV

         for (uvcount = 0; uvcount < 8; uvcount++)
            fs >> mapnode->m_controluv[uvcount]; // UV
      }
   }

   //Load Bones
   LoadBones(fs, &model->m_bonehead, NULL);
   SetBasePath(oldbasepath);

   return model;
*/
	return NULL;
}

void CHxtApp::LoadBones(FileStream &fs, IEBone **parentuplink, IEBone *parent)
{
   BYTE child, sibling=TRUE;
   IEBone **siblinguplink = NULL;


   while (sibling && !fs.Eof()) {
      fs.Read(&child, 1);
      fs >> sibling;
      IEBone *bone = new IEBone;
      
      if (parentuplink) {
         *parentuplink = bone;
         parentuplink = NULL;
      }

      if (siblinguplink)
         *siblinguplink = bone;
      fs.ParseToken(bone->m_name, '\0');

      fs >> bone->m_pivotpos;
      fs >> bone->m_endpos;
      fs >> bone->m_rollpos;

      LONG count;
      fs >> count;
      bone->m_vertexidarray.SetSize(count);
      for (int i =0; i < count; i++) {
         fs >> (LONG &)bone->m_vertexidarray[i];
      }

      if (child)
         LoadBones(fs, &bone->m_child, bone);

      siblinguplink = sibling?&bone->m_sibling:NULL;
   }
}
