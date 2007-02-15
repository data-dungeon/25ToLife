#ifndef __WRITE_MODE_TYPES_H
#define __WRITE_MODE_TYPES_H

enum RGBADataSize
{
	RGBA_8888 = 0,
	RGBA_5551,
	RGBA_ALPHA_BASED,					// use 5551 if possible

	RGBA_DATA_SIZES
} ;


enum RGBAStorageMethod
{
	RGBA_STORE_NONE = 0,
	RGBA_STORE_COMPRESSED,
	RGBA_STORE_UNCOMPRESSED,

	RGBA_STORE_METHODS
} ;

// JLB: Format = ( (RGBADataSize << 16) | RGBAStorageMethod )
typedef unsigned int RGBADataFormat;

inline RGBADataFormat			createRGBADataFormat( RGBAStorageMethod _RGBAMethod, RGBADataSize _RGBASize)
{
	return ( (_RGBASize << 16) | _RGBAMethod );
}				

inline RGBADataSize 				extractRGBADataSize( RGBADataFormat fmt)					{ return (RGBADataSize) (fmt >> 16); }
inline RGBAStorageMethod 		extractRGBAStorageMethod( RGBADataFormat fmt)				{ return (RGBAStorageMethod) (fmt & 0xffff); }



enum NormalDataSize
{
	NORMAL_FLOAT = 0,
	NORMAL_INT16,
	NORMAL_INT8,

	NORMAL_DATA_SIZES
} ;

enum NormalStorageMethod
{
	NORMAL_STORE_NONE = 0,
	NORMAL_STORE_ROW_STANDARD,
	NORMAL_STORE_ROW_COMPACT,
	NORMAL_STORE_COLUMN,

	NORMAL_STORAGE_METHODS
} ;

// JLB: Format = ((NormalDataSize << 16) | NormalStorageMethod)
typedef unsigned int NormalDataFormat;

inline NormalDataFormat  		createNormalDataFormat(  NormalStorageMethod _NormalMethod, NormalDataSize _NormalSize)
{
	return ( (_NormalSize << 16) | _NormalMethod);
}				

inline NormalDataSize 			extractNormalDataSize( NormalDataFormat fmt)				{ return (NormalDataSize) (fmt >> 16); }
inline NormalStorageMethod 	extractNormalStorageMethod( NormalDataFormat fmt)		{ return (NormalStorageMethod) (fmt & 0xffff); }



#endif