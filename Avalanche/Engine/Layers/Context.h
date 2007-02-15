#ifndef    __CONTEXT_H
#define    __CONTEXT_H

#include "platform/BaseType.h"

struct ts_Context
{
	unsigned char *pStackBase;
	unsigned char *pStack;
};

void ctxInit( ts_Context *pNewContext, void ( *pWakeAddr)( void), void *pLocalData, s32 s32StackSize, s32 s32LocalDataSize);

#if defined(__cplusplus)
extern "C" {
#endif
void ctxSwitch( ts_Context *pNewContext, ts_Context *pSaveContext);
void ctxKillSwitch( ts_Context *pNewContext );
void InterruptSaveRegisters();
void InterruptRestoreRegisters();
#if defined(__cplusplus)
}
#endif


#endif