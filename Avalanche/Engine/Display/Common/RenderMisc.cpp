/*************************************/
/*                                   */
/*   RenderMisc.c                    */
/*   new engine  04/11/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   miscellaneous render api        */
/*                                   */
/*************************************/

/* miscellaneous render api, including some development only stuff */

/******************* includes ****************************/

#include "Display/DisplayPCH.h"
#include "Math/fastrand.h"

#ifdef GCN
#include "Display/GCN/Shader.h"
#endif


/******************* defines *****************************/

#ifdef CDROM
#define LINE_BUFFER_SIZE	32
#define SPHERE_BUFFER_SIZE	16
#else
#define LINE_BUFFER_SIZE	3072
#define SPHERE_BUFFER_SIZE	512
#endif

/******************* macros ******************************/

/******************* structures **************************/

struct ts_Line
{
	Vector3Packed	a, b;
	ts_bRGBA			color;
};

struct ts_Sphere
{
	Vector3Packed	origin;
	float				radius;
	ts_bRGBA			color;
};

/******************* externs *****************************/

/******************* local variables ********************/

static Vector3 loop[ 12 ] =
{
	Vector3(  1.000f,  0.000f, 0.00f ),
	Vector3(  0.866f,  0.500f, 0.00f ),
	Vector3(  0.500f,  0.866f, 0.00f ),
	Vector3(  0.000f,  1.000f, 0.00f ),
	Vector3( -0.500f,  0.866f, 0.00f ),
	Vector3( -0.866f,  0.500f, 0.00f ),
	Vector3( -1.000f,  0.000f, 0.00f ),
	Vector3( -0.866f, -0.500f, 0.00f ),
	Vector3( -0.500f, -0.866f, 0.00f ),
	Vector3( -0.000f, -1.000f, 0.00f ),
	Vector3(  0.500f, -0.866f, 0.00f ),
	Vector3(  0.866f, -0.500f, 0.00f ),
};
#define NUM_LOOP_POINTS ( sizeof(loop) / sizeof(loop[0]) )

#define NUM_SPHERE_LOOPS 6

/******************* global variables ********************/

/******************* local variables *********************/

#if defined(WIN32) || defined(PS2) || defined(GCN)
ts_Line		lineBuffer[LINE_BUFFER_SIZE];
u32			lines = 0;
ts_Sphere	sphereBuffer[SPHERE_BUFFER_SIZE];
u32			spheres = 0;
#endif

/******************* local prototypes ********************/

#ifdef WIN32
int LineCompare(const void *elem1, const void *elem2);
#endif

/******************* functions ***************************/

/******************* development only ********************/

#ifndef CONSUMER_BUILD

//============================================================================
#ifdef WIN32
void _flushLines()
{
	u32 line;
	u32 color;

	// Anything to do?
	if (lines == 0)
		return;

	// Sort the list
	qsort(lineBuffer, lines, sizeof(ts_Line), LineCompare);

	// Set some states
	//RenderState__Request( RS_ZTEST, RS_FALSE );
	RenderState__Request( RS_CULL, RS_CULL_NONE );
	TextureState__Default();

	line = 0;
	while (line < lines)
	{
		// setup our color
		Draw__Color( lineBuffer[line].color.R / 255.0f,
					 lineBuffer[line].color.G / 255.0f,
					 lineBuffer[line].color.B / 255.0f,
					 lineBuffer[line].color.A / 255.0f );

		// Draw it
		Draw__Begin( D_LINES );

		// Draw by color
		color = *((u32 *)&lineBuffer[line].color);
		while ((line < lines) && (color == *((u32 *)&lineBuffer[line].color)))
		{
			Draw__Vector( lineBuffer[line].a.x(), lineBuffer[line].a.y(), lineBuffer[line].a.z() );
			Draw__Vector( lineBuffer[line].b.x(), lineBuffer[line].b.y(), lineBuffer[line].b.z() );
			line++;
		}

		Draw__End();
	}

	// Restore states
	RenderState__Default();

	// Flush the buffer
	lines = 0;
}
#endif

