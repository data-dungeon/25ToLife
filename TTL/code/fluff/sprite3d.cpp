//////////////////////////////////////////////////////////////////////////////
//
// sprite3d.cpp
//
// Display 3d objects as sprites.
//
// Nate Robins, July 2003.
//
//////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"
#include "hud/hudlib.h"

//////////////////////////////////////////////////////////////////////////////

DynList<Sprite3d> Sprite3d::d_spriteList( 8, 8, true );

//////////////////////////////////////////////////////////////////////////////

Sprite3d::Sprite3d( const char* geometry, float screenX, float screenY )
{
	d_position.X( screenX );
	d_position.Y( screenY );

	d_scale.X( 1.0f );
	d_scale.Y( 1.0f );

	d_actor = CreateActor( geometry, d_position );
	ASSERT( d_actor != 0 );

	Sprite3d::d_spriteList.Add( this );
}

//////////////////////////////////////////////////////////////////////////////

Sprite3d::~Sprite3d()
{
	// looks like the cast deletes this for me.
	//delete d_actor;
}

//////////////////////////////////////////////////////////////////////////////

void Sprite3d::SetPosition( float screenX, float screenY )
{
	d_position.X( screenX );
	d_position.Y( screenY );
}

//////////////////////////////////////////////////////////////////////////////

void Sprite3d::SetScale( float scaleX, float scaleY )
{
	ASSERT( d_actor );
	d_scale.X( scaleX );
	d_scale.Y( scaleY );
}

//////////////////////////////////////////////////////////////////////////////

CActor* Sprite3d::CreateActor( const char* geometry, Vector3 position )
{
	// geometry should be the name of the replace object (e.g., "amulet").
	CActor* actor = new CActor;

	DirCos3x3 orientation; orientation.SetYRotation( 0 );
	actor->Place( position, orientation );

	if ( actor->Initialize() )
	{
//		strcpy( actor->EnvName(), name );
//		actor->SetEnvType( GetEnvType( name ) );
//		actor->Category() |= GetActorCategory( name );
//		actor->MoveToNonProtagonistCast();

		return actor;
	}
	else
	{
		delete actor;
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////

static Vector3 ScreenToCamera( Vector3 screen )
{
	float width = Sprite::SimulatedScreenWidth();
	float height = Sprite::SimulatedScreenHeight();

	screen.Y( height - screen.Y() - 1 );
	screen.X( ( ( screen.X() / width  ) - 0.5f ) * 2.0f );
	screen.Y( ( ( screen.Y() / height ) - 0.5f ) * 2.0f );

	return Vector3( screen.X(), screen.Y(), -1.0f );
}

static Vector3 CameraToWorld( Vector3 camera )
{
	return g4CameraToWorld.Transform( camera.X(), camera.Y(), camera.Z() );
}

static Vector3 ScreenToWorld( Vector3 screen )
{
	return CameraToWorld( ScreenToCamera( screen ) );
}

static DirCos3x3 Billboard()
{
	DirCos3x3 m;

	m.BuildFromVector( g4CameraToWorld.GetRow2() );

	return m;
}

void Sprite3d::Update()
{
	d_actor->GetBodyInWorld() = ScreenToWorld( d_position );
	d_actor->GetBodyToWorld() = Billboard();
	d_actor->Instance()->Matrix().ReverseCatScale( d_scale.X(), d_scale.Y(), 1.0f );
}

//////////////////////////////////////////////////////////////////////////////

void Sprite3d::UpdateAll()
{
	for ( int i = 0; i < d_spriteList.Count(); i++ )
	{
		Sprite3d* sprite = d_spriteList[ i ];
		sprite->Update();
	}
}

//////////////////////////////////////////////////////////////////////////////

void Sprite3d::DestroyAll()
{
	d_spriteList.Reset();
}

//////////////////////////////////////////////////////////////////////////////
