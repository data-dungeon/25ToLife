/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                    - easy mpeg streaming -
 *
 *                         Version 0.30
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            audidec.c
 *                   functions for audio decoder
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.10           12.17.1999      umemura     the first version
 *       0.20           04.04.2000      kaol/ume    spu2 -> sd
 *       0.30           08.21.2001      mikedean    avoids touching IOP heap
 */
#include <eekernel.h>
#include <stdio.h>
#include <sif.h>
#include <sifdev.h>
#include <sifrpc.h>

#include <sdmacro.h>	// HAS defines for SD_P_MVOL etc..etc..
#include <string.h>
#include "Layers/mstream/mstream.h"

#include "audiodec.h"

#define AUTODMA_CH 		0
#define AU_HEADER_SIZE 	40
#define UNIT_SIZE 		1024
#define PRESET_VALUE(count)	(count)

static void iopGetArea(int *pd0, int *d0, int *pd1, int *d1, AudioDec *ad, int pos);
static int sendToIOP2area(int pd0, int d0, int pd1, int d1, u_char *ps0, int s0, u_char *ps1, int s1);
static int sendToIOP(int dst, u_char *src, int size);

//////////////////////////////////////////////////////////////////
// Create audio decoder
//////////////////////////////////////////////////////////////////
int audioDecCreate(
AudioDec *ad,
u_char *buff,
int buffSize,
int iopBuff,
int iopBuffSize,
u_char *zeroBuff,
int iopZeroBuff,
int zeroBuffSize,
float volume)
{
 	ad->state = AU_STATE_INIT;
 	ad->hdrCount = 0;
 	ad->data = buff;
 	ad->put = 0;
 	ad->count = 0;
 	ad->size = buffSize;
 	ad->totalBytes = 0;
 	ad->totalBytesSent = 0;

 	ad->iopBuffSize = iopBuffSize;
 	ad->iopLastPos = 0;
 	ad->iopPausePos = 0;

 	// Audio data buffer on IOP
 	ad->iopBuff = iopBuff;

 	// Zero data buffer on IOP
 	ad->iopZero     = iopZeroBuff;
 	ad->iopZeroSize = zeroBuffSize;

	ad->volume = volume;

 	// send zero data to IOP
 	memset(zeroBuff, 0, zeroBuffSize);
 	sendToIOP(ad->iopZero, zeroBuff, zeroBuffSize);

 	return 1;
}

//////////////////////////////////////////////////////////////////
// Delete audio decoder
//////////////////////////////////////////////////////////////////
int audioDecDelete(AudioDec *ad)
{
	// Zero the volumevolume
	MultiStream::AutoDMAVolume(AUTODMA_CH, 0, 0, false);

	// Stop the autodma
	MultiStream::AutoDMAStop(AUTODMA_CH);

	return 1;
}

//////////////////////////////////////////////////////////////////
// Start to play audio data
//////////////////////////////////////////////////////////////////
void audioDecStart(AudioDec *ad)
{
	int volume = (s16)(0x7fff * ad->volume);

	MultiStream::AutoDMAVolume(AUTODMA_CH, volume, volume, false);
	MultiStream::AutoDMAStartWrite(AUTODMA_CH, true,
	 											ad->iopBuff,
												(ad->iopBuffSize / UNIT_SIZE) * UNIT_SIZE,
	 											ad->iopBuff + ad->iopPausePos);

	ad->state = AU_STATE_PLAY;
}

/////////////////////////////////////////////////////////////////
// Re-initialize audio decoder
/////////////////////////////////////////////////////////////////
void audioDecReset(AudioDec *ad)
{
	// Clear the pcm volume
	MultiStream::AutoDMAVolume(AUTODMA_CH, 0, 0, true);

	ad->state = AU_STATE_INIT;
	ad->hdrCount = 0;
	ad->put = 0;
	ad->count = 0;
	ad->totalBytes = 0;
	ad->totalBytesSent = 0;
	ad->iopLastPos = 0;
	ad->iopPausePos = 0;
}

