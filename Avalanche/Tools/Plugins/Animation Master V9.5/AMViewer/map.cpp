// Dan  4/11/00  \Dan85\AMViewer\map.cpp

#include "stdafx.h"
#include "AMViewer.h"
#include "Model.h"
#include "BitmapNo.h"
#include "Matrix34.h"
#include "MainFrm.h"
#include "AMViewerView.h"
#include <Math.h>
#ifdef _MAC
#include "3rdParty\Conix\glut.h"
#else
#include <gl\glut.h>
#endif

extern CAMViewerApp theApp;
extern CMainFrame *g_mainframe;

BOOL Map::BuildTextureMaps()
{
   BitmapNode bn;
   
   if (bn.Load((LPCTSTR)m_filename, BitmapNode::LOADUNCOMPRESSED) != BitmapNode::LOAD_OK)
      return FALSE;

   int dstwidth, dstheight;
   int exp;
   exp = (int)(log10((double)bn.m_width-MAPBORDER*2)/log10(2.));
   exp = max(1, min(8, exp));
   dstwidth = (int)pow(2.0f, exp)+MAPBORDER*2;

   exp = (int)(log10((double)bn.m_height-MAPBORDER*2)/log10(2.0f));
   exp = max(1, min(8, exp));
   dstheight = (int)(pow(2.0f, exp)+MAPBORDER*2);

   if (dstwidth != bn.m_width || dstheight != bn.m_height) {
      if (!bn.ScaleBitmap(dstwidth, dstheight))
         return FALSE;
   }
   UBYTE *rgb;
   int bytesperpixel;

   rgb = bn.CreateRGB(&bytesperpixel);
   if (rgb==NULL)
      return FALSE;

   if (m_buildclamp) {
      glGenTextures(1, &m_textureclamp);
      glBindTexture(GL_TEXTURE_2D, m_textureclamp);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//   	glTexImage2D(GL_TEXTURE_2D, 0, bytesperpixel==3?GL_RGB:GL_RGBA, bn.m_width, bn.m_height, 0, bytesperpixel==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, rgb);
   	gluBuild2DMipmaps(GL_TEXTURE_2D, bytesperpixel==3?GL_RGB:GL_RGBA, bn.m_width, bn.m_height, bytesperpixel==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, rgb);
   }
   
   if (m_buildrepeat) {
      glGenTextures(1, &m_texturerepeat);
      glBindTexture(GL_TEXTURE_2D, m_texturerepeat);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR:GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, g_mainframe->m_modelview->m_bbilinear?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST);
//   	glTexImage2D(GL_TEXTURE_2D, 0, bytesperpixel==3?GL_RGB:GL_RGBA, bn.m_width, bn.m_height, 0, bytesperpixel==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, rgb);
   	gluBuild2DMipmaps(GL_TEXTURE_2D, bytesperpixel==3?GL_RGB:GL_RGBA, bn.m_width, bn.m_height, bytesperpixel==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, rgb);
   }

   BitmapNode::Free(rgb);// must do this because rgb was allocated by Non debug Version of MFC
   return TRUE;
}



