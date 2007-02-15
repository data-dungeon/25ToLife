#ifndef __MESH_PASS_INFO_H
#define __MESH_PASS_INFO_H


struct MeshPassInfo
{
	enum PassFlag
	{
		PASS_FLAG_NONE = 						0x0000,
		PASS_FLAG_UNCOMPRESSED_COLORS	=	0x0001,
		PASS_FLAG_DYNAMIC_TEXTURE = 		0x0002,
		PASS_FLAG_ENVIRONMENT_MAPPED =	0x0004,
		PASS_FLAG_NO_Z_WRITE = 				0x8000,
	} ;

	enum PassType
	{
		PASS_TYPE_NORMAL,
		PASS_TYPE_TEXTURE_BLEND,
		PASS_TYPE_NORMAL_MAPPING
	} ;

	PassType					mPassType;

	unsigned int			mPassFlags;
				
	// stream indexes (-1 == no stream for this pass)
	int						idxRGBAStream;
	int						idxNormalStream;
	int						idxTexCoordStream;

	int						m_textureID;
	int						m_sortKey;
} ;

#endif