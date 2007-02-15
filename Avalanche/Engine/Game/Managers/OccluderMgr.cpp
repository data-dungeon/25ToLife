//============================================================================
//=
//= OccluderMgr - contains all visual occluder planes in the world.
//=               used for trimming instances out of the display pipeline
//=               which are obscured.
//=
//============================================================================

#include "Game/GamePCH.h"
#include "Game/Managers/OccluderMgr.h"

#define MAX_VU_OCCLUDERS 44

//============================================================================

COccluder::COccluder( Vector3CRef min, Vector3CRef max, Graphics4x4& orient )
{
	// this constructor assumes that this is a plane, not a box.  therefore there
	// is "short" axis that needs to be found and discarded.

	DirCos3x3 rot;
	Vector3 trans;
	float scaleX;
	float scaleY;
	float scaleZ;
	float shearXY;
	float shearXZ;
	float shearYZ;
	orient.Deconstruct( rot, trans, scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ );

	float dx = Math::Abs( min.x() - max.x() ) * scaleX;
	float dy = Math::Abs( min.y() - max.y() ) * scaleY;
	float dz = Math::Abs( min.z() - max.z() ) * scaleZ;

	if ( dx <= dy && dx <= dz )
	{
		d_point[ 0 ] = Vector3( ( min.x() + max.x() ) * 0.5f, min.y(), min.z() ) * orient;
		d_point[ 1 ] = Vector3( ( min.x() + max.x() ) * 0.5f, min.y(), max.z() ) * orient;
		d_point[ 2 ] = Vector3( ( min.x() + max.x() ) * 0.5f, max.y(), max.z() ) * orient;
		d_point[ 3 ] = Vector3( ( min.x() + max.x() ) * 0.5f, max.y(), min.z() ) * orient;
	}
	else if ( dy <= dx && dy <= dz )
	{
		d_point[ 0 ] = Vector3( min.x(), ( min.y() + max.y() ) * 0.5f, min.z() ) * orient;
		d_point[ 1 ] = Vector3( min.x(), ( min.y() + max.y() ) * 0.5f, max.z() ) * orient;
		d_point[ 2 ] = Vector3( max.x(), ( min.y() + max.y() ) * 0.5f, max.z() ) * orient;
		d_point[ 3 ] = Vector3( max.x(), ( min.y() + max.y() ) * 0.5f, min.z() ) * orient;
	}
	else
	{
		d_point[ 0 ] = Vector3( min.x(), min.y(), ( min.z() + max.z() ) * 0.5f ) * orient;
		d_point[ 1 ] = Vector3( min.x(), max.y(), ( min.z() + max.z() ) * 0.5f ) * orient;
		d_point[ 2 ] = Vector3( max.x(), max.y(), ( min.z() + max.z() ) * 0.5f ) * orient;
		d_point[ 3 ] = Vector3( max.x(), min.y(), ( min.z() + max.z() ) * 0.5f ) * orient;
	}
}

//============================================================================

void COccluder::Setup( Vector3CRef cam )
{
	// plane eq for each of the bounding planes
	d_plane[ 0 ].Set( cam, d_point[0], d_point[1] );
	d_plane[ 1 ].Set( cam, d_point[1], d_point[2] );
	d_plane[ 2 ].Set( cam, d_point[2], d_point[3] );
	d_plane[ 3 ].Set( cam, d_point[3], d_point[0] );
	d_plane[ 4 ].Set( d_point[ 0 ], d_point[ 1 ], d_point[ 2 ] );
	if ( d_plane[ 4 ].IsFacing( cam ) )
	{
		d_plane[ 0 ].Invert();
		d_plane[ 1 ].Invert();
		d_plane[ 2 ].Invert();
		d_plane[ 3 ].Invert();
		d_plane[ 4 ].Invert();
	}

	// remember camera pos, for drawing purposes
	//d_cameraPos = cam;
}

//============================================================================

bool COccluder::IsOccluded( Vector3CRef point ) const
{
	return d_plane[ 0 ].IsFacing( point ) &&
			 d_plane[ 1 ].IsFacing( point ) &&
			 d_plane[ 2 ].IsFacing( point ) &&
			 d_plane[ 3 ].IsFacing( point ) &&
			 d_plane[ 4 ].IsFacing( point );
}

//============================================================================

