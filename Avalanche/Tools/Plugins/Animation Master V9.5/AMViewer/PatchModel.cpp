// Dan  6/23/99  \Dan71\AMViewer\PatchModel.cpp

#include "stdafx.h"
#include "Model.h"
#include "BitmapNo.h"
#include "MainFrm.h"
#include "AMViewerView.h"
#ifdef _MAC
#include "3rdParty\Conix\glut.h"
#else
#include <gl\glut.h>
#endif

#define SMALL           .01

extern CMainFrame *g_mainframe;

void PatchModel::SetSpan()
{
   Model::SetSpan();

   for (int i=0; i<m_controlvertexlist.GetSize(); i++) {
      Vector &vertex = m_controlvertexlist.ElementAt(i);
      float length = vertex.Norm();
      if (length > m_span)
         m_span = length;
   }
}

static void ShrinkBoneCVandNLists(Bone *bone)
{
   for (;bone;bone=bone->m_sibling) {
      bone->m_controlvertexidarray.FreeExtra();
      bone->m_normalidarray.FreeExtra();
      if (bone->m_child)
         ShrinkBoneCVandNLists(bone->m_child);
   }
}

void PatchModel::FillListIndexesOnBones()
{
   if (m_bonehead==NULL)
      return;
   CArray<Bone *, Bone *> vertexbonelist;// contains bone * for each vertex;
   vertexbonelist.SetSize(m_vertexlist.GetSize());
   FillVertexBoneList(m_bonehead, vertexbonelist);

   CMap<LONG, LONG &, Bone *, Bone *> cvtobonemap;
   cvtobonemap.InitHashTable(997);

   CMap<LONG, LONG &, Bone *, Bone *> ntobonemap;
   ntobonemap.InitHashTable(997);

// Add controlvertexid's to CMap to remove duplicates
// Add normalid's to CMap to remove duplicates
   int count = m_patchlist.GetSize();
   for (int pi=0; pi<count; pi++) {
      Patch &patch = m_patchlist[pi];
      Bone *bone = vertexbonelist[patch.m_vertexid[0]];

      if (bone) {
         cvtobonemap.SetAt(patch.m_controlvertexid[2], bone);
         cvtobonemap.SetAt(patch.m_controlvertexid[0], bone);
         cvtobonemap.SetAt(patch.m_controlvertexid[3], bone);
         ntobonemap.SetAt(patch.m_normalid[0], bone);
      }
      bone = vertexbonelist[patch.m_vertexid[1]];
      if (bone) {
         cvtobonemap.SetAt(patch.m_controlvertexid[1], bone);
         cvtobonemap.SetAt(patch.m_controlvertexid[5], bone);
         cvtobonemap.SetAt(patch.m_controlvertexid[4], bone);
         ntobonemap.SetAt(patch.m_normalid[1], bone);
      }
      bone = vertexbonelist[patch.m_vertexid[2]];
      if (bone) {
         cvtobonemap.SetAt(patch.m_controlvertexid[9], bone);
         cvtobonemap.SetAt(patch.m_controlvertexid[11], bone);
         cvtobonemap.SetAt(patch.m_controlvertexid[8], bone);
         ntobonemap.SetAt(patch.m_normalid[2], bone);
      }
      bone = vertexbonelist[patch.m_vertexid[3]];
      if (bone) {
         cvtobonemap.SetAt(patch.m_controlvertexid[10], bone);
         cvtobonemap.SetAt(patch.m_controlvertexid[6], bone);
         cvtobonemap.SetAt(patch.m_controlvertexid[7], bone);
         ntobonemap.SetAt(patch.m_normalid[3], bone);
      }
   }

   for (POSITION pos = cvtobonemap.GetStartPosition(); pos;) {
      Bone *bone;
      LONG cvindex;
      
      cvtobonemap.GetNextAssoc(pos, cvindex, bone);
      bone->m_controlvertexidarray.Add(cvindex);
   }
   cvtobonemap.RemoveAll();

   for (pos = ntobonemap.GetStartPosition(); pos;) {
      Bone *bone;
      LONG normalindex;
      
      ntobonemap.GetNextAssoc(pos, normalindex, bone);
      bone->m_normalidarray.Add(normalindex);
   }
   ntobonemap.RemoveAll();

   ShrinkBoneCVandNLists(m_bonehead);
}
   
