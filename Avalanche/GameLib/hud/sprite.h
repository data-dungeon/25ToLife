//=============================================================================
// File: sprite.h          | TODO: Some usefull comments here...
// Date: 4/24/01           |
// Auth: Daniel Silver     |
//=============================================================================

#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "Display/Common/TextureState.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Game/Managers/TextureMgr/Texture.h"
#include "Game/Managers/SpriteMgr.h"
#include "Math/Matrix.h"
#include "hud/animator.h"
#include "hud/hudobject.h"
#include "hud/stringex.h"

//=============================================================================
// Sprite class definition
//=============================================================================
//
// The Sprite class is built around the Sprite Manager, providing high-level
// functionality suitable for advanced 2D overlays.
//
// To achieve a high degree of flexability, the Sprite class employs key frame
// based animation for each of its channels, all of which can be animated
// independantly of one another.  See Animator.h for information regarding
// the animation system.
//
// To facilitate the creation of complex sprites, that is, multiple sprite
// pieces that join together to produce a complex image, the Sprite class
// supports hierarchtical grouping.  This simplifies the implementation of
// complex sprite groups immensly by allowing the programmer to join sprites
// together at the root, then manipulating only the root to affect every sprite
// attached to it.  A good example of this would be a radar on the heads up
// display, where each blip is a separate sprite overlapping overlapping the
// radar itself.
//
// Please note that the coordinate system for the Sprite class is different
// than that of the Sprite Manager.  Since working with complex sprites usually
// involves working at the pixel level, and because the programmer must know
// exactly the offset of an image in pixels, the Sprite class interprets
// size and position as absolute pixels, where as the Sprite Manager interprets
// them as normalized screen coordinates.
//
// The advantage of this system is that it is simple to work with if you find
// yourself working in pixel space.  The disadvantage is that your sprites
// will only look correct at the resolution at which they were designed.
// Porting the application to another platform with a different screen
// resolution will reproduce the sprites differently.  To compensate for this,
// the Sprite class has the ability to simulate any given screen resolution.
// It would be a good idea for ALL applications, regarldess of their physical
// screen resolution, to simulate a standard resolution, perhaps NTSC or VGA.
//
//=============================================================================
//
// NOTE: The Sprite class uses texture sets for its image data.  A texture set
//			can be acquired by calling dblLoad() and passing in a filename
//			corresponding to a .dbl file containing the texture(s).
//
//=============================================================================
// How to create a simple sprite.
//=============================================================================
//
// Define the sprite either statically or dynamically:
//
//		Sprite *mysprite = new Sprite;
//
// Call the creation function to assign a texture to the sprite and initialize
// the sprite's internal data structures:
//
//		mysprite->Create( textureset, index );
//
// Alternatively:
//
//		mysprite->SetTextureSet( textureset, index );
//		mysprite->Reset();
//
// Next, position the sprite somewhere on the screen:
//
//		mysprite->SetPosition( 320.0f, 240.0f );
//
// Finally, call the update function to recompute the visibility of the sprite:
//
//		mysprite->Update();
//
// Sprite::Update() should be called every frame to ensure that any changes
// made between frames will be updated properly.
//
//=============================================================================
// How to build a complex sprite group
//=============================================================================
//
// Create several sprites either dynamically or statically:
//
// class MyRobot
// {
//		Sprite m_head;				// hands, feet, fingers, toes etc.. were ommitted
//		Sprite m_torso;			// from the example for brevity =)
//		Sprite m_leftarm;
//		Sprite m_rightarm;
//		Sprite m_leftleg;
//		Sprite m_rightleft;
//	};
//
// Call the creation function to assign a texture to each sprite and initialize
// internal data structures:
//
//		MyRobot robot;
//
//		robot.m_head.Create( robottexture, 0 );		// texture index 0 is the head
//		robot.m_torso.Create( robottexture, 1 );		// texture index 1 is the torso
//		robot.m_leftarm.Create( robottexture, 2 );	// texture index 2 is the left arm
//		robot.m_rightarm.Create( robottexture, 2 );	// texture index 2 is also the left arm, soo...
//		robot.m_rightarm.SetScale( -1.0f, 1.0f );		// flip the texture horizontally
//		robot.m_leftleg.Create( robottexture, 3 );	// texture index 3 is the left leg
//		robot.m_rightleg.Create( robottexture, 3 );	// texture index 3 is also the left leg, soo...
//		robot.m_rightleg.Scale( -1.0f, 1.0f );			// flip the texture horizontally
//
// Next, construct the hierarchtical order:
//
//		robot.m_head.AddChild( &robot.m_torso );		// attach the torso to the head and
//		robot.m_torso.AddChild( &robot.m_leftarm );	// attach the legs and arms to the torso so that
//		robot.m_torso.AddChild( &robot.m_rightarm );	// changes to the head (scale, rotation, etc..)
//		robot.m_torso.AddChild( &robot.m_leftleg );	// affect the entire body, while changes to
//		robot.m_torso.AddChild( &robot.m_rightleft );// torso only effect the legs and arms...
//
// Finally, call the update function to recompute the visibility of the entire
// sprite group:
//
//		robot.m_head.Update();								// Attached sprites will be updated too!!
//
// Alternatively, you can update group manually:
//
//		robot.m_head.Draw();									// Sprite::Draw() does not update
//		robot.m_torso.Draw();								// attached sprites so you'll have to do
//		robot.m_leftarm.Draw();								// it manually, and in the correct order!
//		robot.m_rightarm.Draw();
//		robot.m_leftleg.Draw();								// You'll probably want to stick to just
//		robot.m_rightleft.Draw();							// using Sprite::Update() =)
//
// Changes to top-most sprite will affect any attached sprites, for example:
//
//		robot.m_head.SetScale( 0.5f, 0.5f );			// shrink the entire sprite group!
//		robot.m_head.Update();
//
//=============================================================================
// How to create an animated sprite.
//=============================================================================
//
// The animation system in the sprite class has undergone a redesign.  The new
// interface is simpler and far more powerful.  The new interface allows all
// channels to be animated independently of each other.
//
// (A channel is any of the parameters that define a sprite, such as the x and y
// component of the position, scale, origin, and rotation, etc...)
//
// Working with animated sprites isn't much different from regular sprites.
// After the sprite has been created, simply define a set of key frames for
// each channel to be animated, then press Play.
//
// The following example animates the sprite's x coordinate.  At zero seconds,
// the x position is 0.0f.  At one second, the x posision is 640.0f.  And at
// two seconds, the x position is 0.0f again.
//
//		mysprite.m_anim.x.SetFrame( 0.0f, 0.0f );
//		mysprite.m_anim.x.SetFrame( 1.0f, 640.0f );
//		mysprite.m_anim.x.SetFrame( 2.0f, 0.0f );
//		mysprite.m_anim.Play( true );
//
// Specifiying true when playing the animation tells the animation system to
// repeat the animation forever.  As you can see from the example, the sprite
// would move from the left side of the screen to the right and back again and
// continue to do so until the animation is stopped.
//
//		mysprite.m_anim.sprite.
//
// To find out more about the animation system, see Animator.h and Animator.cpp.
//=============================================================================
// FAQ (Frequently Anticipated Questions!)
//=============================================================================
//
// 1. What is a sprite.
// --------------------
//		A sprite is a rectangular image that uses a texture for its image data.
//		The Sprite class wraps the Sprite Manager in order to provide a C++
//		interface and adds functionality.
//
//		The Sprite class is capable of scaling, rotating, and grouping sprites
//		in hierarchtical trees, giving you the power to create advanced on-screen
//		effects with 2d imagery.
//
// 2. How do I use texture sets with my sprites?
// ---------------------------------------------
//		You'll need to	prepare your images in TGA format and write them into a
//		dbl file using ImportExport.
//
//		In its current form, using ImportExport to generate the file takes these
//		steps:
//
//			1. Prepare your files in .TGA format (24-bit or 32-bit) and store
//				them in the same directory.
//			2. Make a text file with the extention .lst (for example, myfiles.lst)
//				This text file simply contains, in order, the files you want munged
//				into the texture set.  Example:
//
//					sprite1.tga
//					sprite2.tga
//					sprite3.tga
//					...
//					spriteN.tga
//
//			3. From the command line, run ImportExport <filename.lst> <output.dbl>
//			4. Copy the .dbl to the game's data directory.
//
//		Load the dbl into memory by calling dblLoad( "<filename.dbl>" );
//
//		Get the latest version of ImportExport.exe from SourceSafe
//		Path=$/Multigen/Creator
//
// 3. Can I use multiple texture sets?
// -----------------------------------
//		Yes, there is no limit on the number of texture sets you can use with
//		your sprites, however, for performance reasons, you should use as few
//		as possible.  Store as many textures as you can within the same texture
//		set.
//
//		Be adviced that the size of a texture set may be limited by the amount
//		of video memory on the target platform.
//
// 4. Can I change the texture set on the fly?
// -------------------------------------------
//		Yes.  Use Sprite.SetTextureSet(), then call Sprite.Update().
//
// 5. Can I use sprite tables?
// ---------------------------
//		Yes.  When you have a single texture with multiple cells, you can specify
//		while creating the sprite that the texture has n columns and n rows.  You
//		specify which cell to use when creating the sprite, or by calling
//		Sprite.SetTexture( n ).
//
//		Texture cells are numbered from left to right, top to bottom, starting at
//		0, the upper-left cell.  In a Sprite Table with 4 columns and 4 rows, the
//		table would look like this:
//			  _______________
//			 |	  |   |   |   |
//			 |	0 | 1 | 2 | 3 |
//        |___|___|___|___|
//			 |	  |   |   |   |
//			 |	4 | 5 | 6 | 7 |
//        |___|___|___|___|
//			 |	  |   |   |   |
//			 |	8 | 9 | 10| 11|
//        |___|___|___|___|
//			 |	  |   |   |   |
//			 |	12| 13| 14| 15|
//        |___|___|___|___|
//
//		If, for example, you wanted to the 9th cell, you'd create the sprite with
//		4 columns and 4 rows, and specifiy the 9 as the desired cell.
//
//			Sprite.Create( TextureSetHandle, TextureIndex, 4, 4, 9 );
//
// 6. Can I use irregularly spaced sprite tables?
// ----------------------------------------------
//		No.  There is no direct support for sprite tables whose cells are
//		irregularly spaced.  However, you can hand-code this yourself by meddling
//		with the texture coordinated manually as you create each sprite.
//
//		An irregularly spaced sprite table would look like this:
//
//			  _______________
//			 |	          |   |
//			 |	          |   |
//        |           |   |
//			 |	          |   |
//			 |     0     | 1 |
//        |           |   |
//			 |           |   |
//			 |           |   |
//        |___________|___|
//			 |	      |       |
//			 |	  2   |   3   |
//        |_______|_______|
//
// 7. What is the difference between the origin and the offset?
// --------------------------------------------------------------
//		The origin is specified as a normalized vector, from 0 to 1, while the
//		offset is specified in pixels.  The offset is usefull when you need to
//		connect several sprites together and you know the exact texture offsets,
//		The offset is always relative to the origin.
//
// 8. How do I change the size of a sprite?
// ----------------------------------------
//		Use Sprite::SetSize().  The width and height of the sprite are specified
//		in pixiels.  The default size of the sprite is the same as the source
//		texture.  Resizing the sprite will not affect child sprites.
//
// 9. How do I specify vertex colors?
// ----------------------------------
//		Use Sprite::SetVertexColor().
//		The vertices are numbered like this:
//
//		0 ________ 1
//		 |        |
//		 |        |
//		 | Sprite |	// The winding order, starting from the upper-left.
//		 |        |
//		 |________|
//    3          2
//
//	10. How do I specify texture coordinates?
//	-----------------------------------------
//		Use Sprite::SetTextureCoordinates().
//		The vertices are numbered like this:
//
//		0 ________ 1
//		 |        |
//		 |        |
//		 | Sprite |	// The winding order, starting from the upper-left.
//		 |        |
//		 |________|
//    3          2
//
// 11. What is the clipping region?
// --------------------------------
//		The clipping region is a set of coordinates that allows you to crop only
//		the portion of the sprite that you want to be visible.
//
//		There are 4 corners to the clipping region, and you can position them as
//		desired.  Only the area inside the clipping region will be visible.
//
//		The coordinates are specified as 0 to 1, with (0,0) being the top left
//		corner of the sprite and (1,1) being the bottom right corner.

