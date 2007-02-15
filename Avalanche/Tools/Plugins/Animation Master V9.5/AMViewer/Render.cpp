// RC  9/26/2000  \Master85\AMViewer\Render.cpp

#include "stdafx.h"
#include <Math.h>
#include "AMViewerDoc.h"
#include "Model.h"
#include "AMViewer.h"
#include "AMViewerView.h"
#include "MainFrm.h"

extern CMainFrame *g_mainframe;

class DefaultAttr : public Attr 
{
public:
   DefaultAttr() {
      m_diffusecolor.Set(1.0f, 1.0f, 1.0f);   
      m_ambiance = .2f;
   }
};

static DefaultAttr f_defaultattr;

enum { DL_SHADED, DL_WIREFRAME, DL_VERTICES, DL_NORMALS, DL_TANGENTS };

inline ColorVector GetSpecularity( Attr *attr, Vector &n )
{
   static Vector d(0,0,-1);

   Vector result = n;
   result *= 2 * n.z;
   result += d;
   result.Normalize();

   float spec;
   if ((spec = -result.Dot( d )) > .5) {
      if ((spec = (float)pow( (double)spec, 8.0 + pow( 2.0, (double)9*(1.0f-attr->m_specularsize))) * attr->m_specularintensity) > .001) {
         ColorVector speccolor = spec;
         speccolor *= attr->m_specularcolor;
         return speccolor;
      }
   }
   static ColorVector nospeccolor(0.0f);
   return nospeccolor;
}
#include "gl\glu.h"

void CAMViewerView::Render()
{
   Model *model = GetDocument()->m_model;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   if (model) {
      GetDocument()->ApplyAction(GetCurrentPlayTime());

      model->TransformModel(this);

      m_patchsplitter.m_zoom = m_zoom;
      model->Render(this);
	  if (m_bshowbones)
		model->RenderBones(this);
   }
   glFlush();
}

#define RADTODEG  (180.0f / 3.14f)

void GetRotation(const Vector &vect,Vector &rotation)
{
	rotation.y = RADTODEG * (float)atan2( vect.x, vect.z );
	rotation.x = RADTODEG * (float)atan2( -vect.y, sqrt(vect.x*vect.x + vect.z*vect.z));
}

GLubyte ditherPattern1[] = {
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,0x55,0x55,0x55,0x55};

void RenderBone(Bone *bone,const Matrix34 &parentscreenmatrix,CAMViewerView *view)
{
  for (; bone; bone = bone->m_sibling) {

	  Matrix34 screenmatrix;
      screenmatrix = parentscreenmatrix * bone->m_matrix;
  
	Vector pivot = screenmatrix * bone->m_pivot;
	Vector end = screenmatrix * bone->m_end;
	Vector roll = screenmatrix * bone->m_roll;
	view->Project(pivot);	
	view->Project(end);
	view->Project(roll);

	double radius = (roll-pivot).Norm()*0.15f;
	Vector bonevect = end - pivot;
	double bonevectlength = bonevect.Norm();
	
	::glDisable(GL_LIGHTING);
	::glColor3f(1.0f,1.0f,0.5f);
	::glLineWidth(1.0f);

//	::glEnable(GL_COLOR_MATERIAL);
	::glDisable( GL_TEXTURE_2D );
 

	Vector rotate;
	GetRotation(bonevect,rotate);
	
	::glPolygonStipple(ditherPattern1);
	::glEnable(GL_POLYGON_STIPPLE);

	GLUquadricObj *obj = gluNewQuadric();
	::gluQuadricDrawStyle(obj,GLU_FILL);
	::gluQuadricNormals(obj,GLU_SMOOTH);
	::gluQuadricOrientation(obj,GLU_OUTSIDE);

//	Draw Pivot Cone
	float pivotconelength = (float)bonevectlength * 0.10f;
	Vector pivotcone(bonevect);
	pivotcone *= 0.10f;
	pivotcone += pivot;

//  Draw End Cone
	float endconelength = (float)bonevectlength * 0.90f;

	for (int i = 0; i < 2; i++)
	{
		if (i ==0)
			::glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		else
			::glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);


	::glPushMatrix();

		::glTranslatef(pivot.x,pivot.y,pivot.z);
		
		::glRotatef(rotate.y,0.0f,1.0f,0.0f);
		::glRotatef(rotate.x,1.0f,0.0f,0.0f);
		
		::gluCylinder(obj,0.0001f,radius,pivotconelength,4,1);

	::glPopMatrix();

	::glPushMatrix();

		::glTranslatef(pivotcone.x,pivotcone.y,pivotcone.z);
		
		::glRotatef(rotate.y,0.0f,1.0f,0.0f);
		::glRotatef(rotate.x,1.0f,0.0f,0.0f);
		
		::gluCylinder(obj,radius,0.0001f,endconelength,4,1);

	::glPopMatrix();
	}
	
	::gluDeleteQuadric(obj);

//	Put things Back
	::glDisable(GL_POLYGON_STIPPLE);

	::glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
	::glLineWidth(1.0f);

	::glDisable(GL_LIGHTING);
	::glEnable( GL_TEXTURE_2D );

//	Continue down tree
      if (bone->m_child)      
         RenderBone(bone->m_child,screenmatrix,view);
  }
}

