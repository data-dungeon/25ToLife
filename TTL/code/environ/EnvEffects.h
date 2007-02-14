/**********************************************/
/*                                            */
/* EnvEffects.h                               */
/* big juju  4/16/03                          */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* environmental effects                      */
/*                                            */
/**********************************************/

/* coordinate actors and particles to create cmobination effects, typically using simple geometry */

#ifndef __ENVEFFECTS_H
#define __ENVEFFECTS_H

/******************* class *******************************/

class CEnvEffects
{
public:

	/* Constructor/destructor */

	CEnvEffects();
	~CEnvEffects();

public:

	/* splash types */

	enum SPLASH_TYPE
	{
		SPLASH_EXPLODE = 0,	// grenade explodes underwater
		SPLASH_TAK,				// tak jumps into water
		SPLASH_SMALL,			// small object (like melon) falls in water

		/* must be last */

		SPLASH_COUNT
	};

	/* simple routines */

	/* routines in .cpp file */

	bool CreateSplash(const SPLASH_TYPE Type, Vector3CRef vPos) const;

protected:

};

#endif // __ENVEFFECTS_H
