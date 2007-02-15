/***************************************************************************/
// Simple drawing utility
/***************************************************************************/

/******************* includes ****************************/

#include "EngineHelper/EngineHelperPCH.h"
#include "EngineHelper/timer.h"


#ifndef CONSUMER_BUILD
class QueuedDraw
{
public:	

	void Draw( void );

	DrawUtility::DrawType type;
	Vector3 point[3];
	DrawUtility::COLOR color[3];
	float size;
	float lifetime;

	QueuedDraw *next;
	QueuedDraw *prev;
};

void QueuedDraw::Draw( void )
{
	switch( type )
	{
		case DrawUtility::LINE:
			DrawUtility::Line( point[0], point[1], color[0] );
			break;

		case DrawUtility::POINT:
			DrawUtility::Point( point[0], color[0], size );
			break;

		case DrawUtility::SPHERE:
			DrawUtility::Sphere( point[0], color[0], size );
			break;

		case DrawUtility::BOX:
			DrawUtility::Box( point[0], point[1], NULL, NULL, color[0] );
			break;

		case DrawUtility::NORMAL:
			DrawUtility::Normal( point[0], point[1], color[0], size );
			break;

		case DrawUtility::NORMALPLANE:
			DrawUtility::NormalPlane( point[0], point[1], color[0], color[1], color[2], size );
			break;

		case DrawUtility::TRIANGLE:
			DrawUtility::Line( point[0], point[1], color[0] );
			DrawUtility::Line( point[1], point[2], color[0] );
			DrawUtility::Line( point[2], point[0], color[0] );
			break;
		
		case DrawUtility::LOZENGE:
			DrawUtility::Lozenge( point[0], point[1], size, color[0] );
			break;
	}
}

DoublyLinkedList<QueuedDraw> DrawList;

void DrawUtility::QueueLine( Vector3CRef a, Vector3CRef b, COLOR c, float lifetime )
{
	QueuedDraw *draw = new QueuedDraw;
	draw->type = LINE;
	draw->point[0] = a;
	draw->point[1] = b;
	draw->color[0] = c;
	draw->lifetime = lifetime;
	DrawList.AddTail( draw );
}

void DrawUtility::QueuePoint( Vector3CRef p, COLOR c, float radius, float lifetime )
{
	QueuedDraw *draw = new QueuedDraw;
	draw->type = POINT;
	draw->point[0] = p;
	draw->color[0] = c;
	draw->size = radius;
	draw->lifetime = lifetime;
	DrawList.AddTail( draw );
}

void DrawUtility::QueueSphere( Vector3CRef a, COLOR c, float radius, float lifetime )
{
	QueuedDraw *draw = new QueuedDraw;
	draw->type = SPHERE;
	draw->point[0] = a;
	draw->color[0] = c;
	draw->size = radius;
	draw->lifetime = lifetime;
	DrawList.AddTail( draw );
}

void DrawUtility::QueueLozenge( Vector3CRef a, Vector3CRef b, float radius, COLOR color, float lifetime )
{
	QueuedDraw *draw = new QueuedDraw;
	draw->type = LOZENGE;
	draw->point[0] = a;
	draw->point[1] = b;
	draw->color[0] = color;
	draw->size = radius;
	draw->lifetime = lifetime;
	DrawList.AddTail( draw );
}

void DrawUtility::QueueBox( Vector3CRef a, Vector3CRef b, COLOR c, float lifetime )
{
	QueuedDraw *draw = new QueuedDraw;
	draw->type = BOX;
	draw->point[0] = a;
	draw->point[1] = b;
	draw->color[0] = c;
	draw->lifetime = lifetime;
	DrawList.AddTail( draw );
}

void DrawUtility::QueueNormal( Vector3CRef origin, Vector3CRef dir, COLOR c, float length, float lifetime )
{
	QueuedDraw *draw = new QueuedDraw;
	draw->type = NORMAL;
	draw->point[0] = origin;
	draw->point[1] = dir;
	draw->color[0] = c;
	draw->size = length;
	draw->lifetime = lifetime;
	DrawList.AddTail( draw );
}