void Model::RenderBones(CAMViewerView *view)
{
	view->m_matrix.SetTranslate(Vector(0.0f));
	Vector translate = view->m_matrix * view->m_offset;
	translate.z -= view->m_focaldistance;
	view->m_matrix.SetTranslate(translate);

	RenderBone(m_bonehead,view->m_matrix,view);
}

//************************* Patch Model Render ***************************************

void PatchModel::Render(CAMViewerView *view)
{
   int drawnlast = -1; // no subdivisions render

   view->m_patchsplitter.m_drawnlast=-1;
   view->m_patchsplitter.m_model = this;
   view->m_patchsplitter.m_numpatches = 0;

   if (view->m_bperspective)
      view->m_patchsplitter.m_viewplane = -view->m_focallength;
   else
      view->m_patchsplitter.m_viewplane = 0;

   m_lasttexture = 0;
   
   int patchcount = m_patchlist.GetSize();
   for (int patchindex=0; patchindex < patchcount; patchindex++) {
      Patch &patch = m_patchlist.ElementAt(patchindex);

// Cull offscreen polys
      Vector v0, v1, v2, v3;
      v0 = m_transformedvertexlist[patch.m_vertexid[0]];
      v1 = m_transformedvertexlist[patch.m_vertexid[1]];
      v2 = m_transformedvertexlist[patch.m_vertexid[2]];
      v3 = m_transformedvertexlist[patch.m_vertexid[3]];

      if (view->m_bsubdivided) { // vertices are not projected yet
         view->Project( v0 );
         view->Project( v1 );
         view->Project( v2 );
         view->Project( v3 );
      }

      if ((v0.x < -1 && v1.x < -1 && v2.x < -1 && v3.x < -1)
       || (v0.x > 1 && v1.x > 1 && v2.x > 1 && v3.x > 1)
       || (v0.y < -1 && v1.y < -1 && v2.y < -1 && v3.y < -1)
       || (v0.y > 1 && v1.y > 1 && v2.y > 1 && v3.y > 1))
         continue;

// Backface cull if not subdividing.  Subdivided patches get culled in splitter
      if (!view->m_bsubdivided)
         if (m_transformednormallist[patch.m_normalid[0]].z < 0 && m_transformednormallist[patch.m_normalid[1]].z < 0 && 
          m_transformednormallist[patch.m_normalid[2]].z < 0 && m_transformednormallist[patch.m_normalid[3]].z < 0)
            continue;

 // put texture on stack
      GLuint texture=m_notexture;
      MapNode *mapnode = NULL;
      if (view->m_bdecals && patch.m_mapnodearray.GetSize()) {
         for (int mapindex = 0; mapindex < patch.m_mapnodearray.GetSize(); mapindex++) {
            if (patch.m_mapnodearray[mapindex]->m_kind == MapNode::COLORMAP || patch.m_mapnodearray[mapindex]->m_kind == MapNode::COOKIECUTMAP) { // only support first color map
               mapnode = patch.m_mapnodearray[mapindex];
               break;
            }
         }
         if (mapnode) {
            if (mapnode->m_texturedisplaylist) { // Model Map insures 0 or 1 uvs
               texture = mapnode->m_texturedisplaylist;
               view->m_patchsplitter.m_0or1uvs = TRUE;
            }
            else { // Linked Maps, UV's can be anything
               if (mapnode->m_repeat.x == 1 && mapnode->m_repeat.y == 1)
                  texture = m_maplist[mapnode->m_mapid].m_textureclamp;
               else
                  texture = m_maplist[mapnode->m_mapid].m_texturerepeat;
                  
               view->m_patchsplitter.m_0or1uvs = FALSE;
            }
         }
      }
      view->m_patchsplitter.m_texture = texture;
// Render Patch Now ****************************************************************************************************

 // Non subdivided 
      if (!view->m_bsubdivided) {
         // Cull patchs behind camera
         if (view->m_bperspective)
            if (m_transformedvertexlist[patch.m_vertexid[0]].z < 0 || 
             m_transformedvertexlist[patch.m_vertexid[1]].z < 0 ||
             m_transformedvertexlist[patch.m_vertexid[2]].z < 0 ||
             m_transformedvertexlist[patch.m_vertexid[3]].z < 0) // should clip patches, but this is good enough for this viewer
               continue; // vertices have been projected so valid z's between 0 and 1;

         //render Normals
         if (view->m_bnormals) {
            if (drawnlast != DL_NORMALS) {
               glDisable( GL_TEXTURE_2D );
               glColor3f(1.0f, 1.0f, 0.0f);
               drawnlast = DL_NORMALS;
            }
            glBegin(GL_LINES);
            float length = .2f * (m_vertexlist[patch.m_vertexid[2]]-m_vertexlist[patch.m_vertexid[0]]).Norm();
            for (int i=0; i < 4; i++) {
               Vector vertex = m_transformedvertexlist[patch.m_vertexid[i]];
               Vector vertexunprojected = vertex;
               view->Unproject(vertexunprojected);
               Vector end = vertexunprojected + m_transformednormallist[patch.m_normalid[i]] * length;
               view->Project(end);
               glVertex3fv((float *)&vertex);
               glVertex3fv((float *)&end);
            }
            glEnd();
         }
         //Render Tangents
         if (view->m_btangents) {
            if (drawnlast != DL_TANGENTS) {
               glDisable( GL_TEXTURE_2D );
               glColor3f(0.0f, 1.0f, 0.0f);
               glPointSize(3.0f);
               drawnlast = DL_TANGENTS;
            }
            glBegin(GL_POINTS);
               for (int i=0; i < 12; i++) {
                  Vector vertex = m_transformedcontrolvertexlist[patch.m_controlvertexid[i]];
                  glVertex3fv((float *)&vertex);
               }
            glEnd();
            
         }
         //Render Vertices
         if (view->m_bvertices) {
            if (drawnlast != DL_VERTICES) {
               glDisable( GL_TEXTURE_2D );
               glColor3f(1.0f, 0.0f, 0.0f);
               glPointSize(3.0f);
               drawnlast = DL_VERTICES;
            }
            glBegin(GL_POINTS);
               glVertex3fv((float *)&(Vector &)m_transformedvertexlist[patch.m_vertexid[0]]);
               glVertex3fv((float *)&(Vector &)m_transformedvertexlist[patch.m_vertexid[1]]);
               glVertex3fv((float *)&(Vector &)m_transformedvertexlist[patch.m_vertexid[2]]);
               glVertex3fv((float *)&(Vector &)m_transformedvertexlist[patch.m_vertexid[3]]);
            glEnd();
         }

//  Setup Texture and fillmode
         if (view->m_bshaded && drawnlast != DL_SHADED) {
            glPolygonMode(GL_FRONT, GL_FILL);
            drawnlast = DL_SHADED;
            glEnable(GL_TEXTURE_2D);
         }
         else if (!view->m_bshaded && drawnlast != DL_WIREFRAME) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            drawnlast = DL_WIREFRAME;
            glEnable(GL_TEXTURE_2D);
         }
         if (m_lasttexture != texture) {
            m_lasttexture = texture;
            glBindTexture(GL_TEXTURE_2D, texture);
         }

         glBegin(GL_QUADS);
// Light, Project and Render
            Attr *attr;
            if (patch.m_attrid!= -1)
               attr = &m_attrlist[patch.m_attrid];
            else
               attr = &f_defaultattr;
            ColorVector color;
            if (texture==m_notexture && view->m_bmaterials)
               color = attr->m_diffusecolor;
            else
               color.Set(1);
            for (int i=0; i < 4; i++) {
               ColorVector litcolor = max(attr->m_ambiance, m_transformednormallist[patch.m_normalid[i]].z);
               litcolor *= color; 
               if (attr->m_specularsize)
                  litcolor += GetSpecularity( attr, m_transformednormallist[patch.m_normalid[i]] );
               litcolor.Max(1.0f);
               glColor4f(litcolor.red, litcolor.green, litcolor.blue, 1.0f-attr->m_transparency);
               if (texture==m_notexture)
                  glTexCoord2f(0,0);
               else
                  glTexCoord2fv((float *)&mapnode->m_uv[i]);
               glVertex3fv((float *)&(Vector &)m_transformedvertexlist[patch.m_vertexid[i]]);
            }
         glEnd();
      }
// Subdivided 
      else {
         SplitPatch splitpatch;

         splitpatch.SetVertices(
          m_transformedvertexlist[patch.m_vertexid[0]],
          m_transformedvertexlist[patch.m_vertexid[1]],
          m_transformedvertexlist[patch.m_vertexid[2]],
          m_transformedvertexlist[patch.m_vertexid[3]],
          m_transformedcontrolvertexlist[patch.m_controlvertexid[0]],
          m_transformedcontrolvertexlist[patch.m_controlvertexid[1]],
          m_transformedcontrolvertexlist[patch.m_controlvertexid[2]],
          m_transformedcontrolvertexlist[patch.m_controlvertexid[5]],
          m_transformedcontrolvertexlist[patch.m_controlvertexid[6]],
          m_transformedcontrolvertexlist[patch.m_controlvertexid[9]],
          m_transformedcontrolvertexlist[patch.m_controlvertexid[10]],
          m_transformedcontrolvertexlist[patch.m_controlvertexid[11]]);
    
         splitpatch.SetNormals(
           m_transformednormallist[patch.m_normalid[0]],
           m_transformednormallist[patch.m_normalid[1]],
           m_transformednormallist[patch.m_normalid[2]],
           m_transformednormallist[patch.m_normalid[3]]);

         view->m_patchsplitter.m_isuvs = texture!=m_notexture;
         view->m_patchsplitter.m_is3point = patch.m_is3point;
         view->m_patchsplitter.m_origpatch = &patch;

         if (texture!=m_notexture) {
            splitpatch.SetUVs(
             mapnode->m_uv[0],
             mapnode->m_uv[1],
             mapnode->m_uv[2],
             mapnode->m_uv[3]);
         }

         view->m_patchsplitter.Subdivide(splitpatch); // Calls SubdivideCallback()
      }
   }
   if (view->m_bsubdivided) {
      CString text;
      text.Format("%d Polys / %d Patches", view->m_patchsplitter.m_numpatches, GetPatchPolyCount());
      g_mainframe->SetPatchPolyCount(text);
   }
}
 
