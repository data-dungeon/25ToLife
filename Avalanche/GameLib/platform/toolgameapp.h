/*************************************************************************/
// Imposter game app so a fucking tool can compile
/*************************************************************************/
#ifndef TOOLGAMEAPP_H
#define TOOLGAMEAPP_H

// The Game app class
class GameApp
{
public:
	// We are a global variable, no parameters alowed
	GameApp(void) {}
	~GameApp(void) {}

	// default "checkLocal" behavior of the Hog system
	bool HogCheckLocal(void);
};

#endif
