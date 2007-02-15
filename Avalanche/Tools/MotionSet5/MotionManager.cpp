#include "stdafx.h"
#include "bezier2.h"
#include "MotionManager.h"
#include "CommandLine.h"
#include "FilePath.h"
#include "BoneGroupTable.h"
#include "DBWriter.h"
#include "MotionStructs.h"
#include "TupAnim.h"

#define MAX_SHORT (32767)
#define MIN_SHORT (-32768)
#define MAX_USHORT (65535)

#define JIFFIES_PER_SECOND (300.0f)	//arbitrary...what's best?   system clock read in sec/1000.  PAL/NTSC common sec/300.  can't animate faster than sec/60 (except on an RGB monitor)
#define SECONDS_PER_JIFFY (1.0f/JIFFIES_PER_SECOND)
#define ANIMFRAMES_PER_SECOND	(30.0f)	
#define SECONDS_PER_ANIMFRAME	(1.0f/30.0f)	
#define JIFFIES_PER_ANIMFRAME	(JIFFIES_PER_SECOND / ANIMFRAMES_PER_SECOND)
#define ANIMFRAMES_PER_JIFFY (ANIMFRAMES_PER_SECOND / JIFFIES_PER_SECOND)
#define JIFFIES_PER_MILLISECOND (JIFFIES_PER_SECOND / 1000.0f)
#define SECONDS_TO_ANIMFRAMES(seconds) ((int)(seconds * ANIMFRAMES_PER_SECOND + 0.5f))	//round to the nearest frame
#define ANIMFRAMES_TO_SECONDS(frames) (((float)frames) * SECONDS_PER_ANIMFRAME)

#define MAX_JIFFIES (MAX_USHORT)	//max value a unsigned short can have 
#define SECONDS_TO_JIFFIES(seconds) ((int)(seconds * JIFFIES_PER_SECOND + 0.5f))	//round to the nearest jiffy
#define JIFFIES_TO_SECONDS(jiffies) (((float)jiffies) * SECONDS_PER_JIFFY)
#define MAX_INTERP_JIFFIES (MAX_SHORT)	//max value a short can have 

//conversion factor to convert from Hash's centimeters to engine's meters
#define KFM_TO_ENGINE_SCALE_FACTOR 0.01

#define MAX_ANIM_VALUE (32767.0f) //max value a signed short can have

#define ANIMVALUE_TO_FLOAT(animvalue, animValToFloat) ((float)animvalue * animValToFloat)	
#define FLOAT_TO_ANIMVALUE(floatvalue, floatToAnimVal) ((short)(floatvalue * floatToAnimVal + 0.5f))

//data reduction: reduce the spline handles from vct2f's to vct2c's. Here are macros to convert back and forth.
#define MAX_ANIMVECT_VALUE (127.0f) //max value a signed char can have
#define MAX_ANIMVECT_FLOAT (8.0f) //max value that anim values can take on
#define ANIMVECT_TO_FLOAT(animvect) ((float)animvect * (MAX_ANIMVECT_FLOAT / MAX_ANIMVECT_VALUE))
#define FLOAT_TO_ANIMVECT(floatvalue) ((short)(floatvalue * (MAX_ANIMVECT_VALUE / MAX_ANIMVECT_FLOAT) + 0.5f))

static inline bool Zero(double x,double epsilon)
{
	return(::fabs(x) <= epsilon);
}

static inline bool Equal(double x,double y,double epsilon)
{
	return(Zero(x - y, epsilon));
}

static char *pChannelItemNames[] = 
{
	"X Translation",
	"Y Translation",
	"Z Translation",
	"X Scale",
	"Y Scale",
	"Z Scale",
	"X Rotation",
	"Y Rotation",
	"Z Rotation",
	"W Rotation"
};

// BoneManager methods
bool BoneManager::Matrix3x3::operator==(const BoneManager::Matrix3x3 &s2) const
{
	return (e00 == s2.e00 &&
			  e01 == s2.e01 &&
			  e02 == s2.e02 &&
			  e10 == s2.e10 &&
			  e11 == s2.e11 &&
			  e12 == s2.e12 &&
			  e20 == s2.e20 &&
			  e21 == s2.e21 &&
			  e22 == s2.e22);
}

unsigned int GetHashValue(const BoneManager::BoneEntry& entry)
{
	unsigned int nameHash = GetHashValue(entry.m_name);
	unsigned int matrixHash = GetHashValue(entry.m_bonetoparent);
	unsigned int groupHash = GetHashValue(entry.m_groupNumber);
	return nameHash+matrixHash+groupHash;
}

inline bool operator==(const BoneManager::BoneEntry &s1,const BoneManager::BoneEntry &s2)
{
	return (s1.m_name == s2.m_name &&
			  s1.m_bonetoparent == s2.m_bonetoparent &&
			  s1.m_groupNumber == s2.m_groupNumber);
}

int BoneManager::AddBone(const BoneManager::BoneEntry &boneEntry)
{
	m_uniqueBoneNames.Add(boneEntry.m_name);
	return m_boneEntries.Add(boneEntry);
}

const BoneManager::BoneEntry &BoneManager::GetBone(int index) const
{
	return m_boneEntries[index];
}

int BoneManager::GetNumBones(void) const
{
	return m_boneEntries.GetSize();
}

int BoneManager::GetNumUniqueBoneNames(void) const
{
	return m_uniqueBoneNames.GetSize();
}


MotionManager::MotionEntry::MotionEntry(void)
{
	Clear();
}

void MotionManager::MotionEntry::Clear(void)
{
	m_KFMData.Clear();
	m_motionIDName.Empty();
	m_additionalMotionIDNames.RemoveAll();
	m_fileName.Empty();
	m_boneGroupBitMask = 0xffffffff;
	m_transErrorTolerance = 0.0f;
	m_transErrorToleranceMultiplier = 1.0f;
	m_rotErrorTolerance = 0.0f;
	m_rotErrorToleranceMultiplier = 1.0f;
	m_scaleErrorTolerance = 0.0f;
	m_scaleErrorToleranceMultiplier = 1.0f;
	m_flags = 0;
	m_userDataEntries.RemoveAll();
}

bool MotionManager::MotionEntry::ReadMotion()
{
#if 1
   char fileName [_MAX_FNAME];
   char path[_MAX_FNAME];
   char ext[_MAX_FNAME]; 
   FilePath::GetExt(m_fileName,ext);
   FilePath::GetFileName(m_fileName,fileName);
   FilePath::GetPath(m_fileName,path);

   TupString filePathName = path;
   filePathName += fileName;
   filePathName += ".anm";
   TupAnim tupAnim;
   if (!tupAnim.LoadAndConvertFile((const char*)filePathName,&m_KFMData))
   {
      filePathName = path;
      filePathName += fileName;
      filePathName += ".kfm";
      if (!m_KFMData.ReadKFMFile((const char *)filePathName))
         return false;
   }
   return true;
#else
   char ext[_MAX_FNAME]; 
   FilePath::GetExt(m_fileName,ext);

   if (stricmp(ext,".kfm")==0)
   {
      if (m_KFMData.ReadKFMFile(m_fileName))
         return true;
   }
   else if (stricmp(ext,".anm")==0)
   {
      TupAnim tupAnim;
      if (tupAnim.LoadAndConvertFile(m_fileName,&m_KFMData))
         return true;
   }
#endif
}

MotionManager::MotionManager(CommandLine &commandLine) :
	m_commandLine(commandLine),
	m_pBoneGroupTable(NULL),
	m_numErrors(0),
	m_totalSourceChannels(0),
	m_totalSourceChannelItems(0),
	m_totalSourceKeys(0),
	m_totalLinearKeys(0),
	m_totalMotionsWritten(0),
	m_totalBoneEntriesWritten(0),
	m_totalChannelsWritten(0),
	m_totalChannelItemsWritten(0),
	m_totalKeyframesWritten(0),
	m_totalKeyframesSize(0),
	m_totalFlatNonDefaultChannelItems(0),
	m_totalDefaultChannelItems(0),
	m_totalIgnoredChannels(0),
	m_totalDefaultChannels(0),
	m_totalChannels(0),
	m_totalChannelItems(0),
	m_bFoundDuplicateMotionIDNames(false)
{
}

void MotionManager::ReadInputFile(void)
{
	char extension[_MAX_EXT];
	FilePath::GetExt(m_commandLine.m_inputFileName,extension);
	if (!stricmp(extension,".txt"))
	{
		printf("Reading Input Text file %s\n",(const char *)m_commandLine.m_inputFileName);
		if (!ParseTextFile())
		{
			throw(Exception(ERROR_FATAL,"ReadInputFile\n   Error Reading file %s\n",(const char *)m_commandLine.m_inputFileName));
		}
	}
   else if (!stricmp(extension,".kfmlst"))
   {
      printf("Reading Input Text file %s\n",(const char *)m_commandLine.m_inputFileName);
      if (!ParseTextFile())
      {
         throw(Exception(ERROR_FATAL,"ReadInputFile\n   Error Reading file %s\n",(const char *)m_commandLine.m_inputFileName));
      }
   }
	else if (!stricmp(extension,".mset"))
	{
		printf("Reading Input MSet file %s\n",(const char *)m_commandLine.m_inputFileName);
		if (!ParseMSetFile())
		{
			throw(Exception(ERROR_FATAL,"ReadInputFile\n   Error Reading file %s\n",(const char *)m_commandLine.m_inputFileName));
		}
	}
	else // other extensions
	{
		printf("Unknown extension for file %s - Reading as text file\n", (const char *)m_commandLine.m_inputFileName);
		if (!ParseTextFile())
		{
			throw(Exception(ERROR_FATAL,"ReadInputFile\n   Error Reading file %s\n",(const char *)m_commandLine.m_inputFileName));
		}
	}
}


enum TextCommands
{
	MOTION_ID,
	MOTION_FILE,
	MOTION_BONEGROUPEXCLUDE,
	MOTION_BONE_GROUP_EXCLUDE,
	MOTION_BONEGROUPFILTER,
	MOTION_BONE_GROUP_FILTER,
	MOTION_ERRORTOLERANCE,
	MOTION_ERROR_TOLERANCE,
	MOTION_ERROR_TOLERANCE_MULTIPLIER,
	MOTION_TRANSLATION_SCALE_FACTOR,
	MOTION_BONE_FILE,
	MOTION_CONTROL,
	MOTION_BONEGROUPTABLE,
	MOTION_BONE_GROUP_TABLE,
	MOTION_SET_NAME,
	MOTION_FLAGS,
	MOTION_BONE_GROUP_INCLUDE,
	MOTION_TRANSLATION_ERROR_TOLERANCE,
	MOTION_TRANSLATION_ERROR_TOLERANCE_MULTIPLIER,
	MOTION_ROTATION_ERROR_TOLERANCE,
	MOTION_ROTATION_ERROR_TOLERANCE_MULTIPLIER,
	MOTION_SCALE_ERROR_TOLERANCE,
	MOTION_SCALE_ERROR_TOLERANCE_MULTIPLIER,
	MOTION_USER_DATA,
	NUM_TEXT_COMMANDS
};

static char *pCommandNames[NUM_TEXT_COMMANDS] =
{
	"MOTION_ID:", // MOTION_ID
	"MOTION_FILE:", // MOTION_FILE
	"MOTION_BONEGROUPEXCLUDE:", // MOTION_BONEGROUPEXCLUDE
	"MOTION_BONE_GROUP_EXCLUDE:", // MOTION_BONE_GROUP_EXCLUDE
	"MOTION_BONEGROUPFILTER:", // MOTION_BONEGROUPEXCLUDE
	"MOTION_BONE_GROUP_FILTER:", // MOTION_BONE_GROUP_EXCLUDE
	"MOTION_ERRORTOLERANCE:", // MOTION_ERRORTOLERANCE
	"MOTION_ERROR_TOLERANCE:", // MOTION_ERROR_TOLERANCE
	"MOTION_ERROR_TOLERANCE_MULTIPLIER:", // MOTION_ERROR_TOLERANCE_MULTIPLIER
	"MOTION_TRANSLATION_SCALE_FACTOR:", // MOTION_TRANSOATION_SCALE_FACTOR
	"MOTION_BONE_FILE:", // MOTION_BONE_FILE
	"MOTION_CONTROL:", // MOTION_CONTROL
	"MOTION_GROUPBONETABLE:", // MOTION_BONEGROUPTABLE
	"MOTION_GROUP_BONE_TABLE:", // MOTION_BONE_GROUP_TABLE
	"MOTION_SET_NAME:", // MOTION_SET_NAME
	"MOTION_FLAGS:", // MOTION_FLAGS
	"MOTION_BONEGROUPINCLUDE:", // MOTION_BONE_GROUP_INCLUDE
	"MOTION_TRANSLATION_ERROR_TOLERANCE:", // MOTION_TRANSLATION_ERROR_TOLERANCE
	"MOTION_TRANSLATION_ERROR_TOLERANCE_MULTIPLIER:", // MOTION_TRANSLATION_ERROR_TOLERANCE_MULTIPLIER
	"MOTION_ROTATION_ERROR_TOLERANCE:", //	MOTION_ROTATION_ERROR_TOLERANCE
	"MOTION_ROTATION_ERROR_TOLERANCE_MULTIPLIER:", // MOTION_ROTATION_ERROR_TOLERANCE_MULTIPLIER
	"MOTION_SCALE_ERROR_TOLERANCE:", // MOTION_SCALE_ERROR_TOLERANCE
	"MOTION_SCALE_ERROR_TOLERANCE_MULTIPLIER:", // MOTION_SCALE_ERROR_TOLERANCE_MULTIPLIER
	"MOTION_USER_DATA:", // MOTION_USER_DATA
};

