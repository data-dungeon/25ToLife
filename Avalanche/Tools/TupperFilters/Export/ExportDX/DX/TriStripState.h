#ifndef __TRISTRIP_STATE_H
#define __TRISTRIP_STATE_H

#include "VertexDescriptor.h"


typedef struct _TriStripState
{
	unsigned int				d_ShaderFlags;

	unsigned int				d_TextureContextIndex;
	float							d_EVMapOpacity;

	StreamDescriptor			d_StreamDesc;
	VertexDescriptor			d_VertexDesc;

	unsigned int				d_MaterialIndex;

} TriStripState;

bool operator!= ( const struct _TriStripState &state1, const struct _TriStripState &state2)
{
	if( state1.d_VertexDesc != state2.d_VertexDesc )
		return true;

	if( state1.d_StreamDesc != state2.d_StreamDesc )
		return true;

	if( state1.d_TextureContextIndex != state2.d_TextureContextIndex )
		return true;

	if( state1.d_EVMapOpacity != state2.d_EVMapOpacity )
		return true;

	if( state1.d_ShaderFlags != state2.d_ShaderFlags )
		return true;

	return false;
} 

class TriStripSort
{

public:
	bool operator( ) ( const struct _TriStripState &state1, const struct _TriStripState &state2) const
	{

		if( state1.d_VertexDesc < state2.d_VertexDesc )
			return true;

		if( state1.d_StreamDesc < state2.d_StreamDesc )
			return true;

		if( state1.d_TextureContextIndex < state2.d_TextureContextIndex)
			return true;

		if( state1.d_EVMapOpacity < state2.d_EVMapOpacity)
			return true;

		if( state1.d_ShaderFlags < state2.d_ShaderFlags )
			return true;

		return false;
	}
} ;


class ShaderDescriptor
{
public:
	unsigned int				d_ShaderFlags;

	unsigned int				d_StreamIndex;

	unsigned int				d_TextureContextID;
	float							d_EnvMapOpacity;

	ShaderDescriptor(
		unsigned int			_ShaderFlags,
		unsigned int			_StreamIndex,
		unsigned int			_TextureContextID,
		float						_EnvMapOpacity
		) 
	{
		d_ShaderFlags = _ShaderFlags;
		d_StreamIndex = _StreamIndex;
		d_TextureContextID = _TextureContextID;
		d_EnvMapOpacity = _EnvMapOpacity;
	}

	bool operator < ( const ShaderDescriptor & shdrR ) const
	{
		if( d_StreamIndex < shdrR.d_StreamIndex )
			return true;

		if( d_ShaderFlags < shdrR.d_ShaderFlags )
			return true;

		if( d_TextureContextID < shdrR.d_TextureContextID )
			return true;

		if( d_EnvMapOpacity < shdrR.d_EnvMapOpacity )
			return true;

		return false;
	}

	bool operator == ( const ShaderDescriptor & shdrR ) const
	{
		if( d_StreamIndex != shdrR.d_StreamIndex )
			return false;

		if( d_ShaderFlags != shdrR.d_ShaderFlags )
			return false;

		if( d_TextureContextID != shdrR.d_TextureContextID )
			return false;

		if( d_EnvMapOpacity != shdrR.d_EnvMapOpacity )
			return false;

		return true;
	}

} ;

#endif
