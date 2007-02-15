/*
**
**  File:   cactor.h
**  Date:   March 28, 2000
**  By:     Bryant Collard
**  Desc:   Basic CActor class.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/stage/cactor.h $
**  $Revision: #321 $
**  $DateTime: 2005/08/05 03:07:10 $
**    $Author: Rob $
**
*/

#ifndef CACTOR_H
#define CACTOR_H

#define MAX_ACTOR_INSTANCES 4

#if defined ( _DEBUG ) && defined ( WIN32 )
#define ENABLE_RECORD
#endif

#include <stdarg.h>
#include "platform/BaseType.h"
#include "Math/Quatern.h"
#include "Math/Matrix.h"
#include "Game/Database/Som.h"
#include "Game/Misc/ActorInstance.h"
#include "Game/Database/Som.h"
#include "stage/CActorSoundEmitter.h"
#include "collide/aabb.h"
#include "Game/Animation/MotionDB.h"
#include "stage/StageEntity.h"
#include "Game/Managers/replayMgr.h"
#include "GameHelper/Message.h"
#include "Game/Animation/Motion.h"
#include "Game/Misc/ParameterSet.h"
#include "stage/ActorDefines.h"
#include "Game/Database/DynamicObject.h"
#include "GameHelper/Handle.h"

class CCastMember;
class LoadSOM;
class CActorChains;
class CMeshList;
class XZRect;
class CLightInfluences;
class CObjectList;
class	MotionChannel;
class CNetActor;
class CInterface;
class ScriptVM;

// --- defines used for Save/Load game
#define LOAD_DATA false
#define SAVE_DATA true
#define HIDDEN_BIT 0x000000001
#define MAX_SKELETONS_PER_ACTOR	3
enum {
			PRIMARY_SKELETON = 0,
			SECONDARY_SKELETON,
			TERTIARY_SKELETON,
			MAX_SKELETONS,
};

#define ACTOR_HANDLE_MASK_WIDTH  7
handledef(CActor) ActorHandle;

class CActor : public CStageEntity
{
  public:
	CActor(void);
	CActor(const CEntityType &i_type);
	virtual ~CActor(void);
	static void Destroy(CActor* i_actor);

	// Geometry system accessors
	bool SetGeometryFromWorldInstance( int index, const char *modelName, int master = -1 );
	bool SetGeometry( int index, ts_Geometry* i_geometry, int master = -1 );
	void SetSkeletonIndices (int primary = -1, int secondary = -1, int tertiary = -1);
	void SetSkeletonIndex (int WhichSkeleton, int GeomIndex);
	int  GetSkeletonIndex (int WhichSkeleton);
	bool SetGeometryAttachBone( int index, const char *boneName );
	bool SetGeometryOffset ( int index, const Vector3 &pos );
	bool SetGeometryRotation ( int index, const Vector3 &rot );
	CActor* DetachGeometryWithNewActor( int DetachIndex, int AttachIndex = 0 );
	bool DetachGeometry ( int DetachIndex, int Assert = 1 );
	void UpdateGeometrySlaves();
	void InitializeGeometry(int i_index);
	virtual bool Initialize(void);
	virtual void InitializeVisualProperties(void) {} 
	void UpdateGeomBoundingVolumes();

	// Handle
	ActorHandle GetHandle(void) const {return(d_handleCurator.Get());}
	operator ActorHandle() const {return(d_handleCurator.Get());}
	static CActor* FromHandle(ActorHandle i_handle)
		{
			return CHandleCurator<CActor, ACTOR_HANDLE_MASK_WIDTH>::Retrieve(i_handle);
		}
	static ActorHandle ToHandle(const CActor* i_actor)
		{
			return((i_actor != NULL) ?
					i_actor->GetHandle() : (ActorHandle)INVALID_OBJECT_HANDLE);
		}
//	static ActorHandle ActiveHandle(ActorHandle i_handle)
//		{
//			return(CHandleCurator<CActor, ACTOR_HANDLE_MASK_WIDTH>::Retrieve(
//					i_handle) != NULL ? i_handle : (ActorHandle)INVALID_OBJECT_HANDLE);
//		}