static const char *spaceString 	= "; ,	|\n\r";     // semi-colon, comma, space, tab.

static const char *EatWhiteSpace(const char *pString)
{
	char curChar;

	while(*pString != 0)
	{
		curChar = *pString;

		/* -----------------2/11/99 12:47PM------------------
		* If the curChar is not a space character then return
		* a pointer to this character.
		* --------------------------------------------------*/
		if(strchr(spaceString, curChar) == NULL)
			return(pString);

		pString++;
	}
	return(NULL);
}

static const char *GetParameter(const char *pString)
{
	// Remove white spaces before command string
	pString 	= EatWhiteSpace(pString);

	if(pString == NULL)
		return(NULL);

	// Find end of command string
	pString  = strpbrk(pString, spaceString);

	if(pString == NULL)
		return(NULL);
	// Remove spaces before command string setting
	pString = EatWhiteSpace(pString);
	return(pString);
}


bool MotionManager::ParseTextFile(void)
{
	const int MAX_LINE_LENGTH = 256;
	FILE		*fp;
	char 		curLine[MAX_LINE_LENGTH];
	char 		curUprLine[MAX_LINE_LENGTH];

	fp = fopen(m_commandLine.m_inputFileName, "rt");
	if(fp == NULL)
	{
		printf("Can't open file %s", (const char *)m_commandLine.m_inputFileName);
		return false;
	}

	MotionEntry currentEntry;
	bool bSetKFMFileOnCurrent = false;

	while(fgets(curLine, MAX_LINE_LENGTH, fp) != NULL)
	{
		// if a comment is in the line then place a null at it's position
		char *pCommentFound = strstr(curLine,"#");
		if (pCommentFound)
		{
			*pCommentFound = '\0';
		}
		strcpy(curUprLine,curLine);
		strupr(curUprLine);

		int foundCommandIndex=-1;
		for (int commandIndex=0;commandIndex<NUM_TEXT_COMMANDS;commandIndex++)
		{
			if(strstr(curUprLine, pCommandNames[commandIndex]) != NULL)
			{
				foundCommandIndex = commandIndex;
				break;
			}
		}
		if (foundCommandIndex!=-1)
		{
			switch (foundCommandIndex)
			{
				case MOTION_BONE_FILE:
				case MOTION_CONTROL:
				case MOTION_FLAGS:
				case MOTION_TRANSLATION_SCALE_FACTOR:
				case MOTION_BONEGROUPTABLE:
				case MOTION_BONE_GROUP_TABLE:
					break;
				case MOTION_ID:
					{
						const char *pInf;
						pInf = GetParameter(curLine);
						if(pInf)
						{
							if (currentEntry.m_flags & MotionEntry::FLAGS_MOTION_ID_NAME)
							{
								if (currentEntry.m_flags & MotionEntry::FLAGS_MOTION_FILE_NAME)
								{
									m_motionEntries.Add(currentEntry);
									currentEntry.Clear();
									TupString newName(pInf);
									newName.TrimRight();
									currentEntry.m_motionIDName = newName;
									currentEntry.m_flags |= MotionEntry::FLAGS_MOTION_ID_NAME;
									int oldUniqueIDSize = m_uniqueMotionIDs.GetSize();
									int newUniqueIDSize = m_uniqueMotionIDs.Add(newName);
									if (newUniqueIDSize != oldUniqueIDSize)
									{
										printf("MOTION_ID: Duplicate ID %s already used\n",(const char *)newName);
										m_bFoundDuplicateMotionIDNames = true;
									}
								}
								else
								{
									TupString addName(pInf);
									addName.TrimRight();
									currentEntry.m_additionalMotionIDNames.Add(addName);
									printf("MOTION_ID: Adding additional ID %s to Motion ID %s\n",(const char *)addName,(const char *)currentEntry.m_motionIDName);
									int oldUniqueIDSize = m_uniqueMotionIDs.GetSize();
									int newUniqueIDSize = m_uniqueMotionIDs.Add(addName);
									if (newUniqueIDSize != oldUniqueIDSize)
									{
										printf("MOTION_ID: Duplicate ID %s already used\n",(const char *)addName);
										m_bFoundDuplicateMotionIDNames = true;
									}
								}
							}
							else
							{
								currentEntry.Clear();
								TupString newName(pInf);
								newName.TrimRight();
								currentEntry.m_motionIDName = newName;
								currentEntry.m_flags |= MotionEntry::FLAGS_MOTION_ID_NAME;
								int oldUniqueIDSize = m_uniqueMotionIDs.GetSize();
								int newUniqueIDSize = m_uniqueMotionIDs.Add(newName);
								if (newUniqueIDSize != oldUniqueIDSize)
								{
									printf("MOTION_ID: Duplicate ID %s already used\n",(const char *)newName);
									m_bFoundDuplicateMotionIDNames = true;
								}
							}
						}
						else
						{
							printf("MOTION_ID: parameter not found\n");
						}
					}
					break;
				case MOTION_FILE:
					{
						const char *pInf;
						pInf = GetParameter(curLine);
						if(pInf)
						{
							// if we already have a filename specified do another entry
							if (currentEntry.m_flags & MotionEntry::FLAGS_MOTION_FILE_NAME)
							{
								m_motionEntries.Add(currentEntry);
								currentEntry.Clear();
								char fName[_MAX_FNAME];
								_splitpath( pInf, NULL, NULL, fName, NULL );
								currentEntry.m_motionIDName = fName;
								currentEntry.m_flags |= MotionEntry::FLAGS_MOTION_ID_NAME;
								int oldUniqueIDSize = m_uniqueMotionIDs.GetSize();
								int newUniqueIDSize = m_uniqueMotionIDs.Add(currentEntry.m_motionIDName);
								if (newUniqueIDSize != oldUniqueIDSize)
								{
									printf("MOTION_ID: Duplicate ID %s already used\n",(const char *)currentEntry.m_motionIDName);
									m_bFoundDuplicateMotionIDNames = true;
								}
							}

							currentEntry.m_fileName = pInf;
							currentEntry.m_fileName.TrimRight();
							currentEntry.m_flags |= MotionEntry::FLAGS_MOTION_FILE_NAME;
						}
						else
						{
							printf("MOTION_FILE: parameter not found for MOTION_ID: %s\n",(const char *)currentEntry.m_motionIDName);
						}
					}
					break;
				case MOTION_BONEGROUPEXCLUDE:
				case MOTION_BONE_GROUP_EXCLUDE:
					{
						const char *pInf;
						pInf = GetParameter(curLine);
						if(pInf)
						{
							char parameter[MAX_LINE_LENGTH];
							strcpy(parameter,pInf);

							int boneGroupID;

							char *pToken;
							pToken = strtok(parameter, "|\n");
							while (pToken != NULL)
							{
								TupString boneName(pToken);
								boneName.TrimLeft();
								boneName.TrimRight();
								boneGroupID = m_pBoneGroupTable->GetBoneGroupFromName(boneName);

								if (boneGroupID==-1)
								{
									sscanf(boneName, "%d", &boneGroupID);
								}
								if(boneGroupID >= 32)
								{
									printf("MOTION_BONEGROUPEXCLUDE: bone group number too large %d for MOTION_ID: %s\n",boneGroupID,(const char *)currentEntry.m_motionIDName);
								}
								else
								{
									currentEntry.m_boneGroupBitMask &= ~(1<<boneGroupID);
									currentEntry.m_flags |= MotionEntry::FLAGS_BONE_GROUP_EXCLUDES;
								}
								// Get next token
								pToken = strtok( NULL, "|\n" );
							}
						}
					}
					break;
				case MOTION_BONEGROUPFILTER:
				case MOTION_BONE_GROUP_FILTER:
				case MOTION_BONE_GROUP_INCLUDE:
					{
						const char *pInf;
						pInf = GetParameter(curLine);
						if(pInf)
						{
							char parameter[MAX_LINE_LENGTH];
							strcpy(parameter,pInf);

							int boneGroupID;
							unsigned int boneGroupMask=0;

							char *pToken;
							pToken = strtok(parameter, "|\n");
							while (pToken != NULL)
							{
								TupString boneName(pToken);
								boneName.TrimLeft();
								boneName.TrimRight();
								boneGroupID = m_pBoneGroupTable->GetBoneGroupFromName(boneName);

								if (boneGroupID==-1)
								{
									sscanf(boneName, "%d", &boneGroupID);
								}
								if(boneGroupID >= 32)
								{
									printf("MOTION_BONEGROUPFILTER: bone group number too large %d for MOTION_ID: %s\n",boneGroupID,(const char *)currentEntry.m_motionIDName);
								}
								else
								{
									boneGroupMask |= (1<<boneGroupID);
									currentEntry.m_flags |= MotionEntry::FLAGS_BONE_GROUP_FILTERS;
								}
								// Get next token
								pToken = strtok( NULL, "|\n" );
							}
							if (boneGroupMask)
							{
								currentEntry.m_boneGroupBitMask &= boneGroupMask;
							}
						}
					}
					break;
				case MOTION_ERRORTOLERANCE:
				case MOTION_ERROR_TOLERANCE:
					{
						const char *pInf;
						float errorTolerance;

						pInf = GetParameter(curLine);
						if(pInf)
						{
							int numScan = sscanf(pInf, "%f", &errorTolerance);
							if( (numScan != 1) || (errorTolerance < 0.0) || (errorTolerance > 100.0) )
							{
								printf("MOTION_ERROR_TOLERANCE: ErrorTolerance invalid or out of range (0.0 .. 100.0) for MOTION_ID: %s",(const char *)currentEntry.m_motionIDName);
							}
							else
							{
								currentEntry.m_transErrorTolerance = errorTolerance;
								currentEntry.m_rotErrorTolerance = errorTolerance;
								currentEntry.m_scaleErrorTolerance = errorTolerance;
								currentEntry.m_flags |= MotionEntry::FLAGS_ERROR_TOLERANCE_ALL;
							}
						}
					}
					break;
				case MOTION_ERROR_TOLERANCE_MULTIPLIER:
					{
						const char *pInf;
						float errorToleranceMultiplier;

						pInf = GetParameter(curLine);
						if(pInf)
						{
							int numScan = sscanf(pInf, "%f", &errorToleranceMultiplier);
							if( (numScan != 1) || (errorToleranceMultiplier < 0.0) || (errorToleranceMultiplier > 1000.0) )
							{
								printf("MOTION_ERROR_TOLERANCE_MULTIPLIER: Warning ErrorTolerance multiplier %f invalid or out of range (0.0 .. 1000.0) for MOTION_ID: %s",errorToleranceMultiplier,(const char *)currentEntry.m_motionIDName);
							}
							currentEntry.m_transErrorToleranceMultiplier = errorToleranceMultiplier;
							currentEntry.m_rotErrorToleranceMultiplier = errorToleranceMultiplier;
							currentEntry.m_scaleErrorToleranceMultiplier = errorToleranceMultiplier;
							currentEntry.m_flags |= MotionEntry::FLAGS_ERROR_TOLERANCE_MULTIPLIER_ALL;
						}
					}
					break;
				case MOTION_TRANSLATION_ERROR_TOLERANCE:
					{
						const char *pInf;
						float errorTolerance;

						pInf = GetParameter(curLine);
						if(pInf)
						{
							int numScan = sscanf(pInf, "%f", &errorTolerance);
							if( (numScan != 1) || (errorTolerance < 0.0) || (errorTolerance > 100.0) )
							{
								printf("MOTION_TRANSLATION_ERROR_TOLERANCE: ErrorTolerance invalid or out of range (0.0 .. 100.0) for MOTION_ID: %s",(const char *)currentEntry.m_motionIDName);
							}
							else
							{
								currentEntry.m_transErrorTolerance = errorTolerance;
								currentEntry.m_flags |= MotionEntry::FLAGS_TRANS_ERROR_TOLERANCE;
							}
						}
					}
					break;
				case MOTION_TRANSLATION_ERROR_TOLERANCE_MULTIPLIER:
					{
						const char *pInf;
						float errorToleranceMultiplier;

						pInf = GetParameter(curLine);
						if(pInf)
						{
							int numScan = sscanf(pInf, "%f", &errorToleranceMultiplier);
							if( (numScan != 1) || (errorToleranceMultiplier < 0.0) || (errorToleranceMultiplier > 1000.0) )
							{
								printf("MOTION_TRANSLATION_ERROR_TOLERANCE_MULTIPLIER: Warning ErrorTolerance multiplier %f invalid or out of range (0.0 .. 1000.0) for MOTION_ID: %s",errorToleranceMultiplier,(const char *)currentEntry.m_motionIDName);
							}
							currentEntry.m_transErrorToleranceMultiplier = errorToleranceMultiplier;
							currentEntry.m_flags |= MotionEntry::FLAGS_TRANS_ERROR_TOLERANCE_MULTIPLIER;
						}
					}
					break;
				case MOTION_ROTATION_ERROR_TOLERANCE:
					{
						const char *pInf;
						float errorTolerance;

						pInf = GetParameter(curLine);
						if(pInf)
						{
							int numScan = sscanf(pInf, "%f", &errorTolerance);
							if( (numScan != 1) || (errorTolerance < 0.0) || (errorTolerance > 100.0) )
							{
								printf("MOTION_ROTATION_ERROR_TOLERANCE: ErrorTolerance invalid or out of range (0.0 .. 100.0) for MOTION_ID: %s",(const char *)currentEntry.m_motionIDName);
							}
							else
							{
								currentEntry.m_rotErrorTolerance = errorTolerance;
								currentEntry.m_flags |= MotionEntry::FLAGS_ROT_ERROR_TOLERANCE;
							}
						}
					}
					break;
				case MOTION_ROTATION_ERROR_TOLERANCE_MULTIPLIER:
					{
						const char *pInf;
						float errorToleranceMultiplier;

						pInf = GetParameter(curLine);
						if(pInf)
						{
							int numScan = sscanf(pInf, "%f", &errorToleranceMultiplier);
							if( (numScan != 1) || (errorToleranceMultiplier < 0.0) || (errorToleranceMultiplier > 1000.0) )
							{
								printf("MOTION_ROTATION_ERROR_TOLERANCE_MULTIPLIER: Warning ErrorTolerance multiplier %f invalid or out of range (0.0 .. 1000.0) for MOTION_ID: %s",errorToleranceMultiplier,(const char *)currentEntry.m_motionIDName);
							}
							currentEntry.m_rotErrorToleranceMultiplier = errorToleranceMultiplier;
							currentEntry.m_flags |= MotionEntry::FLAGS_ROT_ERROR_TOLERANCE_MULTIPLIER;
						}
					}
					break;
				case MOTION_SCALE_ERROR_TOLERANCE:
					{
						const char *pInf;
						float errorTolerance;

						pInf = GetParameter(curLine);
						if(pInf)
						{
							int numScan = sscanf(pInf, "%f", &errorTolerance);
							if( (numScan != 1) || (errorTolerance < 0.0) || (errorTolerance > 100.0) )
							{
								printf("MOTION_SCALE_ERROR_TOLERANCE: ErrorTolerance invalid or out of range (0.0 .. 100.0) for MOTION_ID: %s",(const char *)currentEntry.m_motionIDName);
							}
							else
							{
								currentEntry.m_scaleErrorTolerance = errorTolerance;
								currentEntry.m_flags |= MotionEntry::FLAGS_SCALE_ERROR_TOLERANCE;
							}
						}
					}
					break;
				case MOTION_SCALE_ERROR_TOLERANCE_MULTIPLIER:
					{
						const char *pInf;
						float errorToleranceMultiplier;

						pInf = GetParameter(curLine);
						if(pInf)
						{
							int numScan = sscanf(pInf, "%f", &errorToleranceMultiplier);
							if( (numScan != 1) || (errorToleranceMultiplier < 0.0) || (errorToleranceMultiplier > 1000.0) )
							{
								printf("MOTION_SCALE_ERROR_TOLERANCE_MULTIPLIER: Warning ErrorTolerance multiplier %f invalid or out of range (0.0 .. 1000.0) for MOTION_ID: %s",errorToleranceMultiplier,(const char *)currentEntry.m_motionIDName);
							}
							currentEntry.m_scaleErrorToleranceMultiplier = errorToleranceMultiplier;
							currentEntry.m_flags |= MotionEntry::FLAGS_SCALE_ERROR_TOLERANCE_MULTIPLIER;
						}
					}
					break;
				case MOTION_SET_NAME:
					{
						const char *pInf;
						pInf = GetParameter(curLine);
						if(pInf)
						{
							if(m_commandLine.m_outputFileName.GetLength() != 0)
							{
								if (!m_commandLine.m_bInputFileSetOutputFileName)
								{
									printf("MOTION_SET_NAME (%s) command processed, but command-switch overridden (%s) - using (%s)\n",
										pInf, (const char *)m_commandLine.m_outputFileName, (const char *)m_commandLine.m_outputFileName);
								}
								else
								{
									printf("MOTION_SET_NAME (%s) command found but using prior command - (%s)\n",
										pInf, (const char *)m_commandLine.m_outputFileName);
								}
							}
							else
							{
								m_commandLine.m_bInputFileSetOutputFileName = true;
								m_commandLine.m_outputFileName = pInf;
								m_commandLine.m_outputFileName.TrimRight();
								printf("MOTION_SET_NAME:        %s\n\r", (const char *)m_commandLine.m_outputFileName);
							}
						}
					}
					break;
				case MOTION_USER_DATA:
					{
						const char *pInf;
						pInf = GetParameter(curLine);
						if(pInf)
						{
							TupString parString(pInf);
							int equalIndex = parString.Find('=');
							if (equalIndex!=-1)
							{
								MotionEntry::UserDataEntry userDataEntry;
								userDataEntry.m_keyName = parString.Left(equalIndex);
								userDataEntry.m_keyName.TrimLeft();
								userDataEntry.m_keyName.TrimRight();
								userDataEntry.m_keyName.TrimLeft("\"");
								userDataEntry.m_keyName.TrimRight("\"");
								userDataEntry.m_valueName = parString.Mid(equalIndex+1);
								userDataEntry.m_valueName.TrimLeft();
								userDataEntry.m_valueName.TrimRight();
								userDataEntry.m_valueName.TrimLeft("\"");
								userDataEntry.m_valueName.TrimRight("\"");
								currentEntry.m_userDataEntries.Add(userDataEntry);
							}
							else
							{
								printf("MOTION_USER_DATA: value %s for MOTION_ID: %s needs to be in the format of key=value",pInf,(const char *)currentEntry.m_motionIDName);
							}
						}
						else
						{
							printf("MOTION_USER_DATA: value missing for MOTION_ID: %s needs to be in the format of key=value",(const char *)currentEntry.m_motionIDName);
						}
					}
					break;
			}
		}
		else
		{
			//unrecognized command - try parsing it as a filename
			const char *line = EatWhiteSpace(curLine);//ensure there is data on the line to try to parse as a filename
			if( line && strlen(line) && (strlen(curLine) > 2))
			{
				// if we already have a filename specified do another entry
				if (currentEntry.m_flags & MotionEntry::FLAGS_MOTION_FILE_NAME)
				{
					m_motionEntries.Add(currentEntry);
					currentEntry.Clear();
				}
				if ((currentEntry.m_flags & MotionEntry::FLAGS_MOTION_ID_NAME)==0)
				{
					char fName[_MAX_FNAME];
					_splitpath( line, NULL, NULL, fName, NULL );
					currentEntry.m_motionIDName = fName;
					currentEntry.m_flags |= MotionEntry::FLAGS_MOTION_ID_NAME;
				}
				currentEntry.m_fileName = line;
				currentEntry.m_fileName.TrimRight();
				currentEntry.m_flags |= MotionEntry::FLAGS_MOTION_FILE_NAME;
			}
		}
	}

	// add current if done
	if (currentEntry.m_flags & MotionEntry::FLAGS_MOTION_FILE_NAME)
	{
		m_motionEntries.Add(currentEntry);
	}

	fclose(fp);

	if (m_bFoundDuplicateMotionIDNames)
	{
		printf("Aborting due to duplicate motion ID Names\n");
		return false;
	}
	return true;
}

