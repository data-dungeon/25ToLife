/*************************************************************************/
// Here is how an individual game controls how all of the game
// engine is initialized
/*************************************************************************/
#ifndef GAMEINIT_H
#define GAMEINIT_H

//-------------------------------
// YOU MUST FILL ONE OF THESE OUT
// (OR CALL Defaults on it)
//-------------------------------
struct GAMEINIT
{
	// Display stuff (some drivers ignore this)
	bool		hires;
	int		frameBPP;
	int		zBPP;
	bool		cheapFSAA;
	bool		palMode;
	bool		fullScreen;
	int		displayBPP;
	bool		displayHalfHeight;
	
	// Xbox specific stuff
	#ifdef _XBOX
	int		width;
	int		height;
	int		format;
	int		multisampletype;
	#endif
	
	// layers stuff
	int		maxOpenFiles;

	// game engine stuff
	int		maxParticles;
	int		maxParticleEmitters;
	int		maxSprites;
	int		maxDistortions;
	int		maxAttachers;
	int		maxSwooshes;
	int		maxSwooshEmitters;
	int      maxScenes;
	int		maxSceneNodes;
	int		maxTextureSets;
	int		maxDataOwners;
	int		maxDataRecords;
	int		maxCustomChunkLoaders;
	int		maxPoolMatrices;
	int		maxPoolFrustums;
	int		maxPoolInstances;

	// console stuff
	char		consoleScriptPath[64];
	char		consoleScriptLibrary[64];
};

#endif
