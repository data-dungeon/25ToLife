#ifndef __motnset_h
#define __motnset_h


#include "Game/Animation/Motion.h"

#define MAX_MOTION_SET_NAME_LEN		128
#define MAX_MOTION_NAME_LEN			40
#define MAX_MOTION_FILE_NAME_PATH   128
#define MAX_BONE_FILE_NAME_PATH		MAX_MOTION_FILE_NAME_PATH

#define FLAG_WORK_MOTN_ID			0x00000001
#define FLAG_WORK_MOTN_FILE			0x00000002
#define FLAG_WORK_MOTN_CTRL			0x00000004
#define FLAG_WORK_MOTN_FLAGS		0x00000008

#define FLAG_WORK_BONE_FILE			0x00000010
#define FLAG_WORK_MOTN_BONEGROUPFILTER	0x00000020
#define FLAG_WORK_MOTN_ERRORTOLERANCE	0x00000040
#define FLAG_WORK_MOTN_TRANSLATION_SCALE_FACTOR	0x00000080


#define FLAG_WORK_MOTN_ALL	 		(	FLAG_WORK_MOTN_ID									\
												|FLAG_WORK_MOTN_FILE								\
												|FLAG_WORK_MOTN_CTRL								\
												|FLAG_WORK_MOTN_FLAGS							\
												|FLAG_WORK_BONE_FILE								\
												|FLAG_WORK_MOTN_BONEGROUPFILTER				\
												|FLAG_WORK_MOTN_ERRORTOLERANCE				\
												|FLAG_WORK_MOTN_TRANSLATION_SCALE_FACTOR	\
											)

#define MAX_DEFINE_NAME_LEN		40
#define MAX_DEFINES					200
#define MAX_BONE_NAME_LEN			32

typedef struct boneNameStr
{
	char	boneName[MAX_BONE_NAME_LEN];
}ts_boneName;

typedef struct motnInfStr
{
	struct motnInfStr	*next;
	char 					motnName[MAX_MOTION_NAME_LEN];
	char					motnFile[MAX_MOTION_FILE_NAME_PATH];
	char					boneFile[MAX_BONE_FILE_NAME_PATH];
	unsigned long		motnFlags;
	unsigned long		motnCtrl;
	BoneGroupBitMask	boneGroupBitMask;
	float					motnErrorTolerance;
	float					motnTranslationScaleFactor;
}ts_motnInf;

typedef struct motnSetStr
{
	char				setName[MAX_MOTION_SET_NAME_LEN];
	char				setHeaderName[MAX_MOTION_SET_NAME_LEN];
	char				setDefineName[MAX_MOTION_NAME_LEN];
	char				setGBTFileName[_MAX_PATH];
	ts_motnInf		*pMotnList;
	ts_motnInf		*workMotn;
	unsigned short workGroupId;
	unsigned long  workMotnFlags;
	unsigned long	buttonIndex;
}ts_motnSet;

typedef struct boneCtrlInfStr
{
	struct boneCtrlInfStr	*next;
	unsigned short				numCtrl;
	char							*ctrlBuf;
}ts_boneCtrlInf;

extern void initMotn(void);
extern void deInitMotn(void);
extern void writeOutputData(ts_motnInf *pMotnInf);
extern void printError( const char *pcFmt, ...);
extern void printWarning( const char *pcFmt, ...);
extern void printInfo( const char *pcFmt, ...);
extern float GetErrorTolerance();
extern float GetTranslationScaleFactor();
void SetMotionSetName( char *pName);
void SetGBTName( char *pName);

#endif __motnset_h