	// Mark for deletion at the end of the frame, or after some number of seconds
	void MarkForDeath(float fSeconds = 0.0f);
	bool MarkedForDeath(void)						{ return(d_markedForDeath && (d_lienCount <= 0)); }
	void SetLifetime(float fSeconds)				{ d_fLifetime = fSeconds; }
	float Lifetime( void ) const              { return( d_fLifetime ); }
	virtual bool ConfirmDeath()					{ return(true); }	// can be overridden to prevent death and do something else instead, like reset
	void SetLien(void)								{ d_lienCount++; }
	void ClearLien(void)								{ if (--d_lienCount < 0) d_lienCount = 0; }

	virtual void Placed(void);
	void Place(float i_x, float i_z, float i_heading);
	void Place(Vector3CRef i_pos, float i_heading );
	void Place(Vector3CRef i_pos, Vector3CRef i_euler);
	void Place(Vector3CRef i_pos, const DirCos3x3 &i_orientation);
	void Place(const Graphics4x4 &i_transformation);

	inline const Vector3& GetBodyInWorld(void) const {return(d_bodyInWorld);}
	inline const DirCos3x3& GetBodyToWorld(void) const {return(d_bodyToWorld);}
	inline Vector3& GetBodyInWorld(void) {return(d_bodyInWorld);}
	inline DirCos3x3& GetBodyToWorld(void) {return(d_bodyToWorld);}
	inline Vector3& GetVelocityInWorld(void) {return(d_velocityInWorld);}
	inline Vector3& GetAngVelocityInBody(void) {return(d_omegaInBody);}
	inline Vector3& Scale(void) {return(d_scale);}
	virtual Vector3& GetFocusInWorld(void) {return(d_bodyInWorld);}

	CMessageOwner &GetMessageOwner(void) {return(d_messageOwner);}

	// Get at interfaces
	virtual CInterface* GetInterface(uint i_id) { return NULL; }

	// Geometry and Instance access
	inline CActorInstance* Instance( int index = 0 ) { ASSERT( index >= 0 && index < MAX_ACTOR_INSTANCES ); return d_instance[ index ]; }

	// Get our AABB
	inline AABB& GetAABB()
		{ return (AABB&) Instance( d_skeletons[PRIMARY_SKELETON] )->BoundingBox(); }

	// Made virtual so actors can react to changes in status.
	virtual void Hide( bool flag = false )
	{
		for ( int i = 0 ; i < MAX_ACTOR_INSTANCES ; i++ )
			if ( Instance( i ) )
				Instance( i )->Hide( flag );
	}
	virtual bool Hidden(bool justRoot = true)
	{
		int count = justRoot ? 1 : MAX_ACTOR_INSTANCES;
		for ( int i = 0 ; i < count ; i++ )
			if ( Instance( i ) && !Instance(i)->Hidden())
				return false;
		return true;
	}

	// Get at our main matrix (local to world)
	inline Graphics4x4 &LocalToWorld( int index = 0 )
		{ return Instance(index)->Matrix(); }

	// Get at the base actor, and possible associated mesh list
	inline CMeshList*	MeshList(void)	{ return(d_meshList); }

	// Scripts
	inline ScriptVM* Script( int scriptIndex ) const { return d_script[ scriptIndex ]; }
	inline void SetScript( int scriptIndex, ScriptVM* script ) { d_script[ scriptIndex ] = script; }
	inline u32 GetActionFlags() const { return d_actionFlags; }
	inline void SetActionFlags( u32 flags ) { d_actionFlags = flags; }

	// Get at our instance name
	inline const char* InstanceName( int index = 0 )
		{ return Instance(index)->Name(); }
	
	// motion
	bool LoadMotion( const char* motionFile, int geomIndex, int motionDBIndex );
	bool LoadAnimScript( const char* scriptFile, int scriptIndex );