//============================================================================
#ifdef PS2
void _flushLines()
{
	Vector4 v1;
	Vector4 v2;

	// Anything to do?
	if (lines == 0)
		return;

	// Set some states
	RenderState__Request( RS_CULL, RS_CULL_NONE );

	// send init data..
	GSState__Set(SCE_GS_PRIM, SCE_GS_SET_PRIM(1, 0,0,0,0,0,0,0,0));

	TextureState__Default();

	// flush the current render state...
	GSState__Send();

	u32 startLine = 0;

// TJC - this really isnt the right value... its from the way the code
// used to draw the lines... but it'll work.. :)... we could go much
// higher with the new code, but Im not sure it would really be benificial...
#define MAX_LINES_PER_PASS ((MASTERLIST_QW_MAX_CHAIN_SIZE-6) * 2 / 3)

	while(lines)
	{
		// where to start, & how many lines this buffer
		ts_Line* currLine = &lineBuffer[startLine];
		int passLines = lines;
		if(passLines > MAX_LINES_PER_PASS) 
			passLines = MAX_LINES_PER_PASS;

		// move counts
		lines -= passLines;
		startLine += passLines;

		// how many qwords will we be sending
		int qwc = ((passLines * 3 + 1) >> 1) + 1;
			
		MasterList::SetFlushed(true);

		MasterList::AppendDMATAG(qwc+1);
//		if(MasterList::GetFlushed())
//		{
//			MasterList::Append(createNOP());
//		}
//		else
		{
			MasterList::Append(createFLUSH());
			MasterList::SetFlushed(true);
		}

		MasterList::Append(createDIRECT(qwc));
		MasterList::Append(
			createGIFTAG(
				passLines,	// nloop
				3,				// nreg
				0x551,		// regs
				true,			// eop
				GIFTag::FLG_REGLIST,
				SCE_GS_PRIM_LINE));
				
		for(;passLines;passLines--, currLine++)
		{
			// build the gs data...
			// just for fun... divide the color in assembly...
			// bad pairing and dependancies... but few instructions...
			union { ts_bRGBA c; u32 i; } color;
			color.c = currLine->color.PS2CorrectColor();
			MasterList::Append(color.i);
			MasterList::Append((u32)0);

			// do a little math on the point...
			// we steal values from the PS2Renderer to make this work...
			// since I know where those values are...
			
			const Matrix4x4 &m = *(Matrix4x4*)&PS2Renderer::GetWorldToScreen();
			Vector4 vtmp;
			v1 = currLine->a.Transform3(m);
			v1.w( (16.0f / v1.w()));
			v1 *= v1.w();
			v2 = currLine->b.Transform3(m);
			v2.w( (16.0f / v2.w()));
			v2 *= v2.w();
#define MAX_GS_POS 65535.0f
			if(v1.x() > MAX_GS_POS || v1.y() > MAX_GS_POS || v1.x() < 0.0f || v1.y() < 0.0f || v1.z() < 0.0f ||
				v2.x() > MAX_GS_POS || v2.y() > MAX_GS_POS || v2.x() < 0.0f || v2.y() < 0.0f || v2.z() < 0.0f )
			{
				MasterList::Append((u64)0);
				MasterList::Append((u64)0);
			}
			else
			{
				MasterList::Append(SCE_GS_SET_XYZ2( (s32)v1.x(), (s32)v1.y(), (s32)v1.z() ) );
				MasterList::Append(SCE_GS_SET_XYZ2( (s32)v2.x(), (s32)v2.y(), (s32)v2.z() ) );
			}
		}

		// if we did an odd # of lines, there is one dword of garbage at the end...
		if((u32)MasterList::CurrentDMAPointer() & 0xF)
		{
			MasterList::Append((u64)0);
		}

		MasterList::Append(createNOP());
		MasterList::Append(createNOP());
		MasterList::Append(createNOP());
		MasterList::Append(createFLUSH());
	}

	// Restore states
	RenderState__Default();

	// Flush the buffer
	lines = 0;
}
#endif