//		It is important to note that when clipping is enabled, vertex colors may
//		not be computed correctly over the surface of the sprite because of the
//		way hardware interpolates colors, and may cause banding to occure when
//		the vertices are animating.  To the untrained eye, this is usually
//		undetectable.
//
// 12. How does animation work?
// ----------------------------
//		The animation interface has undergone a complete re-design.  The new
//		interface is different from the old interface in that every channel can
//		now be animated independantly of each other.  Each channel has exactly
//		one Animator.
//
//		An Animator is an interface for creating, managing, and directing key
//		frames.  When an Animator is playing, values for missing frames are
//		interpolated from existing key frames, smoothing out the animation.
//		The only draw back to using animators is that the interpolation is linear.
//		I am currently researching different ways to interpolate non-linearly.
//
//		NOTE: The new interface is not entirely complete.  Texture animation is
//				not implemented at this time.  Sorry!
//
//=============================================================================
// Need additional info?  Ideas?  Call Dan @ 227
//=============================================================================

class Sprite;

class SpriteInterface : public TreeNode
{
	DECLARE_CLASSOBJECT( SpriteInterface );
	DECLARE_CLASSFACTORY( SpriteInterface );
	DECLARE_TREENODE( SpriteInterface );

	public:
		SpriteInterface( void );
		~SpriteInterface( void );

