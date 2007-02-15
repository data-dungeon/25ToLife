/*************************************/
/*                                   */
/*   CutSceneMgr.h                   */
/*   big juju   5/2/03               */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   cut scenes!                     */
/*                                   */
/*************************************/

/* cut scenes provide a way for data to drive the engine without player input */

#ifndef __CUTSCENEMGR_H
#define __CUTSCENEMGR_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include <GeomUtil/AnimCurve.h>

/******************* forward declarations ****************/

class PathCamera;
class GeneralCamera;
class CCutSceneMgr;
class SigGen;
class SoundStream;
class MediaFile;

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/* a cut scene data stream interpreter can use animation curves to drive things */

typedef struct
{
	int nType;
	CAnimCurve AnimCurve;
} ts_InterpreterCurve;

/******************* global variables ********************/

/******************* global prototypes *******************/

/******************* class *******************************/

/* a class to execute cut scene data */

class CCutSceneInterpreter
{
public:
	/* Constructor/destructor */

	CCutSceneInterpreter();
	~CCutSceneInterpreter();
	void Initialize(bool bFromConstructor);
	void SetPermanent();
	void Cleanup();

	/* known keys */

	enum 
	{
		/*** only add to end of list unless you want to re-output all cut scenes! ***/

		CUT_SCENE_KEY_DONT_CARE = -1,
		CUT_SCENE_TYPE = 0,
		CUT_SCENE_VERSION,
		CUT_SCENE_COUNT,
		CUT_SCENE_AT_LABEL,
		CUT_SCENE_LABEL,
		CUT_SCENE_START,
		CUT_SCENE_LENGTH,
		CUT_SCENE_TIME,
		CUT_SCENE_END,				// end of data stream
		CUT_SCENE_EVENT_TYPE,
		CUT_SCENE_DISTANCE,
		CUT_SCENE_SPEED,
		CUT_SCENE_PATH,
		CUT_SCENE_NAME,
		CUT_SCENE_NEW,
		CUT_SCENE_CLASS,
		CUT_SCENE_MODEL_NAME,
		CUT_SCENE_KFM,
		CUT_SCENE_TARGET,
		CUT_SCENE_INDEX,
		CUT_SCENE_PARENT,
		CUT_SCENE_CURVE_COUNT,
		CUT_SCENE_CURVE_TYPE,
		CUT_SCENE_CURVE,
		CUT_SCENE_X,
		CUT_SCENE_Y,
		CUT_SCENE_Z,
		CUT_SCENE_PARTICLE_EFFECT,
		CUT_SCENE_SOUND,
		CUT_SCENE_LOOP,
		CUT_SCENE_FOCAL_LENGTH,
		CUT_SCENE_VERTICAL_APERTURE,
		CUT_SCENE_HORIZONTAL_APERTURE,
		CUT_SCENE_TRANSFORM_XZY,
		CUT_SCENE_FADE,
		CUT_SCENE_LIGHT_TYPE,
		CUT_SCENE_RED,
		CUT_SCENE_GREEN,
		CUT_SCENE_BLUE,
		CUT_SCENE_RESTORE_TO_BASE_POSE,
		CUT_SCENE_SHADOW,
		CUT_SCENE_SHADOW_RADIUS,
		CUT_SCENE_SHADOW_ALPHA,
		CUT_SCENE_TRIGGER_EVENT,
		CUT_SCENE_SHOW_CLUB,
		CUT_SCENE_SHOW_RATTLE,
		CUT_SCENE_SHOW_BLOWGUN,

		/*** only add to end of list unless you want to re-output all cut scenes! ***/

		/* must be last */

		MAX_KEYS
	} KNOWN_KEYS;

	/* known event types (a cut scene data stream is a series of timed events) */

	enum 
	{
		/*** only add to end of list unless you want to re-output all cut scenes! ***/

