/*************************************/
/*                                   */
/*   g_ParticleMgr.h                 */
/*   big juju prototype  05/24/99    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   particle manager                */
/*                                   */
/*************************************/

#ifndef __G_PARTICLEMGR_H
#define __G_PARTICLEMGR_H

/******************* includes *****************************/

/* system includes */

/* engine includes */

#include "Layers/LayersStruct.h"			// low-level include
#include "Game/Managers/ParticleDB.h"	// for MAX_E_LOD_RADIUS
#include "Math/plane.h"

/******************* forward declarations ****************/

struct ts_RecordList;		// from DBFile.h
struct ts_TextureSet;		// from TextureMgr.h
class CScene;				// from SceneMgr.h
class SplinePath;				// from Gamelib\Path\SplinePath.h

/******************* defines *****************************/

/* this effect ID can be passed to an emitter to make it temporarily inactive. permanently inactive
emitters should be killed by user */

#define NO_PARTICLE_EFFECT				0xffffffff

/* for vu1 particle renderer */

#define PARTICLE_INIT_DATA_DMA_QWC			29	// amount of data to be dma'd to particle setup data area-- must match defines in Particles.vsm
#define PARTICLE_INIT_DATA_TOTAL_QWC		54	// total particle setup data-- must match defines in Particles.vsm
#define PARTICLE_INIT_BUFFER_SIZE			(1024 - PARTICLE_INIT_DATA_TOTAL_QWC) / 2		// total vu data ram is 1024 words
#define PARTICLE_DATA_HEADER_QWC				5															// four gs registers and a control word
#define PARTICLE_INIT_PACKET_QWC				(PARTICLE_INIT_DATA_DMA_QWC + 8)		// how much to reserve on scratchpad in FastVars.cpp
#define PARTICLE_DATA_PACKET_QWC				(PARTICLE_DATA_HEADER_QWC + 5)		// how much to reserve on scratchpad in FastVars.cpp
#define PARTICLE_INIT_MATRIX_QWC				4	// where matrix data goes in init packet
#define PARTICLE_INIT_CAMERA_QWC				18	// where camera's position in world space goes in init packet
#define PARTICLE_INIT_LIGHT_QWC				19	// where light data goes in init packet

/* the following relationships should be true (some constants come from Particles.vsm):

	PARTICLE_INIT_DATA_DMA_QWC = GIFTAG1 + 1
	GIFTAG2 = GIFTAG1 + OUTBUF_SIZE
	PARTICLE_INIT_DATA_TOTAL_QWC = GIFTAG2 + OUTBUF_SIZE

*/

/* number of particles that will fit in one dma packet. vu ram is double buffered, plus there is a INIT_DATA section set
aside before the remaining ram is divided into 2. each particle takes 3 quad words in vu ram-- vertex, velocity/rotation,
color. the color data is 4 bytes per particle, packed together 4 per dma quad word, and gets unpacked correctly to one
quad word per particle. however, the vif unpacks 4 colors per quad word of dma data. for example, if there is 1 color, the 
vif will still unpack 4. so the max. particles count has to take that into account or else the empty colors in a dma quad word
will get unpacked past the end of the target buffer */

#define MAX_PARTICLES_PER_RENDER_PACKET	(((PARTICLE_INIT_BUFFER_SIZE - PARTICLE_DATA_HEADER_QWC) / 3) & ~3)	// "/ 3" since each particle is 3 vu ram words. "& ~3" is for color alignment

/* default value for emitter group */

#define EMITTER_DEFAULT_GROUP	0

/* number of particle data files that can be loaded at one time. calls to routines like ParticleMgr__CreateEffect don't
have to know about loaded files-- they typically reference effects by name, and the routine takes care of figuring out
which particle file is being used. the particles from each file are kept in separate lists, so that we can support
dynamic texture sets on the ps2 without having to somehow merge texture sets from the different particle files into one
dynamic texture set. this also supports the possibility of loading a new particle data file at some later time, or 
removing an existing one */

#define MAX_PARTICLE_DATA_FILES	20

/******************* macros ******************************/

/******************* structures **************************/

/* collision test callback */

