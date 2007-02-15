/************************************/
/*                                  */
/* Bounds.c                         */
/* big juju prototype  11/18/99     */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* bounding boxes and spheres       */
/*                                  */
/************************************/

/* handles bounding checks via bounding boxes and spheres */

/******************* includes ****************************/

#include "Game/GamePCH.h"



/******************* defines *****************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

/* clear a bounding box */

void BoundingBox__Clear(ts_BoundingBox *pBox)
{
	pBox->Min.Set(MAXFLOAT32,MAXFLOAT32,MAXFLOAT32);
	pBox->Max = -pBox->Min;
}

/* is a bounding box empty? */

bool BoundingBox__IsEmpty(const ts_BoundingBox *pBox)
{
	return(pBox->Min.x() == MAXFLOAT32);
}

/* include a new point in a bounding box */

void BoundingBox__Include(ts_BoundingBox *pBox, Vector3CRef vPoint)
{
	ASSERT((vPoint.x() != MAXFLOAT32) && (vPoint.x() != -MAXFLOAT32));	// if this hits, you are probably expanding one bounding box to include another one (by including its corners) that's empty
	pBox->Min = Vector3::Min(pBox->Min, vPoint);
	pBox->Max = Vector3::Max(pBox->Max, vPoint);
}

/* include some new points in a bounding box */

void BoundingBox__IncludeCount(ts_BoundingBox *pBox, const Vector3 *pPoint, const int nCount)
{
	for (int i = 0; i < nCount; i++)
	{
		pBox->Min = Vector3::Min(pBox->Min, *pPoint);
		pBox->Max = Vector3::Max(pBox->Max, *pPoint);
		pPoint++;
	}
}

/* does box contain passed point? */

bool BoundingBox__ContainsPoint(const ts_BoundingBox *pBox, Vector3CRef vPoint)
{
	return((vPoint.x() >= pBox->Min.x()) &&
			 (vPoint.y() >= pBox->Min.y()) &&
			 (vPoint.z() >= pBox->Min.z()) &&
			 (vPoint.x() <= pBox->Max.x()) &&
			 (vPoint.y() <= pBox->Max.y()) &&
			 (vPoint.z() <= pBox->Max.z()));
}
bool BoundingBox__ContainsPointXZ(const ts_BoundingBox *pBox, Vector3CRef vPoint)
{
	return((vPoint.x() >= pBox->Min.x()) &&
			 (vPoint.z() >= pBox->Min.z()) &&
			 (vPoint.x() <= pBox->Max.x()) &&
			 (vPoint.z() <= pBox->Max.z()));
}

/* does box contain another box? */

bool BoundingBox__ContainsBox(const ts_BoundingBox *pBox, const ts_BoundingBox *pContainedBox)
{
	return ((BoundingBox__ContainsPoint(pBox, pContainedBox->Min)) &&
			  (BoundingBox__ContainsPoint(pBox, pContainedBox->Max)));
}

/* calculate the center of a bounding box */

Vector3 BoundingBox__GetCenter(const ts_BoundingBox *pBox)
{
	return (pBox->Min + pBox->Max) * 0.5f;
}

/* scale the size of a bounding box */

void BoundingBox__Scale(ts_BoundingBox *pBox, const float fScale)
{
	Vector3 vCenter = BoundingBox__GetCenter(pBox);
	Vector3 distance = (pBox->Max - vCenter) * fScale;

	pBox->Max = vCenter + distance;
	pBox->Min = vCenter - distance;
}

/* create a bounding box that includes both the passed bounding boxes */

void BoundingBox__Union(const ts_BoundingBox *pBoxIn1, const ts_BoundingBox *pBoxIn2, ts_BoundingBox *pBoxOut)
{
	pBoxOut->Min = Vector3::Min(pBoxIn1->Min, pBoxIn2->Min);
	pBoxOut->Max = Vector3::Max(pBoxIn1->Max, pBoxIn2->Max);
}

/* do two bounding boxes intersect? */

