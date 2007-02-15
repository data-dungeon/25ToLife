#ifndef __SPRITEFILE_H__
#define __SPRITEFILE_H__

#include "hud/sprite.h"

typedef struct
{
	u32 Type;
	u32 Version;
	u32 Size;
	u32 Reserved0;
	u32 Reserved1;
}
SPRITEFILEHEADER;

typedef struct _SPRITEFILESTRUCT
{
	SpriteState State;
	u32 NumChildren;
}
SPRITEFILESTRUCT;

#endif __SPRITEFILE_H__