typedef bool (*t_CollisionTest)(Vector3 *, Vector3 *, Vector3 *, Vector3 *, u32 *, bool, bool);

/* for particle callbacks on death */

typedef void (*t_ParticleDeathCallback)(void *, t_Handle);

/******************* global variables ********************/

/******************* global prototypes *******************/

int ParticleMgr__Init(int nMaxParticles, int nMaxParticleEmitters, int nMaxParticleCreateRecords);
void ParticleMgr__Reset(void);
void ParticleMgr__Shutdown(void);
void ParticleMgr__SetOptionalNameExtension(const char* extension); // valid until Reset() or Shutdown() or replaced
#ifndef CONSUMER_BUILD
bool ParticleMgr__IsEmpty(void);
#endif //CONSUMER_BUILD
void ParticleMgr__Enable(void);
void ParticleMgr__Disable(void);
void ParticleMgr__Toggle(void);
void ParticleMgr__Lock(bool bLocked);
void ParticleMgr__Pause(void);
void ParticleMgr__UnPause(void);
void ParticleMgr__KillAllParticles(t_Handle Handle = INVALID_HANDLE);
void ParticleMgr__KillAllEmitters(t_Handle Handle = INVALID_HANDLE);
void ParticleMgr__KillEmitterGroup(u16 u16Group, t_Handle Handle = INVALID_HANDLE);
t_Handle ParticleMgr__LoadData(char *pFilename);
void ParticleMgr__UnloadData(t_Handle Handle);
bool ParticleMgr__FindEffectByName(const char *pName, int &nDataIndex, int &nEffectIndex);
bool ParticleMgr__FindEmitterByName(const char *pName, int &nDataIndex, int &nEmitterIndex);
bool ParticleMgr__FindParticleByName(const char *pName, int &nDataIndex, int &nParticleIndex);
bool ParticleMgr__FindSwooshByName(const char *pName, int &nDataIndex, int &nSwooshIndex);
bool ParticleMgr__FindSwooshEmitterByName(const char *pName, int &nDataIndex, int &nSwooshEmitterIndex);
int ParticleMgr__GetActiveParticleCount(void);
int ParticleMgr__GetActiveEmitterCount(void);
int ParticleMgr__GetMaxParticleCount(void);
int ParticleMgr__GetMaxEmitterCount(void);
int ParticleMgr__GetDataIndex(t_Handle Handle);
void ParticleMgr__LockTextureSet(int nDataIndex);
void ParticleMgr__UnLockTextureSet(int nDataIndex);
void ParticleMgr__RegisterCollisionTest(t_CollisionTest CollisionTest);
void ParticleMgr__Advance(void);
void ParticleMgr__PrepareRender(void);
void ParticleMgr__Render(CScene *pScene);
bool ParticleMgr__CreateEffect(
	const char *pEffectName,
	const Vector3Padded *pvPos,
	const Vector3Padded *pvVelUnitVector = NULL,
	const float *pfVelMagnitude = NULL,
	const Vector3Padded *pvAdditiveVel = NULL,
	const float fFirstAdvanceTime = 0.0f,
	const float fScale = 1.0f,
	const int nScaleRandomPct = 0,
	const float fConstantAlpha = 1.0f,
	const float fColorMult = 1.0f,
	const u32 u32ForceFlags = 0,
	const int iSceneIndex = -1,
	const CPlaneList *pPlaneList = NULL,
	const SplinePath *pPath = NULL);
bool ParticleMgr__CreateEffect(
	const char *pEffectName,
	const Vector3Packed *pvPos,
	const Vector3Packed *pvVelUnitVector = NULL,
	const float *pfVelMagnitude = NULL,
	const Vector3Packed *pvAdditiveVel = NULL,
	const float fFirstAdvanceTime = 0.0f,
	const float fScale = 1.0f,
	const int nScaleRandomPct = 0,
	const float fConstantAlpha = 1.0f,
	const float fColorMult = 1.0f,
	const u32 u32ForceFlags = 0,
	const int iSceneIndex = -1,
	const CPlaneList *pPlaneList = NULL,
	const SplinePath *pPath = NULL);