		void SetSpriteHost( Sprite* pSpriteHost ) { m_pSpriteHost = pSpriteHost; }
		Sprite* SpriteHost( void ) { return m_pSpriteHost; }

		virtual bool Init( void* Param );
		virtual void Kill( void );
		virtual void Process( void );
		virtual bool Save( MemoryBank& m );
		virtual bool Load( MemoryBank& m );

	protected:
		virtual void OnUpdate( void );

	protected:
		Sprite *m_pSpriteHost;
};

//=============================================================================
// Sprite : <class description>
//=============================================================================

struct color4
{
	float r;
	float g;
	float b;
	float a;
};

class Sprite;

class SpriteAnim
{
	public:
		SpriteAnim( void );
		virtual ~SpriteAnim();

		void SetTarget( Sprite *pSprite );
		void SetPriority( char Priority );
		void Play( bool Looped );
		void Resume( bool Looped );
		void Stop( void );
		void Rewind( void );
		void Jump( float Time );
		void Clear( void );

		struct _texture {
			Animator32 texture;		// texture.texture;
		} texture;

		struct _position {
			Animator x;					// position.x
			Animator y;					// position.y
		} position;

		struct _size {
			Animator x;					// size.x
			Animator y;					// size.y
		} size;

		struct _scale {
			Animator x;					// scale.x
			Animator y;					//	scale.y
		} scale;