bool MotionManager::ParseMSetFile(void)
{
	m_motionEntries.RemoveAll();

	TupperwareAggregate *pAgg;
	if (TupperwareLoadSaveManager::Read(m_commandLine.m_inputFileName,&pAgg))
	{
		TupperwareScalar *pDataScalar;

		pDataScalar = pAgg->FindScalarByKey("OutputFileName");
		if (pDataScalar)
		{
			if (pDataScalar->GetDataType()==TupperwareScalar::DATA_STRING)
			{
				if (m_commandLine.m_outputFileName.GetLength())
				{
					printf("OutputFileName %s specified but using command line file %s\n",pDataScalar->GetAsString(),(const char *)m_commandLine.m_outputFileName);
				}
				else
				{
					m_commandLine.m_outputFileName = pDataScalar->GetAsString();
					m_commandLine.m_bInputFileSetOutputFileName = true;
				}
			}
		}

		pDataScalar = pAgg->FindScalarByKey("HeaderFileName");
		if (pDataScalar)
		{
			if (pDataScalar->GetDataType()==TupperwareScalar::DATA_STRING)
			{
				if (m_commandLine.m_headerFileName.GetLength())
				{
					printf("HeaderFileName %s specified but using command line file %s\n",pDataScalar->GetAsString(),(const char *)m_commandLine.m_headerFileName);
				}
				else
				{
					m_commandLine.m_headerFileName = pDataScalar->GetAsString();
					m_commandLine.m_bInputFileSetHeaderFileName = true;
				}
			}
		}

		// get the motion entries      
		for (TupperwareIterator motionIterator = pAgg->FindByKey("Motion"); !motionIterator.Finished(); motionIterator.Next())
		{
			MotionEntry newEntry;

			TupperwareAggregate *pMotionAgg = motionIterator.GetAsAggregate();

			TupperwareList *pDataList;

			// Motion ID
			pDataScalar = pMotionAgg->FindScalarByKey("ID");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_STRING)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_MOTION_ID_NAME;
					newEntry.m_motionIDName = pDataScalar->GetAsString();
				}
			}

			// Motion File Name
			pDataScalar = pMotionAgg->FindScalarByKey("MotionFileName");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_STRING)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_MOTION_FILE_NAME;
					newEntry.m_fileName = pDataScalar->GetAsString();
				}
			}

			// Additional IDs
			for (TupperwareIterator additionIDIterator = pMotionAgg->FindByKey("AdditionalID"); !additionIDIterator.Finished(); additionIDIterator.Next())
			{
				pDataScalar = additionIDIterator.GetAsScalar();
				if (pDataScalar)
				{
					if (pDataScalar->GetDataType()==TupperwareScalar::DATA_STRING)
					{
						TupString additionalName(pDataScalar->GetAsString());
						newEntry.m_additionalMotionIDNames.Add(additionalName);
					}
				}
			}

			// Bone Group Filter
			pDataList = pMotionAgg->FindListByKey("BoneGroupFilter");
			if (pDataList)
			{
				if (pDataList->GetDataType()==TupperwareList::DATA_INT)
				{
					unsigned int boneGroupMask=0;

					int numValues = pDataList->GetLength();
					int *pIntData = pDataList->GetAsInt();
					for (int valueIndex=0;valueIndex<numValues;valueIndex++)
					{
						int value = pIntData[valueIndex];
						if (value<32)
						{
							boneGroupMask |=  (1<<value);
							newEntry.m_flags |= MotionEntry::FLAGS_BONE_GROUP_FILTERS;
						}
					}
					if (boneGroupMask)
					{
						newEntry.m_boneGroupBitMask &= boneGroupMask;
					}
				}
				else if (pDataList->GetDataType()==TupperwareList::DATA_STRING)
				{
					unsigned int boneGroupMask=0;

					int numValues = pDataList->GetLength();
					char **pStringData = pDataList->GetAsString();
					for (int valueIndex=0;valueIndex<numValues;valueIndex++)
					{
						int value = m_pBoneGroupTable->GetBoneGroupFromName(pStringData[valueIndex]);
						if (value==-1)
						{
							printf("BoneGroupName %s not found\n",pStringData[valueIndex]);
						}
						else if (value<32)
						{
							boneGroupMask |=  (1<<value);
							newEntry.m_flags |= MotionEntry::FLAGS_BONE_GROUP_FILTERS;
						}
					}
					if (boneGroupMask)
					{
						newEntry.m_boneGroupBitMask &= boneGroupMask;
					}
				}
			}

			// Exclude Groups
			pDataList = pMotionAgg->FindListByKey("ExcludeBoneGroups");
			if (pDataList)
			{
				if (pDataList->GetDataType()==TupperwareList::DATA_INT)
				{
					int numValues = pDataList->GetLength();
					int *pIntData = pDataList->GetAsInt();
					for (int valueIndex=0;valueIndex<numValues;valueIndex++)
					{
						int value = pIntData[valueIndex];
						if (value<32)
						{
							newEntry.m_boneGroupBitMask &= ~(1<<value);
							newEntry.m_flags |= MotionEntry::FLAGS_BONE_GROUP_EXCLUDES;
						}
					}
				}
				else if (pDataList->GetDataType()==TupperwareList::DATA_STRING)
				{
					unsigned int boneGroupMask=0;

					int numValues = pDataList->GetLength();
					char **pStringData = pDataList->GetAsString();
					for (int valueIndex=0;valueIndex<numValues;valueIndex++)
					{
						int value = m_pBoneGroupTable->GetBoneGroupFromName(pStringData[valueIndex]);
						if (value==-1)
						{
							printf("BoneGroupName %s not found\n",pStringData[valueIndex]);
						}
						else if (value<32)
						{
							newEntry.m_boneGroupBitMask &= ~(1<<value);
							newEntry.m_flags |= MotionEntry::FLAGS_BONE_GROUP_FILTERS;
						}
					}
					if (boneGroupMask)
					{
						newEntry.m_boneGroupBitMask &= boneGroupMask;
					}
				}

			}

			pDataScalar = pMotionAgg->FindScalarByKey("ErrorTolerance");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_ERROR_TOLERANCE_ALL;
					newEntry.m_transErrorTolerance = pDataScalar->GetAsFloat();
					newEntry.m_rotErrorTolerance = pDataScalar->GetAsFloat();
					newEntry.m_scaleErrorTolerance = pDataScalar->GetAsFloat();
				}
			}

			pDataScalar = pMotionAgg->FindScalarByKey("ErrorToleranceMultiplier");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_ERROR_TOLERANCE_MULTIPLIER_ALL;
					newEntry.m_transErrorToleranceMultiplier = pDataScalar->GetAsFloat();
					newEntry.m_rotErrorToleranceMultiplier = pDataScalar->GetAsFloat();
					newEntry.m_scaleErrorToleranceMultiplier = pDataScalar->GetAsFloat();
				}
			}

			pDataScalar = pMotionAgg->FindScalarByKey("TranslationErrorTolerance");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_TRANS_ERROR_TOLERANCE;
					newEntry.m_transErrorTolerance = pDataScalar->GetAsFloat();
				}
			}

			pDataScalar = pMotionAgg->FindScalarByKey("TranslationErrorToleranceMultiplier");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_TRANS_ERROR_TOLERANCE_MULTIPLIER;
					newEntry.m_transErrorToleranceMultiplier = pDataScalar->GetAsFloat();
				}
			}

			pDataScalar = pMotionAgg->FindScalarByKey("RotationErrorTolerance");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_ROT_ERROR_TOLERANCE;
					newEntry.m_rotErrorTolerance = pDataScalar->GetAsFloat();
				}
			}

			pDataScalar = pMotionAgg->FindScalarByKey("RotationErrorToleranceMultiplier");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_ROT_ERROR_TOLERANCE_MULTIPLIER;
					newEntry.m_rotErrorToleranceMultiplier = pDataScalar->GetAsFloat();
				}
			}

			pDataScalar = pMotionAgg->FindScalarByKey("ScaleErrorTolerance");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_SCALE_ERROR_TOLERANCE;
					newEntry.m_scaleErrorTolerance = pDataScalar->GetAsFloat();
				}
			}

			pDataScalar = pMotionAgg->FindScalarByKey("ScaleErrorToleranceMultiplier");
			if (pDataScalar)
			{
				if (pDataScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
				{
					newEntry.m_flags |= MotionEntry::FLAGS_SCALE_ERROR_TOLERANCE_MULTIPLIER;
					newEntry.m_scaleErrorToleranceMultiplier = pDataScalar->GetAsFloat();
				}
			}

			pDataScalar = pMotionAgg->FindScalarByKey("UserData");
			if (pDataScalar)
			{
				
				if (pDataScalar->GetName() && pDataScalar->GetDataType()==TupperwareScalar::DATA_STRING)
				{
					MotionEntry::UserDataEntry userDataEntry;
					userDataEntry.m_keyName = pDataScalar->GetName();
					userDataEntry.m_valueName = pDataScalar->GetAsString();
					newEntry.m_userDataEntries.Add(userDataEntry);
				}
			}

			// if we have a filename
			if (newEntry.m_flags&MotionEntry::FLAGS_MOTION_FILE_NAME)
			{
				// if no motion id then set one from the filename
				if ((newEntry.m_flags&MotionEntry::FLAGS_MOTION_ID_NAME)==0)
				{
					char fName[_MAX_FNAME];
					_splitpath( newEntry.m_fileName, NULL, NULL, fName, NULL );
					newEntry.m_motionIDName = fName;
					newEntry.m_flags |= MotionEntry::FLAGS_MOTION_ID_NAME;
				}
				m_motionEntries.Add(newEntry);
			}
		}

		delete pAgg;
		return true;
	}
	else
	{
		return false;
	}

}


