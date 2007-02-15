#pragma once

#define ANALYZED_DATA_H

class AnalyzedData
{
public:

	class Keyframe
	{
	public:
		double m_doubleTime;
		unsigned short m_shortTime;
		short m_shortValue;
		double m_doubleValue;

		Keyframe(void);
	};

	class ChannelItem
	{
	public:
		enum ChannelIndex
		{
			XTRANS,YTRANS,ZTRANS,XSCALE,YSCALE,ZSCALE,XQUAT,YQUAT,ZQUAT,WQUAT,NUMCHANNELITEMS
		};

		enum Flags
		{
			FLAGS_ACTIVE_CHANNEL_ITEM				= 0x0001,
			FLAGS_FLAT_NON_DEFAULT_KEYS			= 0x0002,
		};

		unsigned long m_flags;

		TupArray<Keyframe> m_keyframes;
		double m_minValue;
		double m_maxValue;
		double m_scaleValue;
		double m_errorFactor;

		ChannelItem(void);
	};

	class Channel
	{
	public:
		enum Flags
		{
			FLAGS_ACTIVE_CHANNEL				= 0x0001,
		};

		TupString m_name;
		TupArray<ChannelItem> m_channelItems;
		unsigned short m_activeChannelItems;
		unsigned short m_boneGroupId;
		unsigned long m_flags;
		int m_boneManagerIndex;

		Channel(void);
	}; 

	TupArray<Channel> m_channels;
	unsigned short m_totalFrameTime;
	int m_numActiveChannels;
	unsigned long m_boneGroupsUsed;
	int m_totalBonesUsed;

	AnalyzedData(void);

};