bool COccluder::IsOccluded( CInstance* instance ) const
{
	const ts_BoundingSphere& s = instance->BoundingSphere();
	return d_plane[ 0 ].IsFacing( s ) &&
			 d_plane[ 1 ].IsFacing( s ) &&
			 d_plane[ 2 ].IsFacing( s ) &&
			 d_plane[ 3 ].IsFacing( s ) &&
			 d_plane[ 4 ].IsFacing( s );
}

//============================================================================

void COccluder::Draw()
{
//	DrawUtility::Line( d_point[0], d_point[1], DrawUtility::RED );
//	DrawUtility::Line( d_point[1], d_point[2], DrawUtility::RED );
//	DrawUtility::Line( d_point[2], d_point[3], DrawUtility::RED );
//	DrawUtility::Line( d_point[3], d_point[0], DrawUtility::RED );
//	for ( int i = 0 ; i < 4 ; i++ )
//	{
//		Vector3 center = (d_point[i] + d_point[(i+1)%4]) * 0.5f;
//		DrawUtility::Line( center, center + d_plane[i].Normal()*2.0f, DrawUtility::YELLOW );
//		DrawUtility::Line( center, center + d_plane[4].Normal()*2.0f, DrawUtility::BLUE );
//	}
}

//============================================================================

const CPlane& COccluder::GetPlane(int index) const
{
	return d_plane[index];
}

//============================================================================

COccluderMgr::COccluderMgr() : d_occluders( 0, 4 )
{
}

//============================================================================

void COccluderMgr::Reset()
{
	d_occluders.Reset();
}

//============================================================================

void COccluderMgr::Add( const COccluder& occluder )
{
	d_occluders.Add( occluder );
}

//============================================================================

void COccluderMgr::Draw()
{
	for ( int i = 0 ; i < d_occluders.Count() ; i++ )
	{
		d_occluders[ i ].Draw();
	}
}

//============================================================================

void COccluderMgr::Setup( Vector3CRef cameraPos )
{
	for ( int i = 0 ; i < d_occluders.Count() ; i++ )
	{
		d_occluders[ i ].Setup( cameraPos );
	}

	#ifdef PS2
	SetupPS2Optimization();
	#endif
}

//============================================================================

bool COccluderMgr::IsOccluded( CInstance* instance )
{
	int i = 0;

	// TJC - temporarily remove this optimization as it seems to not work for at least
	// one person.
	#ifdef PS2
	const ts_BoundingSphere& s = instance->BoundingSphere();
	i = MAX_VU_OCCLUDERS;  // the uCode handles the first MAX_VU_OCCLUDERS planes.

	#ifdef CDROM
	if(PS2OptimizedIsOccluded(s))
		return true;
	// dont return false here because we need to fall through to the normal code.
	#else
	int themax = Math::Min( d_occluders.Count(),  MAX_VU_OCCLUDERS );
	if(PS2OptimizedIsOccluded(s))
	{
		for ( int j = 0 ; j < themax ; j++ )
		{
			if ( d_occluders[ j ].IsOccluded( instance ) )
			{
				return true;
			}
		}
		ASSERTS(false, "PS2 VU0 optimized occluder results dont match CPU occluders (A)");
	}
	else
	{
		for ( int j = 0 ; j < themax ; j++ )
		{
			if ( d_occluders[ j ].IsOccluded( instance ) )
			{
				ASSERTS(false, "PS2 VU0 optimized occluder results dont match CPU occluders (B)");
				return true;
			}
		}
	}
	#endif
	#endif

	for ( ; i < d_occluders.Count() ; i++ )
	{
		if ( d_occluders[ i ].IsOccluded( instance ) )
			return true;
	}
	return false;
}

//============================================================================