void MyPatchSplitter::SubdivideCallback(SplitPatch &patch) 
{
// Cull patchs behind camera
   if (m_view->m_bperspective)
      if (patch.m_p1.z > -1 || patch.m_p2.z > -1 || patch.m_p3.z > -1 || patch.m_p3.z > -1) // should clip poly, but this is good enough for this viewer
         return; // vertices have not been projected so valid z's are less than -1;

   m_numpatches++;

   // Draw norms before we project so we can calc length and end pos with out unprojecting
   if (m_view->m_bnormals) {
      if (m_drawnlast != DL_NORMALS) {
         glDisable( GL_TEXTURE_2D );
         glColor3f(1.0f, 1.0f, 0.0f);
         m_drawnlast = DL_NORMALS;
      }
      glBegin(GL_LINES);
         float length = .2f * (patch.m_p1-patch.m_p16).Norm();
         Vector end = patch.m_p1 + patch.m_n[0] * length;
         m_view->Project(end);
         Vector start = patch.m_p1;
         m_view->Project(start);
         glVertex3fv((float *)&start);
         glVertex3fv((float *)&end);

         end = patch.m_p4 + patch.m_n[1] * length;
         m_view->Project(end);
         start = patch.m_p4;
         m_view->Project(start);
         glVertex3fv((float *)&start);
         glVertex3fv((float *)&end);

         end = patch.m_p16 + patch.m_n[2] * length;
         m_view->Project(end);
         start = patch.m_p16;
         m_view->Project(start);
         glVertex3fv((float *)&start);
         glVertex3fv((float *)&end);

         end = patch.m_p13 + patch.m_n[3] * length;
         m_view->Project(end);
         start = patch.m_p13;
         m_view->Project(start);
         glVertex3fv((float *)&start);
         glVertex3fv((float *)&end);
      glEnd();
   }

   // project all vertices now that we have finished splitting
   m_view->Project(patch.m_p1);
   m_view->Project(patch.m_p4);
   m_view->Project(patch.m_p16);
   m_view->Project(patch.m_p13);
   // Render Tangents
   if (m_view->m_btangents) {
      if (m_drawnlast != DL_TANGENTS) {
         glDisable( GL_TEXTURE_2D );
         glColor3f(0.0f, 1.0f, 0.0f);
         glPointSize(1.0f);
         m_drawnlast = DL_TANGENTS;
      }
      glBegin(GL_POINTS);
      for (int i=0; i < 12; i++) {
         Vector vertex = m_model->m_transformedcontrolvertexlist[m_origpatch->m_controlvertexid[i]];
         m_view->Project(vertex);// not projected yet because of patch splitter
         glVertex3fv((float *)&vertex);
      }
      glEnd();
   }

   // Render Vertices
   if (m_view->m_bvertices) {
      if (m_drawnlast != DL_VERTICES) {
         glDisable( GL_TEXTURE_2D );
         glColor3f(1.0f, 0.0f, 0.0f);
         glPointSize(3.0f);
         m_drawnlast = DL_VERTICES;
      }
      glBegin(GL_POINTS);
         glVertex3fv((float *)&patch.m_p1);
         glVertex3fv((float *)&patch.m_p4);
         glVertex3fv((float *)&patch.m_p16);
         glVertex3fv((float *)&patch.m_p13);
      glEnd();
   }

//  Setup Texture and fillmode
   if (m_view->m_bshaded && m_drawnlast != DL_SHADED) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      m_drawnlast = DL_SHADED;
      glEnable( GL_TEXTURE_2D );
   }
   else if (!m_view->m_bshaded && m_drawnlast != DL_WIREFRAME) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      m_drawnlast = DL_WIREFRAME;
      glEnable( GL_TEXTURE_2D );
   }
   if (m_model->m_lasttexture != m_texture) {
      m_model->m_lasttexture = m_texture;
      glBindTexture(GL_TEXTURE_2D, m_texture);
   }  

   glBegin(GL_QUADS);