		struct _origin {
			Animator x;					// origin.x
			Animator y;					// origin.y
		} origin;

		struct _offset {
			Animator x;					// offset.x
			Animator y;					// offset.y
		} offset;

		struct _rotation {
			Animator z;					// rotation.z
		} rotation;

		struct _color {
			Animator8 a;				// color.a
			Animator8 r;				// color.r
			Animator8 g;				// color.g
			Animator8 b;				// color.b
		} color;

		struct _familycolor {
			Animator8 a;				// familycolor.a
			Animator8 r;				// familycolor.r;
			Animator8 g;				// familycolor.g;
			Animator8 b;				// familycolor.b;
		} familycolor;

		/* // chopped
		struct _vertex {
			Animator x;					// vertex[n].x
			Animator y;					// vertex[n].y
			Animator8 r;				// vertex[n].r
			Animator8 g;				// vertex[n].g
			Animator8 b;				// vertex[n].b
			Animator8 a;				// vertex[n].a
			Animator u;					// vertex[n].u
			Animator v;					// vertex[n].v
		} vertex[4];

		struct _crop {
			Animator x;					// crop[n].x
			Animator y;					// crop[n].y
		} crop[4];
		*/
};

struct color32
{
	union {
		struct {
			byte b;
			byte g;
			byte r;
			byte a;
		};
		u32 color;
	};
};

