#pragma once

#include "..\Utility\DynamicMatrix.h"
#include "..\Utility\MathTools.h"
#include "Vertex.h"
#include "Edge.h"
#include "UV.h"
#include "VertexNormal.h"
#include "PatchVertex.h"

class UVMatrixSet
{
protected:
   DynamicMatrix m_uvmat,m_uvfmat;

public:
   UVMatrixSet();
   ~UVMatrixSet();
   UVMatrixSet(const UVMatrixSet &other);
   UVMatrixSet	&operator = ( const UVMatrixSet &other );

   DynamicMatrix *GetUVMatrix();
   DynamicMatrix *GetUVFlagMatrix();
};

class UVCVsSet
{
protected:
   Vector	m_uvcvs[16];

public:
   UVCVsSet();
   ~UVCVsSet();
   UVCVsSet( const UVCVsSet &other );
   UVCVsSet	&operator = ( const UVCVsSet &other );

   void	   SetUVCVs(Vector *pUVCVs);
   Vector	*GetUVCVs();
};

class TessPatch
{
public:
   Vector	m_cvs[16];
   Vector	m_vncvs[16];
   CArray<UVCVsSet> m_uvsets;

public:
   TessPatch();
   ~TessPatch();

   TessPatch(const TessPatch &other);
   TessPatch	&operator = ( const TessPatch &other );

   void SetCVs(Vector *pCVs);
   Vector *GetCVs() { return m_cvs; }

   void SetVNCVs(Vector *pCVs);
   Vector *GetVNCVs() { return m_vncvs; }

   void SetNumUVSets(UINT nSize);
   UINT GetNumUVSets() { return m_uvsets.GetSize(); }
   void SetUVCVs(Vector *pUVCVs,const UINT nUVSetIndex = 0);
   Vector *GetUVCVs(const UINT nUVSetIndex = 0);

   void	SplitPatchHorizontally(double s,TessPatch *patchA,TessPatch *patchB);
   void	SplitPatchVertically(double t,TessPatch *patchA,TessPatch *patchB);

   static void SplitCVsHorizontally(double s,Vector *pCVs,Vector *ApCVs,Vector *BpCVs);
   static void SplitCVsVertically(double t,Vector *pCVs,Vector *ApCVs,Vector *BpCVs);

   static void SplitCVsHorizontally(double s,Vector *pCVs,Vector *pVNCVs,Vector *ApCVs,Vector *BpCVs,Vector *ApVNCVs,Vector *BpVNCVs);
   static void SplitCVsVertically(double t,Vector *pCVs,Vector *pVNCVs,Vector *ApCVs,Vector *BpCVs,Vector *ApVNCVs,Vector *BpVNCVs);
};

class QuadPatch
{
protected:
   TessPatch            m_tesspatch;
   DynamicMatrix			m_vmat,m_vnmat,m_fmat;
   CArray<UVMatrixSet>	m_uvmatrixsets;

public:
   QuadPatch();
   ~QuadPatch();
   QuadPatch( const QuadPatch &other );
   QuadPatch	&operator = ( const QuadPatch &other );

   void     Clear();

   void     SetCVs(Vector *pCVs);
   Vector   *GetCVs();
   Vector   *GetVNCVs();

   void     SetUVCVSetsSize(UINT nSize); 
   int      GetNumUVCVSets();
   void     SetUVCVs(Vector *pUVCVs,UINT nUVSetIndex = 0);
   Vector	*GetUVCVs(UINT nUVSetIndex = 0);

   void     SetUVMatrixSetsSize(UINT nSize);
   int      GetNumUVMatrixSets();

   DynamicMatrix *GetVertexMatrix();
   DynamicMatrix *GetVertexNormalMatrix();
   DynamicMatrix *GetFlagMatrix();
   DynamicMatrix	*GetUVMatrix(UINT nUVSetIndex = 0);
   DynamicMatrix	*GetUVFlagMatrix(UINT nUVSetIndex = 0);

   TessPatch &GetTessPatch();
};

class UV;

class TessEdgeUVSet
{
protected:
   CArray<UINT> m_uvs;

public:

   TessEdgeUVSet();
   ~TessEdgeUVSet();
   TessEdgeUVSet(const TessEdgeUVSet &other);
   TessEdgeUVSet	&operator = ( const TessEdgeUVSet &other );
   UINT GetNumUVs();
   void SetNumUVs(const UINT nSize);
   void SetUVAt(const UINT nIndex,UV *pUV);
   void SetUVAtGrow(const UINT nIndex,UV *pUV);
   UV *GetUVAt(const UINT nIndex);
   CArray<UINT> *GetUVs();
};

class Vertex;
class VertexNormal;
class PatchEdge;
class PatchVertex;

class TessEdge
{
protected:

   union {
      ULONG m_nTessEdgeFlags;
      struct {
         ULONG m_bIsTemporary    : 1,
               m_bDeleteVertices : 1;
      };
   };

   CArray<UINT> m_vertices;
   CArray<UINT> m_vertexnormals;
   CArray<TessEdgeUVSet> m_uvsets;
   PatchEdge *m_pEdge;

public:
   TessEdge();
   ~TessEdge();
   TessEdge(const TessEdge &other);

   TessEdge	&operator = ( const TessEdge &other );

   void Clear();

   BOOL IsDirtyTessellation() const;

   UINT GetNumVertices() const;
   void SetNumVertices(const UINT nSize);
   void SetVertexAt(const UINT nIndex,PatchVertex *pVertex);
   void SetVertexAtGrow(const UINT nIndex, PatchVertex *pVertex);
   PatchVertex *GetVertexAt(const UINT nIndex) const;
 
   UINT GetNumVertexNormals();
   void SetNumVertexNormals(const UINT nSize);
   void SetVertexNormalAt(const UINT nIndex,VertexNormal *pVertexNormal);
   void SetVertexNormalAtGrow(const UINT nIndex, VertexNormal *pVertexNorml);
   VertexNormal *GetVertexNormalAt(const UINT nIndex) const;
 
   void SetNumUVSets(const UINT nSize);
   UINT GetNumUVSets();

   UINT GetNumUVs(const UINT nUVSetIndex=0);
   void SetNumUVs(const UINT nSize,UINT nUVSetIndex=0);

   void SetUVAt(const UINT nIndex,UV *pUV, const UINT nUVSetIndex = 0);
   void SetUVAtGrow(const UINT nIndex, UV *pUV, const UINT nUVSetIndex = 0);
   UV *GetUVAt(const UINT nIndex, const UINT nUVSetIndex = 0);

   void GetHalves(TessEdge &edge1,TessEdge &edge2);

   CArray<UINT> *GetVertices();
   CArray<UINT> *GetVertexNormals();
   CArray<UINT> *GetUVs(const UINT nUVSetIndex);

   void SetEdge(PatchEdge *pEdge) { m_pEdge = pEdge; }
   PatchEdge *GetEdge() { return m_pEdge; }

   void Reverse();
   void Reverse(TessEdge &tessedge);

   BOOL FindClosestIntersection( const Vector &point, Vector &intersection, float &param );

   BOOL IsTemporary() { return m_bIsTemporary; }
   void SetTemporaryFlag(BOOL bFlag) { m_bIsTemporary = bFlag; }

   BOOL IsDeleteVertices() { return m_bDeleteVertices; }
   void SetDeleteVertices(BOOL bDeleteVertices) { m_bDeleteVertices = bDeleteVertices; }
};


#include "PatchExtras.hpp"