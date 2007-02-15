// Dan  4/11/00  \Dan85\AMViewer\Ply.cpp

#include "stdafx.h"
#include "StrClass.h"
#include "Ply.h"
#include "Archive.h"
#include "F_Protos.h"

void PLYModel::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	   return;

   int tint;
   int count;
   BYTE tbyte;
   char name[3];

   CString oldbasepath = GetBasePath().Get();
   SetBasePath((LPCTSTR)ar.GetFile()->GetFilePath());

   ar.Read(name, 3);

   if (!(name[0]=='P' && name[1]=='L' && name[2] == 'Y')) {
      CString text;
      text.Format("Invalid poly file %s", ar.GetFile()->GetFileTitle());
      AfxMessageBox(text, MB_ICONEXCLAMATION);
      AfxThrowUserException();
   }

   ar >> tint; //Version
   if (tint != 2) {
      AfxMessageBox("Invalid poly file version", MB_ICONEXCLAMATION);
      AfxThrowUserException();
   }

// Load Material Attributes List
   ar >> count; //Num Attr
   m_attrlist.SetSize(count);
   for (int i =0; i < count; i++) {
      Attr &attr = m_attrlist.ElementAt(i);
      BYTE red, green, blue;
      ar >> red >> green >> blue;
      attr.m_diffusecolor.red = red/255.0f;
      attr.m_diffusecolor.green = green/255.0f;
      attr.m_diffusecolor.blue = blue/255.0f;
      ar >> attr.m_diffusefalloff;
      ar >> red >> green >> blue;//spec color
      attr.m_specularcolor.red = red/255.0f;
      attr.m_specularcolor.green = green/255.0f;
      attr.m_specularcolor.blue = blue/255.0f;
      ar >> attr.m_specularsize;
      ar >> attr.m_specularintensity;
      ar >> attr.m_ambiance;
      ar >> attr.m_roughness;
      ar >> attr.m_roughnessscale;
      ar >> attr.m_reflectivity;
      ar >> attr.m_transparency;
      ar >> attr.m_refraction;
   }

// Load Texture Maps List
   ar >> count; //Num Textures
   m_maplist.SetSize(count);
   for (i =0; i < count; i++) {
      Map &map = m_maplist.ElementAt(i);
      map.m_filename = ReadString(ar);
      String name = (LPCTSTR)map.m_filename;
      map.m_filename = name.GetAbsolutePath().Get();
      ar >> map.m_width;
      ar >> map.m_height;
   }

// Load Vertex List
   ar >> count;
   m_vertexlist.SetSize(count);
   m_transformedvertexlist.SetSize(count);
   for (i =0; i < count; i++)
      ar >> m_vertexlist[i];

// Load Normals Vertex List
   ar >> count;
   m_normallist.SetSize(count);
   m_transformednormallist.SetSize(count);
   for (i =0; i < count; i++)
      ar >>  m_normallist[i];

// Load Poly List
   ar >> count;
   m_polylist.SetSize(count);
   int pc=0, tpc=count-1;
   for (i =0; i < count; i++) {
      LONG attrid;
      ar >> attrid;

      Poly *poly;
      if (m_attrlist.GetSize() && m_attrlist[attrid].m_transparency!=0)
         poly = &m_polylist[tpc--];
      else
         poly = &m_polylist[pc++];

      poly->m_attrid = attrid;
      ar >> poly->m_vertexid[0] >> poly->m_vertexid[1] >> poly->m_vertexid[2] >> poly->m_vertexid[3];
      for (int n = 0; n < 4; n++ )
         ar >> poly->m_normalid[n];

      // Load Poly Texture Maps
      int mapcount;
      ar >> mapcount;
      poly->m_mapnodearray.SetSize(mapcount);
      for (int mc=0; mc < mapcount; mc++) {
         MapNode *mapnode = new MapNode;
         poly->m_mapnodearray.SetAt(mc, mapnode);
         ar >> mapnode->m_mapid;
         ar >> mapnode->m_percent;
         ar >> mapnode->m_repeat;
         ar >> tbyte; // kind
         mapnode->m_kind = tbyte;
         ar >> tbyte; // seamless;
         mapnode->m_isseamless = tbyte;

         if (mapnode->m_mapid != -1) {
            if (mapnode->m_repeat.x == 1 && mapnode->m_repeat.y == 1)
               m_maplist[mapnode->m_mapid].m_buildclamp = TRUE;
            else   
               m_maplist[mapnode->m_mapid].m_buildrepeat = TRUE;
         }

         for (int uvcount = 0; uvcount < 4; uvcount++) {
            ar >> mapnode->m_uv[uvcount]; // UV
            mapnode->m_uv[uvcount].x *= mapnode->m_repeat.x;
            mapnode->m_uv[uvcount].y *= mapnode->m_repeat.y;
         }
      }
   }

//Load Bones
   LoadBones(ar, &m_bonehead, NULL);
   SetBasePath((LPCTSTR)oldbasepath);
}