void MotionManager::WriteMSetFile(void)
{
	TupperwareAggregate *pAgg = new TupperwareAggregate;

	if (m_commandLine.m_bInputFileSetOutputFileName)
	{
		pAgg->AddScalarString("OutputFileName",NULL,m_commandLine.m_outputFileName);
	}

	if (m_commandLine.m_headerFileName)
	{
		pAgg->AddScalarString("HeaderFileName",NULL,m_commandLine.m_headerFileName);
	}

	int numMotions = m_motionEntries.GetSize();
	for (int motionIndex=0;motionIndex<numMotions;motionIndex++)
	{
		MotionEntry &motionEntry = m_motionEntries[motionIndex];

		TupperwareAggregate *pMotionAgg = pAgg->AddAggregate("Motion",NULL);

		if (motionEntry.m_flags & MotionEntry::FLAGS_MOTION_ID_NAME)
		{
			pMotionAgg->AddScalarString("ID",NULL,motionEntry.m_motionIDName);
		}

		if (motionEntry.m_flags & MotionEntry::FLAGS_MOTION_FILE_NAME)
		{
			pMotionAgg->AddScalarString("MotionFileName",NULL,motionEntry.m_fileName);
		}

		for (int additionalIDIndex = 0; additionalIDIndex < motionEntry.m_additionalMotionIDNames.GetSize(); additionalIDIndex++)
		{
			pMotionAgg->AddScalarString("AdditionalID",NULL,motionEntry.m_additionalMotionIDNames[additionalIDIndex]);
		}

		// if any bones are excluded
		if (motionEntry.m_flags & (MotionEntry::FLAGS_BONE_GROUP_EXCLUDES|MotionEntry::FLAGS_BONE_GROUP_FILTERS))
		{
			int numIncludedBones = 0;
			for (int bit=0;bit<32;bit++)
			{
				if ((1<<bit)&motionEntry.m_boneGroupBitMask)
					numIncludedBones++;
			}

			// exclude or filter
			if (numIncludedBones>16)
			{
				TupArray<int> excludedBones;
				for (int bit=0;bit<32;bit++)
				{
					if (((1<<bit)&motionEntry.m_boneGroupBitMask)==0)
					{
						excludedBones.Add(bit);
					}
				}
				pMotionAgg->AddListInt("ExcludeBoneGroups",NULL,excludedBones);
			}
			else
			{
				TupArray<int> includedBones;
				for (int bit=0;bit<32;bit++)
				{
					if ((1<<bit)&motionEntry.m_boneGroupBitMask)
					{
						includedBones.Add(bit);
					}
				}
				pMotionAgg->AddListInt("BoneGroupFilter",NULL,includedBones);
			}
		}

		if ((motionEntry.m_flags & MotionEntry::FLAGS_ERROR_TOLERANCE_ALL) == MotionEntry::FLAGS_ERROR_TOLERANCE_ALL &&
			motionEntry.m_transErrorTolerance == motionEntry.m_rotErrorTolerance &&
			motionEntry.m_transErrorTolerance == motionEntry.m_scaleErrorTolerance)
		{
			pMotionAgg->AddScalarFloat("ErrorTolerance",NULL,(float)motionEntry.m_transErrorTolerance);
		}
		else
		{
			if (motionEntry.m_flags & MotionEntry::FLAGS_TRANS_ERROR_TOLERANCE)
			{
				pMotionAgg->AddScalarFloat("TranslationErrorTolerance",NULL,(float)motionEntry.m_transErrorTolerance);
			}
			if (motionEntry.m_flags & MotionEntry::FLAGS_ROT_ERROR_TOLERANCE)
			{
				pMotionAgg->AddScalarFloat("RotationErrorTolerance",NULL,(float)motionEntry.m_rotErrorTolerance);
			}
			if (motionEntry.m_flags & MotionEntry::FLAGS_SCALE_ERROR_TOLERANCE)
			{
				pMotionAgg->AddScalarFloat("ScaleErrorTolerance",NULL,(float)motionEntry.m_scaleErrorTolerance);
			}
		}

		if ((motionEntry.m_flags & MotionEntry::FLAGS_ERROR_TOLERANCE_MULTIPLIER_ALL) == MotionEntry::FLAGS_ERROR_TOLERANCE_MULTIPLIER_ALL &&
			motionEntry.m_transErrorToleranceMultiplier == motionEntry.m_rotErrorToleranceMultiplier &&
			motionEntry.m_transErrorToleranceMultiplier == motionEntry.m_scaleErrorToleranceMultiplier)
		{
			pMotionAgg->AddScalarFloat("ErrorToleranceMultiplier",NULL,(float)motionEntry.m_transErrorToleranceMultiplier);
		}
		else
		{
			if (motionEntry.m_flags & MotionEntry::FLAGS_TRANS_ERROR_TOLERANCE_MULTIPLIER)
			{
				pMotionAgg->AddScalarFloat("TranslationErrorToleranceMultiplier",NULL,(float)motionEntry.m_transErrorToleranceMultiplier);
			}
			if (motionEntry.m_flags & MotionEntry::FLAGS_ROT_ERROR_TOLERANCE_MULTIPLIER)
			{
				pMotionAgg->AddScalarFloat("RotationErrorToleranceMultiplier",NULL,(float)motionEntry.m_rotErrorToleranceMultiplier);
			}
			if (motionEntry.m_flags & MotionEntry::FLAGS_SCALE_ERROR_TOLERANCE_MULTIPLIER)
			{
				pMotionAgg->AddScalarFloat("ScaleErrorToleranceMultiplier",NULL,(float)motionEntry.m_scaleErrorToleranceMultiplier);
			}
		}
	}

	if (!TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,m_commandLine.m_outputMSetFileName,pAgg))
	{
		delete pAgg;
		throw(Exception(ERROR_FATAL,"WriteMSetFile\n   Error Writing %s\n",(const char *)m_commandLine.m_outputFileName));
	}
	delete pAgg;
}


void MotionManager::PrintInfo( const char *pcFmt, ...)
{
	if(m_commandLine.m_bVerbose)
	{
		char Buffer[1024];

		va_list				PrintArgs;

		va_start( PrintArgs, pcFmt);

		vsprintf( Buffer, pcFmt, PrintArgs);
		printf( "VERBOSE: %s", Buffer);

		va_end( PrintArgs);
	}
}