bool BoundingBox__IntersectBox(const ts_BoundingBox *pBox1, const ts_BoundingBox *pBox2, ts_BoundingBox *pBoxOut /*= NULL*/)
{
	if (pBox1->Max.x() < pBox2->Min.x())
		return(FALSE);
	if (pBox1->Min.x() > pBox2->Max.x())
		return(FALSE);
	if (pBox1->Max.z() < pBox2->Min.z())
		return(FALSE);
	if (pBox1->Min.z() > pBox2->Max.z())
		return(FALSE);

	/* maybe more likely to miss in X-Z plane, so do y test last */

	if (pBox1->Max.y() < pBox2->Min.y())
		return(FALSE);
	if (pBox1->Min.y() > pBox2->Max.y())
		return(FALSE);

	/* they intersect. do they want the intersection volume? */

	if (pBoxOut)
	{
		pBoxOut->Min = Vector3::Max(pBox1->Min, pBox2->Min);
		pBoxOut->Max = Vector3::Min(pBox1->Max, pBox2->Max);
	}
	return(TRUE);
}

/* do two bounding boxes intersect, x/z plane only? */

bool BoundingBox__IntersectBoxXZ(const ts_BoundingBox *pBox1, const ts_BoundingBox *pBox2, ts_BoundingBox *pBoxOut /*= NULL*/)
{
	if (pBox1->Max.x() < pBox2->Min.x())
		return(FALSE);
	if (pBox1->Min.x() > pBox2->Max.x())
		return(FALSE);
	if (pBox1->Max.z() < pBox2->Min.z())
		return(FALSE);
	if (pBox1->Min.z() > pBox2->Max.z())
		return(FALSE);

	/* they intersect. do they want the intersection volume? */

	if (pBoxOut)
	{
		*pBoxOut = *pBox1;
		if (pBox2->Min.x() > pBoxOut->Min.x())
			pBoxOut->Min.x( pBox2->Min.x());
		if (pBox2->Min.z() > pBoxOut->Min.z())
			pBoxOut->Min.z( pBox2->Min.z());
		if (pBox2->Max.x() < pBoxOut->Max.x())
			pBoxOut->Max.x( pBox2->Max.x());
		if (pBox2->Max.z() < pBoxOut->Max.z())
			pBoxOut->Max.z( pBox2->Max.z());
	}

	return(TRUE);
}

/* check box against sphere */

bool BoundingBox__IntersectSphere(const ts_BoundingBox *pBox, const ts_BoundingSphere *pSphere)
{
	Vector3 sphereMin = pSphere->Center - pSphere->Radius;
	Vector3 sphereMax = pSphere->Center + pSphere->Radius;


	if (pBox->Max.x() < sphereMin.x())	// sphere min x
		return(FALSE);
	if (pBox->Min.x() > sphereMax.x())	// sphere max x
		return(FALSE);
	if (pBox->Max.z() < sphereMin.z())	// sphere min z
		return(FALSE);
	if (pBox->Min.z() > sphereMax.z())	// sphere max z
		return(FALSE);

	/* maybe more likely to miss in X-Z plane, so do y test last */

	if (pBox->Max.y() < sphereMin.y())	// sphere min y
		return(FALSE);
	if (pBox->Min.y() > sphereMax.y())	// sphere max y
		return(FALSE);

	/* they intersect */

	return(TRUE);
}

/* check box against sphere, x/z plane only */

bool BoundingBox__IntersectSphereXZ(const ts_BoundingBox *pBox, const ts_BoundingSphere *pSphere)
{
	Vector3 sphereMin = pSphere->Center - pSphere->Radius;
	Vector3 sphereMax = pSphere->Center + pSphere->Radius;

	if (pBox->Max.x() < sphereMin.x())	// sphere min x
		return(FALSE);
	if (pBox->Min.x() > sphereMax.x())	// sphere max x
		return(FALSE);
	if (pBox->Max.z() < sphereMin.z())	// sphere min z
		return(FALSE);
	if (pBox->Min.z() > sphereMax.z())	// sphere max z
		return(FALSE);

	/* they intersect */

	return(TRUE);
}

/* transform all 8 corners of the passed bounding box and include them all in new bounding box */

