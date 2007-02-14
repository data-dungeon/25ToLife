#ifndef AIPERCEPTION_H
#define AIPERCEPTION_H

#define SOUND_INCIDENT_TABLE_SIZE 50
#define MAX_STORED_LAST_POSITIONS 6
#define TRANSPARENT_SURFACES 3

/* system includes */

/* engine includes */

/* game includes */

#include "Components/Component.h"
#include "CollideHelper/SightEntity.h"

class AiBehavior;

class PerceptionEntity : public CSightEntity
{
 public:
	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity, ESelect i_select);
};

class AiPerception : public CActorComponent
{
public:
	AiPerception( CCompActor &i_actor );
	~AiPerception();

	const char *ComponentName( void ) { return "AiPerception"; }

	void Initialize(void);
	void Reset(void);
	ActorHandle VisionCheck();
	bool GetLastKnownLocation(Vector3 &pos);
	ActorHandle LastKnownTarget(){return lastKnownTarget;}
	void Update(float deltaSec);
	bool GetSoundHeard(void);
	bool GetLastSoundLocation(Vector3 &location, int type = -1);  //any type will return if no parameter sent
	bool GetSoundLaterThanSight();
	float GetLastSeenOrHeardTime();
	float GetLastSeenTime(){return lastSeenTime;}
	float GetLastHeardTime(){return lastHeardTime;}
	bool GetLastDistanceSquared(float &distance);
	bool GetRecentContact(float seconds);
	bool GetRecentSighting(float seconds);

	bool GetAtLeastOneStimulis();
	Vector3 GetLastCentroid();
	bool BlindMansStick(Vector3 destination);
	void ListenForFootsteps(float deltaSec);
	virtual void CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event);
	float GetPercentVisible() {return percentVisible;}
	void SetVisualRadius(float radius) {visualRadiusSqrd = radius * radius;}
	void SetHearingRadius(float radius) {hearingRadiusSqrd = radius * radius;}
	bool inSight;
	void AddSoundIncident(int type, Vector3 location, float intensity = 1.0f, float gameTime = g_timer.GetStartSec(), int participantId = g_players.GetParticipantId(0), int teamId = g_referee.GetTeam(g_players.GetParticipantId(0)));
	bool AvatarCanSeeMe();
	void ClearInsight() {inSight = false; lastKnownTarget = INVALID_OBJECT_HANDLE;}
	void BodyControlPeek(int newPeek);
	void SeedAvatarPosition();
	float GetTimeInSight() {return (g_timer.GetStartSec() - inFocusTime);}
	bool CordCheck(Vector3 fromPos, Vector3 toPos, CActor* targetActor = NULL, int type = CORD_WORLD_AND_ACTORS);
	bool GetSoundTypeHeard(int i_soundType);

	//levels of visability checking, defaults to most detailed check.
	typedef enum
	{
		CHECK_FOV_AND_RANGE,
		CHECK_FOR_PARTIAL_VISABILITY_NO_PROCESSING,
		CHECK_FOR_PARTIAL_VISABILITY,

		VISION_CHECK_COUNT
	} AI_VISION_CHECK_LEVEL;

	typedef enum
	{
		EMPTY_ROW,
		WEAPON_IMPACT,
		WEAPON_FIRED,
		EXPLOSION,  //car etc.
		GLASS_BREAK,
		GERNADE,
		TASER,
		TAUNT,
		LANDING_FROM_FALL,
		DEATH_SCREAM,
		WOUND_SCREAM,
		ALERT_SCREAM,
		FOOTSTEP_SOUND,
		AVATAR_COLLISION,
		DOOR_KICKED,

		SOUNT_TYPE_COUNT
	} AI_SOUND_TYPES;

	//spots for peek check
	typedef enum
	{
		PEEK_NONE,
		PEEK_LEFT,
		PEEK_RIGHT,
		PEEK_CROUCH,
		PEEK_CHECK_COUNT
	} AI_VISION_PEEK_CHECKS;

	typedef enum
	{
		CORD_WORLD_AND_ACTORS,
		CORD_WORLD,
		CORD_ACTOR
	} TYPE_CORD_CHECKS;

	typedef enum
	{
		SIGHTSTATE_NOT_IN_SIGHT,
		SIGHTSTATE_NO_TERRAIN_OBSCURING,
		SIGHTSTATE_PEEK_FIRST_POSITIVE,
		SIGHTSTATE_IN_SIGHT_NORMAL,
		SIGHTSTATE_IN_SIGHT_PEEK,
		SIGHTSTATE_PEEKING_TRY_NORM
	} SIGHTSTATES;

	typedef enum
	{
		FOV_IDLE,
		FOV_ALERT,
		FOV_LONGRANGE
	} FOV_TYPES;

