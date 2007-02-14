//============================================================================
//=
//= ScriptFunc.h - Script header file that prototypes the functions inside
//=                the engine.
//=
//============================================================================

// this is temporary until the actor class is formalized in the script
#define actor int

// animation
extern float GetMotionFloat( string varName, int motionIndex );
extern int GetMotionInt( string varName, int motionIndex );
extern string GetMotionString( string varName, int motionIndex );
extern void StartMotion( string motionName, int startFrame, int flags );
extern void EnableMotion( int motionIndex, int enable );
extern void StartInterpMotion( string motionName, int startFrame, int flags, int numFrames );
extern void StopMotion( int motionIndex, string motionName );
extern int  MotionEnded( int motionIndex );
extern int  InterpDone( int motionIndex );
extern int AnimAction( actor source, int motionIndex );
extern void SetAnimationRate( int motionIndex, float rate );
extern float GetAnimationRate( int motionIndex );
extern void SetProceduralPosition( int motionIndex, float zeroToOne );
extern int  AnimFrame( int motionIndex );
extern void SyncAnimFrame( actor source, actor dest, int motionIndex );

// parameter set
extern int    ParmGetInt( string key );
extern float  ParmGetFloat( string key );
extern string ParmGetString( string key );
extern void   ParmSetInt( actor act, string key, int value );
extern void   ParmSetFloat( actor act, string key, float value );
extern void   ParmSetString( actor act, string key, string value );
extern void   ParmSet( actor act, string initString );

// math
extern float sin( float v );
extern float asin( float v );
extern float cos( float v );
extern float acos( float v );
extern float atan2( float x, float y );
extern float sqrt( float v );
extern ( float sin, float cos ) sincos( float v );
extern float pow( float x, float y );
extern float exp( float v );
extern ( float vec[3] ) Vector3Add( float v1[3], float v2[3] );
extern ( float vec[3] ) Vector3Sub( float v1[3], float v2[3] );
extern ( float vec[3] ) Vector3CrossProduct( float v1[3], float v2[3] );
extern ( float vec[3] ) Vector3Scale( float v1[3], float scale );
extern ( float vec[3] ) Vector3Normalize( float v1[3] );
extern float Vector3DotProduct( float v1[3], float v2[3] );
extern float Vector3Length( float v[3] );
extern float NormalAngle( float v );
extern float AngleDelta( float angle1, float angle2 );
extern float Deg2Rad( float d );
extern float Rad2Deg( float r );

// random
extern int   GetRandomInt( int min, int max );
extern float GetRandomReal( float min, float max );
extern float GetRandomRealExc( float min, float max );

// events
extern void SendMessageToActor( string message, int data, actor receiver );
extern void SendMessageVector3ToActor( string message, float vec[3], actor receiver );
extern void SendMessageToAll( string message, int data );
extern void SendMessageToAllFromActor( string message, int date, actor sender);
extern void SetMessageDataTranslator( string message, int translatorType );
extern void SendStringMessageToSession( string message, string data );
extern ( float v[ 3 ] ) GetVector3( int messageData );

// script Virtual Machine
extern void ClearStacks();
extern void SetMessageHandlerRules( string message, int rules );
extern void RemoveMessageHandler( string message );
extern void RouteMessageToFunction( string message, string functionName );

// communication between scripts
extern void SetAction( int action );
extern int  GetAction( int scriptIndex );
extern void ClearFlag( int flag );
extern void SetFlag( int flag );
extern int  GetFlag();
extern int GetScriptInt( actor act, int script, string name );
extern int GetScriptFloat( actor act, int script, string name );

// sound
extern void SfxInit();
extern void SfxPlay( string soundname );
extern void SfxStop( string soundname );
extern void SfxChangeVolume( string soundname, float volume );
extern int SfxIsPlaying( string soundname );

extern int DialogPlay( string dialogName, int preemt );
extern int DialogStop( string dialogName );
extern int DialogIsPlaying();