	// Animation and bone stuff
	Graphics4x4 GetSubMatrix(int iSubCount, int index = 0);
	void SetSubMatrix(int iSubCount, Graphics4x4 &g4InputMatrix, int index =0);
	bool GetBone(char *boneName, BoneID &boneID, int index = 0);
	bool GetMatchBone(char *boneName, BoneID &boneID, int index = 0);
	ts_Bone *GetBone(BoneID boneID, int index = 0);
	ts_Bone *GetBone(char *boneName, int index = 0);
	ts_ModelBone *GetModelBone(BoneID boneID, int index = 0);
	void SetBoneMatrix(BoneID boneID, Matrix3x3 &g3NewMatrix);
	void SetBoneMatrix(BoneID boneID, Graphics4x4 &g4NewMatrix);
	void SetBoneMatrix(BoneID boneID, Matrix3x3 &g3NewMatrix,
			float rPercentage);		// slerp versions
	void SetBoneMatrix(BoneID boneID, Graphics4x4 &g4NewMatrix,
			float rPercentage);		// slerp versions
	void SetBoneMatrix(ts_Quatvw &tsqFrom, BoneID boneID,
			Matrix3x3 &g3NewMatrix, float rMorphPct);
	void SetBoneMatrix(ts_Quatvw &tsqFrom, BoneID boneID,
			Graphics4x4 &g4NewMatrix, float rMorphPct);
	void SetBoneMatrix(ts_Quatvw &tsqFrom, BoneID boneID,
			Matrix3x3 &g3NewMatrix, float rPercentage, float rMorphPct);//slerp
	void SetBoneMatrix(ts_Quatvw &tsqFrom, BoneID boneID,
			Graphics4x4 &g4NewMatrix, float rPercentage, float rMorphPct);//slerp
	void GetBoneCompositeMatrix(BoneID boneID, Graphics4x4 &g4OutputMatrix, int index = 0);
	void GetBoneCompositeMatrix(BoneID boneID, Matrix3x3 &g3OutputMatrix, int index = 0);
	void GetParentBoneCompositeMatrix(BoneID boneID, Graphics4x4 &g4OutputMatrix, int index = 0);
	void GetParentBoneCompositeMatrix(BoneID boneID, Matrix3x3 &g3OutputMatrix, int index = 0);
	void GetBoneToModel(BoneID boneID, Graphics4x4 &g4OutputMatrix, int index = 0);
	void GetBoneToModel(BoneID boneID, Matrix3x3 &g3OutputMatrix, int index = 0);
	Vector3 GetEndPosition(BoneID boneID, int index = 0);
	Vector3 GetStartPosition(BoneID boneID, int index = 0);
	Vector3 PointLocalToWorld(Vector3CRef v3LocalPoint);
	Vector3 PointWorldToLocal(Vector3CRef v3WorldPoint);
	Vector3 PointModelToWorld(Vector3CRef v3ModelPoint);
	DirCos3x3 GetRelativeMatrix(BoneID boneID, const Matrix3x3 &g3Absolute);
	Vector3 GetBoneWorldEndPosition(BoneID boneID, Vector3CRef v3Offset);
	Vector3 GetBoneWorldEndPosition(BoneID boneID);
	Vector3 GetBoneWorldStartPosition(BoneID boneID, Vector3CRef v3Offset);
	Vector3 GetBoneWorldStartPosition(BoneID boneID);
	void SetBoneVisibility(BoneID boneID, bool bVisibility);
	bool GetBoneVisibility(BoneID boneID);
	void SetProceduralPosition(float zeroToOne, int motionIndex = MOTION1);
	void SetSecondaryProceduralPosition(float zeroToOne);

	// Play a motion
	ts_MotnInf *GetMotnInf( int index = MOTION1 ) { ASSERT( index >= 0 && index < MAX_MOTIONS ); return &d_motnInf[index]; }
	MotionChannel* GetMotionChannel( int index = MOTION1 ) { ASSERT( index >= 0 && index < MAX_MOTIONS ); return d_motnInf[ index ].currentChannel; }
	const char *GetMotionString( const char *name, int index = MOTION1 );
	float GetMotionFloat( const char *name, int index = MOTION1 );
	int GetMotionInt( const char *name, int index = MOTION1 );