void MotionManager::PrintError( const char *pcFmt, ...)
{
	char Buffer[1024];

	va_list				PrintArgs;

	va_start( PrintArgs, pcFmt);

	vsprintf( Buffer, pcFmt, PrintArgs);
	printf( "Error: %s\n", Buffer);
	m_numErrors++;

	va_end( PrintArgs);
}

void MotionManager::ReadMotionAndAnalyze(void)
{
   int numErrorFiles=0;
   int numEntries = m_motionEntries.GetSize();
   for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
   {
      MotionEntry &entry = m_motionEntries[entryIndex];
      printf("Reading Motion file %s\n",(const char *)entry.m_fileName);
      if (!entry.ReadMotion())
      {
         printf("Error Reading Motion file %s\n",(const char *)entry.m_fileName);
         numErrorFiles++;
      }
		else
		{
			printf("Analyzing Motion file %s\n",(const char *)entry.m_fileName);
			AnalyzeMotion(entry);
			entry.m_KFMData.Clear(); // clear the kfm data after analyze phase
		}
   }
   if (numErrorFiles)
   {
      throw(Exception(ERROR_FATAL,"ReadMotionFiles\n   Error Reading %d Motion files\n",numErrorFiles));
   }
}

void MotionManager::ReadBoneGroupTableFile(void)
{
	if (!m_commandLine.m_boneGroupTableFileName.GetLength())
	{
		throw (Exception(ERROR_FATAL,"ReadBoneGroupTableFile\n  Bone group table file not specified."));
	}
	delete m_pBoneGroupTable;
	m_pBoneGroupTable = NULL;
	printf("Reading Bone Group Table file %s\n",(const char *)m_commandLine.m_boneGroupTableFileName);
	TupperwareAggregate *pAgg=NULL;
	if (TupperwareLoadSaveManager::Read(m_commandLine.m_boneGroupTableFileName,&pAgg))
	{
		try
		{
			m_pBoneGroupTable = new BoneGroupTable(pAgg);
		}
		catch (Exception e)
		{
			throw (Exception(ERROR_FATAL,"ReadBoneGroupTableFile\n  %s",(const char *)(e)));
		}
		catch (...)
		{
			throw (Exception(ERROR_FATAL,"ReadBoneGroupTableFile\n  Unhandled Error"));
		}
		delete pAgg;
		pAgg = NULL;
	}
	else
	{
		throw (Exception(ERROR_FATAL,"ReadBoneGroupTableFile\n  Error reading Bone Group Table File %s\n.",
			(const char *)m_commandLine.m_boneGroupTableFileName));
	}
}

void MotionManager::WriteHeaderFile(void)
{
	TupString headerFileName;

	if (m_commandLine.m_headerFileName.GetLength()==0)
	{
		char fname[_MAX_FNAME];
		FilePath::GetFileName(m_commandLine.m_inputFileName,fname);
		headerFileName = fname;
		headerFileName+="_m.h";
	}
	else
	{
		headerFileName = m_commandLine.m_headerFileName;
	}

	printf("Writing header file:   %s\n", (const char *)headerFileName);

	FILE *fp = fopen(headerFileName, "wt");
	if(!fp)
	{
		throw (Exception(ERROR_FATAL,"Could not open file %s for writing\n",(const char *)headerFileName));
	}

	int numMotions = m_motionEntries.GetSize();
	// loop through motions (kfm files)
	for (int motionIndex=0;motionIndex<numMotions;motionIndex++)
	{
		MotionEntry &motionEntry = m_motionEntries[motionIndex];
		TupString motionIDName = motionEntry.m_motionIDName;
		motionIDName.MakeUpper();
		fprintf(fp, "#define %-40s 0x%08x\n", (const char *)motionIDName, ((1000 << 16) | motionIndex));
		for (int additionalIndex=0; additionalIndex < motionEntry.m_additionalMotionIDNames.GetSize(); additionalIndex++)
		{
			TupString additionalMotionIDName = motionEntry.m_additionalMotionIDNames[additionalIndex];
			additionalMotionIDName.MakeUpper();
			fprintf(fp, "#define %-40s 0x%08x\n", (const char *)additionalMotionIDName, ((1000 << 16) | motionIndex));
		}
	}
	fclose(fp);
}

void MotionManager::AnalyzeMotion(MotionEntry &motionEntry)
{
	KFM &kfm = motionEntry.m_KFMData;
	AnalyzedData &analyzedData = motionEntry.m_analyzedData;

	// if error tolerance is not set then use the command line value
	if ((motionEntry.m_flags & MotionEntry::FLAGS_TRANS_ERROR_TOLERANCE)==0)
	{
		motionEntry.m_transErrorTolerance = m_commandLine.m_transErrorTolerance;
	}
	if ((motionEntry.m_flags & MotionEntry::FLAGS_ROT_ERROR_TOLERANCE)==0)
	{
		motionEntry.m_rotErrorTolerance = m_commandLine.m_rotErrorTolerance;
	}
	if ((motionEntry.m_flags & MotionEntry::FLAGS_SCALE_ERROR_TOLERANCE)==0)
	{
		motionEntry.m_scaleErrorTolerance = m_commandLine.m_scaleErrorTolerance;
	}

	int numChannels = kfm.m_channels.GetSize();
	motionEntry.m_analyzedData.m_channels.SetSize(numChannels);

	analyzedData.m_totalFrameTime = (unsigned short)ceil(kfm.m_length.GetSeconds() * ANIMFRAMES_PER_SECOND); //round up to nearest integral frame

	// loop through bones on a kfm

	int firstChannel = m_commandLine.m_bExcludeModelBone ? 1 : 0;
	for (int channelIndex=firstChannel;channelIndex<numChannels;channelIndex++)
	{
		KFM::Channel &srcChannel = kfm.m_channels[channelIndex];
		AnalyzedData::Channel &dstChannel = analyzedData.m_channels[channelIndex];

		dstChannel.m_name = srcChannel.m_name;
		m_totalSourceChannels++;
		// get integer bone group number from the bone group table using the name of the bone
		dstChannel.m_boneGroupId = (unsigned short)m_pBoneGroupTable->GetBoneGroup(dstChannel.m_name);
		if (dstChannel.m_boneGroupId==(unsigned short)-1)
			dstChannel.m_boneGroupId = 0; // use default bone group if not found in table
		// is the bone name one we want to use in the database
		// if this bone is an orient like bone or the like it will be skipped
		if (m_pBoneGroupTable->IsValidMotionChannel(dstChannel.m_name)!=0)
		{
			PrintInfo("Valid Channel %s belongs to bone group %d\n",(const char *)dstChannel.m_name,dstChannel.m_boneGroupId);

			PrintInfo( " B2P=%.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f\n", 
			(float)srcChannel.m_parentToBoneMatrix.GetXAxis().x,
			(float)srcChannel.m_parentToBoneMatrix.GetXAxis().y,
			(float)srcChannel.m_parentToBoneMatrix.GetXAxis().z,
			(float)srcChannel.m_parentToBoneMatrix.GetYAxis().x,
			(float)srcChannel.m_parentToBoneMatrix.GetYAxis().y,
			(float)srcChannel.m_parentToBoneMatrix.GetYAxis().z,
			(float)srcChannel.m_parentToBoneMatrix.GetZAxis().x,
			(float)srcChannel.m_parentToBoneMatrix.GetZAxis().y,
			(float)srcChannel.m_parentToBoneMatrix.GetZAxis().z);

//           printf("Analyzing: %s.\n",dstChannel.m_name);

			dstChannel.m_channelItems.SetSize(KFM::ChannelItem::NUMCHANNELITEMS);
			int channelItemIndex;
			for (channelItemIndex=0;channelItemIndex<KFM::ChannelItem::NUMCHANNELITEMS;channelItemIndex++)
			{
				m_totalSourceChannelItems++;
				KFM::ChannelItem &srcChannelItem = srcChannel.m_channelItems[channelItemIndex];
				AnalyzedData::ChannelItem &dstChannelItem = dstChannel.m_channelItems[channelItemIndex];
				switch (channelItemIndex)
				{
					case KFM::ChannelItem::XTRANS:
					case KFM::ChannelItem::YTRANS:
					case KFM::ChannelItem::ZTRANS:
						BuildChannelItemKeys(motionEntry,srcChannelItem,dstChannelItem,channelItemIndex,motionEntry.m_transErrorTolerance*motionEntry.m_transErrorToleranceMultiplier);
						break;
					case KFM::ChannelItem::XSCALE:
					case KFM::ChannelItem::YSCALE:
					case KFM::ChannelItem::ZSCALE:
						BuildChannelItemKeys(motionEntry,srcChannelItem,dstChannelItem,channelItemIndex,motionEntry.m_scaleErrorTolerance*motionEntry.m_scaleErrorToleranceMultiplier);
						break;
					case KFM::ChannelItem::XQUAT:
					case KFM::ChannelItem::YQUAT:
					case KFM::ChannelItem::ZQUAT:
					case KFM::ChannelItem::WQUAT:
						BuildChannelItemKeys(motionEntry,srcChannelItem,dstChannelItem,channelItemIndex,motionEntry.m_rotErrorTolerance*motionEntry.m_rotErrorToleranceMultiplier);
						break;
				}
			}

			// check to see which channel items are needing to be output
			dstChannel.m_activeChannelItems = GetActiveChannelItems(dstChannel);

			// not excluded from the bone groups
			if ((1<<dstChannel.m_boneGroupId) & motionEntry.m_boneGroupBitMask)
			{
				// update the bone groups actually used for this motion
				// this needs to be set regardless of the channel actually being used
				analyzedData.m_boneGroupsUsed|=(1<<dstChannel.m_boneGroupId); 

				// actually has some non default channel item data
				if (dstChannel.m_activeChannelItems)
				{
					m_totalChannels++;

					dstChannel.m_flags |= AnalyzedData::Channel::FLAGS_ACTIVE_CHANNEL; // this one is going to go out
					analyzedData.m_totalBonesUsed++; // count how many bones are used for this motion

					// add bone name and matrix to the bone manager (these are shared between the motions)
					BoneManager::BoneEntry boneEntry;
					boneEntry.m_motionName = motionEntry.m_fileName;
					boneEntry.m_bonetoparent.e00 = (float)srcChannel.m_parentToBoneMatrix.GetXAxis().x;
					boneEntry.m_bonetoparent.e01 = (float)srcChannel.m_parentToBoneMatrix.GetXAxis().y;
					boneEntry.m_bonetoparent.e02 = (float)srcChannel.m_parentToBoneMatrix.GetXAxis().z;
					boneEntry.m_bonetoparent.e10 = (float)srcChannel.m_parentToBoneMatrix.GetYAxis().x;
					boneEntry.m_bonetoparent.e11 = (float)srcChannel.m_parentToBoneMatrix.GetYAxis().y;
					boneEntry.m_bonetoparent.e12 = (float)srcChannel.m_parentToBoneMatrix.GetYAxis().z;
					boneEntry.m_bonetoparent.e20 = (float)srcChannel.m_parentToBoneMatrix.GetZAxis().x;
					boneEntry.m_bonetoparent.e21 = (float)srcChannel.m_parentToBoneMatrix.GetZAxis().y;
					boneEntry.m_bonetoparent.e22 = (float)srcChannel.m_parentToBoneMatrix.GetZAxis().z;
					boneEntry.m_name = dstChannel.m_name;
					boneEntry.m_groupNumber = dstChannel.m_boneGroupId;
					dstChannel.m_boneManagerIndex = m_boneManager.AddBone(boneEntry);

					// adjust values of dest channels
					for (int channelItemIndex=0;channelItemIndex<KFM::ChannelItem::NUMCHANNELITEMS;channelItemIndex++)
					{
						if (dstChannel.m_activeChannelItems&(1<<channelItemIndex)) // is it needed?
						{
							m_totalChannelItems++;

							KFM::ChannelItem &srcChannelItem = srcChannel.m_channelItems[channelItemIndex];
							AnalyzedData::ChannelItem &dstChannelItem = dstChannel.m_channelItems[channelItemIndex];
							BuildChannelItem(motionEntry,srcChannel,dstChannelItem,channelItemIndex);
							if ((dstChannelItem.m_flags & AnalyzedData::ChannelItem::FLAGS_FLAT_NON_DEFAULT_KEYS)==0)
								m_totalLinearKeys += dstChannelItem.m_keyframes.GetSize();
							m_totalSourceKeys += srcChannelItem.m_keyframes.GetSize();
						}
						else
						{
							m_totalDefaultChannelItems++;
						}
					}
				}
				else
				{
					m_totalDefaultChannels++;
				}
			}
			else
			{
				m_totalIgnoredChannels++;
			}
		}
		else
		{
			PrintInfo("Invalid Channel %s belongs to bone group %d\n",(const char *)dstChannel.m_name,dstChannel.m_boneGroupId);
			m_totalIgnoredChannels++;
		}
	}
}

