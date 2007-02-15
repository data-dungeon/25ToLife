#pragma	  once

#define MOTION_STRUCTS_H

#define MAX_BONENAME_LEN		16		//bone name used to correlate motion channels: bones names with motion MUST be unique in this many characters

#define MOTION_HEADER_NAME_LEN	16

struct MotionStructs
{
	struct KeyFrame
	{
		unsigned short	m_time;
		short				m_value;
	};

	struct ChannelItem
	{
		unsigned short	m_numKeyFrames;
		unsigned short	m_engineCachedIndex;
		KeyFrame *m_pKeyFrames;
		union
		{
			float	m_scaleValue; // when we have keys
			float m_flatNonDefaultValue; // if we don't have keys
		};
	};

	struct Matrix3x3
	{
		float e00, e01, e02;
		float e10, e11, e12;
		float e20, e21, e22;
	};

	struct BoneEntry
	{
		char m_name[MAX_BONENAME_LEN];
		unsigned short m_groupNo;
		unsigned short m_pad;
		Matrix3x3 m_bonetoparent;
	};

	struct Channel
	{
		unsigned short	m_channelItemBitArray;
		unsigned short m_pad;
		BoneEntry *m_pBoneEntry;
		ChannelItem	*m_pChannelItem;
	};

	struct MotionEntryHeader
	{
		char m_name[MOTION_HEADER_NAME_LEN];
		unsigned short m_totalFrames;
		unsigned short m_numChannels;
		Channel *m_pChannel;
		u32 m_boneGroupBitMask;
		// user data follows
	};

	struct MotionFileHeader
	{
		unsigned short m_numMotions;
		unsigned short m_numBoneEntries;
		MotionEntryHeader **m_pMotionEntryTable;
		BoneEntry *m_pBoneEntries;
	};

	// Motion Name Structs
	struct MotionNameEntry
	{
		char *m_pName;
	};

	struct MotionNameHeader
	{
		unsigned short m_numMotions;
		MotionNameEntry **m_pMotionNameEntryTable;
	};
#if 0
	enum ChannelIndex
	{
		XTRANS,YTRANS,ZTRANS,XSCALE,YSCALE,ZSCALE,XQUAT,YQUAT,ZQUAT,WQUAT,NUMCHANNELITEMS
	};
	void FixData(MotionFileHeader *pMotionData);
};

#define DEINIT_PTR( p ) \
{ \
	if ( *(int*)&(p) != 0 ) \
	{ \
		*(int*)&(p) = ( ((int)(p) - (int)&(p)) - 1 ); \
	} \
}

#define INIT_PTR( p ) \
{ \
	if ( *(int*)&(p) != 0 ) \
	{ \
		*(int*)&(p) = (((int)(p) + (int)&(p)) + 1); \
	} \
}

// the following code should fix up the data
inline void MotionStructs::FixData(MotionFileHeader *pMotionData)
{
	// Swap pMotionData here
	INIT_PTR(pMotionData->m_pMotionEntryTable);
	INIT_PTR(pMotionData->m_pBoneEntries);

	// fix bone entries
	for (int boneEntryIndex=0;boneEntryIndex<pMotionData->m_numMotions;boneEntryIndex++)
	{
		BoneEntry &boneEntry = pMotionData->m_pBoneEntries[boneEntryIndex];
		// Swap boneEntry here
	}

	for (int motionIndex=0;motionIndex<pMotionData->m_numMotions;motionIndex++)
	{
		// Swap pMotionData->m_pMotionEntryTable[motionIndex] here
		INIT_PTR(pMotionData->m_pMotionEntryTable[motionIndex]);
		MotionEntryHeader &motionEntryHeader = *pMotionData->m_pMotionEntryTable[motionIndex];
		INIT_PTR(motionEntryHeader.m_pChannel);
		for (int channelIndex=0;channelIndex<motionEntryHeader.m_numChannels;channelIndex++)
		{
			Channel &channel = motionEntryHeader.m_pChannel[channelIndex];
			// Swap channel here
			INIT_PTR(channel.m_pBoneEntry);
			INIT_PTR(channel.m_pChannelItem);

			int channelItemPackedIndex=0;
			for (int channelItemIndex=0;channelItemIndex<NUMCHANNELITEMS;channelItemIndex++)
			{
				if ((1<<channelItemIndex) & channel.m_channelItemBitArray)
				{
					ChannelItem &channelItem = channel.m_pChannelItem[channelItemPackedIndex];
					// Swap channelItem here
					INIT_PTR(channelItem.m_pKeyFrames); 
					channelItemPackedIndex++;
					for (int keyFrameIndex=0;keyFrameIndex<channelItem.m_numKeyFrames;keyFrameIndex++)
					{
						KeyFrame &keyframe = channelItem.m_pKeyFrames[keyFrameIndex];
						// Swap keyframe here
					}
				}
			}
		}
	}
}
#else
};
#endif