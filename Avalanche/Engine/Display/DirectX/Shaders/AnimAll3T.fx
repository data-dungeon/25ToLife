/* 	                                       Animated Model Shader                                      */

/* --------------------------------------------------------------------------------------------------
	Standard Input/Output Structures
   -------------------------------------------------------------------------------------------------- */
struct VS_INPUT
{
	float4				Pos			: POSITION;
	float3				Normal0		: NORMAL0;
    float4				Color0		: COLOR0;
    float2				Tex0		: TEXCOORD0;
    float2				Tex1		: TEXCOORD1;
    float2				Tex2		: TEXCOORD2;
} ;

struct VS_OUTPUT
{
    float4				Pos			: POSITION;
    float4				Diffuse		: COLOR0;
    float4				Specular	: COLOR1;
    float				Fog			: FOG;
    float2				Tex0		: TEXCOORD0;
    float2				Tex1		: TEXCOORD1;
    float2				Tex2		: TEXCOORD2;
} ;

/* --------------------------------------------------------------------------------------------------
	Locally Defined Structures
   -------------------------------------------------------------------------------------------------- */
struct LightPoint
{ // 3 constant registers
	float4				Color;
	float3				Pos;
	float3				Attenuations;
} ;

struct LightDirectional
{ // 2 constant registers
	float4				Color;
	float3				Direction;
} ;	

/* --------------------------------------------------------------------------------------------------
	Local Variables
   -------------------------------------------------------------------------------------------------- */
float4x4			mWorldView					: register(c0);		//  4 registers	@	c00
float4x4			mWorldProj					: register(c4);		//  4 registers	@	c04

float2				mFogData					: register(c8);		//	1 register	@	c08

LightPoint			mPointLights[3]				: register(c9);		//  9 registers	@	c09
LightDirectional	mDirectionalLights[3]		: register(c18);	//  6 registers	@	c18

float3				mSkyVec						: register(c27);	//  1 register  @	c24
float4				mAmbientSky					: register(c28);	//  1 register	@	c25
float4				mAmbientGround				: register(c29);	//  1 register	@	c26

float4x4			mBoneToWorldMatrices[16]	: register(c32);	// 48 registers	@	c32

/* --------------------------------------------------------------------------------------------------
	Local Constants
   -------------------------------------------------------------------------------------------------- */
static float		cHalf = 0.5f;

/* --------------------------------------------------------------------------------------------------
	Local Functions
   -------------------------------------------------------------------------------------------------- */
float3 applyPointLight( LightPoint Light, float3 worldPos, float3 worldNormal )
{
	float		dist = length( Light.Pos - worldPos );
	float3		direction = normalize( Light.Pos - worldPos );
	
	float		atten = 1.0f / (Light.Attenuations[0] + Light.Attenuations[1] * dist + Light.Attenuations[2] * dist * dist );
	
	return Light.Color * clamp( dot( worldNormal, direction ) * atten, 0.0f, 1.0f );
}

float3 applyDirectionalLight( LightDirectional Light, float3 worldNormal )
{
	return Light.Color * clamp( dot( worldNormal, Light.Direction ), 0.0f, 1.0f);
}

float3 applyHemisphericalAmbient( float3 skyVec, float3 skyColor, float3 groundColor, float3 worldNormal )
{
    // Hemi Calc
	float		angle = cHalf + ( dot( worldNormal, skyVec ) * cHalf );

	return lerp( groundColor, skyColor, (float3) angle );
}

/* --------------------------------------------------------------------------------------------------
	Main( )
   -------------------------------------------------------------------------------------------------- */
VS_OUTPUT AnimAll3T( VS_INPUT i )
{
    VS_OUTPUT   o;

    // Transform Position & Normal to World
	float4		worldPos = mul( float4( i.Pos.xyz, 1.0f ), mBoneToWorldMatrices[i.Pos.w] );
	float3		worldNormal = normalize( mul( float4( i.Normal0.xyz, 0.0f ), mBoneToWorldMatrices[i.Pos.w] ) );
	
    // Transform Position to View
    o.Pos = mul( worldPos, mWorldProj );
    
	// Shade ( VertexColor + Ambient + Point + Directional )
	float3 sumColor = applyHemisphericalAmbient( mSkyVec, mAmbientSky, mAmbientGround, worldNormal );
	
	sumColor +=	applyPointLight( mPointLights[0], worldPos, worldNormal );
	sumColor +=	applyPointLight( mPointLights[1], worldPos, worldNormal );
	sumColor +=	applyPointLight( mPointLights[2], worldPos, worldNormal );
	
	sumColor +=	applyDirectionalLight( mDirectionalLights[0], worldNormal );
	sumColor +=	applyDirectionalLight( mDirectionalLights[1], worldNormal );
	sumColor +=	applyDirectionalLight( mDirectionalLights[2], worldNormal );
		
	o.Diffuse.rgb = sumColor.rgb;
	o.Diffuse.a = i.Color0.a;

	o.Specular.r = 0.0f;
	o.Specular.g = 0.0f;
	o.Specular.b = 0.0f;
	o.Specular.a = 0.0f;
	
	// calculate Fog Value
	float4		camPos = mul( worldPos, mWorldView );

	float		fogDist = camPos.z - mFogData.x;
	
	o.Fog = 1.0f - clamp( fogDist * mFogData.y, 0.0f, 1.0f );
	
    // Texture Coordinates
    o.Tex0 = i.Tex0;
    o.Tex1 = i.Tex1;
    o.Tex2 = i.Tex2;

    return o;
}

