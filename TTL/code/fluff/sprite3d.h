//////////////////////////////////////////////////////////////////////////////
//
// sprite3d.h
//
// Display 3d objects as sprites.
//
// Nate Robins, July 2003.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SPRITE3D_H
#define SPRITE3D_H

//////////////////////////////////////////////////////////////////////////////

#include "container/Darray.h"

//////////////////////////////////////////////////////////////////////////////

class CActor;

//////////////////////////////////////////////////////////////////////////////

class Sprite3d
{
public:

	// constructors/destructors.
	Sprite3d( const char* geometry, float screenX, float screenY );
	~Sprite3d();

	// set the position of the sprite in screen space.
	void SetPosition( float screenX, float screenY );

	// set the scale of the sprite.
	void SetScale( float scaleX, float scaleY );

public:

	// update all the sprites.
	static void UpdateAll();

	// delete all the sprites.
	static void DestroyAll();

private:

	// create actor object
	CActor* CreateActor( const char* geometry, Vector3 position );

	// update this sprite.
	void Update();

private:

	CActor* d_actor;	// handle to the environmental object (for geometry).

	Vector3 d_position;
	Vector3 d_scale;

private:

	static DynList<Sprite3d> d_spriteList;
};

//////////////////////////////////////////////////////////////////////////////

#endif // SPRITE3D_H