void PatchModel::BuildNormals()
{
   CMap <NormalKey, NormalKey &, LONG, LONG &> normaltable;

   normaltable.InitHashTable(997);
   m_normallist.SetSize(0, 1000);

   for (int patchindex = 0; patchindex < m_patchlist.GetSize(); patchindex++) {
      Patch &patch = m_patchlist.ElementAt(patchindex);
      Vector tan1, tan2, vertex, normal;
      LONG normalid;
      NormalKey key;

      tan1 = m_controlvertexlist[patch.m_controlvertexid[2]];
      tan2 = m_controlvertexlist[patch.m_controlvertexid[0]];
      vertex = m_vertexlist[patch.m_vertexid[0]];
      tan1-=vertex;
      tan2-=vertex;
      normal = tan2^tan1;

      if (fabs(normal.x) < SMALL && fabs(normal.y) < SMALL
       && fabs(normal.z) < SMALL) {
         if (patch.m_is3point) //3 point patch when 0 and 3 are the same
            tan1 = m_controlvertexlist[patch.m_controlvertexid[10]];
         else
            tan1 = m_vertexlist[patch.m_vertexid[3]];
         tan1 -= vertex;
         normal = tan2^tan1;
      }

      normal.Normalize(); 

      key.Set(normal, patch.m_vertexid[0]);
      if (!normaltable.Lookup(key, normalid)) {
         normalid = m_normallist.Add(normal);
         normaltable.SetAt(key, normalid);
      }
      patch.m_normalid[0] = normalid;


      tan1 = m_controlvertexlist[patch.m_controlvertexid[1]];
      tan2 = m_controlvertexlist[patch.m_controlvertexid[5]];
      vertex = m_vertexlist[patch.m_vertexid[1]];
      tan1-=vertex;
      tan2-=vertex;
      normal = tan2^tan1;

      if (fabs(normal.x) < SMALL && fabs(normal.y) < SMALL
       && fabs(normal.z) < SMALL) {
         tan1 = m_vertexlist[patch.m_vertexid[0]];
         tan1 -= vertex;
         normal = tan2^tan1;
      }

      normal.Normalize();

      key.Set(normal, patch.m_vertexid[1]);
      if (!normaltable.Lookup(key, normalid)) {
         normalid = m_normallist.Add(normal);
         normaltable.SetAt(key, normalid);
      }
      patch.m_normalid[1] = normalid;


      tan1 = m_controlvertexlist[patch.m_controlvertexid[9]];
      tan2 = m_controlvertexlist[patch.m_controlvertexid[11]];
      vertex = m_vertexlist[patch.m_vertexid[2]];
      tan1-=vertex;
      tan2-=vertex;
      normal = tan2^tan1;

      if (fabs(normal.x) < SMALL && fabs(normal.y) < SMALL
       && fabs(normal.z) < SMALL) {
         tan1 = m_vertexlist[patch.m_vertexid[1]];
         tan1 -= vertex;
         normal = tan2^tan1;
      }

      normal.Normalize();
      
      key.Set(normal, patch.m_vertexid[2]);
      if (!normaltable.Lookup(key, normalid)) {
         normalid = m_normallist.Add(normal);
         normaltable.SetAt(key, normalid);
      }
      patch.m_normalid[2] = normalid;


      if (patch.m_is3point)
         patch.m_normalid[3] = patch.m_normalid[0];
      else {
         tan1 = m_controlvertexlist[patch.m_controlvertexid[10]];
         tan2 = m_controlvertexlist[patch.m_controlvertexid[6]];
         vertex = m_vertexlist[patch.m_vertexid[3]];
         tan1-=vertex;
         tan2-=vertex;
         normal = tan2^tan1;

         if (fabs(normal.x) < SMALL && fabs(normal.y) < SMALL
          && fabs(normal.z) < SMALL) {
            tan1 = m_vertexlist[patch.m_vertexid[2]];
            tan1 -= vertex;
            normal = tan2^tan1;
         }

         normal.Normalize();

         key.Set(normal, patch.m_vertexid[3]);
         if (!normaltable.Lookup(key, normalid)) {
            normalid = m_normallist.Add(normal);
            normaltable.SetAt(key, normalid);
         }
         patch.m_normalid[3] = normalid;
      }
   }
   m_normallist.FreeExtra();
   m_transformednormallist.SetSize(m_normallist.GetSize());
}

