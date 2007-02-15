
#include "Stdafx.h"
#include "HashPlugin.h"
#include "HPolyModel.h"
#include "HPoly.h"
#include "FileStream.h"
#include "Ply.h"
#include "Vector2.h"

PlyTranslator::PlyTranslator() 
{
}

void PlyTranslator::OnImport(HPolyModel *pModel, const char *strFileName)
{
   FileStream fs;

   fs.Open( strFileName, BaseStream::BS_BINARY | BaseStream::BS_INPUT);
   
   char name[3];
   fs.Read(name, 3);

   if (!(name[0]=='P' && name[1]=='L' && name[2] == 'Y'))
      THROW_FATAL_ERROR("Unknow file format.");

   int nVersion;
   fs >> nVersion;
   if (nVersion != 2)
      THROW_FATAL_ERROR((String)"Unknown format version: " + nVersion + ".");
   
   float value;
   BYTE red, green, blue;

   int nCount;

// Load Attributes List
   fs >> nCount;

   for (int i=0; i<nCount; i++)
   {
      fs >> red >> green >> blue; // diffuse
      fs >> value;                // attr.m_diffusefalloff;
      fs >> red >> green >> blue; // specular color
      fs >> value;                // attr.m_specularsize;
      fs >> value;                // attr.m_specularintensity;
      fs >> value;                // attr.m_ambiance;
      fs >> value;                // attr.m_roughness;
      fs >> value;                // attr.m_roughnessscale;
      fs >> value;                // attr.m_reflectivity;
      fs >> value;                // attr.m_transparency;
      fs >> value;                // attr.m_refraction;
   }

// Load Texture Maps List
   fs >> nCount;
   WORD width, height;
   for (int i=0; i<nCount; i++)
   {
      fs.ParseToken('\0');
      String mapname;
      fs.GetToken(mapname);//relative path
      fs >> width;
      fs >> height;
   }

// Load Vertex List
   fs >> nCount;
   CArray<HVertex *, HVertex *> vertexarray;
   vertexarray.SetSize(nCount);

   Vector pos;
   for (int i=0; i<nCount; i++)
   {
      fs >> pos;
      vertexarray.SetAt(i, pModel->AddVertex(pos));
   }

// Load Normals List
   fs >> nCount;
   CArray<HVertexNormal *, HVertexNormal *> normalarray;
   normalarray.SetSize(nCount);

   Vector dir;
   for (int i=0; i < nCount; i++)
   {
      fs >> dir;
      normalarray.SetAt(i, pModel->AddVertexNormal(/*dir*/));
   }

// Load Poly List
   fs >> nCount;

   LONG nAttrID;
   LONG nVertexID[4];
   LONG nNormalID[4];

   int nMapCount;
   LONG nMapID;
   float fPercent;
   Vector2 repeat;
   Vector2 uv[4]; 
   BYTE kind;
   BYTE seamless;

   for (int i=0; i<nCount; i++)
   {
      HPoly *poly = pModel->AddPoly();
      fs >> nAttrID;
      fs >> nVertexID[0] >> nVertexID[1] >> nVertexID[2] >> nVertexID[3];
      fs >> nNormalID[0] >> nNormalID[1] >> nNormalID[2] >> nNormalID[3];

      UINT nNumSides = 4;
      if (nVertexID[3] == nVertexID[0])
         nNumSides = 3;
      poly->SetNumSides( nNumSides );
      for (UINT v=0; v<(UINT)nNumSides; v++)
      {
         poly->SetVertexAt( v, vertexarray[nVertexID[v]] );
         poly->SetVertexNormalAt( v, normalarray[nNormalID[v]] );
      }

      // Load Poly Texture Maps
      fs >> nMapCount;
      for (int mc=0; mc<nMapCount; mc++)
      {
         fs >> nMapID;
         fs >> fPercent;
         fs >> repeat;
         fs >> kind;
         fs >> seamless;

         for (int nUVCount=0; nUVCount<4; nUVCount++)
            fs >> uv[nUVCount];
      }
   }

   pModel->BuildPolys();
}
