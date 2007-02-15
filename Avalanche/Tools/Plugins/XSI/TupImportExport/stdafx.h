#if !defined(AFX_STDAFX_H_INCLUDED_)
#define AFX_STDAFX_H_INCLUDED_

#include "helper.h"
    #ifndef unix
        #if _MSC_VER > 1000
            #pragma once
        #endif // _MSC_VER > 1000

        // Insert your headers here

//////////////////////////////////////////////////////////////////////
//	Tupperware Includes
//////////////////////////////////////////////////////////////////////

// General
#include "tupperware/aggregate.h"
#include "tupperware/loadsave.h"
#include "tupperware/tuparray.h"
#include "tupperware/tupstring.h"
#include <tupperware/TupSort.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>

// Attrib
//#include <tupperattrib/keyvalue.h>
//#include <tupperattrib/keyvaluecontext.h>
//#include <tupperattrib/keyselection.h>
//#include <tupperattrib/configroot.h>
//#include <tupperattrib/configchecklist.h>

// Support
#include <Tupperware/keeper.h>
#include <Tupper3d/objectconverter.h>

// Scene
#include <Tupper3d/scene.h>

// Object
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

// Node
#include <Tupper3d/node.h>
#include <Tupper3d/nodepool.h>

// Transform
#include <Tupper3d/transform.h>
#include <Tupper3d/staticmatrix.h>

// Material
#include <Tupper3d/material.h>
#include <Tupper3d/materialpool.h>
#include <Tupper3d/standardmaterial.h>
#include <Tupper3d/standardmaterialmap.h>
#include <Tupper3d/multisubobjectmaterial.h>
#include <Tupper3d/multisubobjectmaterialsubmaterial.h>

// Map
#include <Tupper3d/map.h>
#include <Tupper3d/mappool.h>
#include <Tupper3d/bitmapmap.h>
#include <Tupper3d/compositemap.h>
#include <Tupper3d/compositemapsubmap.h>

// Image
#include <Tupper3d/image.h>
#include <Tupper3d/imagepool.h>

// Modifier
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

#include <tupper3d/tupvertex.h>
#include <Tupper3d/objecthelper.h>

// image data (for image reader)
#include <Tupper3d/imageData.h>
#include <Tupper3d/imageDataFrame.h>
#include <Tupper3d/imageDataMap.h>
#include <Tupper3d/imageDataPalette.h>

// Animation
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

// Camera
#include <Tupper3d/camera.h>

// Light
#include <Tupper3d/light.h>

// Locator
#include <Tupper3d/locator.h>

extern const char *g_pExporterDate;
extern const char *g_pExporterTime;

    #endif // unix

#endif // AFX_STDAFX_H__58147ED2_DF67_426B_923B_532FF0D0DE6B__INCLUDED_
