
#include  <io.h>
#include  <stdio.h>
#include "stdafx.h"
#include "..\..\..\amsdk\viewer\KFM.h"
#include "LoadKFM.h"
#include "GroupBoneTable.h"
#include "Game/Animation/motion.h"
#include "parsefcn.h"
#include "plugins.h"
#include "Game/Animation/ApplyKFM.h"
#include <Math/MathClass.h>

#define signof(f) ((f<0)?-1:1)
#define MAX_KEYFRAMES 65536	//max number of keyframes we can handle in any one channel (unsigned short)

extern ts_motnSet	glbMotnSet;

unsigned int BonesInGroups( KFMAction &kfmaction, BoneGroupBitMask boneGroupBitMask, BoneGroupBitMask *boneGroupBitMaskActual );
unsigned short ChannelItemBitArray( Channel &channel );
float fixtan(float tan);
void RemoveKeyframe(ts_MChannelItem *channelItem, unsigned short keyframe);

char *chanNames[Channel::NUMCHANNELITEMS] = { "XTRANS", "YTRANS", "ZTRANS", "XSCALE", "YSCALE", "ZSCALE", "XQUAT", "YQUAT", "ZQUAT", "WQUAT" };
float floatToAnimVal[Channel::NUMCHANNELITEMS] = {	TRANSLATION_FLOAT_TO_VAL,
																	TRANSLATION_FLOAT_TO_VAL,
																	TRANSLATION_FLOAT_TO_VAL,
																	SCALE_FLOAT_TO_VAL,
																	SCALE_FLOAT_TO_VAL,
																	SCALE_FLOAT_TO_VAL,
																	QUATERNION_FLOAT_TO_VAL,
																	QUATERNION_FLOAT_TO_VAL,
																	QUATERNION_FLOAT_TO_VAL,
																	QUATERNION_FLOAT_TO_VAL};
float animValToFloat[Channel::NUMCHANNELITEMS] = {	TRANSLATION_VAL_TO_FLOAT,
																	TRANSLATION_VAL_TO_FLOAT,
																	TRANSLATION_VAL_TO_FLOAT,
																	SCALE_VAL_TO_FLOAT,
																	SCALE_VAL_TO_FLOAT,
																	SCALE_VAL_TO_FLOAT,
																	QUATERNION_VAL_TO_FLOAT,
																	QUATERNION_VAL_TO_FLOAT,
																	QUATERNION_VAL_TO_FLOAT,
																	QUATERNION_VAL_TO_FLOAT};
float maxAnimValue[Channel::NUMCHANNELITEMS] = {	MAX_TRANSLATION_VALUE,
																	MAX_TRANSLATION_VALUE,
																	MAX_TRANSLATION_VALUE,
																	MAX_SCALE_VALUE,
																	MAX_SCALE_VALUE,
																	MAX_SCALE_VALUE,
																	MAX_QUATERNION_VALUE,
																	MAX_QUATERNION_VALUE,
																	MAX_QUATERNION_VALUE,
																	MAX_QUATERNION_VALUE};
ts_MChannelItem channelItem[Channel::NUMCHANNELITEMS];
ts_MKeyFrame keyFrame[Channel::NUMCHANNELITEMS][MAX_KEYFRAMES];

int grandtotalChannelBytes = 0;
int grandtotalChannelItemBytes = 0;
int grandtotalKeyframeBytes = 0;
int grandtotalKeyframeBytesRemoved = 0;
int grandtotalInputKeyframes = 0;
int grandtotalInputTransFrames = 0;
int grandtotal2FrameChannels = 0;

int firstBone = 0;

char *SMPTETime(int frames)
{
	static char cBuf[16];

	int secs = frames / 30;
	int mins = secs / 60;
	secs = secs % 60;
	frames = frames % 30;
	sprintf(cBuf, "%2.2d:%2.2d:%2.2d", mins, secs, frames);

	if((secs == 6) && (frames == 20))
		int temp=0;

	return cBuf;
}