//////////////////////////////////////////////////////////////////
// Get empty areas
// ////////////////////////////////////////////////////////////////
void audioDecBeginPut(AudioDec *ad,
	u_char **ptr0, int *len0, u_char **ptr1, int *len1)
{
 	// return ADS header area when (state == AU_STATE_INIT)
 	if (ad->state == AU_STATE_INIT)
	{
		*ptr0 = (u_char*)&ad->sshd + ad->hdrCount;
		*len0 = AU_HDR_SIZE - ad->hdrCount;
		*ptr1 = (u_char*)ad->data;
		*len1 = ad->size;
		return;
 	}

 	// Return the empty areas
 	int len = ad->size - ad->count;

 	// area0
	if (ad->size - ad->put >= len)
	{
		*ptr0 = ad->data + ad->put;
		*len0 = len;
		*ptr1 = NULL;
		*len1 = 0;
 	}
	else
	{
		// area0 + area1
		*ptr0 = ad->data + ad->put;
		*len0 = ad->size - ad->put;
		*ptr1 = ad->data;
		*len1 = len - (ad->size - ad->put);
 	}
}

//////////////////////////////////////////////////////////////////
// Update pointer
//////////////////////////////////////////////////////////////////
void audioDecEndPut(AudioDec *ad, int size)
{
 	if (ad->state == AU_STATE_INIT)
	{
		int hdr_add = min(size, AU_HDR_SIZE - ad->hdrCount);
		ad->hdrCount += hdr_add;

		if (ad->hdrCount >= AU_HDR_SIZE)
		{
 			ad->state = AU_STATE_PRESET;

#ifdef _DEBUG
 			printf("-------- audio information --------------------\n");
 			printf("[%c%c%c%c]\n"
 						"header size:                            %d\n"
 						"type(0:PCM big, 1:PCM little, 2:ADPCM): %d\n"
 						"sampling rate:                          %dHz\n"
 						"channels:                               %d\n"
 						"interleave size:                        %d\n"
 						"interleave start block address:         %d\n"
 						"interleave end block address:           %d\n",
 						ad->sshd.id[0],
 						ad->sshd.id[1],
 						ad->sshd.id[2],
 						ad->sshd.id[3],
 						ad->sshd.size,
 						ad->sshd.type,
 						ad->sshd.rate,
 						ad->sshd.ch,
 						ad->sshd.interSize,
 						ad->sshd.loopStart,
 						ad->sshd.loopEnd);
	 		printf("[%c%c%c%c]\n"
 						"data size:                              %d\n",
 						ad->ssbd.id[0],
 						ad->ssbd.id[1],
 						ad->ssbd.id[2],
 						ad->ssbd.id[3],
 						ad->ssbd.size);
#endif
		}

		size -= hdr_add;
 	}

 	ad->put = (ad->put + size) % ad->size;
 	ad->count += size;
 	ad->totalBytes += size;
}

//////////////////////////////////////////////////////////////////
// Check to see if enough data is already sent to IOP or not
//////////////////////////////////////////////////////////////////
int audioDecIsPreset(AudioDec *ad)
{
	return ad->totalBytesSent >= PRESET_VALUE(ad->iopBuffSize);
}

//////////////////////////////////////////////////////////////////
// Send data to IOP
//////////////////////////////////////////////////////////////////
int audioDecSendToIOP(AudioDec *ad)
{
 	int pd0, pd1, d0, d1;
 	u_char *ps0, *ps1;
 	int s0, s1;
 	int count_sent = 0;
 	int countAdj;
 	int pos = 0;

 	switch (ad->state)
	{
		case AU_STATE_INIT:
 			return 0;
 			break;

		case AU_STATE_PRESET:
 			pd0 = ad->iopBuff + (ad->totalBytesSent) % ad->iopBuffSize;
 			d0 = ad->iopBuffSize - ad->totalBytesSent;
 			pd1 = 0;
 			d1 = 0;
 			break;

		case AU_STATE_PLAY:
			pos = MultiStream::GetPCMPlaybackPosition() - ad->iopBuff;
 			iopGetArea(&pd0, &d0, &pd1, &d1, ad, pos);
 			break;
	}

	ps0 = ad->data + (ad->put - ad->count + ad->size) % ad->size;
 	ps1 = ad->data;

 	// adjust to UNIT_SIZE boundary
 	countAdj = (ad->count / UNIT_SIZE) * UNIT_SIZE;

 	s0 = min(ad->data + ad->size - ps0, countAdj);
 	s1 = countAdj - s0;

 	if (d0 + d1 >= UNIT_SIZE && s0 + s1 >= UNIT_SIZE)
		count_sent = sendToIOP2area(pd0, d0, pd1, d1, ps0, s0, ps1, s1);

 	ad->count -= count_sent;

 	ad->totalBytesSent += count_sent;
 	ad->iopLastPos = (ad->iopLastPos + count_sent) % ad->iopBuffSize;

 	return count_sent;
}