void MotionManager::PrintAnalyzeStats(void)
{
	printf("Source KFM Statistics\n");
	printf("Total Source Channels %d\n",m_totalSourceChannels);
	printf("Total Source Channel Items %d\n",m_totalSourceChannelItems);
	printf("Total Source Keys %d\n",m_totalSourceKeys);
	printf("Analyzed Data Statistics\n");
	printf("Total Channels %d\n",m_totalChannels);
	printf("Total Ignored Channels %d\n",m_totalIgnoredChannels);
	printf("Total Default Channels %d\n",m_totalDefaultChannels);
	printf("Total Channel Items %d\n",m_totalChannelItems);
	printf("Total Default Channel Items %d\n",m_totalDefaultChannelItems);
	printf("Total Flat Nondefault Channel Items %d\n",m_totalFlatNonDefaultChannelItems);
	printf("Total Linear Fit Keys %d\n",m_totalLinearKeys);
}

void MotionManager::BuildChannelItemKeys(const MotionEntry &motionEntry,const KFM::ChannelItem &srcChannelItem,AnalyzedData::ChannelItem &dstChannelItem,int channelItemIndex,double errorTolerance)
{
	HashTime endTime = srcChannelItem.m_keyframes[srcChannelItem.m_keyframes.GetSize()-1].m_time;
	// sample at 300 fps when we have a valid error tolerance
	// sample at 30 fps if the error tolerance is zero
	HashTime sampleInterval(1/300.0);
	if (errorTolerance==0)
		sampleInterval = (1/30.0);

	int numSamples = (int)(endTime/sampleInterval)+1;
	// we need at least 2 samples
	if (numSamples<2)
		numSamples=2;
	endTime = sampleInterval*numSamples;
	TupArray<HashVector2> sampleArray;
	sampleArray.SetSize(numSamples);
	double maxValue = -1.0e20;
	double minValue = 1.0e20;
	double scaleFactor = 1.0;

	// set scale if translation
	if (channelItemIndex>=KFM::ChannelItem::XTRANS && channelItemIndex<=KFM::ChannelItem::ZTRANS)
		scaleFactor = KFM_TO_ENGINE_SCALE_FACTOR;

	for (int sampleIndex=0;sampleIndex<numSamples;sampleIndex++)
	{
		HashTime sampleTime = sampleInterval * sampleIndex;
		double value = srcChannelItem.Interpolate(sampleTime) * scaleFactor;
		sampleArray[sampleIndex].x = sampleTime.GetSeconds();
		sampleArray[sampleIndex].y = value;
		if (value>maxValue)
			maxValue = value;
		if (value<minValue)
			minValue = value;
	}

	double maxExtent = GenMath::MAX(::fabs(maxValue),::fabs(minValue));
	// smallest it can be is 1
	if (maxExtent<1.0) 
		maxExtent = 1.0;

	dstChannelItem.m_errorFactor = maxExtent*errorTolerance;

	dstChannelItem.m_maxValue = maxValue;
	dstChannelItem.m_minValue = minValue;

	TupArray<HashVector2> newLinearPoints;
	FitCurve(sampleArray,newLinearPoints,dstChannelItem.m_errorFactor);
	int numKeyFrames = newLinearPoints.GetSize();
	dstChannelItem.m_keyframes.SetSize(numKeyFrames);

	int keyFrameIndex;
	for (keyFrameIndex=0;keyFrameIndex<numKeyFrames;keyFrameIndex++)
	{
		dstChannelItem.m_keyframes[keyFrameIndex].m_doubleTime = newLinearPoints[keyFrameIndex].x;
		dstChannelItem.m_keyframes[keyFrameIndex].m_doubleValue = newLinearPoints[keyFrameIndex].y;
	}
}

unsigned short MotionManager::GetActiveChannelItems(const AnalyzedData::Channel &channel)
{
	//usually, lots of the channelitems are not adjusted by artist, and can be defaulted to save space
	//determine which are NOT defaulted and set a bit in a bit array to indicate such

	unsigned short usChannelsPresent = 0;

	int channelItemIndex;
	for(channelItemIndex=AnalyzedData::ChannelItem::XTRANS; channelItemIndex<=AnalyzedData::ChannelItem::ZTRANS; channelItemIndex++)
	{
		const AnalyzedData::ChannelItem &channelItem = channel.m_channelItems[channelItemIndex];
		if (!Zero(channelItem.m_maxValue,channelItem.m_errorFactor) || !Zero(channelItem.m_minValue,channelItem.m_errorFactor))
		{
			usChannelsPresent |= 1<<channelItemIndex;
		}
	}
	for( channelItemIndex=AnalyzedData::ChannelItem::XSCALE; channelItemIndex<=AnalyzedData::ChannelItem::ZSCALE; channelItemIndex++)
	{
		const AnalyzedData::ChannelItem &channelItem = channel.m_channelItems[channelItemIndex];
		if (!Equal(channelItem.m_maxValue,1.0,channelItem.m_errorFactor) || !Equal(channelItem.m_minValue,1,channelItem.m_errorFactor))
		{
			usChannelsPresent |= 1<<channelItemIndex;
		}
	}
	for( channelItemIndex=AnalyzedData::ChannelItem::XQUAT; channelItemIndex<=AnalyzedData::ChannelItem::ZQUAT; channelItemIndex++)
	{
		const AnalyzedData::ChannelItem &channelItem = channel.m_channelItems[channelItemIndex];
		if (!Zero(channelItem.m_maxValue,channelItem.m_errorFactor) || !Zero(channelItem.m_minValue,channelItem.m_errorFactor))
		{
			usChannelsPresent |= 1<<channelItemIndex;
		}
	}
	for( channelItemIndex=AnalyzedData::ChannelItem::WQUAT; channelItemIndex<=AnalyzedData::ChannelItem::WQUAT; channelItemIndex++)
	{
		const AnalyzedData::ChannelItem &channelItem = channel.m_channelItems[channelItemIndex];
		if (!Equal(channelItem.m_maxValue,1.0,channelItem.m_errorFactor) || !Equal(channelItem.m_minValue,1,channelItem.m_errorFactor))
		{
			usChannelsPresent |= 1<<channelItemIndex;
		}
	}

	return usChannelsPresent;
}

void MotionManager::BuildChannelItem(const MotionEntry &motionEntry,const KFM::Channel &srcChannel,AnalyzedData::ChannelItem &dstChannelItem,int channelItemIndex)
{
	int numKeyFrames = dstChannelItem.m_keyframes.GetSize();

	// if we have too many keyframes to be represented in a unsigned short
	if ( numKeyFrames > USHRT_MAX)
	{
		PrintError("Channel %s Bone %s from KFM %s has too many keyframes (%d)\n",
			pChannelItemNames[channelItemIndex],
			(const char *)srcChannel.m_name,
			(const char *)motionEntry.m_fileName,
			numKeyFrames);
	}

	// if the keys are flat
	if (::fabs(dstChannelItem.m_maxValue-dstChannelItem.m_minValue)<=dstChannelItem.m_errorFactor)
	{
		dstChannelItem.m_flags |= AnalyzedData::ChannelItem::FLAGS_FLAT_NON_DEFAULT_KEYS;
		m_totalFlatNonDefaultChannelItems++;
	}

	double maxValue = dstChannelItem.m_maxValue;
	double minValue = dstChannelItem.m_minValue;

	double maxExtent = GenMath::MAX(::fabs(maxValue),::fabs(minValue));
	// smallest it can be is 1
	if (maxExtent<1.0) 
		maxExtent = 1.0;

	dstChannelItem.m_scaleValue = maxExtent/MAX_ANIM_VALUE;
	double floatToShortValue = MAX_ANIM_VALUE/maxExtent;

	int keyFrameIndex;
	// deal with the time values for each keyframe
	for (keyFrameIndex=0;keyFrameIndex<numKeyFrames;keyFrameIndex++)
	{
		//get the keyframe time and ensure it can be converteed to 16bits worth of jiffies 
		double seconds = dstChannelItem.m_keyframes[keyFrameIndex].m_doubleTime;
		if ( SECONDS_TO_JIFFIES(seconds) > MAX_JIFFIES )
		{
			PrintError("Channel %s Bone %s from KFM %s is longer (%f seconds) than allowed (%f)\n",
				pChannelItemNames[channelItemIndex],
				(const char *)srcChannel.m_name,
				(const char *)motionEntry.m_fileName,
				seconds,
				MAX_JIFFIES);
		}
		dstChannelItem.m_keyframes[keyFrameIndex].m_shortTime = SECONDS_TO_JIFFIES(seconds);

		// set up the short value
		double srcValue = dstChannelItem.m_keyframes[keyFrameIndex].m_doubleValue;
		dstChannelItem.m_keyframes[keyFrameIndex].m_shortValue = FLOAT_TO_ANIMVALUE(srcValue,floatToShortValue);
	}
}

class MotionInfo
{
public:
	int m_numChannels;
	int m_numChannelItems;
	int m_numKeyframes;
	int m_totalOutputSize;
	TupString m_name;
	TupString m_filename;
	int m_uniqueFilenameIndex;
};

class ChannelInfo
{
public:
	TupString m_name;
	int m_numMotions;
	int m_numChannelItems;
	int m_numKeyframes;
	int m_totalOutputSize;

	ChannelInfo(void) : m_numMotions(0),m_numChannelItems(0),m_numKeyframes(0),m_totalOutputSize(0)
	{
	}

	bool ChannelInfo::operator==(const ChannelInfo& other) const
	{
		return (strcmp(m_name,other.m_name)==0);
	}

	bool ChannelInfo::operator<=(const ChannelInfo& other) const
	{
		return (strcmp(m_name,other.m_name)<1);
	}

	bool ChannelInfo::operator>=(const ChannelInfo& other) const
	{
		return (strcmp(m_name,other.m_name)>-1);
	}

	bool ChannelInfo::operator<(const ChannelInfo& other) const
	{
		return (strcmp(m_name,other.m_name)<0);
	}

	bool ChannelInfo::operator>(const ChannelInfo& other) const
	{
		return (strcmp(m_name,other.m_name)>0);
	}
};

