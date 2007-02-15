#include "Stdafx.h"
#include "DBL.h"
#include "HPolyModel.h"
#include "Vector.h"
#include "Vector2.h"
#include "HPoly.h"
#include "HUVPoly.h"
#include "HVertexNormal.h"
#include "HUV.h"

#include "Tup.h"

#include "ExportDBLOptionsDlg.h"

DBLModelTranslator::DBLModelTranslator()
{
}

void DBLModelTranslator::OnExport(HPolyModel *pModel, const char *strFileName)
{
   ExportDBLOptionsDlg dlg;
   if (dlg.DoModal() != IDOK)
      return;

   TupModelTranslator *pTupModelTranslator = SNEW TupModelTranslator();
   pTupModelTranslator->InitTupSceneForExport(strFileName);
   pTupModelTranslator->RecurseBranch((HHierObject*)pModel,pTupModelTranslator->m_pWorldNodeAgg);

   // load dll into memory
   HINSTANCE hInstance = LoadLibraryEx(dlg.m_strFilterChain, NULL, 0);
   if (hInstance)
   {
      bool (*vProcess)(TupperwareAggregate *, int, char**);

      vProcess = (bool (*)(TupperwareAggregate *, int, char**))(const char *(*)())GetProcAddress(hInstance, "LibProcess");

      if (vProcess)
      {
         char *params[3];
         params[0] = (char *)strFileName;
         params[1] = (char *)(LPCTSTR)dlg.m_strProcessFilename;
         params[2] = (char *)strFileName;
         bool result = vProcess(pTupModelTranslator->m_pSceneAgg,3,params);

         FreeLibrary(hInstance);
      }
   }

   pTupModelTranslator->DestroyTupScene();

   delete pTupModelTranslator;

}

/* M export for NPatchViewer

class VertexInfo
{
public:
   Vector vertex;
   Vector normal;
   Vector2 uv;
};

void MTranslator::OnExport(HPolyModel *pModel, const char *strFileName)
{
   ExportTupSceneOptionsDlg dlg;
   if (dlg.DoModal() != IDOK)
      return;


   FILE *fp;
   fp = fopen(strFileName, "w");
   if (fp == NULL)
   {
      printf("Cannot open .m file for writing!\n");
      return;
   }

   CArray<VertexInfo> vertexinfos;
   vertexinfos.SetSize(pModel->GetNumVertexNormals());
   CMap<HVertexNormal*,HVertexNormal*,UINT,UINT> vnmap;

   CArray<UINT> indices;

   UINT nCount = 0;
   UINT nIndex;
   HUV *pUV;
   for (POSITION pos = pModel->GetHeadFacePosition();pos;)
   {
      HPoly *pPoly = pModel->GetNextFace(pos);
      int nNumVerts = (int)pPoly->GetNumSides();
      for (int i = 0; i < nNumVerts; i++)
      {
         HVertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(i);
         if (!vnmap.Lookup(pVertexNormal,nIndex))
         {
            HVertex *pVertex = pPoly->GetVertexAt(i);
            vertexinfos.GetAt(nCount).vertex = pVertex->GetPos();
            vertexinfos.GetAt(nCount).normal = pVertexNormal->GetDir();
            HUVPoly *pUVPoly = pPoly->GetHeadUVPoly();
            if (pUVPoly)
            {
               pUV = pUVPoly->GetUVAt(i);
               vertexinfos.GetAt(nCount).uv = pUV->GetPos();
            }
            vnmap.SetAt(pVertexNormal,nCount++);
         }
      }
      UINT nFirstIndex,nIndex;
      HVertexNormal *pFirstVertexNormal = pPoly->GetVertexNormalAt(0);
      if (!vnmap.Lookup(pFirstVertexNormal,nFirstIndex))
      {
         ASSERT(FALSE);
      }

      for (int i = 0; i < nNumVerts-1; i++)
      {
         indices.Add(nFirstIndex);
         HVertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(i);
         if (!vnmap.Lookup(pVertexNormal,nIndex))
         {
            ASSERT(FALSE);
         }
         indices.Add(nIndex);
         pVertexNormal = pPoly->GetVertexNormalAt(i+1);
         if (!vnmap.Lookup(pVertexNormal,nIndex))
         {
            ASSERT(FALSE);
         }
         indices.Add(nIndex);
      }
   }

   for (int i = 0; i < vertexinfos.GetSize(); i++)
   {
      fprintf(fp,"V %f %f %f %f %f %f %f %f\n",
         vertexinfos[i].vertex.x, vertexinfos[i].vertex.y, vertexinfos[i].vertex.z,
         vertexinfos[i].normal.x, vertexinfos[i].normal.y, vertexinfos[i].normal.z,
         vertexinfos[i].uv.x, vertexinfos[i].uv.y);
   }

   for (i = 0; i < indices.GetSize(); i++)
   {
      fprintf(fp,"I %d\n",indices[i]);
   }

   fclose(fp);
}
*/