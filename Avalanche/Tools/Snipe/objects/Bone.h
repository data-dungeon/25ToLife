#pragma once

#include "TransformableObject.h"
#include "ContainerObject.h"

class Vertex;
class PolyModel;

class Bone : public TransformableObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Bone, TransformableObject)
private:
   static GLuint m_glCallList;

   CMap<UINT, UINT, Vertex *, Vertex *> m_vertexmap;
   Matrix44 m_prevobjecttoworld;

public:
            Bone();
   virtual ~Bone();

   static int GetNumPickElements() { return 2; }

   Bone *GetRootCache() { return (Bone *)TransformableObject::GetRootCache(); } // Note: returns this if it is the cache
   Bone *GetCache() { return (Bone *)TransformableObject::GetCache(); } // Note: returns this if it is the cache

   UINT GetNumVertices() { return m_vertexmap.GetCount(); }
   POSITION GetVertexHeadPosition() { return m_vertexmap.GetStartPosition(); }
   Vertex *GetNextVertex( POSITION &pos )
   {
      UINT nID;
      Vertex *pVertex;
      m_vertexmap.GetNextAssoc( pos, nID, pVertex );
      return pVertex;
   }
   void AddVertex( Vertex *pVertex, BOOL bAddToVertexBoneList=TRUE );
   BOOL RemoveVertex( Vertex *pVertex, BOOL bRemoveFromVertexBoneList=TRUE );
   Vertex *FindVertex( UINT nID ) 
   { 
      Vertex *pVertex = NULL; 
    
      m_vertexmap.Lookup( nID, pVertex );
      return pVertex;
   }
   Vector GetPos();
   void RemoveAllVertices() { m_vertexmap.RemoveAll(); }

   PolyModel *FindModel();

   void Draw( CGLBaseView *pBaseView, RGBAFloat &color, BOOL bDrawBone=TRUE, BOOL bDrawEnd=TRUE );

// TransformableObject overrides
   virtual void PositionVertices( PolyModel *pModel, BOOL bTessellated );
   virtual BOOL IsBone() { return TRUE;}

// InstanceObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_BONE; };

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
   virtual void Draw( CGLBaseView *pView );
   virtual void DrawSelected( CGLBaseView *pView );
   virtual void DrawMirrored( CGLBaseView *pView );
   virtual void DrawFocused( CGLBaseView *pView );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
};

class BoneContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(BoneContainer, ContainerObject)

            BoneContainer();
   virtual ~BoneContainer();

   UINT GetNumTransformableObjects() { return (UINT)GetNumChildren(); }
   TransformableObject *GetTransformableObjectAt( UINT nIndex ) { return (TransformableObject *)GetChildAt( (int)nIndex ); }

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView );
};