		DATA_STREAM_EVENT_ATTACH_TO_PATH = 0,
		DATA_STREAM_EVENT_PATH_DISTANCE,
		DATA_STREAM_EVENT_PATH_SPEED,
		DATA_STREAM_EVENT_ANIMATE,
		DATA_STREAM_EVENT_TARGET,
		DATA_STREAM_EVENT_ATTACH_TO_CURVE,
		DATA_STREAM_EVENT_SET_POSITION,
		DATA_STREAM_EVENT_TAK_POSITION,
		DATA_STREAM_EVENT_SET_ROTATION,
		DATA_STREAM_EVENT_GO,		// e.g. particle effect interpreter creates a particle effect, sound interpreter plays a sound
		DATA_STREAM_EVENT_SHOW,		// e.g. particle emitter activates
		DATA_STREAM_EVENT_HIDE,		// e.g. particle emitter deactivates
		DATA_STREAM_EVENT_CAMERA_SETTINGS,
		DATA_STREAM_EVENT_SET_SCALE,
		DATA_STREAM_EVENT_SET_COLOR,

		/*** only add to end of list unless you want to re-output all cut scenes! ***/

		/* must be last */

		MAX_EVENT_TYPES
	};

	/* known curve types (determines what channel curve controls, e.g. x-translation, y-rotation) */

	enum 
	{
		/*** only add to end of list unless you want to re-output all cut scenes! ***/

		CURVE_TYPE_UNKNOWN = 0,
		CURVE_TYPE_TRANSLATE_X,
		CURVE_TYPE_TRANSLATE_Y,
		CURVE_TYPE_TRANSLATE_Z,
		CURVE_TYPE_ROTATE_X,
		CURVE_TYPE_ROTATE_Y,
		CURVE_TYPE_ROTATE_Z,
		CURVE_TYPE_SCALE_X,
		CURVE_TYPE_SCALE_Y,
		CURVE_TYPE_SCALE_Z,
		CURVE_TYPE_FOCAL_LENGTH,
		CURVE_TYPE_VERTICAL_APERTURE,
		CURVE_TYPE_HORIZONTAL_APERTURE,
		CURVE_TYPE_DEPTH_OF_FIELD,
		CURVE_TYPE_PATH_POSITION,
		CURVE_TYPE_COLOR_RED,
		CURVE_TYPE_COLOR_GREEN,
		CURVE_TYPE_COLOR_BLUE,

		/*** only add to end of list unless you want to re-output all cut scenes! ***/

		/* must be last */

		MAX_CURVE_TYPES
	};

	/* known interpreter data types-- though generic, each interpreter is tasked to handle a particular data type */

	typedef enum
	{
		UNKNOWN = 0,
		FILE,						// the entire filestream file-- this one's a bit different since it's not time-code/event based
		ACTOR,					// control an actor during the cut scene
		LOAD_SCREEN_ACTOR,	// simpified actor (ts_Actor record instead of CActor) for load screen
		CAMERA,					// control the PathCamera camera
		PARTICLE_EMITTER,		// control a particle emitter
		PARTICLE_EFFECTS,		// control particle effects-- effects are one-shot, can be bundled together
		LIGHT,					// control a realtime light, typically local
		DISTORTION,				// control full screen distortion
		MOTION_BLUR,			// control motion blur
		SOUND,					// sound effects
		DUMMY_PARENT,			// to support hierarchical animation
		TRIGGER,					// can send events to a world actor by instance name

		/* must be last */

		MAX_INTERPRETER_TYPES
	} INTERPRETER_TYPE;

	/* key value types */

	typedef enum
	{
		KEY_INT,
		KEY_FLOAT,
		KEY_STRING,
	} KEY_TYPE;

	/* initialize */

	bool Initialize(char *pData, char *pDataEnd, CCutSceneInterpreter *pNext, CCutSceneMgr *pCutSceneMgr, bool bLoadScreen);
	bool InitActor();
	bool InitLoadScreenActor();
	bool InitCamera();
	bool InitEmitter();
	bool InitEffects();
	bool InitLight();
	bool InitDistortion();
	bool InitMotionBlur();
	bool InitSound();
	bool InitTrigger();
	bool InitDummyParent();
	void UnhandledInitializeKey();

	/* execute */

