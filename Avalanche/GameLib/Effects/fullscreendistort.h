
// HI!  NOTE:
//
// The arrangement of this stuff in the directories isnt very good.  I know this.
// Specifically, the full screen distort is a part of gamelib/effects because:
//  1) its an effect
//  2) it uses SigGen thats in the same directory.
// yet its called by engine/...  which means that its breaking the layering of
// these libraries.  the only really good solution would be to move SigGen into
// engine/ but thats quite a bit of work, and breaking people right now is a bad
// idea... Milestones and all that.  Because SourceSafe wouldnt remove the obsolete
// versions in gamelib/effects, the results would be kind of messy.  So for now,
// engine will just have to include this stuff...
// Another alternative you might decide to suggest is to have simloop call the
// distort code instead of the engine, which is actually what I started out doing,
// but that doesnt really work because the engine has control over the entire render
// sequence, and the distortions need to be rendered after the scene, but before
// sprites... which given the current render system, is not possible.  really, the
// main render calls should be in an ordered list that the game can manipulate.
// that way, if you want the full screen distort to affect the sprites, you can just
// move it later in the list.  And it means that the game can add/remove things
// in the list at particular points (add-after and add-before style functions would
// be a good way to handle this).
//

#include "Effects/siggen.h"

#ifndef FULLSCREENDISTORT
#define FULLSCREENDISTORT


namespace FullScreenDistort
{
	//
	// here are the routines you call to set up visual effects for a world
	//


	// you can distort in X and Y independantly
	enum DIMENSION
	{
		X, Y
	};

	// this code can perform in a bunch of different modes...
	// they only have any impact on the scene if an effect exists
	// with feedback==false...
	enum MODE
	{
		NORMAL,		// neither buffer is negated (cost = 2O)
		NEGATIVE,	// both buffers are negated	(cost = 3O)
		EMBOSS,		// the backbuffer is negated after its copied (cost = 3O)
		GHOSTLY,		// the copy of the backbuffer is negated (cost = 3O)
		GREYSCALE,	// the copy of the backbuffer is converted to black and white (cost=40)
	};

	// you must call init if your going to do effects.
	// gridx and gridy are the number of grid cells in each dimension that the
	// screen is divided into.  you should use the lowest numbers you can that
	// still look decent...
	// feedbackOpacity is how opaque the feedback effect is... hard to discribe
	// in artist terms, so, heres programmer terms... it the alpha value used to
	// copy the front buffer over the back buffer... a 1.0 would result in the new
	// image being entirely overwritten with the old image, while a 0.0f would result
	// no affect on the backbuffer.
	void Init(int _gridX, int _gridY, float _feedbackOpacity = 1.0f, MODE mode = NORMAL);
	bool Inited();
	bool IsActive();

	// You must have a matching call to Shutdown for any call to Init
	void Shutdown(void);

	// Init parameters that can be changed on the fly...
	void SetMode(MODE _mode);
	void SetOpacity(float opacity);

	// Add full screen distortions using this function.
	// returns a hendle to allow future deletion of the effect...
	u32 Add(DIMENSION dim, float centerx, float centery, float innerradius, float outerradius, bool inversearea, float attacktime, float sustaintime, float decaytime, bool useOmega, bool feedBack, SigGen* generator);

	// Turn on looped sustain
	void SetLoopedSustain(u32 handle, bool loop);
	bool GetLoopedSustain(u32 handle);
	
	// Is this disortion still playing?
	bool IsActive(u32 handle);

	// used to delete an effect before its demise.
	void Remove(u32 handle);

	// Set the filter color that filters the screen color.
	// Filter colors are on an overbrightened scale where 0x00 == 0.0f, 0x80 == 1.0f, 0xFF = 2.0f
	// filter colors only work if there is an effect with feedback==false onscreen...
	void SetFilterColor(u32 color);

	// move the filter color towards 'color' from whatever it is, based on distance from xpos, ypos
	// Filter colors are on an overbrightened scale where 0x00 == 0.0f, 0x80 == 1.0f, 0xFF = 2.0f
	// filter colors only work if there is an effect with feedback==false onscreen...
	void SetFilterColorPoint(float xpos, float ypos, float innerradius, float outerradius, u32 color);

  // JMH - tell the distortion to use the OS Timer (for time effects)
	void UseOSTimer(bool flag);

	//
	// these are called by the engine to manage this stuff...
	//

	void UpdateAll(void);

	void RenderAll(void);
};

#endif	// FULLSCREENDISTORT