// These return handles and most be stopped!!!!!!!!!!
extern int StreamPlay( string streamName, float volume, actor playAt );
extern int StreamPlayAtPosition( string streamName, float volume, float vec[3] );
extern int StreamStop( int stream );
extern int StreamChangeVolume( int stream, float volume );
extern void StreamUpdatePosition( int stream, actor playAt );
extern int StreamIsPlaying( int stream );

// This just plays it and you can't do anything else
extern int StreamPlayAndForget( string streamName, float volume, float pos[3] );

// time
extern float GameTime();
extern float GetFrameSec();

// lights
extern int  LightCreate( float pos[ 3 ], float colorR, float colorG, float colorB, float attenuation );
extern void LightDestroy( int light );
extern void LightEnable( int light, int enable );
extern void LightBreak( int light, float pos[ 3 ], int damage );
extern int	LightFindLightByName( string lightName );

// Halo's
extern void HaloEnable( actor act, int enable );

// instance
extern int  FindInstanceByName( string instanceName );
extern void InstanceHide( int instance, int hide );

// Named points in world
extern ( float pos[ 3 ] ) GetNamedPoint( string pointName );

// Collision
extern void EnableCollisionLayer( int layer );
extern void DisableCollisionLayer( int layer );

// actor
extern void SetIgnoreEntity( actor act, actor ignore );
extern void ActorDeactivatePathFollower( actor act );
extern void ActorDeactivatePathFollowerCam( actor act );
extern void ActorActivatePathFollower( actor act );
extern void ActorActivatePathFollowerCam( actor act );
extern ( float pos[ 3 ] ) ActorGetPosition( actor act );
extern ( float vel[ 3 ] ) ActorGetVelocity( actor act );
extern void ActorSetVelocity( actor act, float vel[ 3 ] );
extern ( float vel[ 3 ] ) ActorGetAngularVelocity( actor act );
extern ( float vec[ 3 ] ) ActorGetFacingVector( actor act );
extern ( float vec[ 3 ] ) ActorGetUpVector( actor act );
extern void ActorMatchPositionAndOrientation( actor act, actor matchto );
extern float ActorGetRotationX( actor act );
extern float ActorGetRotationY( actor act );
extern float ActorGetRotationZ( actor act );
extern void ActorMarkForDeath( actor act, float time );
extern void ActorHide( actor act, int hide );
extern void ActorSetInstanceVisRegionStatic( actor act, int useWorld );
extern void ActorSetInstanceVisRegionDynamic( actor act );
extern actor FindActorByInstanceName( string instanceName );
extern void ActorEnableInteraction( actor act, int enable );
extern void ActorSetCategoryBits( actor act, int categoryBits );
extern void ActorClearCategoryBits( actor act, int categoryBits );
extern void ActorSetPositiveInclude( actor act, int mask );
extern void ActorSetPositiveIncludeBits( actor act, int mask );
extern void ActorClearPositiveIncludeBits( actor act, int mask );
extern void ActorSetNegativeInclude( actor act, int mask );
extern void ActorSetNegativeIncludeBits( actor act, int mask );
extern void ActorClearNegativeIncludeBits( actor act, int mask );
extern void ActorSetPositiveExclude( actor act, int mask );
extern void ActorSetPositiveExcludeBits( actor act, int mask );
extern void ActorClearPositiveExcludeBits( actor act, int mask );
extern void ActorSetNegativeExclude( actor act, int mask );
extern void ActorSetNegativeExcludeBits( actor act, int mask );
extern void ActorClearNegativeExcludeBits( actor act, int mask );
extern void ActorLoadMotion( actor act, string motionFile, int geomIndex );
extern void ActorLoadAnimScript( actor act, string scriptFile, int scriptIndex );
extern void ActorSetAlpha( actor act, float alpha );
extern void ActorSetScriptFloat( actor act, string varName, int scriptIndex, float f );
extern void ActorSetScriptInt( actor act, string varName, int scriptIndex, int i );
extern float ActorGetScriptFloat( actor act, string varName, int scriptIndex );
extern int ActorGetScriptInt( actor act, string varName, int scriptIndex );
extern void ActorSetGeometryFromWorldInstance( actor act, int geomIndex, string instanceName );
extern void ActorSetGeometry( actor act, int geomIndex, string geomFilename );
extern void ActorSetGeometryCullRadius( actor act, int geomIndex, float radius );
extern void ActorHideGeometry( actor act, int geomIndex, int hideFlag );
extern int ActorGetNetMaster( actor act );
extern void ActorSetStateDistributed( actor act, int stateDistributed );
extern int ActorRequestNetDataSlot( actor act, string slotName, int maxBufferSize );
extern int ActorWriteNetDataSlot( actor act, string slotName, int buffer, int bufferSize );
extern void ActorSendNetData( actor act );
extern int ActorOpenNetDataSlot( actor act, string slotName );
extern int ActorReadNetDataSlot( actor act, string slotName );
extern void ActorSetDiscontinuity( actor act, int mask );
extern void ActorSetCollisionSurfaceName( actor act, string surfName );
extern float ActorGetCollisionXSize( actor act );
extern float ActorGetCollisionYSize( actor act );
extern float ActorGetCollisionZSize( actor act );
extern void ActorSetCollisionXSize( actor act, float size );
extern void ActorSetCollisionYSize( actor act, float size );
extern void ActorSetCollisionZSize( actor act, float size );
extern void ActorScaleCollisionXSize( actor act, float size );
extern void ActorScaleCollisionYSize( actor act, float size );
extern void ActorScaleCollisionZSize( actor act, float size );
extern actor ActorCreate( string className );
extern actor ActorCreateFromParametersWithPrefix( actor act, string prefix, float pos[3], float heading, string instanceName, string overrideParms);
extern actor ActorCreateFromInitString( actor act, string initString, float pos[3], float heading, string instanceName );
extern void ActorSetInstanceName( actor act, string name );
extern void UpdatePain (actor act, int direction, int location, int munitionID, float damage);
extern float ActorFindFloor( actor act );
extern int ActorValid( actor act );
extern int ActorGetInstancePtr( actor act );
extern string ActorGetInstanceName( actor act );
extern void ActorSetImpotent( actor act, int flag );
extern void ActorSetJuggernaut( actor act, int flag );

