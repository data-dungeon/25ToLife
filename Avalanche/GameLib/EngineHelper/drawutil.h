/***************************************************************************/
// Simple drawing utility
/***************************************************************************/
#ifndef DRAWUTIL_H
#define DRAWUTIL_H

/* system includes */

/* engine includes */

#include "Layers/LayersStruct.h"
#include "Game/Database/Som.h"

/* game includes */

#include "linearAlgebra/rayseg.h"

// All static
class DrawUtility
{
public:
	// Here is how you specify a color
	typedef enum
		{
		RED,
		GREEN,
		BLUE,
		CYAN,
		MAGENTA,
		YELLOW,
		WHITE,
		BLACK,
		RED_ALPHA,
		GREEN_ALPHA,
		BLUE_ALPHA,
		CYAN_ALPHA,
		MAGENTA_ALPHA,
		YELLOW_ALPHA,
		WHITE_ALPHA,
		BLACK_ALPHA,
		NODRAW
		} COLOR;
	
	#ifndef CONSUMER_BUILD
		// draw types for the queue
		typedef enum
		{
			LINE,
			POINT,
			SPHERE,
			BOX,
			NORMAL,
			NORMALPLANE,
			TRIANGLE,
			LOZENGE,
		} DrawType;

		// Here are the handy-dandy functions
		static void QueueLine( Vector3CRef a, Vector3CRef b, COLOR c = WHITE, float lifetime = 0.0f );
		static void QueuePoint( Vector3CRef p, COLOR c = WHITE, float radius = 0.2f, float lifetime = 0.0f );
		static void QueueSphere( Vector3CRef a, COLOR c = WHITE, float radius = 0.2f, float lifetime = 0.0f );
		static void QueueBox( Vector3CRef a, Vector3CRef b, COLOR c = WHITE, float lifetime = 0.0f );
		static void QueueNormal( Vector3CRef origin, Vector3CRef dir, COLOR c = GREEN, float length = 0.75f, float lifetime = 0.0f );
		static void QueueNormalPlane( Vector3CRef origin, Vector3CRef dir, COLOR normalColor = GREEN, COLOR negNormalColor = RED, COLOR gridColor = CYAN, float length = 1.0f, float lifetime = 0.0f );
		static void QueueTriangle( Vector3CRef a, Vector3CRef b, Vector3CRef c, COLOR color = GREEN, float lifetime = 0.0f );
		static void QueueLozenge( Vector3CRef a, Vector3CRef b, float radius, COLOR color = WHITE, float lifetime = 0.0f );
	#endif // CONSUMER_BUILD

	static void FakeRay(const Ray &r, COLOR c = WHITE);
	static void Line(Vector3CRef a, Vector3CRef b, COLOR c = WHITE);
	static void Line(Vector3CRef a, Vector3CRef b, ts_fRGB fColor);
	static void Point(Vector3CRef p, COLOR c = WHITE, float radius = 0.20f);
	static void Sphere(Vector3CRef p, COLOR c = WHITE, float radius = 0.20f);
	static void Sphere(Vector3CRef p, ts_fRGB *fColor, float radius = 0.20f);
	static void Box(Vector3CRef a, Vector3CRef b, Matrix3x3* rot = NULL, Vector3* pos = NULL, COLOR c = WHITE);
	static void Box(Vector3CRef a, Vector3CRef b, const Graphics4x4 &tranform, COLOR c = WHITE);
	static void Normal(Vector3CRef origin, Vector3CRef dir, COLOR c = GREEN, float length = 0.75f);
	static void NormalPlane(Vector3CRef origin, Vector3CRef dir, COLOR normalColor = GREEN, COLOR negNormalColor = RED, COLOR gridColor = CYAN, float length = 1.0f);
	static void Arrow(Vector3CRef origin, Vector3CRef arrow, COLOR c = RED, float width = 0.10f);
	static void Tristrip(Graphics4x4 &transform, ts_TriStripHeader* triStrip, COLOR c = WHITE);
	static void Lozenge(Vector3CRef a, Vector3CRef b, float radius, COLOR c = WHITE);
	static void TriangleFace(Vector3CRef a, Vector3CRef b, Vector3CRef c, COLOR color = WHITE, float maxDist = 0.02f, float maxFrac = 0.5f);

	static void DrawQueue( void );
	static void ClearQueue( void );

private:
	// Convert a color into a hardware color
	static void Color(COLOR c, ts_bRGBA &rgba);
};

#endif