//============================================================================
#ifdef GCN
void _flushLines()
{
	
	u32 line;

	// Anything to do?
	if( lines == 0)
	{
		return;
	}

	static ShaderInst		*pShader = NULL;

	if( pShader == NULL)
	{
		ts_TextureContext		tc;

		tc.TextureCount = 0;

		pShader = g_ShaderMgr.GetShader( &tc, false);
	}

	// Set some states
//	RenderState__Request( RS_ZTEST, RS_FALSE);
	RenderState__Request( RS_CULL, RS_CULL_NONE);
	RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( 1.0f));

	g_ShaderMgr.UseShader( pShader);

	GXColor	gxColor;

	gxColor.r = 255;
	gxColor.g = 255;
	gxColor.b = 255;
	gxColor.a = 255;
	g_ShaderMgr.SetupLightingChannel( ShaderMgr::GCN_CHANNEL_HAS_VTXCOLOR, gxColor);

	GXSetLineWidth( 12, GX_TO_ZERO);

	GXClearVtxDesc( );

	//we always have position info
	GXSetVtxDesc( GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc( GX_VA_CLR0, GX_DIRECT);

	GXBegin(	GX_LINES, GX_VTXFMT0, lines * 2);

	line = 0;
	while( line < lines)
	{

		u8		r,
				g,
				b,
				a;

		r = (u8)lineBuffer[line].color.R;
		g = (u8)lineBuffer[line].color.G;
		b = (u8)lineBuffer[line].color.B;
		a = (u8)lineBuffer[line].color.A;

		GXPosition3f32( lineBuffer[line].a.x(), lineBuffer[line].a.y(), lineBuffer[line].a.z());
		GXColor4u8( r, g, b, a);

		GXPosition3f32( lineBuffer[line].b.x(), lineBuffer[line].b.y(), lineBuffer[line].b.z());
		GXColor4u8( r, g, b, a);

		line++;
	}

	GXEnd( );

	// Restore states
	RenderState__Default( );

	// Flush the buffer
	lines = 0;
}
#endif

//============================================================================

void drawBufferedSpheres()
{
	if ( spheres > 0 )
	{
		// build sphere centered at origin
		Graphics4x4		m;
		Vector3			v[ NUM_LOOP_POINTS * NUM_SPHERE_LOOPS ];
		int				i, j;

		// add all the hoops
		for ( i = 0 ; i < NUM_SPHERE_LOOPS ; i++ )
		{
			if ( i == NUM_SPHERE_LOOPS - 1 ) m.SetXRotation( Math::HalfPi );
			else                             m.SetYRotation( Math::Pi * i / ( NUM_SPHERE_LOOPS - 1 ) );
			m.TransformArray( loop, &v[ i * NUM_LOOP_POINTS ], NUM_LOOP_POINTS );
		}

		// for all the spheres add the lines
		for ( j = 0 ; j < (int)spheres ; j++ )
		{
			if ( lines + ( NUM_LOOP_POINTS * NUM_SPHERE_LOOPS ) >= LINE_BUFFER_SIZE )
				_flushLines();

			for ( i = 0 ; i < NUM_LOOP_POINTS * NUM_SPHERE_LOOPS ; i++ )
			{
				Vector3 a = v[ i ];
				Vector3 b = v[ ( i / NUM_LOOP_POINTS ) * NUM_LOOP_POINTS + ( ( i + 1 ) % NUM_LOOP_POINTS ) ];
				a = ( a * sphereBuffer[ j ].radius ) + sphereBuffer[ j ].origin;
				b = ( b * sphereBuffer[ j ].radius ) + sphereBuffer[ j ].origin;
				gfxDrawLineWC( &a, &b, &sphereBuffer[ j ].color );
			}
		}
		_flushLines();

		spheres = 0;
	}
}

void gfxDrawBufferedLines(void)
{
	// draw all queued up lines
	_flushLines();

	// draw all queued up spheres
	drawBufferedSpheres();
}

//============================================================================

#ifdef WIN32
int LineCompare(const void *elem1, const void *elem2)
{
	ts_Line *line1 = (ts_Line *)elem1;
	ts_Line *line2 = (ts_Line *)elem2;

	u32 *color1 = (u32 *)&line1->color;
	u32 *color2 = (u32 *)&line2->color;

	return(*color1 - *color2);
}
#endif

//============================================================================

// Throw away any buffered lines
void gfxClearLineBuffer()
{
#if defined(WIN32) || defined(PS2) || defined(GCN)
	lines = 0;
#endif
}

//============================================================================

