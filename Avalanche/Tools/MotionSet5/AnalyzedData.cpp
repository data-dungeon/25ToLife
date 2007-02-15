#include "Stdafx.h"
#include "AnalyzedData.h"

// Keyframe methods
AnalyzedData::Keyframe::Keyframe(void)
{
	m_shortTime = 0;
	m_shortValue = 0;
}


// ChannelItem methods
AnalyzedData::ChannelItem::ChannelItem(void) :
	m_flags(0)
{
}

// Channel methods
AnalyzedData::Channel::Channel(void) :
	m_activeChannelItems(0),
	m_boneGroupId(0),
	m_flags(0)
{
}

AnalyzedData::AnalyzedData(void) :
	m_totalFrameTime(0),
	m_numActiveChannels(0),
	m_boneGroupsUsed(0),
	m_totalBonesUsed(0)
{
}