// Light, Project and Render
      Attr *attr;
      if (m_origpatch->m_attrid!= -1)
         attr = &m_model->m_attrlist[m_origpatch->m_attrid];
      else
         attr = &f_defaultattr;

      ColorVector color;
      if (m_texture==m_model->m_notexture && m_view->m_bmaterials)
         color = attr->m_diffusecolor;
      else
         color.Set(1);

      ColorVector litcolor = max(attr->m_ambiance, patch.m_n[0].z);
      litcolor *= color; 
      if (attr->m_specularsize)
         litcolor += GetSpecularity( attr, patch.m_n[0] );
      litcolor.Max(1.0f);
      glColor4f(litcolor.red, litcolor.green, litcolor.blue, 1.0f-attr->m_transparency);
      if (m_texture==m_model->m_notexture)
         glTexCoord2f(0,0);
      else
         glTexCoord2fv((float *)&patch.m_uv[0]);
      glVertex3fv((float *)&patch.m_p1);

      litcolor = max(attr->m_ambiance, patch.m_n[1].z);
      litcolor *= color; 
      if (attr->m_specularsize)
         litcolor += GetSpecularity( attr, patch.m_n[1] );
      litcolor.Max(1.0f);
      glColor4f(litcolor.red, litcolor.green, litcolor.blue, 1.0f-attr->m_transparency);
      if (m_texture==m_model->m_notexture)
         glTexCoord2f(0,0);
      else
         glTexCoord2fv((float *)&patch.m_uv[1]);
      glVertex3fv((float *)&patch.m_p4);

      litcolor = max(attr->m_ambiance, patch.m_n[2].z);
      litcolor *= color; 
      if (attr->m_specularsize)
         litcolor += GetSpecularity( attr, patch.m_n[2] );
      litcolor.Max(1.0f);
      glColor4f(litcolor.red, litcolor.green, litcolor.blue, 1.0f-attr->m_transparency);
      if (m_texture==m_model->m_notexture)
         glTexCoord2f(0,0);
      else
         glTexCoord2fv((float *)&patch.m_uv[2]);
      glVertex3fv((float *)&patch.m_p16);

      litcolor = max(attr->m_ambiance, patch.m_n[3].z);
      litcolor *= color; 
      if (attr->m_specularsize)
         litcolor += GetSpecularity( attr, patch.m_n[3] );
      litcolor.Max(1.0f);
      glColor4f(litcolor.red, litcolor.green, litcolor.blue, 1.0f-attr->m_transparency);
      if (m_texture==m_model->m_notexture)
         glTexCoord2f(0,0);
      else
         glTexCoord2fv((float *)&patch.m_uv[3]);
      glVertex3fv((float *)&patch.m_p13);

   glEnd();
}