	void StartMotion( const char* name, s32 startFrame, u32 flags, u32 boneGroupMask = 0xffffffff );
	void StartMotion( ts_MotionHeader* motHdr, s32 startFrame, u32 flags, u32 boneGroupMask = 0xffffffff );

	void StopMotion( const char* name, int motionIndex = MOTION1 );
	void StopMotion( ts_MotionHeader* motHdr, int motionIndex = MOTION1 );

	void EnableMotion( int motionIndex = MOTION1 ) { ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS ); motionDisabled[motionIndex] = false; }
	void DisableMotion( int motionIndex = MOTION1 ) { ASSERT( motionIndex >= 0 && motionIndex < MAX_MOTIONS ); motionDisabled[motionIndex] = true; }
	bool StartAnimation( const char* actionName, bool force = false, bool process = false, int nScriptIndex = ANIM1_SCRIPT );

	void SetAnimationRate(float i_animationRate, int i_motionIndex = MOTION1)
	{
		ASSERT( i_motionIndex >= MOTION1 && i_motionIndex < MAX_MOTIONS );
		CMotion__SetAnimationRate( &d_motnInf[i_motionIndex], i_animationRate );
	}

	float GetAnimationRate(int i_motionIndex = MOTION1)
	{
		ASSERT( i_motionIndex >= MOTION1 && i_motionIndex < MAX_MOTIONS );
		return CMotion__GetAnimationRate( &d_motnInf[i_motionIndex] );
	}

	s16 AnimAction( void );
	s16 AnimActionSecondary( int anim );

	// Access to sound objects

	// there are two types of emitters, an sfx emitter and a voice emitter.  
	// the sfx emitter is for general sound-effect sounds, such as 
	// interactions with environment and/or actions that the actor has.
	// the voice emitter is for speech, or any other sound that should not
	// be interrupted and is important informationally.

	// an sfx emitter (a polyphonic, multiple channel emitter).
	CActorSoundEmitter& Sfx()
		{ return d_emitterSfx; }

	// a voice emitter (a monophonic, single channel emitter).
	CActorSoundEmitter& Voice()
		{ return d_emitterVoice; }

	void SetDeleteWhenNoCast(bool i_deleteWhenNoCast)
			{d_deleteWhenNoCast = i_deleteWhenNoCast;};
	bool ShouldDelete(void) {return (d_deleteWhenNoCast && !d_castHead);};
	float GetMaxUpdateSec(void) {return(d_maxUpdateSec);}
	void SetMaxUpdateSec( float sec ) { d_maxUpdateSec=sec;}

	virtual void RunAnimation( bool forceUpdate );
	virtual void BeginFrame(void);
	virtual void Update(float i_deltaSec) {}
	virtual void EndFrame(void);

	inline const void SetFirstFrame(bool bState)					{ d_bFirstFrame = bState; }
	inline const bool IsFirstFrame() const							{ return(d_bFirstFrame); }

	// The involvement member indicates how the actor involves itself in the
	// game. The member has two parts, the LSBs indicate the current
	// involvement. The bits to the left indicate the desired involvement. The
	// two parts are normally equal.

	// Bits
	static const uint INVOLVE_NONE;
	static const uint INVOLVE_ACTIVE;
	static const uint INVOLVE_STAGE;
	static const uint INVOLVE_PROTAGONIST;

	// Categories
	static const uint INVOLVE_INACTIVE_NONPROTAGONIST;
	static const uint INVOLVE_ACTIVE_NONPROTAGONIST;
	static const uint INVOLVE_INACTIVE_PROTAGONIST;
	static const uint INVOLVE_ACTIVE_PROTAGONIST;

	void SetInvolvement(uint i_involvement);
	void RequestInvolvement(uint i_involvement);
	void AcceptInvolvement(void);
	uint GetCurrentInvolvement(void);
	uint GetDesiredInvolvement(void);
	bool IsActive(void);

	virtual void SyncState(void) {}

	// Network stuff
	void DistributeNetActor(bool i_persistent = false);
	void BindNetActor(CNetActor* i_netActor);
	void UnbindNetActor(bool i_killNetActor = false);
	void PropagateNetActor(float i_gameTime);
	void SetNetMaster(bool i_netMaster);
	CNetActor* GetNetActor(void) {return(d_netActor);}
	bool IsNetMaster(void) {return(d_netMaster);}

	// In a networked environment, an entity in the shared, virtual world is
	// implemented as an actor on each host. The actor on one of the hosts acts
	// as a master and it is in charge of the entity's state. The actors on the
	// other hosts act as proxies for the master actor and may have completely
	// different implementations than the master. In particular, the master may
	// contain data that is not needed by the proxies. If the role of master is
	// to shift to another host, then this "master data" needs to be communicated
	// to the proxy. There may also be some "metadata" that describes the
	// master data. The metadata only needs to be communicated when the proxy
	// is first established and when the structure of the data changes. The
	// following routines facilitate transmission of the master data and its
	// metadata.
	virtual int GetNetMasterMetaDataSize(void);
	virtual bool HasNetMasterMetaDataChangedSinceLastWrite(void);
	virtual void WriteNetMasterMetaData(void* &io_buffer);
	virtual int GetNetMasterDataSize(void);
	virtual bool HasNetMasterDataChangedSinceLastWrite(void);
	virtual void WriteNetMasterData(void* &io_metaDataBuffer,
			void* &io_dataBuffer);
	virtual void ReadNetMasterData(void* &io_metaDataBuffer,
			void* &io_dataBuffer);

	// Set the minimum time range between master data updates. The actual time
	// is randomly selected in the range in an effort to distribute the updates.
	void SetNetMasterDataMinUpdateTime(uint32 i_lowMS, uint32 i_highMS);

	// The following routines are to be overridden and are where the actual
	// work gets done.
	virtual int GetMasterMetaDataSize(void) {return(0);}
	virtual bool HasMasterMetaDataChangedSinceLastWrite(void) {return(false);}
	virtual void WriteMasterMetaData(void* &io_buffer) {}
	virtual int GetMasterDataSize(void) {return(0);}
	virtual bool HasMasterDataChangedSinceLastWrite(void)
		{return(GetMasterDataSize() > 0);}
	virtual void WriteMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer) {}
	virtual void ReadMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer) {}

	// In general, the goal is to allow actors to be unaware of the networked
	// environment and for network code deal with pushing data around. On the
	// other hand, another goal is to make it possible to configure actors at
	// runtime via scripts. An immediate result is that it must also be possible
	// to configure the network data at runtime and some of that burden must be
	// borne outside network code. The following routines provide a mechanism to
	// do that. As an actor configures itself, it can request a slot in a data
	// buffer to be sent from the master to its proxies and then write that data
	// as appropriate. When data is recieved by the proxies, a "NetDataReceived"
	// message is sent to the actor. The actor can register a handler for that
	// message. The actor (including the master) can read the data at any time.
	// NOTE: This data should change relatively infrequently. If data changes
	// continuously, or even frequently, another mechanism should be found.
	//
	// Provide an opportunity to setup net data when the master is distributed.
	virtual void SetupNetData(void);

	// Provide an opportunity to read the net data when the proxy is established.
	virtual void LoadNetData(void);

	// Request a slot with a given name and a maximum size. False is returned if
	// the name is not unique (may be a hashed name) or if the request is not
	// made on the master.
	bool RequestNetDataSlot(const char* i_name, int i_maxSize);

	// Write data to a slot. If the actor is the master, the slot is found and
	// the size does not exceed the maximum size, the slot is cleared, data is
	// written into the slot, the buffer is sent and true is returned.
	bool WriteNetDataSlot(const char* i_name, void* i_data, int i_size);

	// Force a send of the current contents of the data buffer.
	void SendNetData(void);

	// Get a pointer to a slot for reading or writing. NULL is returned if the
	// actor is not distrbuted, the actor is not the master, or slot cannot be
	// found.
	void* OpenNetDataSlot(const char* i_name);

	// Get a pointer to a slot for reading. NULL is returned if the actor is not
	// distrbuted or slot cannot be found.
	const void* ReadNetDataSlot(const char* i_name);

	// Discontinuity
	void ClearDiscontinuity(void) {d_discontinuity = DISCONTINUITY_NONE;}
	void SetDiscontinuity(uint32 i_discontinuity)
		{d_discontinuity = i_discontinuity;}
	void ClearDiscontinuityBits(uint32 i_discontinuity)
		{d_discontinuity &= ~i_discontinuity;}
	void SetDiscontinuityBits(uint32 i_discontinuity)
		{d_discontinuity |= i_discontinuity;}
	uint32 GetDiscontinuity(void) {return(d_discontinuity);}

	// Manage dynamic elements
	virtual void LoadDynamicLinkageParams(void);
	bool GetDynamicExtents(Vector3 &io_minExtent, Vector3 &io_maxExtent);
	void SetObjectList(CObjectList* i_objectList);
	void ResetDynamicElements(void);
	void UpdateMesh(float i_deltaSec);
	void AddInstanceToScene(int index);

	inline bool InScene(int index = 0)
		{ return Instance(index)->RenderedLastFrame(); }