t_Handle ParticleMgr__CreateOwnedParticle(
	const char *pParticleName,
	const Vector3Padded *pvPos,
	const Vector3Padded *pvVelUnitVector,
	const float *pfVelMagnitude,
	const Vector3Padded *pvAdditiveVel,
	const t_ParticleDeathCallback DeathCallback,
	void *context,
	const float fScale = 1.0f,
	const float fConstantAlpha = 1.0f);
void *ParticleMgr__GetParticleSource(int nDataIndex, int nEntryIndex);
void *ParticleMgr__GetEffectSource(int nDataIndex, int nEntryIndex);
void *ParticleMgr__GetEmitterSource(int nDataIndex, int nEntryIndex);
void *ParticleMgr__GetSwooshSource(int nDataIndex, int nEntryIndex);
void *ParticleMgr__GetSwooshEmitterSource(int nDataIndex, int nEntryIndex);
t_Handle ParticleMgr__GetDBHandle(int nDataIndex);					// for pigpen
ts_TextureSet *ParticleMgr__GetTextureSet(int nDataIndex);	// for pigpen
u16 ParticleMgr__RequestReservedEmitterGroupID(void);
void ParticleMgr__AdjustLights(float fAmbientMult, float fDirectionalMult, float fLeadInSeconds, float fSustainSeconds, float fLeadOutSeconds);

/* one-shot setting. i feel like this is a hack, but i can't come up with anything cleaner. the issue is that there are */
/* rarely-used fields in emitters/effects. i could add them to the end of the creation routines as default parameters, */
/* but we get up to 5 or 6 default parameters, which makes getting to the last, rarely-needed one painful and prone */
/* to error (e.g. you have to know the default values to use for the intervening parameters). at first my solution was */
/* to have a separate routine to call AFTER creation, but not all data structures are accessible after creation */
/* (e.g. creating an effect doesn't return a handle). so have values that you can register with the particle manager to */
/* be used the next time the creation routine is called to handle the rarely needed fields */

void ParticleMgr__SetOneShotEmitterVelTransform(Graphics4x4 *pVelTransform);
void ParticleMgr__SetOneShotEmitterTarget(Vector3Packed *pvTarget);
void ParticleMgr__SetOneShotEffectVelTransform(Graphics4x4 *pVelTransform);
void ParticleMgr__SetOneShotEffectTarget(Vector3Packed *pvTarget);
void ParticleMgr__SetOneShotOwnedParticleVelTransform(Graphics4x4 *pVelTransform);
void ParticleMgr__SetOneShotOwnedParticleEffectTarget(Vector3Packed *pvTarget);

/*** particle routines-- used for owned particles, which are currently the only ones for which handles are made public ***/

bool Particle__SetPosition(const t_Handle Handle, const Vector3 *pvPos);
Vector3 Particle__GetPosition(const t_Handle Handle);
bool Particle__SetVelocity(const t_Handle Handle, const Vector3 *pvVel);
Vector3 Particle__GetVelocity(const t_Handle Handle);
bool Particle__SetScale(const t_Handle Handle, const float fScale);
bool Particle__SetRotation(const t_Handle Handle, const Vector3 *pvRotRadians);
bool Particle__SetRotationVelocity(const t_Handle Handle, const Vector3 *pvRotVelRadians);
bool Particle__SetNormal(const t_Handle Handle, const Vector3 *pvNormal);
bool Particle__SetColor(const t_Handle Handle, const ts_bRGBA Color);
#ifdef _DEBUG
bool Particle__MakeItGoodForTrails(const t_Handle Handle, const float fSeconds);	// for debugging, particles can be useful for showing trails
#endif //_DEBUG
bool Particle__Kill(const t_Handle Handle, const bool bCallbackOK);	// see note at implementation about bCallbackOK parameter
bool Particle__SetOrbitVelocity(const t_Handle Handle, const float fVel);
float Particle__GetOrbitVelocity(const t_Handle Handle);

/*** particle emitter routines ***/

t_Handle ParticleEmitter__CreateFromRecord(
	const char *pEmitterName,
	const Vector3Padded *pvEmitterPos,
	const Vector3Padded *pvEmitterVel,
	const Vector3Padded *pvEjectVel,
	bool bEjectVelChanges,
	const Vector3Padded *pvUp,
	bool bStationary,
	u16 u16Group = EMITTER_DEFAULT_GROUP,
	int iSceneIndex = -1);
