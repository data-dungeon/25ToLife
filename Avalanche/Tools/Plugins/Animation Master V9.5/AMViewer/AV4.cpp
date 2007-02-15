// Dan  8/27/99  \Dan80\AMViewer\AV3.cpp

#include "stdafx.h"
#include "StrClass.h"
#include "AV4.h"
#include "Archive.h"
#include "F_Protos.h"
#include "HashFile.h"
#include "..\avalancheexporter\agpshader.h"

void AV4Model::Serialize(CArchive &ar)
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

   if (!(name[0]=='A' && name[1]=='V' && name[2] == '4')) {
      CString text;
      text.Format("Invalid Avatar 4 file %s", ar.GetFile()->GetFileTitle());
      AfxMessageBox(text, MB_ICONEXCLAMATION);
      AfxThrowUserException();
   }

   ar >> tint; //Version
   if (tint != 4) {
      AfxMessageBox("Invalid Avatar 4 file version", MB_ICONEXCLAMATION);
      AfxThrowUserException();
   }

// Load Attributes List
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

// Load Material List
   ar >> count; // Num Materials
   for (i=0; i < count; i++) {
	  CString matname; // we don't current use procedural textures in this viewer, this is just the name anyways
      matname = ReadString(ar);
   }

//	Load Shader List
	ar >> count; // Num Materials
	for (i=0; i < count; i++) {
		AGPShader shader;
		ar.Read(&shader.AGPStruct,sizeof(FullAGPShaderExport));
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

// Load Tangent Control Vertex List
   ar >> count;
   m_controlvertexlist.SetSize(count);
   m_transformedcontrolvertexlist.SetSize(count);
   for (i =0; i < count; i++)
      ar >>  m_controlvertexlist[i];

// Load Patch List
   ar >> count;
   m_patchlist.SetSize(count);
   int pc=0, tpc=count-1;
   for (i =0; i < count; i++) {
      LONG attrid;
      ar >> attrid;

	   int matcount;
	   ar >> matcount;
	   int matid;
	   for (int matc=0; matc < matcount; matc++) // we don't use the Material ID in this viewer
		   ar >> matid;

//	Load Shader Indicies
	int shadercount = 0,shaderid;
	ar >> shadercount;
	for (int j = 0; j < shadercount; j++)
		ar >> shaderid;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      Patch *patch;
      if (m_attrlist.GetSize() && attrid != -1 && m_attrlist[attrid].m_transparency!=0)
         patch = &m_patchlist[tpc--];
      else
         patch = &m_patchlist[pc++];
      ar >> patch->m_flags;   

      patch->m_attrid = attrid;
      ar >> patch->m_vertexid[0] >> patch->m_vertexid[1] >> patch->m_vertexid[2] >> patch->m_vertexid[3];
      for (int t = 0; t < 12; t++ )
         ar >> patch->m_controlvertexid[t];
      // Load Patches Texture Maps
      int mapcount;
      ar >> mapcount;
      patch->m_mapnodearray.SetSize(mapcount);
      for (int mc=0; mc < mapcount; mc++) {
         PatchMapNode *mapnode = new PatchMapNode;
         patch->m_mapnodearray.SetAt(mc, mapnode);
         ar >> mapnode->m_mapid;
         ar >> mapnode->m_percent;
         ar >> mapnode->m_repeat;
         ar >> tbyte; // kind
         mapnode->m_kind = tbyte;
         ar >> tbyte; // seamless;
         mapnode->m_isseamless = tbyte;
         ar >> tbyte; // isgroupdecal;
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

         for (uvcount = 0; uvcount < 8; uvcount++)
            ar >> mapnode->m_controluv[uvcount]; // Control UV
      }
   }

//Load Bones
   LoadBones(ar, &m_bonehead, NULL);
   SetBasePath((LPCTSTR)oldbasepath);
}