#ifdef ENABLE_RECORD
	virtual bool CreateRecordBuffer(int i_numTicks);
	virtual void DeleteRecordBuffer(void);
	void SetIdle(void);
	void SetRecord(void);
	void SetPlayback(float i_targetTime = 0.0f);
	bool ManageRecording(float i_frameOffsetSec, float &io_deltaSec);
	float GetRecordingStart(void);
	float GetRecordingEnd(void);
	float GetForwardStepSize(float i_currentTime);
	float GetReverseStepSize(float i_currentTime);
#endif

	// Get the current cast head
	CCastMember *CastHead(void)
		{ return(d_castHead); }

	// lighting info. typically, an actor will be real-time lit, with no adjustments. there are times when
	// we want to adjust the actor's lighting. one case is to have the actor adjust his lighting state
	// based on the vertex colors of the geometry he is on (our collision data embeds color info for each
	// triangle/obb). in this case, the ambient and directional lights are affected by multipliers derived
	// from the collision data color

	typedef enum
	{
		LIGHT_COLOR_SOURCE_NONE = 0,
		LIGHT_COLOR_SOURCE_TERRAIN,
		LIGHT_COLOR_SOURCE_MANUAL,
	};
	#define LIGHT_COLOR_U16_MAX_MULT	1.0f	// currently, we consider range of values in u16 light color to be 0 to 1
	void SetLightColorSource(int nSource);
	inline int LightColorSource(void) { return(d_nLightColorSource); }
	inline void SetLightColorFloor(float fFloor) { d_fLightColorFloor = fFloor; }	// set minumum multiplier (0.0 to 1.0)
	void SetLightColor(u16 ColorMult);
	void SetLightColor(ts_fRGB &ColorMult);
	void ResetLightColor();
	void AdvanceLightColorMult();
	inline float LightColorMult(void) { return(d_fLightColorMult); }
	inline void SetLightColorMultStep(float fStep) { d_fLightColorMultStep = fStep; }
	ts_fRGB AmbientLight();
	/* typically d_pBaseAmbientLight points at the light manager's ambient light, but an actor can have a separate value in d_BaseAmbientLight if desired */
	inline void SetBaseAmbientLight(ts_fRGB *pAmbientLight) { d_pBaseAmbientLight = pAmbientLight; }
	inline void SetBaseAmbientLight(ts_fRGB AmbientLight) { d_BaseAmbientLight = AmbientLight; SetBaseAmbientLight(&d_BaseAmbientLight); }
	inline void SetAmbientStep(float fStep) { d_fAmbientStep = fStep; }
	inline ts_fRGB &GetCurrentColorMult()	{ return(d_CurrentColorMult); }
	void ForceLightColorToTarget();
	CActorChains* GetChains(void) {return(d_chains);}
	
  protected:
	void SetLightColorDefaults();
	void CalculateLightColorMult();
	void ForceLightColorMultToTarget();
	void ForceAmbientToTarget();
  public:

	/* for debugging-- actors in the non-protagonist cast get a chance to draw their obb if showing collision data. */
	/* almost nobody implements this */
	virtual void CollisionDebugDraw(XZRect *rect) {}

	// --- a way to override the assigned id
	void setUniqueId( u32 id ) { m_uiUniqueID = id; }
	// --- this actor was added during the course of the game
	void setAsDynamicCActor( );

	// Called during Place so derived classes can sync up.
	virtual void SyncActor(void) {}

	// Computes the geometry local-to-world matrix from the actor location and
	// orientation.
	void PlaceGeometry(void);

  protected:

	// Here just to let scaling happen. This will go away
	virtual void ScaleActor();