struct SPRITESTATESTRUCT
{
	struct {
		t_Handle handle;				// textureset.handle
		u32 texture;					// textureset.texture
		u16 cell;						// textureset.cell
		u8 cols;							// textureset.cols
		u8 rows;							// textureset.rows
		u32 sizex;						// textureset.sizex
		u32 sizey;						// textureset.sizey
	} textureset;
	struct {
		float x;							// position.x
		float y;							// position.y
	} position;
	struct {
		float x;							// size.x
		float y;							// size.y
	} size;
	struct {
		float x;							// scale.x
		float y;							// scale.y
	} scale;
	struct {
		float x;							// origin.x
		float y;							// origin.y
	} origin;
	struct {
		float x;							// offset.x
		float y;							// offset.y
	} offset;
	struct {
		float z;							// rotation.z
	} rotation;
	color32 color;
	color32 familycolor;
	color32 vertexcolor[4];
	struct {
		float x;							// vertexposition[n].x
		float y;							// vertexposition[n].y
	} vertexposition[4];
	struct {
		float x;							// texcoords[n].x
		float y;							// texcoords[n].y
	} texcoords[4];
	struct {
		float x;							// clipcoords[n].x
		float y;							// clipcoords[n].y
	} clipcoords[4];
	struct {
		int priority;					// sprite.priority
		u8 visible;						// sprite.visible
		u8 blendmode;					// sprite.blendmode
		u8 reserved0;					// sprite.reserved0
		u8 reserved1;					// sprite.reserved1
	} sprite;
	struct {
		u8 enabled;						// shadow.enabled
		u8 intensity;					// shadow.intensity
		s8 x;								// shadow.x-- offset in pixels from item being shadowed
		s8 y;								// shadow.y-- offset in pixels from item being shadowed
	} shadow;
};

class Sprite : public HudObject
{
	DECLARE_CLASSOBJECT( Sprite );
	DECLARE_TREENODE( Sprite );

	enum
	{
		STATE_DRAWPENDING		= (1<<0),
		STATE_DEADCHILDREN	= (1<<1),
	};

	public:
		Sprite( void );
		virtual ~Sprite();

		//////////////////////////////////////////////////////////////////////////
		// Simulated screen resolution

		static void SimulateScreenResolution( float Width, float Height );
		static float SimulatedScreenWidth( void );
		static float SimulatedScreenHeight( void );

		//////////////////////////////////////////////////////////////////////////
		// Viewport

		static void SetViewPortOrigin( float x, float y );
		static float ViewPortX( void );
		static float ViewPortY( void );
		static void ViewPortOrigin( float &x, float &y );

		//////////////////////////////////////////////////////////////////////////
		// Render the draw list

