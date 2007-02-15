// Dan  4/11/00  \Dan85\AMViewer\Transform.cpp

#include "stdafx.h"
#include "AMViewerView.h"
#include "Model.h"

void PatchModel::TransformBoneVertices(CAMViewerView *view, Bone *bone, const Matrix34 &parentscreenmatrix)
{
   for (; bone; bone=bone->m_sibling) {
      Matrix34 screenmatrix;
      screenmatrix = parentscreenmatrix * bone->m_matrix;

      int count = bone->m_normalidarray.GetSize();
      Matrix33 m33 = ToMatrix33(screenmatrix);
      for (int i=0; i < count; i++) {
         int id = bone->m_normalidarray[i];
         m_transformednormallist[id] = 
          m33 * m_normallist.ElementAt(id);
      }

      count = bone->m_vertexidarray.GetSize();
      for (i=0; i < count; i++) {
         int id = bone->m_vertexidarray[i];
         m_transformedvertexlist[id] = 
          screenmatrix * m_vertexlist.ElementAt(id);
         if (!view->m_bsubdivided)
            view->Project(m_transformedvertexlist[id]);
      }
   
      if (view->m_btangents || view->m_bsubdivided) {
         count = bone->m_controlvertexidarray.GetSize();
         for (i=0; i < count; i++) {
            int id = bone->m_controlvertexidarray[i];
            m_transformedcontrolvertexlist[id] = 
             screenmatrix * m_controlvertexlist.ElementAt(id);
            if (!view->m_bsubdivided)
               view->Project(m_transformedcontrolvertexlist[id]);
         }
      }

      if (bone->m_child)
         TransformBoneVertices(view, bone->m_child, screenmatrix);
   }
}

void PatchModel::TransformModel(CAMViewerView *view)
{
   view->m_matrix.SetTranslate(Vector(0.0f));
   Vector translate = view->m_matrix * view->m_offset;
   translate.z -= view->m_focaldistance;
   view->m_matrix.SetTranslate(translate);

   if (m_bonehead)
      TransformBoneVertices(view, m_bonehead, view->m_matrix);
   else { // No Bones
      int count = m_normallist.GetSize();
      Matrix33 m33 = ToMatrix33(view->m_matrix);
      for (int i=0; i < count; i++) {
         m_transformednormallist[i] = 
          m33 * m_normallist.ElementAt(i);
      }

      count = m_vertexlist.GetSize();
      for (i=0; i < count; i++) {
         m_transformedvertexlist[i] = 
          view->m_matrix * m_vertexlist.ElementAt(i);
         if (!view->m_bsubdivided)
            view->Project(m_transformedvertexlist[i]);
      }

      if (view->m_btangents || view->m_bsubdivided) {
         count = m_controlvertexlist.GetSize();
         for (i=0; i < count; i++) {
            m_transformedcontrolvertexlist[i] = 
             view->m_matrix * m_controlvertexlist.ElementAt(i);
            if (!view->m_bsubdivided)
               view->Project(m_transformedcontrolvertexlist[i]);
         }
      }
   }
}

//********************** POLYMODEL ********************************************

void PolyModel::TransformBoneVertices(CAMViewerView *view, Bone *bone, const Matrix34 &parentscreenmatrix)
{
   for (; bone; bone=bone->m_sibling) {
      Matrix34 screenmatrix;
      screenmatrix = parentscreenmatrix * bone->m_matrix;

      int count = bone->m_normalidarray.GetSize();
      Matrix33 m33 = ToMatrix33(screenmatrix);
      for (int i=0; i < count; i++) {
         int id = bone->m_normalidarray[i];
         m_transformednormallist[id] = 
          m33 * m_normallist.ElementAt(id);
      }

      count = bone->m_vertexidarray.GetSize();
      for (i=0; i < count; i++) {
         int id = bone->m_vertexidarray[i];
         m_transformedvertexlist[id] = 
          screenmatrix * m_vertexlist.ElementAt(id);
         view->Project(m_transformedvertexlist[id]);
      }
   
      if (bone->m_child)
         TransformBoneVertices(view, bone->m_child, screenmatrix);
   }
}

void PolyModel::TransformModel(CAMViewerView *view)
{
   view->m_matrix.SetTranslate(Vector(0.0f));
   Vector translate = view->m_matrix * view->m_offset;
   translate.z = translate.z - view->m_focaldistance;
   view->m_matrix.SetTranslate(translate);

   if (m_bonehead)
      TransformBoneVertices(view, m_bonehead, view->m_matrix);
   else { // No Bones
      int count = m_normallist.GetSize();
      Matrix33 m33 = ToMatrix33(view->m_matrix);
      for (int i=0; i < count; i++)
         m_transformednormallist[i] = m33 * m_normallist.ElementAt(i);

      count = m_vertexlist.GetSize();
      for (i=0; i < count; i++) {
         m_transformedvertexlist[i] = 
          view->m_matrix * m_vertexlist.ElementAt(i);
         view->Project(m_transformedvertexlist[i]);
      }
   }
}