private:
	// the actors name, the first variable (for prominent visibility in the debugger)
	char* d_Name;

	// Actor state.
public:
	Vector3 d_bodyInWorld;
	DirCos3x3 d_bodyToWorld;
	Vector3 d_velocityInWorld;
	Vector3 d_omegaInBody;

	Vector3 d_scale;

	float d_maxUpdateSec;

	// Cast management.
	CCastMember* d_castHead;
	bool d_deleteWhenNoCast;
	bool d_markedForDeath;
	int d_lienCount;
	uint d_involvement;

	// Network
	virtual CNetActor* CreateNetActor(void);
	CNetActor* d_netActor;
	bool d_netMaster;

	// Make sure minimal state gets distributed if not done somewhere else.
	void SetupDistributedState(void);
	void WriteDistributedState(void);
	bool ReadDistributedState(void);
	void SetStateDistributed(bool i_stateDistributed)
		{d_stateDistributed = i_stateDistributed;}

	bool d_stateDistributed;
	REF_SEND_MESSAGE_HANDLER(CNetDataReceived, CActor) d_netDataReceivedHandler;
	struct SDistributedState
	{
		Vector3Packed d_bodyInWorld;
		DirCos3x3 d_bodyToWorld;
		Vector3Packed d_velocityInWorld;
		Vector3Packed d_omegaInBody;
	};

	// Discontinuity
	uint32 d_discontinuity;

	// Chains
	CActorChains* d_chains;

	// Meshes
	CMeshList* d_meshList;

	// Sound
	CActorSoundEmitter d_emitterSfx;
	CActorSoundEmitter d_emitterVoice;

	// lighting (for implementing color data embedded in collision data)
	int			d_nLightColorSource;				// none, terrain, manual
	float			d_fLightColorFloor;				// minimum light color multiplier, 0.0 to 1.0
	u16			d_u16LastColorMult;				// last value, for TERRAIN mode
	ts_fRGB		d_CurrentColorMult;					// last value, for MANUAL mode
	float			d_fLightColorMult;				// strength multiplier for non-ambient lights, 0 to 1, based on grayscale value of color
	float			d_fLightColorMultTarget;		// value for d_fLightColorMult to move toward so that we don't get too-sudden shifts
	bool			d_bLightColorMultAtTarget;
	float			d_fLightColorMultStep;			// how fast to move to new value (max delta to move per frame, range 0.00001 to 1.0f)
	ts_fRGB		*d_pBaseAmbientLight;			// typically the world ambient light
	ts_fRGB		d_BaseAmbientLight;				// local copy in case code wants to hand-set ambient light for an actor
	ts_fRGB		d_AmbientLightMult;				// multiplier for current base ambient light
	ts_fRGB		d_AmbientLightMultTarget;		// value for d_AmbientLightMult to move toward so that we don't get too-sudden shifts
	bool			d_bAmbientLightMultAtTarget;
	float			d_fAmbientStep;					// how fast to move to new value (max delta to move per frame, range 0.00001 to 1.0f)

	u32			m_uiUniqueID;						// DAS, assign a unique ID to all CActor's