void DrawUtility::QueueNormalPlane( Vector3CRef origin, Vector3CRef dir, COLOR normalColor, COLOR negNormalColor, COLOR gridColor, float length, float lifetime )
{
	QueuedDraw *draw = new QueuedDraw;
	draw->type = NORMALPLANE;
	draw->point[0] = origin;
	draw->point[1] = dir;
	draw->color[0] = normalColor;
	draw->color[1] = negNormalColor;
	draw->color[2] = gridColor;
	draw->size = length;
	draw->lifetime = lifetime;
	DrawList.AddTail( draw );
}

void DrawUtility::QueueTriangle( Vector3CRef a, Vector3CRef b, Vector3CRef c, COLOR color, float lifetime )
{
	QueuedDraw *draw = new QueuedDraw;
	draw->type = TRIANGLE;
	draw->point[0] = a;
	draw->point[1] = b;
	draw->point[2] = c;
	draw->color[0] = color;
	draw->lifetime = lifetime;
	DrawList.AddTail( draw );
}

void DrawUtility::DrawQueue( void )
{
		float dt = g_timer.GetFrameSec();
		QueuedDraw *draw = DrawList.Head();
		while( draw )
		{
			QueuedDraw *next = draw->next;
			draw->Draw();
			if( draw->lifetime != 0.0f )
			{
				draw->lifetime -= dt;
				if( draw->lifetime <= 0.0f )
					DrawList.Remove( draw );
			}
			draw = next;
		}
}

void DrawUtility::ClearQueue( void )
{
	DrawList.RemoveAll();
}
#endif


