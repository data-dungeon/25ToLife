/****************************************************************************
 *
 * MassiveAdObject.h
 *
 * Copyright (C) 2005 Massive Incorporated
 *
 * Author: Massive Incorporated
 *                                                                         
 * Purpose: 
 *			      
 *
 ****************************************************************************/


#ifndef MASSIVEADOBJECT_H
#define MASSIVEADOBJECT_H

#include "Display/DisplayPCH.h"
#include "MassiveMediaTracking.h"

typedef enum {
	ESTAT_NO_IE = 0,
	ESTAT_RETRIEVE_IE,
	ESTAT_IE_FAILED,
	ESTAT_IE_RETRIEVED,
	ESTAT_RETRIEVE_CREX,
	ESTAT_CREX_FAILED,
	ESTAT_CREX_RETREIVED,
	ESTAT_DOWNLOAD_MEDIA,
	ESTAT_MEDIA_FAILED,
	ESTAT_MEDIA_DOWNLOADED,
	ESTAT_REPORT_IMPRESSION,
	ESTAT_MAX
} tIEStatus;

typedef enum __MAO_IETYPE{
	eTEXTURE_IE = 0,
	eAUDIO_IE,
	eVIDEO_IE,
	eMODEL_IE,
	eMAX_IE
}MAO_IETYPE;

typedef struct MASSIVEIE
{
	char *m_sIEName;							// The Inventory Elements name within the Massive AdServer
    tIEStatus m_eStatus;						// The status of the current request for the IE
    MassiveMediaHandle m_hInvElem;				// A Inventory Element handle 
    MassiveMediaHandle m_hCrEx;					// The current Creative Execution handle
	MMT_ERROR m_eLastError;						// The last error in returned by MMT_UpdateTarget.
	unsigned int textureNum;					// tracks which material we are replacing 	
	MAO_IETYPE ieType;							// Media type
	ts_TextureContext *p_TextureContext;		// 25 to life's Mesh's Texture struct
	MASSIVEIE	*prev, *next;
}MassiveIE;



#ifdef    __cplusplus
extern "C"
{
#endif 

extern DoublyLinkedList<MassiveIE>	m_MassiveIE;

extern MassiveIE *MassiveObjectInit(const char *IEName,unsigned int textureNum,MAO_IETYPE ieType,ts_TextureContext *p_TextureContext);

extern bool MassiveObjectSetMassiveTexture(MassiveIE *mao);

extern bool MassiveObjectFrameUpdate(MassiveIE *mao);

extern bool MassiveObjectRequestIEInfo(MassiveIE *mao);

extern bool MassiveObjectProcessIEInfoRequest(MassiveIE *mao);

extern bool MassiveObjectRequestCrExInfo(MassiveIE *mao);

extern bool MassiveObjectProcessCrExInfoRequest(MassiveIE *mao);

extern bool MassiveObjectRequestMediaData(MassiveIE *mao);

extern bool MassiveObjectProcessAdMediaDataRequest(MassiveIE *mao);

extern bool MassiveObjectCommitAdMediaToGame(MassiveIE *mao);

extern bool MassiveObjectReportImpressions(MassiveIE *mao, unsigned int size, float angle, unsigned int ScreenHeight, unsigned int ScreenWidth);

extern bool MassiveObjectFrameTick();

#ifdef    __cplusplus
}
#endif  

#endif