//////////////////////////////////////////////////////////////////
// Get empty area of IOP audio buffer
//////////////////////////////////////////////////////////////////
static void iopGetArea(
int *pd0,
int *d0,
int *pd1,
int *d1,
AudioDec *ad,
int pos)
{
	int len = (pos + ad->iopBuffSize - ad->iopLastPos - UNIT_SIZE) % ad->iopBuffSize;
	int diff = pos - ad->iopLastPos;

 	// corrects overrun bug in previous version
 	if (diff >= 0 && diff < UNIT_SIZE)
	{
		*pd0 = ad->iopBuff;
		*d0  = 0;
		*pd1 = ad->iopBuff;
		*d1  = 0;
		return;
 	}

 	// adjust to UNIT_SIZE boundary
 	len = (len / UNIT_SIZE) * UNIT_SIZE;

 	if (ad->iopBuffSize -  ad->iopLastPos >= len)
	{
		// area0
		*pd0 = ad->iopBuff + ad->iopLastPos;
		*d0 = len;
		*pd1 = 0;
		*d1 = 0;
 	}
	else
	{
		// area0 + area1
		*pd0 = ad->iopBuff + ad->iopLastPos;
		*d0 = ad->iopBuffSize - ad->iopLastPos;
		*pd1 = ad->iopBuff;
		*d1 = len - (ad->iopBuffSize - ad->iopLastPos);
 	}
}

//////////////////////////////////////////////////////////////////
// Send data to IOP
//////////////////////////////////////////////////////////////////
static int sendToIOP2area(
int pd0,
int d0,
int pd1,
int d1,
u_char *ps0,
int s0,
u_char *ps1,
int s1)
{
 	if (d0 + d1 < s0 + s1)
	{
		int diff = (s0 + s1) - (d0 + d1);
		if (diff >= s1)
		{
 			s0 -= (diff - s1);
 			s1 = 0;
		}
		else
		{
 			s1 -= diff;
		}
 	}

 	//
 	// (d0 + d1 >= s0 + s1)
 	//
 	if (s0 >= d0)
	{
		sendToIOP(pd0,			ps0,		d0);
		sendToIOP(pd1,			ps0 + d0,	s0 - d0);
		sendToIOP(pd1 + s0 - d0,	ps1,		s1);
 	}
	else
	{
		// s0 < d0
		if (s1 >= d0 - s0)
		{
 			sendToIOP(pd0,		ps0,		s0);
 			sendToIOP(pd0 + s0,		ps1,		d0 - s0);
 			sendToIOP(pd1,		ps1 + d0 - s0,	s1 - (d0 - s0));
		}
		else
		{ // s1 < d0 - s0
 			sendToIOP(pd0,		ps0,		s0);
 			sendToIOP(pd0 + s0,		ps1,		s1);
		}
 	}

 	return s0 + s1;
}

//////////////////////////////////////////////////////////////////
// Send data to IOP
//////////////////////////////////////////////////////////////////
static int sendToIOP(
int dst,
u_char *src,
int size)
{
	if (size <= 0)
		return 0;

	sceSifDmaData transData;
	transData.data = (u_int)src;
	transData.addr = (u_int)dst;
	transData.size = size;
	transData.mode = 0; // caution
	FlushCache(0);

	int did = sceSifSetDma(&transData, 1);
	while (sceSifDmaStat(did) >= 0);

	return size;
}

