// Dan  12/20/99  \Dan80\Plugin\IE\PLY\Import.cpp

#include "stdafx.h"
#include "IE.h"
#include "HashHand.h"
#include "HashFile.h"
#include "F_Protos.h"
#include "IEModel.h"
#include "ImportModelDialog.h"

BOOL CIEApp::OnImportModelCacheHandle(ModelCacheHandle *mch)
{
   CString caption, filter;
   caption.LoadString(IDS_IMPORTTITLE);
   filter.LoadString(IDS_FILEFILTERS);
   String defdir;//not used for multiselect dlg
   String ext("PLY");
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
      IEPolyModel *patchmodel = ImportPLY(filename);
      if (patchmodel==NULL) {
         ReportImportError(filename);
         continue;
      }
      if (!mch->MergeIEModel(patchmodel, filename.Get(), cdlg.m_peaktolerance, TRUE, TRUE))
         ReportImportError(filename);

      delete patchmodel;
   }
   DeleteStringList(namelist);

   return TRUE;
}

BOOL CIEApp::OnImportObjectCacheContainerHandle(ObjectCacheContainerHandle *occh)
{
   CString caption, filter;
   caption.LoadString(IDS_IMPORTTITLE);
   filter.LoadString(IDS_FILEFILTERS);
   String defdir;//not used for multiselect dlg
   String ext("PLY");
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
      ModelCacheHandle *mch = ModelCacheHandle::New(nameonly.Get());
      if (mch==NULL) {
         ReportImportError(filename);
         return FALSE;
      }
      IEPolyModel *patchmodel = ImportPLY(filename);
      if (patchmodel==NULL) {
         ReportImportError(filename);
         continue;
      }
      if (!mch->MergeIEModel(patchmodel, filename.Get(), cdlg.m_peaktolerance, FALSE, TRUE))
         ReportImportError(filename);

      delete patchmodel;
   }

   return TRUE;
}

void CIEApp::ReportImportError(const String &filename)
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

IEPolyModel *CIEApp::ImportPLY(const String &pfilename)
{
   float tfloat;
   int count;
   char name[3];
   FileStream fs;
   String filename = pfilename;

   if (!fs.Open( filename, HashStream::HS_BINARY | HashStream::HS_LOAD))
      return NULL;

   IEPolyModel *model = new IEPolyModel;

   fs.Read(name, 3);

   if (!(name[0]=='P' && name[1]=='L' && name[2] == 'Y')) {
      CString msg, title;
      msg.LoadString(IDS_INVALIDFILEFORMAT);
	  String temp = (LPCTSTR)msg;
	  temp.Replace("%1", filename);
	  msg = temp.Get();
      title.LoadString(IDS_IMPORTTITLE);
      MessageBox(GetMainApplicationWnd(), msg, title, MB_ICONEXCLAMATION);
      return NULL;
   }

   int version;
   fs >> version;
   if (version != 2) {
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
      attr.SetDiffuseRed((float)red/255.0f);
      attr.SetDiffuseGreen((float)green/255.0f);
      attr.SetDiffuseBlue((float)blue/255.0f);
      fs >> tfloat;// Diffuse Falloff
      attr.SetDiffuseFalloff(tfloat);
      fs >> red >> green >> blue;//spec color
      attr.SetSpecularRed((float)red/255.0f);
      attr.SetSpecularGreen((float)green/255.0f);
      attr.SetSpecularBlue((float)blue/255.0f);
      fs >> tfloat; // Spec Size
      attr.SetSpecularSize(tfloat);
      fs >> tfloat; // Spec Intensity
      attr.SetSpecularIntensity(tfloat);
      fs >> tfloat; // Ambiance
      attr.SetAmbiance(tfloat);
      fs >> tfloat; // Rough
      attr.SetRoughness(tfloat);
      fs >> tfloat; // Rough Scale
      attr.SetRoughnessScale(tfloat);
      fs >> tfloat; // Reflect
      attr.SetReflectivity(tfloat);
      fs >> tfloat; // Transp
      attr.SetTransparency(Vector(tfloat));
      fs >> tfloat; // Refract
      attr.SetRefraction(tfloat);
   }

// Load Texture Maps List
   fs >> count; //Num Textures
   model->m_maplist.SetSize(count);
   for (i=0; i < count; i++) {
      IEMap &map = model->m_maplist.ElementAt(i);
      String mapname;
      fs.ParseToken(mapname, '\0');
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

   // Load Normals Vertex List
   fs >> count;
   model->m_normallist.SetSize(count);
   for (i =0; i < count; i++)
      fs >>  model->m_normallist[i];

   // Load Poly List
   fs >> count;
   model->m_polyarray.SetSize(count);
   for (i =0; i < count; i++) {
      IEPoly *poly = new IEPoly;
      model->m_polyarray.SetAt(i, poly);

      fs >> poly->m_attrid;
      fs >> poly->m_vertexid[0] >> poly->m_vertexid[1] >> poly->m_vertexid[2] >> poly->m_vertexid[3];
      for (int n = 0; n < 4; n++ )
         fs >> poly->m_normalid[n];

      // Load Poly Texture Maps
      int mapcount;
      fs >> mapcount;
      poly->m_mapnodearray.SetSize(mapcount);
      for (int mc=0; mc < mapcount; mc++) {
         IEMapNode *mapnode = new IEMapNode;
         poly->m_mapnodearray.SetAt(mc, mapnode);
         fs >> mapnode->m_mapid;
         fs >> mapnode->m_percent;
         fs >> mapnode->m_repeat;
         BYTE tbyte;
         fs >> tbyte; // kind
         mapnode->m_kind = tbyte;
         fs >> tbyte; // seamless;
         mapnode->m_isseamless = tbyte;

         for (int uvcount = 0; uvcount < 4; uvcount++)
            fs >> mapnode->m_uv[uvcount]; // UV
      }
   }

   //Load Bones
   LoadBones(fs, &model->m_bonehead, NULL);
   SetBasePath(oldbasepath);

   return model;
}

void CIEApp::LoadBones(FileStream &fs, IEBone **parentuplink, IEBone *parent)
{
   BYTE child, sibling=TRUE;
   IEBone **siblinguplink = NULL;
   String bonename;

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
      fs.ParseToken(bonename, '\0');
      strcpy(bone->m_name, bonename.Get());

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