// health
extern void DoDamage( actor act, float damage, int damageID );
extern void AddHealth( actor act, float health );
extern float GetHealth( actor act );
extern float GetMaxHealth( actor act );
extern void Kill( actor act );

// particles
extern void ParticleEffectCreate( string name, float pos[3], float vel[3] );
extern void ParticleEffectCreateNoVel( string name, float pos[3] );
extern int ParticleEmitterCreate( string name, float pos[3] );
extern void ParticleEmitterKill( int emitterHandle );
extern void ParticleEmitterActivate( int emitterHandle );
extern void ParticleEmitterDeactivate( int emitterHandle );
extern void ParticleEmitterSetLOD( int emitterHandle, float lod1RadiusOverride, float lod2RadiusOverride );
extern void ParticleEmitterSetPath( int emitterHandle, string path );

// screen vade
extern void FadeOut();
extern void FadeIn();
extern int  IsFading();

// interfaces
extern void StartAction( int action );
extern void EndAction( int action );
extern int ActionInProgress( int action );
extern void MoveHeading( float degrees );
extern void MoveTurnRate( float degreesPerSec );
extern void MoveTurnRateNormalized( float fracPerSed );
extern void MoveDirection( float x, float y, float z, float speed );
extern void MoveDirectionNormalized( float x, float y, float z, float speed );
extern void MoveToPoint( float x, float y, float z, float speed );
extern void MoveToPointNormalized( float x, float y, float z, float speed );
extern void MoveForward( float speed );
extern void MoveForwardNormalized( float speed );
extern void MoveSideways( float speed );
extern void MoveSidewaysNormalized( float speed );

// View Interface
extern void ViewLock();
extern void ViewUnlock();
extern void ViewLookAtPoint( float p[3] );
extern void ViewLookDirection( float p[3] );
extern (float view[3]) ViewGetDirection();
extern float ViewGetHeadingOffset();