//******************************** POLY MODELS ****************************************
void PolyModel::Render(CAMViewerView *view)
{
   if (view->m_bnormals)
      RenderNormals(view);

   if (view->m_bvertices)
      RenderVertices();

   m_lasttexture = 0;
   m_lastattr = -1;
   m_lastattrdiffuse = -1;

   if (view->m_bshaded)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   else //wireframe
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   
   if (view->m_bdecals && (view->m_bnormals || view->m_bvertices)) // These two modes turned of GL_TEXTURE_2D
      glEnable( GL_TEXTURE_2D );

   RenderPolys(view);
}

void PolyModel::RenderPolys(CAMViewerView *view)
{
   int polycount = m_polylist.GetSize();
   for (int polyindex=0; polyindex < polycount; polyindex++) {
      Poly &poly = m_polylist.ElementAt(polyindex);

// Cull offscreen polys
      Vector v0, v1, v2, v3;
      v0 = m_transformedvertexlist[poly.m_vertexid[0]];
      v1 = m_transformedvertexlist[poly.m_vertexid[1]];
      v2 = m_transformedvertexlist[poly.m_vertexid[2]];
      v3 = m_transformedvertexlist[poly.m_vertexid[3]];

      if ((v0.x < -1 && v1.x < -1 && v2.x < -1 && v3.x < -1)
       || (v0.x > 1 && v1.x > 1 && v2.x > 1 && v3.x > 1)
       || (v0.y < -1 && v1.y < -1 && v2.y < -1 && v3.y < -1)
       || (v0.y > 1 && v1.y > 1 && v2.y > 1 && v3.y > 1))
         continue;

// Backface cull if not subdividing.  Subdivided patches get culled in splitter
      if (m_transformednormallist[poly.m_normalid[0]].z < 0 && m_transformednormallist[poly.m_normalid[1]].z < 0 && 
       m_transformednormallist[poly.m_normalid[2]].z < 0 && m_transformednormallist[poly.m_normalid[3]].z < 0)
         continue;

// Cull polys behind camera
      if (view->m_bperspective)
         if (v0.z < 0 || v1.z < 0 || v2.z < 0 || v3.z < 0) // should clip poly, but this is good enough for this viewer
            continue; // vertices have been projected, so valid z's are 0 to 1

// put texture on stack
      GLuint texture=m_notexture;
      MapNode *mapnode = NULL;
      if (view->m_bdecals && poly.m_mapnodearray.GetSize()) {
         for (int mapindex = 0; mapindex < poly.m_mapnodearray.GetSize(); mapindex++) {
            if (poly.m_mapnodearray[mapindex]->m_kind == MapNode::COLORMAP || poly.m_mapnodearray[mapindex]->m_kind == MapNode::COOKIECUTMAP) { // only support first color map
               mapnode = poly.m_mapnodearray[mapindex];
               break;
            }
         }
         if (mapnode) {
            if (mapnode->m_texturedisplaylist)
               texture = mapnode->m_texturedisplaylist;
            else {
               if (mapnode->m_repeat.x == 1 && mapnode->m_repeat.y == 1)
                  texture = m_maplist[mapnode->m_mapid].m_textureclamp;
               else   
                  texture = m_maplist[mapnode->m_mapid].m_texturerepeat;
            }
         }
      }

      if (m_lasttexture != texture) {
         m_lasttexture = texture;
         glBindTexture(GL_TEXTURE_2D, texture);
      }

// Light and Render
      Attr *attr;
      if (poly.m_attrid != -1)
         attr = &m_attrlist[poly.m_attrid];
      else
         attr = &f_defaultattr;

      ColorVector color;
      if (texture==m_notexture && view->m_bmaterials)
         color = attr->m_diffusecolor;
      else
         color.Set(1);

      glBegin(GL_QUADS);
      for (int i=0; i < 4; i++) {
         ColorVector litcolor = max(attr->m_ambiance, m_transformednormallist[poly.m_normalid[i]].z);
         litcolor *= color; 
         if (attr->m_specularsize)
            litcolor += GetSpecularity( attr, m_transformednormallist[poly.m_normalid[i]] );
         litcolor.Max(1.0f);
         glColor4f(litcolor.red, litcolor.green, litcolor.blue, 1.0f-attr->m_transparency);

         if (texture==m_notexture)
            glTexCoord2f(0,0);
         else
            glTexCoord2fv((float *)&mapnode->m_uv[i]);

         Vector &vertex = m_transformedvertexlist[poly.m_vertexid[i]];
         glVertex3fv((float *)&vertex);
      }
      glEnd();
   }
}