t_Handle ParticleEmitter__CreateFromRecord(
	const char *pEmitterName,
	const Vector3Packed *pvEmitterPos,
	const Vector3Packed *pvEmitterVel,
	const Vector3Packed *pvEjectVel,
	bool bEjectVelChanges,
	const Vector3Packed *pvUp,
	bool bStationary,
	u16 u16Group = EMITTER_DEFAULT_GROUP,
	int iSceneIndex = -1);
bool ParticleEmitter__FinishOrbiter(t_Handle Handle, float fRadius, float fAngularVelRadians, const Vector3Packed *pvUp);
bool ParticleEmitter__UpdateTireEmitter(t_Handle Handle, char *pEmitterName);
bool ParticleEmitter__Activate(t_Handle Handle);
bool ParticleEmitter__Deactivate(t_Handle Handle);
bool ParticleEmitter__IsActive(t_Handle Handle);
bool ParticleEmitter__InScene(t_Handle Handle);
bool ParticleEmitter__Lock(t_Handle Handle);
bool ParticleEmitter__Unlock(t_Handle Handle);
bool ParticleEmitter__IsLocked(t_Handle Handle);
bool ParticleEmitter__ChangeEffect(t_Handle Handle, char *pEffectName);
#ifdef _DEBUG
bool ParticleEmitter__IsTireType(t_Handle Handle);
#endif //_DEBUG
bool ParticleEmitter__UpdateDelay(t_Handle Handle, float fDelay, bool bPreserveOriginalDelay = false);
bool ParticleEmitter__ClearTimer(t_Handle Handle);
bool ParticleEmitter__ResetLastPos(t_Handle Handle);
bool ParticleEmitter__Kill(t_Handle Handle);
bool ParticleEmitter__SetScale(t_Handle Handle, float fScale, int nRandomPct = 0);
bool ParticleEmitter__SetConstantAlpha(t_Handle Handle, float fConstantAlpha);
bool ParticleEmitter__SetConstantAlpha(t_Handle Handle, float *pfConstantAlpha);	// point off to external value
bool ParticleEmitter__SetColorMult(t_Handle Handle, float fColorMult);
bool ParticleEmitter__SetColorMult(t_Handle Handle, float *pfColorMult);	// point off to external value
bool ParticleEmitter__SetPosition(t_Handle Handle, Vector3CRef position);
Vector3 ParticleEmitter__GetPosition(t_Handle Handle);
bool ParticleEmitter__SetLifetime(t_Handle Handle, float fLifetime);
float ParticleEmitter__GetLifetime(t_Handle Handle);
bool ParticleEmitter__SetPlaneList(t_Handle Handle, CPlaneList* pPlaneList, bool bMemFree = true);
bool ParticleEmitter__SetFluxSize(t_Handle Handle, float fFluxSize);
bool ParticleEmitter__SetFluxCeiling(t_Handle Handle, float fFluxCeiling);
bool ParticleEmitter__SetEjectVelPtr(t_Handle Handle, Vector3Packed *pEjectVel);
bool ParticleEmitter__SetEjectVel(t_Handle Handle, Vector3CRef ejectVel);
bool ParticleEmitter__AdvancesWhileManagerLocked(t_Handle Handle);
bool ParticleEmitter__FreezeForOneMoreFrame(t_Handle Handle);
bool ParticleEmitter__SetParticlePath(t_Handle Handle, SplinePath *pPath);
bool ParticleEmitter__HasParticlePath(t_Handle Handle);
bool ParticleEmitter__UseOSTimer(t_Handle Handle, bool bUseOSTimer = true);
bool ParticleEmitter__SetLOD(t_Handle Handle, float fLOD1Radius, float fLOD2Radius);


#ifndef CONSUMER_BUILD
int ParticleMgr__GetActiveCount(void);
int ParticleMgr__GetRenderedCount(void);
#endif //CONSUMER_BUILD



#endif // __G_PARTICLEMGR_H