		static void DrawAll( void );
		static void DrawCancel( void );

		//////////////////////////////////////////////////////////////////////////
		// Returns the number of clock cycles spent rendering sprites

		static int GetSpritePerformanceStatistics( void );

		//////////////////////////////////////////////////////////////////////////
		// Creation management functions

		virtual void Create( t_Handle textureset, int texture, int cell = 0, int cols = 1, int rows = 1 );
		virtual void Copy( Sprite *sprite );		// Duplicate a sprite
		virtual void Reset( void );					// Reset the sprite to its default state
		virtual void Release( void );					// Release the memory and/or sprite handles
		virtual void ReleaseAll( void );				// Releases itself and all attached sprites
		virtual void ReleaseHandles( void );		// Kills each sprite and returns their handles to the sprite manager.

		//////////////////////////////////////////////////////////////////////////
		// If you override this function, you MUST override AreHandlesReleased()
		// and provide appropriate calls to InformParentOurHandlesArentRelease().

		virtual void ReleaseHandlesRecursively( void );

		//////////////////////////////////////////////////////////////////////////
		// Interface management functions

		virtual SpriteInterface* AddInterface( const char* pInterfaceName, void* Param );
		virtual SpriteInterface* GetInterface( const char* pInterfaceName );
		virtual void ProcessInterfaces( void );
		virtual void DeleteInterfaces( void );

		//////////////////////////////////////////////////////////////////////////
		// Rendereing functions

		virtual void Update( void );					// Update the sprite and its children and enter them in the draw list
		virtual void Draw( void );						// Render the sprite immediately

		//////////////////////////////////////////////////////////////////////////
		// Texture management functions

		virtual void ClearTextureSet( bool force = false );
		virtual void SetTextureSet( t_Handle textureset, int texture, int cell = 0, int cols = 1, int rows = 1 );
		virtual void SetTexture( int texture, int cell = 0, int cols = 1, int rows = 1 );
		virtual void SelectCell( int cell );
		virtual t_Handle GetTextureSet( void );
		virtual int GetTexture( void );

		//////////////////////////////////////////////////////////////////////////
		// Appearance

		virtual void UseTextureSize( void );
		virtual void GetTextureSize( float &width, float &height );
		virtual void SetSize( float width, float height );		// Set the width and height of the sprite, specified in pixels
		virtual void SetWidth( float width );
		virtual void SetHeight( float height );
		virtual void Grow( float width, float height );			// Adjust the current size
		virtual void SetPosition( float x, float y );			// Set the current position, specified in pixels
		virtual void GetPosition( float &x, float &y );			// Get the current position, specified in pixels
		virtual void SetPositionX( float x );						// Set the current position, x-coordinate only
		virtual void SetPositionY( float y );						// Set the current position, y-coordinate only
		virtual float GetPositionX( void );							// Returns the current x-coordinate position
		virtual float GetPositionY( void );							// Returns the curretn y-coordinate position
		virtual void SetScreenPosition( float x, float y );	// Positions the sprite at a specific screen coordinate regardless of the parent's position
		virtual void GetScreenPosition( float &x, float &y );	// Get the position of the sprite as a screen coordinate.
		virtual void ScreenPosition( float x, float y );
		virtual void Move( float x, float y );						// Adjust the current position
		virtual void SetScale( float x, float y );				// Set the vertical and horizontal scaling factors
		virtual void Scale( float x, float y );					// Adjust the vertical and horizontal scaling factors
		virtual void SetRotation( float degrees );				// Set the rotation around the sprite's origin
		virtual void Rotate( float degrees );						// Adjust the rotation around the sprite's origin
		virtual void SetOrigin( float x, float y );				// Specified in normalized coordinates:
																				//		x = 0 = left, x = 1 = right,
																				//		y = 1 = top, y = 1 = bottom,
																				//		x = y = 0.5 = center
		virtual void SetOffset( float x, float y );				// Specified in pixels
		virtual void GetOffset( float &x, float &y );
		virtual void CopyColor( Sprite *pSource );
		virtual void SetColor( float red, float green, float blue, float alpha );
		virtual void SetColor( int red, int green, int blue, int alpha );
		virtual void SetColor( int color32 );
		virtual void SetAlpha( float alpha );
		virtual void SetFamilyColor( float red, float green, float blue, float alpha );
		virtual void SetFamilyColor( int red, int green, int blue, int alpha );
		virtual void SetFamilyColor( u32 color32 );
		virtual void SetVertexColor( int vertex, float red, float green, float blue, float alpha );
		virtual void SetVertexColor( int vertex, int red, int green, int blue, int alpha );
		virtual void SetVertexColor( int vertex, u32 color32 );
		virtual void SetVertexPosition( int vertex, float x, float y );
		virtual void SetTextureCoordinates( int vertex, float u, float v );
		virtual void SetTextureRect( float u1, float v1, float u2, float v2 );
		virtual void SetBaseColor( float r, float g, float b, float a );
		virtual void SetBaseColor( int r, int g, int b, int a );
		virtual void SetBaseColor( u32 color );
		virtual void SetBlendMode( u8 bm );
		virtual u8 GetBlendMode( void );