void PolyModel::RenderVertices()
{
   glDisable( GL_TEXTURE_2D );
   glColor3f(1.0f, 0.0f, 0.0f);
   glPointSize(3.0f);
   int polycount = m_polylist.GetSize();

   glBegin(GL_POINTS);
      for (int polyindex=0; polyindex < polycount; polyindex++) {
         Poly &poly = m_polylist.ElementAt(polyindex);
         // Cull offscreen polys
         Vector v0, v1, v2, v3;
         v0 = m_transformedvertexlist[poly.m_vertexid[0]];
         v1 = m_transformedvertexlist[poly.m_vertexid[1]];
         v2 = m_transformedvertexlist[poly.m_vertexid[2]];
         v3 = m_transformedvertexlist[poly.m_vertexid[3]];

         if ((v0.x < -1 && v1.x < -1 && v2.x < -1 && v3.x < -1)
          || (v0.x > 1 && v1.x > 1 && v2.x > 1 && v3.x > 1)
          || (v0.y < -1 && v1.y < -1 && v2.y < -1 && v3.y < -1)
          || (v0.y > 1 && v1.y > 1 && v2.y > 1 && v3.y > 1))
            continue;

         // Backface cull if not subdividing.  Subdivided patches get culled in splitter
         if (m_transformednormallist[poly.m_normalid[0]].z < 0 && m_transformednormallist[poly.m_normalid[1]].z < 0 && 
          m_transformednormallist[poly.m_normalid[2]].z < 0 && m_transformednormallist[poly.m_normalid[3]].z < 0)
            continue;
         for (int i=0; i < 4; i++) {
            Vector &vertex = m_transformedvertexlist.ElementAt(poly.m_vertexid[i]);
            glVertex3fv((float *)&vertex);
         }
      }
	glEnd();
}