#ifdef PS2
u16 signs(Vector3 v)
{
	u16 result = 0;
	if(v.X() < 0.0f)
		result |= 0x80;
	if(v.Y() < 0.0f)
		result |= 0x40;
	if(v.Z() < 0.0f)
		result |= 0x20;
	if(v.W() < 0.0f)
		result |= 0x10;
	if(result != 0)
		result |= 0x02;
	return result;
}
bool COccluderMgr::PS2OptimizedIsOccluded( const ts_BoundingSphere& sphere )
{
	int occcount = ((Math::Min(d_occluders.Count(), MAX_VU_OCCLUDERS) + 3) >> 2) << 2;
	if(occcount == 0) return false;

	int results;
	{
		register u128 spherein asm("vf16");
		register u128 acc asm("vf0"); // this is a trick to help keep the compiler from mixing ACC accesses across routines.
		Vector4 spherevec( sphere.Center );
		spherevec.w( sphere.Radius );
		spherein = spherevec.vec();
	
		// this asm needs to be one block to keep the interlock from getting
		// a non-interlocking instruction before it.
		asm(
			"ctc2.i		%3,		vi01		\r\n"
			"vcallms		VU0_Occluder_Test_CodeStart  \r\n"
			"cfc2.i		%0,		vi02     \r\n" : "=r" (results), "=j" (acc), "+j" (spherein) : "r" (occcount));
//		dbgPrint( "VU0: %x\n",  results );
	}
	//#define TEST_C_SIMULATION
	#ifdef TEST_C_SIMULATION
	{
		int plane = 0;
		int retval = 0;
		int simulretval = 0;
		for ( ; occcount > 0 ; occcount -= 4 )
		{
			Vector4 t0 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
			Vector4 t1 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
			Vector4 t2 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
			Vector4 t3 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;
			Vector4 t4 = dat[plane++] * sphere.Center.x() + dat[plane++] * sphere.Center.y() + dat[plane++] * sphere.Center.z() + dat[plane++] - sphere.Radius;

			u16 t0flags = signs( t0 ) & 0x0002;
			u16 t1flags = signs( t1 ) & 0x0002;
			u16 t2flags = signs( t2 ) & 0x0002;
			u16 t3flags = signs( t3 ) & 0x0002;
			u16 sgns = signs( t4 );
			u16 t4flagsx = sgns & 0x0080;
			u16 t4flagsy = sgns & 0x0040;
			u16 t4flagsz = sgns & 0x0020;
			u16 t4flagsw = sgns & 0x0010;

			t0flags -= 0x0002;
			t1flags -= 0x0002;
			t2flags -= 0x0002;
			t3flags -= 0x0002;
			t4flagsx -= 0x0080;
			t4flagsy -= 0x0040;
			t4flagsz -= 0x0020;
			t4flagsw -= 0x0010;

			u16 occ0result = t0flags & t4flagsx;
			u16 occ1result = t1flags & t4flagsy & (1 - occcount);
			u16 occ2result = t2flags & t4flagsz & (2 - occcount);
			u16 occ3result = t3flags & t4flagsw & (3 - occcount);

			u16 occresult = occ0result | occ1result | occ2result | occ3result;

			simulretval |= occresult;

			if(
				(                   t0.x() < 0.0f || t0.y() < 0.0f || t0.z() < 0.0f || t0.w() < 0.0f || t4.x() < 0.0f) &&
				(1-occcount >= 0 || t1.x() < 0.0f || t1.y() < 0.0f || t1.z() < 0.0f || t1.w() < 0.0f || t4.y() < 0.0f) &&
				(2-occcount >= 0 || t2.x() < 0.0f || t2.y() < 0.0f || t2.z() < 0.0f || t2.w() < 0.0f || t4.z() < 0.0f) &&
				(3-occcount >= 0 || t3.x() < 0.0f || t3.y() < 0.0f || t3.z() < 0.0f || t3.w() < 0.0f || t4.w() < 0.0f))
				;
			else
				retval |= 0x8000;

		}
		//results = retval;
//		dbgPrint( "CPUSIM: %x\n", simulretval );
//		dbgPrint( "CPU: %x\n",  retval );
	}
	#endif
	return (results & 0x8000) == 0x8000;
}
#endif

//============================================================================

#ifdef TEST_C_SIMULATION
#define PACKEMUP(currpos, x,y,z,w) MasterList::Append4xF(currpos,x,y,z,w); dat[j++].Set(x,y,z,w);
#else
#define PACKEMUP(currpos, x,y,z,w) MasterList::Append4xF(currpos,x,y,z,w);
#endif