// components
extern void AttachComponent( actor act, string componentName );
extern void InitializeComponents( actor act );
extern int IsComponentActive( actor act, string componentName );
extern int HasComponent( actor act, string componentName );
extern void ActivateComponent( actor act, string componentName );
extern void DeactivateComponent( actor act, string componentName );
extern void ActivateComponentInClass( actor act, string className );
extern void DeactivateComponentInClass( actor act, string className );
extern (float v[3]) GetPropertyVector( actor act, string propertyName );
extern float GetPropertyFloat( actor act, string propertyName );
extern int GetPropertyInt( actor act, string propertyName );
extern int GetPropertyPointer( actor act, string propertyName );
extern int GetPropertyHandle( actor act, string propertyName );
extern void SetPropertyVector( actor act, string propertyName, float v[3] );
extern void SetPropertyFloat( actor act, string propertyName, float f );
extern void SetPropertyInt( actor act, string propertyName, int i );
extern void SetPropertyPointer( actor act, string propertyName, int p );
extern void SetPropertyHandle( actor act, string propertyName, int i );
extern int WallFall();
extern void AbortVault();

// Weapons queries
extern string GetWeaponDisplayName( string weapon );
extern string GetWeaponModelName( string weapon );
extern int GetWeaponSlot( string weapon );

// Inventory management
extern void ShowCurrentWeapon( actor act );
extern void HideCurrentWeapon( actor act );

extern int FindWeaponSlot( actor act, string weapon );
extern int AddWeapon( actor act, string weapon, int selectIfBetter );
extern int RemoveWeapon( actor act, string weapon );

extern string GetWeaponInSlot( actor act, int slot );
extern void RemoveWeaponInSlot( actor act, int slot );

// ammo management
extern int GetSlotAmmo( actor act, int slot );
extern int SlotNeedsAmmo( actor act, int slot );
extern void AddSlotAmmo( actor act, int slot, int amount );
extern int GetSlotAmmoCode( actor act, int slot );
extern void ApplySlotAmmoCode( actor act, int slot, int ammoCode );
extern ( int inBelt, int inClip ) GetSlotSplitAmmo( actor act, int slot );
extern void SetSlotSplitAmmo( actor act, int slot, int inBelt, int inClip );
extern int WeaponAmmoCompatible( string weapon1, string weapon2 );

// Loadout management
extern string TakeLoadoutSnapshot( actor act );
extern void FreeLoadoutSnapshot( string snapshot );
extern void ApplyLoadoutSnapshot( actor act, string snapshot );

extern int GetSelectedWeaponSlot( actor act );
extern void SetSelectedWeaponSlot( actor act, int slot );

extern void SetPathCamFadeIn( int f );
extern void SetPathCamFadeOut( int f );

extern void SetPathCamTargetingBehavior( int targetingBehavior );
extern void SetCameraType( int camType );
extern void SetPathCamTimeActive( float time );
extern void SetPathTextMessage( string s );
extern void SetPathCamSpeed( float speed );
extern void SetPathCamTargetSpeed( float speed );
extern void SetPathCamEndDelay( float t );
extern void SetPathCamAccel( float accel );
extern void SetPathCamDecel( float decel );
extern void SetPathCamTargetPoint( float v[ 3 ] );
extern void SetPathCamTarget( actor a );

// Rigid Body
extern void RigidBodySetMass( actor act, float mass );
extern void RigidBodySetInertia( actor act, float Ixx, float Iyy, float Izz );
extern void RigidBodySetBodyAcceleration( actor act, float accel[3], int autoApply );
extern void RigidBodySetBodyVelocity( actor act, float velocity[3] );
extern void RigidBodySetBodyAngularVelocity( actor act, float velocity[3] );
extern float RigidBodyGetKineticEnergy( actor act );

// Triggerbox
extern int	 TriggerBox_ActorCount( actor act );
extern actor TriggerBox_GetActor( actor act, int index );

