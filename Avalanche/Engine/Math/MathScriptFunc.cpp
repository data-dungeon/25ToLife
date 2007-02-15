//============================================================================
//=
//= MathScriptFunc.cpp - 
//=    Script Interface routines to module "math"
//=
//============================================================================

#include "Math/MathPCH.h"

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

//============================================================================

static void Script_sin( ScriptVM* vm )
{
	vm->Push( Math::Sin( vm->PopFloat() ) );
}

//============================================================================

static void Script_asin( ScriptVM* vm )
{
	vm->Push( Math::ArcSin( vm->PopFloat() ) );
}

//============================================================================

static void Script_cos( ScriptVM* vm )
{
	vm->Push( Math::Cos( vm->PopFloat() ) );
}

//============================================================================

static void Script_acos( ScriptVM* vm )
{
	vm->Push( Math::ArcCos( vm->PopFloat() ) );
}

//============================================================================

static void Script_atan2( ScriptVM* vm )
{
	float y = vm->PopFloat();
	float x = vm->PopFloat();
	vm->Push( Math::ArcTan2( x, y ) );
}

//============================================================================

static void Script_sqrt( ScriptVM* vm )
{
	vm->Push( Math::Sqrt( vm->PopFloat() ) );
}

//============================================================================

static void Script_sincos( ScriptVM* vm )
{
	float radians = vm->PopFloat();

	float s, c;
	Math::SinCos( radians, s, c );

	vm->Push( s );
	vm->Push( c );
}

//============================================================================

static void Script_pow( ScriptVM* vm )
{
	float y = vm->PopFloat();
	float x = vm->PopFloat();
	vm->Push( Math::Pow( x, y ) );
}

//============================================================================

static void Script_exp( ScriptVM* vm )
{
	vm->Push( Math::Exp( vm->PopFloat() ) );
}

//============================================================================

static void Script_Vector3Length( ScriptVM* vm )
{
	Vector3 v = vm->PopVector3();
	float len = v.Length();
	vm->Push( len );
}

//============================================================================

static void Script_Vector3Add( ScriptVM* vm )
{
	Vector3 v2 = vm->PopVector3();
	Vector3 v1 = vm->PopVector3();
	vm->Push( v1 + v2 );
}

//============================================================================

static void Script_Vector3Sub( ScriptVM* vm )
{
	Vector3 v2 = vm->PopVector3();
	Vector3 v1 = vm->PopVector3();
	vm->Push( v1 - v2 );
}

//============================================================================

static void Script_Vector3CrossProduct( ScriptVM* vm )
{
	Vector3 v2 = vm->PopVector3();
	Vector3 v1 = vm->PopVector3();
	vm->Push( Vector3::Cross( v1, v2 ) );
}

//============================================================================

static void Script_Vector3DotProduct( ScriptVM *vm )
{
	Vector3 v2 = vm->PopVector3();
	Vector3 v1 = vm->PopVector3();
	vm->Push( Vector3::Dot( v1, v2 ) );
}

//============================================================================

static void Script_Vector3Scale( ScriptVM *vm )
{
	float	  scale = vm->PopFloat();
	Vector3 v     = vm->PopVector3();
	vm->Push( v * scale );
}

//============================================================================

static void Script_Vector3Normalize( ScriptVM *vm )
{
	Vector3 v = vm->PopVector3();
	v.SafeNormalize();
	vm->Push( v );
}

//============================================================================

static void Script_NormalAngle( ScriptVM *vm )
{
	float v = vm->PopFloat();
	vm->Push( Math::NormalAngle( v ) );
}

//============================================================================

static void Script_AngleDelta( ScriptVM *vm )
{
	float angle2 = vm->PopFloat();
	float angle1 = vm->PopFloat();
	vm->Push( Math::AngleDelta( angle1, angle2 ) );
}

//============================================================================

static void Script_Deg2Rad( ScriptVM *vm )
{
	vm->Push( Math::Deg2Rad( vm->PopFloat() ) );
}

//============================================================================

static void Script_Rad2Deg( ScriptVM *vm )
{
	vm->Push( Math::Rad2Deg( vm->PopFloat() ) );
}

//============================================================================

void Math_RegisterScriptFuncs()
{
	SCRIPT_REGISTER( sin                 );
	SCRIPT_REGISTER( asin                );
	SCRIPT_REGISTER( cos                 );
	SCRIPT_REGISTER( acos                );
	SCRIPT_REGISTER( atan2               );
	SCRIPT_REGISTER( sqrt                );
	SCRIPT_REGISTER( sincos              );
	SCRIPT_REGISTER( pow                 );
	SCRIPT_REGISTER( exp                 );
	SCRIPT_REGISTER( Vector3Length       );
	SCRIPT_REGISTER( Vector3Add          );
	SCRIPT_REGISTER( Vector3Sub          );
	SCRIPT_REGISTER( Vector3CrossProduct );
	SCRIPT_REGISTER( Vector3DotProduct   );
	SCRIPT_REGISTER( Vector3Scale        );
	SCRIPT_REGISTER( Vector3Normalize    );
	SCRIPT_REGISTER( NormalAngle         );
	SCRIPT_REGISTER( AngleDelta          );
	SCRIPT_REGISTER( Deg2Rad			 );
	SCRIPT_REGISTER( Rad2Deg	         );
}

//============================================================================