// Draw a line segment specified in world coordinates
void gfxDrawLineWC(
const Vector3 *a,
const Vector3 *b,
ts_bRGBA *color)
{
#if defined(WIN32) || defined(PS2) || defined(GCN)
	//ASSERT(lines < LINE_BUFFER_SIZE);
	if (lines < LINE_BUFFER_SIZE)
	{
		ts_Line	*line = &lineBuffer[lines++];

		line->a = *a;
		line->b = *b;
		line->color = *color;
	}
#endif
}

//============================================================================

// Draw a line segment specified in model coordinates, model transform given by m
void gfxDrawLineMC(
Graphics4x4 *m,
Vector3 *a,
Vector3 *b,
ts_bRGBA *color)
{
#if defined(WIN32) || defined(PS2) || defined(GCN)
//yuch-- this will be pc-specific implementation
	Vector3 ma;
	Vector3 mb;

	ma = *a * *m;
	mb = *b * *m;

	gfxDrawLineWC(&ma, &mb, color);
#endif
}

//============================================================================

// Draw a triangle specified in world coordinates
void gfxDrawTriangleWC(Vector3 *a, Vector3 *b, Vector3 *c, ts_bRGBA *color)
{
#ifdef PS2
	gfxDrawLineWC(a, b, color);
	gfxDrawLineWC(b, c, color);
	gfxDrawLineWC(c, a, color);
#endif //PS2

//yuch-- this will be pc-specific implementation
#ifdef WIN32
// DX_TODO - jlb - doesn't work right now. DX BeginScene( ) not called yet
	return;

	// Set some states
	RenderState__Request( RS_POLYGONMODE, RS_POLYGONMODE_LINE );
	RenderState__Request( RS_CULL, RS_CULL_BACK );
	RenderState__Request( RS_ZTEST, RS_FALSE );
	RenderState__Request( RS_LIGHTING, RS_FALSE );
	RenderState__Request( RS_ZBIAS, RenderState__ftoi( -5.0f ) );
	RenderState__Request( RS_ZDIST, RenderState__ftoi( 1.0f ) );
	TextureState__Default();

//yuch-- i think this is necessary, since the triangles are put into display lists...SceneMgr calls these
// before it calls Buffcomm_Execute, but without the calls here the triangles get animation matrix results
// (they move with whatever is animating on screen). these calls weren't here before, so i'll leave them
// commented out
	// setup matrices-- world coords
//	MatrixStack__Mode( MS_MODELVIEW );
//	MatrixStack__Load( &g4WorldToCamera );

	// setup our color
	Draw__Color( color->R / 255.0f, color->G / 255.0f, color->B / 255.0f, color->A / 255.0f );

	// Draw it
	Draw__Begin( D_TRIANGLES );
	Draw__Vector( a->x(), a->y(), a->z() );
	Draw__Vector( b->x(), b->y(), b->z() );
	Draw__Vector( c->x(), c->y(), c->z() );
	Draw__End();

	// Restore states
	RenderState__Default();
#endif //WIN32
}

//============================================================================

// Draw a triangle segment specified in model coordinates, model transform given by m
void gfxDrawTriangleMC(Graphics4x4 *m, Vector3 *a, Vector3 *b, Vector3 *c, ts_bRGBA *color)
{
	Vector3 ma;
	Vector3 mb;
	Vector3 mc;

	ma = *a * *m;
	mb = *b * *m;
	mc = *c * *m;

	gfxDrawTriangleWC(&ma, &mb, &mc, color);
}