	bool Execute(float fTime);
	bool ExecuteActor(float fTime);
	bool ExecuteLoadScreenActor(float fTime);
	bool ExecuteCamera(float fTime);
	bool ExecuteEmitter(float fTime);
	bool ExecuteEffects(float fTime);
	bool ExecuteLight(float fTime);
	bool ExecuteDistortion(float fTime);
	bool ExecuteMotionBlur(float fTime);
	bool ExecuteSound(float fTime);
	bool ExecuteTrigger(float fTime);
	bool ExecuteDummyParent(float fTime);
	void UnhandledEvent();
	bool GetExecuteKey(int nCheckKey1 = CUT_SCENE_KEY_DONT_CARE, int nCheckKey2 = CUT_SCENE_KEY_DONT_CARE, int nCheckKey3 = CUT_SCENE_KEY_DONT_CARE);	// can be expanded if needed
	bool GetOptionalKey(int nKey);
	bool GetNextTimecode();
	void SkipToNextEvent();
	CActor *LinkToActor(bool bAsTarget = false);
	void SetActiveCamera(u32 u32CameraFlag);
	void EvaluateCurves(float fTime);
	void BuildTransform();
	void CalculateLightDirection();

	/* event processing */

	bool AttachToPath();
	bool AttachToCurve();
	bool SetPosition();
	bool SetRotation();
	bool SetScale();
	bool SetColor();
	void Hide(bool bState);

	/* parsing */

	bool GetKeyValuePair(bool bIgnoreUnknownKey = false);
	bool GoToLabel(const char *pLabel);

	/* list processing */

	const char *FindKnownKeyByID(int nKeyID);
	bool GetEventType();
	int GetCurveType();

	/* static */

	static void FindAnimCurveData();

	/* access */

	CCutSceneInterpreter *Next() const					{ return(d_pNext); }
	const INTERPRETER_TYPE Type() const					{ return(d_Type); }
	const int KeyID() const									{ return(d_nKeyID); }	// from "known keys" enum, above
	const KEY_TYPE KeyType() const						{ return(d_KeyType); }	// int, float, string
	const int KeyValueInt() const							{ ASSERT(d_KeyType == KEY_INT); return(d_nKeyValue); }
	const float KeyValueFloat() const					{ ASSERT(d_KeyType == KEY_FLOAT); return(d_fKeyValue); }
	const char *KeyValueString() const					{ ASSERT(d_KeyType == KEY_STRING); return(d_cKeyValue); }
	char *CurrentData() const								{ return(d_pData); }
	void SetData(char *pData)								{ d_pData = pData; }
	void SetFlag(u32 u32Flags)								{ d_u32Flags |= u32Flags; }
	void ClearFlag(u32 u32Flags)							{ d_u32Flags &= ~u32Flags; }
	bool IsFlagSet(u32 u32Flags) const					{ return((d_u32Flags & u32Flags) != 0); }
	static void SetAnimDataHandle(t_Handle Handle)	{ d_AnimDataHandle = Handle; }
	static t_Handle GetAnimDataHandle()					{ return(d_AnimDataHandle); }
	const int GetIndex() const								{ return(d_nIndex); }
	const int GetParentIndex() const						{ return(d_nParentIndex); }
	const CCutSceneInterpreter *GetParent() const	{ return(d_pParent); }
	void SetParent(CCutSceneInterpreter *pParent)	{ d_pParent = pParent; }
	CActor *GetActor() const								{ return(d_pCActor); }

protected:

	/* next interpreter-- chain them together for memory release purposes */

	CCutSceneInterpreter *d_pNext;
	CCutSceneMgr *d_pCutSceneMgr;		// cut scene manager that owns interpreter

	/* parent interpreter-- implement hierarchical animation */

	int d_nIndex;	// export index, so we can hook up to parent
	int d_nParentIndex;
	CCutSceneInterpreter *d_pParent;

	/* type of data interpreter is working on-- an interpreter can read any kind of data, but each data stream */
	/* is a particular type for stronger error checking */

	INTERPRETER_TYPE d_Type;
	const char *d_pTypeLabel;	// for error reporting convenience

	/* current data pointer */

	char *d_pData;
	char *d_pDataEnd;

	/* each interpreter typically tracks something */

