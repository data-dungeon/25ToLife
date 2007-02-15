#ifndef __SHADER_RECORDS_H
#define __SHADER_RECORDS_H

typedef struct _ShaderRecord
{
	enum ShaderFlags
	{
		SHFLAG_ANIM							= 0x00000001,
		SHFLAG_ENV_MAP						= 0x00000002,
		SHFLAG_TWO_SIDED					= 0x00000004,
		SHFLAG_NO_SHADOW					= 0x00000008
	} ;

	unsigned int				d_ShaderFlags;

	unsigned int				d_StreamIndex;

	float							d_EnvMapOpacity;
	unsigned int				d_TextureContextID;

	unsigned int				d_NumShaderFragments;
	unsigned int				d_OffsetShaderFragments;

	unsigned int				d_SizeShaderData;
	unsigned int				d_OffsetShaderData;
} ShaderRecord;

#endif