void BoundingBox__Transform(const ts_BoundingBox *pBoxIn, ts_BoundingBox *pBoxOut, const Graphics4x4 *m)
{
	Vector3 vCorner;
	Vector3 vTransformed;

	BoundingBox__Clear(pBoxOut);

	// min, min, min

	vCorner = pBoxIn->Min;
	vTransformed = vCorner * *m;
	BoundingBox__Include(pBoxOut, vTransformed);

	// min, min, max

	vCorner.z( pBoxIn->Max.z() );
	vTransformed = vCorner * *m;
	BoundingBox__Include(pBoxOut, vTransformed);

	// min, max, min

	vCorner.y( pBoxIn->Min.y() );
	vCorner.z( pBoxIn->Max.z() );
	vTransformed = vCorner * *m;
	BoundingBox__Include(pBoxOut, vTransformed);
	
	// min, max, max

	vCorner.y( pBoxIn->Max.y() );
	vTransformed = vCorner * *m;
	BoundingBox__Include(pBoxOut, vTransformed);

	// max, min, min

	vCorner.x( pBoxIn->Max.x() );
	vCorner.y( pBoxIn->Min.y() );
	vCorner.z( pBoxIn->Min.z() );
	vTransformed = vCorner * *m;
	BoundingBox__Include(pBoxOut, vTransformed);
	
	// max, min, max

	vCorner.z( pBoxIn->Max.z() );
	vTransformed = vCorner * *m;
	BoundingBox__Include(pBoxOut, vTransformed);
	
	// max, max, min

	vCorner.y( pBoxIn->Min.y() );
	vCorner.z( pBoxIn->Max.z() );
	vTransformed = vCorner * *m;
	BoundingBox__Include(pBoxOut, vTransformed);
	
	// max, max, max

	vCorner.y( pBoxIn->Max.y() );
	vTransformed = vCorner * *m;
	BoundingBox__Include(pBoxOut, vTransformed);
}

/* check for intersection between two bounding spheres */

bool BoundingSphere__IntersectSphere(const ts_BoundingSphere *pSphere1, const ts_BoundingSphere *pSphere2)
{
	float	fDistance;
	
	/* if the distance between the two points is greater than the two radii added together, then fail immediately */

	ASSERT(pSphere1->Radius >= 0.0f);
	ASSERT(pSphere2->Radius >= 0.0f);
	fDistance = pSphere1->Radius + pSphere2->Radius;

	/* distance check. this used to have a check for each delta as it is calculated, but it was only faster if */
	/* the first delta was > fDistance, otherwise it was significantly slower-- avoid those branches! */

	Vector3 delta = pSphere1->Center - pSphere2->Center;
	float lensq = delta.LengthSquared();
	return(lensq <= fDistance * fDistance);
}

//transform a bounding sphere radius - don't care about rotations or translations - just scale by the largest scale factor

float ScaleRadius(const Graphics4x4 *pMatrix, const float fRadius)
{
	float fLargestScale;

	if( Math::Abs(pMatrix->r00) > Math::Abs(pMatrix->r11))
	{
		if( Math::Abs(pMatrix->r00) > Math::Abs(pMatrix->r22))
			fLargestScale = pMatrix->r00;
		else
			fLargestScale = pMatrix->r22;
	}
	else
	{
		if( Math::Abs(pMatrix->r11) > Math::Abs(pMatrix->r22))
			fLargestScale = pMatrix->r11;
		else
			fLargestScale = pMatrix->r22;
	}

	return (fRadius * Math::Abs(fLargestScale));
}

#ifndef CONSUMER_BUILD
#ifndef TOOLS
/* draw the bounding box for visualization purposes */

