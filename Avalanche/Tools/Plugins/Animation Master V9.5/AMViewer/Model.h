// Dan  4/11/00  \Dan85\AMViewer\Model.h

#define __MODEL_H

#include "AM-GL.h"
#include "Afxtempl.h"

#ifndef __VECTOR_H
#include "Vector.h"
#endif
#ifndef __VECTOR2_H
#include "Vector2.h"
#endif
#ifndef __MATRIX34_H
#include "Matrix34.h"
#endif
#ifndef __OLDATTR_H
#include "OldAttr.h"
#endif

#define MAPBORDER 0

class CAMViewerView;

class NormalKey 
{
public:
   Vector m_normal;
   LONG   m_vertexid;

   BOOL operator == ( const NormalKey &other ) const
   {
      return (m_normal == other.m_normal && m_vertexid == other.m_vertexid);
   }

   void Set(const Vector &normal, LONG vertexid) {
      m_normal = normal;
      m_vertexid = vertexid;
   }
};

inline UINT AFXAPI HashKey(NormalKey &key)
{
	return ((UINT)(void*)(DWORD)key.m_vertexid) >> 4; // this will hash to only common vertex collections
}

class MapNode 
{
public:
   enum { COLORMAP, TRANSPARENCYMAP, BUMPMAP, SPECULARMAP, DIFFUSEMAP,
          MIRRORMAP, AMBIANCEMAP, COOKIECUTMAP, DISPLACEMENTMAP, FRACTALMAP };

   LONG    m_mapid;
   float   m_percent;
   Vector2 m_repeat;
   Vector2 m_uv[4];
   GLuint m_texturedisplaylist;//used only by model maps

   union { 
      DWORD  m_flags;
      struct {
         DWORD m_kind       : 4,
               m_isseamless : 1;
      };
   };

   MapNode() {
      m_repeat.Set(1.0f,1.0f);
      m_percent = 1.0f;
      m_flags = 0L;
      m_kind = COLORMAP;
      m_texturedisplaylist = 0;
   }

   virtual ~MapNode() {
      if (m_texturedisplaylist)
         glDeleteTextures(1, &m_texturedisplaylist);
   }
};


class PatchMapNode : public MapNode 
{
public:
   Vector2 m_controluv[8]; // 8 corner tangent control vertices
};

class Map
{
public:
   CString m_filename;
   WORD    m_width, m_height;
   GLuint  m_textureclamp, m_texturerepeat; // non model map display list

   union {
      unsigned long m_flags;
      struct {
         unsigned long m_buildclamp  : 1,
                       m_buildrepeat : 1;
      };
   };

   Map() { 
      m_width = m_height = 0;
      m_textureclamp = m_texturerepeat = 0;
      m_flags = 0;
   }

   virtual ~Map() {
       if (m_textureclamp)
          glDeleteTextures(1, &m_textureclamp);
       if (m_texturerepeat)
          glDeleteTextures(1, &m_texturerepeat);
   }

   BOOL BuildTextureMaps();
};

class Patch
{
public:
   CArray<PatchMapNode *, PatchMapNode *>m_mapnodearray;
   LONG m_attrid;
   LONG m_vertexid[4];
   LONG m_controlvertexid[12];
   LONG m_normalid[4];
   union {
      LONG  m_flags;
      struct {
         LONG m_is3point : 1;
      };
   };

   Patch() {
      m_flags = 0;
   }

   virtual ~Patch() {
      for (int i=0;i < m_mapnodearray.GetSize(); i++) {
         PatchMapNode *mapnode = m_mapnodearray.ElementAt(i);
         delete mapnode;
      }
   }
};

class Poly
{
public:
   CArray<MapNode *, MapNode *>m_mapnodearray;
   LONG m_attrid;
   LONG m_vertexid[4];
   LONG m_normalid[4];
   
   virtual ~Poly() {
      for (int i=0;i < m_mapnodearray.GetSize(); i++) {
         MapNode *mapnode = m_mapnodearray.ElementAt(i);
         delete mapnode;
      }
   }
};

