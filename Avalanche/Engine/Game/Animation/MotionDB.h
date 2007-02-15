//============================================================================
//=
//= MotionDB.h
//=
//============================================================================

#ifndef _MOTIONDB_H
#define _MOTIONDB_H

#include "platform/BaseType.h"			// low-level include
#include "Layers/LayersStruct.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif	//GCN

//============================================================================

struct ts_MotionHeader;			// from g_Motion.h
struct ts_MotionFileHeader;
class CActorInstance;

//============================================================================

t_Handle		motLoadMotionChunk(const char *pFilename, CActorInstance* inst = NULL, const int nIndex = 0);
t_Handle		motLoadMotionChunk(t_Handle Handle, CActorInstance* inst = NULL, const int nIndex = 0);
ts_MotionHeader*		GetMotionHeader(ts_MotionFileHeader *pMotDbEntry, u32 motnDefine);

//============================================================================

#endif //_MOTIONDB_H
