#ifndef __DX_ANIM_SHADER_H
#define __DX_ANIM_SHADER_H

#include "DXRenderShaderBase.h"
#include "DXStreamDescriptor.h"
#include "DXBase.h"
#include "DXPublished.h"
#include "DXVShader.h"

class DXAnimShader : public DXRenderShaderBase
{
#define MAX_PLIGHTS			3
#define MAX_DLIGHTS			3

#pragma pack(16)
	typedef struct _PLightData
	{
		D3DCOLORVALUE		Clr;
		float					Pos[4];
		float					Attenuations[4];
	} PLightData;

	typedef struct _DLightData
	{
		D3DCOLORVALUE		Clr;
		float					Dir[4];
	} DLightData;

	typedef struct _SLightData
	{
		D3DCOLORVALUE		Clr;
		float					Pos_Power[4];
	} SLightData;

	typedef struct _ShaderData
	{
		PLightData		PLights[MAX_PLIGHTS];
		DLightData		DLights[MAX_DLIGHTS];
		float				EyePosWorld[4];
		SLightData		SLight;
		float				SkyVec[4];
		D3DCOLORVALUE	ASkyColor;
		D3DCOLORVALUE	AGroundColor;
	} RegsLight;
#pragma pack( )

	static RegsLight		m_RegDataLights;

	bool						b_SendLights;

	float						d_SpecularPower;

	DWORD *					p_LocalDesc;

	DXVShader &				d_Shader;

	void					convertHWLights( void );
	void					sendShaderData( void );

public:
	DXAnimShader( 
		DXVShader &				_Shader,
		bool						_SendLights,
		float						_SpecularPower = 0.0f
		) ;

	~DXAnimShader( void );

	void					render( void );
} ;

#endif

