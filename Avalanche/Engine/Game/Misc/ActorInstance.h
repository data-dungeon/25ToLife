//============================================================================
//=
//= ActorInstance.h
//=
//============================================================================

#ifndef _ACTOR_INSTANCE_H
#define _ACTOR_INSTANCE_H

#include "Game/Database/Instance.h"

struct ts_MotionFileHeader;
struct ts_MotionHeader;
class  MotionChannelTable;
class	 MotionChannel;
class  MotionNameHash;

//============================================================================

#define MAX_MOTION_FILES			4

//============================================================================

class CActorInstance : public CInstance
{
public:
	CActorInstance();
	~CActorInstance();

	void									Init( ts_Body* body, u32 maxChannels );

	void									SetPosition( Vector3CRef vPos );
	Vector3								GetPosition();
	void									SetRotation( Vector3CRef vRot );

	void									SetMaster( int index );
	int									GetMaster();

	float									GetCullRadius( void ) { return BoundingSphere().Radius; }
	void									SetCullRadius( float radius );

	void									SetMotionFile( int index, ts_MotionFileHeader* motionFile, MotionNameHash* nameTable );
	void									ClearMotionFile( int index );
	ts_MotionHeader*					LookupMotion( const char* name );

	inline MotionChannelTable*		GetMotionChannelTable() const { return d_motionChannelTable; }

public:
	// master/slave info
	int								d_master;									// which geom am I a slave of (-1 means not a slave)
	char								d_boneName[16];							// bone we are attached to (name) - ("" means no attached bone)
	Vector3							d_offset;									// custom offset from attached bone point
	Vector3							d_rotation;									// custom rotation from attached bone point

	// motion
	ts_MotionFileHeader*			d_motionFiles[ MAX_MOTION_FILES ];	// available Motion Data Base files
	MotionNameHash*				d_motionNames[ MAX_MOTION_FILES ];	// name lookup for motion file
	MotionChannelTable*			d_motionChannelTable;					//	ptr to channel table
};

//============================================================================

#endif // _ACTOR_INSTANCE_H
