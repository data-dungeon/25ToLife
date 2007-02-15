/***************************************************************************/
// A simple game construct, this will evolve
/***************************************************************************/
#ifndef GAME_H
#define GAME_H

struct GAMEINIT;

class Game
{
public:
	// Make destructor virtual
	virtual ~Game(void) {}

	// setup the defaults for init'ing all the systems
	// Comes with the current defaults in it
	virtual void SetupGameInit(GAMEINIT &init) = 0;

	// Initialize
	virtual bool Initialize(void) = 0;

	// Run for a frame (returns false when the game should return to system)
	virtual bool Run(void) = 0;

	// Terminate the game
	virtual bool Terminate(void) = 0;

	// Trigger an exit to the system
	virtual void Exit(void) = 0;
};

#endif