#ifdef ENABLE_RECORD
	virtual void RecordState(int i_entry);
	virtual void LoadState(int i_entry);
#endif

  protected:
	CMessageOwner d_messageOwner;

public:
	void DestroyInstance( int index );

  private:
	void ConstructActor();
	bool MakeInstance( int index, ts_Geometry* i_geometry);

	// The handle.
	CHandleCurator<CActor, ACTOR_HANDLE_MASK_WIDTH> d_handleCurator;

	// actor can have a lifetime, at end of which he dies. 0.0 means forever
	float d_fLifetime;
	
#ifdef ENABLE_RECORD
	int FindEntry(float i_targetTime);

	struct SRecordEntry
	{
		Vector3Packed d_bodyInWorld;
		DirCos3x3 d_bodyToWorld;
		Vector3 d_velocityInWorld;
		Vector3 d_omegaInBody;
		float d_deltaSec;
		float d_entrySec;
	} *d_recordBuffer;
	int d_numRecordBufferEntries;
	bool d_bufferFull;
	int d_nextWriteEntry;
	int d_currentReadEntry;
	enum
	{
		RECORDING,
		PLAYING,
		IDLE
	} d_recordState;
#endif

	friend class CCastMember;

	// --- new lighting data used to light the actor
	CLightInfluences			*d_pLightInfluences;
	u32							m_curModelIndex;
	CReplayHandle				m_replayHandle;
	u32							m_saveGameCauseData;

