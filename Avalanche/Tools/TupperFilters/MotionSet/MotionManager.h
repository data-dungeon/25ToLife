#pragma once

#define MOTION_MANAGER_H

#ifndef KFM_H
#include "KFM.h"
#endif

#ifndef ANALYZED_DATA_H
#include "AnalyzedData.h"
#endif

class Options;
class BoneGroupTable;

class BoneManager
{
public:
	struct Matrix3x3
	{
		float e00, e01, e02;
		float e10, e11, e12;
		float e20, e21, e22;

		bool operator==(const Matrix3x3 &s1) const;
	};

	class BoneEntry
	{
	public:
		TupString m_motionName;
		Matrix3x3 m_bonetoparent;
		TupString m_name;
		unsigned short m_groupNumber;
	};

private:
	Keeper<BoneEntry> m_boneEntries;
	Keeper<TupString> m_uniqueBoneNames;
public:

	int AddBone(const BoneEntry &boneEntry);
	const BoneEntry &GetBone(int index) const;
	int GetNumBones(void) const;

	int GetNumUniqueBoneNames(void) const;

};

class MotionManager
{

	class MotionEntry
	{
	public:
		class UserDataEntry
		{
		public:
			TupString m_keyName;
			TupString m_valueName;
		};

		enum flags
		{
			FLAGS_KFM_DATA										= 0x0001,
			FLAGS_MOTION_ID_NAME								= 0x0002,
			FLAGS_MOTION_FILE_NAME							= 0x0004,
			FLAGS_BONE_GROUP_EXCLUDES						= 0x0008,
			FLAGS_BONE_GROUP_FILTERS						= 0x0010,
			FLAGS_TRANS_ERROR_TOLERANCE					= 0x0020,
			FLAGS_TRANS_ERROR_TOLERANCE_MULTIPLIER		= 0x0040,
			FLAGS_ROT_ERROR_TOLERANCE						= 0x0080,
			FLAGS_ROT_ERROR_TOLERANCE_MULTIPLIER		= 0x0100,
			FLAGS_SCALE_ERROR_TOLERANCE					= 0x0200,
			FLAGS_SCALE_ERROR_TOLERANCE_MULTIPLIER		= 0x0400,
			FLAGS_ERROR_TOLERANCE_ALL						= FLAGS_TRANS_ERROR_TOLERANCE | FLAGS_ROT_ERROR_TOLERANCE |	FLAGS_SCALE_ERROR_TOLERANCE,
			FLAGS_ERROR_TOLERANCE_MULTIPLIER_ALL		= FLAGS_TRANS_ERROR_TOLERANCE_MULTIPLIER | FLAGS_ROT_ERROR_TOLERANCE_MULTIPLIER | FLAGS_SCALE_ERROR_TOLERANCE_MULTIPLIER,
		};

		KFM m_KFMData;
		AnalyzedData m_analyzedData;

		TupString m_motionIDName;
		TupArray<TupString> m_additionalMotionIDNames;
		TupString m_fileName;
		unsigned long m_boneGroupBitMask;
		double m_transErrorTolerance;
		double m_transErrorToleranceMultiplier;
		double m_rotErrorTolerance;
		double m_rotErrorToleranceMultiplier;
		double m_scaleErrorTolerance;
		double m_scaleErrorToleranceMultiplier;
		int m_flags;
		TupArray<UserDataEntry> m_userDataEntries;

		MotionEntry(void);
		void Clear(void);
      bool ReadMotion();
	};

	Keeper<TupString> m_uniqueMotionIDs;
	bool m_bFoundDuplicateMotionIDNames;

	TupArray<MotionEntry> m_motionEntries;
	BoneManager m_boneManager;

	BoneGroupTable *m_pBoneGroupTable;
	int m_numErrors;

	int m_totalSourceChannels;
	int m_totalSourceChannelItems;
	int m_totalSourceKeys;

	int m_totalIgnoredChannels;
	int m_totalDefaultChannels;
	int m_totalChannels;
	int m_totalChannelItems;
	int m_totalLinearKeys;

	int m_totalMotionsWritten;
	int m_totalBoneEntriesWritten;
	int m_totalChannelsWritten;
	int m_totalChannelItemsWritten;
	int m_totalKeyframesWritten;
	int m_totalDefaultChannelItems;
	int m_totalFlatNonDefaultChannelItems;
	int m_totalKeyframesSize;

	Options &m_options;
	FilterDataManager &m_dataManager;
public:
	MotionManager(Options &options, FilterDataManager &dataManager);
	void ReadInputData(void);
	void ReadMotionAndAnalyze(void);
	void ReadBoneGroupTableFile(void);
	void AnalyzeMotion(MotionEntry &motionEntry);
	void PrintAnalyzeStats(void);
	void WriteDatabaseFile(void);
	void WriteHeaderFile(void);
	void WriteMSetFile(void);


private:
	bool ParseTextFile(void);
	bool ParseMSetData(TupperwareAggregate *pParentAgg);

	void PrintInfo( const char *pcFmt, ...);
	void PrintError( const char *pcFmt, ...);

	void BuildChannelItemKeys(const MotionEntry &motionEntry,const KFM::ChannelItem &srcChannelItem,AnalyzedData::ChannelItem &dstChannelItem,int channelItemIndex,double errorTolerance);
	unsigned short GetActiveChannelItems(const AnalyzedData::Channel &channel);
	void BuildChannelItem(const MotionEntry &motionEntry,const KFM::Channel &srcChannel,AnalyzedData::ChannelItem &dstChannelItem,int channelItemIndex);

	void FitCurve(const TupArray<HashVector2> &points, TupArray<HashVector2> &linearPointsFill, double error);
	void FitLinear(const TupArray<HashVector2> &points, int first, int last, TupArray<HashVector2> &linearPointsFill, double error);
	double ComputeMaxError(const TupArray<HashVector2> &points, int first, int last,int &splitPoint);

};