void PolyModel::RenderNormals(CAMViewerView *view)
{
   glDisable( GL_TEXTURE_2D );
   glBegin(GL_LINES);
      glColor3f(1.0f, 1.0f, 0.0f);
      int polycount = m_polylist.GetSize();
      for (int polyindex=0; polyindex < polycount; polyindex++) {
         Poly &poly = m_polylist.ElementAt(polyindex);
         // Cull offscreen polys
         Vector v0, v1, v2, v3;
         v0 = m_transformedvertexlist[poly.m_vertexid[0]];
         v1 = m_transformedvertexlist[poly.m_vertexid[1]];
         v2 = m_transformedvertexlist[poly.m_vertexid[2]];
         v3 = m_transformedvertexlist[poly.m_vertexid[3]];

         if ((v0.x < -1 && v1.x < -1 && v2.x < -1 && v3.x < -1)
          || (v0.x > 1 && v1.x > 1 && v2.x > 1 && v3.x > 1)
          || (v0.y < -1 && v1.y < -1 && v2.y < -1 && v3.y < -1)
          || (v0.y > 1 && v1.y > 1 && v2.y > 1 && v3.y > 1))
            continue;

         // Backface cull if not subdividing.  Subdivided patches get culled in splitter
         if (m_transformednormallist[poly.m_normalid[0]].z < 0 && m_transformednormallist[poly.m_normalid[1]].z < 0 && 
          m_transformednormallist[poly.m_normalid[2]].z < 0 && m_transformednormallist[poly.m_normalid[3]].z < 0)
            continue;
                
         float length = .2f * (m_vertexlist[poly.m_vertexid[2]]-m_vertexlist[poly.m_vertexid[0]]).Norm();
         for (int i=0; i < 4; i++) {
            Vector &vertex = m_transformedvertexlist.ElementAt(poly.m_vertexid[i]);
            Vector vertexunprojected = vertex;
            view->Unproject(vertexunprojected);
            Vector end = vertexunprojected + m_transformednormallist[poly.m_normalid[i]] * length;
            view->Project(end);
            glVertex3fv((float *)&vertex);
            glVertex3fv((float *)&end);
         }
      }
	glEnd();
}