void PatchModel::Init(const CString &filename) 
{
   BuildNormals();
   Model::Init(filename);
}

void PatchModel::RebuildModelMapBinds()
{
   for (int patchindex = 0; patchindex < m_patchlist.GetSize(); patchindex++) {
      Patch &patch = m_patchlist.ElementAt(patchindex);
      int mapnodecount = patch.m_mapnodearray.GetSize();
      for (int mapnodeindex = 0; mapnodeindex < mapnodecount; mapnodeindex++) {
         MapNode *mapnode = patch.m_mapnodearray[mapnodeindex];
         if (mapnode->m_texturedisplaylist) {
            glBindTexture(GL_TEXTURE_2D, mapnode->m_texturedisplaylist);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR:GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST);
         }
      }
   }
}

BOOL PatchModel::BuildModelMaps(int index, Map &map)
{
   BitmapNode bn;
   if (bn.Load((LPCTSTR)map.m_filename, BitmapNode::LOADUNCOMPRESSED) != BitmapNode::LOAD_OK)
      return FALSE;
   
   for (int patchindex = 0; patchindex < m_patchlist.GetSize(); patchindex++) {
      Patch &patch = m_patchlist.ElementAt(patchindex);
      int mapnodecount = patch.m_mapnodearray.GetSize();
      for (int mapnodeindex = 0; mapnodeindex < mapnodecount; mapnodeindex++) {
         MapNode *mapnode = patch.m_mapnodearray[mapnodeindex];
         if (mapnode->m_mapid == index) {
            CRect rect;
            rect.left   = (int)(mapnode->m_uv[0].x * (map.m_width - 1) + .5);
            rect.top    = (int)(mapnode->m_uv[0].y * (map.m_height - 1) + .5);
            rect.right  = (int)(mapnode->m_uv[2].x * (map.m_width - 1) + .5);
            rect.bottom = (int)(mapnode->m_uv[2].y * (map.m_height - 1) + .5);

            rect.right  = max(rect.left, rect.right);
            rect.bottom = max(rect.bottom, rect.top);

            mapnode->m_uv[0].x = 0.0f;
            mapnode->m_uv[0].y = 0.0f;
            mapnode->m_uv[1].x = 1.0f;
            mapnode->m_uv[1].y = 0.0f; 
            mapnode->m_uv[2].x = 1.0f;
            mapnode->m_uv[2].y = 1.0f;
            mapnode->m_uv[3].x = 0.0f;
            mapnode->m_uv[3].y = 1.0f;

            BitmapNode *newbn = bn.CopyBitmap(rect);
            if (newbn == NULL)
               return FALSE;

            int dstwidth, dstheight;
            int exp;
            exp = (int)(log10((double)newbn->m_width-MAPBORDER*2)/log10(2.0));
            exp = max(1, min(8, exp));
            dstwidth = (int)(pow(2.0f, exp)+MAPBORDER*2);

            exp = (int)(log10((double)newbn->m_height-MAPBORDER*2)/log10(2.0));
            exp = max(1, min(8, exp));
            dstheight = (int)(pow(2.0f, exp)+MAPBORDER*2);

            if (dstwidth != newbn->m_width || dstheight != newbn->m_height) {
               if (!newbn->ScaleBitmap(dstwidth, dstheight))
                  return FALSE;
            }
            UBYTE *rgb;
            int bytesperpixel;

            rgb = newbn->CreateRGB(&bytesperpixel);
            if (rgb==NULL)
               return FALSE;

            glGenTextures(1, &mapnode->m_texturedisplaylist);
            glBindTexture(GL_TEXTURE_2D, mapnode->m_texturedisplaylist);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR:GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST);
//         	glTexImage2D(GL_TEXTURE_2D, 0, bytesperpixel==3?GL_RGB:GL_RGBA, newbn->m_width, newbn->m_height, 0, bytesperpixel==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, rgb);
   	      gluBuild2DMipmaps(GL_TEXTURE_2D, bytesperpixel==3?GL_RGB:GL_RGBA, newbn->m_width, newbn->m_height, bytesperpixel==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, rgb);

            delete newbn;    
            BitmapNode::Free(rgb);// must do this because rgb might be allocated by Non debug Version of MFC
         }
      }
   }
   return TRUE;
}