// Basic Mover Interface
extern void MoveTo( actor act, float rot[ 3 ], float vel );
extern void MoveToX( actor act, float rot, float vel );
extern void MoveToY( actor act, float rot, float vel );
extern void MoveToZ( actor act, float rot, float vel );
extern void MoveRel( actor act, float rot[ 3 ], float vel );
extern void MoveRelX( actor act, float rot, float vel );
extern void MoveRelY( actor act, float rot, float vel );
extern void MoveRelZ( actor act, float rot, float vel );
extern void RotateTo( actor act, float rot[ 3 ], float vel );
extern void RotateToX( actor act, float rot, float vel );
extern void RotateToY( actor act, float rot, float vel );
extern void RotateToZ( actor act, float rot, float vel );
extern void RotateRel( actor act, float rot[ 3 ], float vel );
extern void RotateRelX( actor act, float rot, float vel );
extern void RotateRelY( actor act, float rot, float vel );
extern void RotateRelZ( actor act, float rot, float vel );
extern void MoveAtConstantVelocity( actor act, float vel[3] );
extern void RotateAtConstantVelocity( actor act, float vel[3] );

// Cut Scenes
extern void PlayCutScene();

// FMV
extern void PlayFMV( string Filename, int AllowInterruptions );

// Text Box
extern void ShowTextMessage( string name, float delay, int pauseIt, int override );

// Checkpoint
extern void SetupCheckpoint( int checkpoint, float pos[ 3 ], float facing[ 3 ] );
extern void ResetAllCheckpointActors( int checkpoint );

// Game Type
extern int IsSinglePlayer();
extern int IsMultiPlayer();

// Surfaces
extern int GetSurfaceIndex( string name );

// ActorList Component
extern int   ActorList_Count();
extern int   ActorList_Add( actor act );
extern int   ActorList_Remove( actor act );
extern int   ActorList_Contains( actor act );
extern actor ActorList_Get( int index );

// Stuff to manipulate shared body components (eyes, head, etc)
extern int   SharedBody_Hidden( string slot );
extern void  SharedBody_Hide( string slot, int flag );

// Game Locks
extern void  GameLocks_Unlock( string key );
extern int   GameLocks_IsUnlocked( string key );

// InventoryList Component
extern void	AddInventory( actor act, string key, int value );
extern int	GetInventory( actor act, string key );
extern void	RemoveInventory( actor act, string key, int value );
extern void InventoryCheckpointHit( actor act );
extern void InventoryCheckpointReset( actor act );

// Persistent Data Component
extern void	SetGlobal( actor act, string name, int value );

// first-third camera view
extern void SetCameraFirstPersonLock( int iBool );

// switch geometry on player to another character
extern void ChangeAvatarClothes( string clothes );
extern void ResetAvatarClothes();

// ghost camera access
extern void LaunchGhostCamera();

enum
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
};

// debug stuff
#ifndef CONSUMER_BUILD
extern void DrawDebugLine(float pt1[3], float pt2[3], int color);
extern void DrawDebugSphere(float pt1[3], int color, float radius);
extern void DrawDebugPoint(float pt1[3], int color);
extern void QueueDebugVector(float pt1[3], float pt2[3], int color, float lifetime);
extern void QueueDebugLine(float pt1[3], float pt2[3], int color, float lifetime);
extern void QueueDebugPoint(float pt1[3], float pt2[3], int color, float radius, float lifetime);
#else
#define DrawDebugLine(float pt1[3], float pt2[3], int color)
#define DrawDebugSphere(float pt1[3], int color, float radius)
#define DrawDebugPoint(float pt1[3], int color)
#define QueueDebugVector(float pt1[3], float pt2[3], int color, float lifetime)
#define QueueDebugLine(float pt1[3], float pt2[3], int color, float lifetime)
#define QueueDebugPoint(float pt1[3], float pt2[3], int color, float radius, float lifetime)
#endif

//============================================================================
// Navigate library interface
//============================================================================
enum
{
	Navigate_PathType_OneWay = 0,
	Navigate_PathType_BackAndForth,
	Navigate_PathType_Circuit,
	Navigate_PathType_Count
};
extern int FindBestMesh( float pos[ 3 ] );
extern ( float pos[ 3 ] ) FindRandomPosition( int meshHandle );
extern int PathConnection_Create_2( int meshHandle, int pathType, float posA[ 3 ], float posB[ 3 ] );
extern int PathConnection_Create_3( int meshHandle, int pathType, float posA[ 3 ], float posB[ 3 ], float posC[ 3 ] );
extern int PathConnection_CheckCreationDone( int connectionHandle );
extern int PathConnection_CheckEndOfPath( int connectionHandle );
extern ( float pos[ 3 ] ) PathConnection_GetNextWaypoint( int connectionHandle );
extern float PathConnection_GetDistanceToNextWaypoint( int connectionHandle );
extern ( float pos[ 3 ] ) PathConnection_GetDirectionToNextWaypointNormalized( int connectionHandle );
extern void PathConnection_ConnectToClosestPoint( int connectionHandle, float pos[ 3 ] );
extern int PathConnection_MoveToNextWaypoint( int connectionHandle );
extern void PathConnection_Release( int connectionHandle );