// Draw a quad specified in screen coordinates
void gfxDrawQuadSC(Vector2 *verts, ts_bRGBA *colors, Vector2 *textureUVs, int nTextureIndex)
{
//yuch-- this will be pc-specific implementation
#ifdef WIN32
	// setup attributes
	RenderState__Request( RS_POLYGONMODE, RS_POLYGONMODE_FILL );
	RenderState__Request( RS_CULL, RS_CULL_NONE );
	RenderState__Request( RS_ZWRITE, RS_FALSE );
	RenderState__Request( RS_ZTEST, RS_FALSE );
	TextureState__Default();

	// setup matrices
	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Identity();
	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Identity();

	// setup colors & render state
	LOCKUP(); // new texture manager doesnt deal in indicies..  what to do?
	TextureState__RequestAll( 0, TS_TYPE_2D, NULL, TS_METHOD_MODULATE, TS_WRAP_REPEAT, TS_WRAP_REPEAT, TS_FILTER_BILINEAR );

	// draw quad
	DrawArray__Enable( DA_VECTOR );
	DrawArray__Enable( DA_COLOR );
	DrawArray__Enable( DA_TEXTURE );
	DrawArray__Disable( DA_NORMAL );
	DrawArray__VectorPointer( 3, DA_FLOAT, 0, verts );
	DrawArray__ColorPointer( 4, DA_UBYTE, 0, colors );
	DrawArray__TexturePointer( 2, DA_FLOAT, 0, textureUVs );

	RenderState__Request( RS_PRIMITIVE, RS_PRIMITIVE_DRAWARRAY );
	RenderState__Begin();
	DrawArray__Render( DA_QUADS, 0, 1 );
	RenderState__End();

	// restore
	RenderState__Default();
#endif //WIN32
}

//============================================================================

// Draw a quad specified in normalized screen coordinates where the origin is in the
// upper left-hand corner with positive x-axis to the right and positive y-axis down.
void gfxDrawQuadSC1(Vector2 *verts, ts_bRGBA *colors, Vector2 *textureUVs, int nTextureIndex)
{
//yuch-- this will be pc-specific implementation
#ifdef WIN32
	// setup attributes
	RenderState__Request( RS_POLYGONMODE, RS_POLYGONMODE_FILL );
	RenderState__Request( RS_CULL, RS_CULL_NONE );
	RenderState__Request( RS_ZWRITE, RS_FALSE );
	RenderState__Request( RS_ZTEST, RS_FALSE );
	TextureState__Default();

	// setup matrices
	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Identity();
	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Identity();
	MatrixStack__Scale( 2.0f, -2.0f, 1.0f );
	MatrixStack__Translate( -0.5f, -0.5f, 0.0f );

	// setup colors & render state
	LOCKUP(); // new texture manager doesnt deal in indicies..  what to do?
	TextureState__RequestAll( 0, TS_TYPE_2D, NULL, TS_METHOD_MODULATE, TS_WRAP_REPEAT, TS_WRAP_REPEAT, TS_FILTER_BILINEAR );

	// draw quad
	DrawArray__Enable( DA_VECTOR );
	DrawArray__Enable( DA_COLOR );
	DrawArray__Enable( DA_TEXTURE );
	DrawArray__Disable( DA_NORMAL );
	DrawArray__VectorPointer( 3, DA_FLOAT, 0, verts );
	DrawArray__ColorPointer( 4, DA_UBYTE, 0, colors );
	DrawArray__TexturePointer( 2, DA_FLOAT, 0, textureUVs );

	RenderState__Request( RS_PRIMITIVE, RS_PRIMITIVE_DRAWARRAY );
	RenderState__Begin();
	DrawArray__Render( DA_QUADS, 0, 1 );
	RenderState__End();

	//glPopAttrib();
	RenderState__Default();
#endif //WIN32
}

//============================================================================

// Draw a "mark" centered at p
void gfxDrawMarkWC(
const Vector3 *p,
float radius,
ts_bRGBA *color)
{
#if defined(WIN32) || defined(PS2)
	float theta, phi;
	Vector3 v;
	int i;
	Vector3 newPoint;
	for (i = 0; i < 8; i++)
	{
		theta = g_random.Unit() * Math::TwoPi;
		phi = g_random.Unit() * Math::Pi;
		v.Set(
			radius * Math::FastSin(phi) * Math::FastCos(theta),
			radius * Math::FastSin(phi) * Math::FastSin(theta),
			radius * Math::FastCos(phi));
		newPoint = *p + v;
		gfxDrawLineWC(p, &newPoint, color);
	}
#endif //PS2

//yuch-- this will be pc-specific implementation
#if 0 //def WIN32
	int i;
	float theta, phi;
	float v[3];

	// Set some states
	RenderState__Request( RS_CULL, RS_CULL_NONE );
	RenderState__Request( RS_ZWRITE, RS_FALSE );
	TextureState__Default();

	// setup our color
	Draw__Color( color->R / 255.0f, color->G / 255.0f, color->B / 255.0f, color->A / 255.0f );

	// Draw it
	Draw__Begin( D_LINES );

	// Jesus this is stupid and slow
	for (i = 0; i < 8; i++)
	{
		theta = g_random.Unit() * Math::TwoPi;
		phi = g_random.Unit() * Math::Pi;
		v[0] = radius * Math::FastSin(phi) * Math::FastCos(theta);
		v[1] = radius * Math::FastSin(phi) * Math::FastSin(theta);
		v[2] = radius * Math::FastCos(phi);

		Draw__Vector( p->x(), p->y(), p->z() );
		Draw__Vector( p->x() + v[ 0 ], p->y() + v[ 1 ], p->z() + v[ 2 ] );
	}

	Draw__End();

	// Restore states
	RenderState__Default();
#endif //WIN32
}