private:
	REF_SEND_MESSAGE_HANDLER(MunitionFired, AiPerception) d_munitionFiredHandler;
	REF_SEND_MESSAGE_HANDLER(MunitionImpact, AiPerception) d_munitionImpactHandler;
	REF_SEND_MESSAGE_HANDLER(TeammateShot, AiPerception) d_teammateShotHandler;
	REF_SEND_MESSAGE_HANDLER(TeammateAlerted, AiPerception) d_teammateAlertedHandler;
	REF_SEND_MESSAGE_HANDLER(AvatarSpotted, AiPerception) d_avatarSpottedHandler;
	REF_BASIC_MESSAGE_HANDLER(DoorKicked, MESSAGE_ACTOR_HANDLE, AiPerception) d_doorKickedHandler;
	REF_SEND_MESSAGE_HANDLER(LobbedExplosionWarning, AiPerception) d_lobbedExplosionWarning;
	REF_BASIC_MESSAGE_HANDLER(ActorDied, MESSAGE_VALUE, AiPerception ) d_actorDiedHandler;
	REF_BASIC_MESSAGE_HANDLER(ShootInstStart, MESSAGE_VALUE, AiPerception ) d_shootAtInstanceStart;
	REF_BASIC_MESSAGE_HANDLER(ShootInstStop, MESSAGE_VALUE, AiPerception ) d_shootAtInstanceStop;
	REF_BASIC_MESSAGE_HANDLER(Taunted, MESSAGE_VALUE, AiPerception ) d_tauntedHandler;

	void SetCurrentPos(Vector3 curPos);
	Vector3 GetLaggedLastPos();
	ActorHandle DetermineTarget();
	void SetupPerceptionEntity(int level);

	void Sighted(CActor* target, Vector3 toPos, bool frameOnly = false);
	Vector3 GetFromPos();
	Vector3 GetToPos(CActor* target);

	int myId;
	float lastPosTime;
	ActorHandle lastKnownTarget;
	float lastSeenTime;
	float lastHeardTime;
	float visualRadiusSqrd;
	float hearingRadiusSqrd;
	Vector3 lastCentroid;
	float footstepTimer;
	Vector3 lastPosGroup[MAX_STORED_LAST_POSITIONS];
	int lastPosCounter;
	bool lastPosGroupFilledOnce;
	float percentVisible;
	int currentPeekType;
	int desiredPeekType;
	int peekFrameDelay;
	float peekTimer;
	int sightFrameDelay;
	int peekCounter;
	float peekPercentVisible;
	int sightState;
	int attemptedTargetId;
	int fov;
	float inFocusTime;
	bool setEchoLastSeen;
	float soundPosTimer;

	PerceptionEntity perceptionEntity;

	struct SoundIncident
	{
		int type;
		float intensity;
		float gameTime;
		Vector3 location;
		int participantId;
		int teamId;
	};
	SoundIncident SoundIncidentTable[SOUND_INCIDENT_TABLE_SIZE];  //fifo
	int soundIncidentCursor;
	AiBehavior *aiBehavior;

	uint8 transparentMaterials[TRANSPARENT_SURFACES];
};

#endif
