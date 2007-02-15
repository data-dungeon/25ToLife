/* 	                                       Animated Model Shader                                      */

/* --------------------------------------------------------------------------------------------------
	Standard Input/Output Structures
   -------------------------------------------------------------------------------------------------- */
struct VS_INPUT
{
	float4				Pos			: POSITION;
	float4				Normal0		: NORMAL0;
	float4				Color0		: COLOR0;
	float2				Tex0		: TEXCOORD0;
} ;

struct VS_OUTPUT
{
    float4				Pos			: POSITION;
    float4				Diffuse		: COLOR0;
    float4				Specular	: COLOR1;
} ;

/* --------------------------------------------------------------------------------------------------
	Local Variables
   -------------------------------------------------------------------------------------------------- */
float4x4			mWorldView					: register(c0);		//  4 registers	@	c00 - c03
float4x4			mWorldProj					: register(c4);		//	4 registers @	c04

float4				mShadowColor				: register(c8);		//  1 register	@	c08

float4x4			mBoneToWorldMatrices[16]	: register(c32);	// 64 registers	@	c32

/* --------------------------------------------------------------------------------------------------
	Main( )
   -------------------------------------------------------------------------------------------------- */
VS_OUTPUT AnimShadowCast( VS_INPUT i )
{
    VS_OUTPUT   o;

    // Transform Position to World
	float4		worldPos = mul( float4( i.Pos.xyz, 1.0f ), mBoneToWorldMatrices[i.Pos.w] );
	
    // Transform Position to View
    o.Pos = mul( worldPos, mWorldProj );
    
	o.Diffuse = mShadowColor;
	
	o.Specular.r = 0.0f;
	o.Specular.g = 0.0f;
	o.Specular.b = 0.0f;
	o.Specular.a = 0.0f;
	
    return o;
}
