//============================================================================
//=
//= ActorInstance.cpp
//=
//============================================================================

#include "Game/GamePCH.h"

//============================================================================

CActorInstance::CActorInstance()
{
	int i;

	for ( i = 0 ; i < MAX_MOTION_FILES ; i++ )
	{
		d_motionFiles[ i ] = 0;
		d_motionNames[ i ] = 0;
	}

	d_motionChannelTable = NULL;

	memset( &d_boneName, 0, sizeof(d_boneName) );

	d_offset.Clear();
	d_rotation.Clear();
}

//============================================================================

CActorInstance::~CActorInstance()
{
	if( d_motionChannelTable )
		delete d_motionChannelTable;

	SceneMgr::RemoveInstance( this );

	CInstance::Destroy();
}

//============================================================================

void CActorInstance::Init( ts_Body* body, u32 maxChannels )
{
	if ( body )
		d_motionChannelTable = new MotionChannelTable( maxChannels, body );
}

//============================================================================

int CActorInstance::GetMaster()
{
	return d_master;
}
//============================================================================

void CActorInstance::SetMaster( int index )
{
	d_master = index;
}

//============================================================================

void CActorInstance::SetPosition( Vector3CRef vPos )
{
	Matrix().SetTranslation( vPos );
	UpdateBoundingVolumes();
}

//============================================================================

void CActorInstance::SetRotation( Vector3CRef vRot )
{
	Vector3 vPos = Matrix().GetTranslation();
	Matrix().SetXYZRotation( vRot );
	Matrix().CatTranslation( vPos.x(), vPos.y(), vPos.z() );
	UpdateBoundingVolumes();
}

//============================================================================

Vector3 CActorInstance::GetPosition()
{
	return Matrix().GetTranslation();
}

//============================================================================

void CActorInstance::ClearMotionFile( int index )
{
	ASSERT( index >= 0 && index < MAX_MOTION_FILES );
	d_motionFiles[ index ] = 0;
	d_motionNames[ index ] = 0;
}

//============================================================================

void CActorInstance::SetMotionFile( int index, ts_MotionFileHeader* motionFile, MotionNameHash* nameTable )
{
	ASSERT( index >= 0 && index < MAX_MOTION_FILES );
	d_motionFiles[ index ] = motionFile;
	d_motionNames[ index ] = nameTable;
}

//============================================================================

ts_MotionHeader* CActorInstance::LookupMotion( const char* name )
{
	for ( int i = 0 ; i < MAX_MOTION_FILES ; i++ )
	{
		if ( d_motionNames[ i ] )
		{
			int id = d_motionNames[ i ]->LookupID( name );
			if ( id >= 0 )
			{
				ASSERT_PTR( d_motionFiles[ i ] );
				ts_MotionHeader* motHdr = GetMotionHeader( d_motionFiles[ i ], id );
				ASSERT_PTR( motHdr );
				return motHdr;
			}
		}
	}
	// didn't find it, return the first motion in the first file, just so we
	// don't crash
 	ASSERTF( 0, ( "motion name '%s' not found", name ) );
	return GetMotionHeader( d_motionFiles[0], 0 );
}

//============================================================================

void CActorInstance::SetCullRadius( float radius )
{
	BoundingSphere().Radius = radius;
	UpdateBoundingVolumes();
}

//============================================================================