// cover API
extern int Cover_StartSearch( float targetPos[ 3 ], float volumeCenter[ 3 ], float volumeRadius );
extern int Cover_CheckSearchCompleted( int coverHandle );
extern void Cover_ReleaseSearch( int coverHandle );
extern ( float pos[ 3 ] ) Cover_GetBestPosition( int coverHandle, float fPreferredDistanceFromTarget, float fDesiredOffsetFromCoverPoint );

//============================================================================
//============================================================================
//============================================================================
//TTL Specific....found in main/TtlScriptFuncs.cpp
//============================================================================
//============================================================================
//============================================================================
extern (float screenPos[ 2 ])  GetWorldToScreen( int sceneNum, float worldPos[3] );
extern void CreateMuzzleFlash();

// weapons
extern int GetWeaponClass( int weaponID );
extern string GetProjectileClass( int munitionType );
extern int GetMunitionImpactType( int messageData );
extern float GetMunitionImpactDamage( int messageData );
extern actor GetMunitionImpactShooter( int messageData );
extern ( float pos[ 3 ] ) GetMunitionImpactIntersection( int messageData );
extern ( float pos[ 3 ] ) GetMunitionImpactNormal( int messageData );
extern int GetMunitionImpactMaterial( int messageData );
extern int GetMunitionMaterial( int messageData );
extern float GetMunitionImpactDamage( int messageData );
extern ( float pos[ 3 ] ) GetMunitionFromDir( int messageData );
extern int GetMunitionFired_WeaponID( int messageData );
extern int GetMunitionFired_WeaponHand( int messageData );

// Car bomb!
extern void CarBomb( actor instigator, float pos[3] );

// Projectile support
extern int ExplodeProjectile( string projectileName, actor instigator, float pos[3] );
extern int FireProjectile( string projectileNAme, actor shooter, float pos[3], float dir[3] );
extern int FireProjectileAt( string projectileNAme, actor shooter, actor victim, float pos[3] );

// afflictions
extern int Affliction_Stunned( actor act );
extern int Affliction_OnFire( actor act );

// collision info
extern ( float pos[ 3 ] ) GetCollisionImpactIntersection( int messageData );
extern ( float pos[ 3 ] ) GetCollisionImpactNormal( int messageData );
extern actor GetCollisionSender( int messageData );

// Shatter
extern int Shatter( actor who, string shatterName, int colorR, int colorG, int colorB, int colorA );

// Jukebox
extern void JukeboxAddSource( actor who );
extern void JukeboxRemoveSource( actor who );
extern void JukeboxNextTrack();

//hud
extern void ShowHud();
extern void HideHud();
extern void Hud_ShowTeamStatus( int team );
extern void Hud_SetRespawnTimer( int team, int seconds );
extern void Hud_SetTeamStatus( int team, int livePlayers, int totalPlayers );

// ai
extern void Box_InsideMe(actor boxActor);

// pickups
extern void Pickup_SetAttachableToActor( actor act, int toggle );  //default is not enabled
extern void Pickup_SetDropOnButton( actor act, int toggle );		//default is not enabled
extern void Pickup_SetDropOnDeath( actor act, int toggle );		//default is not enabled
extern void Pickup_SetParticleEmitter( actor act, string emitterName, float offset[3] );
extern int Pickup_IsHome( actor act );
extern int Pickup_IsDropped( actor act );
extern int Pickup_IsHeld( actor act );
extern int Pickup_IsCaptured( actor act );
extern int Pickup_IsAcquired( actor act );
extern actor Pickup_GetHolder( actor act );
extern actor Pickup_GetDropper( actor act );
extern int Pickup_GetTransitionTime( actor act );