void MotionManager::WriteDatabaseFile(void)
{
	try
	{
		printf("Writing Motion Database file %s\n",(const char *)m_commandLine.m_outputFileName);

		int numMotions = m_motionEntries.GetSize();
		TupArray<LINK_FROM_HANDLE> motionLinks;
		motionLinks.SetSize(numMotions);

		DBWriter dbWriter;
		dbWriter.StartDBLFile(m_commandLine.m_outputFileName,NULL,false,"DB");
		dbWriter.StartChunk( DB_MOTION, DB_MOTION_VERSION,0, "1000" );

		// file header
		BLOCK_HANDLE motionFileHeaderBlock = dbWriter.StartBlock("Motion File Header");
		MotionStructs::MotionFileHeader motionFileHeader;
		memset(&motionFileHeader,0,sizeof(motionFileHeader));
		motionFileHeader.m_numMotions = numMotions;
		motionFileHeader.m_numBoneEntries = m_boneManager.GetNumBones();
		dbWriter.WriteData(&motionFileHeader,sizeof(motionFileHeader));
		LINK_FROM_HANDLE motionFileHeaderBlockMotionTableFromLink = dbWriter.RegisterLinkFrom(motionFileHeaderBlock,GET_OFFSET(motionFileHeader,motionFileHeader.m_pMotionEntryTable),"m_pMotionEntryTable");
		LINK_FROM_HANDLE motionFileHeaderBlockBoneEntriesFromLink = dbWriter.RegisterLinkFrom(motionFileHeaderBlock,GET_OFFSET(motionFileHeader,motionFileHeader.m_pBoneEntries),"m_pBoneEntries");
		dbWriter.EndBlock();
		printf("Motion File Header size is %d bytes\n",dbWriter.GetBlockSize(motionFileHeaderBlock));

		// link from file header to first point in table
		BLOCK_HANDLE motionEntryPtrsBlock = dbWriter.StartBlock("Motion Table Pointers");
		LINK_TO_HANDLE motionEntryPtrsToLink = dbWriter.RegisterLinkTo(motionEntryPtrsBlock,0,"Pointer to Table");
		dbWriter.ConnectLinks(motionFileHeaderBlockMotionTableFromLink,motionEntryPtrsToLink);

		// space for the motion entry pointers
		int motionIndex;
		unsigned long pad = 0;
		for (motionIndex=0;motionIndex<numMotions;motionIndex++)
		{
			motionLinks[motionIndex] = dbWriter.RegisterLinkFrom(motionEntryPtrsBlock,motionIndex*sizeof(u32),"Pointer %d",motionIndex);
			dbWriter.WriteData(&pad,sizeof(pad));
		}
		dbWriter.EndBlock();
		printf("Motion Table Pointers size is %d bytes\n",dbWriter.GetBlockSize(motionEntryPtrsBlock));

		// save out bone entries
		int numBoneEntries = m_boneManager.GetNumBones();
		TupArray<LINK_TO_HANDLE> boneEntryLinks;
		boneEntryLinks.SetSize(numBoneEntries);
		u32 totalBoneTableEntriesSize = 0;
		for (int boneEntryIndex=0;boneEntryIndex<numBoneEntries;boneEntryIndex++)
		{
			BLOCK_HANDLE boneEntryBlock = dbWriter.StartBlock("BoneEntry %d",boneEntryIndex);

			const BoneManager::BoneEntry &boneEntry = m_boneManager.GetBone(boneEntryIndex);
			MotionStructs::BoneEntry dstBoneEntry;
			memset(&dstBoneEntry,0,sizeof(dstBoneEntry));
			strncpy((char *)&dstBoneEntry.m_name, boneEntry.m_name, sizeof(dstBoneEntry.m_name));
			dstBoneEntry.m_groupNo = boneEntry.m_groupNumber;
			dstBoneEntry.m_bonetoparent.e00 = boneEntry.m_bonetoparent.e00;
			dstBoneEntry.m_bonetoparent.e01 = boneEntry.m_bonetoparent.e01;
			dstBoneEntry.m_bonetoparent.e02 = boneEntry.m_bonetoparent.e02;
			dstBoneEntry.m_bonetoparent.e10 = boneEntry.m_bonetoparent.e10;
			dstBoneEntry.m_bonetoparent.e11 = boneEntry.m_bonetoparent.e11;
			dstBoneEntry.m_bonetoparent.e12 = boneEntry.m_bonetoparent.e12;
			dstBoneEntry.m_bonetoparent.e20 = boneEntry.m_bonetoparent.e20;
			dstBoneEntry.m_bonetoparent.e21 = boneEntry.m_bonetoparent.e21;
			dstBoneEntry.m_bonetoparent.e22 = boneEntry.m_bonetoparent.e22;

			// get link to bone entry
			boneEntryLinks[boneEntryIndex] = dbWriter.RegisterLinkTo(boneEntryBlock,0,"Link To Start");

			// link file header to first bone entry
			if (boneEntryIndex==0)
			{
				dbWriter.ConnectLinks(motionFileHeaderBlockBoneEntriesFromLink,boneEntryLinks[boneEntryIndex]);
			}

			dbWriter.WriteData(&dstBoneEntry,sizeof(dstBoneEntry));
			dbWriter.EndBlock();
			totalBoneTableEntriesSize+=dbWriter.GetBlockSize(boneEntryBlock);
			m_totalBoneEntriesWritten++;
		}
		printf("Bone Entries size is %d bytes\n",totalBoneTableEntriesSize);

		int numChannelsProcessed = 0;

		TupArray<MotionInfo> motionInfoTable;
		Keeper<TupString> uniqueFilenames;
		TupArray<int> numUsingUniqueFilenames;
		bool bSomeDuplicateNames = false;

		Keeper<TupString> uniqueChannelNames;
		Keeper<ChannelInfo> channelInfoTable;

		for (motionIndex=0;motionIndex<numMotions;motionIndex++)
		{
			MotionEntry &motionEntry = m_motionEntries[motionIndex];
//			KFM &kfm = motionEntry.m_KFMData;
			AnalyzedData &analyzedData = motionEntry.m_analyzedData;

			u32 motionSize = 0;
			int totalMotionChannels=0;
			int totalMotionChannelItems=0;
			int totalMotionChannelItemKeys=0;

			BLOCK_HANDLE motionEntryBlock = dbWriter.StartBlock("Motion Entry Header %s",motionEntry.m_motionIDName);
			LINK_TO_HANDLE linkTo = dbWriter.RegisterLinkTo(motionEntryBlock,0,"Start");
			dbWriter.ConnectLinks(motionLinks[motionIndex],linkTo);

			MotionStructs::MotionEntryHeader motionEntryHeader;
			memset(&motionEntryHeader,0,sizeof(motionEntryHeader));

			// set up link (only if there are bones used)
			LINK_FROM_HANDLE linkFromMotionEntry;
			if (analyzedData.m_totalBonesUsed)
				linkFromMotionEntry = dbWriter.RegisterLinkFrom(motionEntryBlock,GET_OFFSET(motionEntryHeader,motionEntryHeader.m_pChannel),"m_pChannel");

			motionEntryHeader.m_numChannels = analyzedData.m_totalBonesUsed;
			motionEntryHeader.m_pChannel = NULL;
			motionEntryHeader.m_boneGroupBitMask = analyzedData.m_boneGroupsUsed;
			motionEntryHeader.m_totalFrames = analyzedData.m_totalFrameTime;

			char fname[_MAX_FNAME];
			_splitpath( motionEntry.m_fileName, NULL, NULL, fname, NULL );
			//write out the LOW-ORDER bytes of the file name
			int nameLength = strlen(fname);
			if (nameLength>(sizeof(motionEntryHeader.m_name)-1))
			{
				strcpy(motionEntryHeader.m_name, &fname[nameLength-(sizeof(motionEntryHeader.m_name)-1)]); // use the right part of the name
				printf("Name %s truncated to %s\n",fname,motionEntryHeader.m_name);
			}
			else
			{
				strcpy(motionEntryHeader.m_name, fname);
			}

			dbWriter.WriteData(&motionEntryHeader,sizeof(motionEntryHeader));

			int numUserEntries = motionEntry.m_userDataEntries.GetSize();
			for (int userEntryIndex=0;userEntryIndex<numUserEntries;userEntryIndex++)
			{
				MotionEntry::UserDataEntry &userDataEntry = motionEntry.m_userDataEntries[userEntryIndex];
				dbWriter.WriteData((const char *)userDataEntry.m_keyName,userDataEntry.m_keyName.GetLength()+1);
				dbWriter.WriteData((const char *)userDataEntry.m_valueName,userDataEntry.m_valueName.GetLength()+1);
			}
			TupString endString;
			dbWriter.WriteData((const char *)endString,endString.GetLength()+1);
			dbWriter.AlignFile();

			dbWriter.EndBlock();
			motionSize += dbWriter.GetBlockSize(motionEntryBlock);

			m_totalMotionsWritten++;

			int numChannels = analyzedData.m_channels.GetSize();

			bool bFirstChannel = true;
			// keep track of links from channels to first channel item
			TupArray<LINK_FROM_HANDLE> channelToChannelItemLinks;
			channelToChannelItemLinks.SetSize(numChannels);

			// save out channel data
			for (int channelIndex=0;channelIndex<numChannels;channelIndex++)
			{
//				KFM::Channel &srcChannel = kfm.m_channels[channelIndex];
				AnalyzedData::Channel &dstChannel = analyzedData.m_channels[channelIndex];

				// is this bone going to be used?
				if (dstChannel.m_flags & AnalyzedData::Channel::FLAGS_ACTIVE_CHANNEL)
				{
					int oldChannelInfoSize = uniqueChannelNames.GetSize();
					int channelInfoIndex = uniqueChannelNames.Add(dstChannel.m_name);
					if (channelInfoIndex == oldChannelInfoSize)
					{
						ChannelInfo newChannelInfo;
						newChannelInfo.m_name = dstChannel.m_name;
						channelInfoTable.Add(newChannelInfo);
					}
					ChannelInfo &channelInfo = channelInfoTable[channelInfoIndex];
					channelInfo.m_numMotions++;

					BLOCK_HANDLE channelBlock = dbWriter.StartBlock("Motion %s Channel %s",(const char *)motionEntry.m_motionIDName,(const char *)dstChannel.m_name);

					MotionStructs::Channel channel;
					memset(&channel,0,sizeof(channel));

					if (bFirstChannel)
					{
						// link me to the motion entry
						LINK_TO_HANDLE linkToChannel = dbWriter.RegisterLinkTo(channelBlock,0,"First Channel");
						dbWriter.ConnectLinks(linkFromMotionEntry,linkToChannel);
						bFirstChannel = false;
					}
					channel.m_channelItemBitArray = dstChannel.m_activeChannelItems;

					// set up from link to channel items
					channelToChannelItemLinks[channelIndex] = dbWriter.RegisterLinkFrom(channelBlock,GET_OFFSET(channel,channel.m_pChannelItem),"Channel Item Ptr");
					// link to bone entry
					LINK_FROM_HANDLE channelToBoneEntryLink = dbWriter.RegisterLinkFrom(channelBlock,GET_OFFSET(channel,channel.m_pBoneEntry),"Bone Entry Ptr");
					dbWriter.ConnectLinks(channelToBoneEntryLink,boneEntryLinks[dstChannel.m_boneManagerIndex]);

					dbWriter.WriteData(&channel,sizeof(channel));

					totalMotionChannels++;
					m_totalChannelsWritten++;

					dbWriter.EndBlock();
					u32 blockSize = dbWriter.GetBlockSize(channelBlock);
					motionSize += blockSize;
					channelInfo.m_totalOutputSize += blockSize;
				}
			}

			// save out channel item data
			for (int channelIndex=0;channelIndex<numChannels;channelIndex++)
			{
//				KFM::Channel &srcChannel = kfm.m_channels[channelIndex];
				AnalyzedData::Channel &dstChannel = analyzedData.m_channels[channelIndex];

				// is this bone going to be used?
				if (dstChannel.m_flags & AnalyzedData::Channel::FLAGS_ACTIVE_CHANNEL)
				{
					int channelInfoIndex = uniqueChannelNames.GetIndex(dstChannel.m_name);
					ChannelInfo &channelInfo = channelInfoTable[channelInfoIndex];

					int channelItemIndex;

					TupArray<LINK_FROM_HANDLE> channelItemLinks;
					channelItemLinks.SetSize(KFM::ChannelItem::NUMCHANNELITEMS);

					bool bFirstChannelItem = true;

					// output the channel items
					for (channelItemIndex=0;channelItemIndex<KFM::ChannelItem::NUMCHANNELITEMS;channelItemIndex++)
					{
						if (dstChannel.m_activeChannelItems&(1<<channelItemIndex)) // is it needed?
						{
							channelInfo.m_numChannelItems++;

							BLOCK_HANDLE channelItemBlock = dbWriter.StartBlock("Motion %s Channel %s Item %s",(const char *)motionEntry.m_motionIDName,(const char *)dstChannel.m_name,pChannelItemNames[channelItemIndex]);

							if (bFirstChannelItem)
							{
								// link me to the motion entry
								LINK_TO_HANDLE linkToChannelItem = dbWriter.RegisterLinkTo(channelItemBlock,0,"First ChannelItem");
								dbWriter.ConnectLinks(channelToChannelItemLinks[channelIndex],linkToChannelItem);
								bFirstChannelItem = false;
							}

//							KFM::ChannelItem &srcChannelItem = srcChannel.m_channelItems[channelItemIndex];
							AnalyzedData::ChannelItem &dstChannelItem = dstChannel.m_channelItems[channelItemIndex];

							MotionStructs::ChannelItem channelItem;
							memset(&channelItem,0,sizeof(channelItem));

							// if this channel item has keys (not nondefault flat)
							if ((dstChannelItem.m_flags & AnalyzedData::ChannelItem::FLAGS_FLAT_NON_DEFAULT_KEYS)==0)
							{
								channelItem.m_numKeyFrames = dstChannelItem.m_keyframes.GetSize();
								// set up link from to keys
								channelItemLinks[channelItemIndex] = dbWriter.RegisterLinkFrom(channelItemBlock,GET_OFFSET(channelItem,channelItem.m_pKeyFrames),"Keyframe Ptr");
								channelItem.m_scaleValue = (float)dstChannelItem.m_scaleValue;
							}
							else
							{
								// numKeyFrames is zero
								// no link to keys
								// set flatnondefault value to value of first key
								channelItem.m_flatNonDefaultValue = (float)dstChannelItem.m_keyframes[0].m_doubleValue;
							}

							dbWriter.WriteData(&channelItem,sizeof(channelItem));

							totalMotionChannelItems++;
							m_totalChannelItemsWritten++;

							dbWriter.EndBlock();
							u32 blockSize = dbWriter.GetBlockSize(channelItemBlock);
							motionSize += blockSize;
							channelInfo.m_totalOutputSize += blockSize;
						}
					}

					// output the keyframes
					for (channelItemIndex=0;channelItemIndex<KFM::ChannelItem::NUMCHANNELITEMS;channelItemIndex++)
					{
//						KFM::ChannelItem &srcChannelItem = srcChannel.m_channelItems[channelItemIndex];
						AnalyzedData::ChannelItem &dstChannelItem = dstChannel.m_channelItems[channelItemIndex];

						if (dstChannel.m_activeChannelItems&(1<<channelItemIndex) &&
							(dstChannelItem.m_flags & AnalyzedData::ChannelItem::FLAGS_FLAT_NON_DEFAULT_KEYS)==0)
						{
							BLOCK_HANDLE keyFrameBlock = dbWriter.StartBlock("Motion %s Channel %s Item %s Keyframes",(const char *)motionEntry.m_motionIDName,(const char *)dstChannel.m_name,pChannelItemNames[channelItemIndex]);

							// link me to the motion entry
							LINK_TO_HANDLE linkToKeyframe = dbWriter.RegisterLinkTo(keyFrameBlock,0,"First Keyframe");
							dbWriter.ConnectLinks(channelItemLinks[channelItemIndex],linkToKeyframe);

							int numKeyFrames = dstChannelItem.m_keyframes.GetSize();

							int keyFrameIndex;
							// deal with the time values for each keyframe
							for (keyFrameIndex=0;keyFrameIndex<numKeyFrames;keyFrameIndex++)
							{
								AnalyzedData::Keyframe &dstKeyFrame = dstChannelItem.m_keyframes[keyFrameIndex];

								MotionStructs::KeyFrame keyFrame;
								keyFrame.m_time = dstKeyFrame.m_shortTime;
								keyFrame.m_value = dstKeyFrame.m_shortValue;
								dbWriter.WriteData(&keyFrame,sizeof(keyFrame));

								totalMotionChannelItemKeys++;
								m_totalKeyframesWritten++;
								m_totalKeyframesSize+= sizeof(keyFrame);
								channelInfo.m_numKeyframes++;
							}

							dbWriter.EndBlock();
							u32 blockSize = dbWriter.GetBlockSize(keyFrameBlock);
							motionSize += blockSize;
							channelInfo.m_totalOutputSize += blockSize;
						}
					}
				}
			}
			if (totalMotionChannelItemKeys==0)
			{
				printf("Warning Motion %s has flat data - try using a lower tolerance\n",(const char *)motionEntry.m_motionIDName);
			}
			MotionInfo motionInfo;
			motionInfo.m_numChannels = totalMotionChannels;
			motionInfo.m_numChannelItems = totalMotionChannelItems;
			motionInfo.m_numKeyframes = totalMotionChannelItemKeys;
			motionInfo.m_totalOutputSize = motionSize;
			motionInfo.m_name = motionEntry.m_motionIDName;
			motionInfo.m_filename = motionEntry.m_fileName;

			// keep track of unique filenames
			FilePath::GetFileName(motionEntry.m_fileName,fname);
			int oldCount = uniqueFilenames.GetSize();
			motionInfo.m_uniqueFilenameIndex = uniqueFilenames.Add(TupString(fname));
			// unique name
			if (oldCount == motionInfo.m_uniqueFilenameIndex)
			{
				int startCount = 0;
				numUsingUniqueFilenames.Add(startCount);
			}
			else
			{
				bSomeDuplicateNames = true;
			}
			numUsingUniqueFilenames[motionInfo.m_uniqueFilenameIndex]++;

			motionInfoTable.Add(motionInfo);
		}
		dbWriter.FinishChunk();

		u32 nameChunkSize=0;
		if (m_commandLine.m_bWriteNamesAsChunk)
		{
			dbWriter.StartChunk( DB_MOTION_NAMES, DB_MOTION_NAMES_VERSION,0, "1000" );
			dbWriter.WriteData(&numMotions,sizeof(numMotions));

			BLOCK_HANDLE ptrBlockHandle = dbWriter.StartBlock("Motion Name Pointers");
			for (motionIndex=0;motionIndex<numMotions;motionIndex++)
			{
				dbWriter.WriteData(&pad,sizeof(pad));
				motionLinks[motionIndex] = dbWriter.RegisterLinkFrom(ptrBlockHandle,motionIndex*sizeof(u32),"Motion Name Pointer %d",motionIndex);
			}
			dbWriter.EndBlock();

			for (motionIndex=0;motionIndex<numMotions;motionIndex++)
			{
				MotionEntry &motionEntry = m_motionEntries[motionIndex];
				BLOCK_HANDLE stringHandle = dbWriter.StartBlock("Motion Name String %s",(const char *)motionEntry.m_motionIDName);
				dbWriter.WriteData((const char *)motionEntry.m_motionIDName,motionEntry.m_motionIDName.GetLength()+1);
				LINK_TO_HANDLE linkToHandle = dbWriter.RegisterLinkTo(stringHandle,0,"Motion Name String %d",motionIndex);
				dbWriter.ConnectLinks(motionLinks[motionIndex],linkToHandle);
				dbWriter.EndBlock();
			}
			nameChunkSize = dbWriter.FinishChunk();
		}

		u32 totalFileSize = dbWriter.FinishDBLFile();

		printf ("\nMotion Information:\n");
		printf ("                                            #       # Channel      #      Output\n");
		printf ("Name                                     Channels     Items    Keyframes   Size      Filename\n");
		for (int motionInfoIndex = 0; motionInfoIndex < motionInfoTable.GetSize(); motionInfoIndex++)
		{
			MotionInfo &motionInfo = motionInfoTable[motionInfoIndex];
			printf("%-40s    %2d         %3d       %5d    %6d   %s\n",
				(const char *)motionInfo.m_name,
				motionInfo.m_numChannels, motionInfo.m_numChannelItems, motionInfo.m_numKeyframes, motionInfo.m_totalOutputSize, 
				(const char *)motionInfo.m_filename);
		}

		TupSort<ChannelInfo>::SortAscending(channelInfoTable.GetData(),channelInfoTable.GetSize());

		printf ("\nChannel Information:\n");
		printf ("                                             #     # Channel       #      Output\n");
		printf ("Name                                      Motions    Items     Keyframes   Size\n");
		for (int channelInfoIndex = 0; channelInfoIndex < channelInfoTable.GetSize(); channelInfoIndex++)
		{
			ChannelInfo &channelInfo = channelInfoTable[channelInfoIndex];
			printf("%-40s    %3d       %4d       %5d    %6d\n",
				(const char *)channelInfo.m_name,
				channelInfo.m_numMotions,channelInfo.m_numChannelItems, channelInfo.m_numKeyframes, channelInfo.m_totalOutputSize);
		}


		if (bSomeDuplicateNames)
		{
			printf ("\nDuplicate Filename Information:\n");
			for(int uniqueIndex = 0; uniqueIndex < uniqueFilenames.GetSize(); uniqueIndex++)
			{
				if (numUsingUniqueFilenames[uniqueIndex] > 1)
				{
					printf ("\nFilename %s used in the following:\n",(const char *)uniqueFilenames[uniqueIndex]);
					for (int motionIndex = 0; motionIndex < motionInfoTable.GetSize(); motionIndex++)
					{
						MotionInfo &motionInfo = motionInfoTable[motionIndex];
						if (motionInfo.m_uniqueFilenameIndex == uniqueIndex)
						{
							printf ("   %-40s   %s\n",motionInfo.m_name,motionInfo.m_filename);
						}
					}
				}
			}
		}

		printf("\nTotal bytes written = %d\n",totalFileSize);
		printf("Total motions written: %d size %d bytes %2.2f%% of total\n",
				m_totalMotionsWritten,
				m_totalMotionsWritten*sizeof(MotionStructs::MotionEntryHeader),
				(double)(m_totalMotionsWritten*sizeof(MotionStructs::MotionEntryHeader)*100.0)/totalFileSize);
		printf("Total bone entries written: %d size %d bytes %2.2f%% of total\n",
				m_totalBoneEntriesWritten,
				m_totalBoneEntriesWritten*sizeof(MotionStructs::BoneEntry),
				(double)(m_totalBoneEntriesWritten*sizeof(MotionStructs::BoneEntry)*100.0)/totalFileSize);
		printf("Total bone entries with duplicate bone names = %d\n",
			m_boneManager.GetNumBones()-m_boneManager.GetNumUniqueBoneNames());
		printf("Total channels written: %d size %d bytes %2.2f%% of total\n",
			m_totalChannelsWritten,
			m_totalChannelsWritten*sizeof(MotionStructs::Channel),
			(double)(m_totalChannelsWritten*sizeof(MotionStructs::Channel)*100.0)/totalFileSize);
		printf("Total channelitems written: %d size %d bytes %2.2f%% of total\n",
			m_totalChannelItemsWritten,
			m_totalChannelItemsWritten*sizeof(MotionStructs::ChannelItem),
			(double)(m_totalChannelItemsWritten*sizeof(MotionStructs::ChannelItem)*100.0)/totalFileSize);
		printf("Total keyframes written: %d size %d bytes %2.2f%% of total\n",
			m_totalKeyframesWritten,
			m_totalKeyframesSize,
			(double)(m_totalKeyframesSize*100.0)/totalFileSize);
		printf("Name Chunk Size: %d bytes %2.2f%% of total\n",
			nameChunkSize,
			(double)(nameChunkSize*100.0)/totalFileSize);

	}
	catch (Exception e)
	{
		throw (Exception(ERROR_FATAL,"WriteDatabaseFile\n  %s",(const char *)(e)));
	}
	catch (...)
	{
		throw (Exception(ERROR_FATAL,"WriteDatabaseFile\n  Unhandled Error"));
	}
}


