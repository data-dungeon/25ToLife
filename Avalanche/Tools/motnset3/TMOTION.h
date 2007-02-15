/* --------------------------------------------------
 * File    : MOTION.H
 * Created : Mon Feb 15 15:29:17 1999
 * Descript:
 * --------------------------------------------------*/
#ifndef __TMOTION_H
#define __TMOTION_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

#include <Math/Vector.h>

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
   Vector3				Axis;
   Vector3				Trans;
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

#endif __TMOTION_H
