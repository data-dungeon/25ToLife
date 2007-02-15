#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <GroupBoneTable.h>
#include "l_SLLList.h"
#include <mpostr.h>
#include <dbwriter.h>
#include <Math/Vector.h>
#include <Math/Quatern.h>
#include <Game/Animation/Motion.h>
#include <Game/Animation/MotionDB.h>
#include "parse.h"
#include "motnset.h"
#include "LoadKFM.h"
#include "parsefcn.h"
#include "Game/Database/DBFile.h"


ts_motnSet	glbMotnSet;

DWORD 			getComment(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getMotionSetName(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getMotion(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getMotionFile(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getMotionCtrl(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getMotionFlags(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getBoneGroupFilter(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getBoneGroupExclude(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getMotionErrorTolerance(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getMotionTranslationScaleFactor(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getDefines(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 			getControlInc(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 	      getMotionGroupId(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 	      getGroupBoneTableFile(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
DWORD 	      finishMotionSet(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags);
char 				*getParameter(char *pString);
void 				WritePSXMotion( mpostream &str, char *fileName);
void 				writeMotnTable(mpostream &str, ts_motnInf *pMotnList);
void 				writeMotnHeader(ts_motnInf *pMotnInf);
unsigned long 	countMotnFiles(ts_motnInf *pMotnList);


typedef struct defineTableStr
{
	char 				defineName[MAX_DEFINE_NAME_LEN];
   long				defineValue;

}ts_defineTable;

ts_defineTable	*defineTable;

ts_parse	parseCommands[] = {
									{"MOTION_SET_END:",		finishMotionSet},	// EOF Cleanup routine
									{"MOTION_FILE:",			getMotionFile},	// if unrecognized command, try processing it as a file name

									//the real parse table starts here
									{"#",						getComment},	// comment line - this is first so that comment lines are rejected immediately
									{"MOTION_FILE:",			getMotionFile},	// if unrecognized command, try processing it as a file name
									{"MOTION_CONTROL_INC:",		getControlInc},
									{"MOTION_FLAGS_INC:",		getControlInc},
									{"MOTION_SET_NAME:",		getMotionSetName},
									{"MOTION_GROUP_ID:",		getMotionGroupId},
									{"MOTION_GROUPBONETABLE:",	getGroupBoneTableFile},
									{"MOTION_ID:",				getMotion},
									{"MOTION_CONTROL:",			getMotionCtrl},
									{"MOTION_FLAGS:",			getMotionFlags},
									{"MOTION_BONEGROUPFILTER:",	getBoneGroupFilter},
									{"MOTION_BONEGROUPINCLUDE:",	getBoneGroupFilter},
									{"MOTION_BONEGROUPEXCLUDE:",	getBoneGroupExclude},
									{"MOTION_ERRORTOLERANCE:",	getMotionErrorTolerance},
									{"MOTION_TRANSLATION_SCALE_FACTOR:",	getMotionTranslationScaleFactor},
									{"MOTION_BONE_FILE:",		NULL},	// Bone file not needed reading Andrew Mare's motion files.
									};

ts_parse parseDefines [] = {
											{"#DEFINE", getDefines},
											};


ts_parseListInf	parseInfArray[]	=	{
														{parseCommands, sizeof(parseCommands)/sizeof(parseCommands[0])},
														{parseDefines, sizeof(parseDefines)/sizeof(parseDefines[0])},
													};


DWORD 	getMotionSetName(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	char *pSetName;
   char *pStrEndPtr;

	pSetName = getParameter(curLine);

   // Remove any aditional spaces after file name
   if((pStrEndPtr = strpbrk(pSetName, spaceString)) != NULL)
		*pStrEndPtr = 0x00;

	if(pSetName == NULL)
   {
		printParseError("MOTION_SET_NAME: Not Found");
		return(FALSE);
   }

   if((strlen(pSetName)+ 1) > MAX_MOTION_SET_NAME_LEN)
	{
		char c[128];
		sprintf(c, "MOTION_SET_NAME: Longer than %d characters", MAX_MOTION_SET_NAME_LEN);
		printParseError(c);
		return(FALSE);
   }

	if(strlen(glbMotnSet.setName) != 0)
	{
		printWarning("MOTION_SET_NAME (%s) command processed, but command-switch overridden (%s) - using (%s)\n\r", pSetName, glbMotnSet.setName, glbMotnSet.setName);
	}
	else
	{
		printf("MOTION_SET_NAME:        %s\n\r", pSetName);
		SetMotionSetName( pSetName);
	}

	return(TRUE);
}

DWORD 	finishMotionSet(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	unsigned long  rCode;

	if(glbMotnSet.workMotnFlags != 0)
   {
		if(glbMotnSet.workMotnFlags & (FLAG_WORK_MOTN_FILE))
		{
			printParseError("MOTN_ID: Previous didn't include at least a MOTION_FILE");
			return(FALSE);
		}
		else
		{
			//add previous motion to list
	 		sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
	 			  	 SLL_ADD_TO_TAIL, &rCode);
			glbMotnSet.workMotn = NULL;
		}
   }
	return(TRUE);
}

void startNewMotion(char *pInf)
{
	ts_motnInf  *pMotnInf;

	//start a new motion entry

  	glbMotnSet.workMotnFlags = FLAG_WORK_MOTN_ALL;

	pMotnInf = (ts_motnInf *)malloc(sizeof(ts_motnInf));
   memset(pMotnInf, 0x00, sizeof(ts_motnInf));

   // Save motion name in motion inf link structure
   strcpy(pMotnInf->motnName, pInf);

   glbMotnSet.workMotn = pMotnInf;

	glbMotnSet.workMotn->boneGroupBitMask = ALL_BONE_GROUPS;//assume we're to load all bone groups contained in this motion
	glbMotnSet.workMotn->motnErrorTolerance = GetErrorTolerance();//assume we're to use the (possibly command-line input) default compression
	glbMotnSet.workMotn->motnTranslationScaleFactor = GetTranslationScaleFactor();//assume we're to use the (possibly command-line input) default compression
}

DWORD 	getMotion(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	unsigned long  rCode;
	char 			*pInf;
   char 			*pStrEndPtr;

	if(glbMotnSet.workMotnFlags != 0)
   {
		if(glbMotnSet.workMotnFlags & FLAG_WORK_MOTN_FILE)
		{
			printParseError("MOTN_ID: Previous motion didn't include at least a MOTION_FILE");
			return(FALSE);
		}
		else
		{
			//add previous motion to list
	 		sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
	 			  	 SLL_ADD_TO_TAIL, &rCode);
			glbMotnSet.workMotn = NULL;
		}

   }

	pInf = getParameter(curLine);
	if(pInf == NULL)
   {
		printParseError("MOTION_ID: Not Found");
		return(FALSE);
   }

   // Remove any aditional spaces after file name
   if((pStrEndPtr = strpbrk(pInf, spaceString)) != NULL)
		*pStrEndPtr = 0x00;

	//start a new motion entry

	startNewMotion(pInf);

	//note that we got an ID
	glbMotnSet.workMotnFlags &= ~FLAG_WORK_MOTN_ID;

   return(TRUE);
}

DWORD 	getComment(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	return(TRUE);
}


DWORD 	getMotionFile(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	unsigned long  rCode;
	char 				*pInf;
   char 				*pStrEndPtr;

	pInf = getParameter(curLine);
	if(pInf == NULL)
   {
#if 0 //we used to require the "MOTION_FILE: filename" command - now assume it's just the "filename"
//		printParseError("MOTION_FILE: Not Found");
//		return(FALSE);
#else
		//for ease of use, assume no command is simply the file name
		pInf = curLine;
#endif
   }

   // Remove any aditional spaces after file name
   if((pStrEndPtr = strpbrk(pInf+strlen(pInf)-1, spaceString)) != NULL)
		*pStrEndPtr = 0x00;

	char fName[_MAX_FNAME];
	_splitpath( pInf, NULL, NULL, fName, NULL );

	//if this is the very first command then start a fresh motion
	if(glbMotnSet.workMotnFlags == 0)
		startNewMotion(fName);
	else
	{
		
		//is this the start of a new motion?
		if( (glbMotnSet.workMotnFlags & FLAG_WORK_MOTN_ID)//new motion  if no ID was specified
			|| ((glbMotnSet.workMotnFlags & FLAG_WORK_MOTN_FILE) == 0)//or  if a motionfile has already been specified
			)
		{
			//add previous motion to list
			sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
				SLL_ADD_TO_TAIL, &rCode);
			glbMotnSet.workMotn = NULL;
			
			startNewMotion(fName);
			
		}
	}

	if(!(glbMotnSet.workMotnFlags & FLAG_WORK_MOTN_FILE))
   {
		printParseError("MOTN_FILE: File Name Already Set");
		return(FALSE);
   }

	printf("MOTION_FILE:            %s\n\r", pInf);
	strcpy(glbMotnSet.workMotn->motnFile, pInf);
	glbMotnSet.workMotnFlags &= ~FLAG_WORK_MOTN_FILE;

	if(!glbMotnSet.workMotnFlags)
   {
 		sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
	 			  	 SLL_ADD_TO_TAIL, &rCode);
		glbMotnSet.workMotn = NULL;
   }

	return(TRUE);
}


DWORD 	getMotionCtrl(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	unsigned long  rCode;
   unsigned long  ctrlFlags;
   unsigned long	index;
	char 				*pInf;
   char 				*pStrEndPtr;
   char				 workBuf[80];

   ctrlFlags = 0;
	if(!(glbMotnSet.workMotnFlags & FLAG_WORK_MOTN_CTRL))
   {
		printParseError("MOTN_CTRL: CTRL Already Set");
		return(FALSE);
   }

	pInf = getParameter(curLine);

	/*
    * Need to parse out control flags here from joystick.h bit definitions.
    */

	if(pInf != NULL)
	{
		while(pInf != NULL)
   	{
			strcpy(workBuf, pInf);
   		if((pStrEndPtr = strpbrk(workBuf, spaceString)) != NULL)
				*pStrEndPtr = 0x00;

	   	if((pStrEndPtr = strpbrk(workBuf, "\n\r")) != NULL)
   		{
				*pStrEndPtr = 0x00;
      	}

			for(index = 0; index < glbMotnSet.buttonIndex; index++)
      	{
				if(strcmp(workBuf, defineTable[index].defineName) == 0)
         	{
 					ctrlFlags |= defineTable[index].defineValue;
            	break;
         	}
      	}
			pInf = getParameter(pInf);
		}
	}

   glbMotnSet.workMotn->motnCtrl = ctrlFlags;
	glbMotnSet.workMotnFlags &= ~FLAG_WORK_MOTN_CTRL;

	if(!glbMotnSet.workMotnFlags)
   {
 		sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
	 			  	 SLL_ADD_TO_TAIL, &rCode);
		glbMotnSet.workMotn = NULL;
   }

	return(TRUE);
}

DWORD 	getMotionFlags(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	unsigned long  rCode;
   unsigned long  motnFlags;
   unsigned long	index;
	char 				*pInf;
   char 				*pStrEndPtr;
   char				 workBuf[80];

   motnFlags = 0;
	if(!(glbMotnSet.workMotnFlags & FLAG_WORK_MOTN_FLAGS))
   {
		printParseError("MOTN_FLAGS: FLAGS Already Set");
		return(FALSE);
   }

	pInf = getParameter(curLine);

	/*
    * Need to parse out control flags here from joystick.h bit definitions.
    */

	if(pInf != NULL)
	{
		while(pInf != NULL)
   	{
			strcpy(workBuf, pInf);
   		if((pStrEndPtr = strpbrk(workBuf, spaceString)) != NULL)
				*pStrEndPtr = 0x00;

	   	if((pStrEndPtr = strpbrk(workBuf, "\n\r")) != NULL)
   		{
				*pStrEndPtr = 0x00;
      	}

			for(index = 0; index < glbMotnSet.buttonIndex; index++)
      	{
				if(strcmp(workBuf, defineTable[index].defineName) == 0)
         	{
 					motnFlags |= defineTable[index].defineValue;
            	break;
         	}
      	}
			pInf = getParameter(pInf);
		}
	}

   glbMotnSet.workMotn->motnFlags = motnFlags;
	glbMotnSet.workMotnFlags &= ~FLAG_WORK_MOTN_FLAGS;

	if(!glbMotnSet.workMotnFlags)
   {
 		sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
	 			  	 SLL_ADD_TO_TAIL, &rCode);
		glbMotnSet.workMotn = NULL;
   }

	return(TRUE);
}

DWORD 	getBoneGroupFilter(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	BoneGroupBitMask	boneGroupBitMask = 0;
	unsigned long		rCode;
	char	 				*pInf;
   char	 				*pToken;

	pInf = getParameter(curLine);

	/*
    * Need to parse group numbers into BoneGroupBitMask
    */

	if(pInf != NULL)
	{
		BoneGroupID boneGroupID;

		pToken = strtok(pInf, "|\n");
		while (pToken != NULL)
		{
			sscanf(pToken, "%d", &boneGroupID);
			if(boneGroupID >= sizeof(BoneGroupBitMask)*8)
			{
				printParseError("MOTN_BONEGROUPFILTER: bone group number too large");
				return(FALSE);
			}
			boneGroupBitMask |= (1<<boneGroupID);
			
	      /* Get next token: */
	      pToken = strtok( NULL, "|\n" );
		}
	}

   glbMotnSet.workMotn->boneGroupBitMask &= boneGroupBitMask;
	glbMotnSet.workMotnFlags &= ~FLAG_WORK_MOTN_BONEGROUPFILTER;

	if(!glbMotnSet.workMotnFlags)
   {
 		sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
	 			  	 SLL_ADD_TO_TAIL, &rCode);
		glbMotnSet.workMotn = NULL;
   }

	return(TRUE);
}

DWORD 	getBoneGroupExclude(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	BoneGroupBitMask	boneGroupBitMask = 0;
	unsigned long		rCode;
	char	 				*pInf;
   char	 				*pToken;

	pInf = getParameter(curLine);

	/*
    * Need to parse group numbers into BoneGroupBitMask
    */

	if(pInf != NULL)
	{
		BoneGroupID boneGroupID;

		pToken = strtok(pInf, "|\n");
		while (pToken != NULL)
		{
			sscanf(pToken, "%d", &boneGroupID);
			if(boneGroupID >= sizeof(BoneGroupBitMask)*8)
			{
				printParseError("MOTN_BONEGROUPFILTER: bone group number too large");
				return(FALSE);
			}
			boneGroupBitMask |= (1<<boneGroupID);
			
	      /* Get next token: */
	      pToken = strtok( NULL, "|\n" );
		}
	}

   glbMotnSet.workMotn->boneGroupBitMask &= ~boneGroupBitMask;
	glbMotnSet.workMotnFlags &= ~FLAG_WORK_MOTN_BONEGROUPFILTER;

	if(!glbMotnSet.workMotnFlags)
   {
 		sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
	 			  	 SLL_ADD_TO_TAIL, &rCode);
		glbMotnSet.workMotn = NULL;
   }

	return(TRUE);
}

DWORD getMotionErrorTolerance(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	unsigned long		rCode;
	char	 				*pInf;
   float errorTolerance;

	pInf = getParameter(curLine);

	/*
    * Need to convert compression value into float
    */

	if(pInf != NULL)
	{
		int numScan = sscanf(pInf, "%f", &errorTolerance);
		if( (numScan != 1) || (errorTolerance < 0.0) || (errorTolerance > 1.0) )
		{
			printParseError("MOTN_ERRORTOLERANCE: ErrorTolerance invalid or out of range (0.0 .. 1.0)");
			return(FALSE);
		}
	}

   glbMotnSet.workMotn->motnErrorTolerance = errorTolerance;
	glbMotnSet.workMotnFlags &= ~FLAG_WORK_MOTN_ERRORTOLERANCE;

	if(!glbMotnSet.workMotnFlags)
   {
 		sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
	 			  	 SLL_ADD_TO_TAIL, &rCode);
		glbMotnSet.workMotn = NULL;
   }

	return(TRUE);
}

DWORD getMotionTranslationScaleFactor(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	unsigned long		rCode;
	char	 				*pInf;
   float translationScaleFactor;

	pInf = getParameter(curLine);

	/*
    * Need to convert compression value into float
    */

	if(pInf != NULL)
	{
		int numScan = sscanf(pInf, "%f", &translationScaleFactor);
		if( (numScan != 1) || (translationScaleFactor <= 0.0) )
		{
			printParseError("MOTN_TRANSLATION_SCALE_FACTOR: TranslationScaleFactor invalid or less than or equal to 0.0");
			return(FALSE);
		}
	}

   glbMotnSet.workMotn->motnTranslationScaleFactor = translationScaleFactor;
	glbMotnSet.workMotnFlags &= ~FLAG_WORK_MOTN_TRANSLATION_SCALE_FACTOR;

	if(!glbMotnSet.workMotnFlags)
   {
 		sllAddToList((ts_voidListSll **)&glbMotnSet.pMotnList, (ts_voidListSll *)glbMotnSet.workMotn,
	 			  	 SLL_ADD_TO_TAIL, &rCode);
		glbMotnSet.workMotn = NULL;
   }

	return(TRUE);
}




DWORD 	getMotionGroupId(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
//	unsigned long  rCode;
   unsigned long	index;
	char 				*pInf;
   char 				*pStrEndPtr;
   char				 workBuf[80];

	pInf = getParameter(curLine);

	/*
    * Need to parse out control flags here from joystick.h bit definitions.
    */

	if(pInf != NULL)
	{
		while(pInf != NULL)
   	{
			strcpy(workBuf, pInf);
   		if((pStrEndPtr = strpbrk(workBuf, spaceString)) != NULL)
				*pStrEndPtr = 0x00;

	   	if((pStrEndPtr = strpbrk(workBuf, "\n\r")) != NULL)
   		{
				*pStrEndPtr = 0x00;
      	}

			for(index = 0; index < glbMotnSet.buttonIndex; index++)
      	{
				if(strcmp(workBuf, defineTable[index].defineName) == 0)
         	{
 					glbMotnSet.workGroupId = (unsigned short)defineTable[index].defineValue;
					strcpy(glbMotnSet.setDefineName, workBuf);
            	return(TRUE);
         	}
      	}

			printError("Can't find MotionGroupID %s\n", pInf);

			pInf = getParameter(pInf);
		}
	}


	return(FALSE);
}

DWORD 	getGroupBoneTableFile(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	char 				*pInf;
   char 				*pStrEndPtr;

	pInf = getParameter(curLine);
	if(pInf == NULL)
   {
		return(FALSE);
   }

   // Remove any aditional spaces after file name
   if((pStrEndPtr = strpbrk(pInf, spaceString)) != NULL)
		*pStrEndPtr = 0x00;

   if((pStrEndPtr = strpbrk(pInf, "\n\r")) != NULL)
		*pStrEndPtr = 0x00;

	if(strlen(glbMotnSet.setGBTFileName) != 0)
	{
		printWarning("MOTION_GROUPBONETABLE (%s) command processed, but command-switch overridden (%s) - using (%s)\n\r", pInf, glbMotnSet.setGBTFileName, glbMotnSet.setGBTFileName);
	}
	else
	{
		SetGBTName(pInf);
	}

	return(TRUE);
}





DWORD 	getControlInc(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	char 				*pInf;
   char 				*pStrEndPtr;


	pInf = getParameter(curLine);
	if(pInf == NULL)
   {
		printParseError("MOTION_CONTROL_INC or MOTION_FLAGS_INC: Not Found");
		return(FALSE);
   }

   // Remove any aditional spaces after file name
   if((pStrEndPtr = strpbrk(pInf, spaceString)) != NULL)
		*pStrEndPtr = 0x00;

   if((pStrEndPtr = strpbrk(pInf, "\n\r")) != NULL)
		*pStrEndPtr = 0x00;

	ParseFileTxt(pInf, parseInfArray[1].listPtr, parseInfArray[1].listSize, 0);

	return(TRUE);
}

DWORD 	getDefines(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen, DWORD typeIndex, char *curLine, DWORD flags)
{
	char 				*pInf;
   char 				*pStrEndPtr;
   char				 workBuf[MAX_MOTION_FILE_NAME_PATH];


	pInf = getParameter(curLine);
	if(pInf == NULL)
   {
		printParseError("Define Type Not Found");
		return(FALSE);
   }

   strcpy(workBuf, pInf);

   // Remove any aditional spaces after file name
   if((pStrEndPtr = strpbrk(workBuf, spaceString)) != NULL)
		*pStrEndPtr = 0x00;

	strcpy(defineTable[glbMotnSet.buttonIndex].defineName, workBuf);

   // Look for define value
   pInf = strpbrk(pInf, spaceString);

   if(pInf == NULL)
   	return(TRUE);

	pInf = eatWhite(pInf);
   if(pInf == NULL)
   	return(TRUE);

   defineTable[glbMotnSet.buttonIndex].defineValue = strtol(pInf, NULL, 0);
	glbMotnSet.buttonIndex++;
   if(glbMotnSet.buttonIndex >= MAX_DEFINES)
   {
		printParseError("Too Many Defines Array Needs to be increased");
		return(FALSE);
   }

	return(TRUE);
}


char *getParameter(char *pString)
{
	// Remove white spaces before command string
	pString 	= eatWhite(pString);

   if(pString == NULL)
   	return(NULL);

   // Find end of command string
   pString  = strpbrk(pString, spaceString);

   if(pString == NULL)
   	return(NULL);
   // Remove spaces before command string setting
	pString = eatWhite(pString);
   return(pString);
}

void initMotn(void)
{
	memset(&glbMotnSet, 0x00, sizeof(glbMotnSet));
   defineTable = (ts_defineTable *)malloc(sizeof(ts_defineTable) * MAX_DEFINES);
   memset(defineTable, 0x00, sizeof(ts_defineTable) * MAX_DEFINES);

	/* initial motion group id is 1000-- DEFAULT_CHUNK_TYPE in engine. we are getting rid of the motion group ids */
	/* in juju, but for now we have to continue to pass a motion group id to motion and script routines for */
	/* backwards compatibility with rugrats. so we will always pass DEFAULT_CHUNK_TYPE */

	glbMotnSet.workGroupId = 1000;
}

void deInitMotn(void)
{
	ts_motnInf *pMotnInf;
	ts_motnInf *pNext;

	free(defineTable);
	for(pMotnInf = glbMotnSet.pMotnList; pMotnInf != NULL; pMotnInf = pNext)
	{
		pNext = pMotnInf->next;
		free(pMotnInf);
	}
}

void printParseError(char *errorString)
{
	MessageBox(NULL, errorString, "PARSER ERROR:", MB_ICONERROR);
}


unsigned long GetFileLen( FILE *pFile)
{
	fseek(pFile, 0, SEEK_END);
	unsigned long ulLen = ftell(pFile);
	rewind(pFile);
	return ulLen;
}

#define 	READ_BUF_LEN	32 * 1024

void WritePSXMotion( mpostream &str, ts_motnInf *pMotnInf)
{
	FILE 			*pFile;
   int			rCode;
   char			*dataBuf;
	char			*fileName = pMotnInf->motnFile;

	if( strstr(fileName, ".KFM") || strstr(fileName, ".kfm") )
		LoadAndWriteKFM(str, fileName, pMotnInf);
	else
	{

		pFile = fopen(fileName, "rb");
		if(pFile == NULL)
		{
			printError( "Can't open %s for read\n", fileName );
			printParseError("WritePSXMotion: fopen() returned NULL");
			return;
		}

		unsigned long ReadBufLen = GetFileLen(pFile);

		dataBuf = (char *)malloc(READ_BUF_LEN);
		if(dataBuf == NULL)
		{
			printError( "Can't allocate buffer to load %s\n", fileName );
			printParseError("WritePSXMotion: Unable to alloc read buf");
			return;
		}
		memset(dataBuf, 0x00, READ_BUF_LEN);

		while(TRUE)
		{
			rCode = fread(dataBuf, 1, READ_BUF_LEN, pFile);
   		if(!rCode)
      		break;

			str.Write((unsigned char *)dataBuf, rCode);
		}
		free(dataBuf);
		fclose(pFile);
	}


	str.Align();
}


mpostream & operator << ( mpostream &str, ts_MFileHdr &Hdr)
{
	str.Write( (unsigned char *) Hdr.ID, 4);
	str << Hdr.usVer;
	str << Hdr.usNumMotn;
	str << Hdr.u32Flags;
	return str;
}


mpostream & operator << ( mpostream &str, ts_Quatvw &Quat)
{
	unsigned short	us;

	us	= (unsigned short)(Quat.V.X() * 4096);
	str << us;
	us	= (unsigned short)(Quat.V.Y() * 4096);
	str << us;
	us	= (unsigned short)(Quat.V.Z() * 4096);
	str << us;
	us	= (unsigned short)(Quat.W * 4096);
	str << us;

	return str;
}

mpostream & operator << (mpostream &str, ts_MHdr &MHdr)
{
	str.Write( (unsigned char *)MHdr.name, sizeof(MHdr.name));
	str << MHdr.type;
	str << MHdr.usFrames;
	str << MHdr.usBones;
	str << (unsigned long)MHdr.pMChannel;   				 // Offset fixed up on load.
   str << MHdr.boneGroupBitMask;

	return(str);
}

mpostream & operator << (mpostream &str, ts_motDbEntry &mDbEntry)
{
	str << mDbEntry.motDbId;
	str << mDbEntry.numMotns;
	str << (unsigned long)mDbEntry.pMotnTable;
	return(str);
}


mpostream & operator << (mpostream &str, ts_MChannel &MChannel)
{
	str.Write( (unsigned char *)MChannel.m_name, sizeof(MChannel.m_name));
	str << MChannel.usMChannelItemBitArray;
	str << MChannel.m_usGroupNo;
	str.Write( (float *)&MChannel.m_bonetoparent, sizeof(MChannel.m_bonetoparent)/sizeof(float));
	str << (unsigned long)MChannel.m_channelitem;   				 // Offset fixed up on load.
	return(str);
}

mpostream & operator << (mpostream &str, ts_MChannelItem &MChannelItem)
{
	str << MChannelItem.m_numkeyframes;
	str << MChannelItem.m_cachedindex;
	str << (unsigned long)MChannelItem.m_keyframes;
	str << MChannelItem.m_cachedvalue;
	str << MChannelItem.m_cachedtime;
	str << MChannelItem.m_pad;
	return(str);
}

mpostream & operator << (mpostream &str, ts_MKeyFrameEngine &MKeyFrame)
{
	str << MKeyFrame.m_time;
	str << MKeyFrame.m_value;
//the engine keyframe structure currently doesn't have spline info
//	str << MKeyFrame.m_it.x;
//	str << MKeyFrame.m_it.y;
//	str << MKeyFrame.m_ot.x;
//	str << MKeyFrame.m_ot.y;
	return(str);
}


void writeOutputData(ts_motnInf *pMotnInf)
{
	unsigned long		listLength;
	unsigned long		numMotnFiles;
//	ts_boneName 		*pBoneNames;
//	ts_K2AngAxisSet	*pMotion;
//	int					numBoneNames;
	DBWriter				*pMotWriter;
	ts_motDbEntry 		motDbEntry;
	ts_MFileHdr			Hdr = { "EQM", 0x0200, 0, 0};


	sllGetListLength((ts_voidListSll *)pMotnInf, &listLength);
	if(listLength == 0)
		return;

	printf("\nWriting Output Data....\n\r");
	printf("Writing:                %s\n\r", glbMotnSet.setName);
	writeMotnHeader(pMotnInf);


	numMotnFiles = countMotnFiles(pMotnInf);

	pMotWriter = new DBWriter( glbMotnSet.setName, DBWriter::PCSt);
	if ( ! pMotWriter->stream_open() )
	{
		printError( "can't open %s for write", glbMotnSet.setName );
		return;
	}

	pMotWriter->DBStart( 0);
	char ChunkName[20];
	itoa(glbMotnSet.workGroupId, ChunkName, 10);
	pMotWriter->ChunkStart( DB_MOTION, DBL_NAMED_CHUNK, DB_MOTION_VERSION, ChunkName);	// use workGroupId as chunk key

	Hdr.usNumMotn = (unsigned short)listLength;
	pMotWriter->stream() << Hdr;

	/* -----------------3/26/99 2:29PM-------------------
	 * Write out space for motDbEntry field to be fixed
	 * up at loadtime.
	 * --------------------------------------------------*/
	memset(&motDbEntry, 0x00, sizeof(ts_motDbEntry));
	motDbEntry.motDbId 	= glbMotnSet.workGroupId;
	motDbEntry.numMotns	= (unsigned short)numMotnFiles;
	pMotWriter->stream() << motDbEntry;

	writeMotnTable(pMotWriter->stream(), pMotnInf);


	for( ; pMotnInf != NULL; pMotnInf = pMotnInf->next)
	{
		WritePSXMotion( pMotWriter->stream(), pMotnInf);
	}

	printf( "\n Grand Totals: ChannelBytes %d, ChannelItemBytes %d, KeyFrameBytes %d, KeyframeBytesRemoved %d, InputKeyframes %d, InputTransFrames %d\n", 
		grandtotalChannelBytes, grandtotalChannelItemBytes, grandtotalKeyframeBytes, grandtotalKeyframeBytesRemoved, grandtotalInputKeyframes, grandtotalInputTransFrames);

	printf("\n Spline bytes = %d, KeyFrameBytes = %d\n", grandtotalChannelItemBytes+grandtotalKeyframeBytes, grandtotalInputKeyframes*4);

	printf("\n Flatline double frames = %d, Flatline double Bytes = %d\n", grandtotal2FrameChannels, grandtotal2FrameChannels*20);
	

	pMotWriter->ChunkEnd( );
	pMotWriter->DBEnd( );
	delete pMotWriter;
	
	CleanUpKFMLoads();

	return;
};

unsigned long countCtrlFlags(ts_motnInf *pMotnList)
{
	unsigned long ctrlFlagCount;


	ctrlFlagCount = 0;
	for(; pMotnList != NULL; pMotnList = pMotnList->next)
	{
		if(pMotnList->motnCtrl)
			ctrlFlagCount++;
	}

	return(ctrlFlagCount);
}

unsigned long countMotnFlags(ts_motnInf *pMotnList)
{
	unsigned long motnFlagCount;


	motnFlagCount = 0;
	for(; pMotnList != NULL; pMotnList = pMotnList->next)
	{
		if(pMotnList->motnFlags)
			motnFlagCount++;
	}

	return(motnFlagCount);
}

unsigned long countMotnFiles(ts_motnInf *pMotnList)
{
	unsigned long motnFileCount;

	motnFileCount = 0;
	for( ; pMotnList != NULL; pMotnList = pMotnList->next)
		motnFileCount++;

	return(motnFileCount);
}


void writeMotnTable(mpostream &str, ts_motnInf *pMotnList)
{
	unsigned long	loadTimeFixup;
	ts_motnInf		*pMotn;

	loadTimeFixup = 0;
	/* -----------------3/26/99 2:10PM-------------------
	 * Write out the storage space for the motion table
	 * array these are addresses to the motions in the
	 * table that will be fixed up at loadtime.
	 * --------------------------------------------------*/
	for(pMotn = pMotnList; pMotn != NULL; pMotn = pMotn->next)
		str << loadTimeFixup;
}


void writeMotnHeader(ts_motnInf *pMotnInf)
{
	unsigned long	motnDefine;
	unsigned long	index;
	FILE 				*fp;
	char 				*pChar;
	char 				tempName[MAX_MOTION_SET_NAME_LEN];
	char 				fileName[MAX_MOTION_SET_NAME_LEN];
	char 				 lineBuf[80];

	if (glbMotnSet.setHeaderName[0] == 0)
	{
		memset(tempName, 0x00, MAX_MOTION_SET_NAME_LEN);
		strcpy(tempName, glbMotnSet.setName);

		if((pChar = strpbrk(tempName, ".")) != NULL)
			*pChar = 0x00;

		if(strlen(tempName) > (MAX_MOTION_SET_NAME_LEN - 4))
		{
			printParseError("MOTION_SET_NAME: Name Too Long (name_m.h)");
			return;
		}

		sprintf(fileName, "%s_m.h", tempName);
	}
	else
	{
		strcpy(fileName, glbMotnSet.setHeaderName);
	}

	printf("Writing:                %s\n\r", fileName);

	fp = fopen(fileName, "wt");

	if(!fp)
	{
		printError( "Could not open file %s for writing", fileName);
		return;
	}

	/* may not have specified a group id name/value */

	if (strlen(glbMotnSet.setDefineName) != 0)
	{
		sprintf(lineBuf, "#define %-30s 0x%04x\n", glbMotnSet.setDefineName, glbMotnSet.workGroupId);
		fwrite(lineBuf, 1, strlen(lineBuf), fp);
	}

	for(index = 0; pMotnInf != NULL; pMotnInf = pMotnInf->next, index++)
	{
		motnDefine = (glbMotnSet.workGroupId << 16) | index;
		sprintf(lineBuf, "#define %-30s 0x%08x\n", pMotnInf->motnName, motnDefine);
		fwrite(lineBuf, 1, strlen(lineBuf), fp);
	}

	fclose(fp);
	return;
}