		//////////////////////////////////////////////////////////////////////////
		// Visibility

		virtual void SetPriority( int priority );					// Higher priority sprites overlap sprites with lower priority.
		virtual void Toggle( void );
		virtual void Show( bool show );
		virtual void Show( void ) {Show(true);}
		virtual void Hide( void ) {Show(false);}
		virtual void ShowAll( void ) {Show();}
		virtual void HideAll( void ) {Hide();}

		//////////////////////////////////////////////////////////////////////////
		// Animation

		virtual void InitAnim( char priority = 0 );				// Create the resources necessary to do animation.
		virtual void KillAnim( void );								// Destroy the resources created by InitAnim().

		//////////////////////////////////////////////////////////////////////////
		// Drop shadow

		virtual void ShadowOn( void );								// Enable the drop shadow.
		virtual void ShadowOff( void );								// Remove the drop shadow.
		virtual void ShadowPosition( s8 x, s8 y );				// Change the offset of the drop shadow.
		virtual void ShadowIntensity( u8 intensity);				// Change the intensity of the drop shadow.
		virtual void ShadowSettings( bool Enabled, u8 Intensity = 255, s8 x = 2, s8 y = 2 ); // All at once

		//////////////////////////////////////////////////////////////////////////
		// Accessors

		virtual t_Handle GetHandle( void );
		virtual float GetWidth( void );
		virtual float GetHeight( void );
		virtual float GetRotation( void );
		virtual void GetSize( float &width, float &height );
		virtual void GetOrigin( float &x, float &y );
		virtual void GetScale( float &x, float &y );
		virtual void ComputeTransform( Graphics4x4 &transform );
		virtual void ComputeVertices( const Graphics4x4 &transform, Vector3 *vertices );
		virtual Graphics4x4 GetLocalTransform( void );
		virtual Graphics4x4 GetParentTransform( void );
		virtual Graphics4x4 GetFamilyTransform( void );
		virtual Graphics4x4 GetFamilyTransformInverse( void );
		virtual float GetParentRotation( void );
		virtual float GetFamilyRotation( void );
		virtual void GetSpriteVertices( Vector3 *vertices );
		virtual void GetSpriteRect( float &x1, float &y1, float &x2, float &y2 );
		virtual void GetFamilyRect( float &x1, float &y1, float &x2, float &y2 );
		virtual int GetSortedPriority( void );
		virtual int GetSortPriority( void );
		virtual int GetFamilyPriority( void );