void LoadAndWriteKFM( mpostream &str, char *fileName, ts_motnInf *pMotnInf )
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	int totalChannelBytes = 0;
	int totalChannelItemBytes = 0;
	int totalKeyframeBytes = 0;
	int totalKeyframeBytesRemoved = 0;
	int total2FrameChannels = 0;

	float errorTolerance = pMotnInf->motnErrorTolerance;
	float translationScaleFactor = pMotnInf->motnTranslationScaleFactor;

	//the model bone is bone zero.  If requested, then don't process it.
	extern bool bExcludeModelBone;
	if(bExcludeModelBone)
		firstBone = 1;

	printInfo( "Processing file %s", fileName);

	_splitpath( fileName, drive, dir, fname, ext ); //(we'll use the name part of the filename)
	
	if(_access(fileName, 4) != 0)	//4 says to check for read access
	{
		printError("Can't Open file %s\n", fileName);
//		exit(EXIT_FAILURE);
		return;
	}

	
	CFile file(fileName, CFile::modeRead);

	CArchive ar(&file, CArchive::load);
	if( !ar.IsLoading() )
		//something wrong with the file - what to do?
		printError("Can't read file %s\n", fileName);

	KFMAction kfmaction;
	kfmaction.Serialize(ar);//load .kfm file into memory - this also computes the channel's m_bonetoparent matrix for us

	//build and output our .mdb KFM format:
	//	MHdr
	// Channels * num_motion_bones
	// Channel_Item_Packets * num_motion_bones

	BoneGroupBitMask boneGroupBitMaskActual;

	//get the number of channels that are valid bones in the desired boneGroup filter (and get the actual bone groups thusly represented)
	unsigned int uiChannels = BonesInGroups(kfmaction, pMotnInf->boneGroupBitMask, &boneGroupBitMaskActual);

	printInfo( "Bone group masK reqested %x, using %x", pMotnInf->boneGroupBitMask, boneGroupBitMaskActual);


	//MHdr
	ts_MHdr mHdr;
	mHdr.usFrames = Math::Ceil(kfmaction.m_length.GetSeconds() * ANIMFRAMES_PER_SECOND);//round up to nearest integral frame
	mHdr.usBones = uiChannels;
	mHdr.boneGroupBitMask = boneGroupBitMaskActual;//note which bone groups actually made it through the filter
	//write out the LOW-ORDER bytes of the file name
	strncpy( (char *)&mHdr.name, (strlen(fname) > sizeof(mHdr.name)) ? (&fname[strlen(fname) - sizeof(mHdr.name)]) : fname, sizeof(mHdr.name)); 
	mHdr.type = MHDR_TYPE_KFM; //indicate that this is a KFM file
	mHdr.pMChannel = 0;

	str << mHdr;
	printInfo( "Header frames %d, channels %d, channellist %d", mHdr.usFrames, mHdr.usBones, kfmaction.m_channellist.GetSize());

	unsigned short usMChannelItemBitArray[1024];//enough for all channels we'll need for any one motion

	int i;
	uint numChannelsProcessed=0;
	uint totalInputKeyframes=0;
	uint totalInputTransFrames=0;

	//Channels
	for (i=firstBone; i < kfmaction.m_channellist.GetSize(); i++)
	{
		BoneGroupID boneGroupID = GetBoneGroupID(LPCTSTR(kfmaction.m_channellist[i].m_name));
		//is this action for a bone that has data and that's a member of a group?
		if (
				IsValidChannel(LPCTSTR(kfmaction.m_channellist[i].m_name))
			&&	((usMChannelItemBitArray[i] = ChannelItemBitArray(kfmaction.m_channellist[i])) != 0)
			&& ((1<<boneGroupID) & boneGroupBitMaskActual)
			)
		{
			//is this bone's group one of the requested ones?
				ts_MChannel channel;

				//fill in the channel struct and output
				channel.usMChannelItemBitArray = usMChannelItemBitArray[i];
				strncpy((char *)&channel.m_name, LPCTSTR(kfmaction.m_channellist[i].m_name), sizeof(channel.m_name));
				channel.m_usGroupNo = boneGroupID;
				channel.m_bonetoparent.e00 = kfmaction.m_channellist[i].m_bonetoparent.GetXAxis().x;
				channel.m_bonetoparent.e01 = kfmaction.m_channellist[i].m_bonetoparent.GetXAxis().y;
				channel.m_bonetoparent.e02 = kfmaction.m_channellist[i].m_bonetoparent.GetXAxis().z;
				channel.m_bonetoparent.e10 = kfmaction.m_channellist[i].m_bonetoparent.GetYAxis().x;
				channel.m_bonetoparent.e11 = kfmaction.m_channellist[i].m_bonetoparent.GetYAxis().y;
				channel.m_bonetoparent.e12 = kfmaction.m_channellist[i].m_bonetoparent.GetYAxis().z;
				channel.m_bonetoparent.e20 = kfmaction.m_channellist[i].m_bonetoparent.GetZAxis().x;
				channel.m_bonetoparent.e21 = kfmaction.m_channellist[i].m_bonetoparent.GetZAxis().y;
				channel.m_bonetoparent.e22 = kfmaction.m_channellist[i].m_bonetoparent.GetZAxis().z;
				channel.m_channelitem = NULL;
				str << channel;
				totalChannelBytes += sizeof(ts_MChannel);
				printInfo( "chanIndex %d channel %d %s group %d, total channel data size %d ", numChannelsProcessed++, i, LPCTSTR(kfmaction.m_channellist[i].m_name), boneGroupID, totalChannelBytes);
				printInfo( " B2P=%.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f ", 
					channel.m_bonetoparent.e00,
					channel.m_bonetoparent.e01,
					channel.m_bonetoparent.e02,
					channel.m_bonetoparent.e10,
					channel.m_bonetoparent.e11,
					channel.m_bonetoparent.e12,
					channel.m_bonetoparent.e20,
					channel.m_bonetoparent.e21,
					channel.m_bonetoparent.e22	);
		}
	}

	int j,k;

	//to normalize the quaternions to a range -1..+1: record the largest queternion and divide all quaterions by it
	float maxQuat = 1.0f;
	for (i=firstBone; i < kfmaction.m_channellist.GetSize(); i++)
	{
			char *name = (char *)LPCTSTR(kfmaction.m_channellist[i].m_name);
			for (int j=0; j < Channel::NUMCHANNELITEMS; j++)
				if (usMChannelItemBitArray[i] & (1<<j))
					for (k=0; k < kfmaction.m_channellist[i].m_channelitem[j].m_keyframes.GetSize(); k++)
					{
						if(j==Channel::XTRANS)
							totalInputTransFrames++;
						if(j==Channel::XQUAT)
							totalInputKeyframes++;
						if( j >= Channel::XTRANS && j <= Channel::ZTRANS )
							printInfo("%s TransChannel %d = %f", name, j, kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[k].m_value);
						if( j >= Channel::XSCALE && j <= Channel::ZSCALE )
							printInfo("%s ScaleChannel %d = %f", name, j, kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[k].m_value);

						if( j >= Channel::XQUAT && j <= Channel::WQUAT )
						{
							maxQuat = max(maxQuat, Math::Abs(kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[k].m_value));
						}
					}
	}

	if(maxQuat > 1.0f)
		printInfo("%s MaxQuat = %f - Normalizing Quaternions", fileName, maxQuat);

	// Channel_Item_Packets * num_motion_bones
	for (i=firstBone; i < kfmaction.m_channellist.GetSize(); i++)
	{
		BoneGroupID boneGroupID = GetBoneGroupID(LPCTSTR(kfmaction.m_channellist[i].m_name));
		//so I can see the bone name
		char *name = (char *)LPCTSTR(kfmaction.m_channellist[i].m_name);

		//process only the valid channels
		//is this action for a valid bone that's a member of a group in the group filter?
		if (
				(usMChannelItemBitArray[i] != 0)
			&& IsValidChannel(LPCTSTR(kfmaction.m_channellist[i].m_name))
			&& ((1<<boneGroupID) & boneGroupBitMaskActual)
			)
		{

			//first create the channelitems
			for (j=0; j < Channel::NUMCHANNELITEMS; j++)
				if (usMChannelItemBitArray[i] & (1<<j))
				{
					//output this channelitem that has data
					unsigned int numkeyframes = kfmaction.m_channellist[i].m_channelitem[j].m_keyframes.GetSize();
					if ( numkeyframes > USHRT_MAX)
						printError("file %s, bone %s has too many keyframes (%d)\n", fileName, name, numkeyframes);
					channelItem[j].m_numkeyframes = numkeyframes;
					channelItem[j].m_cachedindex = 0;
					channelItem[j].m_cachedtime = 0;
					channelItem[j].m_cachedvalue = kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[0].m_value;
					channelItem[j].m_keyframes = &keyFrame[j][0];
				}

/************************establish engine-format data*********************************************/
			//then create each channelitem's keyframe data
			for (j=0; j < Channel::NUMCHANNELITEMS; j++)
				if (usMChannelItemBitArray[i] & (1<<j))
				{
					int k;

					//output this channelitem's keyframe data
					for (k=0; k < kfmaction.m_channellist[i].m_channelitem[j].m_keyframes.GetSize(); k++)
					{
						float fConvert;

						if( j >= Channel::XTRANS && j <= Channel::ZTRANS )
							fConvert = HASH_TO_ENGINE_SCALE_FACTOR * translationScaleFactor;//values are distance: convert from Hash to Engine coordinates
						else if( j >= Channel::XQUAT && j <= Channel::WQUAT )
							fConvert = 1 / maxQuat;//values are quaternion rotations: normalize to -1..1
						else
							fConvert = 1.0f;//values are scale factors: leave alone

						//get the keyframe time and ensure it can be converteed to 16bits worth of jiffies 
						float seconds = kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[k].m_time.GetSeconds();
						if ( SECONDS_TO_JIFFIES(seconds) > MAX_JIFFIES )
							printError("file %s, channel %s is longer (%f seconds) than allowed (%f)\n", fileName, name, seconds, USHRT_MAX / JIFFIES_PER_SECOND);

						keyFrame[j][k].m_time = SECONDS_TO_JIFFIES(seconds);
						int frame = SECONDS_TO_ANIMFRAMES(seconds);

						//get the keyframe value - it appears to be normalized from -1.0 to +1.0 - ensure so
						float value = kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[k].m_value * fConvert;
						if (Math::Abs(value) > maxAnimValue[j])
							printError("file %s, bone %s channel %s at frame %s, has a larger value (%f) than allowed (%f)\n", fileName, name, chanNames[j], SMPTETime(frame), value, maxAnimValue[j]);

						keyFrame[j][k].m_value = FLOAT_TO_ANIMVALUE(value, floatToAnimVal[j]);//convert from float to short

						
						
#ifdef SPLINE_INTERPOLATION
						//get the spline in/out tangents...for quats these appear to be sin/cosines and thus are between -1.0 to +1.0.  For translations, these appear to be in centimeters
						//thus y-values need to be converted to engine space (* fConvert)

						
						float itanx = fixtan(kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[k].m_it.x);
						float itany = fixtan(kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[k].m_it.y * fConvert);

						//ensure that the tangents fit in our tangent fixed point range - normalize if not
						if(Math::Abs(itanx) > MAX_ANIMVECT_FLOAT)
						{
							printWarning("file %s, bone %s channel %s at frame %s, has a larger in-tangentX (%f) than allowed (%f), clamping\n", fileName, name, chanNames[j], SMPTETime(frame), itanx, MAX_ANIMVECT_FLOAT);
							itany = (itany / abs(itanx)) * MAX_ANIMVECT_FLOAT;
							itanx = MAX_ANIMVECT_FLOAT * signof(itanx);
						}

						if(Math::Abs(itany) > MAX_ANIMVECT_FLOAT)
						{
							printWarning("file %s, bone %s channel %s at frame %s, has a larger in-tangentY (%f) than allowed (%f), clamping\n", fileName, name, chanNames[j], SMPTETime(frame), itany, MAX_ANIMVECT_FLOAT);
							itanx = (itanx / abs(itany)) * MAX_ANIMVECT_FLOAT;
							itany = MAX_ANIMVECT_FLOAT * signof(itany);
						}
						
						keyFrame[j][k].m_it.x = FLOAT_TO_ANIMVECT(itanx);
						keyFrame[j][k].m_it.y = FLOAT_TO_ANIMVECT(itany);

						
						
						float otanx = fixtan(kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[k].m_ot.x);
						float otany = fixtan(kfmaction.m_channellist[i].m_channelitem[j].m_keyframes[k].m_ot.y * fConvert);

						//ensure that the tangents fit in our tangent fixed point range - normalize if not
						if(Math::Abs(otanx) > MAX_ANIMVECT_FLOAT)
						{
							printWarning("file %s, bone %s channel %s at frame %s, has a larger out-tangentX (%f) than allowed (%f), clamping\n", fileName, name, chanNames[j], SMPTETime(frame), otanx, MAX_ANIMVECT_FLOAT);
							otany = (otany / abs(otanx)) * MAX_ANIMVECT_FLOAT;
							otanx = MAX_ANIMVECT_FLOAT * signof(otanx);
						}

						if(Math::Abs(otany) > MAX_ANIMVECT_FLOAT)
						{
							printWarning("file %s, bone %s channel %s at frame %s, has a larger out-tangentY (%f) than allowed (%f), clamping\n", fileName, name, chanNames[j], SMPTETime(frame), otany, MAX_ANIMVECT_FLOAT);
							otanx = (otanx / abs(otany)) * MAX_ANIMVECT_FLOAT;
							otany = MAX_ANIMVECT_FLOAT * signof(otany);
						}

						keyFrame[j][k].m_ot.x = FLOAT_TO_ANIMVECT(otanx);
						keyFrame[j][k].m_ot.y = FLOAT_TO_ANIMVECT(otany);
#endif SPLINE_INTERPOLATION

					}
				}

/************************Compress engine-format data*********************************************/
			//Compress Data: remove any redundant keyframe data items:
			// Don't get rid of the first and last keyframe (so interpolation in and out and cycling anims work well).
			// For all other keyframes k: sample the spline from keyframe k-1 to k+1 and if the sample values without keyframe k
			// are close enough to samples with keyframe k then keyframe k is redundant: discard.
			if(!Math::Zero(errorTolerance))
			{
				for (j=0; j < Channel::NUMCHANNELITEMS; j++)
					if (usMChannelItemBitArray[i] & (1<<j))
					{
						int k;

//						printInfo("");
//						printInfo("Testing Bone %s, Channel %s, keyframes %d ", name, chanNames[j], channelItem[j].m_numkeyframes);

						//test all but first and last keyframe
						for (k = 1; k < (volatile)(channelItem[j].m_numkeyframes) - 1; k++)
						{
							//can keyframe k be removed (i.e. is it redundant?)

							bool removeKeyframe = true;//assume it can be removed

#if 1
							//try quick reject: is it exactly the same as previous frame? - if straight line ten reject without testing
							if( 0
#ifdef SPLINE_INTERPOLATION
								||	(channelItem[j].m_keyframes[k].m_it.x != 0)
								|| (channelItem[j].m_keyframes[k-1].m_it.x != 0)
								||	(channelItem[j].m_keyframes[k].m_it.y != 0)
								||	(channelItem[j].m_keyframes[k-1].m_it.y != 0)
								||	(channelItem[j].m_keyframes[k].m_ot.x != 0)
								||	(channelItem[j].m_keyframes[k-1].m_ot.x != 0)
								||	(channelItem[j].m_keyframes[k].m_ot.y != 0)
								||	(channelItem[j].m_keyframes[k-1].m_ot.y != 0)
#endif SPLINE_INTERPOLATION
								||	(Math::Abs(channelItem[j].m_keyframes[k].m_value - channelItem[j].m_keyframes[k-1].m_value) > 4)	//if values are close - up to 4 seems to work well
//								||	(channelItem[j].m_keyframes[k].m_value != channelItem[j].m_keyframes[k-1].m_value)
#ifdef SPLINE_INTERPOLATION
								|| (channelItem[j].m_keyframes[k+1].m_it.x != 0)
								||	(channelItem[j].m_keyframes[k+1].m_it.y != 0)
								||	(channelItem[j].m_keyframes[k+1].m_ot.x != 0)
								||	(channelItem[j].m_keyframes[k+1].m_ot.y != 0)
#endif SPLINE_INTERPOLATION
								||	(Math::Abs(channelItem[j].m_keyframes[k].m_value - channelItem[j].m_keyframes[k+1].m_value) > 4)	//if values are close - up to 4 seems to work well
//								||	(channelItem[j].m_keyframes[k].m_value != channelItem[j].m_keyframes[k+1].m_value)

								)
#endif
							{

								//this frame is different than last frame - test for redundancy: see if removing this frame doesn't alter the curve much

								//show progress indicator.  anyway to show to the console window even when output is redirected into a file?
//								printf("%d / %d : %d / %d : %d / %d          \r", i, kfmaction.m_channellist.GetSize(), j, Channel::NUMCHANNELITEMS, k, channelItem[j].m_numkeyframes);
								
//								printInfo("Testing Bone %s, Channel %s, keyframe %d, time = %d, time-1 %d, time+1 %d", name, chanNames[j], k, channelItem[j].m_keyframes[k].m_time, channelItem[j].m_keyframes[k-1].m_time, channelItem[j].m_keyframes[k+1].m_time);

								//test samples from time keyframe-1 to keyframe+1
								for (unsigned short time = channelItem[j].m_keyframes[k-1].m_time; time <= channelItem[j].m_keyframes[k+1].m_time; time += (unsigned short)(JIFFIES_PER_ANIMFRAME/2))
								{
									float val1; //value if we leave keyframe k in
									float val2; //value if we leave keyframe k out
									if(time < channelItem[j].m_keyframes[k].m_time)
										val1 = InterpolateValue( &channelItem[j], time, k-1, animValToFloat[j]);
									else
										val1 = InterpolateValue( &channelItem[j], time, k, animValToFloat[j]);
									val2 = InterpolateValue2( &channelItem[j], time, k-1, animValToFloat[j]);
									
//									printInfo("Bone %s, Channel %s, keyframe %d, time %d, val1 %f, val2 %f", name, chanNames[j], k, time, val1, val2);

									if(Math::Abs(val1 - val2) > errorTolerance)
									{
										removeKeyframe = false;
//										printInfo("Not Removing Channel %s, keyframe %d", chanNames[j], k);

										break;
									}
								}
							}


							if(removeKeyframe)
							{
//								printInfo("Removing Redundant Channel %s, keyframe %d", chanNames[j], k);
								RemoveKeyframe(&channelItem[j], k);
								k--;//adjust loop counter so that the now new k'th keyframe will be tested
								totalKeyframeBytesRemoved += sizeof(ts_MKeyFrame);
							}	
						}
					}
			}

				
/************************Output engine-format data*********************************************/
			//Now output the channelitems
			for (j=0; j < Channel::NUMCHANNELITEMS; j++)
				if (usMChannelItemBitArray[i] & (1<<j))
				{
					//output this channelitem that has data
					str << channelItem[j];
					totalChannelItemBytes += sizeof(ts_MChannelItem);
					printInfo( "%s gID=%d channelitem %d %s, total channel item data size %d", name, boneGroupID, j, chanNames[j], totalChannelItemBytes);
				}

			//then output each channelitem's keyframe data
			for (j=0; j < Channel::NUMCHANNELITEMS; j++)
				if (usMChannelItemBitArray[i] & (1<<j))
				{
					int k;

					//output this channelitem's keyframe data
					for (k=0; k < channelItem[j].m_numkeyframes; k++)
					{
						ts_MKeyFrameEngine keyFrameEngine;
						keyFrameEngine.m_time = keyFrame[j][k].m_time;
						keyFrameEngine.m_value = keyFrame[j][k].m_value;
//the engine keyframe structure currently doesn't have spline info
//						keyFrameEngine.m_it = keyFrame[j][k].m_it;
//						keyFrameEngine.m_ot = keyFrame[j][k].m_ot;
						str << keyFrameEngine;
						totalKeyframeBytes += sizeof(ts_MKeyFrameEngine);
						printInfo( "keyframedata %d %s, time=%d, val=%d, total keyframe data size = %d", k, chanNames[j], keyFrame[j][k].m_time, keyFrame[j][k].m_value, totalKeyframeBytes);
					}
					printInfo( "%s keyframedata %d %s, num keyframes %d, total keyframe data size %d keyframe bytes removed %d\n", name, j, chanNames[j], k, totalKeyframeBytes, totalKeyframeBytesRemoved);
					if(channelItem[j].m_numkeyframes == 2
						&& (keyFrame[j][0].m_value == keyFrame[j][1].m_value)
						)
						total2FrameChannels++;
				}
		}
	}

	printf( "Totals for %s: ChannelBytes %d, ChannelItemBytes %d, KeyFrameBytes %d, KeyframeBytesRemoved %d at Error Tolerance %f, InputKeyFrames = %d, InputTransFrames = %d\n", 
			fileName, totalChannelBytes, totalChannelItemBytes, totalKeyframeBytes, totalKeyframeBytesRemoved, errorTolerance, totalInputKeyframes, totalInputTransFrames);

	grandtotalChannelBytes += totalChannelBytes;   
	grandtotalChannelItemBytes += totalChannelItemBytes;
	grandtotalKeyframeBytes += totalKeyframeBytes;  
	grandtotalKeyframeBytesRemoved += totalKeyframeBytesRemoved;
	grandtotalInputKeyframes += totalInputKeyframes;
	grandtotalInputTransFrames += totalInputTransFrames;
	grandtotal2FrameChannels += total2FrameChannels;

}

