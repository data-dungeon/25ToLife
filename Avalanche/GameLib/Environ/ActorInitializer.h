//============================================================================
//=
//= ActorInitializer.h - handles creating actors specified in world database
//=
//============================================================================

#ifndef _ACTORINITIALIZER_H
#define _ACTORINITIALIZER_H

#include "Layers/LayersStruct.h"

typedef bool (*GameInitGeometry)(CActor *);

class Image;


//============================================================================

class ActorInitializer
{
public:
	static bool InitGeometry( CActor* actor, GameInitGeometry gameInit = NULL);
	static bool InitFromInstance( CActor* actor );
	static bool LoadAIScript( CActor* actor, const char* scriptFilename );
	static bool InitVisualPropertiesFromInstance( CActor* actor );

	// texture funcs
	static bool TextureBlit ( Image* IP, int	BlitType, Image* Src, char* BlendTexture, int x, int y, int blit_w, int blit_h, ts_Hsv HSV);

private:
	static bool InitInstanceGeometry( CActor* actor );
	static bool InitReplaceGeometry( CActor* actor );
};

//============================================================================

#endif //_ACTORINITIALIZER_H