/***************************************************************************/
/***************************************************************************/
void DrawUtility::FakeRay(
const Ray &r,
COLOR c)
{
#ifndef CONSUMER_BUILD
	Vector3 a = r.o;

	float radius = 1000.0f * 2.0f;
	Vector3 b = r.o + r.dir * radius;

	ts_bRGBA color;
	Color(c, color);

	gfxDrawLineWC(&a, &b, &color);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Line(
Vector3CRef a,
Vector3CRef b,
COLOR c)
{
#ifndef CONSUMER_BUILD
	ts_bRGBA color;
	Color(c, color);

	gfxDrawLineWC(&a, &b, &color);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Line(
Vector3CRef a,
Vector3CRef b,
ts_fRGB fColor)
{
#ifndef CONSUMER_BUILD
	ts_bRGBA bColor;
	bColor.A = 255;
	bColor.R = (u8)(fColor.R * 255.0f);
	bColor.G = (u8)(fColor.G * 255.0f);
	bColor.B = (u8)(fColor.B * 255.0f);

	gfxDrawLineWC(&a, &b, &bColor);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Point(
Vector3CRef p,
COLOR c,
float radius)
{
#ifndef CONSUMER_BUILD
	ts_bRGBA color;
	Color(c, color);

	gfxDrawMarkWC(&p, radius, &color);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Sphere(
Vector3CRef p,
COLOR c,
float radius)
{
#ifndef CONSUMER_BUILD
	ts_bRGBA color;
	Color(c, color);

	gfxDrawSphereWC(&p, radius, &color);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Sphere(
Vector3CRef p,
ts_fRGB *fColor,
float radius)
{
#ifndef CONSUMER_BUILD
	ts_bRGBA bColor;

	bColor.A = 255;
	bColor.R = (u8)(fColor->R * 255.0f);
	bColor.G = (u8)(fColor->G * 255.0f);
	bColor.B = (u8)(fColor->B * 255.0f);

	gfxDrawSphereWC(&p, radius, &bColor);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Box(
Vector3CRef a,
Vector3CRef b,
Matrix3x3* rot,
Vector3* pos,
COLOR c)
{
#ifndef CONSUMER_BUILD
	Vector3 box[8];

	box[0].Set( a.x(),
					a.y(), 
					a.z() );

	box[1].Set( b.x(),
					a.y(),
					a.z() );

	box[2].Set( a.x(),
					b.y(),
					a.z() );

	box[3].Set( b.x(),
					b.y(),
					a.z() );

	box[4].Set( a.x(),
					a.y(),
					b.z() );

	box[5].Set( b.x(),
					a.y(),
					b.z() );

	box[6].Set( a.x(),
					b.y(),
					b.z() );

	box[7].Set( b.x(),
					b.y(),
					b.z() );

	int i;
	if (rot)
	{
		for (i = 0; i < 8; i++)
			box[i] = box[i] * (*rot);
	}

	if (pos)
	{
		for (i = 0; i < 8; i++)
			box[i] += (*pos);
	}

	Line(box[0], box[1], c);
	Line(box[1], box[3], c);
	Line(box[3], box[2], c);
	Line(box[2], box[0], c);
	Line(box[4], box[5], c);
	Line(box[5], box[7], c);
	Line(box[7], box[6], c);
	Line(box[6], box[4], c);
	Line(box[0], box[4], c);
	Line(box[1], box[5], c);
	Line(box[2], box[6], c);
	Line(box[3], box[7], c);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Box(
Vector3CRef a,
Vector3CRef b,
const Graphics4x4 &transform,
COLOR c)
{
#ifndef CONSUMER_BUILD
	Vector3 box[8];

	box[0].Set( a.x(),
					a.y(),
					a.z() );

	box[1].Set( b.x(),
					a.y(),
					a.z() );

	box[2].Set( a.x(),
					b.y(),
					a.z() );

	box[3].Set( b.x(),
					b.y(),
					a.z() );

	box[4].Set( a.x(),
					a.y(),
					b.z() );

	box[5].Set( b.x(),
					a.y(),
					b.z() );

	box[6].Set( a.x(),
					b.y(),
					b.z() );

	box[7].Set( b.x(),
					b.y(),
					b.z() );

	for (int i = 0; i < 8; i++)
		box[i] = box[i] * transform;

	Line(box[0], box[1], c);
	Line(box[1], box[3], c);
	Line(box[3], box[2], c);
	Line(box[2], box[0], c);
	Line(box[4], box[5], c);
	Line(box[5], box[7], c);
	Line(box[7], box[6], c);
	Line(box[6], box[4], c);
	Line(box[0], box[4], c);
	Line(box[1], box[5], c);
	Line(box[2], box[6], c);
	Line(box[3], box[7], c);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Normal(
Vector3CRef origin,
Vector3CRef dir,
COLOR c,
float length)
{
#ifndef CONSUMER_BUILD
	Vector3 endpoint = origin + dir * length;
	Line(origin, endpoint, c);

	Point(endpoint, c, 0.05f);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::NormalPlane(
Vector3CRef origin,
Vector3CRef dir,
COLOR normalColor,
COLOR negNormalColor,
COLOR gridColor,
float length)
{
#ifndef CONSUMER_BUILD
	Vector3 xAxis, yAxis, zAxis;
	float mag = Math::Sqrt(dir.y() * dir.y() + dir.z() * dir.z());
	if (Math::DivByZero(1.0f, mag))
	{
		mag = 1.0f / Math::Sqrt(dir.y() * dir.y() + dir.x() * dir.x());
		xAxis.Set(dir.y() * mag, -dir.x() * mag, 0.0f);
		zAxis = xAxis.Cross(dir);
	}
	else
	{
		mag = 1.0f / mag;
		zAxis.Set(0.0f, -dir.z() * mag, dir.y() * mag);
		xAxis = dir.Cross(zAxis);
	}

	xAxis *= length;
	yAxis = length * dir;
	zAxis *= length;

	Line(origin, origin + yAxis, normalColor);
	Line(origin, origin - yAxis, negNormalColor);
	Line(origin - xAxis, origin + xAxis, gridColor);
	Line(origin - zAxis, origin + zAxis, gridColor);
	Line(origin - zAxis - xAxis, origin - zAxis + xAxis, gridColor);
	Line(origin + zAxis - xAxis, origin + zAxis + xAxis, gridColor);
	Line(origin - zAxis - xAxis, origin + zAxis - xAxis, gridColor);
	Line(origin - zAxis + xAxis, origin + zAxis + xAxis, gridColor);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Arrow(
Vector3CRef origin,
Vector3CRef arrow,
COLOR c,
float width)
{
#ifndef CONSUMER_BUILD
	Vector3 endpoint = origin + arrow;
	Vector3 dir = arrow.Normalized();
	Vector3 side;
	if ((dir.y() > 0.9995f) || (dir.y() < -0.9995f))
		side.Set(0.0f, -dir.z(), dir.y());
	else
		side.Set(dir.z(), -dir.x(), 0.0f);
	side.Normalize();
	Vector3 up = dir.Cross(side);

	ts_bRGBA color;
	Color(c, color);

	gfxDrawLineWC(&origin, &endpoint, &color);

	Vector3 corner1, corner2;
	endpoint -= width * dir;
	width *= 0.5f;

	side *= width;
	corner1 = endpoint + side;
	corner2 = endpoint - side;
	gfxDrawTriangleWC(&endpoint, &corner1, &corner2, &color);

	up *= width;
	corner1 = endpoint + up;
	corner2 = endpoint - up;
	gfxDrawTriangleWC(&endpoint, &corner1, &corner2, &color);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Tristrip(
Graphics4x4 &transform,
ts_TriStripHeader* triStrip,
COLOR c)
{
#ifndef CONSUMER_BUILD
	ts_bRGBA color;
	Color(c, color);

	gfxDrawTristripMC(&transform, (Vector4 *) TRI_STRIP_SKIP_HEADER(triStrip), triStrip->u16PointCount, &color);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Lozenge(
Vector3CRef a,
Vector3CRef b,
float radius,
COLOR c)
{
#ifndef CONSUMER_BUILD
	ts_bRGBA color;
	Color(c, color);

	gfxDrawLozengeWC(&a, &b, radius, &color);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::TriangleFace(
Vector3CRef a,
Vector3CRef b,
Vector3CRef c,
COLOR color,
float maxDist,
float maxFrac)
{
#ifndef CONSUMER_BUILD
/*
 *	Transform color.
 */
	ts_bRGBA drawColor;
	Color(color, drawColor);
/*
 *	Find edge directions and lengths. If any side has zero length,
 *	just draw edges.
 */
	Vector3 edgeA = b - a;
	Vector3 edgeB = c - b;
	Vector3 edgeC = a - c;
	float sideA = edgeA.Length();
	float sideB = edgeB.Length();
	float sideC = edgeC.Length();
	if (Math::DivByZero(1.0f, sideA) ||
			Math::DivByZero(1.0f, sideB) ||
			Math::DivByZero(1.0f, sideC))
	{
		gfxDrawLineWC(&a, &b, &drawColor);
		gfxDrawLineWC(&b, &c, &drawColor);
		gfxDrawLineWC(&c, &a, &drawColor);
		return;
	}
	edgeA *= (1.0f / sideA);
	edgeB *= (1.0f / sideB);
	edgeC *= (1.0f / sideC);
/*
 *	Find the max fraction of the inradius.
 */
	float inradius = 0.5f * Math::Sqrt((sideB + sideC - sideA) *
			(sideC + sideA - sideB) * (sideA + sideB - sideC) /
			(sideA + sideB + sideC));
	if (maxFrac <= 0.0f)
		inradius = 0.0f;
	else if (maxFrac < 1.0f)
		inradius *= maxFrac;
/*
 *	Find the smallest of the max distance and max inradius fraction.
 */
	Vector3 border;
	if (maxDist < inradius)
		border.Set(0.0f, maxDist, 0.0f);
	else
		border.Set(0.0f, inradius, 0.0f);
/*
 *	Find the normal.
 */
	Vector3 normal;
	if (sideA < sideB)
	{
		if (sideA < sideC)
			normal = edgeC.Cross(edgeB.Negated());
		else
			normal = edgeB.Cross(edgeA.Negated());
	}
	else if (sideB < sideC)
		normal = edgeA.Cross(edgeC.Negated());
	else
		normal = edgeB.Cross(edgeA.Negated());
	normal.Normalize();
/*
 *	Setup the coefficient matrix.
 */
	Matrix3x3 coeff, coeffInv;
	coeff.SetColumn2(normal);
/*
 *	Do vertex A.
 */
	Vector3 insideA;
	coeff.SetColumn0(edgeA + edgeC);
	coeff.SetColumn1(normal.Cross(edgeA));
	if (coeff.Invert(coeffInv))
		insideA = a + border * coeffInv;
	else
		insideA = a;
/*
 *	Do vertex B.
 */
	Vector3 insideB;
	coeff.SetColumn0(edgeB + edgeA);
	coeff.SetColumn1(normal.Cross(edgeB));
	if (coeff.Invert(coeffInv))
		insideB = b + border * coeffInv;
	else
		insideB = b;;
/*
 *	Do vertex C.
 */
	Vector3 insideC;
	coeff.SetColumn0(edgeC + edgeB);
	coeff.SetColumn1(normal.Cross(edgeC));
	if (coeff.Invert(coeffInv))
		insideC = c + border * coeffInv;
	else
		insideC = c;
/*
 *	Draw.
 */
	gfxDrawLineWC(&a, &insideA, &drawColor);
	gfxDrawLineWC(&b, &insideB, &drawColor);
	gfxDrawLineWC(&c, &insideC, &drawColor);
	gfxDrawLineWC(&insideA, &insideB, &drawColor);
	gfxDrawLineWC(&insideB, &insideC, &drawColor);
	gfxDrawLineWC(&insideC, &insideA, &drawColor);
#endif
}

/***************************************************************************/
/***************************************************************************/
void DrawUtility::Color(
COLOR c,
ts_bRGBA &rgba)
{
#ifndef CONSUMER_BUILD
	memset(&rgba, '\0', sizeof(ts_bRGBA));
	rgba.A = 255;

	switch (c)
	{
		case RED:
			rgba.R = 255;
			break;
		case GREEN:
			rgba.G = 255;
			break;
		case BLUE:
			rgba.B = 255;
			break;
		case CYAN:
			rgba.G = rgba.B = 255;
			break;
		case MAGENTA:
			rgba.R = rgba.B = 255;
			break;
		case YELLOW:
			rgba.R = rgba.G = 255;
			break;
		case WHITE:
			rgba.R = 255;
			rgba.G = 255;
			rgba.B = 255;
			break;
		case BLACK:
			rgba.R = 0;
			rgba.G = 0;
			rgba.B = 0;
			break;
		case RED_ALPHA:
			rgba.R = 255;
			rgba.A = 64;
			break;
		case GREEN_ALPHA:
			rgba.G = 255;
			rgba.A = 64;
			break;
		case BLUE_ALPHA:
			rgba.B = 255;
			rgba.A = 64;
			break;
		case CYAN_ALPHA:
			rgba.G = rgba.B = 255;
			rgba.A = 64;
			break;
		case MAGENTA_ALPHA:
			rgba.R = rgba.B = 255;
			rgba.A = 64;
			break;
		case YELLOW_ALPHA:
			rgba.R = rgba.G = 255;
			rgba.A = 64;
			break;
		case WHITE_ALPHA:
			rgba.R = 255;
			rgba.G = 255;
			rgba.B = 255;
			rgba.A = 64;
			break;
		case BLACK_ALPHA:
			rgba.R = 0;
			rgba.G = 0;
			rgba.B = 0;
			rgba.A = 64;
			break;
		case NODRAW:
			rgba.R = 0;
			rgba.G = 0;
			rgba.B = 0;
			rgba.A = 0;
			break;
 	}
#endif
}
