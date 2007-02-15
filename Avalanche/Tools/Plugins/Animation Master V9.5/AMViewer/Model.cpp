// Dan  6/23/99  \Dan71\AMViewer\Model.cpp

#include "stdafx.h"
#include "AMViewerDoc.h"
#include "AMViewerView.h"
#include "ScriptView.h"
#include "Archive.h"
#include "MainFrm.h"
#include "AV2.h"
#include "AV3.h"
#include "AV4.h"
#include "Ply.h"


extern CMainFrame *g_mainframe;

Model::Model()
{
   m_span = -1.0f;
   m_bonehead = NULL;
   m_lasttexture = 0;
   m_lastattr = -1;
   m_lastattrdiffuse = -1;
}

Model::~Model()
{
   DeleteBones(m_bonehead);
}

void Model::DeleteBones(Bone *startbone) 
{
   Bone *sibling, *bone;
   
   for (bone=startbone; bone; bone=sibling) {
      sibling = bone->m_sibling;
      if (bone->m_child)
         DeleteBones(bone->m_child);
      delete bone;
   }   
}

void Model::LoadBones(CArchive &ar, Bone **parentuplink, Bone *parent) 
{
   BYTE child, sibling=TRUE;
   Bone **siblinguplink = NULL;

   while (sibling && ar.Read(&child, 1)==1) {
      ar >> sibling;
      Bone *bone = new Bone;
      
      if (parentuplink) {
         *parentuplink = bone;
         parentuplink = NULL;
      }
      bone->m_parent=parent;

      if (siblinguplink)
         *siblinguplink = bone;
      bone->m_name = ReadString(ar);

      Vector pivotpos, endpos, rollpos;

      ar >> pivotpos;
      ar >> endpos;
      ar >> rollpos;

	  bone->m_pivot = pivotpos;
	  bone->m_end = endpos;
	  bone->m_roll = rollpos;

      bone->ComputeBoneToStandard(pivotpos, endpos, rollpos);

      LONG count;
      ar >> count;
      bone->m_vertexidarray.SetSize(count);
      for (int i =0; i < count; i++) {
         ar >> (LONG &)bone->m_vertexidarray[i];
      }

      if (child)
         LoadBones(ar, &bone->m_child, bone);

      siblinguplink = sibling?&bone->m_sibling:NULL;
   }
}

void Model::SetSpan()
{
   for (int i=0; i<m_vertexlist.GetSize(); i++) {
      Vector &vertex = m_vertexlist.ElementAt(i);
      float length = vertex.Norm();
      if (length > m_span)
         m_span = length;
   }
}

void Model::Init(const CString &filename) 
{
   m_name = filename;
   SetSpan();
   FillListIndexesOnBones();
   BuildTextures();
}

void Model::RebuildBinds()
{
// non model maps
   for (int i=0; i<m_maplist.GetSize(); i++) {
      Map &map = m_maplist.ElementAt(i);
      if (map.m_textureclamp) {
         glBindTexture(GL_TEXTURE_2D, map.m_textureclamp);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR:GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST);
      }

      if (map.m_texturerepeat) {
         glBindTexture(GL_TEXTURE_2D, map.m_texturerepeat);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR:GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST);
      }
   }
// model maps
   RebuildModelMapBinds();
}

void Model::BuildTextures()
{
   CString modelmapfilename = m_name.Left(m_name.ReverseFind('.'))+".BMP";
   static GLubyte white[] = {255, 255, 255};

   m_notexture = 0;

   glGenTextures(1, &m_notexture);
   glBindTexture(GL_TEXTURE_2D, m_notexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);
   
   int i;
   for (i=0; i<m_maplist.GetSize(); i++) {
      Map &map = m_maplist.ElementAt(i);
      if (modelmapfilename == (LPCTSTR)map.m_filename)
         BuildModelMaps(i, map); // cuts up the model map and stores a separate texture display list for each patch
      else
         map.BuildTextureMaps();
   }
}

void Model::FillVertexBoneList(Bone *bone, CArray<Bone *, Bone *> &vertexbonelist)
{
   for (;bone;bone=bone->m_sibling) {
      int count = bone->m_vertexidarray.GetSize();
      for (int i=0; i<count; i++) {
         LONG id = bone->m_vertexidarray[i];
         vertexbonelist[bone->m_vertexidarray[i]]=bone;
      }
      if (bone->m_child)
         FillVertexBoneList(bone->m_child, vertexbonelist);
   }
}

BOOL CAMViewerDoc::LoadModel(const CString &filename)
{
   CString ext;

   ext = filename.Mid(filename.ReverseFind('.')+1);
   ext.MakeLower();

   Model *model;

   if (ext == "av2")
      model = new AV2Model;
   else if (ext == "av3")
      model = new AV3Model;
   else if (ext == "av4")
	   model = new AV4Model;
   else if (ext == "ply")
      model = new PLYModel;
   else {
      CString text;
      text.Format("Unknown file format %s.", filename);
      AfxMessageBox( filename, MB_OK );
      return FALSE;
   }
      
   CFile file;
   CFileException fileexception;
 
   if (!file.Open( filename, CFile::modeRead, &fileexception)) {
      CString text;
      text.Format("Can't open file %s, error = %u\n", filename, fileexception.m_cause);
      AfxMessageBox( text, MB_OK );
      return FALSE;
   }

   CArchive ar(&file, CArchive::load);
   TRY {
      CWaitCursor wait;
      if (file.GetLength() != 0)
         model->Serialize(ar);
      ar.Close();
      file.Close();
   }
   CATCH(CUserException, e) {
      ar.Close();
      file.Close();
      delete model;
      return FALSE;
   }
   AND_CATCH_ALL(e) {
      ar.Close();
      file.Close();
      delete model;
      e->ReportError();
      return FALSE;
   }
   END_CATCH_ALL

   model->Init(filename);
   SetModel(model);

   CString name = filename.Mid(filename.ReverseFind(PATHSEP)+1);//Remove Path
   g_mainframe->m_scriptview->SetDlgItemText(IDC_MODELNAME_EDIT, name);
   return TRUE;
}