//============================================================================

// Draw a "mark" centered at p
void gfxDrawMarkMC(
Graphics4x4 *m,
Vector3 *p,
float radius,
ts_bRGBA *color)
{
	Vector3 mp;

	mp = *p * *m;

	gfxDrawMarkWC(&mp, radius, color);
}

//============================================================================

// Draw a "sphere" centered at p
void gfxDrawSphereWC( const Vector3 *p, float radius, ts_bRGBA *color )
{
	if ( spheres < SPHERE_BUFFER_SIZE )
	{
		sphereBuffer[ spheres ].origin = *p;
		sphereBuffer[ spheres ].radius = radius;
		sphereBuffer[ spheres ].color  = *color;
		spheres++;
	}
}

//============================================================================

// Draw a "lozenge"
#define LOOP_MIDPOINT NUM_LOOP_POINTS / 2
void gfxDrawLozengeWC( const Vector3 *a, const Vector3 *b, float radius, ts_bRGBA *color )
{
	// Find a center and axis. If the endpoints are coincident, draw a sphere.
	Vector3			center = 0.5f * (*a + *b);
	Vector3			yAxis = *b - *a;
	if (!yAxis.SafeNormalize())
	{
		gfxDrawSphereWC(&center, radius, color);
		return;
	}

	// Let the x-axis be the cross product of the y-axis and the world axis it is
	// the "most perpendicular" to.
	Vector3			xAxis;
	Vector3			absAxis = yAxis.Abs();
	if (absAxis.X() < absAxis.Y())
	{
		if (absAxis.X() < absAxis.Z())
			xAxis.Set(0.0f, yAxis.Z(), -yAxis.Y());
		else
			xAxis.Set(yAxis.Y(), -yAxis.X(), 0.0f);
	}
	else if (absAxis.Y() < absAxis.Z())
		xAxis.Set(-yAxis.Z(), 0.0f, yAxis.X());
	else
		xAxis.Set(yAxis.Y(), -yAxis.X(), 0.0f);
	xAxis.Normalize();

	// Build a base transformation matrix.
	Graphics4x4		base;
	base.SetTranslation(center);
	base.SetRow0(xAxis);
	base.SetRow1(yAxis);
	base.SetRow2(xAxis.Cross(yAxis));

	int				i, j;
	Graphics4x4		m;
	Vector3			v[ NUM_LOOP_POINTS ];
	Vector3			height = *b - center;
	Vector3			twiceHeight = 2.0f * height;
	Vector3			side;

	// Draw longitudinal hoops
	for ( i = 0 ; i < 6 ; i++ )
	{
		//-- build transform matrix
		m.SetYRotation( Math::Pi * i / 6 );
		m.CatScale( radius, radius, radius );
		m = m * base;

		//-- transform
		m.TransformArray( loop, v, NUM_LOOP_POINTS );

		//-- draw line segments
		v[0] += height;
		for ( j = 0 ; j < LOOP_MIDPOINT ; j++ )
		{
			v[j + 1] += height;
			gfxDrawLineWC( &v[ j ], &v[ j + 1 ], color );
		}

		side = v[LOOP_MIDPOINT];
		v[LOOP_MIDPOINT] -= twiceHeight;
		gfxDrawLineWC( &side, &v[ LOOP_MIDPOINT ], color );

		for ( j = LOOP_MIDPOINT ; j < NUM_LOOP_POINTS - 1 ; j++ )
		{
			v[j + 1] -= height;
			gfxDrawLineWC( &v[ j ], &v[ j + 1 ], color );
		}

		side = v[0] - twiceHeight;
		gfxDrawLineWC( &v[ j ], &side, color );
		gfxDrawLineWC( &side, &v[ 0 ], color );
	}

	// Draw latitudinal hoops

	//-- build transform matrix
	m.SetXRotation( Math::HalfPi );
	m.CatScale( radius, radius, radius );
	m = m * base;

	//-- transform
	m.TransformArray( loop, v, NUM_LOOP_POINTS );

	//-- draw line segments
	v[0] += height;
	for ( j = 0 ; j < NUM_LOOP_POINTS - 1 ; j++ )
	{
		v[j + 1] += height;
		gfxDrawLineWC( &v[ j ], &v[ j + 1 ], color );
	}
	gfxDrawLineWC( &v[ j ], &v[ 0 ], color );

	v[0] -= twiceHeight;
	for ( j = 0 ; j < NUM_LOOP_POINTS - 1 ; j++ )
	{
		v[j + 1] -= twiceHeight;
		gfxDrawLineWC( &v[ j ], &v[ j + 1 ], color );
	}
	gfxDrawLineWC( &v[ j ], &v[ 0 ], color );
}

