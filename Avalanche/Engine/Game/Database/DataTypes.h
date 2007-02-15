/*************************************/
/*                                   */
/*   g_DataTypes.h                   */
/*   big juju prototype  06/01/99    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   data types                      */
/*                                   */
/*************************************/

/* a list of all data types in big juju. once you know something's data type, you can always cast a pointer to it to
the proper structure. this is useful for ownership checking. for example, when something requests a matrix, it passes
the owner's data type so if the matrix gets stolen by something else we can clean up sensibly. some data types may have
sub-types, like instances */

#ifndef __G_DATATYPES_H
#define __G_DATATYPES_H

#include "platform/BaseType.h"	// low-level include

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/******************* defines *****************************/

/* data types-- up to 256 types */

enum
{
	DATA_INSTANCE,				// instantiation of something
	DATA_PARTICLE,				// a particle (ts_Particle)
	DATA_DISTORTION,			// a distortion record
// do not remove this unless you are prepared to rebuild all data!
/* obsolete */	OBSOLETE_DATA_GRID,				// a grid section
	DATA_LEVEL,					// a game level
	DATA_LIGHT,					// a light source
	DATA_CAMERA,				// a camera
	DATA_MATRIX,				// a matrix from the matrix pool
	DATA_AURA,					// an aura record
	DATA_SOM,					// a SOM model (ts_PatchSOM)
	DATA_COLLISION_SOM,		// a SOM model for generating collision data (ts_CollisionSOM), for use by TerrainToola
	DATA_PS2_DMA_SOM,
	DATA_DBL_FILE,				// a loaded dbl file
	DATA_LOD,					// an LOD record (ts_LOD)
	DATA_GCN_SOM,				// a GCN optimized SOM
	DATA_SCRIPT,				// compiled script data (created by animrip)
// do not remove this unless you are prepared to rebuild all data!
/* obsolete */	OBSOLETE_DATA_MOTION,			// compiled motion data (created by motnset3)
	DATA_MULTI_SOM,			// a hack on a SOM to test dynamic meshes
	DATA_FILESTREAM,			// a loaded FileStream file, for recording call to FileStream::PreLoad
	DATA_GCN_SOM2,				// a GCN optimized SOM, version 2
	DATA_COMPRESSED_FILE,	// a compiled-into-code (or possibly pre-loaded) compressed file
	DATA_SOM_LIST,				// used to dynamically merge several soms that share a bone structure (body by pieces)

	MAX_DATA_TYPES				// must be last
};

/* instance sub-types */

enum
{
	INSTANCE_STATIC,					// static geometry
	INSTANCE_ANIMATED,				// animated instance

	MAX_INSTANCE_TYPES				// must be last
};

/******************* macros ******************************/

/* EVERY data structure begins with a data header. this is a 32-bit word, typically divided up into bytes. the first
byte is the base data type of the record, from the constants enumerated in datatypes.h. so given a pointer to a data
record, you can always figure out what it is by using the DATA_TYPE macro. the extra bytes are typically used as flags,
or a sub-type, or a priority value-- there are different flavors of data header defined in datatypes.h. the generic data
header only knows about the base data type, which is always there */

#define DATA_TYPE(ptr)			(((ts_GenericDataHeader *) ptr)->u8DataType)
#define VALID_DATA_TYPE(ptr)	(DATA_TYPE(ptr) < MAX_DATA_TYPES)

/******************* structures **************************/

/* all data structures reserve the first 32-bit word for type use. the base data type of the structure is always the
first byte. there are various flavors of the other 3 bytes: sub-types, flags, priorities, etc. the generic flavor is for
when all you have is a pointer to a data record and you want to know what type it is. the first byte is guaranteed to
be the data type, but you don't know what the other three bytes are */

struct ts_GenericDataHeader
{
	u8		u8DataType;		// base data type, from enumerated list above
	u8		u8Pad1;
	u8		u8Pad2;
	u8		u8Pad3;
};

struct ts_FlagsDataHeader
{
	u8		u8DataType;		// base data type, from enumerated list above
	u8		u8Flags2;		// more flags
	u16	u16Flags;		// flags
};

struct ts_SubFlagsDataHeader
{
	u8		u8DataType;		// base data type, from enumerated list above
	u8		u8SubDataType;	// sub type
	u16	u16Flags;		// flags
};

struct ts_PriDataHeader
{
	u8		u8DataType;		// base data type, from enumerated list above
	u8		u8Priority;		// priority
	u8		u8Pad1;
	u8		u8Pad2;
};

/******************* global variables ********************/

/******************* global prototypes *******************/

#ifdef __cplusplus
}
#endif //__cplusplus



#endif // __G_DATATYPES_H
