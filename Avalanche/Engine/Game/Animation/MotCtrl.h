#ifndef __MOTCTRL_H
#define __MOTCTRL_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "Math/Quatern.h"
#include "Game/Animation/Bone.h"	// for MAX_BONENAME_LEN
#include "EngineHelper/Hash.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif	//GCN

/******************* forward declarations ****************/

/******************* defines *****************************/

#define MOTION_HEADER_NAME_LEN    16	// animation name

/******************* macros ******************************/

/******************* structures **************************/

/* structures used to build up/parse the .mdb file */

typedef u32 BoneGroupBitMask;
#define ALL_BONE_GROUPS (0xffffffff)	//special BoneGroupBitMask to process all groups

/* a key frame consists of a time and a value, both 16-bit for good compression */

struct ts_MotionKeyFrame
{
   u16				d_u16Time;	// in jiffies (one jiffy is 1/300 of a second)
   s16				d_s16Value;
#ifdef GCN
	inline void EndianSwap()
	{
		ByteSwap(d_u16Time);
		ByteSwap(d_s16Value);
	}
#endif	//GCN
};

/* a channel item is the key frame data for one channel component-- x/y/z translate, x/y/z scale, or x/y/z/w rotation */

struct ts_BoneChannelItem
{
	u16					d_u16NumKeyFrames;
	u16					d_u16CachedKeyFrameIndex;	// last active keyframe pair in use (index and index + 1)
	ts_MotionKeyFrame	*d_pKeyFrames;					// array of key frames
	union
	{
		float				d_fScaleValue;					// when we have keys
		float				d_fFlatNonDefaultValue;		// if we don't have keys
	};
#ifdef GCN
	inline void EndianSwap()
	{
		ByteSwap(d_u16NumKeyFrames);
		ByteSwap(d_u16CachedKeyFrameIndex);
		ByteSwap(d_pKeyFrames);		// the swap happens when it's an offset, not a pointer
		ByteSwap(d_fScaleValue);
	}
#endif	//GCN
};

/* this data used to be repeated for each channel entry, but a lot of it is shared, so combine it to save space */

struct ts_BoneEntry
{
	char			d_Name[MAX_BONENAME_LEN];
	u16			d_u16GroupNum;
	u16			d_u16Pad;
	Matrix3x3	d_mBoneToParent;
#ifdef GCN
	inline void EndianSwap()
	{
		ByteSwap(d_u16GroupNum);
		ByteSwap(d_u16Pad);	// in case it gets used later, this will error out if name is changed
		ByteSwap(d_mBoneToParent.e00);
		ByteSwap(d_mBoneToParent.e01);
		ByteSwap(d_mBoneToParent.e02);
		ByteSwap(d_mBoneToParent.e10);
		ByteSwap(d_mBoneToParent.e11);
		ByteSwap(d_mBoneToParent.e12);
		ByteSwap(d_mBoneToParent.e20);
		ByteSwap(d_mBoneToParent.e21);
		ByteSwap(d_mBoneToParent.e22);
	}
#endif	//GCN
};

/* a channel contains all the data to animate a single bone during a motion */

struct ts_BoneChannel
{
	u16						d_u16ChannelItemBitArray;	// which channel components this channel contains
	u16						d_u16Pad;
	ts_BoneEntry			*d_pBoneEntry;					// info about bone this channel controls (typically shared with other channels)
	ts_BoneChannelItem	*d_pBoneChannelItem;			// array of bone channel item records
#ifdef GCN
	inline void EndianSwap()
	{
		ByteSwap(d_u16ChannelItemBitArray);
		ByteSwap(d_u16Pad);				// in case it gets used later, this will error out if name is changed
		ByteSwap(d_pBoneEntry);			// the swap happens when it's an offset, not a pointer
		ByteSwap(d_pBoneChannelItem);	// the swap happens when it's an offset, not a pointer
	}
#endif //GCN
};

/* each individual motion (converted from a kfm file) has one of these */

struct ts_MotionHeader
{
	char					d_Name[MOTION_HEADER_NAME_LEN];
	u16					d_u16NumFrames;			// length of animation in frames (each frame is 1/30 second)
	u16					d_u16NumBoneChannels;	// each bone is animated by a corresponding channel
	ts_BoneChannel		*d_pBoneChannel;			// array of bone channel records
	BoneGroupBitMask	d_BoneGroupBitMask;

#ifdef GCN
	inline void EndianSwap()
	{
		ByteSwap(d_u16NumFrames);
		ByteSwap(d_u16NumBoneChannels);
		ByteSwap(d_pBoneChannel);			// the swap happens when it's an offset, not a pointer
		ByteSwap(d_BoneGroupBitMask);
	}
#endif //GCN
};

struct ts_MotionFileHeader
{
	u16					d_u16NumMotions;
	u16					d_u16NumBoneEntries;
	ts_MotionHeader	**d_ppMotionHeaderTable;	// array of motion header pointers
	ts_BoneEntry		*d_pBoneEntries;				// array of bone entry records
#ifdef GCN
	inline void EndianSwap()
	{
		ByteSwap(d_u16NumMotions);
		ByteSwap(d_u16NumBoneEntries);
		ByteSwap(d_ppMotionHeaderTable);	// the swap happens when it's an offset, not a pointer
		ByteSwap(d_pBoneEntries);			// the swap happens when it's an offset, not a pointer
	}
#endif //GCN
};

class MotionNameHash
{
public:
	MotionNameHash( int size )
	{
		d_hashSize  = size;
		d_hashTable = new HashEntry[ size ];
		memset( d_hashTable, 0, sizeof( HashEntry ) * size );
	}
	~MotionNameHash()
	{
		for ( int i = 0 ; i < d_hashSize ; i++ )
			if ( d_hashTable[ i ].name )
				delete [] d_hashTable[ i ].name;
		delete d_hashTable;
	}
	void Add( const char* name, int id )
	{
		ASSERT_PTR( name );
		ASSERT( name[ 0 ] );

		if ( name && name[0] )
		{
			int nameLen = strlen( name );
			u32 hash    = Hash::CalcChecksum( name, nameLen );

			int i;
			for ( i = hash % d_hashSize ; d_hashTable[ i ].name ; )
				i = ( i + 1 ) % d_hashSize;

			d_hashTable[ i ].hash = hash;
			d_hashTable[ i ].name = new char[ nameLen + 1 ];
			d_hashTable[ i ].id   = id;
			strcpy( d_hashTable[ i ].name, name );
		}
	}
	int LookupID( const char* name ) const
	{
		ASSERT_PTR( name );
		ASSERT( name[ 0 ] );

		if ( name && name[0] )
		{
			int nameLen = strlen( name );
			u32 hash    = Hash::CalcChecksum( name, nameLen );

			for ( int i = hash % d_hashSize ; d_hashTable[ i ].name ; i = ( i + 1 ) % d_hashSize )
				if ( d_hashTable[ i ].hash == hash && !strcmp( d_hashTable[ i ].name, name ) )
					return d_hashTable[ i ].id;
		}

		return -1;
	}
	u32 Size() const 
	{
		int size = 0;
		size += sizeof( *this );
		size += sizeof( HashEntry ) * d_hashSize;

		for ( int i = 0 ; i < d_hashSize ; i++ )
			if ( d_hashTable[ i ].name )
				size += strlen( d_hashTable[ i ].name ) + 1;

		return size;
	}

private:
	struct HashEntry
	{
		u32   hash;
		char* name;
		int   id;
	};
	HashEntry*	d_hashTable;
	int			d_hashSize;
};

#endif //__MOTCTRL_H