	CActor *d_pCActor;
//	ts_Actor *d_pLoadScreenActor;
	PathCamera *d_pPathCamera;
	GeneralCamera *d_pGeneralCamera;
	t_Handle d_Handle;
	SigGen *d_pSignalGenerator1;
	SigGen *d_pSignalGenerator2;
	float d_fKFMStartTime;
	u16 d_u16AnimFrames;

	/* position, rotation, and scale typically get controlled by curves and then built into a transform matrix */

	Vector3 d_vPosition;
	Vector3 d_vRotationRadians;
	Vector3 d_vScale;

	/* other stuff that can be controlled by curves-- not all interpreters may use all of these */

	float d_fFocalLength;
	float d_fVerticalAperture;
	float d_fHorizontalAperture;
	ts_fRGB d_Color;

	/* a transform. for particle emitters, we can control the ejection direction by rotating the emitter (used by */
	/* calling ParticleMgr__SetOneShotEmitterVelTransform with a pointer to this). for dummy parents, we can implement */
	/* hierarchical animation */

	Graphics4x4 d_mTransform;
	
	/* next time code at which there is action to take */

	float d_fNextTimecode;

	/* some flags */

public:

	typedef enum
	{
		PATH_CAMERA				= 0x00000001,
		GENERAL_CAMERA			= 0x00000002,
		ALL_CAMERAS				= PATH_CAMERA | GENERAL_CAMERA,
		TRANSFORM_XZY			= 0x00000004,	// if not set, transform x-y-z
		DELETE_ACTOR			= 0x00000008,	// need to delete actor when cut scene is over
		WAS_STATIONARY			= 0x00000010,	// for restoring actor's IsEnvStationary state after cut scene
		WAS_PROTAGONIST		= 0x00000020,	// for restoring actor's collision state
		WAS_ENV_ACTIVE			= 0x00000040,	// for restoring environmental actor's EnvActive state
		PERMANENT				= 0x00000080,	// permanent interpreter set up at CutSceneMgr construction
		PERMANENT_AVAILABLE	= 0x00000100,	// permanent, but not in use yet
		EXECUTED					= 0x00000200,	// has been executed this frame
		AMBIENT_LIGHT			= 0x00000400,	// light type
		DIRECTIONAL_LIGHT		= 0x00000800,	// light type
		LOCAL_LIGHT				= 0x00001000,	// light type
		RESTORE_TO_BASE_POSE	= 0x00002000,	// for already-existing actor, restore to base pose at end of cut scene
		HAS_EXECUTED			= 0x00004000,	// may want to take special action the first call to Execute
		CLUB_VISIBLE			= 0x00008000,	// club needs to be visible during this cut scene
		RATTLE_VISIBLE			= 0x00010000,	// rattle needs to be visible during this cut scene
		BLOWGUN_VISIBLE		= 0x00020000,	// blowgun needs to be visible during this cut scene
		KFM_PLAYING				= 0x00040000,	// actor is playing a kfm
		KFM_LOOPING				= 0x00080000,	// actor is playing a looping kfm
	};

protected:

	u32 d_u32Flags;

	/* for convenience, number of entries in static lists */

	static int d_nKeyListSize;
	static int d_nInterpreterTypeSize;

	/* for logging chunks that contain anim curve data */

#define MAX_ANIM_CURVE_CHUNKS 10
	static void *d_pAnimCurveChunkData[MAX_ANIM_CURVE_CHUNKS];
	static int d_nAnimCurveChunkCount;

	/* additional anim data */

	static t_Handle d_AnimDataHandle;

	/* filled in by GetKeyValuePair */

#define MAX_KEY_LENGTH 100
	char d_Key[MAX_KEY_LENGTH];
	int d_nKeyID;
	KEY_TYPE d_KeyType;
	int d_nKeyValue;
	float d_fKeyValue;
	char d_cKeyValue[MAX_KEY_LENGTH];
	int d_nEvent;