class Bone
{
public:
   Bone *m_child, *m_sibling, *m_parent;
   Matrix34 m_matrix, m_bonetostandard, m_standardtoaction;
   CString m_name;
 
   Vector m_pivot,m_end,m_roll;

   CArray<LONG, LONG> m_vertexidarray;
   CArray<LONG, LONG> m_controlvertexidarray;
   CArray<LONG, LONG> m_normalidarray;
   
   Bone() {
      m_child = m_sibling = NULL;
      m_controlvertexidarray.SetSize(0, 200);
      m_normalidarray.SetSize(0, 200);
      m_matrix = NULL;
   }

   void ComputeBoneToStandard(const Vector &pivotpos, const Vector &endpos, const Vector &rollpos);
   void ComputeStandardToAction(Matrix33 &bonetoparent);
   Matrix33 ComputeParentToBone();
   Matrix34 CreateMatrix( const TSQ &tsq );
};

class Model 
{
public:
   CArray<Vector, Vector &> m_vertexlist;
   CArray<Vector, Vector &> m_transformedvertexlist;
   CArray<Vector, Vector &> m_normallist;
   CArray<Vector, Vector &> m_transformednormallist;
   CArray<Attr, Attr &> m_attrlist;
   CArray<Map, Map &> m_maplist;
   CString m_name;
   GLuint m_lasttexture;
   GLuint m_notexture;
   int m_lastattrdiffuse;
   int m_lastattr;
   float m_span;
   Bone *m_bonehead;

   Model();
   virtual ~Model();

   void DeleteBones(Bone *startbone);
   void LoadBones(CArchive &ar, Bone **parentuplink, Bone *parent);
   void BuildTextures();
   void RebuildBinds();
   void FillVertexBoneList(Bone *bone, CArray<Bone *, Bone *> &vertexbonelist);
   
   virtual void SetSpan();
   virtual void Init(const CString &filename);
   virtual BOOL BuildModelMaps(int index, Map &map) { return TRUE; }
   virtual void RebuildModelMapBinds() {}
   virtual void TransformModel(CAMViewerView *view){}
   virtual void FillListIndexesOnBones() {}
   virtual void Render(CAMViewerView *view) {}
   virtual void Serialize(CArchive &ar) = 0;
   virtual int  GetPatchPolyCount()=0;

   void RenderBones(CAMViewerView *view);
};

class PatchModel : public Model
{
public:
   CArray<Vector, Vector &> m_controlvertexlist;
   CArray<Vector, Vector &> m_transformedcontrolvertexlist;
   CArray<Patch, Patch &> m_patchlist;

   void BuildNormals();
   void TransformBoneVertices(CAMViewerView *view, Bone *bone, const Matrix34 &parentscreenmatrix);

   virtual void TransformModel(CAMViewerView *view);
   virtual BOOL BuildModelMaps(int index, Map &map);
   virtual void RebuildModelMapBinds();
   virtual int GetPatchPolyCount() { return m_patchlist.GetSize(); }
   virtual void SetSpan();
   virtual void FillListIndexesOnBones();
   virtual void Init(const CString &filename);
   virtual void Render(CAMViewerView *view);
};

class PolyModel : public Model
{
public:
   CArray<Poly, Poly &> m_polylist;

   void RenderPolys(CAMViewerView *view);
   void RenderVertices();
   void RenderNormals(CAMViewerView *view);
   void TransformBoneVertices(CAMViewerView *view, Bone *bone, const Matrix34 &parentscreenmatrix);

   virtual void TransformModel(CAMViewerView *view);
   virtual BOOL BuildModelMaps(int index, Map &map);
   virtual void RebuildModelMapBinds();
   virtual int GetPatchPolyCount() { return m_polylist.GetSize(); }
   virtual void FillListIndexesOnBones();
   virtual void Render(CAMViewerView *view);
};
