/************************************/
/*                                  */
/* g_Bounds.h                       */
/* big juju prototype  11/18/99     */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* bounding boxes and spheres       */
/*                                  */
/************************************/

#ifndef __G_BOUNDS_H
#define __G_BOUNDS_H

#include "Math/Vector.h"

/******************* structures **************************/

/* a bounding box */

struct ts_BoundingBox
{
	Vector3Packed		Min;
	Vector3Packed		Max;
};

/* a bounding sphere */

struct ts_BoundingSphere
{
	Vector3Packed		Center;
	float			Radius;
};

/* combined bounding record */

struct ts_Bounds
{
	ts_BoundingSphere	BoundingSphere;
	ts_BoundingBox		BoundingBox;
};

/******************* global variables ********************/

/******************* global prototypes *******************/

void BoundingBox__Clear(ts_BoundingBox *pBox);
bool BoundingBox__IsEmpty(const ts_BoundingBox *pBox);
void BoundingBox__Include(ts_BoundingBox *pBox, Vector3CRef vPoint);
void BoundingBox__IncludeCount(ts_BoundingBox *pBox, const Vector3 *pPoint, const int nCount);
bool BoundingBox__ContainsPoint(const ts_BoundingBox *pBox, Vector3CRef vPoint);
bool BoundingBox__ContainsPointXZ(const ts_BoundingBox *pBox, Vector3CRef vPoint);
bool BoundingBox__ContainsBox(const ts_BoundingBox *pBox, const ts_BoundingBox *pContainedBox);
Vector3 BoundingBox__GetCenter(const ts_BoundingBox *pBox);
void BoundingBox__Scale(ts_BoundingBox *pBox, float fScale);
void BoundingBox__Union(const ts_BoundingBox *pBoxIn1, const ts_BoundingBox *pBoxIn2, ts_BoundingBox *pBoxOut);
bool BoundingBox__IntersectBox(const ts_BoundingBox *pBox1, const ts_BoundingBox *pBox2, ts_BoundingBox *pBoxOut = NULL);
bool BoundingBox__IntersectBoxXZ(const ts_BoundingBox *pBox1, const ts_BoundingBox *pBox2, ts_BoundingBox *pBoxOut = NULL);
bool BoundingBox__IntersectSphere(const ts_BoundingBox *pBox, const ts_BoundingSphere *pSphere);
bool BoundingBox__IntersectSphereXZ(const ts_BoundingBox *pBox, const ts_BoundingSphere *pSphere);
void BoundingBox__Transform(const ts_BoundingBox *pBoxIn, ts_BoundingBox *pBoxOut, const Graphics4x4 *m);
bool BoundingSphere__IntersectSphere(const ts_BoundingSphere *pSphere1, const ts_BoundingSphere *pSphere2);
float ScaleRadius(const Graphics4x4 *pMatrix, const float fRadius);
#ifndef CONSUMER_BUILD
void BoundingBox__Draw(const ts_BoundingBox *pBox);
#endif //CONSUMER_BUILD



#endif // __G_BOUNDS_H
