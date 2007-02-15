// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__5128F74D_6242_4586_A897_6F6EDDE36C3D__INCLUDED_)
#define AFX_STDAFX_H__5128F74D_6242_4586_A897_6F6EDDE36C3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#if _MSC_VER > 1200

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>

#include <GL/GL.h>
#include "Include/GL/glut.h"
#include "Include/GL/Glext.h"
typedef void (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
extern PFNGLADDSWAPHINTRECTWINPROC glAddSwapHintRectWIN;
extern PFNGLADDSWAPHINTRECTWINPROC glAddSwapHintRectWIN;
extern PFNGLBINDBUFFERPROC         glBindBuffer;
extern PFNGLBUFFERDATAPROC         glBufferData;
extern PFNGLBUFFERSUBDATAPROC      glBufferSubData;
extern PFNGLDELETEBUFFERSPROC      glDeleteBuffers;
extern PFNGLGENBUFFERSPROC         glGenBuffers;
extern PFNGLMAPBUFFERPROC          glMapBuffer;
extern PFNGLUNMAPBUFFERPROC        glUnmapBuffer;
inline GLboolean glSet( GLenum nCap, GLboolean bValue )
{
   GLint bPrev;
   glGetIntegerv( nCap, &bPrev );
   if (bValue)
      glEnable( nCap );
   else
      glDisable( nCap );
   return bPrev;
}

// just for looks so we know where the virtual table base is
#define virt_base

// Functions that throw const string message rather than returning BOOLS.
#define catch_msg

#include "../Utility/AString.h"
#include "../Utility/RGBColor.h"
#include "../Utility/ATime.h"
#include "../Utility/Vector.h"
#include "../Utility/Vector2.h"
#include "../Utility/Vector4.h"
#include "../Utility/HArray.h"

#include "Interface/Global.h"

#include <afx.h>

#ifdef _DEBUG
   #define SNEW new (__FILE__, __LINE__)
#else
   #define SNEW new
#endif

#define SNIPEEXPORT   __declspec(dllexport)

//{{AFX_INSERT_LOCATION}}



// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__5128F74D_6242_4586_A897_6F6EDDE36C3D__INCLUDED_)