//Converts a HashPatch into a quad
/*
Quad *PatchModel::HashPatchToQuad(Patch *patch)
{
   Quad *quad = new Quad;
   
   quad->m_vertex[0] = m_transformedvertexlist[patch->m_vertexid[0]];
   quad->m_vertex[2] = m_transformedvertexlist[patch->m_vertexid[3]];
   quad->m_vertex[6] = m_transformedvertexlist[patch->m_vertexid[1]];
   quad->m_vertex[8] = m_transformedvertexlist[patch->m_vertexid[2]];
   
   quad->m_vertex[1] = .5 * (m_transformedvertexlist[patch->m_vertexid[3]] + m_transformedvertexlist[patch->m_vertexid[0]]) + 
    (m_transformedcontrolvertexlist[patch->m_controlvertexid[2]] - m_transformedvertexlist[patch->m_vertexid[0]] - m_transformedvertexlist[patch->m_vertexid[3]] + 
    m_transformedcontrolvertexlist[patch->m_controlvertexid[6]])*3/8;

   quad->m_vertex[7] = .5 * (m_transformedvertexlist[patch->m_vertexid[2]]+ m_transformedvertexlist[patch->m_vertexid[1]]) + 
    (m_transformedcontrolvertexlist[patch->m_controlvertexid[5]] - m_transformedvertexlist[patch->m_vertexid[1]] - m_transformedvertexlist[patch->m_vertexid[2]] + 
    m_transformedcontrolvertexlist[patch->m_controlvertexid[9]])*3/8;
    

   Vector tangent1 = (m_transformedcontrolvertexlist[patch->m_controlvertexid[0]] - 
    m_transformedvertexlist[patch->m_vertexid[0]] + m_transformedcontrolvertexlist[patch->m_controlvertexid[10]]-
    m_transformedvertexlist[patch->m_vertexid[3]])/2;

   Vector tangent2 = (m_transformedvertexlist[patch->m_vertexid[1]] - 
    m_transformedcontrolvertexlist[patch->m_controlvertexid[1]] + m_transformedvertexlist[patch->m_vertexid[2]]-
    m_transformedcontrolvertexlist[patch->m_controlvertexid[11]])/2;

   quad->m_vertex[4] = .5 * (quad->m_vertex[1] + quad->m_vertex[7]) + 
    (tangent1 - tangent2)*3/8;

   quad->m_vertex[3] = .5 * (m_transformedvertexlist[patch->m_vertexid[0]] + m_transformedvertexlist[patch->m_vertexid[1]]) + 
    (m_transformedcontrolvertexlist[patch->m_controlvertexid[1]] - m_transformedvertexlist[patch->m_vertexid[1]] - m_transformedvertexlist[patch->m_vertexid[0]] + 
    m_transformedcontrolvertexlist[patch->m_controlvertexid[0]])*3/8;
    
   quad->m_vertex[5] = .5 * (m_transformedvertexlist[patch->m_vertexid[3]] + m_transformedvertexlist[patch->m_vertexid[2]]) + 
    (m_transformedcontrolvertexlist[patch->m_controlvertexid[11]] - m_transformedvertexlist[patch->m_vertexid[2]] - m_transformedvertexlist[patch->m_vertexid[3]] + 
    m_transformedcontrolvertexlist[patch->m_controlvertexid[10]])*3/8;

   return quad;
}
*/