// tagframes
extern void TagFrame_AssumeRoleOfClosestSpawn( actor act, int flag );
extern int TagFrame_CanInteract( actor act, actor querier);
extern void TagFrame_Reset( actor act );
extern int TagFrame_AnyBlueTags();
extern int TagFrame_AnyRedTags();

// objective manager
extern int ObjectiveMgr_Add( actor act );
extern actor ObjectiveMgr_GetFirstDropped();
extern int ObjectiveMgr_AllCaptured();

// path
extern void Path_SetAtRandomPoint();

//============================================================================
// participant
extern int Participant_GetActor( int actorId );
extern int Participant_GetId( actor act );
extern int Participant_GetTeam( actor act );
extern int Participant_GetRole( actor act );
extern int Participant_GetSpawnTicket( int actorId );

//============================================================================
// ConfigMgr
extern int ConfigMgr_TestOptionValue( string key, string value );
extern string ConfigMgr_GetCurrentLevelName();
extern int ConfigMgr_GetChoiceAttributeInt( string name, string attribute );
extern string ConfigMgr_GetChoiceAttributeString( string name, string attribute );

//============================================================================
// referee
extern int Referee_IsSetupPhase();
extern int Referee_IsReadyPhase();
extern int Referee_IsPopulatingPhase();
extern int Referee_IsPlayingPhase();
extern int Referee_IsReportingPhase();
extern void Referee_SetRoundsInMatch( int rounds );
extern void Referee_PreventSetupPhaseExit();
extern void Referee_PreventPopulatingPhaseExit();
extern void Referee_PreventPlayingPhaseExit();
extern void Referee_PreventReportingPhaseExit();
extern void Referee_AllowSetupPhaseExit();
extern void Referee_AllowPopulatingPhaseExit();
extern void Referee_AllowPlayingPhaseExit();
extern void Referee_AllowReportingPhaseExit();
extern void Referee_CommandSetupPhaseExit();
extern void Referee_CommandPopulatingPhaseExit();
extern void Referee_CommandPlayingPhaseExit();
extern void Referee_CommandReportingPhaseExit();
extern void Referee_SetSetupPhaseEndTime( int time );
extern void Referee_SetPopulatingPhaseEndTime( int time );
extern void Referee_SetPlayingPhaseEndTime( int time );
extern void Referee_SetReportingPhaseEndTime( int time );
extern void Referee_SetSetupPhaseLifetime( int time );
extern void Referee_SetPopulatingPhaseLifetime( int time );
extern void Referee_SetPlayingPhaseLifetime( int time );
extern void Referee_SetReportingPhaseLifetime( int time );
extern void Referee_SetRemainingPhaseTime( int time, int exceedLifetime );
extern int Referee_GetElapsedPhaseTime();
extern int Referee_GetRemainingPhaseTime();
extern int Referee_GetCurrentPhaseStartTime();
extern int Referee_IsTimedPhase();
extern int Referee_GetNumStartPlayers();
extern int Referee_GetNumPlayers();
extern int Referee_GetNumNonPlayers();
extern int Referee_GetNumParticipants();
extern void Referee_SetUnlimitedSpawnTickets( int teamId );
extern void Referee_SetUnlimitedSpawnTicketsForAll();
extern void Referee_SetNumSpawnTickets( int teamId, int numSpawnTickets );
extern void Referee_SetNumSpawnTicketsForAll( int numSpawnTickets );
extern int Referee_DoesTeamLimitSpawnTickets( int teamId );
extern int Referee_DoesTeamLimitSpawnTicketsForParticipant( int participantId );
extern int Referee_DoesTeamLimitSpawnTicketsForLocalAvatar();
extern int Referee_RequestSpawnTicket( int teamId );
extern int Referee_RequestSpawnTicketForParticipant( int participantId );
extern int Referee_RequestSpawnTicketForLocalAvatar();
extern int Referee_GetLocalPlayerAgentId();
extern void Referee_InstallTeamingModel( string name );
extern void Referee_InstallScoringModel( string name );
extern int Referee_DoAllAgentsHaveAvatars();
extern int Referee_GetOverallMVP();
extern int Referee_GetTeamMVP( int iTeamId );
extern int Referee_GetCurrentHighestScore();
extern int Referee_GetCurrentHighestTeamTagCount();
extern int Referee_GetTeamTagCount( int iTeamId );
extern int Referee_IsBlueTeamWinning();
extern int Referee_IsRedTeamWinning();
extern int Referee_IsBlueTeamWinner();
extern int Referee_IsRedTeamWinner();
extern int Referee_CheckTeamDown( int iTeamID );
extern int Referee_IsPlayerBlue( int iParticipantID );
extern int Referee_IsPlayerRed( int iParticipantID );
extern int Referee_IsPlayerFriend( int iParticipantID );
extern int Referee_IsPlayerEnemy( int iParticipantID );
extern int Referee_IsPlayerNeutral( int iParticipantID );
extern int Referee_IsAmbient( int iParticipantID );
extern int Referee_IsBlueAllAlive();
extern int Referee_IsBlueAllDead();
extern int Referee_IsBlueAllPermanentlyDead();
extern int Referee_IsRedAllAlive();
extern int Referee_IsRedAllDead();
extern int Referee_IsRedAllPermanentlyDead();
extern int Referee_GetRedPlayerCount();
extern int Referee_GetBluePlayerCount();
extern int Referee_GetTeam( int iPlayerId );
extern int Referee_GetRedAliveCount();
extern int Referee_GetBlueAliveCount();
extern int Referee_IsAvatarActive();
extern int Referee_GetRoundCount();

