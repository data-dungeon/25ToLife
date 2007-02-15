#include "stdafx.h"
#include "GeometricObject.h"
#include "GLModelView.h"
#include "Properties/UVMapSurfaceDriver.h"
#include "ModelDoc.h"

#define USECALLLIST

void GeometricObject::Draw( CGLBaseView *pBaseView )
{
   if (pBaseView->m_bDoPick) {
      if (DrawPick( pBaseView ))
         return;
   }

   GeometricObject *pCache = GetRootCache();
   if (!RebuildGeometry(pBaseView))
      return;

   TransformableObjectAttribsPusher glattribs( this, pBaseView );

   CGLModelView *pModelView = (CGLModelView*)pBaseView;

   if (IsCache()) // we are editing at the component level
   {
#ifdef USECALLLIST
      if (pModelView->GetOperation()==CGLBaseView::OP_NAVIGATING) {
         if (m_glCallList) {
            glCallList( m_glCallList );
            HierObject::Draw( pBaseView );
            return;
         }
         else {
            m_glCallList = glGenLists( 1 );
            glNewList( m_glCallList, GL_COMPILE_AND_EXECUTE );
         }
      }
      else if (m_glCallList) {
         glDeleteLists( m_glCallList, 1 );
         m_glCallList = 0;
      }
#endif
      glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT| GL_CURRENT_BIT);

      DrawImmediateMode( pModelView );

      glPopAttrib();

#ifdef USECALLLIST
      if (m_glCallList)
         glEndList();
#endif
   }
   else
   {
      glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT| GL_CURRENT_BIT);

      DrawRetainedMode( pModelView );

      glPopAttrib();
   }

   HierObject::Draw( pBaseView );
}

void GeometricObject::DrawImmediateMode( CGLModelView *pModelView )
{
}

void GeometricObject::DrawRetainedMode( CGLModelView *pModelView )
{
   //glEnableClientState( GL_VERTEX_ARRAY );
   glVertexPointer( 3, GL_FLOAT, sizeof(RTVertex), &m_vertexarray[0].GetPos() );

   if (pModelView->IsDisplayShaded() && !GetRootCache()->m_surfacearray.IsEmpty())
      DrawSurfaces();

   if (pModelView->GetCurrentSelectionSOT()==SOT_Vertex)
      DrawControlPoints();
}

void GeometricObject::DrawSurfaces()
{
   m_nNumQuads = 0;
   m_nNumNGons = 0;

   glEnable( GL_LIGHTING );

   //glEnableClientState( GL_NORMAL_ARRAY );
   glNormalPointer( GL_FLOAT, sizeof(RTVertex), &m_vertexarray[0].GetNormal() );

   //glEnableClientState( GL_TEXTURE_COORD_ARRAY );
   glTexCoordPointer(2, GL_FLOAT, sizeof(RTVertex), &m_vertexarray[0].GetUV() );

   GeometricObject *pCache = GetRootCache();
   for (int nSurface=0; nSurface<pCache->m_surfacearray.GetCount(); nSurface++)
   {
      GeometricObjectSurface &surface = pCache->m_surfacearray[nSurface];

      if (surface.m_nCullFaceMode)
      {
         glCullFace( GL_BACK );
         glEnable( GL_CULL_FACE );
      }
      else
      {
         glDisable(GL_CULL_FACE);
      }

      surface.Bind();
      if (IsSelected())
      {
         GLfloat param[4];
         param[0] = 0.3f, param[1] = 0.3f, param[2] = 0.05f, param[3] = 1.0f;
         glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, param );
      }
      glDrawElements( GL_QUADS, surface.m_quadarray.GetCount()*4, GL_UNSIGNED_INT, surface.m_quadarray.GetData());
      for (int i=0; i<surface.m_polyarray.GetCount(); i++)
         glDrawElements( GL_POLYGON, surface.m_polyarray[i].GetVertexCount(), GL_UNSIGNED_INT, surface.m_polyarray[i].GetVertexIndexArray() );
      surface.UnBind();
      m_nNumQuads += surface.m_quadarray.GetSize();
      m_nNumNGons += surface.m_polyarray.GetSize();
   }
}

void GeometricObject::DrawControlPoints()
{
   glDisable( GL_LIGHTING );
   glColor3f( 1, 0, 0 );
   glPointSize(5.0f);
   glDrawElements( GL_POINTS, m_controlpointarray.GetCount(), GL_UNSIGNED_INT, m_controlpointarray.GetData());
}

void GeometricObjectInformation::DrawInfo(CGLModelView *pView)
{
   glPushAttrib( GL_COLOR_BUFFER_BIT  );

   glColor3f( 1.0f, 1.0f, 1.0f );

   String str;
   str = "NumVertices = ";
   str += String((ULONG)m_nNumSurfaceVertices);
   pView->DrawText(20,20,str);

   str = "NumEdges = ";
   str += String((ULONG)m_nNumSurfaceEdges);
   pView->DrawText(20,40,str);

   str = "NumSurfaces = ";
   str += String((ULONG)m_nNumSurfaces);
   pView->DrawText(20,60,str);

   str = "NumQuads = ";
   str += String((ULONG)m_nNumSurfaceQuads);
   pView->DrawText(20,80,str);

   str = "NumNGons = ";
   str += String((ULONG)m_nNumSurfaceNGons);
   pView->DrawText(20,100,str);

   glPopAttrib();
}