#ifdef PS2
void COccluderMgr::SetupPS2Optimization(void)
{
	// we load the occluders up to the vector unit to allow it to handle them faster.
	// they are DMAed up to uMem in here.
	// we only support the first 44 occluders through this optimized path.  beyond that
	// your back to the slow code... but that should handle most (if not all) of the
	// occluders for most worlds.
	// note that this can run over the number of occluders by up to 4.  Thats ok, since
	// the extra values will not get used.
	// we also dont touch the first 28 QWs of VU0 mem because its used by the frustum
	// culling code, which we need to coexist with.

	int realOccluderCount = d_occluders.Count();

	// # of occs uploaded to VU (rounded up to multiple of 4)
	int occcount = ((Math::Min(realOccluderCount, MAX_VU_OCCLUDERS) + 3) >> 2) << 2;
	if(occcount == 0) return;
	int qwc = occcount * 5;
	void* miscdata = MasterList::UseMiscData();
	ASSERT(MASTERLIST_CHUNK_SIZE >= qwc+1); // guess we cant even do 44 at a time anymore?
	unsigned char* currpos = (unsigned char*)miscdata;
	MasterList::Append(currpos, MasterList::createDMATAG(qwc, TAGID_END ));
	MasterList::Append(currpos, VIFInline::createNOP() );
	MasterList::Append(currpos, VIFInline::createUNPACK( 0x001C,  V4_32,  qwc ) );
	int j = 0;
	for ( int i = 0 ; i < occcount; i+=4 )
	{
		COccluder* occluder[4];
		for( int occ = 0; occ < 4; occ++ )
		{
			if(i+occ < realOccluderCount)
				occluder[occ] = &d_occluders[i+occ];
			else
				occluder[occ] = &d_occluders[realOccluderCount-1];
		}

		for (int occ = 0; occ < 4; occ++)
		{
			PACKEMUP(currpos, 
						occluder[occ]->GetPlane( 0 ).Normal().x(),
						occluder[occ]->GetPlane( 1 ).Normal().x(),
						occluder[occ]->GetPlane( 2 ).Normal().x(),
						occluder[occ]->GetPlane( 3 ).Normal().x());
			PACKEMUP(currpos,
						occluder[occ]->GetPlane( 0 ).Normal().y(),
						occluder[occ]->GetPlane( 1 ).Normal().y(),
						occluder[occ]->GetPlane( 2 ).Normal().y(),
						occluder[occ]->GetPlane( 3 ).Normal().y());
			PACKEMUP(currpos,
						occluder[occ]->GetPlane( 0 ).Normal().z(),
						occluder[occ]->GetPlane( 1 ).Normal().z(),
						occluder[occ]->GetPlane( 2 ).Normal().z(),
						occluder[occ]->GetPlane( 3 ).Normal().z());
			PACKEMUP(currpos,
						occluder[occ]->GetPlane( 0 ).D(),
						occluder[occ]->GetPlane( 1 ).D(),
						occluder[occ]->GetPlane( 2 ).D(),
						occluder[occ]->GetPlane( 3 ).D());
		}

		PACKEMUP(currpos,
					occluder[0]->GetPlane( 4 ).Normal().x(),
					occluder[1]->GetPlane( 4 ).Normal().x(),
					occluder[2]->GetPlane( 4 ).Normal().x(),
					occluder[3]->GetPlane( 4 ).Normal().x());
		PACKEMUP(currpos,
					occluder[0]->GetPlane( 4 ).Normal().y(),
					occluder[1]->GetPlane( 4 ).Normal().y(),
					occluder[2]->GetPlane( 4 ).Normal().y(),
					occluder[3]->GetPlane( 4 ).Normal().y());
		PACKEMUP(currpos,
					occluder[0]->GetPlane( 4 ).Normal().z(),
					occluder[1]->GetPlane( 4 ).Normal().z(),
					occluder[2]->GetPlane( 4 ).Normal().z(),
					occluder[3]->GetPlane( 4 ).Normal().z());
		PACKEMUP(currpos,
					occluder[0]->GetPlane( 4 ).D(),
					occluder[1]->GetPlane( 4 ).D(),
					occluder[2]->GetPlane( 4 ).D(),
					occluder[3]->GetPlane( 4 ).D());
	}
	*D0_TADR = ( ( ( u32 ) miscdata ) & 0x0FFFFFFF ) | 0x80000000;
	*D0_QWC = 0;
	*D0_CHCR = D_CHCR_SCHAIN_MODE_M | D_CHCR_DIR_M | D_CHCR_STR_M | D_CHCR_TTE_M;
	DMA_Wait( D0_CHCR );
}
#endif

