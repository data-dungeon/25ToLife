// Dan  4/11/00  \Dan85\AMViewer\PolyModel.cpp

#include "stdafx.h"
#include "Model.h"
#include "BitmapNo.h"
#include "MainFrm.h"
#include "AMViewerView.h"

extern CMainFrame *g_mainframe;

static void ShrinkBoneNLists(Bone *bone)
{
   for (;bone;bone=bone->m_sibling) {
      bone->m_normalidarray.FreeExtra();
      if (bone->m_child)
         ShrinkBoneNLists(bone->m_child);
   }
}

void PolyModel::FillListIndexesOnBones()
{
   if (m_bonehead==NULL)
      return;
   CArray<Bone *, Bone *> vertexbonelist;// contains bone * for each vertex;
   vertexbonelist.SetSize(m_vertexlist.GetSize());
   FillVertexBoneList(m_bonehead, vertexbonelist);

   CMap<LONG, LONG &, Bone *, Bone *> ntobonemap;
   ntobonemap.InitHashTable(997);

// Add normalid's to CMap to remove duplicates
   int count = m_polylist.GetSize();
   for (int pi=0; pi<count; pi++) {
      Poly &poly = m_polylist[pi];
      Bone *bone = vertexbonelist[poly.m_vertexid[0]];

      if (bone)
         ntobonemap.SetAt(poly.m_normalid[0], bone);

      bone = vertexbonelist[poly.m_vertexid[1]];
      if (bone)
         ntobonemap.SetAt(poly.m_normalid[1], bone);

      bone = vertexbonelist[poly.m_vertexid[2]];
      if (bone)
         ntobonemap.SetAt(poly.m_normalid[2], bone);

      bone = vertexbonelist[poly.m_vertexid[3]];
      if (bone)
         ntobonemap.SetAt(poly.m_normalid[3], bone);
   }

   for (POSITION pos = ntobonemap.GetStartPosition(); pos;) {
      Bone *bone;
      LONG normalindex;
      
      ntobonemap.GetNextAssoc(pos, normalindex, bone);
      bone->m_normalidarray.Add(normalindex);
   }
   ntobonemap.RemoveAll();

   ShrinkBoneNLists(m_bonehead);
}

void PolyModel::RebuildModelMapBinds()
{
   for (int polyindex = 0; polyindex < m_polylist.GetSize(); polyindex++) {
      Poly &poly = m_polylist.ElementAt(polyindex);
      int mapnodecount = poly.m_mapnodearray.GetSize();
      for (int mapnodeindex = 0; mapnodeindex < mapnodecount; mapnodeindex++) {
         MapNode *mapnode = poly.m_mapnodearray[mapnodeindex];
         if (mapnode->m_texturedisplaylist) {
            glBindTexture(GL_TEXTURE_2D, mapnode->m_texturedisplaylist);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR:GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST);
         }
      }
   }
}

BOOL PolyModel::BuildModelMaps(int index, Map &map)
{
   BitmapNode bn;
   if (bn.Load((LPCTSTR)map.m_filename, BitmapNode::LOADUNCOMPRESSED) != BitmapNode::LOAD_OK)
      return FALSE;
   
   for (int polyindex = 0; polyindex < m_polylist.GetSize(); polyindex++) {
      Poly &poly = m_polylist.ElementAt(polyindex);
      int mapnodecount = poly.m_mapnodearray.GetSize();
      for (int mapnodeindex = 0; mapnodeindex < mapnodecount; mapnodeindex++) {
         MapNode *mapnode = poly.m_mapnodearray[mapnodeindex];
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
            glTexImage2D(GL_TEXTURE_2D, 0, bytesperpixel==3?GL_RGB:GL_RGBA, newbn->m_width, newbn->m_height, MAPBORDER, bytesperpixel==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, rgb);

            BitmapNode::Delete(newbn);    
            BitmapNode::Free(rgb);// must do this because rgb where allocated by Non debug Version of MFC
         }
      }
   }
   return TRUE;
}