public:
	u32 getID					( )						{ return( m_uiUniqueID ); }

	//-- used for geometry swapping of models
	CLightInfluences*			getLightSet				( )											{ return( d_pLightInfluences ); };
	void							RecordReplayData		( );
	void							PlayReplayData			( );
	static void					RecordReplayInterface( void *object );
	static void					PlayReplayInterface	( void *object );
	inline void					SetSaveGameCauseData	( u32 causeData ) { m_saveGameCauseData = causeData; }

	struct cactorSaveGameData
	{
		Vector3Packed			d_bodyInWorld;
		Vector3Packed			d_velocityInWorld;
		Vector3Packed			d_omegaInBody;
		DirCos3x3				d_bodyToWorld;
		float						d_fLifetime;
		ts_FlagsDataHeader	d_dh;
		ts_MotnInf				d_motnInf[MAX_MOTIONS];
		u32						d_uStartMotionFlags[MAX_MOTIONS];
		u32						m_scriptsSaved;
		u32						m_ignoreEntityID;
		int						d_lienCount;
		bool						d_deleteWhenNoCast:1;
		bool						d_markedForDeath:1;
		uint						d_involvement;
	};
	
	const u32			GetLocalSaveGameMemoryNeeded							( void ) const;
	virtual const u32 GetSaveGameMemoryNeeded									( void );
	virtual const u32 SerializeSaveGame											( char *pBuf, bool bDirection );

public:
	static const CEntityType &ClassType(void) {return(s_actorClass);}
	virtual void Draw(CCollisionEvent* i_event);
	virtual const char* GetClassName(void);

	// PAB
	int	d_skeletons[MAX_SKELETONS_PER_ACTOR];		// skeletons for this actor

private:

	static const CEntityType s_actorClass;
	ScriptVM*		 d_script[ MAX_SCRIPTS ];
	u32				 d_actionFlags;							// flags so different scripts can communciate
	CActorInstance* d_instance[ MAX_ACTOR_INSTANCES ];
	ts_MotnInf		 d_motnInf[MAX_MOTIONS];				// primary and additional animation
	bool d_bFirstFrame;					// forces the animation to be applied to bones in the first frame

	bool motionDisabled[MAX_MOTIONS];

public:
	inline ParameterSet& Parm() { return d_parm; }
private:
	ParameterSet d_parm;

	// notion of identity
public:
	const char* Name() const
	{
		return d_Name;
	}
	void SetName( const char* name )
	{
		ASSERT( name );
		ASSERT( name[ 0 ] );
		int len = strlen( name );
		if ( d_Name )
			delete [] d_Name;
		d_Name = new char[ len + 1 ];
		memcpy( d_Name, name, len );
		d_Name[ len ] = 0;
	}

};

// Specialize ActorHandle member functions.
inline ActorHandle::CHandle(CActor* i_value)
{
	d_value = CActor::ToHandle(i_value);
}

inline ActorHandle &ActorHandle::operator=(CActor* i_value)
{
	d_value = CActor::ToHandle(i_value);
	return(*this);
}

inline ActorHandle::operator CActor*() const
{
	return(CActor::FromHandle(d_value));
}

#endif