//return the number of bones that this motion has that are in the requested bone groups
unsigned int BonesInGroups( KFMAction &kfmaction, BoneGroupBitMask boneGroupBitMaskFilter, BoneGroupBitMask *boneGroupBitMaskActual )
{
	unsigned int uiBonesInGroups=0;
	BoneGroupID boneGroupID;

	*boneGroupBitMaskActual = 0;//init the actual number of groups to empty

	for (int i=firstBone; i < kfmaction.m_channellist.GetSize(); i++)
	{
		//is this action for a bone that's a member of a group?
		boneGroupID = GetBoneGroupID(LPCTSTR(kfmaction.m_channellist[i].m_name));
		if (
				IsValidChannel(LPCTSTR(kfmaction.m_channellist[i].m_name))
			&& (ChannelItemBitArray(kfmaction.m_channellist[i]) != 0)
			)
		{
			_ASSERT(boneGroupID < sizeof(BoneGroupBitMask)*8);
			//is this bone's group one of the requested ones?
			if ((1<<boneGroupID) & boneGroupBitMaskFilter)
			{
				uiBonesInGroups++;
				*boneGroupBitMaskActual |= (1<<boneGroupID);//indicate what bone groups were actually found
			}
		}
	}

	return uiBonesInGroups;
}

unsigned short ChannelItemBitArray( Channel &channel )
{
	//usually, lots of the channelitems are not adjusted by artist, and can be defaulted to save space
	//determine which are NOT defaulted and set a bit in a bit array to indicate such

	unsigned short usChannelsPresent = 0;

	for( int i=Channel::XTRANS; i<=Channel::ZTRANS; i++)
	{
		for( int j = 0; j < channel.m_channelitem[i].m_keyframes.GetSize(); j++)
			if( !Math::Zero(channel.m_channelitem[i].m_keyframes[j].m_value, 0.01f) )//don't store identity of x, y or z translation == 0
			{
				float val = channel.m_channelitem[i].m_keyframes[j].m_value;
				usChannelsPresent |= 1<<i;
				printInfo("Translation Data found on bone %s = %f\n", channel.m_name, val);
				break;
			}
	}
	for( i=Channel::XSCALE; i<=Channel::ZSCALE; i++)
	{
		for( int j = 0; j < channel.m_channelitem[i].m_keyframes.GetSize(); j++)
			if( !Math::Equal(channel.m_channelitem[i].m_keyframes[j].m_value, 1, 0.01f) )//don't store identity of x,y or z scale == 1
			{
				float val = channel.m_channelitem[i].m_keyframes[j].m_value;
				usChannelsPresent |= 1<<i;
				printInfo("Scale Data found on bone %s = %f\n", channel.m_name, val);
				break;
			}
	}
	for( i=Channel::XQUAT; i<=Channel::ZQUAT; i++)
	{
		for( int j = 0; j < channel.m_channelitem[i].m_keyframes.GetSize(); j++)
			if( !Math::Zero(channel.m_channelitem[i].m_keyframes[j].m_value, 0.01f) )//don't store identity of x,y or z quat == 0
			{
				float val = channel.m_channelitem[i].m_keyframes[j].m_value;
				usChannelsPresent |= 1<<i;
				break;
			}
	}
	for( i=Channel::WQUAT; i<=Channel::WQUAT; i++)
	{
		for( int j = 0; j < channel.m_channelitem[i].m_keyframes.GetSize(); j++)
			if( !Math::Equal(channel.m_channelitem[i].m_keyframes[j].m_value, 1, 0.01f) )//don't store identity of w quat == 1
			{
				float val = channel.m_channelitem[i].m_keyframes[j].m_value;
				usChannelsPresent |= 1<<i;
				break;
			}
	}

	return usChannelsPresent;
}