		virtual bool IsVisible( void );
		virtual bool IsHidden( void );
		virtual float GetAlpha( void );
		virtual float GetScaleX( void ) { return(m_state.scale.x); }
		virtual float GetScaleY( void ) { return(m_state.scale.y); }
		virtual void GetBaseColor( int &r, int &g, int &b, int &a );
		virtual void GetBaseColor( float &r, float &g, float &b, float &a );
		virtual void GetFamilyColor( int &r, int &g, int &b, int &a );
		virtual void GetFamilyColor( float &r, float &g, float &b, float &a );
		virtual bool DrawPending( void );
		virtual void SetDrawPending( bool TrueOrFalse );

		//////////////////////////////////////////////////////////////////////////

		virtual void ScreenPositionToRelativePosition( float &x, float &y );
		virtual void RelativePositionToScreenPosition( float &x, float &y );
		virtual bool PointInSprite( float x, float y );

		//////////////////////////////////////////////////////////////////////////
		// Cropping

		virtual void Crop( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4 );
		virtual void CropRect( float x1, float y1, float x2, float y2 );

		//////////////////////////////////////////////////////////////////////////
		// Save and load functions

		virtual void GetStateStruct( SPRITESTATESTRUCT &state );
      virtual int GetSpriteFormat( SPRITESTATESTRUCT &state );
		virtual bool SaveObject( MemoryBank &m, char *texture );
		virtual bool LoadObject( MemoryBank &m, char *texture );

	protected:
		virtual void AddToDrawList( void );
		virtual void RemoveFromDrawList( void );
		virtual void ComputeTexture( void );
		virtual void ComputeSprite( void );
		virtual void ComputeShadow( void );
		virtual u32 ComputeFlags( void );
		virtual void ReleaseSprites( void );
		virtual bool CreateShadows( void );
		virtual void ReleaseShadows( void );

		//////////////////////////////////////////////////////////////////////////
		// These functions are available to sub-classes that wish to perform
		// additional processing when these events occur.

		virtual void OnCreate( void );			// when the sprite is created
		virtual void OnRelease( void );			// when the sprite is destroyed
		virtual void OnHide( void );				// when the sprite turns invisible (todo: not yet enabled)
		virtual void OnShow( void );				// when the sprite becomes visible (todo: not yet enabled)
		virtual void OnUpdate( void );			// just before the sprite is added to the draw list
		virtual void OnDraw( void );				// just before the sprite is drawn

		//////////////////////////////////////////////////////////////////////////
		// TJC - You MUST override this and provide appropriate calls to
		// InformParentsOurHandlesArentReleased() if you override
		// ReleaseHandles()!!
		virtual bool AreHandlesReleased(void);
		// TJC - Call this any time you allocate something that
		// ReleaseHandles() frees.
		void InformParentsOurHandlesArentReleased(void);
		bool ChildHandlesAreReleased( void );
		void SetChildHandlesAreReleased( bool TrueOrFalse );

	public:
		static float m_screenx, m_invscreenx;
		static float m_screeny, m_invscreeny;
		static float m_viewportx;
		static float m_viewporty;

		SPRITESTATESTRUCT m_state;
		SpriteAnim *m_anim;

	protected:
		struct _m_status {
			struct {
				t_Handle sprite;
				t_Handle shadow;
			} handles;
			struct {
				TextureDatabase *texturedatabase;
				t_Handle handle;				// m_status.texture.handle;
				int texture;					// m_status.texture.texture;
			} texture;
		} m_status;

	private:
		static Sprite *m_pDrawList;
		static Sprite *m_pDrawTail;
		SpriteInterface *m_pInterfaces;
		Sprite *m_pDrawNext;
		Sprite *m_pDrawPrev;
		u8 m_StateFlags;
};

#define SetGroupColor SetFamilyColor

#define FLOATTOINT(f) (Math::FloatAsInt(f))
#define FLOATTOINTABS(f) (FLOATTOINT(f)&0x7fffffff)

#endif // __SPRITE_H__