//============================================================================
// TeamingModel interface
extern int TeamingModel_ValidTeams();
extern void TeamingModel_AssignAlphaToRedTeam();
extern void TeamingModel_AssignAlphaToBlueTeam();
extern void TeamingModel_AssignBetaToRedTeam();
extern void TeamingModel_AssignBetaToBlueTeam();
extern int TeamingModel_GetPlayerProfileVariant( int iPlayerId );
extern int TeamingModel_GetTeamProfileVariant( int iTeamId );
extern string TeamingModel_GetTeamName( int iTeamId );
extern string TeamingModel_GetRedTeamName();
extern string TeamingModel_GetBlueTeamName();

//============================================================================
// PlayerSpawnMgr
extern int PlayerSpawnMgr_SpawnAvatar( int bForce, int rearm );		// return whether the avatar was successfully spawned
extern void PlayerSpawnMgr_ControlSpawn( float pos[3], int participant );	// switch control of a spawn point
extern int PlayerSpawnMgr_FindRoleOfNearestSpawnPoint( float pos[3] );
extern int PlayerSpawnMgr_HasSpawnPoint( );
extern int PlayerSpawnMgr_BlueHasSpawnPoint();
extern int PlayerSpawnMgr_RedHasSpawnPoint();

//============================================================================
// session
extern int Session_GetCurrentTimeMS();						// return the current session time in ms.

//============================================================================
// Player
extern actor Players_GetActor( int iIndex );
extern int Players_GetActorHandle( int iIndex );
extern int Players_GetParticipantId( int iIndex );
extern int Players_GetAvatar();
extern int Players_GetAvatarId();
extern void Players_ActivateControls( int activate );
extern string Players_GetPlayerDisplayName( int iParticipantID );

//============================================================================
// initial parameters for a level
extern void LevelInit_SetCharacter( string who );
extern void LevelInit_SetWeapons( string weap );
extern void LevelInit_SetTag( string tag );
extern void LevelInit_SetVoiceset( string voiceset);

//============================================================================
// GameEnv support
extern int GameEnv_IsWeaponMaster();

//============================================================================
// migration utility functions
extern void PackBuffer( int buffer, ... );
extern void UnpackBuffer( int buffer, ... );
extern int BufferSize( ... );

//============================================================================
// misc script functions


// use this for *TEMPORARY* strings - it goes away the next time someone calls this function
extern string Misc_CatStrings( string a, string b );
extern string sprintf( string fmt, ... );


