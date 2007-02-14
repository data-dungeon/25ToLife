//-----------------------------------------------------------
//
// fmvplay.cpp
//
// Plays an FMV.. wow, did I really need to explain that!
//
//-----------------------------------------------------------

#include "TTLPCH.h"
#include "fmv/fmvplay.h"
#include "fmv/fmv.h"

static	FMVPlayer	*CFmvPlayer;

//-----------------------------------------------------------
void	CFmvPlay::Init()
{
	// init the movie player instance
	CFmvPlayer = new FMVPlayer;
}

//-----------------------------------------------------------
void	CFmvPlay::DeInit()
{
	CFmvPlayer->End();

	if (CFmvPlayer)
	{
		delete CFmvPlayer;
		CFmvPlayer = NULL;
	}
}

//-----------------------------------------------------------
void	CFmvPlay::Play(char *Filename, bool AllowInterruptions)
{
	bool bAborted = false;

	Init();
	CFmvPlayer->Queue( Filename, AllowInterruptions );
	CFmvPlayer->Begin(NULL);

	// play the movie
	while( !bAborted )
	{
		if( !CFmvPlayer->Play(bAborted) )
			break;
	}
	DeInit();
}
