/* --------------------------------------------------
 * File    : MOTION.H
 * Created : Mon Feb 15 15:29:17 1999
 * Descript:
 * --------------------------------------------------*/
#ifndef __MOTION_H
#define __MOTION_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <m_struct.h>

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
typedef struct K2MFHdr
{
	char 					ID[4];
   unsigned short 	usVer;
   unsigned short		usFrames;
   unsigned short 	usBones;
} ts_K2MFHdr;

typedef struct	K2AngAxis
{
	float					fRot;
   l_Vct3f				Axis;
   l_Vct3f				Trans;
} ts_K2AngAxis;

typedef struct K2AngAxisHdr
{
	char 					*pBoneName;
   ts_K2AngAxis		*pMotion;
} ts_K2AngAxisHdr;

typedef struct K2AngAxisSet
{
	unsigned short		usFrames;
   unsigned short		usBones;
 	ts_K2AngAxisHdr	*pMotionSet;
} ts_K2AngAxisSet;

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

#endif __MOTION_H
