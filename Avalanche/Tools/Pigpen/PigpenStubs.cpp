/*************************************/
/*                                   */
/*   PigpenStubs.cpp                 */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   fix-ups for pigpen              */
/*                                   */
/*************************************/

/* define stub versions of unresolved external symbols-- pigpen does not include the whole game engine */

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "PigpenPCH.h"
#include <Layers/Media.h>
#include <Game/Animation/Bone.h>
#include <Game/Managers/SceneMgr.h>
#include <CmdCon/CmdCon.h>
#include <Effects/AHFMgr.h>
#include <Effects/FullScreenDistort.h>
#include <Effects/WorldSpaceDistort.h>
#include <Effects/FastNoise.h>
#include <EngineHelper/cinst.h>
#include <Effects/Water/WaterMgr.h>
#include <GameHelper/Event.h>
#include <gamehelper/loadsom.h>
#include <Platform/GameApp.h>
#include <Stage/Stage.h>
#include <Audio/AudioSys.h>
#include <audio/group/shandle.h>
#include <WatchClient/WatchClient.h>
#include <XML/XML.h>
#include <Environ/EnvDataMgr.h>
#include <Environ/Simple.h>
#include <CollideHelper/OBBChild.h>
#include <GameHelper/Compression/Compress2.h>
#include <Platform/Systemi.h>
#include <mathutil/Ballistic.h>
#include <GameHelper/Event.h>

/* these files are intended to be included by app, not compiled as part of the Stage project */

#include <Stage/EntityType.cpp>
#define GAME_ENTITY_CATEGORY_CPP
#include <Stage/EntityCategory.cpp>

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

Stage *g_stagePtr = NULL;
CommandConsole *g_consolePtr = NULL;
GAMEAPP_BASECLASS	*GAMEAPP_BASECLASS::me;
LoadSOM g_loadSOM;
WaterMgr *g_waterMgrPtr = NULL;
CEnvDataMgr g_EnvDataMgr;
Audio *g_audioPtr = NULL;
AniHFMgr *g_aniHFMgrPtr = NULL;
SystemInputDriver *g_inputPtr = NULL;
EventDispatcher g_eventDispatcher;

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

void CActor::AdvanceLightColorMult()
{
}

Vector3 CActor::GetBoneWorldStartPosition(BoneID boneID)
{
	static Vector3 v;
	v.Clear();
	return(v);
}

Vector3 CActor::GetBoneWorldEndPosition(BoneID boneID)
{
	static Vector3 v;
	v.Clear();
	return(v);
}

bool CActor::GetBoneVisibility(BoneID boneID)
{
	return(false);
}

ts_Bone *CProtagonist::GetOBBChildBone(unsigned int index)
{
	return(NULL);
}

void CProtagonist::UpdateOBBChildList()
{
}

bool Audio::LoadSoundBankChunk( void *pData, struct ts_DBChunkHdr_tag *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	return(true);
}

#ifdef _DEBUG
void SoundHandleTracker::Report(bool assertIfNotEmpty)
{
}
#endif //_DEBUG

xmlElement *xmlParseBuffer( unsigned char *buffer, unsigned int length, bool owns_data)
{
	return(NULL);
}

void xmlFreeElement(xmlElement *element)
{
}

WatchClient *WatchClient::Instance()
{
	return(NULL);
}

void WatchClient::Reset()
{
}

void WatchClient::RegisterVariable(char* context, float* var, float val, float varLow, float varHigh, char* txt, int event, EventHandler* eventDest)
{
}

void WatchClient::RegisterVariable(char* context, int* var, int val, float varLow, float varHigh, char* txt, int event, EventHandler* eventDest)
{
}

void WatchClient::RegisterVariable(char* context, int type, void* var, VAR_VAL val, float varLow, float varHigh, char* txt, int event, EventHandler* eventDest)
{
}

void MYCDECL CommandConsole::Notify(const char *format, ...)
{
}

void MYCDECL CommandConsole::Echo(const char *format, ...)
{
}

bool CommandConsole::ExecuteContext(const char *context)
{
	return(false);
}

bool CommandConsole::DeleteContext(const char *killContext)
{
	return(false);
}

bool CommandConsole::BindScript(const char *context, char *pathFile)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, int &var, bool hex, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, uint &var, bool hex, uint flags, bool isSigned, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, int32 &var, bool hex, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, uint32 &var, bool hex, uint flags, bool isSigned, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, int16 &var, bool hex, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, uint16 &var, bool hex, uint flags, bool isSigned, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, int8 &var, bool hex, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, uint8 &var, bool hex, uint flags, bool isSigned, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, float &var, CONREALTYPE type, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, float &var, bool invert , float convert, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, float &var, const char* type, TSimpleTransformFn setTransform, TSimpleTransformFn getTransform, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, float &var, const char* type, TArgTransformFn setTransform, TArgTransformFn getTransform, void* arg, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, float &var, float &invVar, CONREALTYPE type, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, bool &var, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateToggle(char *context, char *name, bool &var, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, char *var, int maxlen, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, Vector3Packed *var, bool normalize, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, Matrix4x4 &var, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

bool CommandConsole::CreateVar(char *context, char *name, Matrix3x3 &var, uint flags, int event, EventHandler* eventDest)
{
	return(false);
}

void CommandConsole::Error(const char *format, ...)
{
}

HOG_CHECKLOCAL GameApp::HogCheckLocal()
{
	return(CHECKLOCAL_BEFORE);	// this stub routine is used, must return true
}

LoadSOM::LoadSOM(void)
{
}

LoadSOM::~LoadSOM(void)
{
}

void WaterMgr::SpawnSpriteEffect(int effectNumber, int poolIdx, const Vector3& position, const Vector3& velocity)
{
}


int WaterMgr::Test(Segment &seg, Vector3* point, Vector3* flowVector)
{
	return(-1);	// no collision
}

void LoadSOM::AddVisRegion(CVisRegion *pVisRegion)
{
}

CEnvDataMgr::CEnvDataMgr()
{
}

bool AniHFMgr::Test(Segment &seg, AnimatedHeightField::Collision &collision,Vector3* flowVector)
{
	return(false);
}

void FullScreenDistort::Init(int _gridX, int _gridY, float _feedbackOpacity, MODE mode)
{
}
void FullScreenDistort::Shutdown(void)
{
}
u32 FullScreenDistort::Add(DIMENSION dim, float centerx, float centery, float innerradius, float outerradius, bool inversearea, float attacktime, float sustaintime, float decaytime, bool useOmega, bool feedBack, SigGen* generator)
{
	return(0);
}
void FullScreenDistort::RenderAll(void)
{
}
void FullScreenDistort::UpdateAll(void)
{
}

void WorldSpaceDistort::AdvanceAll()
{
}

void FastNoise::Setup(float frequency, int lookups)
{
}

U8* COMPRESS_DecompressBuffer( U8* compBuffer, U8 *decompBuffer )
{
	return(NULL);
}

S32 COMPRESS_GetUncompressedSize( U8* compBuffer )
{
	return(0);
}

CPartition::~CPartition()
{
}

FastNoise::~FastNoise()
{
}

Vector3 &CBallistic::Evaluate(float fSeconds)
{
	static Vector3 vPos;
	return(vPos);
}

ts_fRGB CActor::AmbientLight()
{
	static ts_fRGB Color;
	return(Color);
}

EventDispatcher::EventDispatcher(void)
{
}

EventDispatcher::~EventDispatcher(void)
{
}

void EventDispatcher::Reset()
{
}

