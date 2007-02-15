////////////////////////////////////////////////////////////////////////////
//
// DummyDrawUtility
//
// fake draw utility so we don't need to include the world....
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DummyDrawUtility.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 1:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * OBBWorld is now online... Just needs to get the node and face
 * attributes
 */

#include "Pch.h"

void DrawUtility::FakeRay(const Ray &r,COLOR c) {}
void DrawUtility::Line(const Vector3 &a,const Vector3 &b,COLOR c){}
void DrawUtility::Point(const Vector3 &p,COLOR c,float radius){}
void DrawUtility::Sphere(const Vector3 &p,COLOR c,float radius){}
void DrawUtility::Normal(const Vector3 &origin,const Vector3 &dir,COLOR c,float length){}
void DrawUtility::NormalPlane(const Vector3 &origin,const Vector3 &dir,COLOR normalColor,COLOR negNormalColor,COLOR gridColor,float length){}
void DrawUtility::Arrow(const Vector3 &origin,const Vector3 &arrow,COLOR c,float width){}
void DrawUtility::Tristrip(Graphics4x4 &transform,ts_TriStripHeader* triStrip,COLOR c){}
void DrawUtility::Color(COLOR c,ts_bRGBA &rgba){}

