//
//
//  File:   Avalanche.ddl
//  Date:   November 13, 2003
//  By:     Bryant Collard
//  Desc:   The general DDL definitions. This file is to be included in the
//          game specific DDL file.
//
//  Copyright (c) 2003, Avalanche Software Inc.
//  All Rights Reserved.
//
//

// Voice Chat

#include <Extensions/VoiceChat.ddl>

// Shared defines.

#include <Network/NetZ/AvalancheDefines.h>

// Generally useful datasets

dataset DSBuffer
{
	buffer d_buffer;
} upon_request_filter;

dataset DSVector
{
	float d_x;
	float d_y;
	float d_z;
} upon_request_filter;

// Talker

dataset DSTalkerProps
{
	float d_pitchWarp;
	float d_pitchOffset;
	float d_spectrumWarp;
	float d_spectrumOffset;
	float d_spectrumLedge;
	float d_robot;
	float d_whisper;
	int16 d_environ;
	bool  d_headset;
	bool  d_voiceToSpeakers;
	int64 d_id;
	int32 d_userFlags;
} upon_request_filter;

doclass DOTalker
{
	DSTalkerProps d_props;
};

// Message Owner

doclass DOMessageOwner
{
	void SendFlag(uint32 i_hash, dohandle i_sender);
	void PostFlag(uint32 i_hash, dohandle i_sender);
	void SendBuffer(uint32 i_hash, buffer i_data, dohandle i_sender);
	void PostBuffer(uint32 i_hash, buffer i_data, dohandle i_sender);
};

// Host

dataset DSHostState
{
	bool d_acceptImmigration;
#ifdef LATENCY_COMPENSATION
	float d_hostTimePerSessionTime;
	float d_hostVsSessionIntercept;
#endif
} upon_request_filter;

dataset DSSessionLock
{
	byte d_ids;
	dohandle d_grantee;
} upon_request_filter;

doclass DOHost : DOMessageOwner
{
	DSHostState d_state;
	DSSessionLock d_lock;
};

// 6 degree of freedom dead reckoned state

dsproperty DRSixDOF : deadreckoning;

dataset DSSixDOF
{
#ifdef LATENCY_COMPENSATION
	float d_time;
#endif
	byte d_flags;
	int16 d_posX;
	int16 d_posY;
	int16 d_posZ;
	int16 d_velX;
	int16 d_velY;
	int16 d_velZ;
	int16 d_orientX;
	int16 d_orientY;
	int16 d_orientZ;
	int16 d_orientW;
	int16 d_omegaX;
	int16 d_omegaY;
	int16 d_omegaZ;
} unreliable, DRSixDOF;

// Ground follower dead reckoned state

dsproperty DRGroundFollower : deadreckoning;

dataset DSGroundFollower
{
#ifdef LATENCY_COMPENSATION
	float d_time;
#endif
	int16 d_posX;
	int16 d_posY;
	int16 d_posZ;
	int16 d_velX;
	int16 d_velY;
	int16 d_velZ;
	int16 d_heading;
	int16 d_headingRate;
	byte d_flags;
} unreliable, DRGroundFollower;

// Path follower dead reckoned state

dsproperty DRPathFollower : deadreckoning;

dataset DSPathFollower
{
#ifdef LATENCY_COMPENSATION
	float d_time;
#endif
	byte d_flags;
	int8 d_state;
	int16 d_speedTarget;
	int16 d_speedCurrent;
	int16 d_distance;
} unreliable, DRPathFollower;

// Actor

dataset DSActorProps
{
	string d_parm;
} constant;

dataset DSActorMigrate
{
	bool d_persistent;
} upon_request_filter;

dataset DSActorInteraction
{
	uint32 d_involvement;
	uint32 d_category;
	uint32 d_positiveInclude;
	uint32 d_negativeInclude;
	uint32 d_positiveExclude;
	uint32 d_negativeExclude;
} upon_request_filter;

doclass DOActor : DOMessageOwner
{
	DSActorProps d_props;
	DSActorMigrate d_migrate;
	DSActorInteraction d_interaction;
	DSBuffer d_masterMetaData;
	DSBuffer d_masterData;
	DSBuffer d_configData;
};

// Generic

doclass DOGeneric : DOActor
{
	DSSixDOF d_state;
	DSVector d_acceleration;
};

// Path Follower

doclass DOPathFollower : DOActor
{
	DSPathFollower d_state;
};

// World

dataset DSStartTime
{
	uint64 d_time;
} upon_request_filter;

dataset DSWorldExtents
{
	bool d_set;
	float d_centerX;
	float d_centerY;
	float d_centerZ;
	float d_extentX;
	float d_extentY;
	float d_extentZ;
} upon_request_filter;

doclass DOWorld : DOMessageOwner
{
	DSStartTime d_start;
	DSWorldExtents d_extents;
};

wellknown DOWorld g_doWorld;
