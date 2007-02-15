/* SCE CONFIDENTIAL
 $PSLibId$
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                         - Multistream -
 *
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Europe
 *                        All Rights Reserved.
 *
 *                            ms_switches.h
 *                  IOP code of Multistream's API
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *			7.0			11/June/2003	Jason Page
 */

#ifndef MS7_DSP
#define MS7_DSP

int SOUND_DSPCallback(int ch, void *common);
void SOUND_DSPProcess(void);
int SOUND_DSPWaitForVoiceTransComplete(void);
int SOUND_DSPThread(void);
int SOUND_InitDSPEngine(int value,int priority);
void SOUND_DSPSync(int spuchan);
void SOUND_DSP_InitFilter(int j);
void SOUND_DSP_Filter(int LR, short *data,int j);
void SOUND_DSP_InitDistortion(int j);
void SOUND_DSP_Distortion(int LR,short *data,int j);

void SOUND_DSP_InitCompressor(int j);
void SOUND_DSP_Compressor(int LR,short *data,short *data2,int j);
int SOUND_DSP_InitFlange(int j,int Depth, int Speed);
void SOUND_DSP_Flange(int LR,short *data,u_int j);
void SOUND_DSP_CloseFlange(int j);
void SOUND_WaitForDSPToComplete(void);

void SOUND_SetDSP(int slot,int type,int value);
void SOUND_DSP_InitNoiseGate(int j);
void SOUND_DSP_NoiseGate(int LR,short *data,int j);
void SOUND_DSP_MixCores(short *ptr1, short *ptr2, short *ptr3, short *ptr4);
void SOUND_DSP_RingMod(int LR,short *data,int j);
void SOUND_DSPOnOff(int core, int flag);
void SOUND_ClearDSPEffects(u_int core);
void SOUND_SetDSPEffect(u_int core,u_int offset, u_int type);
void SOUND_DSP_InitRingMod(int j);
void SOUND_DSP_DetectPitch(int core,short *data,short *data2,int j);
void SOUND_DSP_InitDetectPitch(int j);
void SOUND_SetDSPCount(u_int c);
u_int SOUND_AllocateEEDSP(u_int type,u_int packets);
void SOUND_AddDSPTransAddress(u_int addr1,u_int addr2,int counter);
void SOUND_TransferDSPEE(void);
int SOUND_FindDSPBlock(int f);
int SOUND_CheckDSPBlocks(void);
void SOUND_CloseDSP(void);
void SOUND_ReallyCloseDSP(void);
void SOUND_DSPInit(void);
void SOUND_DSPCopyCorrectBlock(void);
long long _FastFilter(int LowPass, int BPass, short *data, int ResAndFrq);

#endif