///////////////////////////////////////////////////////////////////////////////
// Useful wrapper for the full screen distort stuff
///////////////////////////////////////////////////////////////////////////////
#ifndef FSDSYS_H
#define FSDSYS_H

#include "Effects/fullscreendistort.h"
#include "EngineHelper/Singleton.h"

#define FSD_CENTERX 0.46f
#define FSD_CENTERY 0.5f

// distort flags
#define DISTORT_FLAGS_USE_OSTIMER			0x00000002
#define DISTORT_FLAGS_OWNERDELETE 			0x00000004  // Use this if you EVER touch it after the create
																		// and make sure to call delete on the ptr after
																		// StopEffect

class CFSDEffect
{
public:
	CFSDEffect( const char *pEffectName, const Vector3 *pos, int flags );
	~CFSDEffect();

	void InitEffect( void );
	void Calculate( void );
	void StopEffect( void );
	bool Update( void );

	// Is this an auto delete?
	bool AutoDelete( void )
		{ return ((distortFlags & DISTORT_FLAGS_OWNERDELETE) == 0); }

	// state of the effect
	bool IsActive()
		{ return GetState() != FSDFX_DONE; }
	enum State
	{
		FSDFX_ATTACK,
		FSDFX_SUSTAIN,
		FSDFX_DECAY,
		FSDFX_DONE
	};
	State GetState();

	// Play with the effect (dear god, use OWNERDELETE if you play with these after
	// the first frame.)
	void SetLoopedSustain(bool loopSustainOn)
		{ loopSustain = loopSustainOn; }
	bool GetLoopedSustain()
		{ return loopSustain; }

	// Reset the effect to sustain
	bool ResetToSustain();

	void DisplayOn( bool flag );
	void InitParams( void );

	void RegisterWatchVariables( const char *context );
	static void SaveDistortContext( int command, void* param );
	static void RefreshDistortion( int command, void* param );
	static void InitChanged(void* param);
	static void XChanged(void* param);
	static void YChanged(void* param);
	void AddDebugVar( const char *varName, int *var, int low, int high, int step );
	void AddDebugVar( const char *varName, float *var, float low, float high, float step );
	void AddDebugVarX( const char *varName, int *var, int low, int high, int step );
	void AddDebugVarX( const char *varName, float *var, float low, float high, float step );
	void AddDebugVarY( const char *varName, int *var, int low, int high, int step );
	void AddDebugVarY( const char *varName, float *var, float low, float high, float step );

	// EventHandler implementation
	void StuffChanged(u32 u32Event);

	char currentContext[128];
	float fRefreshTimer;

private:
	BaseMenu *pDistortMenu;
	const Vector3 *worldPos;
	int distortFlags;
	u32 dHandle1;
	u32 dHandle2;
	bool bDisplayOn;
	bool bCenterOnScreen;
	bool loopSustain;

	// for calculating params
	float effectAttackTimer;
	float effectSustainTimer;
	float effectDecayTimer;
	ts_ABGR cColor;
	float tempOpacity;
	ts_ABGR cpColor;
	float temp_cpInnerRadius;
	float temp_cpOuterRadius;
	float temp_xInnerRadius;
	float temp_xOuterRadius;
	float temp_yInnerRadius;
	float temp_yOuterRadius;

	// init params
	int gridX, gridY;
	float opacity;
	int mode;	//FullScreenDistort::MODE	mode;

	// color params
	int cColorA;
	int cColorR;
	int cColorG;
	int cColorB;

	// color point params
	float cpCenterX, cpCenterY;
	float cpInnerRadius, cpOuterRadius;
	int cpColorA;
	int cpColorR;
	int cpColorG;
	int cpColorB;
	int cpCenterOnInitiator;

	// x grid params
	int xOn;
	float xCenterX, xCenterY;
	float xInnerRadius, xOuterRadius;
	int xInverseArea;
	float xAttackTime;
	float xSustainTime;
	float xDecayTime;
	int xUseOmega;
	int xFeedBack;
	int xSigType;	//SigGen::TYPE xSigType;
	float xSigFreq;
	float xSigAmp;
	int xCenterOnInitiator;

	// y grid params
	int yOn;
	float yCenterX, yCenterY;
	float yInnerRadius, yOuterRadius;
	int yInverseArea;
	float yAttackTime;
	float ySustainTime;
	float yDecayTime;
	int yUseOmega;
	int yFeedBack;
	int ySigType;	//SigGen::TYPE ySigType;
	float ySigFreq;
	float ySigAmp;
	int yCenterOnInitiator;

	// effects params
	float effectAttackTime;
	float effectSustainTime;
	float effectDecayTime;
	float startOpacity;
	int cStartColorA;
	int cStartColorB;
	int cStartColorG;
	int cStartColorR;
	float cpStartInnerRadius;
	float cpStartOuterRadius;
	int cpStartColorA;
	int cpStartColorB;
	int cpStartColorG;
	int cpStartColorR;
};

class CFSDSystem
{
public:
	CFSDSystem( void );
	~CFSDSystem();

	CFSDEffect *StartEffect( const char *pEffectName, const Vector3 *pos = NULL, int flags = 0);
	void StopEffect( CFSDEffect *pDistort );
	bool IsBusy( void );
	void Update( void );

	// EventHandler implementation
#ifdef ENABLE_WATCHCLIENT
	CMessageOwner m_messageOwner;
#endif

private:
	CFSDEffect 						*pDistort;
	DynamicArray<CFSDEffect*> 	m_distortions;	// filters to apply to the camera.

#ifdef ENABLE_WATCHCLIENT
	REF_BASIC_MESSAGE_HANDLER(WatchHandler, MESSAGE_VALUE, CFSDSystem) m_watchHandler;
#endif
};

// There can be only one!
DefineSingleton(CFSDSystem)
#define g_fsdSystem GetSingleton(CFSDSystem)

#endif