void BoundingBox__Draw(const ts_BoundingBox *pBox)
{
	/* on inspection, bounding boxes are 8-byte aligned... */

	static ts_bRGBA BoxColor[8] = {
		{255, 255, 255, 255}, 
		{  0, 255, 255, 255}, 
		{255,   0, 255, 255}, 
		{255, 255,   0, 255}, 
		{  0, 255,   0, 255}, 
		{255,   0,   0, 255}, 
		{  0,   0, 255, 255}, 
		{  0,   0,   0, 255}}; 
	ts_bRGBA *pColor = &BoxColor[(((u32) pBox) >> 3) & 7];	// base color on address so it's always the same
	float fRed =   (float) pColor->R / 255.0f;
	float fGreen = (float) pColor->G / 255.0f;
	float fBlue =  (float) pColor->B / 255.0f;

	RenderState__Request( RS_CULL, RS_CULL_NONE );
	RenderState__Request( RS_ZWRITE, RS_FALSE );

	TextureState__Default();

	for(int i = 0; i < 2; i++)
	{
		int type;
		if( i == 0 )
		{
			type = D_QUADS;
			Draw__Color( fRed * 0.75f, fGreen * 0.75f, fBlue * 0.75f, 0.2f );
		}
		else
		{
			type = D_LINELOOP;
			Draw__Color( fRed, fGreen, fBlue, 1.0f );
		}

  		//bottom
		Draw__Begin( type );
  		Draw__Vector( pBox->Min.x(), pBox->Min.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Min.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Min.y(), pBox->Max.z() );
  		Draw__Vector( pBox->Min.x(), pBox->Min.y(), pBox->Max.z() );
		Draw__End();

  		//top
		Draw__Begin( type );
  		Draw__Vector( pBox->Min.x(), pBox->Max.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Max.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Max.y(), pBox->Max.z() );
  		Draw__Vector( pBox->Min.x(), pBox->Max.y(), pBox->Max.z() );
		Draw__End();

  		//front
		Draw__Begin( type );
  		Draw__Vector( pBox->Min.x(), pBox->Min.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Min.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Max.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Min.x(), pBox->Max.y(), pBox->Min.z() );
		Draw__End();

  		//back
		Draw__Begin( type );
  		Draw__Vector( pBox->Min.x(), pBox->Min.y(), pBox->Max.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Min.y(), pBox->Max.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Max.y(), pBox->Max.z() );
  		Draw__Vector( pBox->Min.x(), pBox->Max.y(), pBox->Max.z() );
		Draw__End();

  		//left
		Draw__Begin( type );
  		Draw__Vector( pBox->Min.x(), pBox->Min.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Min.x(), pBox->Max.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Min.x(), pBox->Max.y(), pBox->Max.z() );
  		Draw__Vector( pBox->Min.x(), pBox->Min.y(), pBox->Max.z() );
		Draw__End();

  		//right
		Draw__Begin( type );
  		Draw__Vector( pBox->Max.x(), pBox->Min.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Max.y(), pBox->Min.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Max.y(), pBox->Max.z() );
  		Draw__Vector( pBox->Max.x(), pBox->Min.y(), pBox->Max.z() );
		Draw__End();
	}

	RenderState__Default();

#if 0
	/* the 8 corners */

	Vector3 vCorner[8];
	vCorner[0] = pBox->Min;					// min, min, min
	vCorner[1] = vCorner[0];
	vCorner[1].z(pBox->Max.z());			// min, min, max
	vCorner[2] = vCorner[1];
	vCorner[2].y(pBox->Max.y());			// min, max, max
	vCorner[3] = vCorner[2];
	vCorner[3].z(pBox->Min.z());			// min, max, min
	vCorner[4] = pBox->Min;
	vCorner[4].x(pBox->Max.x());			// max, min, min
	vCorner[5] = vCorner[4];
	vCorner[5].z(pBox->Max.z());			// max, min, max
	vCorner[6] = vCorner[5];
	vCorner[6].y(pBox->Max.y());			// max, max, max
	vCorner[7] = vCorner[6];
	vCorner[7].z(pBox->Min.z());			// max, max, min

	/* and draw-- here's the picture:
	
             +y         3--------------7
             ^        /              / |      
             |      /              /   |   
             |    /              /     |   
             |  /              /       |
              /              /         |
             2--------------6          |
             |              |          |
             |              |          |
             |          |   |          |
             |          0-  |          4
             |         /    |         /
             |              |       /
             |              |     /
             |              |   / 
             |              | /
             1--------------5    -------> +x
            /
          /
        /
      +z   
	
	
	*/

	ts_bRGBA Color = {0x80, 0, 0, 0x40};

	/* min x */

	gfxDrawTriangleWC(&vCorner[0], &vCorner[2], &vCorner[1], &Color);
	gfxDrawTriangleWC(&vCorner[0], &vCorner[3], &vCorner[2], &Color);

	/* min y */

	gfxDrawTriangleWC(&vCorner[0], &vCorner[1], &vCorner[5], &Color);
	gfxDrawTriangleWC(&vCorner[0], &vCorner[5], &vCorner[4], &Color);

	/* min z */

	gfxDrawTriangleWC(&vCorner[0], &vCorner[4], &vCorner[7], &Color);
	gfxDrawTriangleWC(&vCorner[0], &vCorner[7], &vCorner[3], &Color);

	/* max x */

	gfxDrawTriangleWC(&vCorner[4], &vCorner[5], &vCorner[6], &Color);
	gfxDrawTriangleWC(&vCorner[4], &vCorner[6], &vCorner[7], &Color);

	/* max y */

	gfxDrawTriangleWC(&vCorner[2], &vCorner[3], &vCorner[7], &Color);
	gfxDrawTriangleWC(&vCorner[2], &vCorner[7], &vCorner[6], &Color);

	/* max z */

	gfxDrawTriangleWC(&vCorner[1], &vCorner[2], &vCorner[6], &Color);
	gfxDrawTriangleWC(&vCorner[1], &vCorner[6], &vCorner[5], &Color);
#endif //0
}
#endif //TOOLS
#endif //CONSUMER_BUILD