	/* data may be driven by animation curves (keyframed curves directly from maya, as opposed to paths and */
	/* existing motion data). permanent interpreters (to support load screen, see comment by MAX_PERMANENT_INTERPRETERS */
	/* #define, below) also have pre-malloc'd InterpreterCurve buffers, of MAX_PERMANENT_INTERPRETER_CURVES size */

//MAX_PERMANENT_INTERPRETER_CURVES has to be high since the permanent interpreters are used during normal cut scenes too
//and this has to work for the worst-case scenario. the alternative is to have a d_pPermanentInterpreterCurve buffer that
//is smaller, handling only the load screen worst case scenario, and malloc'ing another for cut scenes if the required
//number of curves is larger than MAX_PERMANENT_INTERPRETER_CURVES. i'm going to leave it as is for now, since the total
//memory cost is low-- there is only a small number of permanent interpreters
#define MAX_PERMANENT_INTERPRETER_CURVES	20
	ts_InterpreterCurve *d_pInterpreterCurve;
	int d_nInterpreterCurveMaxCount;	// how many were malloc'd
	int d_nInterpreterCurveCount;		// currently used
};

/*** cut scene manager ***/

class CCutSceneMgr
{
public:

	/* Constructor/destructor */

	CCutSceneMgr();
	~CCutSceneMgr();
	bool Cleanup(bool bDeletePermanent = false);
	void DeallocInterpreters();
	CCutSceneInterpreter *AllocInterpreter();
	void DeallocInterpreter(CCutSceneInterpreter *pInterpreter);

public:

	/* flags for d_u32Flags */

	enum
	{
		/* fade flags */

		CUT_SCENE_FADE_NONE							= 0x00000000,
		CUT_SCENE_FADE_IN								= 0x00000001,	// fade out of game, into cut scene
		CUT_SCENE_FADE_OUT							= 0x00000002,	// fade out of cut scene, back to game
		CUT_SCENE_WAIT_FOR_FADE_OUT				= 0x00000004,	// CCutSceneMgr::Update should wait for game fade out to finish before starting cut scene

		/* other stuff */

		CUT_SCENE_LOAD_SCREEN						= 0x00000008,	// cut scene being played during load screen
		CUT_SCENE_LEAVE_CAMERA						= 0x00000010,	// don't restore camera when cut scene ends
		CUT_SCENE_INTERRUPTABLE						= 0x00000020,	// cut scene can be interrupted by a button press
		CUT_SCENE_AMBIENT_LIGHTS_REMOVED			= 0x00000040,	// cut scene contains new ambient light, original one needs to be restored
		CUT_SCENE_DIRECTIONAL_LIGHTS_REMOVED	= 0x00000080,	// cut scene contains new directional lights, original ones need to be restored
		CUT_SCENE_DISABLE_BLACK_BARS				= 0x00000100,	// no black cinema bars at top and bottom of screen
		CUT_SCENE_LIGHT_SET_ACTIVE					= 0x00000200,	// there are two light managers-- LightMgr and CLightSet
		CUT_SCENE_LOADING								= 0x00000400,	// cut scene is loading (on cdrom, it's asynchronous)
		CUT_SCENE_PLAYING								= 0x00000800,	// something for IsCutScenePlaying to check
		CUT_SCENE_CLEANUP_INTERRUPT				= 0x00001000,	// set if Cleanup call at start of PlayCutScene interrupted a cut scene-- should happen in preview only, i think
		CUT_SCENE_PREVIEW								= 0x00002000,	// preview mode for cut scene artists
		CUT_SCENE_QUEUE_AUDIO						= 0x00004000,	// on cdrom, need to queue audio before cut scene
		CUT_SCENE_WAIT_FOR_AUDIO_START			= 0x00008000,	// on cdrom, wait for audio to start, then start cut scene
		CUT_SCENE_HAS_EXECUTED						= 0x00010000,	// for identifying the first time that the cut scene executes interpreters
		CUT_SCENE_DONT_SAVE_CAMERA					= 0x00020000,	// use previously saved camera
	};

	/* simple routines */