void CleanUpKFMLoads()
{
	RemoveBoneGroupMapHashTable();
}

float fixtan(float tan)
{
#if 0
	if(tan > 1.0f && tan < 1.04f)//Hash weirdness: sometimes (usually rightthumb) these tangents get to be 1.03 - force these weirdies to fit
		return(1.0f);
	if(tan < -1.0f && tan > -1.04f)//Hash weirdness: sometimes (usually rightthumb) these tangents get to be 1.03 - force these weirdies to fit
		return(-1.0f);
#endif
	return(tan);
}

void RemoveKeyframe(ts_MChannelItem *channelItem, unsigned short index)
{
	//copy over keyframe[index]

	//careful: according to the memcpy documentaion, an overlapped memcpy will have unpredictable results
	//these give different results, so I'm using the "safe slow" version
#if 1
	//a safe, but slower copy
	for( int i=index; i<channelItem->m_numkeyframes-1; i++)
		channelItem->m_keyframes[i] = channelItem->m_keyframes[i+1];
#else
	//an unsafe, but presumably faster copy
	memcpy(&channelItem->m_keyframes[index], &channelItem->m_keyframes[index+1], (channelItem->m_numkeyframes-index-1)*sizeof(channelItem->m_keyframes));
#endif

	channelItem->m_numkeyframes--;
}