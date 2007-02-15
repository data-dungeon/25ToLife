// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__58C0982F_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
#define AFX_STDAFX_H__58C0982F_D8D2_11D3_83A5_00E0811049D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Functions that throw const string message rather than returning BOOLS.
#define catch_msg

#include "../Utility/RGBColor.h"

#include "SDKMacros.h"
#include "Vector.h"

//////////////////////////////////////////////////////////////////////
//
//	Tupperware Includes
//
//////////////////////////////////////////////////////////////////////

// tupperware general
#include "tupperware/aggregate.h"
#include "tupperware/loadsave.h"
#include "tupperware/tuparray.h"
#include "tupperware/tupstring.h"
#include <tupperware/TupSort.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>

// support
#include <Tupper3d/objectconverter.h>
// scene
#include <Tupper3d/scene.h>
// object
#include <Tupper3d/object.h>
#include <Tupper3d/objectpool.h>
#include <Tupper3d/trimesh.h>
#include <Tupper3d/trimeshmap.h>
#include <tupper3d/polymesh.h>
#include <tupper3d/polymeshmap.h>
#include <Tupper3d/triangleStrip.h>
#include <Tupper3d/derived.h>
#include <Tupper3d/derivedmodifier.h>
#include <Tupper3d/beziershape.h>
#include <Tupper3d/spline.h>
#include <Tupper3d/SplineShape.h>
#include <Tupper3d/bezierSpline.h>
#include <Tupper3d/BezierShape.h>
#include <Tupper3d/xref.h>
#include <Tupper3d/bone.h>
// node
#include <Tupper3d/node.h>
#include <Tupper3d/nodepool.h>
// transform
#include <Tupper3d/transform.h>
#include <Tupper3d/staticmatrix.h>
// material
#include <Tupper3d/material.h>
#include <Tupper3d/materialpool.h>
#include <Tupper3d/standardmaterial.h>
#include <Tupper3d/standardmaterialmap.h>
#include <Tupper3d/multisubobjectmaterial.h>
#include <Tupper3d/multisubobjectmaterialsubmaterial.h>
// map
#include <Tupper3d/map.h>
#include <Tupper3d/mappool.h>
#include <Tupper3d/bitmapmap.h>
#include <Tupper3d/compositemap.h>
#include <Tupper3d/compositemapsubmap.h>
// image
#include <Tupper3d/image.h>
#include <Tupper3d/imagepool.h>
// modifier
#include <Tupper3d/modifier.h>
#include <Tupper3d/modifierpool.h>
#include <Tupper3d/applyvcmodifier.h>
#include <Tupper3d/vertexpaintmodifier.h>
#include <Tupper3d/vertexpaintmodifiercontext.h>
#include <Tupper3d/holdvcmodifier.h>
#include <Tupper3d/faceattributesmodifier.h>
#include <Tupper3d/faceattributesmodifiercontext.h>
#include <Tupper3d/skinmodifier.h>
#include <Tupper3d/skinmodifierbone.h>
#include <Tupper3d/skinmodifiercontext.h>
#include <Tupper3d/skinmodifiercontextvertex.h>
#include <Tupper3d/skinmodifiercontextvertexpool.h>
#include <Tupper3d/clothmodifier.h>
#include <Tupper3d/clothmodifiercontext.h>

#include <tupper3d/CoordinateSystemConverter.h>
#include <tupper3d/MultiSubObjectMaterialConverter.h>

#undef RADTODEG
#undef DEGTORAD
#include <tupper3d/tupvertex.h>
#include <Tupper3d/objecthelper.h>

// image data (for image reader)
#include <Tupper3d/imageData.h>
#include <Tupper3d/imageDataFrame.h>
#include <Tupper3d/imageDataMap.h>
#include <Tupper3d/imageDataPalette.h>

// animation
#include <Tupper3d/AnimSet.h>
#include <Tupper3d/AnimSetPool.h>
#include <Tupper3d/AnimChannel.h>
#include <Tupper3d/AnimChannelPool.h>
#include <Tupper3d/AnimCurve.h>
#include <Tupper3d/AnimCurvePool.h>
#include <Tupper3d/AnimGlobals.h>
#include <Tupper3d/AnimClipPool.h>
#include <Tupper3d/AnimInstancedClip.h>
#include <Tupper3d/AnimSourceClip.h>

// camera
#include <Tupper3d/camera.h>

// light
#include <Tupper3d/light.h>

// locator
#include <Tupper3d/locator.h>

#ifdef _DEBUG
#define SNEW new (__FILE__, __LINE__)
#else
#define SNEW new
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_STDAFX_H__58C0982F_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