	void SetLoadScreen(bool bLoadScreen)				{ if (bLoadScreen) d_u32Flags |= CUT_SCENE_LOAD_SCREEN; else d_u32Flags &= ~CUT_SCENE_LOAD_SCREEN; }
	void SetLeaveCamera(bool bLeaveCamera)				{ if (bLeaveCamera) d_u32Flags |= CUT_SCENE_LEAVE_CAMERA; else d_u32Flags &= ~CUT_SCENE_LEAVE_CAMERA; }
	void SetDontSaveCamera(void)							{ d_u32Flags |= CUT_SCENE_DONT_SAVE_CAMERA; }
	CCutSceneInterpreter *GetInterpreterHead()		{ return(d_pInterpreterHead); }
	bool IsCutScenePlaying()								{ return((d_u32Flags & CUT_SCENE_PLAYING) != 0); }
	bool IsCutSceneWaitingForAudio()						{ return((d_u32Flags & CUT_SCENE_WAIT_FOR_AUDIO_START) != 0); }
	bool IsCutSceneExecuting()								{ return((d_u32Flags & CUT_SCENE_HAS_EXECUTED) != 0); }	// starts executing after possible fade out from gameplay
	u32 GetFlags()												{ return(d_u32Flags); }
	void SetFlags(u32 u32FlagBits)						{ d_u32Flags |= u32FlagBits; }
	void ClearFlags(u32 u32FlagBits)						{ d_u32Flags &= ~u32FlagBits; }
	int GetCameraIndex()										{ return(d_nCameraIndex); }
	void SetAudioName(const char *pAudioName)			{ strcpy(d_AudioName, pAudioName); }
	float GetKFMStartTimeAdjust()							{ return(d_fKFMStartTimeAdjust); }

	/* play a cut scene */

	bool PlayCutScene(const char *pName, bool bInterruptable = false, int nCameraIndex = 0, bool bBlackBars = true);
	bool FinishLoad();	// sort of a PlayCutScene, part 2
	bool StopCutScene(bool bOverrideInterruptable = false);
	void Update();	// called by SimLoop::Run once per frame
	static void DialogStartedCallback(void *context, SoundStream *stream);

	/* these are available for use outside the cut scene manager if desired */

	void AddBlackCinemaBars();
	void RemoveBlackCinemaBars();
	void RestoreCamera();

protected:

	bool InitInterpreters();
	void StartGeneralCamera();

	/* for cdrom async load */

	MediaFile *d_pMediaFile;
	void *d_pDBLBuffer;
	int d_nDBLBufferSize;

	/* current cut scene being played */

	char *d_pCutSceneData;
	char *d_pCutSceneDataEnd;
	float d_fCutSceneStart;
	float d_fCutSceneLength;
	float d_fCutSceneTimer;

	/* for development-- artists preview cut scene in engine by calling for a particular cut scene */

	t_Handle d_CutDataHandle;
	t_Handle d_CurveDataHandle;
	t_Handle d_AnimDataHandle;

	/* flags */

	u32 d_u32Flags;

	/* handle black cinema bars at top and bottom of screen */

	t_Handle d_TopSpriteHandle;
	t_Handle d_BottomSpriteHandle;

	/* interpreters for this data */

	CCutSceneInterpreter *d_pInterpreterHead;
	int d_nCutSceneDataStreamCount;	// just informational

	/* keep track of camera/scene status before start of cut scene so we can return to it */

	int d_nCameraIndex;	// index into g_CameraSystem
	uint d_SaveCameraID;
	Vector3 d_vSaveGeneralUp;
	float d_fSaveFieldOfViewY;
	float d_fSaveAspectRatio;
	
	/* .wav file string, if any, from sound datastream. this is for cdrom builds that force the */
	/* .wav file to start at the beginning of the cut scene since there's no other way to sync them */

	char d_AudioName[32];
	int d_nAudioTimeOut;

	/* some cut scenes, for reasons unknown, need global animation time adjustment */

	float d_fKFMStartTimeAdjust;

	/* simplified cut scenes are also used for loading screen. because the loading screen can't do any malloc's */
	/* or else it may screw up memory available for SimLoop::Initialize) we set up some permanent interpreters */

#define MAX_PERMANENT_INTERPRETERS	5	// file, tak, camera, particle emitter, particle effects
	CCutSceneInterpreter *d_pPermanentInterpreter[MAX_PERMANENT_INTERPRETERS];
};



#endif // __CUTSCENEMGR_H