// FitCurve fit a Bezier curve(s) to a set of digitized points 
void MotionManager::FitCurve(const TupArray<HashVector2> &points, TupArray<HashVector2> &linearPointsFill, double error)
{
	linearPointsFill.Add(points[0]); // add leftmost point
	int totalPoints = points.GetSize();
	FitLinear(points, 0, totalPoints-1, linearPointsFill, error);
}

// FitCubic : Fit a Bezier curve to a (sub)set of digitized points
void MotionManager::FitLinear(const TupArray<HashVector2> &points, int first, int last, TupArray<HashVector2> &linearPointsFill, double error)
{
	double maxError;		// Maximum fitting error
	int splitPoint;			// Point to split point set at
	int totalPoints;		// Number of points in subset
	HashVector2 tHatCenter;	// Unit tangent vector at splitPoint

	totalPoints = last - first + 1;

	// Use heuristic if region only has two points in it
	if (totalPoints == 2)
	{
		linearPointsFill.Add(points[last]);
		return;
	}

	// Find max deviation of points to fitted curve
	maxError = ComputeMaxError(points, first, last, splitPoint);
	if (maxError <= error)
	{
		linearPointsFill.Add(points[last]);
		return;
	}

	// Fitting failed -- split at max error point and fit recursively
	FitLinear(points, first, splitPoint, linearPointsFill, error);
	FitLinear(points, splitPoint, last, linearPointsFill, error);
}

// ComputeMaxError :
// Find the maximum squared distance of digitized points
// to fitted curve.
double MotionManager::ComputeMaxError(const TupArray<HashVector2> &points, int first, int last,int &splitPoint)
{
	int	i;
	double dist;
	double maxDist;		// Maximum error
	HashVector2 point;	// Point on curve
	HashVector2 vector;	// Vector from point to curve

	splitPoint = (last - first + 1)/2;
	maxDist = 0.0;
	for (i = first + 1; i < last; i++)
	{
		double percent = (points[i].x-points[first].x)/(points[last].x-points[first].x);
		point = points[first] + ((points[last]-points[first]) * percent);
		vector = point-points[i];
		dist = vector.y*vector.y;
		if (dist > maxDist)
		{
			maxDist = dist;
			splitPoint = i;
		}
	}
	return ::sqrt(maxDist);
}