//============================================================================

void gfxDrawBox( ts_BoundingBox *pBox)
{
#ifdef PS2
	ts_bRGBA color;
	color.R = 0x80;
	color.G = 0x80;
	color.B = 0x80;
	color.A = 0x80;

	Vector3 WorkVector[8];

	WorkVector[0] = pBox->Min;
	
	WorkVector[1] = pBox->Min;
	WorkVector[1].z(pBox->Max.z());
	
	WorkVector[2] = pBox->Min;
	WorkVector[2].y(pBox->Max.y());
	
	WorkVector[3] = pBox->Max;
	WorkVector[3].x(pBox->Min.x());
	
	WorkVector[4] = pBox->Min;
	WorkVector[4].x(pBox->Max.x());
	
	WorkVector[5] = pBox->Max;
	WorkVector[5].y(pBox->Min.y());
	
	WorkVector[6] = pBox->Max;
	WorkVector[6].z(pBox->Min.z());
	
	WorkVector[7] = pBox->Max;
	
//	for (int j = 0; j < 8; j++)
//		WorkVector[j] = WorkVector[j] * mBoneToWorld;

	/* draw box. points 0..3 are min-x face, in z-order */

	ts_bRGBA LightBlue = {0, 255, 255, 255};
	gfxDrawLineWC(&WorkVector[0], &WorkVector[1], &color);
	gfxDrawLineWC(&WorkVector[1], &WorkVector[3], &color);
	gfxDrawLineWC(&WorkVector[3], &WorkVector[2], &color);
	gfxDrawLineWC(&WorkVector[2], &WorkVector[0], &color);
	
	/* draw box. points 4..7 are max-x face, in z-order */
	
	gfxDrawLineWC(&WorkVector[4], &WorkVector[5], &color);
	gfxDrawLineWC(&WorkVector[5], &WorkVector[7], &color);
	gfxDrawLineWC(&WorkVector[7], &WorkVector[6], &color);
	gfxDrawLineWC(&WorkVector[6], &WorkVector[4], &color);
	
	/* points in min-x and max-x faces correspond for remaining edges */
	
	gfxDrawLineWC(&WorkVector[0], &WorkVector[4], &color);
	gfxDrawLineWC(&WorkVector[1], &WorkVector[5], &color);
	gfxDrawLineWC(&WorkVector[2], &WorkVector[6], &color);
	gfxDrawLineWC(&WorkVector[3], &WorkVector[7], &color);
	

#else //PS2/WIN32

	int i;
	int type;

	RenderState__Request( RS_CULL, RS_CULL_NONE );
	RenderState__Request( RS_ZWRITE, RS_FALSE );

	TextureState__Default();

	for( i = 0; i < 2; i++ )
	{
		if( i == 0 )
		{
			type = D_QUADS;
			Draw__Color( 0.75f, 0.75f, 0.75f, 0.2f );
		}
		else
		{
			type = D_LINELOOP;
			Draw__Color( 1.0f, 1.0f, 1.0f, 1.0f );
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
#endif //PS2/WIN32
}

//============================================================================

void gfxDrawTristripMC(
Graphics4x4 *m,
Vector4 *v,
u16 count,
ts_bRGBA *color)
{
//yuch-- this will be pc-specific implementation
#ifdef WIN32
	int i;
	Vector3 p;

	// Set some states
	RenderState__Request( RS_CULL, RS_CULL_NONE );
	RenderState__Request( RS_ZWRITE, RS_FALSE );
	RenderState__Request( RS_POLYGONMODE, RS_POLYGONMODE_LINE );

	TextureState__Default();

	// setup our color
	Draw__Color( color->R / 255.0f, color->G / 255.0f, color->B / 255.0f, color->A / 255.0f );

	// Draw it

	Draw__Begin( D_TRISTRIP );

	for (i = 0; i < count; i++)
	{
		p = *((Vector3 *) &v[i]) * *m;
		Draw__Vector( p.x(), p.y(), p.z() );
	}

	Draw__End();

	// Restore states
	RenderState__Default();
#else
	for(int i = 2; i < count; i++)
	{
		gfxDrawTriangleMC(m, (Vector3*)&v[i-2], (Vector3*)&v[i-1], (Vector3*)&v[i], color);
	}
#endif //WIN32
}
#endif //CONSUMER_BUILD

//============================================================================

/* render a list of triangles, all same-colored, same-textured. this is useful for displaying text, as a more efficient
alternative to sprites */

void gfxRenderTriangles2D(t_Handle TextureSetHandle, u32 u32TextureIndex, ts_bRGBA *pColor, u32 u32TriangleCount, Vector4 *pVertexList, Vector2 *pUVList)
{
	LOCKUP(); // new texture manager doesnt deal in indicies..  what to do?

#if 0
#ifndef PS2
	ts_TextureSet *pTextureSet;

	/* quick exit */

	if (u32TriangleCount == 0)
		return;
	ASSERT(pVertexList);
	ASSERT(pUVList);

	/* default render state, and a couple of sprite-specific things */

	RenderState__Default();
	RenderState__Request( RS_ZTEST, RS_FALSE );	// ignore the z-buffer
	RenderState__Request( RS_PRIMITIVE, RS_PRIMITIVE_DRAWARRAY );
	RenderState__Request( RS_CULL, RS_CULL_NONE );

	/* save and clear projection matrix for 2d stuff */

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Push();
	MatrixStack__Identity();

	/* save and setup model matrix for 2d stuff */

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Push();
	MatrixStack__Identity();
	MatrixStack__Scale( 2.0f, -2.0f, 1.0f );
	MatrixStack__Translate( -0.5f, -0.5f, 0.0f );

	/* texture context */

	pTextureSet = (ts_TextureSet *)dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, TextureSetHandle);
	ASSERT(pTextureSet);
	TextureState__RequestAll( 0, TS_TYPE_2D, TextureMgr__GetBind(pTextureSet, u32TextureIndex), TS_METHOD_MODULATE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR );

	/* base color */

	RenderState__Request( RS_BASECOLOR, RenderState__Colorubv( ( unsigned char * )pColor ) );

	/* enable lists for glDrawArrays */

	DrawArray__Enable( DA_VECTOR );
	DrawArray__VectorPointer( 4, DA_FLOAT, 0, pVertexList );
#ifdef WIN32
	DrawArray__TextureUnit( 1 );
	DrawArray__Disable( DA_TEXTURE );
	DrawArray__TextureUnit( 0 );
#endif //WIN32
	DrawArray__Enable( DA_TEXTURE );
	DrawArray__TexturePointer( 2, DA_FLOAT, 0, pUVList );
	DrawArray__Disable( DA_COLOR );

	/* and draw */

	RenderState__Begin();
	DrawArray__Render( DA_TRIANGLES, 0, u32TriangleCount * 3 );
	RenderState__End();

	/* restore matrices */

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Pop();

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Pop();
#endif //PS2
#endif
}

//============================================================================
